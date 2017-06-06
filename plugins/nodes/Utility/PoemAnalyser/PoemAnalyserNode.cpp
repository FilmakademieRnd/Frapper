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
//! \file "PoemAnalyserNode.cpp"
//! \brief Implementation file for PoemAnalyserNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    1.0
//! \date       24.02.2012 (last updated)
//!

#include "PoemAnalyserNode.h"


namespace PoemAnalyserNode {
using namespace Frapper;

#define PLEASANT	0
#define NICE		1
#define PASSIVE		2
#define SAD			3
#define UNPLEASANT	4
#define NASTY		5
#define ACTIVE		6
#define FUN			7
#define HIMAGERY	8
#define LOIMAGERY	9

#define NRSTATES	8	//number of emotional states from the Whissell analysis -> 8 because we will not include "Low Imagery" and "high Imagery"
#define POEMSTATES	3	//the number of states to consider for the poem

///
/// Constructors and Destructors
///


//!
//! Constructor of the PoemAnalyserNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PoemAnalyserNode::PoemAnalyserNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	m_wordAnalysisGroup = new ParameterGroup("Word Analysis");
	m_poemAnalysisGroup = new ParameterGroup("Poem Analysis");
	m_poemTextGroup = new ParameterGroup("Poem Text");

	m_regularSpeedValue = 90;
	m_regularPitchValue = 90;

	FilenameParameter *filenameDAT = getFilenameParameter("Filename DAT"); 
	if (filenameDAT)
	{
		filenameDAT->setChangeFunction(SLOT(readDATFile()));
		filenameDAT->setCommandFunction(SLOT(reloadDATFile()));
	}
    else
        Log::error("Could not obtain all parameters required for parsing the Poems file.", "PoemAnalyserNode::PoemAnalyserNode");

	FilenameParameter *filenameFOR = getFilenameParameter("Filename FOR"); 
	if (filenameFOR)
	{
		filenameFOR->setChangeFunction(SLOT(readFORFile()));
		filenameFOR->setCommandFunction(SLOT(reloadFORFile()));
	}
    else
        Log::error("Could not obtain all parameters required for parsing the Poems file.", "PoemAnalyserNode::readFORFile");

	FilenameParameter *filenameTEXT = getFilenameParameter("Poem text"); 
	if (filenameTEXT)
	{
		filenameTEXT->setChangeFunction(SLOT(readTextFile()));
		filenameTEXT->setCommandFunction(SLOT(readTextFile()));
	}
	else
		Log::error("Could not obtain all parameters required for parsing the Poems file.", "PoemAnalyserNode::readTextFile");

	m_buttonParameter = new NumberParameter("Analysis Selected", Parameter::T_Command, "Select to analyse the poem");	
	parameterRoot->addParameter(m_buttonParameter);
	m_buttonParameter->setCommandFunction(SLOT(poemAnalysis()));
}


//!
//! Destructor of the PoemAnalyserNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PoemAnalyserNode::~PoemAnalyserNode ()
{
}


//! 
//! Reads the file obtained by the WDAL. This contains the analysis values of each word of the text
//!
//! returns: wordAnalysisGroup
//!
void PoemAnalyserNode::readDATFile()
{
	QFile *file = fileManagement("Filename DAT", "read");

	if (file->isReadable())
	{
		QTextStream ts(file);
		QStringList stateNames = ts.readLine().split(',');

		// Explanation: The first line is of the form: sample,word,emotion,activation,kpleasant,kactive,imagery,frequency,pleasant,nice,passive,sad,unpleasant,nasty,active,fun,himagery,loimagery
		// From this, we just need: word,emotion,activation,pleasant,nice,passive,sad,unpleasant,nasty,active,fun,himagery,loimagery 
		// Meaning positions 1, 2, 3, and the last 10 elements

		while(!ts.atEnd())
		{
			QStringList wordValues = ts.readLine().split(',');

			//Look for position 1 (second word) to get the name 
			QString wordName = wordValues.at(1).toLower();
			ParameterGroup *parameterGroup = m_wordAnalysisGroup->getParameterGroup(wordName);

			//if (!m_wordAnalysisGroup->contains(wordName + "Parameter"))
			if (parameterGroup == 0)
			{
				ParameterGroup *g = new ParameterGroup(wordName);

				int j=0;
				// to get emotion, activation (OJO: "emotion" stands for "pleasantness")
				for (int i=2; i<= 3; i++)
				{
					QString nameParameter = stateNames.at(i);
					QString valueParameter = wordValues.at(i+j) + "," + wordValues.at(i+j+1);

					if (valueParameter.toFloat() > 3.0)
					{
						// Get the values one position before
						valueParameter = wordValues.at(i+j-1) + "," + wordValues.at(i+j);
					}
					else if (valueParameter.compare(",")==0)
						valueParameter = "0,0";

					j++;

					Parameter *p = new Parameter(nameParameter+"Parameter", Parameter::T_String, QVariant(valueParameter));
					g->addParameter(p);
					Log::warning("poemAnalysisGroup: " + p->getName());	
				}

				// to get nice,pleasant,fun,active,nasty,unpleasant,sad,passive,himagery,loimagery
				int lengthNames = stateNames.length();
				int lengthValues = wordValues.length();
				for (int i=1; i<=10; i++)
				{
					QString nameParameter = stateNames.at(lengthNames-i);
					QString valueParameter = wordValues.at(lengthValues-i);

					if (valueParameter.compare("") == 0)
						valueParameter = "0";

					Parameter *p = new Parameter(nameParameter+"Parameter", Parameter::T_String, QVariant(valueParameter));
					g->addParameter(p);
					Log::warning("poemAnalysisGroup: " + p->getName());
				}

				// Now I add the word to the group of words
				m_wordAnalysisGroup->addParameter(g);
				Log::warning("wordAnalysisGroup: " + g->getName());
			}
		}
	}
	else
	{
		Log::error(QString("File %1 could not be loaded!").arg(file->fileName()), "PoemReaderNode::readSUMFile");
		return;
	}
}


//!
//! readFORFile()
//!
//! The .for file generated by the WDAL application contains the general data of the poem, separated by commas. 
//! It is a similar version of the .sum file, except that the .sum file is formatted to be easily read
//!
//! returns: poemAnalysisGroup
//!
void PoemAnalyserNode::readFORFile()
{
	QFile *file = fileManagement("Filename FOR", "read");

	//Clean the ParameterGroup in case a new file is loaded
	if (!m_poemAnalysisGroup->isEmpty())
		m_poemAnalysisGroup->clear();

	if (file->isReadable())
	{
		QTextStream ts(file);
		QStringList lineNames = ts.readLine().split(',');
		QStringList lineValues = ts.readLine().split(',');

		// Explanation: The first line is of the form: 
		//		sample,emotion,activation,imagery,aknown,kpleasant,kactive,kknown,words,frequency,slength,snum,excl,quest,nice,pleasant,fun,active,nasty,unpleasant,sad,passive,himagery,loimagery
		// From this, we just need: memotion,mactivation,mimagery,nice,pleasant,fun,active,nasty,unpleasant,sad,passive,himagery,loimagery
		// Meaning positions 1, 2, 3, and the last 10 elements

		int j=0;
		// to get emotion, activation, imagery
		for (int i=1; i<= 3; i++)
		{
			QString nameParameter = lineNames.at(i);
			QString valueParameter = lineValues.at(i+j) + "," + lineValues.at(i+j+1);
			j++;

			Parameter *p = new Parameter(nameParameter+"Parameter", Parameter::T_String, QVariant(valueParameter));
			m_poemAnalysisGroup->addParameter(p);
			Log::warning("poemAnalysisGroup: " + p->getName());	
		}

		// to get nice,pleasant,fun,active,nasty,unpleasant,sad,passive,himagery,loimagery
		j=0;
		int lengthNames = lineNames.length();
		int lengthValues = lineValues.length();
		for (int i=1; i<=10; i++)
		{
			QString nameParameter = lineNames.at(lengthNames-i);
			QString valueParameter = lineValues.at(lengthValues-(i+j+1))+ "," + lineValues.at(lengthValues-(i+j));
			j++;

			Parameter *p = new Parameter(nameParameter+"Parameter", Parameter::T_String, QVariant(valueParameter));
			m_poemAnalysisGroup->addParameter(p);
			Log::warning("poemAnalysisGroup: " + p->getName());
		}
	}
	else
	{
		Log::error(QString("File %1 could not be loaded!").arg(file->fileName()), "PoemReaderNode::readFORFile");
		return;
	}
}




//!
//! readTextFile
//!
//! Reads the content of a text file with the poem to add to the repository
//!
void PoemAnalyserNode::readTextFile()
{
	QFile *file = fileManagement("Poem text", "read");

	//Clean the ParameterGroup in case a new file is loaded
	if (!m_poemTextGroup->isEmpty())
		m_poemTextGroup->clear();

	if (file->isReadable())
	{
		QTextStream ts(file);
		QString line = "";
		int lineNr = 0;
		int nCurrPos = 0;
		int lineCount = -1;
		
		while(!ts.atEnd())
		{		
			QString lineTmp = ts.readLine();

			if(lineTmp != "")
			{
				nCurrPos = ts.pos();
				QString nameParameter = "Line" + QString::number(lineNr);

				char lastCharacter = getLastCharacter(lineTmp);

				if (lastCharacter == '.' ) // how to detect dash "—"??? TODO
				{
						line = line.append(" " + lineTmp);
						Parameter *p = new Parameter(nameParameter, Parameter::T_String, QVariant(line));
						m_poemTextGroup->addParameter(p);
						line = "";
						lineNr++;		
						lineCount = -1;
				}
				else
				{
					if (lineCount > 3)	//this is a dummy value
						lineTmp = linePostProcessing(lineTmp);
					line = line.append(" " + lineTmp);
					lineTmp = ts.readLine();

					if (lineTmp != "")
					{				
						QString firstCharacter = getFirstCharacter(lineTmp);
						if (firstCharacter == firstCharacter.toUpper())		//the next line begins with capital letter
						{
							//Random between 1.- creating a new line, or 2.- separate it with ".." (longer pause)
							int random = rand() % 2 + 1;
							if (random == 1)
							{
								//1.Create new line
								Parameter *p = new Parameter(nameParameter, Parameter::T_String, QVariant(line));
								m_poemTextGroup->addParameter(p);
								line = "";
								lineNr++;	
								lineCount = -1;
							}
							else if (random == 2)
							{
								//2.Append the ".."
								line = line.append(" .. ");								
							}											
						}			
						ts.seek(nCurrPos);	
						lineCount++;

					}
					else
					{
						//is the line-blank between stanzas
						Parameter *p = new Parameter(nameParameter, Parameter::T_String, QVariant(line));
						m_poemTextGroup->addParameter(p);
						line = "";
						lineNr++;			
						lineCount = -1;
					}
				}
			}
		}
	}
	else
	{
		Log::error(QString("File %1 could not be loaded!").arg(file->fileName()), "PoemReaderNode::readTextFile");
		return;
	}
}


//!
//! Reloads the content of the FOR File, which contains the WDAL analysis of the whole poem
//!
void PoemAnalyserNode::reloadFORFile(){
	
	m_poemAnalysisGroup->clear();

	readFORFile();
}


//!
//! Reloads the content of the DAT File, which contains the WDAL analysis of the whole poem
//!
void PoemAnalyserNode::reloadDATFile(){
	
	m_wordAnalysisGroup->clear();

	readDATFile();
}

//!
//! Reloads the content of the TEXT File, which contains the plain text of the poem
//!
void PoemAnalyserNode::reloadTextFile(){

	m_poemTextGroup->clear();

	readTextFile();
}


//!
//!
QFile * PoemAnalyserNode::fileManagement(QString f, QString mode)
{
	QString filename = getStringValue(f);

	if (filename == "")
		return NULL;

	QFile *file = new QFile(filename);

	if (mode.compare("read") == 0)
	{
		if(!file->open(QFile::ReadOnly)){
			Log::error(QString("File %1 could not be loaded!").arg(filename), "PoemReaderNode::readDATFile");
			return NULL;
		}
	}
	else if (mode.compare("write") == 0)
	{
		if(!file->open(QFile::WriteOnly))
		{
			Log::error(QString("File %1 could not be loaded!").arg(filename), "PoemReaderNode::readDATFile");
			return NULL;
		}
	}
	
	return file;
}


//!
//! Analysis of the poem
//!
//! Re-writes the "poemTextGroup" with the lines tagged with the corresponding emotional state
//! Generates the XML version of the poem to be added to the XML file with the rest of the poems
void PoemAnalyserNode::poemAnalysis()
{
	QString poemXML;
	QString poemAuthor = getParameter("Author")->getValueString();
	QString poemTitle =  getParameter("Title")->getValueString();


	//We analyze per line of the poem -> poemTextGroup
	if (!m_poemTextGroup->isEmpty())
	{
		int linesNr = m_poemTextGroup->getParameterList().size();
		QString nameParameter = "";
		poemXML = "<poem title= \"" + poemTitle + "\" type=\"String\" author= \"" +  poemAuthor  + "\"  defaultValue= \" \n"; 

		ParameterGroup *poemState = getPoemState();

		for (int i=0; i<linesNr; i++)
		{
			nameParameter = "Line" + QString::number(i);
			QString lineParameter = m_poemTextGroup->getParameter(nameParameter)->getValueString();
			lineParameter = lineAnalysisAndTagging(lineParameter, poemState);
			
			// Set the mood of the poem as a tag on the first line. 
			if (nameParameter.compare("Line0") == 0)
			{
				QString poemTag = setPoemTagging(poemState->getParameter("State Name")->getValueString().remove("Parameter"));
				lineParameter = poemTag + "\n" + lineParameter;
			}
			m_poemTextGroup->getParameter(nameParameter)->setValue(QVariant(lineParameter));	// re-write the poem line with the tagged one
			
			//add to XML
			poemXML = poemXML + lineParameter + "\n";
		}

		poemXML = poemXML + "[synthesis:emotion id='QUERY_LOOP' /] \" /> \n";

		writePoemInFile(poemXML);
	}
}

//!
//! getPoemState()
//!
//! Gets the overall state of the poem with its value.
//! Right now we just care for the states: nice, pleasant, fun, nasty, unpleasant and sad
//!
ParameterGroup* PoemAnalyserNode::getPoemState()
{
	ParameterGroup* state = new ParameterGroup("Poem State");
	QString statename;
	float statevalue = 0.0;

	const AbstractParameter::List& statesList = m_poemAnalysisGroup->getParameterList();

	foreach( AbstractParameter *a, statesList )
	{
		QString name = a->getName();

		//Right now we just care for the states: nice, pleasant, fun, nasty, unpleasant and sad
		if (!name.contains("tion") && !name.contains("ry") && !name.contains("ive"))
		{
			float valf = m_poemAnalysisGroup->getParameter(name)->getValueString().toFloat();

			if (valf > statevalue)
			{
				statevalue = valf;
				statename = name;
			}
		}
	}
	Parameter *sName = new Parameter("State Name", Parameter::T_String, QVariant(statename));
	Parameter *sValue = new Parameter("State Value", Parameter::T_Float, QString::number(statevalue));
	state->addParameter(sName);
	state->addParameter(sValue);

	return state;
}

//!
//! Analysis of one line of the poem
//!
//! 1.- We checked the 0 and 1 values of the states of each word. We sum them up and store in a table to know which is the predominant state.
QString PoemAnalyserNode::lineAnalysisAndTagging( QString poemLine, ParameterGroup *poemState)
{
	QTableWidget *tStates = new QTableWidget(10, 2); 	
	tStates = getLineStates(poemLine, tStates);
	
	//Get the most predominant state
	tStates = getLinePredominantState(tStates);

	if (tStates->rowCount() == 0)
	{
		//NO TAGGING
		poemLine = setLineProsodyTagging(poemLine, "", 0.0, poemState);	//We tag the line with prosodic values: speed and pitch
		poemLine = poemLine + " [n]";
	}
	else
	{
		ParameterGroup *lStateGroup = getLineTaggingState(tStates, poemState);
		QString lState = lStateGroup->getParameter("Line State Name")->getValueString();
		float lValue = lStateGroup->getParameter("Line State Value")->getValue().toFloat();

		poemLine = setLineProsodyTagging(poemLine, lState, lValue, poemState);	//We tag the line with prosodic values: speed and pitch
		poemLine = setLineTagging(lState, poemLine);
	}

	return poemLine;
}

//!
//! Table with the states of the word according to the data in the FOR file
QTableWidget* PoemAnalyserNode::getLineStates(QString poemLine, QTableWidget* table)
{
	QRegExp rx1("[\"!#$%&()*+,./:;?@_{|}~-]");	
	QStringList wordList = poemLine.split(" ");

	//Initialization of the table: "pleasant" << "nice" << "passive" << "sad" << "unpleasant" << "nasty" << "active" << "fun" << "himagery" << "loimagery";
	for (int i=0; i<10; i++)
	{
		QTableWidgetItem *name = new QTableWidgetItem(transformNumberToString(i));
		QTableWidgetItem *val = new QTableWidgetItem(0);
		table->setItem(i, 0, name);
		table->setItem(i, 1, val);
	}

	for (int i=0; i<wordList.length(); i++)
	{
		QString word = wordList.at(i);

		if (word.compare("") != 0 && wordIsOk(word))
		{		
			word = word.remove(rx1).toLower();
			ParameterGroup *w = m_wordAnalysisGroup->getParameterGroup(word);

			if (w != 0)
			{
				const AbstractParameter::List& lStates = w->getParameterList();

				foreach(AbstractParameter *a, lStates)
				{
					//ahora estan aqui los estados
					QString name = a->getName();	// name of the state

					if (!name.contains("tion")) // we do not store "emotion" and "activation" values
					{
						// From where to take the value of the state+
						int sValue = ((Parameter *)a)->getValue().toInt();

						if (sValue != 0)
						{				
							// Where to store the value of the state
							name = name.remove("Parameter");
							QTableWidgetItem *sItem = table->findItems( name, Qt::MatchContains ).first();
							int rowNr = table->row(sItem);
							sItem = table->item(rowNr, 1);	//
							int tmp = sItem->text().toInt() + sValue;
							sItem->setText(QString::number(tmp));
							table->setItem(rowNr, 1, sItem);
						}
					}			
				}
			}
		}
	}
	return table;
}

//! geLinePredominantState
//!
//! gets the predominant state among nice, pleasant, fun, nasty, unpleasant and sad
//!
QTableWidget * PoemAnalyserNode::getLinePredominantState( QTableWidget * tStates ) 
{
	int greatervalue = -1;
	tStates->sortByColumn(1, Qt::DescendingOrder);

	for (int i=0; i<tStates->rowCount(); i++)
	{
		int tmp = tStates->item(i, 1)->text().toInt();
		QString name = tStates->item(i, 0)->text();
		if (tmp >= greatervalue && tmp!=0 && !name.contains("imagery") && !name.contains("ive"))
		{
			greatervalue = tmp;
		}
		else
		{
			//Delete this cell. Now we are also removing "himagery" and "loimagery"
			tStates->removeRow(i);
			i--;
		}
	}
	return tStates;
}


//!
//! analyzes the states and returns the one to tag the line
ParameterGroup* PoemAnalyserNode::getLineTaggingState( QTableWidget * tStates, ParameterGroup * poemState )
{
	ParameterGroup* state = new ParameterGroup("Line State");
	QString statename;
	QString statevalue;
	bool sameState = false;
	int rows = tStates->rowCount();

	QString pState = poemState->getParameter("State Name")->getValueString().remove("Parameter");
	float pValue = poemState->getParameter("State Value")->getValue().toFloat();
		
	for (int i=0; i<rows; i++)
	{
		QString lState = tStates->item(i, 0)->text();
		QString lValue = tStates->item(i,1)->text();

		if (lState.compare(pState, Qt::CaseInsensitive) == 0)
		{
			statename = lState;
			statevalue = lValue;
			i = rows;	//finish the for-loop
			sameState = true;
		}
	}

	if (!sameState)
	{
		//we get the first state in the table
		statename = tStates->item(0, 0)->text();
		statevalue = tStates->item(0,1)->text();
	}

	Parameter *sName = new Parameter("Line State Name", Parameter::T_String, QVariant(statename));
	Parameter *sValue = new Parameter("Line State Value", Parameter::T_Float, QString::number(statevalue.toFloat()));
	state->addParameter(sName);
	state->addParameter(sValue);

	return state;
}

//! 
//! setLineTagging
//!
//! Returns the line of the poem tagged with the state
QString PoemAnalyserNode::setLineTagging( QString lState, QString line )
{
	// Conversions to make some of them more PAD-like
	if (lState.compare("fun", Qt::CaseInsensitive) == 0)
		lState = "joy";
	else if (lState.compare("nasty", Qt::CaseInsensitive) == 0)
		lState = "disgust";

	QString tag =  "[synthesis:emotion id='blank' /][synthesis:emotion id='" + lState.toUpper() + "_TRIGGER' /] ";
	line = tag + line + " [n]";

	return line;
}

QString PoemAnalyserNode::setPoemTagging( QString state)
{
	QString tag = "";
	if (state.compare("fun", Qt::CaseInsensitive) == 0 || state.compare("pleasant", Qt::CaseInsensitive) == 0 || state.compare("nice", Qt::CaseInsensitive) == 0) 
	{
		tag = "..[synthesis:emotion id='JOY_LOOP' /][n]";
	}
	else if ( state.compare("unpleasant", Qt::CaseInsensitive) == 0 || state.compare("nasty", Qt::CaseInsensitive) == 0 )
	{
		tag = "..[synthesis:emotion id='UNPLEASANT_LOOP' /][n]";
	}
	else if ( state.compare("sad", Qt::CaseInsensitive) == 0  )
	{
		tag = "..[synthesis:emotion id='SAD_LOOP' /][n]";
	}

	return tag;
}

//! Sets the prosody elements: speed and pitch, in the line
QString PoemAnalyserNode::setLineProsodyTagging( QString poemLine, QString lState, float lValue, ParameterGroup* poemState)
{
	QString pState = poemState->getParameter("State Name")->getValueString().remove("Parameter");
	float pValue = poemState->getParameter("State Value")->getValue().toFloat();
	int speedValue;
	int pitchValue;
	float lineEmotionalValue;
	
	//SPEED	
	lineEmotionalValue =  (pValue*lValue);

	if (pState.compare("fun", Qt::CaseInsensitive) == 0 || pState.compare("pleasant", Qt::CaseInsensitive) == 0 || pState.compare("nice", Qt::CaseInsensitive) == 0) 
	{
		speedValue = m_regularSpeedValue + (m_regularSpeedValue * 0.05);	//increment on 10% the speed - faster			(joy)	
		pitchValue = m_regularPitchValue + (m_regularPitchValue * 0.1);	//increment on 20% the pitch - very much higher 
	}
	else if ( pState.compare("unpleasant", Qt::CaseInsensitive) == 0 || pState.compare("nasty", Qt::CaseInsensitive) == 0 )
	{
		speedValue = m_regularSpeedValue - (m_regularSpeedValue * 0.08);	//decrement on 15% the speed - very much slower (disgust)
		pitchValue = m_regularPitchValue - (m_regularPitchValue * 0.08);	//decrement on 15% the pitch - very much lower 
		lineEmotionalValue *= (-1.0);
	}
	else if ( pState.compare("sad", Qt::CaseInsensitive) == 0  )
	{
		speedValue = m_regularSpeedValue - (m_regularSpeedValue * 0.05);	//decrement on 5% the speed - slightly slower (sad)
		pitchValue = m_regularPitchValue - (m_regularPitchValue * 0.05);	//decrement on 5% the pitch - slightly lower
		lineEmotionalValue *= (-1.0);
	}


	if (lState.compare(pState, Qt::CaseInsensitive) == 0)
	{	
		speedValue = speedValue + (QVariant(lineEmotionalValue).toInt());

		if (speedValue > 115)
			speedValue = 115;
		else if(speedValue < 70)
			speedValue = 70;

		//PITCH
		poemLine = setPitchTagging(poemLine, lState, lineEmotionalValue);
	}

	poemLine = "[synthesis:speed level='" + QString::number(speedValue) + "' ] " + poemLine + " [/synthesis:speed ] ";
	poemLine = "[synthesis:pitch level='" + QString::number(pitchValue) + "' ] " + poemLine + " [/synthesis:pitch ] ";
	return poemLine;
	
}

QString PoemAnalyserNode::setPitchTagging(QString poemLine, QString lState, float wValue)
{
	QRegExp rx1("[\"!#$%&()*+,./:;?@_{|}~-]");	
	QStringList listWords = poemLine.split(" ");

	for (int i=0; i<listWords.size(); i++)
	{
		QString word = listWords.at(i);
		QString wordTmp = listWords.at(i);
		wordTmp = wordTmp.remove(rx1).toLower();

		ParameterGroup* group = m_wordAnalysisGroup->getParameterGroup(wordTmp);

		if (group != 0)
		{
			Parameter *p = group->getParameter(lState+"Parameter");
			float val = p->getValue().toFloat();

			if (val != 0)
			{
				//Just 3 emotional states: happy, sad, anger(unpleasant)
				int newValue = m_regularPitchValue + ((QVariant(wValue).toInt()) );

				if (newValue > 130)
					newValue = 130;
				else if (newValue < 60)
					newValue = 60;
				
				QString newword = "[synthesis:pitch level='"  + QString::number(newValue) + "' ] " + word + " [/synthesis:pitch ] ";

				poemLine = poemLine.replace(word, newword);
			}			
		}
	}

	return poemLine;
}

//!
//! getLastCharacter(QString line)
//!
//! Gets the last character in the line
//!
char PoemAnalyserNode::getLastCharacter( QString line )
{
	QStringList words = line.split(" ");
	QString word = words.at(words.size()-1);
	char character = word.at(word.size()-1).toLatin1();
	return character;
}

//!
//! getFirstCharacter(QString line)
//!
//! Gets the first character in the line
//!
char PoemAnalyserNode::getFirstCharacter( QString line )
{
	QStringList words = line.split(" ");
	QString word = words.at(0);
	char character = word.at(0).toLatin1();
	return character;
}

//! to get rid of words that do not have meaning and biased the result
bool PoemAnalyserNode::wordIsOk( QString word )
{
	bool isOk = true;

	//Eliminate articles
	if (word.compare("the", Qt::CaseInsensitive) == 0)
		isOk = false;
	else if (word.compare("that", Qt::CaseInsensitive) == 0)
		isOk = false;
	else if (word.compare("and", Qt::CaseInsensitive) == 0)
		isOk = false;

	return isOk;
}


QString PoemAnalyserNode::transformNumberToString(int number){

	QString state;

	switch(number){
	case 0:
		state = "pleasant";
		break;
	case 1:
		state = "nice";
		break;
	case 2:
		state = "passive";
		break;
	case 3:
		state = "sad";
		break;
	case 4:
		state = "unpleasant";
		break;
	case 5:
		state = "nasty";
		break;
	case 6:
		state = "active";
		break;
	case 7:
		state = "fun";
		break;
	case 8:
		state = "himagery";
		break;
	case 9:
		state = "loimagery";
		break;
	default:
		state = "neutral";
		break;
	}
	return state;
}


int PoemAnalyserNode::transformStringToNumber(QString state)
{
	if (state.toLower().compare("pleasant"))
		return PLEASANT;
	else if (state.toLower().compare("nice"))
		return NICE;
	else if (state.toLower().compare("passive"))
		return PASSIVE;
	else if (state.toLower().compare("sad"))
		return SAD;
	else if(state.toLower().compare("unpleasant"))
		return UNPLEASANT;
	else if (state.toLower().compare("nasty"))
		return NASTY;
	else if (state.toLower().compare("active"))
		return ACTIVE;
	else if (state.toLower().compare("fun"))
		return FUN;
	else if (state.toLower().compare("himagery"))
		return HIMAGERY;
	else if (state.toLower().compare("loimagery"))
		return LOIMAGERY;
	else
		return -1;
}

//!
//! Writes the tagged poem into the XML file with the poems
//!
void PoemAnalyserNode::writePoemInFile( QString poemXML )
{
	setValue("String out", poemXML, true);
}


//!
//! We add punctuation marks for the sake of the reading.
//! We execute this function just once because we don't want to have too many "commas" in one sentence
//! The right analysis should evaluate what goes behind the conjunctions
//!
QString PoemAnalyserNode::linePostProcessing( QString line)
{
	// basic text processing
	if (line.contains("and"))
		line = line.replace("and", ", and");
	else if (line.contains("who"))
		line = line.replace("who", ", who");
	

	return line;
}

//!
//! Checks if the are negations (negative clauses) in the line. Ej: "was not", "no paradise", etc.
//!
void PoemAnalyserNode::checkNegationInLines( QString line )
{
	// First we check which type of negation is: verb, noun, adverb, pronoun

}

} // namespace PoemAnalyserNode 
