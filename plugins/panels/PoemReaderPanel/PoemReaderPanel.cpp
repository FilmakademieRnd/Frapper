/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "PoemReaderPanel.cpp"
//! \brief Implementation file for PoemReaderPanel class.
//!
//! \author     Diana Arellano
//! \version    1.0
//! \date       06.02.2012 (last updated)
//!

#include "PoemReaderPanel.h"
#include "ParameterTabPage.h"
#include "DoubleSlider.h"
#include "NodeFactory.h"
#include "Log.h"

namespace Frapper {

///
/// Constructors and Destructors
///

//!
//! Constructor of the PoemReaderPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
PoemReaderPanel::PoemReaderPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_PluginPanel, parent, flags)
{

	done = false;

	m_leftvboxLayout = new QVBoxLayout();
    m_leftvboxLayout->setSpacing(3);
	m_leftvboxLayout->setGeometry(QRect(1,1, 50, 100));
    m_leftvboxLayout->setContentsMargins(1, 1, 1, 1);
    m_leftvboxLayout->setObjectName(QString::fromUtf8("m_leftvboxLayout"));

	m_rightvboxLayout = new QVBoxLayout();
    m_rightvboxLayout->setSpacing(3);
    m_rightvboxLayout->setContentsMargins(1, 1, 1, 1);
    m_rightvboxLayout->setObjectName(QString::fromUtf8("m_rightvboxLayout"));
	
	
	// Creation of labels and List Widgets
	// Left layout
	m_lineEdit = new QLineEdit("");
	m_lineEdit->setGeometry(10, 10, 150, 20);
	m_buttonSelector = new QPushButton("Reduce the cloud");
	//buttonSelector->setGeometry(600, 40, 50, 20);
	
	//buttonCloudGenerator->setGeometry(600, 90, 100, 40);

	QLabel *labelNodes = new QLabel("Selected Words:");
	//labelNodes->setGeometry(20, 10,100,20);
	m_ListNodes = new QListWidget(this);
	m_ListNodes->setGeometry(10, 30, 100, 200);


	m_leftvboxLayout->addWidget(m_lineEdit);
	//m_leftvboxLayout->addWidget(m_buttonSelector);
	m_leftvboxLayout->addWidget(labelNodes);
	m_leftvboxLayout->addWidget(m_ListNodes);

	//Right layout
	m_wordsTable = new QTableWidget(4000, 4, this);
	m_wordsTable->setGeometry(600, 150, 300, 200);
	
	//m_tagCloudView = new TagCloudView();

	//buttonCloudGenerator = new QPushButton("Visualize the Cloud", this);
	//m_rightvboxLayout->addWidget(m_wordsTable);
	//m_rightvboxLayout->addWidget(m_tagCloudView);
	//m_rightvboxLayout->addWidget(buttonCloudGenerator);


	// HBOXLAYOUT
	m_hboxLayout = new QHBoxLayout();
    m_hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
	m_hboxLayout->addLayout(m_leftvboxLayout);
	m_hboxLayout->addLayout(m_rightvboxLayout);



	this->setLayout(m_hboxLayout);

	// connection of a signal to a slot
	QObject::connect(m_ListNodes, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(nodeSelected(QListWidgetItem *)));

	m_signalMapper = new QSignalMapper(this);
	connect(m_signalMapper, SIGNAL(mapped(int)),this, SLOT(clicked(int)));

	
	
}


//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PoemReaderPanel::~PoemReaderPanel ()
{
	delete m_ListNodes;
	delete m_wordsTable;
}

///
/// Public Funcitons
///


//!
//! Connects the panel with the scene.
//!
//! \param *nodeModel NodeModel of the scene
//! \param *sceneModel SceneModel of the scene
//!
void PoemReaderPanel::registerControl(NodeModel *nodeModel, SceneModel *sceneModel)
{
	m_nodeModel = nodeModel;
	m_sceneModel = sceneModel;
	
	QObject::connect(m_nodeModel, SIGNAL(nodeSelected(Node *)), this, SLOT(updateSelectedNode(Node *)));
	QObject::connect(m_sceneModel, SIGNAL(modified()), this, SLOT(update()));
	QObject::connect(m_sceneModel, SIGNAL(objectCreated(const QString &)), this, SLOT(update()));
	QObject::connect(m_sceneModel, SIGNAL(selectionChanged(bool)), this, SLOT(update()));
	//QObject::connect(m_buttonSelector, SIGNAL(clicked()), this, SLOT(reduceCloud()));
	
	
	update();	
}


///
/// Private Slots
///


//!
//! Is called when a node is selected
//!
//! \param selectedNode the node which was selected
//!
void PoemReaderPanel::updateSelectedNode(Node * selectedNode)
{
	m_node = selectedNode;
	int buttonId = 1;

	if( selectedNode->getName().contains("poemreader")) 
	{
		const AbstractParameter::List& poemsList = selectedNode->getParameterRoot()->getParameterGroup("poemsGroup")->getParameterList();

		clearLayout(m_rightvboxLayout, true);

		foreach(AbstractParameter *poem, poemsList){
			m_rightvboxLayout->addWidget(new QLineEdit(dynamic_cast<Parameter *>(poem)->getValueString()));
			poems.append(dynamic_cast<Parameter *>(poem)->getValueString());


			QPushButton *button = new QPushButton(poem->getName());
			
			connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));		
			m_signalMapper->setMapping(button, buttonId);
			m_rightvboxLayout->addWidget(button);
			buttonId++;
		}
		done = true;

	}
	
}

void PoemReaderPanel::clearLayout(QLayout* layout, bool deleteWidgets = true)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                delete widget;
        }
        else if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}


void PoemReaderPanel::clicked(int buttonId){

	QString poem = poems.at(buttonId-1);

	QStringList nodeNames = m_nodeModel->getNodeNames();

	for(int i = 0; i < nodeNames.length(); ++i){
		QString nodeName = nodeNames.at(i);

		if (nodeName.contains("poemreader")){

			m_nodeModel->getNode(nodeName)->getParameterRoot()->setValue("Poem Selected", poem, true);	
			i = nodeName.length();
		}
	}
}

//!
//! Updates the panel if the node model changes
//!
void PoemReaderPanel::update()
{
	//m_ListNodes->clear();

	QStringList nodeNames = m_nodeModel->getNodeNames();
	QStringList selectedNodes = m_sceneModel->getSelectedObjects();

	for(int i = 0; i < nodeNames.length(); ++i){
		QString nodeName = nodeNames.at(i);
		QListWidgetItem *newItem = new QListWidgetItem;

		if( selectedNodes.contains(nodeNames.at(i)) && nodeNames.at(i).contains("poemreader") ) {
			newItem->setBackgroundColor(QColor(150,255,150));
			m_node = m_nodeModel->getNode(nodeNames.at(i));

			if(m_lineEdit->text() != ""){
				newItem->setText(m_lineEdit->text());
				m_ListNodes->addItem(newItem);
				m_lineEdit->setText("");
			}



		}

		/*newItem->setText(nodeName);
		m_ListNodes->addItem(newItem);	*/
	}
	
}

void PoemReaderPanel::createWordsTable(Node *selectedNode)
{
	int i = 0;
	int posx = 0;
	int posy = 0;
	int gWordHeight = 0;
	QString nodeName = selectedNode->getName();
	QString typeName = selectedNode->getTypeName();

	if (nodeName.contains("poemreader", Qt::CaseInsensitive)) {

		const AbstractParameter::List& parameterList = selectedNode->getParameterRoot()->getParameterGroup("wordsTable")->getParameterList();

		//m_tagCloudView->scene()->clear();
		m_wordsTable->clear();

		foreach(AbstractParameter *rowList, parameterList) {

			const AbstractParameter::List& wordList = (dynamic_cast<ParameterGroup *>(rowList))->getParameterList();

			QString wordString = dynamic_cast<Parameter *>(wordList.at(0))->getValueString();
			QString freqString = dynamic_cast<Parameter *>(wordList.at(1))->getValueString();
			qreal freqNumber = dynamic_cast<NumberParameter *>(wordList.at(1))->getValue().toReal();
			QString indexString = dynamic_cast<Parameter *>(wordList.at(2))->getValueString();

			if (freqNumber > 1) {
			
				QTableWidgetItem *wordItem = new QTableWidgetItem;
				QTableWidgetItem *frequencyItem = new QTableWidgetItem;
				QTableWidgetItem *indexesItem = new QTableWidgetItem;

				wordItem->setText(wordString);
				frequencyItem->setText(freqString);
				indexesItem->setText(indexString);

				m_wordsTable->insertRow(i);
				m_wordsTable->setItem(i,0,wordItem);
				m_wordsTable->setItem(i,1,frequencyItem);
				m_wordsTable->setItem(i,2,indexesItem);

				//Rendering of the cloud
				/*qreal w = m_tagCloudView->scene()->width();
				if (posx > ceil( m_tagCloudView->scene()->width() * 0.6) ) {
					posx = 0;
					posy = posy + gWordHeight;
					gWordHeight = 0;
				}
				renderWords(m_tagCloudView->scene(), wordString, freqNumber, posx, posy, gWordHeight);*/
				i++;
			}			
		}
	}
}

//!
//! Render the words in the given view
//!
void PoemReaderPanel::renderWords(QGraphicsScene *scene, QString word, qreal size, int &posx, int &posy, int &gWordHeight)
{		
	QGraphicsTextItem * wordItem = new QGraphicsTextItem;
	wordItem->setDefaultTextColor(QColor(0,0,255,255));
	QFont wordItemFont = wordItem->font();

	wordItemFont.setPointSize(ceil(size*12));
	//wordItemFont.setPointSize(3*12);
	wordItem->setFont(wordItemFont);

	wordItem->setPlainText(word);
	wordItem->setPos(posx,posy);

	QRectF rect = wordItem->boundingRect();
	posx = posx + rect.width() + 4;
	if (gWordHeight < rect.height())
		gWordHeight = rect.height();
	
	scene->addItem(wordItem);
}



//!
//! Updates the scene and the panel if a node is selected in the panel
//!
void PoemReaderPanel::nodeSelected(QListWidgetItem * listItem)
{
	QString nodeName = listItem->text();
	Node *selectedNode = m_nodeModel->getNode(nodeName);
	m_sceneModel->selectObject(nodeName);
}


// FUNCTIONS FOR CLOUD GENERATOR -> POEM READER


//!
//! Reduces the amount of words in the wordCloud (Diana)
//!
/*
void PoemReaderPanel::reduceCloud(){
	
	QString word = m_lineEdit->text();
	int index = 0;

	if (findWordInTable(word, index)){

		m_tagCloudView->scene()->clear();
		m_wordsTable->clear();
		
		QString poemIndexes = m_wordsTable->item(index, 2)->text();
		m_poemsindex = poemIndexes.split(",");

		foreach(QString idx, m_poemsindex){
			
		}

	}

}
*/

/*
void PoemReaderPanel::reduceCloud(){

	int poemNumber = 1;

	if (m_nodeModel->getNodeNames().length() > 0){
	QString nodeName = m_node->getName();

	QString file = m_node->getParameterRoot()->getParameter("XML File")->getValueString();
	if  ( nodeName.contains("poemreader", Qt::CaseInsensitive) && (file.compare("") != 0) ) {

		//QString word = m_lineEdit->text();
		QString word = m_node->getParameterRoot()->getParameter("WordIn")->getValueString();
		m_lineEdit->setText(word);

		QString parameterName = word + "Parameter";

		//No verification of existance is done : TODO
		const AbstractParameter::List *parameterList, *wordList;

		QString poemsIndexStr = m_node->getParameterRoot()->getParameterGroup("wordsTable")->getParameterGroup(parameterName)->getParameter("poemIndexes")->getValueString();
		m_poemsindex = poemsIndexStr.split(",");

		//we clean the parameter with all the words of the poems
		m_node->getParameterRoot()->getParameterGroup("wordsTable")->clear();
		
		foreach(QString id, m_poemsindex){
		
			QString name = "poem" + id;
			Parameter *poemParameter = m_node->getParameterRoot()->getParameterGroup("poemsGroup")->getParameter(name);
			QString poem = poemParameter->getValueString();
			QStringList poemList = poem.split(" ");

			foreach (QString str, poemList){
				//we don´t want words of less than 3 characters
				if (str.size() > 3){
					
					parameterName = str + "Parameter";
					ParameterGroup  *rowGroup = m_node->getParameterRoot()->getParameterGroup("wordsTable")->getParameterGroup(parameterName);
					if ( rowGroup == 0 ) {

						rowGroup = new ParameterGroup(parameterName);

						Parameter *wordParameter = new Parameter("word", Parameter::T_String, QVariant(str));
						NumberParameter *frequencyParameter = new NumberParameter("frequency", Parameter::T_Int, QVariant(1));
						Parameter *poemIndexesParameter = new Parameter("poemIndexes", Parameter::T_String, QVariant(QString::number(id.toInt())) );

						rowGroup->addParameter(wordParameter);
						rowGroup->addParameter(frequencyParameter);
						rowGroup->addParameter(poemIndexesParameter);

						m_node->getParameterRoot()->getParameterGroup("wordsTable")->addParameter(rowGroup);
					
					}
					else{	//the word already exists in the table
						int frequency = rowGroup->getParameter("frequency")->getValue().toInt();
						frequency++;
						rowGroup->setValue("frequency", QVariant(frequency));
						
						QString prevPoemIndexes = rowGroup->getParameter("poemIndexes")->getValueString();
			
						if ( !(prevPoemIndexes.contains(QString::number(id.toInt()))) ) { //different poem
							QString s = prevPoemIndexes + "," + QString::number(id.toInt());
							rowGroup->setValue("poemIndexes", QVariant(s));
						}				
					}
				}
			}
			
		}
		createWordsTable(m_node);
		update();
	}
	}

}
*/

void PoemReaderPanel::sendPoem(int buttonId){

	int i=0;


	
		

	
}



//!
//! Reduces the amount of words in the wordCloud (Diana)
//!
void PoemReaderPanel::visualizeCloud(){


	if(m_node->getName().contains("poemreader")){
	
		QString word = m_node->getParameterRoot()->getParameter("WordIn")->getValueString();

		if( word != "" ){
			m_lineEdit->setText(word);
			m_node->getParameterRoot()->getParameter("WordIn")->setValue(QVariant(""));	//clean the input string in the node
			createWordsTable(m_node); // the tablewidget is created again
		}
		//m_tagCloudView->setImage(m_node);	//always visualize the cloud
		update();
	}
}

} // end namespace
