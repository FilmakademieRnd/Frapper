/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "Index2VisemeNode.cpp"
//! \brief Implementation file for Index2VisemeNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    1.0
//! \date       14.01.2013 (last updated)
//!

#include "Index2VisemeNode.h"

namespace Index2VisemeNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(Index2VisemeNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the Index2VisemeNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
Index2VisemeNode::Index2VisemeNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	m_inGroup = new ParameterGroup("In Group");
	parameterRoot->addParameter(m_inGroup);

	m_transitionsSpeed = getParameter("Transitions speed");

	m_outParameter = getParameter("Index Normalized");
	m_outViseme = getParameter("Viseme");

	setChangeFunction("Number of Parameters", SLOT(setupParameters()));
	setChangeFunction("Transitions speed", SLOT(setTransitionsSpeed()));
	
	setupParameters();

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the Index2VisemeNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
Index2VisemeNode::~Index2VisemeNode ()
{
	DEC_INSTANCE_COUNTER
}


//!
//! Creates the input parameters for the node.
//!
void Index2VisemeNode::setupParameters()
{
	// cleanup the world here
	m_inGroup->destroyAllParameters();

	m_countVowelRepeat = 0.0;

	const int nbrParas = getIntValue("Number of Parameters");

	for (int i=0; i<nbrParas; ++i) {
		//NumberParameter *numberParameter = new NumberParameter(QString("In %1").arg(i), Parameter::T_Float, 0.0f);
		NumberParameter *numberParameter = new NumberParameter(QString("%1").arg(i), Parameter::T_Float, 0.0f);
		numberParameter->setNode(this);
		numberParameter->setPinType(Parameter::PT_Input);
		numberParameter->setSelfEvaluating(true);
		numberParameter->setMinValue(-1000.0);
		numberParameter->setMaxValue(1000.0);
		m_inGroup->addParameter(numberParameter);

		numberParameter->setProcessingFunction(SLOT(setParameters()));
	}

	emit selectDeselectObject(getName());
}

//!
//! Sets the parameter values.
//!
void Index2VisemeNode::setParameters()
{
	Parameter *parameter = dynamic_cast<Parameter *>(sender());
	float visemeVal = parameter->getName().toFloat();
	QString visemeName; 
	float val = parameter->getValue().toFloat();
	float threshold = m_transitionsSpeed->getValue().toFloat();

	
	if (val > threshold)
	{				
		//Here we do the temporary mapping for Andreas images: From "viseme index" to "image index"
		if (visemeVal == 0.0)	//Viseme: silence  
		{
			visemeVal = 0.0;	
			visemeName = "silence";
			m_countVowelRepeat = 0.0;
		}
		else if (visemeVal == 2.0  || visemeVal == 9.0)	//Viseme: A   
		{
			visemeVal = 1.0;	
			visemeName = "A";
			m_countVowelRepeat = 0.0;
		}
		//
		// to make longer the vowel
		else if (visemeVal == 1.0)
		{		
			//std::cout << "m_countVowelRepeat: " << m_countVowelRepeat << std::endl; 
			visemeVal = m_countVowelRepeat;	
			if (m_countVowelRepeat < 5)
				m_countVowelRepeat++;

			visemeName = "A";
		}

		else if (visemeVal == 3.0 || visemeVal == 8.0 || visemeVal == 10.0)	//Viseme: O  
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 2.0;
			visemeName = "O/OI";
		}
		else if(visemeVal == 4.0 || visemeVal == 5.0)	//Viseme: e eI e@ (4) and 3: (5)
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 3.0;
			visemeName = "e";
		}
		else if(visemeVal == 6.0)	//Viseme: j i: I I@ 
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 4.0;
			visemeName = "j/i:/I/I@";
		}
		else if(visemeVal == 7.0)	//Viseme: w u:  
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 5.0;
			visemeName = "u";
		}
		else if(visemeVal == 11.0)	//Viseme: Ai   
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 6.0;
			visemeName = "aI";
		}
		else if(visemeVal == 13.0)	//Viseme: r  
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 7.0;
			visemeName = "r";
		}
		else if(visemeVal == 14.0)	//Viseme: l   
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 8.0;
			visemeName = "l";
		}
		else if(visemeVal == 15.0)	//Viseme: x/s/z
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 9.0;
			visemeName = "s";
		}
		else if(visemeVal == 16.0)	//Viseme: sch   
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 10.0;
			visemeName = "sch";
		}
		else if(visemeVal == 17.0)	//Viseme: t
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 11.0;
			visemeName = "T/D";
		}
		else if(visemeVal == 18.0)	//Viseme: f/v   
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 12.0;
			visemeName = "f/v";
		}
		else if(visemeVal == 19.0)	//Viseme: d t n
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 13.0;
			visemeName = "n/d/t";
		}
		else if(visemeVal == 12 || visemeVal == 20.0)	//Viseme: g/k 
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 14.0;
			visemeName = "g/k/N";
		}
		else if(visemeVal == 21.0)	//Viseme: p
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 15.0;
			visemeName = "p/b/m";
		}
		else
		{
			m_countVowelRepeat = 0.0;
			visemeVal = 0.0;
			visemeName = "not_map";
		}
	
		m_outParameter->setValue(QVariant(visemeVal), true);
		m_outViseme->setValue((QVariant(visemeName)), true);
		
	}
}

//!+
//! Sets the threshold to determine the visemes that are active.
//!
void Index2VisemeNode::setTransitionsSpeed()
{
	const int threshold = getFloatValue("Transitions speed");
	m_transitionsSpeed->setValue(QVariant(threshold), false);
}

} // namespace Index2VisemeNode 
