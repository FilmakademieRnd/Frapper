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
//! \file "WiiTrackerNode.h"
//! \brief Header file for WiiTrackerNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       19.05.2009 (last updated)
//!

#ifndef WIITRACKERNODE_H
#define WIITRACKERNODE_H

#include "Node.h"
#include <QtCore/QThread>
#include <QtCore/QList>
#include "wiiuse.h"
#include <QVector>

namespace WiiTrackerNode {
using namespace Frapper;

//!
//! Class representing nodes that can access Wii controllers.
//!
class WiiTrackerNode : public Node //, public QThread
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the WiiTrackerNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    WiiTrackerNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the WiiTrackerNode class.
    //!
    ~WiiTrackerNode ();

public: // functions

    virtual void run ();

public slots: //

    //!
    //! Starts or stops the thread for running the Wii tracker.
    //!
    //! Is called when the value of the run parameter has changed.
    //!
    void runChanged ();

    //!
    //! Is triggered of timer event.
    //!
    void timerUpdate ();

    //!
    //! Enable or disable motion sensing on WiiMotes
    //!
    void triggerMotionSensing();


private: // functions

    //!
    //! Finds and connects with Wiimote controllers that are in discovery mode.
    //!
    //! \return True if at least one Wiimote controller could be connected, otherwise False.
    //!
    bool initialize ();

    //!
    //! Callback that handles a Wiimote event.
    //!
    //! Is called by the wiiuse library when an event occurs on the specified
    //! Wiimote.
    //!
    //! \param wm The Wiimote controller that caused the event.
    //!
    void handleEvent ( struct wiimote_t *wm, int wiimoteID );

    //!
    //! Callback that handles a Wiimote disconnection event.
    //!
    //! Is called when the power button is pressed or when the connection is
    //! interrupted.
    //!
    //! \param wm The Wiimote controller that caused the event.
    //!
    void handleDisconnect ( wiimote *wm );

    void onStopThread ();

private: // data


    //!
	//! Array of wiimotes.
	//!
    wiimote **m_wiimotes;

    //!
    //! The number of available Wiimotes.
    //!
    int m_numFound;

    //!
    //! The number of connected Wiimotes.
    //!
    int m_numConnected;

	//!
	//! Is stop requested.
	//!
    bool m_stopRequested;

    //!
    //! List of roll values (averaging).
    //!
    QVector<QList<double>> m_rollList;

    //!
    //! List of pitch values (averaging).
    //!
	QVector<QList<double>> m_pitchList;

    //!
    //! List of pitch values (averaging).
    //!
    QVector<QList<double>> m_yawList;
    
};

} // namespace WiiTrackerNode

#endif
