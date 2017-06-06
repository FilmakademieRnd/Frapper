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
//! \file "PoemReaderNode.h"
//! \brief Implementation file for PoemReaderNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \date       12.06.2012 (last updated)
//!

#include "PoemReaderNode.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <iostream>

namespace PoemReaderNode {
using namespace Frapper;

///
/// Constructors and Destructors

//!

//! Constructor of the PoemReaderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PoemReaderNode::PoemReaderNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
	parameterRoot(parameterRoot),
	m_stopPoem(false)
{
	m_poemsGroup = new ParameterGroup("poemsGroup");
	parameterRoot->addParameter(m_poemsGroup);

	m_poemLinesGroup = new ParameterGroup("Poem Lines");
	parameterRoot->addParameter(m_poemLinesGroup);
	
	inputStringParameter = parameterRoot->getParameter("WordIn");
	inputTagParameter = parameterRoot->getParameter("TagIn");
	m_poemSelected = parameterRoot->getParameter("Poem Selected");
	animStateParameter = parameterRoot->getParameter("Animation State");
	m_alphaParameter = parameterRoot->getParameter("Alpha");

	// Parameter with the parameter words that will be displayed in the cloud
	m_wordsGroup = new ParameterGroup("wordsTable");
	groupParameter = Parameter::createGroupParameter("wordsGroup", m_wordsGroup);
	parameterRoot->addParameter(groupParameter);
    groupParameter->setPinType(Parameter::PT_Output);
    groupParameter->setSelfEvaluating(true);
	groupParameter->setVisible(false);

	// Connect the XML-File Dialog
	Parameter *filenameParameter = getParameter("XML File");
	if (filenameParameter){
        filenameParameter->setChangeFunction(SLOT(loadXmlFile()));
		filenameParameter->setCommandFunction(SLOT(reloadXmlFile()));
	}
    else
        Log::error("Could not obtain all parameters required for parsing the Poems file.", "PoemReaderNode::PoemReaderNode");

	poemLine = 1;
	m_numberWordsSelected = 0;
	m_recitingPoem = false;

	m_minFrequency = 1;

	 // set animation timer
    m_foutTimer = new QTimer(); 
	m_finTimer = new QTimer();
    connect(m_foutTimer, SIGNAL(timeout()), SLOT(updateFadingOutTimer()));
	connect(m_finTimer, SIGNAL(timeout()), SLOT(updateFadingInTimer()));
    
	// Function to be executed when the parameter changes its value
	setProcessingFunction("WordIn", SLOT(reduceWordsCloud()));
	setProcessingFunction("Animation State", SLOT(triggerPoemLines()));

	setCommandFunction("Stop Poem", SLOT(stopReciting()));
	
}


//!
//! Destructor of the PoemReaderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PoemReaderNode::~PoemReaderNode ()
{
	m_wordsGroup->destroyAllParameters();
	m_poemsGroup->destroyAllParameters();
}


///
/// Public Methods
///



///
/// Private Methods
///


//!
//! Function: createPoemsData() 
//!
//! Creates the Parameter Group that will contain all the poems words
//!
void PoemReaderNode::createPoemsData() 
{
	
	QStringList poemsIndex = m_poemsIndexStr.split(",");

	m_wordsGroup->clear();
	
	//We get the poem to be analyzed according to the indexes in the list poemsindex
	foreach(QString id, poemsIndex)
	{
		QString name = "poem" + id;
		Parameter *poemParameter = m_poemsGroup->getParameter(name);
		QString poemstring = poemParameter->getValueString();

		QStringList poemdata = poemParameter->getValueString().split('&');
		QString poem = poemdata.at(1);

		createPoemParameterGroup(poem, id.toInt());			
	}
}

//!
//!	Function: createPoemParameterGroup(QString poem, int poemId)
//!
//!	Creates the ParameterGroup that stores each poem
//!
void PoemReaderNode::createPoemParameterGroup(QString poem, int poemId)
{
	//Remove punctuation marks from the strings
	QRegExp rx1("[\"!#$%&'()+,./:;?@_`{|}~-]");
	poem.remove(rx1);

	QStringList poemList = poem.split(" ");

	foreach (QString str, poemList)
	{
		//we don´t want words of less than 3 characters
		if ( str.size() > 3 && !str.startsWith("<") && !str.endsWith(">") && !str.contains("=") && !str.startsWith(">") && wordAccepted(str) && !hasBeenSelected(str))
		{	
			QString parameterName = str + "Parameter";
			ParameterGroup  *rowGroup = m_wordsGroup->getParameterGroup(parameterName);
		
			if ( rowGroup == 0 ) 
			{
				rowGroup = new ParameterGroup(parameterName);
				NumberParameter *frequencyParameter = new NumberParameter("frequency", Parameter::T_Int, QVariant(1));
				Parameter *poemIndexesParameter = new Parameter("poemIndexes", Parameter::T_String, QVariant(poemId));

				rowGroup->addParameter(frequencyParameter);
				rowGroup->addParameter(poemIndexesParameter);

				m_wordsGroup->addParameter(rowGroup);
			}
			else
			{	//the word already exists in the list of words
				int frequency = rowGroup->getParameter("frequency")->getValue().toInt();
				frequency++;
				rowGroup->setValue("frequency", QVariant(frequency));
								
				QString prevPoemIndexes = rowGroup->getParameter("poemIndexes")->getValueString();
					
				if ( !(prevPoemIndexes.contains(QString::number(poemId))) ) 
				{ //different poem
					QString s = prevPoemIndexes + "," + QString::number(poemId);
					rowGroup->setValue("poemIndexes", QVariant(s));
				}	
			}
		}
	}
}

//!
//! Function: hasBeenSelected(bool)
//! 
//! Checks if the word of the poem has been already said by the user
//!
bool PoemReaderNode::hasBeenSelected(QString word)
{
	if (!selectedWords.isEmpty())
	{
		foreach(QString str, selectedWords)
		{
			if (word.compare(str) == 0)
				return true;
		}
		return false;
	}
	else return false;
}

//!
//! Function: reduceWordsCloud()
//! 
//! Reduces the amount of words to be recognized and shown in the wordCloud
//!
void PoemReaderNode::reduceWordsCloud()
{
	QString word = inputStringParameter->getValueString();
	QString tag = inputTagParameter->getValueString();
	QStringList poemsindex;
	bool fade = false;

	m_numberWords = getParameter("Nr. Words to recognize")->getValue().toInt();

	//if (word.startsWith("pcw__")) 
	//{		
	//	//the prefix "pcw" indicates that the word has been recognized 
	//	word.remove("pcw__");
	//	word = removeWildCards(word);		

	if( (word.compare("who are you")==0 || word.compare("hello muses")==0 || word.compare("hello")==0 || word.compare("what is this")==0) && m_numberWordsSelected == 0)
	{
		QString reply = replyFromCharacter("greet user", false);
		setValue("Poem Selected", reply, true);
	}
	else
	{
		selectedWords.append(word);	//list of words that have been said by the user

		QString parameterName = word + "Parameter";
		ParameterGroup *parameterGroup = m_wordsGroup->getParameterGroup(parameterName);

		if (parameterGroup != 0)
		{
			m_poemsIndexStr = parameterGroup->getParameter("poemIndexes")->getValueString();
			poemsindex.clear();
			poemsindex = m_poemsIndexStr.split(",");
			
			//if (m_numberWordsSelected < MAXWORDSSELECTED-1 )
			if (m_numberWordsSelected < m_numberWords-1 )
			{					
				m_wordsGroup->clear(); // //we clean the parameter with all the words of the poems
			
				foreach(QString id, poemsindex)
				{				
					QString name = "poem" + id;
					Parameter *poemParameter = m_poemsGroup->getParameter(name);
					QString poemstring = poemParameter->getValueString();

					QStringList poemdata = poemstring.split('&');
					QString poem = poemdata.at(1);
					
					createPoemParameterGroup(poem, id.toInt());										
				}
				m_numberWordsSelected++;

				createGrammar(word);		

				QString reply = replyFromCharacter(word, false);	
				setValue("Poem Selected", reply, true);
			}

			else
			{				
				QString reply = replyFromCharacter(word, true);
				
				//Randomization	for choosing a poem						
				int npoems = poemsindex.count();
				int low = 0;
				int chosen = qrand() % ((npoems) - low) + low;
				QString name = "poem" + poemsindex.takeAt(chosen); // QString::number(chosen);
				setPoemSelected(name, reply);					
			}			
		}
	}
}

//! 
//! Creates the new grammar with the words to be recognized
//!
void PoemReaderNode::createGrammar(QString wordselected)
{
	QString grammar;
	const AbstractParameter::List& parameterList = getGroupValue("wordsGroup", false)->getParameterList();
	ParameterGroup * wordGroup;

	foreach(AbstractParameter *rowList, parameterList) 
	{	
		wordGroup = (dynamic_cast<ParameterGroup *>(rowList));
		QString gropName = wordGroup->getName();
		QString wordString = gropName.remove("Parameter");
		qreal freqNumber = wordGroup->getParameter("frequency")->getValue().toReal();
		QString indexString = wordGroup->getParameter("poemIndexes")->getValueString();

		if (freqNumber > m_minFrequency && wordAccepted(wordString) && wordString.compare(wordselected, Qt::CaseInsensitive) != 0) 
		{
			grammar.append(wordString + "&");
		}	
		//we remove the word with low frequency
		else
		{			
			getGroupValue("wordsGroup", false)->removeParameterGroup(wordGroup->getName());
		}
	}	

	if (!grammar.isEmpty()) 
	{
		setValue("Grammar", grammar, true);
		groupParameter->propagateDirty();
		groupParameter->propagateEvaluation();
	}
	else
	{
		QString message = "No more words in the grammar. Please re-start again";
		reloadXmlFile();		

		setValue("Ready", true, true);
		m_recitingPoem = false;
	}
}


//!
//! Finds a given word in the ParameterList of the Node
//!
bool PoemReaderNode::findWord(QString str)
{
	const AbstractParameter::List& w_list = m_wordsGroup->getParameterList();

	foreach (AbstractParameter *row, w_list) 
	{
        const AbstractParameter::List& r_list = (dynamic_cast<ParameterGroup *>(row))->getParameterList();

		// The first parameter "word" (wordParameter) will be evaluated
		AbstractParameter *word = r_list.at(0);
		QString parameterName = dynamic_cast<Parameter *>(word)->getName();
		QString strInList = (dynamic_cast<Parameter *>(word))->getValueString();
		if (strInList.compare(str) == 0){
			return true;
		}
    }
	return false;
}

//!
//! Eliminates the wildcards "I choose" and "use ... please"
//!
QString PoemReaderNode::removeWildCards(QString word)
{
	if (word.contains("I choose"))
		word.remove("I choose");
	else if (word.contains("use "))
	{
		word.remove("use");
		word.remove("please");
	}
	word = word.trimmed();
	return word;
}

//!
//! This function allows to elicit an empathic response from the character
//!
QString PoemReaderNode::replyFromCharacter(QString word, bool finalWord)
{

	m_recitingPoem = false;

	QString reply = " ";
	QString tagB = "<synthesis:emotion id=\"QUERY_TRIGGER\" />  ";
	QString tagEnd = " <synthesis:emotion id=\"QUERY_LOOP\" /> ";

	if (word.compare("greet user")==0)
	{
		reply = "Hello! I am one of The Muses of Poetry. Select " + QVariant(m_numberWords).toString() + " words and I will recite a poem for you.";		
	}
	else 
	{
		//reply = "That is a beautiful word";
		QString tail = "";
		QStringList list;

		if (!finalWord)
			tail = " .. say another word" + tagEnd;

		word = tagB + word;
		list << word + " is a beautiful word" + tail << word  + " is a Great Choice" + tail << "Ok, I got " + word + tail << word + " is very interesting" + tail << "Let me see if I can work with " + word + tail << word + " sounds wonderful " + tail;

		//Randomization							
		int nsentences = list.count();
		int low = 0;
		int chosen = qrand() % ((nsentences) - low) + low;
		reply = list.at(chosen);		
	}

	return reply;
}

//!
//! 
//!
void PoemReaderNode::stopReciting()
{
	m_stopPoem = true;
}

//!
//! Lexicon that might be found in the poem but should not be recognized
//!
bool PoemReaderNode::wordAccepted(QString word)
{
	//TODO: this should read from XML file or something more elegant
	QStringList words;
	words << "this" << "there" << "that" << "where" << "when" << "what" << "would" << "could" << "will" << "with" << "yours" << "your" << "mine" << "their" << "blue" << "must" << "without"
			<< "into" << "each" << "we'd" << "they" << "though" << "through";

	if (words.contains(word, Qt::CaseInsensitive))
		return false;
	else
		return true;
}



///
/// Private Slots
///

//!
//! Load node settings from XML File
//!
void PoemReaderNode::loadXmlFile()
{
	Q_SLOT

	int indexOfPoem = 1;
	
	// get Filename from Input Parameter
	QString filename = getStringValue("XML File");
    if (filename == "") 
	{ 
		return; 
	}

	// load file or return on error
	QFile* xmlfile = new QFile(filename);
	if (!xmlfile->open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
		Log::error(QString("File %1 could not be loaded!").arg(filename), "PoemReaderNode::loadXmlFile");
		return;
	}
	QXmlStreamReader xml(xmlfile);

	// clear data from parameters
	m_stopPoem = false;
	m_poemsGroup->clear();
	m_poemsIndexStr.clear();
	selectedWords.clear();
	m_numberWordsSelected = 0;
	m_poemSelected->setValue(QVariant(""));
	
	//setValue("Alpha", QVariant(100.0), true);
	
	/* begins reading the content of the XML file */
	while(!xml.atEnd() && !xml.hasError()) 
	{
		xml.readNext();
		QString str = xml.name().toString();

		if(xml.name() == "poems" && xml.isStartElement()) 
		{
			bool inToken = true;

			while(inToken)
			{
				xml.readNext();
				if(xml.name() == "poem")
				{
					QXmlStreamAttributes attrs = xml.attributes();			
					QString poem = attrs.value("defaultValue").toString();

					poem.insert(0,  attrs.value("title").toString() + ". By " +  attrs.value("author").toString() + "&");
					poem.replace('[', '<');
					poem.replace(']', '>');
					poem.replace('\'', '\"');			
					poem.replace('*', '\'');

					if(!attrs.value("title").toString().isEmpty())
					{
						// set the values for the table that contains the data of the cloud
						QString name = "poem" + QString::number(indexOfPoem);
						Parameter *poemParameter = new Parameter( name, Parameter::T_String, QVariant(poem) ); 
						NumberParameter *buttonParameter = new NumberParameter("Select "+QString(name), Parameter::T_Command, name);

						m_poemsGroup->addParameter(poemParameter);
						m_poemsGroup->addParameter(buttonParameter);
						buttonParameter->setCommandFunction(SLOT(updatePoemSelected()));
						m_poemsIndexStr.append(QString::number(indexOfPoem)+",");
						indexOfPoem++;						
					}
				}			
				if(xml.name() == "poems" && xml.isEndElement()) 
				{
					inToken = false;
					m_poemsIndexStr.chop(1);
				}
			}
		}
	}

	// Give Feedback to the log
	if(xml.hasError())
		Log::error(QString("File %1 contains Errors!").arg(filename), "PoemReaderNode::loadXmlFile");
	else 
	{
		Log::info(QString("File %1 successfully loaded.").arg(filename), "PoemReaderNode::loadXmlFile");
		createPoemsData();
		createGrammar("");
		forcePanelUpdate();
	}

	m_finTimer->start(60);	// starts unfading
}


//!
//! wrapper function for loadXmlFile()
//!
void PoemReaderNode::reloadXmlFile()
{
	// return if no file to reload is selected
    if (getStringValue("XML File") == "") return;
	
	loadXmlFile();	
}

//!
//! Notifies that the output parameter with the line of the selected poem has changed
//!
void PoemReaderNode::updatePoemSelected()
{
	Parameter *parameter = dynamic_cast<Parameter *>(sender());
	NumberParameter *numberParameter = static_cast<NumberParameter *>(parameter);
	setPoemSelected(numberParameter->getValueString(), " ");
}


//!
//! Updates the value of the output parameter selected poem.
//! Due to issues with the voice synthesis the poem is split in lines and the update is done by lines 
//! and passed to the TcpClient line by line 
//!
void PoemReaderNode::setPoemSelected(QString name, QString replyCharacter)
{
	int i;
	QString poem;
	QStringList poemLines;

	i=0;
	poemLine = 0;

	setValue("Ready", false, true);	//stop the recognizing device (Kinect)

	m_foutTimer->start(60);			// starts fading
	
	poem = m_poemsGroup->getParameter(name)->getValueString();
	poem.replace('&', ' ');
	poemLines = poem.split("<n>");	//Break the poem by its lines
	
	m_poemLinesGroup->clear();

	foreach(QString line, poemLines)
	{	
		if (i==0)
		{
			line = replyCharacter + " .. " + line;
		}
		m_poemLinesGroup->addParameter(new Parameter("Line"+QString::number(i), Parameter::T_String, QVariant(line)));
		i++;
	}

	m_recitingPoem = true;

	triggerPoemLines();
}


//!
//! Triggers the poem line when the input value regarding the state of the animation is false
//!
void PoemReaderNode::triggerPoemLines(){
	
	//Parameter *param = parameterRoot->getParameter("Animation State");
	bool active = parameterRoot->getParameter("Animation State")->getValue().toBool();

	if (m_recitingPoem)
	{
		setValue("Ready", false, true);	// The "Ready" state will change only when a poem is not being read
	}
	else
		setValue("Is not Poem", true, true);
		
	if (!active){

		if (m_stopPoem)
		{
			parameterRoot->getParameterGroup("Poem Lines")->clear();
			setValue("Poem Selected", "<synthesis:emotion id=\"QUERY_LOOP\">", true);
			reloadXmlFile();		

			setValue("Ready", true, true);
			m_recitingPoem = false;
		}
		else
		{

			setValue("Is not Poem", active, true);

			Parameter *line = parameterRoot->getParameterGroup("Poem Lines")->getParameter("Line"+QString::number(poemLine));

			if (line)  {

				setValue("Is not Poem", false, true);

				poemLine++;
				QString tmp = line->getValueString();

				setValue("Poem Selected", tmp, true);
				if (tmp.contains("QUERY_LOOP")) 
				{				
					reloadXmlFile();		

					setValue("Ready", true, true);
					m_recitingPoem = false;
				}
			}		
		}		
	}
}





void PoemReaderNode::updateFadingOutTimer() {

	qreal alpha = getParameter("Alpha")->getValue().toFloat();

	if (alpha > 0.0 && alpha <= 100.0) {
		alpha -= 5.0;
		setValue("Alpha", QVariant(alpha), true);
		//std::cout << "alpha out: " << alpha << "\n" << std::endl;
		
		if (alpha <= 0.0){
			m_foutTimer->stop();
		}
	}	
}


void PoemReaderNode::updateFadingInTimer() {

	qreal alpha = getParameter("Alpha")->getValue().toFloat();

	if (alpha >= 0.0 && alpha < 100.0) {
		alpha += 5.0;
		setValue("Alpha", QVariant(alpha), true);
		//std::cout << "alpha in: " << alpha << "\n" << std::endl;

		if (alpha >= 100.0){
			m_finTimer->stop();
		}
	}	
}


} // namespace PoemReaderNode 
