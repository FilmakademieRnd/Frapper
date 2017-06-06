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
//! \file "FaceShiftClientUDPNode.h"
//! \brief Header file for FaceShiftClientUDPNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.06.2012 (last updated)
//!

#ifndef FACESHIFTCLIENTUDPNODE_H
#define FACESHIFTCLIENTUDPNODE_H

#include "Node.h"
#include "ParameterGroup.h"
#include <QtCore/QDataStream>
#include <QtNetwork/QUdpSocket>
#include <QTime>

// FaceShift binary network stream format
#include <fsstream.h>

namespace FaceShiftClientUDPNode {
using namespace Frapper;

//!
//! Input node for FaceShift tracking data via tcp
//!
class FaceShiftClientUDPNode : public Node //, public QThread
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the FaceShiftClientUDPNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    FaceShiftClientUDPNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the FaceShiftClientUDPNode class.
    //!
    ~FaceShiftClientUDPNode ();

public: // functions

    virtual void run();


private slots: //

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void toggleRun();

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void processInputData();

    //!
    //! Slot which is called when thread has finished.
    //!
    void threadFinished();

private: // data

    //!
    //! The udp socket.
    //!
    QUdpSocket m_socket;

    //!
    //! Decoder for binary FaceShift network stream
    //!
    fs::fsBinaryTrackingStream m_trackingStream;
    
    //!
    //! The parameter group of animation parameters
    //!
    ParameterGroup* m_animParams;

    //!
    //! Stopwatch to measure update rate of faceshift 
    //!
    QTime m_stopWatch;


private: // functions

    // write decoded head translation to output parameters
    void UpdateHeadTranslation( Ogre::Vector3 trans);

    // write decoded head rotation to output parameters
    void UpdateHeadRotation( Ogre::Quaternion rot );

    // write decoded eye gaze to output parameters
    void UpdateEyeGaze( float pitchLeft, float pitchRight, float yawLeft, float yawRight);
};

} // namespace FaceShiftClientUDPNode 

#endif
