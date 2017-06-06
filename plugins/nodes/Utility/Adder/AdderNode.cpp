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
//! \file "AdderNode.h"
//! \brief Implementation file for AdderNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    1.0
//! \date       06.08.2013 (last updated)
//!

#include "AdderNode.h"

namespace AdderNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the AdderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
AdderNode::AdderNode ( QString name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
m_nbrInputs(0)
{
	// Make General Values Savelable
	setSaveable(true);

	m_outParameter = getNumberParameter("Out");
	m_outRoundedParameter = getNumberParameter("Out (rounded)");

	setChangeFunction("Number of Parameters", SLOT(updateInputCount()));
	updateInputCount();
}


//!
//! Destructor of the AdderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
AdderNode::~AdderNode ()
{
}


///
/// Public Methods
///

//!
//! is called every time the outputCount changes
//!
void AdderNode::updateInputCount()
{
	// get new "Number of Outputs"-Value from interface
	int count = getIntValue("Number of Parameters");

	if(count>m_nbrInputs) {
		do {
			//
			// value greater than outputCount
			addInput();

		} while(count>m_nbrInputs);
	} else while(count<m_nbrInputs) {
		//
		// value smaller than outputCount
		deleteInput();
	}
	forcePanelUpdate();
}

//!
//! add another output slot
//!
void AdderNode::addInput()
{
	// get input parameter group
	m_inGroup = getParameterGroup("In Group");
	
	// set up new input parameter 
	NumberParameter *numberParameter = new NumberParameter(QString("%1").arg(m_nbrInputs+1), Parameter::T_Float, 0.0f);
	numberParameter->setNode(this);
	numberParameter->setPinType(Parameter::PT_Input);
	numberParameter->setSelfEvaluating(true);
	numberParameter->setMinValue(-1000.0);
	numberParameter->setMaxValue(1000.0);			
	m_inGroup->addParameter(numberParameter);
	numberParameter->setVisible(true);

	numberParameter->setProcessingFunction(SLOT(processOutputParameter()));

	// increase m_Inputs
	m_nbrInputs+=1;

	// update node
	notifyChange();
}


//!
//! delete newest output slot
//!
void AdderNode::deleteInput()
{
	// get output parameter group
	ParameterGroup *m_InputParameters = getParameterGroup("In Group");

	// delete parameter
	m_InputParameters->removeParameter(QString("%1").arg(m_nbrInputs));

	// decrease outputCount
	m_nbrInputs-=1;

	// update node
	notifyChange();
}


//!
//! Processes the output parameter depending on its name .
//!
void AdderNode::processOutputParameter ()
{
	const AbstractParameter::List& parameterList = m_inGroup->getParameterList();
	float outValue = 0.0f; 

	foreach (AbstractParameter *absParameter, parameterList) {
		Parameter *parameter = static_cast<Parameter *>(absParameter);
		outValue += parameter->getValue().toFloat();
	}

	m_outParameter->setValue(QVariant(outValue), true);
	m_outRoundedParameter->setValue(QVariant(floor(outValue)), true);
}

} // namespace AdderNode 
