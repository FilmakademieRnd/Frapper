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
//! \file "String2BoolNode.cpp"
//! \brief Random Delegation to AniClipRandomizer-Nodes.
//!
//! \author     Volker Helzle (volker.helzle@filmakademie.de)
//! \version    1.0
//! \date       02.2012 
//!

#include "String2BoolNode.h"
#include <time.h>

namespace StringToBoolNode {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the String2BoolNode class.
//!
//! \param name The name to give the new mesh node.
//!
String2BoolNode::String2BoolNode ( QString name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
parameterRoot(parameterRoot)
{
	// Make General Values Savelable
	setSaveable(true);

	// set processing function for change string input 
	setProcessingFunction("Signal In", SLOT(delegateInput()));	
		
	// Set up a response to change of the number of outputs
	Parameter *outputCount = getParameter("Number of Outputs");
	outputCount->setChangeFunction(SLOT(updateOutputCount()));


	// add outputs to start
	/*addOutput();
	addOutput();*/
	updateOutputCount();
	
}


///
/// Public Destructors
///

//!
//! Destructor of the String2BoolNode class.
//!
String2BoolNode::~String2BoolNode ()
{
	// TODO:
}

///
/// Public Slots
///

//!
//! Delegate Signal to a random output
//!
void String2BoolNode::delegateInput()
{
	Q_SLOT

	// loop through custom parameters and trigger bool if match	
	for (int i = 0; i<m_Outputs.size(); i++) {				
		if ( getValue("Signal In", false).toString() == getValue( QString("string_%1").arg(i+1) , false).toString() ) 
			setValue(QString("output_%1").arg(i+1), true, true);
	}

	// reset outputs after 500ms
	if (getBoolValue("Clean Output after 500ms"))
		QTimer::singleShot(500, this, SLOT(resetOutputs()));

	else if (getBoolValue("Clean All Outputs except triggered at once"))
		resetOutputsExceptChecked(); 

	else if (getBoolValue("Clean All Outputs except triggered after 500ms"))
		QTimer::singleShot(500, this, SLOT(resetOutputsExceptChecked()));
 
}

//!
//! Reset outputs
//!
void String2BoolNode::resetOutputs()
{
	//reset all except triggered one
	for (int i = 0; i<m_Outputs.size(); i++) {
			setValue(QString("output_%1").arg(i+1), false, true);
	}	
	// update node
	notifyChange();
}

//!
//! Reset outputs
//!
//! reset all outputs, except the one that was activated
//!
void String2BoolNode::resetOutputsExceptChecked()
{
	//reset all except triggered one
	for (int i = 0; i<m_Outputs.size(); i++) {
		if ( getValue("Signal In", false).toString() != getValue( QString("string_%1").arg(i+1) , false).toString() ) 
			setValue(QString("output_%1").arg(i+1), false, true);
	}	
	// update node
	notifyChange();
}



//!
//! is called every time the outputCount changes
//!
void String2BoolNode::updateOutputCount()
{
	Q_SLOT

		// get new "Number of Outputs"-Value from interface
		int count = getIntValue("Number of Outputs");

	if(count>m_Outputs.size()) {
		do {
			//
			// value greater than outputCount
			addOutput();

		} while(count>m_Outputs.size());
	} else while(count<m_Outputs.size()) {
		//
		// value smaller than outputCount
		deleteOutput();
	}
	forcePanelUpdate();
}

//!
//! add another output slot
//!
void String2BoolNode::addOutput()
{
	// get output parameter group
	ParameterGroup *m_OutputParameters = parameterRoot->getParameterGroup("Outputs");

	// set up new output parameter 
	Parameter *newOutput;
	// add string parameter
	newOutput = new Parameter(QString("string_%1").arg(m_Outputs.size()+1), Parameter::T_String, "");
	m_OutputParameters->addParameter(newOutput); // add newParameter to group	
	newOutput->setVisible(true);	
	// add bool parameter
	newOutput = new Parameter(QString("output_%1").arg(m_Outputs.size()+1), Parameter::T_Bool, false);
	m_OutputParameters->addParameter(newOutput); // add newParameter to group
	newOutput->setPinType(Parameter::PT_Output);
	newOutput->setVisible(true);
	newOutput->setReadOnly(true);
	
	// increase m_Outputs
	m_Outputs.append(1);

	// update node
	notifyChange();
}


//!
//! delete newest output slot
//!
void String2BoolNode::deleteOutput()
{
	// get output parameter group
	ParameterGroup *m_OutputParameters = parameterRoot->getParameterGroup("Outputs");

	// delete parameter
	m_OutputParameters->removeParameter(QString("string_%1").arg(m_Outputs.size()));
	m_OutputParameters->removeParameter(QString("output_%1").arg(m_Outputs.size()));

	// decrease outputCount
	m_Outputs.removeLast();
	
	// update node
	notifyChange();
}

} // namespace StringToBoolNode 
