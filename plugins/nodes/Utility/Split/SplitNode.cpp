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
//! \file "SplitNode.cpp"
//! \brief Implementation file for SplitNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       14.01.2013 (last updated)
//!

#include "SplitNode.h"

namespace SplitNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the SplitNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
SplitNode::SplitNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	m_outGroup = new ParameterGroup("Out Group");
	parameterRoot->addParameter(m_outGroup);

	m_inParameter = getParameter("In");
	m_inParameter->setSelfEvaluating(true);
	m_inParameter->setProcessingFunction(SLOT(setParameters()));
	m_inParameter->setOnCreateConnectionFunction(SLOT(setupParameters(Parameter *)));
	m_inParameter->setOnDisconnectFunction(SLOT(cleanupParameters()));
}


//!
//! Destructor of the SplitNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
SplitNode::~SplitNode ()
{
}

//!
//! Creates the input parameters for the node.
//!
void SplitNode::setupParameters(Parameter *sourceParameter)
{
	NumberParameter *inParameter = dynamic_cast<NumberParameter *>(sender());
	if (!inParameter || !sourceParameter)
		return;

	const int size = sourceParameter->getSize();
	inParameter->setSize(size);

	for (int i=size; i>0; --i) {
		NumberParameter *numberParameter = new NumberParameter(QString("Out %1").arg(i), Parameter::T_Float, 0.0f);
		numberParameter->setNode(this);
		numberParameter->setPinType(Parameter::PT_Output);
		m_outGroup->addParameter(numberParameter);
		numberParameter->setSelfEvaluating(true);
	}
	emit nodeChanged();
	setParameters();
	forcePanelUpdate();
}

//!
//! Cleans the out parameters.
//!
void SplitNode::cleanupParameters()
{
	m_outGroup->destroyAllParameters();
}

//!
//! Sets the parameter values.
//!
void SplitNode::setParameters()
{
	NumberParameter *parameter = dynamic_cast<NumberParameter *>(sender());

	const QVariantList &srcValues = parameter->getValues();
	const AbstractParameter::List& destValues = m_outGroup->getParameterList();

	for (int i=0; i<srcValues.size(); ++i) {
		if (!destValues.empty()) {
			Parameter *destParameter = static_cast<Parameter *>(destValues.at(i));
			destParameter->setValue(srcValues.at(i), true);
		}
	}
}

} // namespace SplitNode 
