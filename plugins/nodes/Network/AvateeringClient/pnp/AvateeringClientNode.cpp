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
//! \file "AvateeringClientNode.cpp"
//! \brief Implementation file for AvateeringClientNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.06.2012 (last updated)
//!

#include "AvateeringClientNode.h"

#include <OgreQuaternion.h>
#include <OgreVector3.h>
#include <OgreManager.h>
#include <NumberParameter.h>

// file I/O
#include <QDataStream>
#include <QFile>

//#define DRAW_DEBUG

#ifdef DRAW_DEBUG
#include "DebugDrawer.h"
#endif
#include "OgreManager.h"

namespace AvateeringClientNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(AvateeringClientNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the AvateeringClientNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
QDataStream &operator<<(QDataStream &out, const BoneRetarget &data)
{
	out << quint32(data.joint) << quint32(data.mapping) 
		<< data.rx.valueDegrees() << data.ry.valueDegrees() << data.rz.valueDegrees();
	return out;
}

QDataStream &operator>>(QDataStream &in,  BoneRetarget &data)
{
	float rx, ry, rz;
	int joint, mapping;
	in >> joint >> mapping >> rx >> ry >> rz;

	data.joint = (JointType)joint;
	data.mapping = (CoordinateMapping)mapping;
	data.rx = Ogre::Degree(rx);
	data.ry = Ogre::Degree(ry);
	data.rz = Ogre::Degree(rz);

	return in;
}

AvateeringClientNode::AvateeringClientNode ( QString name, ParameterGroup *parameterRoot ) :
    ViewNode(name, parameterRoot),
    m_boneParams(NULL),
	m_updateDraw(0),
	m_selectedBone(-1)
{   
    setChangeFunction("Run", SLOT(toggleRun()));
	setChangeFunction("Bone Setup > Bone", SLOT( BoneValueChanged()));
	setChangeFunction("Bone Setup > Coordinate Mapping", SLOT( CoordinateMappingValueChanged()));
	setChangeFunction("Bone Setup > Rotation", SLOT( RotationValueChanged()));

	setCommandFunction("Load Retarget Data", SLOT(readRetargetSetup()));
	setCommandFunction("Save Retarget Data", SLOT(writeRetargetSetup()));

    m_boneParams = getParameterGroup("Bones");
    assert( m_boneParams );

	m_tcpSocket.setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));

	m_sceneNode = OgreManager::createSceneNode(name);

	initAvateeringData();
	initBoneRetarget();

#ifdef DRAW_DEBUG
	new DebugDrawer( m_sceneNode, 1.0f);
#endif
   
}

//!
//! Destructor of the AvateeringClientNode class.
//!
AvateeringClientNode::~AvateeringClientNode ()
{
#ifdef DRAW_DEBUG
	delete DebugDrawer::getSingletonPtr();
#endif
}


///
/// Private Slots
///

//!
//! Slot which is called when running flag on node is toggled.
//!
void AvateeringClientNode::toggleRun()
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
void AvateeringClientNode::establishConnection()
{
    unsigned int port = getValue("Port").toUInt();
    QString host = getValue("Host").toString();

    if( port > 0 && host != "" )
    {
        m_tcpSocket.connectToHost( host, port);
        Log::debug("Trying to connect to "+host+":"+QString::number(port)+"...", "AvateeringClientNode::run()");

        if (m_tcpSocket.waitForConnected(5000))
        {
            // connected
            Log::debug("Connection established!", "AvateeringClientNode::run()");

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
void AvateeringClientNode::closeConnection()
{
    Log::debug("Closing Connection", "AvateeringClientNode::threadFinished");
    m_tcpSocket.abort();
}

//!
//! Slot which is called when new datagrams are availabled on udp socket.
//!
void AvateeringClientNode::processInputData()
{
	++m_updateDraw %= 2;


#ifdef DRAW_DEBUG
	float line_scale = 0, coord_scale = 0;
	if( m_updateDraw)
	{
		line_scale = this->getFloatValue("line_scale");
		coord_scale = this->getFloatValue("coord_scale");
		DebugDrawer::getSingleton().clear();
	}
#endif
	
	// record frames
	int frames_decoded = 0;

	while( m_tcpSocket.canReadLine())
	{
		frames_decoded++;
		QString text = m_tcpSocket.readLine();
		text = text.replace(",",".");
		QStringList values = text.split(" ");

		int bone = 0;
		float qw=0, qx=0, qy=0, qz=0;
		float px=0, py=0, pz=0;

		if( values.size() >= 5 )
		{
			// read joint indices
			bone = values[0].toInt();
			AvateeringBoneData& refBoneData = m_boneData[bone];

			// read hierarchical rotation of bone
			refBoneData.qw = values[1].toFloat();
			refBoneData.qx = values[2].toFloat();
			refBoneData.qy = values[3].toFloat();
			refBoneData.qz = values[4].toFloat();

			// read position of bone (optional)
			if( values.size() >= 8 )
			{
				// read hierarchical rotation of bone
				refBoneData.px = values[5].toFloat();
				refBoneData.py = values[6].toFloat();
				refBoneData.pz = values[7].toFloat();
			}
		}
	}

	updateBoneParameter();

    if( frames_decoded )
    {
        // measure time since the last data received
        int time = m_stopWatch.elapsed();
        int rate = frames_decoded*1000 / ((time+1)*20); // 20 lines per frame

        m_updateRates.push_back(rate);
        while( m_updateRates.size() > 5)
            m_updateRates.pop_front();

        rate = 0;
        foreach( int value, m_updateRates)
            rate += value;
        
        this->getParameter("Update Rate")->setValue(QVariant(rate));
        m_stopWatch.restart();
    }

#ifdef DRAW_DEBUG
	if( m_updateDraw)
	{
		DebugDrawer::getSingleton().build();
	}
#endif
}

Ogre::SceneNode * AvateeringClientNode::getSceneNode()
{
	return m_sceneNode;
}

void AvateeringClientNode::updateBoneParameter()
{
	foreach( AvateeringBoneData bd, m_boneData)
	{
		 JointType joint = bd.joint;
		 const BoneRetarget& br = m_boneRetarget[joint];

		 float qw=bd.qw;
		 float qx=bd.qx;
		 float qy=bd.qy; 
		 float qz=bd.qz;

		 Ogre::Quaternion avatarRotation;
			
		 // coordinate remapping
		 switch( br.mapping )
		 {
		 case pXpYpZ:	avatarRotation = Ogre::Quaternion(+qw, +qx, +qy, +qz);	break;
		 case nXpYpZ:	avatarRotation = Ogre::Quaternion(+qw, -qx, +qy, +qz);	break;
		 case pXnYpZ:	avatarRotation = Ogre::Quaternion(+qw, +qx, -qy, +qz);	break;
		 case nXnYpZ:	avatarRotation = Ogre::Quaternion(+qw, -qx, -qy, +qz);	break;
		 case pXpYnZ:	avatarRotation = Ogre::Quaternion(+qw, +qx, +qy, -qz);	break;
		 case nXpYnZ:	avatarRotation = Ogre::Quaternion(+qw, -qx, +qy, -qz);	break;
		 case pXnYnZ:	avatarRotation = Ogre::Quaternion(+qw, +qx, -qy, -qz);	break;
		 case nXnYnZ:	avatarRotation = Ogre::Quaternion(+qw, -qx, -qy, -qz);	break;
		 case pXpZpY:	avatarRotation = Ogre::Quaternion(+qw, +qx, +qz, +qy);	break;
		 case nXpZpY:	avatarRotation = Ogre::Quaternion(+qw, -qx, +qz, +qy);	break;
		 case pXnZpY:	avatarRotation = Ogre::Quaternion(+qw, +qx, -qz, +qy);	break;
		 case nXnZpY:	avatarRotation = Ogre::Quaternion(+qw, -qx, -qz, +qy);	break;
		 case pXpZnY:	avatarRotation = Ogre::Quaternion(+qw, +qx, +qz, -qy);	break;
		 case nXpZnY:	avatarRotation = Ogre::Quaternion(+qw, -qx, +qz, -qy);	break;
		 case pXnZnY:	avatarRotation = Ogre::Quaternion(+qw, +qx, -qz, -qy);	break;
		 case nXnZnY:	avatarRotation = Ogre::Quaternion(+qw, -qx, -qz, -qy);	break;
		 case pYpXpZ:	avatarRotation = Ogre::Quaternion(+qw, +qy, +qx, +qz);	break;
		 case nYpXpZ:	avatarRotation = Ogre::Quaternion(+qw, -qy, +qx, +qz);	break;
		 case pYnXpZ:	avatarRotation = Ogre::Quaternion(+qw, +qy, -qx, +qz);	break;
		 case nYnXpZ:	avatarRotation = Ogre::Quaternion(+qw, -qy, -qx, +qz);	break;
		 case pYpXnZ:	avatarRotation = Ogre::Quaternion(+qw, +qy, +qx, -qz);	break;
		 case nYpXnZ:	avatarRotation = Ogre::Quaternion(+qw, -qy, +qx, -qz);	break;
		 case pYnXnZ:	avatarRotation = Ogre::Quaternion(+qw, +qy, -qx, -qz);	break;
		 case nYnXnZ:	avatarRotation = Ogre::Quaternion(+qw, -qy, -qx, -qz);	break;
		 case pYpZpX:	avatarRotation = Ogre::Quaternion(+qw, +qy, +qz, +qx);	break;
		 case nYpZpX:	avatarRotation = Ogre::Quaternion(+qw, -qy, +qz, +qx);	break;
		 case pYnZpX:	avatarRotation = Ogre::Quaternion(+qw, +qy, -qz, +qx);	break;
		 case nYnZpX:	avatarRotation = Ogre::Quaternion(+qw, -qy, -qz, +qx);	break;
		 case pYpZnX:	avatarRotation = Ogre::Quaternion(+qw, +qy, +qz, -qx);	break;
		 case nYpZnX:	avatarRotation = Ogre::Quaternion(+qw, -qy, +qz, -qx);	break;
		 case pYnZnX:	avatarRotation = Ogre::Quaternion(+qw, +qy, -qz, -qx);	break;
		 case nYnZnX:	avatarRotation = Ogre::Quaternion(+qw, -qy, -qz, -qx);	break;
		 case pZpXpY:	avatarRotation = Ogre::Quaternion(+qw, +qz, +qy, +qx);	break;
		 case nZpXpY:	avatarRotation = Ogre::Quaternion(+qw, -qz, +qy, +qx);	break;
		 case pZnXpY:	avatarRotation = Ogre::Quaternion(+qw, +qz, -qy, +qx);	break;
		 case nZnXpY:	avatarRotation = Ogre::Quaternion(+qw, -qz, -qy, +qx);	break;
		 case pZpXnY:	avatarRotation = Ogre::Quaternion(+qw, +qz, +qy, -qx);	break;
		 case nZpXnY:	avatarRotation = Ogre::Quaternion(+qw, -qz, +qy, -qx);	break;
		 case pZnXnY:	avatarRotation = Ogre::Quaternion(+qw, +qz, -qy, -qx);	break;
		 case nZnXnY:	avatarRotation = Ogre::Quaternion(+qw, -qz, -qy, -qx);	break;
		 case pZpYpX:	avatarRotation = Ogre::Quaternion(+qw, +qz, +qx, +qy);	break;
		 case nZpYpX:	avatarRotation = Ogre::Quaternion(+qw, -qz, +qx, +qy);	break;
		 case pZnYpX:	avatarRotation = Ogre::Quaternion(+qw, +qz, -qx, +qy);	break;
		 case nZnYpX:	avatarRotation = Ogre::Quaternion(+qw, -qz, -qx, +qy);	break;
		 case pZpYnX:	avatarRotation = Ogre::Quaternion(+qw, +qz, +qx, -qy);	break;
		 case nZpYnX:	avatarRotation = Ogre::Quaternion(+qw, -qz, +qx, -qy);	break;
		 case pZnYnX:	avatarRotation = Ogre::Quaternion(+qw, +qz, -qx, -qy);	break;
		 case nZnYnX:	avatarRotation = Ogre::Quaternion(+qw, -qz, -qx, -qy);	break;
		 }

		 // additional rotations
		 Ogre::Quaternion rx( br.rx, Ogre::Vector3::UNIT_X);
		 Ogre::Quaternion ry( br.ry, Ogre::Vector3::UNIT_Y);
		 Ogre::Quaternion rz( br.rz, Ogre::Vector3::UNIT_Z);

		 avatarRotation = avatarRotation * rx *ry * rz;

		 setBoneParameter( joint, avatarRotation, Ogre::Vector3(0,0,0));
	}
}

void AvateeringClientNode::setBoneParameter( const int& bone, const Ogre::Quaternion& rot, const Ogre::Vector3& pos )
{
	QVariantList values = QVariantList() << rot.w << rot.x << rot.y << rot.z << pos.x << pos.y << pos.z;

	NumberParameter* boneParameter = static_cast<NumberParameter *>(m_boneParams->getParameterList()->at(bone));
	boneParameter->setValue( QVariant(values), true);
}

void AvateeringClientNode::BoneValueChanged()
{
	EnumerationParameter *parameter = dynamic_cast<EnumerationParameter *>(sender());
	if( parameter)
	{
		m_selectedBone = parameter->getCurrentIndex();
		BoneRetarget& bt = m_boneRetarget[m_selectedBone];

		setValue("Coordinate Mapping", bt.mapping);
		
		QVariantList rotation = QVariantList() << bt.rx.valueDegrees() 
											   << bt.ry.valueDegrees() 
											   << bt.rz.valueDegrees();
		setValue("Rotation", QVariant(rotation));
	}
}

void AvateeringClientNode::CoordinateMappingValueChanged()
{
	EnumerationParameter *parameter = dynamic_cast<EnumerationParameter *>(sender());
	if( parameter && m_selectedBone > 0 && m_selectedBone < NumberOfJoints)
	{
		m_boneRetarget[m_selectedBone].mapping = (CoordinateMapping) parameter->getCurrentIndex();
		updateBoneParameter();
	}
}

void AvateeringClientNode::RotationValueChanged()
{
	NumberParameter *parameter = dynamic_cast<NumberParameter *>(sender());
	if( parameter && m_selectedBone > 0 && m_selectedBone < NumberOfJoints)
	{
		QVariantList values = parameter->getValue().toList();
		m_boneRetarget[m_selectedBone].rx = values[0].toFloat();
		m_boneRetarget[m_selectedBone].ry = values[1].toFloat();
		m_boneRetarget[m_selectedBone].rz = values[2].toFloat();
		updateBoneParameter();
	}
}

void AvateeringClientNode::initAvateeringData()
{
	// Init list of bone data
	for( int i=0; i<NumberOfJoints; i++)
	{
		AvateeringBoneData abd;
		abd.joint = (JointType)i;
		abd.px = 0; abd.py = 0; abd.pz = 0; 
		abd.qw = 0; abd.qx = 0; abd.qy = 0; abd.qz = 0;

		m_boneData.append(abd);
	}
}

void AvateeringClientNode::initBoneRetarget()
{
	// Init list of bone retarget data
	for( int i=0; i<NumberOfJoints; i++)
	{
		BoneRetarget br;
		br.joint = (JointType)i;
		br.mapping = pYnZnX;
		br.rx = Ogre::Degree(0);
		br.ry = Ogre::Degree(0);
		br.rz = Ogre::Degree(0);

		m_boneRetarget.append(br);
	}
}
void AvateeringClientNode::readRetargetSetup()
{
	if( QFile::exists("retargetSetup.dat"))
	{

		QFile file("retargetSetup.dat");
		file.open(QIODevice::ReadOnly);

		QDataStream in(&file);    // read the data serialized from the file

		for( int i=0; i<NumberOfJoints; i++)
		{
			BoneRetarget br;
			in >> br;
			m_boneRetarget.push_back(br);
		}
	}
	else
	{
		Log::error("Could not read retarget data.","AvateeringClientNode::readRetargetSetup");
	}
}

void AvateeringClientNode::writeRetargetSetup()
{
	QFile file("retargetSetup.dat");
	file.open( QIODevice::WriteOnly | QIODevice::Truncate);
	QDataStream out(&file);

	foreach( BoneRetarget br, m_boneRetarget)
		out << br;
}

//
//JointType joint = (JointType)bone;
//
//if( joint == HipCenter)
//{
//	avatarRotation = kinectRotation;
//}
//else if( joint == ShoulderCenter )
//{
//	avatarRotation = kinectRotation;
//}
//else if( joint == Spine)
//{
//	Ogre::Quaternion adjust( Ogre::Degree(20), Ogre::Vector3::UNIT_X);
//	kinectRotation = kinectRotation * adjust;
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, kinectRotation.z, kinectRotation.x);
//}
//else if( joint == Head)
//{
//	Ogre::Quaternion adjust( Ogre::Degree(-30), Ogre::Vector3::UNIT_X);
//	kinectRotation = kinectRotation * adjust;
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, kinectRotation.z, kinectRotation.x);
//}
//else if (joint == ElbowLeft)
//{
//	Ogre::Quaternion adjust( Ogre::Degree(-15), Ogre::Vector3::UNIT_Z);
//	kinectRotation = kinectRotation * adjust;
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, -kinectRotation.z, -kinectRotation.x);
//}
//else if (joint == WristLeft)
//{
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, -kinectRotation.z, -kinectRotation.x);
//}
//else if (joint == HandLeft)
//{
//	Ogre::Quaternion adjust( Ogre::Degree(-90), Ogre::Vector3::UNIT_Y);
//	kinectRotation = kinectRotation * adjust;
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, kinectRotation.x, -kinectRotation.z);
//}
//else if (joint == ElbowRight)
//{
//	Ogre::Quaternion adjust( Ogre::Degree(15), Ogre::Vector3::UNIT_Z);
//	kinectRotation = kinectRotation * adjust;
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, -kinectRotation.z, -kinectRotation.x);
//}
//else if (joint == WristRight)
//{
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, -kinectRotation.z, -kinectRotation.x);
//}
//else if (joint == HandRight)
//{
//	Ogre::Quaternion adjust( Ogre::Degree(90), Ogre::Vector3::UNIT_Y);
//	kinectRotation = kinectRotation * adjust;
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, -kinectRotation.x, kinectRotation.z);
//}
//else if( joint == KneeLeft )
//{
//	// need to combine hip and knee..
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, kinectRotation.z, kinectRotation.x);
//}
//else if( joint == AnkleLeft || joint == AnkleRight )
//{
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, kinectRotation.z, kinectRotation.x);
//}
//else if( joint == KneeRight )
//{
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, kinectRotation.z, kinectRotation.x);
//}
//else if( joint == FootLeft || joint == FootRight )
//{
//	Ogre::Quaternion adjust( Ogre::Degree(-45), Ogre::Vector3::UNIT_X);
//	kinectRotation = kinectRotation * adjust;
//	avatarRotation = Ogre::Quaternion( kinectRotation.w, kinectRotation.y, kinectRotation.z, kinectRotation.x);
//
} // namespace AvateeringClientNode 
