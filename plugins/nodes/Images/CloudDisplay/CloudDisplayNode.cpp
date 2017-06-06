/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; version 2.1 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
-----------------------------------------------------------------------------
*/

//!
//! \file "CloudDisplayNode.h"
//! \brief Implementation file for CloudDisplayNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \date       12.06.2012 (last updated)
//!

#include "CloudDisplayNode.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <iostream>
#include <QFontDatabase>


namespace CloudDisplayNode {
using namespace Frapper;

#define MAXWORDSSELECTED 3
//#define WIDTHSCENE 768
//#define HEIGHTSCENE 1280

///
/// Constructors and Destructors
///

//!

//! Constructor of the CloudDisplayNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
CloudDisplayNode::CloudDisplayNode ( QString name, ParameterGroup *parameterRoot ) :
    ImageNode(name, parameterRoot),
	parameterRoot(parameterRoot),
	m_interval(10)
{

	m_wordsGroup = new ParameterGroup("wordsTable");
	m_poemsGroup = new ParameterGroup("poemsGroup");
	parameterRoot->addParameter(m_poemsGroup);	
	
	// Contains the set of words and their frequency, to be displayed in the Word Cloud
	groupParameter = Parameter::createGroupParameter("wordsGroup", m_wordsGroup);
	parameterRoot->addParameter(groupParameter);
    groupParameter->setPinType(Parameter::PT_Input);
    groupParameter->setSelfEvaluating(true);
	groupParameter->setVisible(false);
	
	// Connect the XML-File Dialog
	Parameter *filenameParameter = getParameter("XML File");
	if (filenameParameter){
        filenameParameter->setChangeFunction(SLOT(loadXmlFile()));
		filenameParameter->setCommandFunction(SLOT(reloadXmlFile()));
	}
    else
        Log::error("Could not obtain all parameters required for parsing the Poems file.", "CloudDisplayNode::CloudDisplayNode");

	// To fill the list with fonts
	m_fontsList = new Frapper::EnumerationParameter("Font Name", 0);
	parameterRoot->addParameter(m_fontsList);
	QFontDatabase *font = new QFontDatabase();
	QStringList str; 
	QStringList list = font->families(QFontDatabase::Any);

	for(int i=0; i<list.size(); i++)
		str.append(QString::number(i));
	
	m_fontsList->setLiterals( QStringList() << list );
	m_fontsList->setValues(QStringList() << str);
	

	int width = parameterRoot->getParameter("Image Width")->getValue().toInt();
	int height = parameterRoot->getParameter("Image Height")->getValue().toInt();
	m_fontSize = parameterRoot->getParameter("Font Size")->getValue().toDouble();	

	outputImageParameter = getParameter(m_outputImageName);		
	uniqueRenderTextureName = createUniqueName("wordCloudTexture");	// create unique name for the reder texture	
	m_texture = Ogre::TextureManager::getSingleton().createManual(uniqueRenderTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, width, height, 0, Ogre::PF_A8B8G8R8, Ogre::TU_RENDERTARGET, 0, false, 0);	
	setValue(m_outputImageName, m_texture, true);

	m_tagCloudView = new TagCloudView(width, height);


	m_wordsTable = new QTableWidget(400, 3);
	rowsInTable = 0;
	poemLine = 1;
	m_numberWordsSelected = 0;

    // create a new QTimer
	m_pTimer = new QTimer(this);
	connect (m_pTimer, SIGNAL(timeout()), this, SLOT(animateCloud()));

	// Function to be executed when the parameter changes its value
	setProcessingFunction("wordsGroup", SLOT(displayCloud()));
	setChangeFunction("Image Width", SLOT(displayCloud()));
	setChangeFunction("Image Height", SLOT(displayCloud()));
	setChangeFunction("Font Name", SLOT(displayCloud()));
	setChangeFunction("Font Size", SLOT(displayCloud()));
	setChangeFunction("Color", SLOT(displayCloud()));
	setChangeFunction("ImageX", SLOT(relocateCloud()));
	setChangeFunction("ImageY", SLOT(relocateCloud()));
	
	setProcessingFunction("Start Animation", SLOT(activateTimer()));
}


//!
//! Destructor of the CloudDisplayNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CloudDisplayNode::~CloudDisplayNode ()
{
	m_wordsGroup->destroyAllParameters();
	m_poemsGroup->destroyAllParameters();
}

void CloudDisplayNode::activateTimer()
{
	const bool activeParameter = getBoolValue("Start Animation");

	if( activeParameter ) {
		m_pTimer->setInterval(m_interval);
		m_pTimer->start(m_interval);
	} else {
		m_pTimer->stop();		
	}
}

//!
//! displays the cloud in the scene with the words from the input GroupParameter
//!
void CloudDisplayNode::displayCloud()
{
	createWordsTable("");

	if (rowsInTable > 0 )
	{
		
		int width = parameterRoot->getParameter("Image Width")->getValue().toInt();
		int height = parameterRoot->getParameter("Image Height")->getValue().toInt();
		m_fontSize = parameterRoot->getParameter("Font Size")->getValue().toFloat();

		m_font = m_fontsList->getCurrentLiteral();

		// Texture & Word cloud creation 	
		m_texture = getImage();
		m_texture->setHeight(height);
		m_texture->setWidth(width);
				
		QColor color = getColorValue("Color");

		int posX = parameterRoot->getParameter("ImageX")->getValue().toInt();
		int posY = parameterRoot->getParameter("ImageY")->getValue().toInt();

		m_tagCloudView->cloudVisualization(m_wordsTable, rowsInTable, m_fontSize, m_font, width, height, posX, posY, color, this);			
		
		setValue(m_outputImageName, m_texture, true);		
	}	
}

//!
//! relocates the cloud in the scene according to the parameters "ImageX" and "ImageY"
//!
void CloudDisplayNode::relocateCloud()
{
	int height = parameterRoot->getParameter("ImageY")->getValue().toInt();
	int width = parameterRoot->getParameter("ImageX")->getValue().toInt();

	m_texture = getTextureValue(m_outputImageName);
	m_tagCloudView->translateInScene(width, height, this);
	setValue(m_outputImageName, m_texture, true);
}

//!
//! changes the positions of each word in the cloud to create an animated effect
//!
void CloudDisplayNode::animateCloud()
{
	float offset = -0.5;
	/*
	int count = 0;

	m_animation = getValue("Start Animation").toBool();*/

	m_texture = getTextureValue(m_outputImageName);

	/*while (m_animation  &&  count < 350)
	{*/
		//count++;
		m_tagCloudView->translateItemsInScene(rowsInTable, m_wordsTable, offset, this);
		setValue(m_outputImageName, m_texture, true);
	//}
}

//! 
//! Creates the widget that holds the words to be represented in the cloud
//!
void CloudDisplayNode::createWordsTable(QString wordselected)
{
	int i = 0;
	rowsInTable=0;
		
	const AbstractParameter::List& parameterList = getGroupValue("wordsGroup")->getParameterList();
	QString grammar;

	if (!parameterList.isEmpty())
	{	
		m_tagCloudView->scene()->clear();
		m_wordsTable->clear();

		foreach(AbstractParameter *rowList, parameterList) {
			
			ParameterGroup* group = (dynamic_cast<ParameterGroup *>(rowList));
			QString groupName = group->getName();
			QString wordString = groupName.remove("Parameter");
			QString freqString = group->getParameter("frequency")->getValueString();
			qreal freqNumber = freqString.toInt();		

			if (freqNumber > 1 ) {//anadir lo de las palabras restringidas -> Se puede hacer por GUI
			
				QTableWidgetItem *wordItem = new QTableWidgetItem;
				QTableWidgetItem *frequencyItem = new QTableWidgetItem;			

				wordItem->setText(wordString);
				frequencyItem->setText(freqString);			

				m_wordsTable->insertRow(rowsInTable);
				m_wordsTable->setItem(rowsInTable,0,wordItem);
				m_wordsTable->setItem(rowsInTable,1,frequencyItem);
			
				rowsInTable++;
			}			
		}	
	}
}

///
/// Private Slots
///

//!
//! load node settings from XML File
//!
void CloudDisplayNode::loadXmlFile()
{
	Q_SLOT

	int index = 1;
	

	// get Filename from Input Parameter
	QString filename = getStringValue("XML File");
    if (filename == "") { return; }

	// load file or return on error
	QFile* xmlfile = new QFile(filename);
	if (!xmlfile->open(QIODevice::ReadOnly | QIODevice::Text)) {
		Log::error(QString("File %1 could not be loaded!").arg(filename), "CloudDisplayNode::loadXmlFile");
		return;
	}
	m_poemsGroup->clear();
	m_numberWordsSelected = 0;
	m_poemSelected->setValue(QVariant(""));
	setValue("Alpha", QVariant(100.0), true);
	
	QXmlStreamReader xml(xmlfile);
	
	// read contents
	while(!xml.atEnd() &&
	      !xml.hasError()) {

		// traverse XML-Tree 
		xml.readNext();
		QString str = xml.name().toString();

		if(xml.name() == "poems" && xml.isStartElement()) {

			bool inToken = true;

			while(inToken){

				xml.readNext();

				if(xml.name() == "poem"){

					QXmlStreamAttributes attrs = xml.attributes();
					QString poem = attrs.value("defaultValue").toString();

					poem.insert(0,  attrs.value("title").toString() + ". By " +  attrs.value("author").toString() + "&");

					poem.replace('[', '<');
					poem.replace(']', '>');
					poem.replace('\'', '\"');					

					if(!attrs.value("title").toString().isEmpty()){
						// set the values for the table that contains the data of the cloud
						QString name = "poem" + QString::number(index);
						Parameter *poemParameter = new Parameter( name, Parameter::T_String, QVariant(poem) ); 
						NumberParameter *buttonParameter = new NumberParameter("Select "+QString(name), Parameter::T_Command, name);

						m_poemsGroup->addParameter(poemParameter);
						m_poemsGroup->addParameter(buttonParameter);

						buttonParameter->setCommandFunction(SLOT(updatePoemSelected()));

						poemsIndexStr.append(QString::number(index)+",");
						index++;						
					}
				}
			
				if(xml.name() == "poems" && xml.isEndElement()) {
					inToken = false;
					poemsIndexStr.chop(1);
				}
			}
		}
	}

	// Give Feedback to the log
	if(xml.hasError())
		Log::error(QString("File %1 contains Errors!").arg(filename), "CloudDisplayNode::loadXmlFile");
	else {
		Log::info(QString("File %1 successfully loaded.").arg(filename), "CloudDisplayNode::loadXmlFile");
		createWordsTable("");
	}
}


//!
//! wrapper function for loadXmlFile()
//!
void CloudDisplayNode::reloadXmlFile()
{

	
	// return if no file to reload is selected
    if (getStringValue("XML File") == "") { return; }
	
	// get output parameter group
	//ParameterGroup *m_OutputParameters = parameterRoot->getParameterGroup("poemsGroup");

	/*
	// clear output parameters
	//m_OutputParameters->destroyAllParameters();
	parameterRoot->getParameterGroup("poemsGroup")->destroyAllParameters();
	
	//parameterRoot->getParameterGroup("wordsTable")->destroyAllParameters();
	m_referenceData = getGroupValue("wordsGroup", false);
	m_referenceData->destroyAllParameters();
	*/
	
	// continue with loadXmlFile()
	loadXmlFile();
	
}


} // namespace CloudDisplayNode 
