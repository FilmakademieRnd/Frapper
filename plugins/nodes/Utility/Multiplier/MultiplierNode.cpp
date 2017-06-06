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
//! \file "MultiplierNode.h"
//! \brief Implementation file for MultiplierNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       14.01.2013 
//!				16.10.2013
//!

#include "MultiplierNode.h"

namespace MultiplierNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the MultiplierNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
MultiplierNode::MultiplierNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	// Make General Values Savelable
	setSaveable(true);

	m_inParameter = getParameter("input");
	m_outParameter = getParameter("output");
	
    // set affections and callback functions
    setProcessingFunction("input", SLOT(processOutputParameter()));
	setProcessingFunction("multi", SLOT(setMultiplier()));
}


//!
//! Destructor of the MultiplierNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MultiplierNode::~MultiplierNode ()
{
}


///
/// Public Methods
///


//!
//! Processes the output parameter depending on its name .
//!
void MultiplierNode::processOutputParameter ()
{
	NumberParameter *parameter = dynamic_cast<NumberParameter *>(sender());
	if (!parameter) 
		return;

	m_multi = getParameter("multi")->getValue().toFloat();

	float outValue = getParameter("input")->getValue().toFloat() * m_multi;

	setValue("output", QVariant(outValue), true);

	//notifyChange();
}

//!
//! Creates the input parameters for the node.
//!
void MultiplierNode::setupParameter()
{
	NumberParameter *parameter = dynamic_cast<NumberParameter *>(sender());
	if (!parameter) 
		return;

	const int size = parameter->getConnectedParameter()->getSize();
	parameter->setSize(size);
	m_outParameter->setSize(size);
}

//!
//! Sets the multiplier used for scaling the value.
//!
void MultiplierNode::setMultiplier()
{
	NumberParameter *parameter = dynamic_cast<NumberParameter *>(sender());
	if (!parameter) 
		return;

	m_multi = parameter->getValue().toFloat();

	processOutputParameter();
}

} // namespace MultiplierNode 
