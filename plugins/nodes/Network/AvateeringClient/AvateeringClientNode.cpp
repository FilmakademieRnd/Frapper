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
		<< data.rx.valueDegrees() << data.ry.valueDegrees() << data.rz.valueDegrees()
		<< data.flip_x << data.flip_y << data.flip_z;
	return out;
}

QDataStream &operator>>(QDataStream &in,  BoneRetarget &data)
{
	float rx, ry, rz;
	int joint, mapping;
	in >> joint >> mapping >> rx >> ry >> rz >> data.flip_x >> data.flip_y >> data.flip_z;

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
	m_selectedBone(-1),
	m_boneScale(0.0f)
{   
    setChangeFunction("Run", SLOT(toggleRun()));
	setChangeFunction("Bone Retarget > Bone", SLOT( BoneValueChanged()));
	setChangeFunction("Bone Retarget > Coordinate Mapping", SLOT( CoordinateMappingValueChanged()));
	setChangeFunction("Bone Retarget > Rotation", SLOT( RotationValueChanged()));
	setChangeFunction("Bone Retarget > Flip X", SLOT( FlipValueChanged()));
	setChangeFunction("Bone Retarget > Flip Y", SLOT( FlipValueChanged()));
	setChangeFunction("Bone Retarget > Flip Z", SLOT( FlipValueChanged()));
	setChangeFunction("Bone Retarget > Retarget Data Filename", SLOT(RetargetDataFilenameChanged()));
	setChangeFunction("PositionScale", SLOT(updatePositionScale()));

	setCommandFunction("Bone Retarget > Load Retarget Data", SLOT(readRetargetSetup()));
	setCommandFunction("Bone Retarget > Save Retarget Data", SLOT(writeRetargetSetup()));
	setCommandFunction("Bone Retarget > Reset Retarget Data", SLOT(resetBoneRetargetData()));
	setCommandFunction("Reset", SLOT(resetAvateeringData()));

	setProcessingFunction("Set Zero Position", SLOT(updateZeroPosition()));

	initAvateeringData();
	resetBoneRetargetData();

    m_boneParams = getParameterGroup("Bones");
    assert( m_boneParams );

	// Disable nagels algorithm
	m_tcpSocket.setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));

	m_CharacterZeroPosition = Ogre::Vector3(0,0,0);

	m_sceneNode = OgreManager::createSceneNode(name);
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

void AvateeringClientNode::loadReady()
{
	// Try to read former bone retarget data
	m_retargetDataFilename = getStringValue("Bone Retarget > Retarget Data Filename");
	readRetargetSetup();
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
		resetAvateeringData();
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
#ifdef DRAW_DEBUG
	++m_updateDraw %= 2;
	float line_scale = 0, coord_scale = 0;
	if( m_updateDraw)
	{
		line_scale = this->getFloatValue("line_scale");
		coord_scale = this->getFloatValue("coord_scale");
		DebugDrawer::getSingleton().clear();
	}
#endif
	
	// record frames
	int frames_decoded = 1;

	// read complete stream
	QStringList lines;
	while( m_tcpSocket.canReadLine())
	{
		lines.append(m_tcpSocket.readLine());
		//frames_decoded++;
	}

	bool bonesDataReceived[NumberOfJoints];
	for( int i=0; i<NumberOfJoints; i++)
		bonesDataReceived[i] = false;

	while( !lines.empty())
	{
		QString text = lines.last();
		lines.removeLast();

		text = text.replace(",","."); // convert de_DE to en_US
		QStringList values = text.split(" ");

		int bone = -1;
		float qw=0, qx=1, qy=0, qz=0;
		float px=0, py=0, pz=0;
		bool tracked = true;

		if( values.size() >= 5 )
		{
			// read joint indices
			bone = values[0].toInt();

			if( bone>0 && bonesDataReceived[bone] )
				continue;
			else
				bonesDataReceived[bone]=true;

			// read hierarchical rotation of bone
			qw = values[1].toFloat();
			qx = values[2].toFloat();
			qy = values[3].toFloat();
			qz = values[4].toFloat();

			// read position of bone (optional)
			if( values.size() >= 8 )
			{
				// read hierarchical rotation of bone
				px = values[5].toFloat();
				py = values[6].toFloat();
				pz = values[7].toFloat();

				if( values.size() >= 9 )
				{
					tracked = (values[8].contains("True")); // damn u, c#
				}
			}
		}
		if( bone >= 0 && bone < NumberOfJoints)
		{
			AvateeringBoneData& refBoneData = m_boneData[bone];
			refBoneData.joint = (JointType) bone;
			refBoneData.orientation = Ogre::Quaternion( qw, qx, qy, qz);
			refBoneData.position = Ogre::Vector3( px, py, pz);
			refBoneData.tracked = tracked;
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
		if( !bd.tracked )
		{
			continue;
		}

		// joint type of current bone 
		const JointType& joint = bd.joint;

		// bone position, currently only hip center is used
		Ogre::Vector3 position(0, 0, 0);
		if( joint == HipCenter )
			position = (bd.position - m_CharacterZeroPosition)*m_boneScale;

		// kinect bone orientation
		Ogre::Quaternion kr = bd.orientation;

		// -- Bone re-targeting starts here --
		const BoneRetarget& br = m_boneRetarget[joint];

		// add rotations around X-/Y-/Z-Axis as additional DOF
		Ogre::Quaternion rx( br.rx, Ogre::Vector3::UNIT_X);
		Ogre::Quaternion ry( br.ry, Ogre::Vector3::UNIT_Y);
		Ogre::Quaternion rz( br.rz, Ogre::Vector3::UNIT_Z);

		// special bone setup, see AvateeringXNA::SetJointTransformation()
		if ( joint == KneeLeft )
		{
			Ogre::Quaternion hipLeft = m_boneData[HipLeft].orientation;
			Ogre::Quaternion kneeLeft = m_boneData[KneeLeft].orientation;
			kr = kneeLeft * hipLeft;
		} 
		else if ( joint == KneeRight )
		{
			Ogre::Quaternion hipRight = m_boneData[HipRight].orientation;
			Ogre::Quaternion kneeRight = m_boneData[KneeRight].orientation;
			kr = kneeRight * hipRight;
		}
		else if ( joint == Spine )
		{
			Ogre::Quaternion hipCenter = m_boneData[HipCenter].orientation;
			Ogre::Quaternion spine = m_boneData[Spine].orientation;
			Ogre::Radian angle = hipCenter.getPitch() - spine.getPitch();
			
			if (angle.valueDegrees() > 0) {
				angle = spine.getPitch();
				const Ogre::Radian correction = angle*angle*0.3 + angle*0.8f;
				const Ogre::Quaternion correctionQuat = kr*Ogre::Quaternion(-correction, Ogre::Vector3::UNIT_X);
				kr = correctionQuat;
			}
		}
		else if ( joint == ShoulderCenter )
		{
			Ogre::Quaternion spine = m_boneData[Spine].orientation;
			Ogre::Quaternion shoulderCenter = m_boneData[ShoulderCenter].orientation;
			Ogre::Radian angle = spine.getPitch() - shoulderCenter.getPitch();
			
			if (angle.valueDegrees() < 0) {
				const Ogre::Quaternion correctionQuat = Ogre::Quaternion(Ogre::Radian(0.0f), Ogre::Vector3::UNIT_X);
				kr = correctionQuat;
			}
		}

		kr = kr*rx*ry*rz;

		// flip axis
		kr.x = br.flip_x ? -kr.x : kr.x;
		kr.y = br.flip_y ? -kr.y : kr.y;
		kr.z = br.flip_z ? -kr.z : kr.z;

		// this will be the final hierachical orientation of the avatar
		Ogre::Quaternion avatarRotation;

		// remapping of coordinate axis
		switch( br.mapping )
		{
		case XYZ:
			avatarRotation = kr; // no mapping required
			break;
		case XZY:
			avatarRotation = Ogre::Quaternion(kr.w, kr.x, kr.z, kr.y);
			break;
		case YXZ:
			avatarRotation = Ogre::Quaternion(kr.w, kr.y, kr.x, kr.z);
			break;
		case YZX:
			avatarRotation = Ogre::Quaternion(kr.w, kr.y, kr.z, kr.x);
			break;
		case ZXY:
			avatarRotation = Ogre::Quaternion(kr.w, kr.z, kr.x, kr.y);
			break;
		case ZYX:
			avatarRotation = Ogre::Quaternion(kr.w, kr.z, kr.y, kr.x);
			break;
		}

		setBoneParameter( joint, avatarRotation, Ogre::Vector3(-position.z, -position.x, /*position.y*/0.0f));
	}
}

void AvateeringClientNode::setBoneParameter( const int& bone, const Ogre::Quaternion& rot, const Ogre::Vector3& pos )
{
	// Quaternions
	//QVariantList values = QVariantList() << rot.w << rot.x << rot.y << rot.z << pos.x << pos.y << pos.z;

	// Euler angles
	Ogre::Matrix3 rotationMatrix;
	rot.ToRotationMatrix(rotationMatrix);

	// convert quaternion to Euler angles here
	Ogre::Radian rx, ry, rz;
	rotationMatrix.ToEulerAnglesXYZ(rx, ry, rz);

	QVariantList values = QVariantList() << rx.valueRadians() << ry.valueRadians() << rz.valueRadians() << pos.x << pos.y << pos.z;

	NumberParameter* boneParameter = static_cast<NumberParameter *>(m_boneParams->getParameterList().at(bone));
	boneParameter->setValue( QVariant(values), true);
}

void AvateeringClientNode::BoneValueChanged()
{
	EnumerationParameter *parameter = dynamic_cast<EnumerationParameter *>(sender());
	if( parameter)
	{
		m_selectedBone = parameter->getCurrentIndex();
		const BoneRetarget& bt = m_boneRetarget[m_selectedBone];
		QVariantList rotation = QVariantList() << bt.rx.valueDegrees() 
											   << bt.ry.valueDegrees() 
											   << bt.rz.valueDegrees();

		setValue("Bone Retarget > Coordinate Mapping", bt.mapping);
		setValue("Bone Retarget > Flip X", bt.flip_x);
		setValue("Bone Retarget > Flip Y", bt.flip_y);
		setValue("Bone Retarget > Flip Z", bt.flip_z);

		setValue("Bone Retarget > Rotation", QVariant(rotation));
	}
}

void AvateeringClientNode::CoordinateMappingValueChanged()
{
	EnumerationParameter *parameter = dynamic_cast<EnumerationParameter *>(sender());
	if( parameter && m_selectedBone >= 0 && m_selectedBone < NumberOfJoints)
	{
		m_boneRetarget[m_selectedBone].mapping = (CoordinateMapping) parameter->getCurrentIndex();
		updateBoneParameter();
	}
}

void AvateeringClientNode::RotationValueChanged()
{
	NumberParameter *parameter = dynamic_cast<NumberParameter *>(sender());
	if( parameter && m_selectedBone >= 0 && m_selectedBone < NumberOfJoints)
	{
		QVariantList values = parameter->getValue().toList();
		m_boneRetarget[m_selectedBone].rx = values[0].toFloat();
		m_boneRetarget[m_selectedBone].ry = values[1].toFloat();
		m_boneRetarget[m_selectedBone].rz = values[2].toFloat();
		updateBoneParameter();
	}
}

void AvateeringClientNode::FlipValueChanged()
{
	if( m_selectedBone >= 0 && m_selectedBone < NumberOfJoints)
	{
		m_boneRetarget[m_selectedBone].flip_x = getBoolValue("Flip X");
		m_boneRetarget[m_selectedBone].flip_y = getBoolValue("Flip Y");
		m_boneRetarget[m_selectedBone].flip_z = getBoolValue("Flip Z");
		updateBoneParameter();
	}
}

void AvateeringClientNode::RetargetDataFilenameChanged()
{
	QString newFilename = getStringValue("Bone Retarget > Retarget Data Filename");
	if( newFilename != "")
	{
		m_retargetDataFilename = newFilename;
	}
	else
	{
		Log::error("Filename is invalid!", "AvateeringClientNode::RetargetDataFilenameChanged");
		setValue("Retarget Data Filename", "retargetSetup.dat");
	}
}

void AvateeringClientNode::initAvateeringData()
{
	// Init list of bone data
	m_boneData.clear();

	for( int i=0; i<NumberOfJoints; i++)
	{
		AvateeringBoneData boneData;
		boneData.joint = (JointType)i;
		boneData.orientation = Ogre::Quaternion( 0.f, 1.f, 0.f, 0.f);
		boneData.position = Ogre::Vector3(0.f, 0.f, 0.f);
		m_boneData.append(boneData);
	}
}

void AvateeringClientNode::resetAvateeringData()
{
	for( int i=0; i<m_boneData.size(); i++)
	{
		AvateeringBoneData& boneData = m_boneData[i];
		boneData.orientation = Ogre::Quaternion::IDENTITY;
		boneData.position = Ogre::Vector3::ZERO;
		setBoneParameter( boneData.joint, boneData.orientation, boneData.position );
	}
}

void AvateeringClientNode::resetBoneRetargetData()
{
	// Init list of bone retarget data
	m_boneRetarget.clear();

	for( int i=0; i<NumberOfJoints; i++)
	{
		BoneRetarget br;
		br.joint = (JointType)i;
		br.mapping = YZX;
		br.rx = Ogre::Degree(0);
		br.ry = Ogre::Degree(0);
		br.rz = Ogre::Degree(0);
		br.flip_x = true;
		br.flip_y = false;
		br.flip_z = true;
		m_boneRetarget.append(br);
	}

	Log::debug("Retargeting setup was reseted.", "AvateeringClientNode::initBoneRetarget");

}
bool AvateeringClientNode::readRetargetSetup()
{
	if( QFile::exists(m_retargetDataFilename))
	{
		m_boneRetarget.clear();

		QFile file( m_retargetDataFilename);
		file.open(QIODevice::ReadOnly);

		QDataStream in(&file);    // read the data serialized from the file

		for( int i=0; i<NumberOfJoints; i++)
		{
			BoneRetarget br;
			in >> br;
			m_boneRetarget.push_back(br);
		}
		Log::debug(QString("Retargeting setup was loaded from \"%1\".").arg(m_retargetDataFilename), "AvateeringClientNode::readRetargetSetup");
		return true;
	}
	else
	{
		Log::error(QString("Could not read data from \"%1\".").arg(m_retargetDataFilename),"AvateeringClientNode::readRetargetSetup");
		return false;
	}
}

void AvateeringClientNode::writeRetargetSetup()
{
	QFile file(m_retargetDataFilename);
	file.open( QIODevice::WriteOnly | QIODevice::Truncate);
	QDataStream out(&file);

	foreach( BoneRetarget br, m_boneRetarget)
		out << br;

	Log::debug(QString("Retargeting setup written to \"%1\".").arg(m_retargetDataFilename), "AvateeringClientNode::writeRetargetSetup");
}

void AvateeringClientNode::updateZeroPosition()
{
	if( getBoolValue("Set Zero Position"))
		m_CharacterZeroPosition = m_boneData[HipCenter].position;
	setValue("Set Zero Position", false);
}

void AvateeringClientNode::updatePositionScale()
{
	NumberParameter *scaleParameter = dynamic_cast<NumberParameter *>(sender());
	if (scaleParameter)
		m_boneScale = scaleParameter->getValue().toFloat();
}

} // namespace AvateeringClientNode 
