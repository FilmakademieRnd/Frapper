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
//! \file "NetworkAnimNode.h"
//! \brief Header file for MeshNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       07.12.2010 (last updated)
//!

#ifndef NETWORKANIMNODE_H
#define NETWORKANIMNODE_H

#include "Node.h"
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QQueue>

namespace NetworkAnimNode {
using namespace Frapper;

//!
//! Class in the Borealis application representing nodes that can
//! contai OGRE entities with animation.
//!
class NetworkAnimNode : public Node //, public QThread
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the NetworkAnimNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    NetworkAnimNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the NetworkAnimNode class.
    //!
    ~NetworkAnimNode ();

public: // functions

    virtual void run();


private slots: //

    //!
    //! Slot which is called when new update interval is set.
    //!
    void setInterval();

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void toggleRun();

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void processPendingDatagrams();

    //!
    //! Slot which is called when timer times out.
    //!
    void timeout ();
    
    //!
    //! Slot which is called when thread has finished.
    //!
    void threadFinished();

private: // functions
	
	float interpol( float index, const QMap<float, float> &valueMap );

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
    //! The current frame.
    //!
    int m_currentFrame;

    //!
    //! The animation chunks.
    //!
	QQueue<QMap<QString, float> > m_animationChunks;
};

} // namespace NetworkAnimNode 

#endif
