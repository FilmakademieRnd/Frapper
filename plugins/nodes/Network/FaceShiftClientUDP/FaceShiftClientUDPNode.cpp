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
//! \file "FaceShiftClientUDPNode.cpp"
//! \brief Implementation file for FaceShiftClientUDPNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.06.2012 (last updated)
//!

#include "FaceShiftClientUDPNode.h"
#include "NumberParameter.h"

namespace FaceShiftClientUDPNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(FaceShiftClientUDPNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the FaceShiftClientUDPNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
FaceShiftClientUDPNode::FaceShiftClientUDPNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_animParams(NULL)
{   
    setChangeFunction("Run", SLOT(toggleRun()));

    m_animParams = getParameterGroup("Animations");
    assert( m_animParams );

    m_socket.setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));
    
}

//!
//! Destructor of the FaceShiftClientUDPNode class.
//!
FaceShiftClientUDPNode::~FaceShiftClientUDPNode ()
{
    quit();
    wait();
}

///
/// Public Slots
///

//!
//! Function which is called to establish a connection
//! Executed by QThread::start()
//!
void FaceShiftClientUDPNode::run()
{
    unsigned int port = getValue("Port").toUInt();
    QString host = getValue("Host").toString();

    if( port > 0 && host != "" )
    {
        if( m_socket.bind( port ))
        {
            Log::debug("Listening on port "+QString::number(port)+"...", "FaceShiftClientUDPNode::run()");

            // connect events
            connect(&m_socket, SIGNAL(readyRead()), this, SLOT(processInputData()), Qt::DirectConnection);
            connect(this, SIGNAL(finished()), this, SLOT(threadFinished()), Qt::DirectConnection);

            m_stopWatch.start();

            // enter execution loop
            exec();
        }
    }
}


//!
//! Slot which is called when new datagrams are availabled on udp socket.
//!
void FaceShiftClientUDPNode::processInputData()
{
    m_trackingStream.append( m_socket.readAll());
    //Log::debug("Tracking data received!", "FaceShiftClientUDPNode::processInputData");

    // record frames
    int frames_decoded = 0;
    fs::fsTrackingData td;
    while ( m_trackingStream.decode( td, true))
    {
        frames_decoded++;

        //Log::debug("Tracking data decoded!", "FaceShiftClientUDPNode::processInputData");
        int numBlendshapes = td.m_coeffs.size();
        assert( numBlendshapes == m_animParams->getParameterList().size());

        for ( int i=0; i< numBlendshapes; i++)
        {
            Parameter* blendshapeParam = dynamic_cast<Parameter *>(m_animParams->getParameterList().at(i));
            assert( blendshapeParam );
            blendshapeParam->setValue( QVariant( td.m_coeffs[i]*100.0f), true);
        }

        // head translation
        UpdateHeadTranslation( td.m_headTranslation );

        // head rotation
        UpdateHeadRotation( td.m_headRotation );

        // UpdateEyeGaze
        UpdateEyeGaze( td.m_eyeGazeLeftPitch, td.m_eyeGazeRightPitch, td.m_eyeGazeLeftYaw, td.m_eyeGazeRightYaw );

    };

    if( frames_decoded )
    {
        // measure time since the last data received
        int time = m_stopWatch.elapsed();
        int rate = frames_decoded*1000 / time+1;
        this->getParameter("Update Rate")->setValue(QVariant(rate));
        m_stopWatch.restart();

        Log::debug("Received and decoded "+QString::number(frames_decoded)+ " frames", "FaceShiftClientUDPNode::processInputData");
    }
}

///
/// Private Slots
///


//!
//! Slot which is called when running flag on node is toggled.
//!
void FaceShiftClientUDPNode::toggleRun()
{
    bool run = getBoolValue("Run");
    if (run || !isRunning()) {
        start();   
    }
    else {
        quit();
    }

}

//!
//! Slot which is called when thread has finished.
//!
void FaceShiftClientUDPNode::threadFinished()
{
    Log::debug("Closing Connection", "FaceShiftClientUDPNode::threadFinished");
    m_socket.abort();
    m_trackingStream.clear();
}

void FaceShiftClientUDPNode::UpdateHeadTranslation( Ogre::Vector3 trans )
{
    QVariantList values  = QVariantList() << trans.x << trans.y << trans.z;
    this->getNumberParameter("Head > HeadTranslate")->setValue(QVariant(values), true);

}

void FaceShiftClientUDPNode::UpdateHeadRotation( Ogre::Quaternion rot )
{
    QVariantList values  = QVariantList() << rot.w << rot.x << rot.y << rot.z;
    this->getNumberParameter("Head > HeadRotate")->setValue(QVariant(values), true);

    float radToAnim = 100.0 / Ogre::Math::HALF_PI; // 90∞ -> full displacement
    float yaw = rot.getYaw().valueRadians() * radToAnim;
    float pitch = rot.getPitch().valueRadians() * radToAnim;
    float roll = rot.getRoll().valueRadians() * radToAnim;

    float headLeft=0, headRight=0, headUp=0, headDown=0, headTiltLeft=0, headTiltRight=0;

    if( yaw > 0)
        headLeft = yaw;
    else
        headRight = -yaw;

    if( pitch > 0)
        headDown = pitch;
    else
        headUp = -pitch;

    if( roll > 0)
        headTiltRight = roll;
    else
        headTiltLeft = -roll;

    this->getNumberParameter("Head > HeadTurnRight")->setValue(QVariant(headRight), true);
    this->getNumberParameter("Head > HeadTurnLeft")->setValue(QVariant(headLeft), true);
    this->getNumberParameter("Head > HeadUp")->setValue(QVariant(headUp), true);
    this->getNumberParameter("Head > HeadDown")->setValue(QVariant(headDown), true);
    this->getNumberParameter("Head > HeadTiltLeft")->setValue(QVariant(headTiltLeft), true);
    this->getNumberParameter("Head > HeadTiltRight")->setValue(QVariant(headTiltRight), true);
}

void FaceShiftClientUDPNode::UpdateEyeGaze( float pitchLeft, float pitchRight, float yawLeft, float yawRight )
{
    const float degToRad = Ogre::Math::PI / 180.0f;

    QVariantList eyeGazeLeft  = QVariantList() << degToRad*pitchLeft  << degToRad*yawLeft  << 0.0 << 0.0 << 0.0 << 0.0;
    QVariantList eyeGazeRight = QVariantList() << degToRad*pitchRight << degToRad*yawRight << 0.0 << 0.0 << 0.0 << 0.0;

    this->getNumberParameter("Eyes > EyeGazeLeft")->setValue(QVariant( eyeGazeLeft ), true);
    this->getNumberParameter("Eyes > EyeGazeRight")->setValue(QVariant( eyeGazeRight ), true);
};

} // namespace FaceShiftClientUDPNode 
