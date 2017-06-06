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
//! \file "TimerNode.h"
//! \brief Header file for TimerNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       18.05.2009 (last updated)
//!

#ifndef TIMERNODE_H
#define TIMERNODE_H

#include "Node.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

// forward declaration
class QTimer;

namespace TimerNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class TimerNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the TimerNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TimerNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the TimerNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TimerNode ();

protected: // events

private slots:	

    //!
    //! Slot for the timer event
    //!
    //! \param event The description of the timer event.
    //!
    void timerEvent ();

    //! 
    //! Slot for the timer (de-)activation
    //! 
    void activateTimer();

	//! 
    //! Slot for the timer inteval change
    //! 
	void changeInterval();

	//! 
    //! Slot for the timer inteval in x/second change
    //! 
	void changePerSecond();


private: //funtions

private: // data
    int     m_baseTime;

    // timer functionality is implemented using a QTimer
    QTimer          *m_pTimer;

    // References to parameters for direct read and write access
    NumberParameter *m_pSystemTime;
    NumberParameter *m_pNormalizedTime;
    NumberParameter *m_pNormalizedTimeInterval;

    Parameter *m_pUpdate;

	QTime *m_sysTime;
};

} // namespace TimerNode 

#endif
