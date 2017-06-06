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
//! \file "AvateeringClientNode.h"
//! \brief Header file for AvateeringClientNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.06.2012 (last updated)
//!

#ifndef AVATEERINGCLIENTNODE_H
#define AVATEERINGCLIENTNODE_H

#include <QtCore/QDataStream>
#include <QtNetwork/QTcpSocket>
#include <QTime>
#include <QDataStream> // file I/O

#include "ViewNode.h"
#include "ParameterGroup.h"

namespace AvateeringClientNode {
using namespace Frapper;

// Summary:
//     This contains all of the possible joint types.
enum JointType
{
	// Summary:
	//     The center of the hip.
	HipCenter = 0,
	//
	// Summary:
	//     The bottom of the spine.
	Spine = 1,
	//
	// Summary:
	//     The center of the shoulders.
	ShoulderCenter = 2,
	//
	// Summary:
	//     The players head.
	Head = 3,
	//
	// Summary:
	//     The left shoulder.
	ShoulderLeft = 4,
	//
	// Summary:
	//     The left elbow.
	ElbowLeft = 5,
	//
	// Summary:
	//     The left wrist.
	WristLeft = 6,
	//
	// Summary:
	//     The left hand.
	HandLeft = 7,
	//
	// Summary:
	//     The right shoulder.
	ShoulderRight = 8,
	//
	// Summary:
	//     The right elbow.
	ElbowRight = 9,
	//
	// Summary:
	//     The right wrist.
	WristRight = 10,
	//
	// Summary:
	//     The right hand.
	HandRight = 11,
	//
	// Summary:
	//     The left hip.
	HipLeft = 12,
	//
	// Summary:
	//     The left knee.
	KneeLeft = 13,
	//
	// Summary:
	//     The left ankle.
	AnkleLeft = 14,
	//
	// Summary:
	//     The left foot.
	FootLeft = 15,
	//
	// Summary:
	//     The right hip.
	HipRight = 16,
	//
	// Summary:
	//     The right knee.
	KneeRight = 17,
	//
	// Summary:
	//     The right ankle.
	AnkleRight = 18,
	//
	// Summary:
	//     The right foot.
	FootRight = 19,

	//
	// Summary:
	//     The total number of joints in this list
	NumberOfJoints = 20,
};

// Enumeration of possible mappings of coordinate spaces
enum CoordinateMapping 
{
	pXpYpZ, nXpYpZ, pXnYpZ, nXnYpZ, pXpYnZ, nXpYnZ, pXnYnZ, nXnYnZ,
	pXpZpY, nXpZpY, pXnZpY, nXnZpY, pXpZnY, nXpZnY, pXnZnY, nXnZnY,
	pYpXpZ, nYpXpZ, pYnXpZ, nYnXpZ, pYpXnZ, nYpXnZ, pYnXnZ, nYnXnZ,
	pYpZpX, nYpZpX, pYnZpX, nYnZpX, pYpZnX, nYpZnX, pYnZnX, nYnZnX,
	pZpXpY, nZpXpY, pZnXpY, nZnXpY, pZpXnY, nZpXnY, pZnXnY, nZnXnY,
	pZpYpX, nZpYpX, pZnYpX, nZnYpX, pZpYnX, nZpYnX, pZnYnX, nZnYnX,
	NumberCoordinateMappings
};

//!
//! This struct defines the data needed to retarget a bone
//!
struct BoneRetarget 
{
	//! The joint type
	JointType joint;
	//! The mapping of the coordinate system
	CoordinateMapping mapping;
	//! Additional rotations around X, Y, Z axis (after mapping) in degree
	Ogre::Degree rx, ry, rz;
};

// define read&write operators for file I/O
QDataStream &operator<<(QDataStream &out, const BoneRetarget &data);
QDataStream &operator>>(QDataStream &in,  BoneRetarget &data);

//!
//! This struct is used to store the data that is streamed from 
//! the Avateering application
//!
struct AvateeringBoneData
{
	//! The type of joint
	JointType joint;
	//! The joint orientation (hierarchical)
	float qw, qx, qy, qz;
	//! The type of joint
	float px, py, pz;
};


//!
//! Input node for Avateering tracking data via tcp
//!
class AvateeringClientNode : public ViewNode
{
	
    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the AvateeringClientNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AvateeringClientNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AvateeringClientNode class.
    //!
    ~AvateeringClientNode ();

	//!
	//! Returns the scene node that contains debug drawings
	//!
	//! \return The scene node containing objects created or modified by this node.
	//!
	virtual Ogre::SceneNode * getSceneNode ();

private slots: //

    //!
    //! Slot which is called when running flag on node is toggled.
    //!
    void toggleRun();

	//! Try to establish a connection using the given host and port informations
	void establishConnection();

	//! Close the current connection
	void closeConnection();

    //! Slot which is called when running flag on node is toggled.
    void processInputData();

	//! Slot which is called when the current bone in setup tab changes
	void BoneValueChanged();

	//! Slot which is called when the coordinate mapping of the current bone changes
	void CoordinateMappingValueChanged();

	//! Slot which is called when the rotation of the current bone changes
	void RotationValueChanged();

	//! Read retarget data from file
	void readRetargetSetup();

	//! Write retarget data to file
	void writeRetargetSetup();

private: // functions

	void setBoneParameter( const int& bone, const Ogre::Quaternion& rot, const Ogre::Vector3& pos );
	void updateBoneParameter();

	void initAvateeringData();
	void initBoneRetarget();

private: // data

    //!
    //! The udp socket.
    //!
    QTcpSocket m_tcpSocket;

   
    //!
    //! The parameter group of animation parameters
    //!
    ParameterGroup* m_boneParams;

    //!
    //! Stopwatch to measure update rate 
    //!
    QTime m_stopWatch;

    //!
    //! The last rates
    //!
    QList<int> m_updateRates;

	//!
	//! The scene node for the skeleton drawing
	//! 
	Ogre::SceneNode *m_sceneNode;

	//!
	//! The scene node for the skeleton drawing
	//! 
	int m_updateDraw;

	//!
	//! The bone data as streamed from the Avateering Application
	//!
	QList<AvateeringBoneData> m_boneData;

	//!
	//! The retarget data for the bone
	//!
	QList<BoneRetarget> m_boneRetarget;
	
	//!
	//! The currently selected bone for the setup
	//!
	int m_selectedBone;
};

} // namespace AvateeringClientNode 

#endif
