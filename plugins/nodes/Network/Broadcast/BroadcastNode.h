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
//! \file "BroadcastNode.h"
//! \brief Header file for MeshNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#ifndef BroadcastNode_H
#define BroadcastNode_H

#include "Node.h"
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

// OGRE
#include "Ogre.h"
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace BroadcastNode {
using namespace Frapper;

//!
//! Class in the Borealis application representing nodes that can
//! contai OGRE entities with animation.
//!
class BroadcastNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the BroadcastNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    BroadcastNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the BroadcastNode class.
    //!
    ~BroadcastNode ();

public: // functions


public slots: //

    //!
    //! Slot which is called when new host address is set.
    //!
    void setHostAddress();

    //!
    //! Slot which is called when new port is set.
    //!
    void setPort();

    //!
    //! Slot which is called when new update interval is set.
    //!
    void setInterval();

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void toggleRun();

    //!
    //! On timer update.
    //!
    void updateTimer ();

protected: // functions


private: // data

    //!
    //! The update timer.
    //!
    QTimer *m_timer;

    //!
    //! The udp socket.
    //!
    QUdpSocket *m_udpSocket;

    //!
    //! The destination host address.
    //!
    QHostAddress m_hostAddress;

    //!
    //! The destination port.
    //!
    unsigned int m_port;

    //!
    //! The sequence id.
    //!
    unsigned int m_sequenceIdHigh;
    unsigned int m_sequenceIdLow;
};

} // namespace BroadcastNode 

#endif
