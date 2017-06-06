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
//! \file "KeyboardNode.cpp"
//! \brief Implementation file for KeyboardNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       19.05.2009 (last updated)
//!

#include "KeyboardNode.h"
#include "ParameterGroup.h"
#include <QCoreApplication>
#include <QKeyEvent>

namespace KeyboardNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the KeyboardNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
KeyboardNode::KeyboardNode ( QString name, ParameterGroup *parameterRoot ) :
	Node(name, parameterRoot),
	m_eatKeys(false)
{
	QCoreApplication::instance()->installEventFilter(this);

	initParameterMap();

	setChangeFunction("Eat Up Keys", SLOT(setOptions()));
	setOptions();
}


//!
//! Destructor of the KeyboardNode class.
//!
KeyboardNode::~KeyboardNode ()
{
}


///
/// Public Functions
///

///
/// Public Slots
///


///
/// Protected Functions
///

bool KeyboardNode::eventFilter(QObject *obj, QEvent *event)
{
	// handle key press
	if( event->type() == QEvent::KeyPress){

		QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);

		// ignore key press events from auto repeat, e.g. when holding down a character key
		if( !keyEvent->isAutoRepeat() ) {
			
			// check if event was handled
			bool handled = handleKeyPressEvent(keyEvent->key());

			if( handled && m_eatKeys ) {
				Log::debug( "Key press event of key " + keyEvent->text() + " was eaten by " + getName() + " node.", "KeyboardNode::eventFilter");
				return true;
			}

			// return value states of event should be proceeded further in event chain
			return false;
		}
	}

	// handle key release
	else if( event->type() == QEvent::KeyRelease){

		QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);

		// ignore key press events from auto repeat, e.g. when holding down a character key
		if( !keyEvent->isAutoRepeat() ) {

			// check if event was handled
			bool handled = handleKeyReleaseEvent(keyEvent->key());

			// return value states of event should be proceeded further in event chain
			return m_eatKeys;
		}
	}
	
	// non keyboard events
	else {
		return QObject::eventFilter(obj, event);
	}
}

bool KeyboardNode::handleKeyPressEvent( int keyId )
{
	if( m_parameterMap.contains(keyId)) {

		Parameter* keyParam = m_parameterMap[keyId];
		if( keyParam->isConnected() && !keyParam->getValue().toBool() ){
			keyParam->setValue( QVariant(true), true );
			return true;
		}
	}
	return false;
}

bool KeyboardNode::handleKeyReleaseEvent( int keyId )
{
	if( m_parameterMap.contains(keyId)) {

		Parameter* keyParam = m_parameterMap[keyId];
		if( keyParam->isConnected() && keyParam->getValue().toBool()) {
			keyParam->setValue( QVariant(false), true );
			return true;
		}
	}
	return false;
}

void KeyboardNode::initParameterMap()
{
	ParameterGroup* group = getParameterGroup( "Characters");
	assert( group );

	m_parameterMap[Qt::Key_A] = group->getParameter("Key_A");
	m_parameterMap[Qt::Key_B] = group->getParameter("Key_B");
	m_parameterMap[Qt::Key_C] = group->getParameter("Key_C");
	m_parameterMap[Qt::Key_D] = group->getParameter("Key_D");
	m_parameterMap[Qt::Key_E] = group->getParameter("Key_E");
	m_parameterMap[Qt::Key_F] = group->getParameter("Key_F");
	m_parameterMap[Qt::Key_G] = group->getParameter("Key_G");
	m_parameterMap[Qt::Key_H] = group->getParameter("Key_H");
	m_parameterMap[Qt::Key_I] = group->getParameter("Key_I");
	m_parameterMap[Qt::Key_J] = group->getParameter("Key_J");
	m_parameterMap[Qt::Key_K] = group->getParameter("Key_K");
	m_parameterMap[Qt::Key_L] = group->getParameter("Key_L");
	m_parameterMap[Qt::Key_M] = group->getParameter("Key_M");
	m_parameterMap[Qt::Key_N] = group->getParameter("Key_N");
	m_parameterMap[Qt::Key_O] = group->getParameter("Key_O");
	m_parameterMap[Qt::Key_P] = group->getParameter("Key_P");
	m_parameterMap[Qt::Key_Q] = group->getParameter("Key_Q");
	m_parameterMap[Qt::Key_R] = group->getParameter("Key_R");
	m_parameterMap[Qt::Key_S] = group->getParameter("Key_S");
	m_parameterMap[Qt::Key_T] = group->getParameter("Key_T");
	m_parameterMap[Qt::Key_U] = group->getParameter("Key_U");
	m_parameterMap[Qt::Key_V] = group->getParameter("Key_V");
	m_parameterMap[Qt::Key_W] = group->getParameter("Key_W");
	m_parameterMap[Qt::Key_X] = group->getParameter("Key_X");
	m_parameterMap[Qt::Key_Y] = group->getParameter("Key_Y");
	m_parameterMap[Qt::Key_Z] = group->getParameter("Key_Z");

	group = getParameterGroup( "Numbers");
	assert( group );

	m_parameterMap[Qt::Key_1] = group->getParameter("Key_1");
	m_parameterMap[Qt::Key_2] = group->getParameter("Key_2");
	m_parameterMap[Qt::Key_3] = group->getParameter("Key_3");
	m_parameterMap[Qt::Key_4] = group->getParameter("Key_4");
	m_parameterMap[Qt::Key_5] = group->getParameter("Key_5");
	m_parameterMap[Qt::Key_6] = group->getParameter("Key_6");
	m_parameterMap[Qt::Key_7] = group->getParameter("Key_7");
	m_parameterMap[Qt::Key_8] = group->getParameter("Key_8");
	m_parameterMap[Qt::Key_9] = group->getParameter("Key_9");
	m_parameterMap[Qt::Key_0] = group->getParameter("Key_0");

	group = getParameterGroup( "Directions");
	assert( group );

	m_parameterMap[Qt::Key_Up]       = group->getParameter("Key_Up");
	m_parameterMap[Qt::Key_Down]     = group->getParameter("Key_Down");
	m_parameterMap[Qt::Key_Left]     = group->getParameter("Key_Left");
	m_parameterMap[Qt::Key_Right]    = group->getParameter("Key_Right");
	m_parameterMap[Qt::Key_Home]     = group->getParameter("Key_Home");
	m_parameterMap[Qt::Key_End]      = group->getParameter("Key_End");
	m_parameterMap[Qt::Key_PageUp]   = group->getParameter("Key_PageUp");
	m_parameterMap[Qt::Key_PageDown] = group->getParameter("Key_PageDown");

	group = getParameterGroup( "Modifiers");
	assert( group );

	m_parameterMap[Qt::Key_Shift]   = group->getParameter("Key_Shift");
	m_parameterMap[Qt::Key_Control] = group->getParameter("Key_Control");
	m_parameterMap[Qt::Key_Meta]    = group->getParameter("Key_Meta");
	m_parameterMap[Qt::Key_Alt]     = group->getParameter("Key_Alt");
	m_parameterMap[Qt::Key_AltGr]   = group->getParameter("Key_AltGr");
	m_parameterMap[Qt::Key_Enter]   = group->getParameter("Key_Enter");
	m_parameterMap[Qt::Key_Return]  = group->getParameter("Key_Return");

	// check parameters valid
	foreach( Parameter* param, m_parameterMap.values()){
		assert( param);
	}
}

void KeyboardNode::setOptions()
{
	m_eatKeys = getBoolValue("Eat Up Keys");
}

} // namespace KeyboardNode
