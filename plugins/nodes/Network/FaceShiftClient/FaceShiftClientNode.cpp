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
//! \file "FaceShiftClientNode.cpp"
//! \brief Implementation file for FaceShiftClientNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.06.2012 (last updated)
//!

#include "FaceShiftClientNode.h"
#include "NumberParameter.h"

#include <OgreQuaternion.h>
#include <OgreVector3.h>

namespace FaceShiftClientNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(FaceShiftClientNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the FaceShiftClientNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
FaceShiftClientNode::FaceShiftClientNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_animParams(NULL)
{   
    setChangeFunction("Run", SLOT(toggleRun()));
	setChangeFunction("Head > Orientation", SLOT(setHeadPose()));
	setChangeFunction("Head > Rotation Offset", SLOT(rotationOffsetChanged()));

	setProcessingFunction("Calibrate Neutral Pose", SLOT(calibrateNeutralPose()));

    // init head orientation parameter
    m_headOrientationParam = this->getNumberParameter("Head > Orientation");
    m_headLeftParam        = this->getNumberParameter("Head > HeadLeft");
    m_headRightParam       = this->getNumberParameter("Head > HeadRight");
    m_headUpParam          = this->getNumberParameter("Head > HeadUp");
    m_headDownParam        = this->getNumberParameter("Head > HeadDown");
    m_headTiltLeftParam    = this->getNumberParameter("Head > HeadTiltLeft");
    m_headTiltRightParam   = this->getNumberParameter("Head > HeadTiltRight");

    assert( m_headOrientationParam && m_headLeftParam && m_headRightParam && m_headUpParam && m_headDownParam && m_headTiltLeftParam && m_headTiltRightParam);

    // init orientation parameter
    m_eyeGazeLeftParam  = this->getNumberParameter("Eyes > EyeGazeLeft");
    m_eyeGazeRightParam = this->getNumberParameter("Eyes > EyeGazeRight");
    m_eyesLeftParam     = this->getNumberParameter("Eyes > EyesLeft");
    m_eyesRightParam    = this->getNumberParameter("Eyes > EyesRight");
    m_eyesUpParam       = this->getNumberParameter("Eyes > EyesUp");
    m_eyesDownParam     = this->getNumberParameter("Eyes > EyesDown");

    assert( m_eyeGazeLeftParam && m_eyeGazeRightParam && m_eyesLeftParam && m_eyesRightParam && m_eyesUpParam && m_eyesDownParam);

    m_eyesMaximumUp    = Ogre::Degree(12.0);
    m_eyesMaximumDown  = Ogre::Degree(10.0);
    m_eyesMaximumLeft  = Ogre::Degree(27.5);
    m_eyesMaximumRight = Ogre::Degree(27.5);

    this->setChangeFunction("Eyes > Maximum Up", SLOT( eyeAngleChanged()));
    this->setChangeFunction("Eyes > Maximum Down", SLOT( eyeAngleChanged()));
    this->setChangeFunction("Eyes > Maximum Left", SLOT( eyeAngleChanged()));
    this->setChangeFunction("Eyes > Maximum Right", SLOT( eyeAngleChanged()));
    eyeAngleChanged();

    this->setChangeFunction("Head > Maximum Head Up", SLOT( headAngleChanged()));
    this->setChangeFunction("Head > Maximum Head Down", SLOT( headAngleChanged()));
    this->setChangeFunction("Head > Maximum Head Left", SLOT( headAngleChanged()));
    this->setChangeFunction("Head > Maximum Head Right", SLOT( headAngleChanged()));
    this->setChangeFunction("Head > Maximum Head Tilt Left", SLOT( headAngleChanged()));
    this->setChangeFunction("Head > Maximum Head Tilt Right", SLOT( headAngleChanged()));

	this->setChangeFunction("Head > Head Yaw Scale", SLOT( headAngleChanged()));
	this->setChangeFunction("Head > Head Roll Scale", SLOT( headAngleChanged()));
	this->setChangeFunction("Head > Head Pitch Scale", SLOT( headAngleChanged()));

    headAngleChanged();

    m_animParams = getParameterGroup("Animations");
    assert( m_animParams );

    m_tcpSocket.setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
    
	m_headRotationOffset = Ogre::Vector3(0,0,0);
}

//!
//! Destructor of the FaceShiftClientNode class.
//!
FaceShiftClientNode::~FaceShiftClientNode ()
{
}


///
/// Private Slots
///

//!
//! Slot which is called when running flag on node is toggled.
//!
void FaceShiftClientNode::toggleRun()
{
    bool run = getBoolValue("Run");

	// check if connection was established
	if( run) 
	{
        establishConnection();
    }
    else 
	{
		closeConnection();
	}

	// get state of connection and set value of run flag
	bool connected = m_tcpSocket.state() == QAbstractSocket::ConnectedState;
	setValue("Run", connected );
}

//!
//! This function tries to establish a connection to faceshift using the given host and port informations
//!
void FaceShiftClientNode::establishConnection()
{
    unsigned int port = getValue("Port").toUInt();
    QString host = getValue("Host").toString();

    if( port > 0 && host != "" )
    {
        m_tcpSocket.connectToHost( host, port);
        Log::debug("Trying to connect to "+host+":"+QString::number(port)+"...", "FaceShiftClientNode::run()");

        if (m_tcpSocket.waitForConnected(5000))
        {
            // connected
            Log::debug("Connection established!", "FaceShiftClientNode::establishConnection");

            //Send headpose state
            this->setHeadPose();

            // connect events
            connect( &m_tcpSocket, SIGNAL(readyRead()), this, SLOT(processInputData()));

            m_stopWatch.start();
        }
        else
        {
            Log::error("Connection timed out after 5s! "+m_tcpSocket.errorString());
        }
    }
}

//!
//! close the current connection
//!
void FaceShiftClientNode::closeConnection()
{
    Log::debug("Closing Connection", "FaceShiftClientNode::threadFinished");
    m_tcpSocket.abort();
    m_trackingStream.clear();
}

//!
//! Slot which is called when new datagrams are availabled on udp socket.
//!
void FaceShiftClientNode::processInputData()
{
    // pipe received binary data from socket to tracking stream
    QByteArray data = m_tcpSocket.readAll();
    m_trackingStream.received( data.size(), data.constData());

    // create message for output log
    //QString msg = QString("%1 bytes of tracking data received!").arg(data.size());
    //Log::debug(msg, "FaceShiftClientNode::processInputData");

    // record frames
    int frames_decoded = 0;
    fs::fsMsgPtr pMsg = m_trackingStream.get_message();
    while ( pMsg)
    {
        frames_decoded++;

        switch ( pMsg->id())
        {
            // Messages containing tracking information
            // These are sent form faceshift to the client application
        case fs::fsMsg::MSG_OUT_TRACKING_STATE:
            processTrackingState( dynamic_cast<fs::fsMsgTrackingState*>( pMsg.get())); 
            break;
        case fs::fsMsg::MSG_OUT_MARKER_NAMES:
        case fs::fsMsg::MSG_OUT_BLENDSHAPE_NAMES:
        case fs::fsMsg::MSG_OUT_RIG:
        default:
            break;
        }

        // get next message and loop
        pMsg = m_trackingStream.get_message();
    };


    int numBlendshapes = m_trackingData.m_coeffs.size();
    assert( numBlendshapes == m_animParams->getParameterList().size());

    for ( int i=0; i< numBlendshapes; i++)
    {
        Parameter* blendshapeParam = dynamic_cast<Parameter *>(m_animParams->getParameterList().at(i));
        assert( blendshapeParam );
        blendshapeParam->setValue( QVariant( m_trackingData.m_coeffs[i]*100.0f), true);
    }

    // head rotation
    UpdateHeadBone();

    // UpdateEyeGaze
    UpdateEyeGaze();



    if( frames_decoded )
    {
        // measure time since the last data received
        int time = m_stopWatch.elapsed();
        int rate = frames_decoded*1000 / (time+1);

        m_updateRates.push_back(rate);
        while( m_updateRates.size() > 5)
            m_updateRates.pop_front();

        rate = 0;
        foreach( int value, m_updateRates)
            rate += value;

        this->getParameter("Update Rate")->setValue(QVariant(rate));
        m_stopWatch.restart();
    }
}

void FaceShiftClientNode::processTrackingState( fs::fsMsgTrackingState *trackingState )
{
    if( !trackingState)
        return;

    //if( trackingState->tracking_data().m_trackingSuccessful )
        m_trackingData = trackingState->tracking_data();

    //Log::debug("Tracking data decoded!", "FaceShiftClientNode::processInputData");
}

void FaceShiftClientNode::UpdateHeadBone()
{
    fs::fsQuaternionf rot = m_trackingData.m_headRotation;
    Ogre::Quaternion q( rot.w, rot.x, rot.y, rot.z);

	Ogre::Radian yaw   = q.getYaw()   *m_headYawScale   + Ogre::Degree( m_headRotationOffset.x);
	Ogre::Radian pitch = q.getPitch() *m_headPitchScale + Ogre::Degree( m_headRotationOffset.z);
	Ogre::Radian roll  = q.getRoll()  *m_headRollScale  + Ogre::Degree( m_headRotationOffset.y);

    float headLeft=0, headRight=0, headUp=0, headDown=0, headTiltLeft=0, headTiltRight=0;

    if( yaw.valueRadians() > 0)
        headLeft = yaw.valueRadians() * 100 / m_HeadMaximumLeft.valueRadians();
    else
        headRight = -yaw.valueRadians() * 100 / m_HeadMaximumRight.valueRadians();

    if( pitch.valueRadians() > 0)
        headDown = pitch.valueRadians() * 100 / m_HeadMaximumDown.valueRadians();
    else
        headUp = -pitch.valueRadians() * 100 / m_HeadMaximumUp.valueRadians();

    if( roll.valueRadians() > 0)
        headTiltRight = roll.valueRadians() * 100 / m_HeadMaximumTiltRight.valueRadians();
    else
        headTiltLeft = -roll.valueRadians() * 100 / m_HeadMaximumTiltLeft.valueRadians();

    m_headRightParam->setValue(QVariant(headRight), true);
    m_headLeftParam->setValue(QVariant(headLeft), true);
    m_headUpParam->setValue(QVariant(headUp), true);
    m_headDownParam->setValue(QVariant(headDown), true);
    m_headTiltLeftParam->setValue(QVariant(headTiltLeft), true);
    m_headTiltRightParam->setValue(QVariant(headTiltRight), true);

    QVariantList values  = QVariantList() << yaw.valueRadians() << roll.valueRadians() << pitch.valueRadians() << 0.0f << 0.0f << 0.0f;
    m_headOrientationParam->setValue(QVariant(values), true);
}

void FaceShiftClientNode::UpdateEyeGaze()
{
    Ogre::Degree eyesPitch = Ogre::Degree( std::max<float>( m_trackingData.m_eyeGazeLeftPitch, m_trackingData.m_eyeGazeRightPitch ));
    Ogre::Degree eyesYaw   = Ogre::Degree( std::max<float>( m_trackingData.m_eyeGazeLeftYaw, m_trackingData.m_eyeGazeRightYaw ));

    QVariantList eyeGazeLeft  = QVariantList() << eyesPitch.valueRadians() << eyesYaw.valueRadians() << 0.0 << 0.0 << 0.0 << 0.0;
    QVariantList eyeGazeRight = QVariantList() << eyesPitch.valueRadians() << eyesYaw.valueRadians() << 0.0 << 0.0 << 0.0 << 0.0;

    float eyesUp=0, eyesDown=0, eyesLeft=0, eyesRight=0;

    if( eyesYaw.valueDegrees() > 0)
        eyesLeft = eyesYaw.valueRadians() * 100 / m_eyesMaximumLeft.valueRadians();
    else
        eyesRight = -eyesYaw.valueRadians() * 100 / m_eyesMaximumRight.valueRadians();

    if( eyesPitch.valueDegrees() > 0)
        eyesDown = eyesPitch.valueRadians() * 100 / m_eyesMaximumDown.valueRadians();
    else
        eyesUp = -eyesPitch.valueRadians() * 100 / m_eyesMaximumUp.valueRadians();

    m_eyeGazeLeftParam->setValue(QVariant( eyeGazeLeft ), true);
    m_eyeGazeRightParam->setValue(QVariant( eyeGazeRight ), true);

    if( eyesLeft > 0 )
        m_eyesLeftParam->setValue(QVariant( eyesLeft ), true);
    else if( m_eyesLeftParam->getValue().toFloat()>0 )
        m_eyesLeftParam->setValue(QVariant( 0 ), true);

    if( eyesRight > 0 )
        m_eyesRightParam->setValue(QVariant( eyesRight ), true);
    else if( m_eyesRightParam->getValue().toFloat()>0 )
        m_eyesRightParam->setValue(QVariant( 0 ), true);

    if( eyesUp > 0 )
        m_eyesUpParam->setValue(QVariant( eyesUp ), true);
    else if( m_eyesUpParam->getValue().toFloat()>0 )
        m_eyesUpParam->setValue(QVariant( 0 ), true);

    if( eyesDown > 0 )
        m_eyesDownParam->setValue(QVariant( eyesDown ), true);
    else if( m_eyesDownParam->getValue().toFloat()>0 )
        m_eyesDownParam->setValue(QVariant( 0 ), true);

};

bool FaceShiftClientNode::sendMessage( fs::fsMsg::MessageType msg )
{
    if( m_tcpSocket.state() == QAbstractSocket::ConnectedState)
    {
        m_tcpSocket.readAll(); // clear all pending data from socket

        std::string message;
        switch( msg )
        {
        case fs::fsMsg::MSG_IN_START_TRACKING:
            m_trackingStream.encode_message( message, fs::fsMsgStartCapturing());
            break;
        case fs::fsMsg::MSG_IN_STOP_TRACKING:
            m_trackingStream.encode_message( message, fs::fsMsgStopCapturing());
            break;
        case fs::fsMsg::MSG_IN_CALIBRATE_NEUTRAL:
            m_trackingStream.encode_message( message, fs::fsMsgCalibrateNeutral());
            break;
        case fs::fsMsg::MSG_IN_SEND_MARKER_NAMES:
            m_trackingStream.encode_message( message, fs::fsMsgSendMarkerNames());
            break;
        case fs::fsMsg::MSG_IN_SEND_BLENDSHAPE_NAMES:
            m_trackingStream.encode_message( message, fs::fsMsgBlendshapeNames());
            break;
        case fs::fsMsg::MSG_IN_SEND_RIG:
            m_trackingStream.encode_message( message, fs::fsMsgSendRig());
            break;
        case fs::fsMsg::MSG_IN_HEADPOSE_RELATIVE:
            m_trackingStream.encode_message( message, fs::fsMsgHeadPoseRelative());
            break;
        case fs::fsMsg::MSG_IN_HEADPOSE_ABSOLUTE:
            m_trackingStream.encode_message( message, fs::fsMsgHeadPoseAbsolute());
            break;
        }

        int result = m_tcpSocket.write( message.c_str(), message.length());
        m_tcpSocket.flush();

        return result > 0;
    }
    return false;
}

void FaceShiftClientNode::calibrateNeutralPose()
{
	if( getBoolValue("Calibrate Neutral Pose"))
		sendMessage( fs::fsMsg::MSG_IN_CALIBRATE_NEUTRAL);
	setValue("Calibrate Neutral Pose", false);
}

void FaceShiftClientNode::setHeadPose()
{
	QString headPose = getEnumerationParameter("Head Orientation")->getCurrentLiteral();
	if( headPose == "Relative" )
		sendMessage(fs::fsMsg::MSG_IN_HEADPOSE_RELATIVE);
	else if( headPose == "Absolute" )
		sendMessage(fs::fsMsg::MSG_IN_HEADPOSE_ABSOLUTE);

}

void FaceShiftClientNode::rotationOffsetChanged()
{
    m_headRotationOffset = this->getVectorValue("Head > Rotation Offset");
}

void FaceShiftClientNode::eyeAngleChanged()
{
    m_eyesMaximumUp   = Ogre::Degree( this->getFloatValue("Eyes > Maximum Up"));
    m_eyesMaximumDown = Ogre::Degree( this->getFloatValue("Eyes > Maximum Down"));
    m_eyesMaximumLeft = Ogre::Degree( this->getFloatValue("Eyes > Maximum Left"));
    m_eyesMaximumRight= Ogre::Degree( this->getFloatValue("Eyes > Maximum Right"));
}

void FaceShiftClientNode::headAngleChanged()
{
    m_HeadMaximumUp   = Ogre::Degree( this->getFloatValue("Head > Maximum Up"));
    m_HeadMaximumDown = Ogre::Degree( this->getFloatValue("Head > Maximum Down"));
    m_HeadMaximumLeft = Ogre::Degree( this->getFloatValue("Head > Maximum Left"));
    m_HeadMaximumRight= Ogre::Degree( this->getFloatValue("Head > Maximum Right"));
    m_HeadMaximumTiltLeft = Ogre::Degree( this->getFloatValue("Head > Maximum Tilt Left"));
    m_HeadMaximumTiltRight= Ogre::Degree( this->getFloatValue("Head > Maximum Tilt Right"));
	m_headYawScale   = this->getFloatValue("Head > Head Yaw Scale");
	m_headRollScale  = this->getFloatValue("Head > Head Roll Scale");
	m_headPitchScale = this->getFloatValue("Head > Head Pitch Scale");
}

} // namespace FaceShiftClientNode 
