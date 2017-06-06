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
//! \file "FaceShiftClientNode.h"
//! \brief Header file for FaceShiftClientNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.06.2012 (last updated)
//!

#ifndef FACESHIFTCLIENTNODE_H
#define FACESHIFTCLIENTNODE_H

#include "Node.h"
#include "ParameterGroup.h"
#include <QtCore/QDataStream>
#include <QtNetwork/QTcpSocket>
#include <QTime>

// FaceShift binary network stream format
#include <fs_network/fsbinarystream.h>

namespace FaceShiftClientNode {
using namespace Frapper;

//!
//! Input node for FaceShift tracking data via tcp
//!
class FaceShiftClientNode : public Node //, public QThread
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the FaceShiftClientNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    FaceShiftClientNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the FaceShiftClientNode class.
    //!
    ~FaceShiftClientNode ();

private slots: //

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void toggleRun();

	//!
	//! Send signal to faceshift to calibrate neutral pose
	//!
	void calibrateNeutralPose();

	//!
	//! Slot for head pose paramter, either relative or absolute
	//!
	void setHeadPose();

	//! Try to establish a connection to faceshift using the given host and port informations
	void establishConnection();

	//! Close the current connection
	void closeConnection();

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void processInputData();

	//!
	//! Called when the rotation offset has changed
	//!
	void rotationOffsetChanged();

    //!
    //! Called when the maximum eye angle for yaw / pitch of eyes changes
    //!
    void eyeAngleChanged();

    //!
    //! Called when the maximum eye angle for yaw / pitch / roll of head changes
    //!
    void headAngleChanged();

private: // functions

	//! process tracking state received from faceshift
	void processTrackingState( fs::fsMsgTrackingState *trackingState );

    //! write decoded head rotation to output parameters
    void UpdateHeadBone();

    //! write decoded eye gaze to output parameters
    void UpdateEyeGaze();

	bool sendMessage( fs::fsMsg::MessageType msg );

private: // data

    //!
    //! The udp socket.
    //!
    QTcpSocket m_tcpSocket;

    //!
    //! Decoder for binary FaceShift network stream
    //!
    fs::fsBinaryStream m_trackingStream;
    
    //!
    //! The parameter group of animation parameters
    //!
    ParameterGroup* m_animParams;

    //!
    //! Stopwatch to measure update rate of faceshift 
    //!
    QTime m_stopWatch;

    //!
    //! The tracking data from the tcp stream
    //!
    fs::fsTrackingData m_trackingData;

    //!
    //! The last rates
    //!
    QList<int> m_updateRates;

	//!
	//! Addition head rotation offset
	//!
	Ogre::Vector3 m_headRotationOffset;

    //! Head orientation parameter
    NumberParameter *m_headOrientationParam;
    NumberParameter *m_headLeftParam;
    NumberParameter *m_headRightParam;
    NumberParameter *m_headUpParam;
    NumberParameter *m_headDownParam;
    NumberParameter *m_headTiltLeftParam;
    NumberParameter *m_headTiltRightParam;

    //! Eye orientation parameter
    NumberParameter *m_eyeGazeLeftParam;
    NumberParameter *m_eyeGazeRightParam;
    NumberParameter *m_eyesLeftParam;
    NumberParameter *m_eyesRightParam;
    NumberParameter *m_eyesUpParam;
    NumberParameter *m_eyesDownParam;
    Ogre::Degree     m_eyesMaximumUp;
    Ogre::Degree     m_eyesMaximumDown;
    Ogre::Degree     m_eyesMaximumLeft;
    Ogre::Degree     m_eyesMaximumRight;

    Ogre::Degree    m_HeadMaximumUp;
    Ogre::Degree    m_HeadMaximumDown;
    Ogre::Degree    m_HeadMaximumLeft;
    Ogre::Degree    m_HeadMaximumRight;
    Ogre::Degree    m_HeadMaximumTiltLeft;
    Ogre::Degree    m_HeadMaximumTiltRight;

	float			m_headYawScale;
	float			m_headRollScale;
	float			m_headPitchScale;

};

} // namespace FaceShiftClientNode 

#endif
