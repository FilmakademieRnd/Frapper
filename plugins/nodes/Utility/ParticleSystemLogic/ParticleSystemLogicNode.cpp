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
//! \file "ParticleSystemLogicNode.h"
//! \brief Implementation file for ParticleSystemLogicNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \date       17.10.2013 (last updated)
//!

#include "ParticleSystemLogicNode.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <iostream>
#include "Log.h"

namespace ParticleSystemLogicNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!

//! Constructor of the ParticleSystemLogicNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ParticleSystemLogicNode::ParticleSystemLogicNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{	

	m_visemeTagParameter = getParameter("Viseme Tag");
	m_visemeIndexParameter = getParameter("Viseme Index");
	m_emotionTagParameter = getParameter("Emotion Tag");
	m_readyState = getParameter("Ready");
	m_activeState = getParameter("Active");

	m_outputText = getParameter("Expressions XML");

	m_expressions_visemes = new ParameterGroup("Expressions and Visemes");
	

	
	// set animation timer
	m_foutTimer = new QTimer(); 
	m_finTimer = new QTimer();
	connect(m_foutTimer, SIGNAL(timeout()), SLOT(updateFadingOutTimer()));
	connect(m_finTimer, SIGNAL(timeout()), SLOT(updateFadingInTimer()));

	// Connections
	m_activeState->setProcessingFunction(SLOT(getExpressionsFromText()));
	m_visemeTagParameter->setProcessingFunction(SLOT(getVisemeTimeandTag()));
	m_emotionTagParameter->setProcessingFunction(SLOT(getExpressionTimeandTag()));
	connect(this, SIGNAL(emitEndReading()), this, SLOT(extractAnimationInfo()));
	
}

//!
//! Destructor of the ParticleSystemLogicNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ParticleSystemLogicNode::~ParticleSystemLogicNode ()
{
	
}

///
/// Private Methods
///

void ParticleSystemLogicNode::getExpressionsFromText()
{
	QString tag = m_emotionTagParameter->getValueString();
	if (m_activeState->getValue().toBool())		//when Ready is false, indicates that poem is being read
	{
		m_startTimestamp = QTime::currentTime();
		m_expressions_visemes->clear();
		listvisemes.clear();
		listexpressions.clear();
		listvisemes_expressions.clear();
		timeline.clear();		

		// At the moment, this node is used to control de Particle System
		startParticleSimulation();
	}
	else
	{
		if(m_readyState->getValue().toBool())
		{
			endParticleSimulation();
			emitEndReading();
		}
	}
}

void ParticleSystemLogicNode::getVisemeTimeandTag()
{
	float visemeIndex = m_visemeIndexParameter->getValue().toFloat();
	//QString viseme = m_visemeTagParameter->getValueString();

	//we use just the vowels to map to open/close mouth
	QString viseme = vowelMapping(visemeIndex);

	if (viseme != "")
	{
		QTime currentTimeStamp = QTime::currentTime();
		int miliseconds = currentTimeStamp.msecsTo(m_startTimestamp) * (-1);
		float frameViseme = ((float)miliseconds / 1000.0) * 25.0 ; 

		QString output = QString::number(visemeIndex) + "_" + viseme + ":" + QString::number(miliseconds) + ":" + QString::number(frameViseme);

		if (!listvisemes_expressions.contains(output))
			listvisemes_expressions.append(output);
	}	
}

void ParticleSystemLogicNode::getExpressionTimeandTag()
{
	QTime currentTimeStamp = QTime::currentTime();
	int miliseconds = currentTimeStamp.msecsTo(m_startTimestamp) * (-1);
	float frameExpression = ((float)miliseconds / 1000.0) * 25.0 ; 

	QString expression = m_emotionTagParameter->getValueString();
	QString output = expression + ":" + QString::number(miliseconds) + ":" + QString::number(frameExpression) ;

	if (!listvisemes_expressions.contains(output) && expression.compare("blank") != 0)
		listvisemes_expressions.append(output);

	// Logic with 3 emotional states: Unpleasant, Sad, Joy
	qreal force; 
	if (expression == "JOY_LOOP" || expression == "JOY_TRIGGER" || expression == "PLEASANT_TRIGGER")
	{
		force = 0.200;
		
	}
	else if (expression == "SAD_LOOP" || expression == "SAD_TRIGGER")
	{
		force = 0.050;
	}
	else 
	{
		force = 0.0100;
	}
	setValue("Inter Particle Force", QVariant(force), true);
}


//! 
//! to create the time list and the visemes/expressions list 

void ParticleSystemLogicNode::extractAnimationInfo()
{
	//1. extract the list with the time
	for (int i=0; i<listvisemes_expressions.size(); i++)
	{
		//std::cout << listvisemes_expressions.at(i).toStdString() << std::endl;

		QStringList item = listvisemes_expressions.at(i).split(":");
		float secs = item.at(1).toFloat() / 1000.0;
		//float secs = item.at(2).toFloat();
		timeline.append(" " + QString::number(secs));
	}
	
	//2. create the groups with the values of the visemes/expressions
	for (int i=0; i<listvisemes_expressions.size(); i++)
	{
		QString listItem = listvisemes_expressions.at(i);
		createVisemeExpressionParameter(listItem, i);
	}

	Parameter *tagParameter = getParameter("Emotion Tag");
	QString tag = tagParameter->getValueString();

	writetoText();

	setValue("Viseme is open", false, true);
}


void ParticleSystemLogicNode::createVisemeExpressionParameter(QString listItem, int positionKey)
{
	QStringList list = listItem.split(":");
	QString name = list.at(0);
	QString frames = list.at(2);
	bool exists = false;

	Parameter *parameter = m_expressions_visemes->getParameter(name);

	const AbstractParameter::List& cues_list = m_expressions_visemes->getParameterList();
	
	QString valuesList;
	QString name_parameter; 

	if (!cues_list.isEmpty())
	{
		foreach (AbstractParameter *cue, cues_list) 
		{
			Parameter *cue_parameter = static_cast<Parameter *>(cue);
			valuesList = cue_parameter->getValueString();
			name_parameter = cue_parameter->getName();

			if (name_parameter.compare(name) == 0)
			{				
				valuesList = valuesList.append(" 1");
			}
			else
			{
				// we have to add zeros in this positions to all the other parameters
				valuesList = valuesList.append(" 0");
			}
			cue_parameter->setValue(valuesList, true);
		}
	}

	if (parameter == 0)	//the parameter does not exist
	{
		QString animvalues;
		if (positionKey > 0)
		{			
			for (int i=0; i<positionKey; i++)
			{
				animvalues.append(" 0");  
			}			
		}
		animvalues.append(" 1");
		parameter = new Parameter(name, Parameter::T_String, QVariant(animvalues));
		m_expressions_visemes->addParameter(parameter);
	}
}


void ParticleSystemLogicNode::writetoText()
{
	QString textXML = "";
	QString line;

	line = "<time id=\"";
	for (int i=0; i<timeline.size(); i++)
	{
		 line.append(timeline.at(i));
	}
	line.append(" \"> \n");
	textXML.append(line);
	

	const AbstractParameter::List& cuesList = m_expressions_visemes->getParameterList();
	foreach(AbstractParameter *item, cuesList)
	{
		QString durations;

		durations = dynamic_cast<Parameter*>(item)->getValueString();
		
		line = "<animation id=\"" + item->getName() + "\">";
		line.append(durations);
		line.append("</animation> \n" );
		textXML.append(line);
	}
	m_outputText->setValue(textXML, true);
}

//!
//! This function filters vowels from the set of visemes. They are divided in 3 groups: 
//!
//!		Open			Close					Central
//! -------------------------------------------------------
//!   A:  = 2		i i: I I@ = 6			   @U = 8
//!   aI  = 11			 w u: = 7			  { @ = 1
//!	  aU  = 9						 e eI e@ U U@ = 4
//!	  O:  = 3								3:	  = 5 
//!   OI  = 10
//!
QString ParticleSystemLogicNode::vowelMapping(float visemeIndex)
{
	QString viseme = "";
	qreal mouthForce = 0.0;
	
	if (visemeIndex == 1.0 || visemeIndex == 6.0 )
	{
		viseme = "Open_A";
		mouthForce = 40.0;
		speakParticleSimulation(true, mouthForce);
	}
	else if (visemeIndex == 2.0 )
	{
		viseme = "Open_O";
		mouthForce = 40.0;
		speakParticleSimulation(true, mouthForce);
	}
	else if (visemeIndex == 3.0 )
	{
		viseme = "Central_E";
		mouthForce = 20.0;
		speakParticleSimulation(true, mouthForce);		
	}
	else if (visemeIndex == 0.0)
	{
		viseme = "Silence";
		mouthForce = 0.0;
		speakParticleSimulation(false, mouthForce);
		
	}
	else if (visemeIndex == 4.0)
	{
		viseme = "Close_I";
		mouthForce = 0.0;
		speakParticleSimulation(false, mouthForce);
	}
	else if (visemeIndex == 5.0)
	{
		viseme = "Close_U";
		mouthForce = 0.0;
		speakParticleSimulation(false, mouthForce);
	}
	else if (visemeIndex == 9.0 || visemeIndex == 10.0 || visemeIndex == 13.0 || visemeIndex == 14.0)
	{
		viseme = "Open Consonant";
		mouthForce = 30.0;
		speakParticleSimulation(true, mouthForce);	
	}
	else
	{
		//setValue("Viseme is open", false);
		speakParticleSimulation(false, mouthForce);
	}

	return viseme;
}

//!!!!!!!! Logic for the Particle Simulation
//!
void ParticleSystemLogicNode::startParticleSimulation()
{
	
	float meshForce = 0.0;	//super empirical value

	speakParticleSimulation(false, meshForce);
	
	m_finTimer->start(40);			// particles come together

	
}

void ParticleSystemLogicNode::endParticleSimulation()
{
	float mouthForce = 0.0;	//super empirical value
	float meshForce = 20.0;	//super empirical value

	speakParticleSimulation(false, mouthForce);
	
	m_foutTimer->start(40);			// particles spread away
	
}

void ParticleSystemLogicNode::speakParticleSimulation(bool visemeIsOpen, qreal mouthForce)
{
	setValue("Viseme is open", visemeIsOpen, true);
	setValue("Mouth Force", mouthForce, true);
}

void ParticleSystemLogicNode::updateFadingOutTimer() {

	qreal meshForce = getParameter("Mesh Force")->getValue().toFloat();

	if (meshForce > 0.0 && meshForce <= 135.0) {
		meshForce -= 15.0;
		setValue("Mesh Force", QVariant(meshForce), true);

		if(meshForce <= 0.0)
			m_foutTimer->stop();
	}	
}


void ParticleSystemLogicNode::updateFadingInTimer() {

	qreal meshForce = getParameter("Mesh Force")->getValue().toFloat();

	if (meshForce >= 0.0 && meshForce < 135.0) {
		meshForce += 15.0;
		setValue("Mesh Force", QVariant(meshForce), true);

		if(meshForce >= 124.0)
			m_finTimer->stop();
	}	

}

}