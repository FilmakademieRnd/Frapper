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
//! \file "MeanNode.cpp"
//! \brief Implementation file for MeanNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       14.01.2013 (last updated)
//!

#include "MeanNode.h"

namespace MeanNode {
using namespace Frapper;


///
/// Constructors and Destructors
///


//!
//! Constructor of the MeanNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
MeanNode::MeanNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CurveEditorDataNode(name, parameterRoot)
{
	m_inGroup = new ParameterGroup("In Group");
	parameterRoot->addParameter(m_inGroup);

	m_outParameter = getParameter("Out");			

	setChangeFunction("Nuber of Parameters", SLOT(setupParameters()));	
	
	setupParameters();
}


//!
//! Destructor of the MeanNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MeanNode::~MeanNode ()
{
}


//!
//! Creates the input parameters for the node.
//!
void MeanNode::setupParameters()
{
	// cleanup the world here
	m_inGroup->destroyAllParameters();


	const int nbrParas = getIntValue("Nuber of Parameters");

	for (int i=1; i<nbrParas+1; ++i) {
		NumberParameter *numberParameter = new NumberParameter(QString("In %1").arg(i), Parameter::T_Float, 0.0f);
		numberParameter->setNode(this);
		numberParameter->setPinType(Parameter::PT_Input);
		numberParameter->setSelfEvaluating(true);
		numberParameter->setMinValue(-1000.0);
		numberParameter->setMaxValue(1000.0);			
		m_inGroup->addParameter(numberParameter);

		if (i == 1)
			numberParameter->setProcessingFunction(SLOT(setParameters()));
	}	
	forcePanelUpdate();
}

//!+
//! Sets the parameter values.
//!
void MeanNode::setParameters()
{	
	float mean = 0.0;
	int numValues = 0;
	const AbstractParameter::List& parameterList = m_inGroup->getParameterList();

	foreach (AbstractParameter *absParameter, parameterList) {
			Parameter *parameter = static_cast<Parameter *>(absParameter);
			if (parameter->getValue().toFloat() != 0) {
				mean += parameter->getValue().toFloat();
				numValues++;
			}
		}
	if (numValues!=0)
		mean = mean/numValues;	
	m_outParameter->setValue(QVariant(mean), true);	
}

} 
