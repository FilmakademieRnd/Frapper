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
//! \file "TriggerNode.h"
//! \brief Implementation file for TriggerNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       22.05.2014 (last updated)
//!

#include "TriggerNode.h"

namespace TriggerNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the TriggerNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TriggerNode::TriggerNode ( QString name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
m_lock(false)
{
	setSaveable(false);

	setCommandFunction("Now", SLOT(processOutputParameter()));

	NumberParameter* triggerBool = new NumberParameter("triggerBool", Parameter::T_Bool, false);
	NumberParameter* triggerInt = new NumberParameter("triggerInt", Parameter::T_Int, false);

	triggerBool->setEnabled(false, false);
	triggerInt->setEnabled(false, false);
	
	triggerBool->setPinType(Parameter::PT_Output);
	triggerInt->setPinType(Parameter::PT_Output);

	parameterRoot->addParameter(triggerBool);
	parameterRoot->addParameter(triggerInt);
}


//!
//! Destructor of the TriggerNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TriggerNode::~TriggerNode ()
{
}


//!
//! Processes the output parameter depending on its name .
//!
void TriggerNode::processOutputParameter ()
{
	if (!m_lock) {
		const bool newState = !getBoolValue("triggerBool");
		setValue("triggerBool", newState, true);
		setValue("triggerInt", (int) newState, true);

		m_lock = true;
		startTimer(500);
	}
}


//!
//! Handles timer events for the Node.
//!
//! \param event The description of the timer event.
//!
void TriggerNode::timerEvent ( QTimerEvent *event )
{
	m_lock = false;
	killTimer(event->timerId());
}

} // namespace TriggerNode 
