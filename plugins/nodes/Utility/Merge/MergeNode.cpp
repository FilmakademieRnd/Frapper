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
//! \file "MergeNode.cpp"
//! \brief Implementation file for MergeNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       14.01.2013 (last updated)
//!

#include "MergeNode.h"

namespace MergeNode {
using namespace Frapper;


///
/// Constructors and Destructors
///


//!
//! Constructor of the MergeNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
MergeNode::MergeNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CurveEditorDataNode(name, parameterRoot)
{
	m_inGroup = new ParameterGroup("In Group");
	parameterRoot->addParameter(m_inGroup);

	m_outParameter = getParameter("Out");

	setChangeFunction("Number of Parameters", SLOT(setupParameters()));

	
	setupParameters();
}


//!
//! Destructor of the MergeNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MergeNode::~MergeNode ()
{
}


//!
//! Creates the input parameters for the node.
//!
void MergeNode::setupParameters()
{
	// cleanup the world here
	m_inGroup->destroyAllParameters();
	ParameterGroup* rootGroup = getParameterRoot();
	rootGroup->removeParameter(m_outParameter, true);


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

	// Recreate m_outParameter
	m_outParameter = new NumberParameter("Out", Parameter::T_Float, 0.0f);
	m_outParameter->setPinType(Parameter::PT_Output);
	rootGroup->addParameter(m_outParameter);

	emit nodeChanged();
	setParameters();
	forcePanelUpdate();
}

//!
//! Sets the parameter values.
//!
void MergeNode::setParameters()
{
	QVariantList values;
	const AbstractParameter::List& parameterList = m_inGroup->getParameterList();

	if (parameterList.size() == 3) {
		m_outParameter->setSize(3);
		m_outParameter->setValue(QVariant::fromValue<Ogre::Vector3>(Ogre::Vector3(
			static_cast<Parameter *>(parameterList.at(0))->getValue().toFloat(),
			static_cast<Parameter *>(parameterList.at(1))->getValue().toFloat(),
			static_cast<Parameter *>(parameterList.at(2))->getValue().toFloat())), true);
	}
	
	else {
		foreach (AbstractParameter *absParameter, parameterList) {
			Parameter *parameter = static_cast<Parameter *>(absParameter);
			values << parameter->getValue();
		}

		m_outParameter->setValues(values, true);
	}
}

} // namespace MergeNode 
