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
//! \file "GeometryAnimationNode.cpp"
//! \brief Implementation file for GeometryAnimationNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       26.11.2012 (last updated)
//!

#ifndef ANIMATABLEMESH_H
#define ANIMATABLEMESH_H


#ifndef Q_MOC_RUN
#include "GeometryNode.h"
#include "OgreManager.h" 
#include "OgreTools.h"
#include <QtCore/QFile>
#include <qvector3d.h>
#endif
using namespace Frapper;

#include "OgreTagPoint.h"

//!
//! Class for bone attachment handling
//!
class BoneAttachment
{

private:
	Ogre::Bone * m_bone;
	Ogre::SceneNode * m_sceneNode;

public:
	BoneAttachment(Ogre::Bone * bone, Ogre::SceneNode * sceneNode) :
		m_bone(bone),
		m_sceneNode(sceneNode)
	{
	}

	~BoneAttachment() {}

	Ogre::Bone *  getBone()
	{
		return m_bone;
	}

	Ogre::SceneNode *  getSceneNode()
	{
		return m_sceneNode;
	}
};

//!
//! Implementation of bone listener
//!
class BoneListener : public Ogre::Node::Listener
{
public:
	BoneListener(const Ogre::String &positionParamName, const Ogre::String &rotationParamName, const Ogre::GpuProgramParametersSharedPtr &gpuParameter) :
	  m_positionParamName(positionParamName),
	  m_rotationParamName(rotationParamName),
	  m_gpuParameter(gpuParameter),
	  Ogre::Node::Listener() {}

	virtual void nodeUpdated(const Ogre::Node *node) 
	{
		const Ogre::Quaternion &qOrientation = node->_getDerivedOrientation();
		const Ogre::Vector3 &vPosition = node->_getDerivedPosition();
		m_gpuParameter->setNamedConstant(m_positionParamName, vPosition);
		m_gpuParameter->setNamedConstant(m_rotationParamName, Ogre::Vector3(qOrientation.x, qOrientation.y, qOrientation.z));
	}

private:
	Ogre::GpuProgramParametersSharedPtr m_gpuParameter;
	Ogre::String m_positionParamName;
	Ogre::String m_rotationParamName;
};

//!
//! Base class for all nodes that produce image data.
//!
class FRAPPER_CORE_EXPORT GeometryAnimationNode : public GeometryNode, Ogre::FrameListener
{

    Q_OBJECT

public: // static constants

public: // constructors and destructors

    //!
    //! Constructor of the GeometryAnimationNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.    
    //!
    GeometryAnimationNode ( const QString &name, 
							ParameterGroup *parameterRoot);

    //!
    //! Destructor of the GeometryAnimationNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~GeometryAnimationNode ();

public:

	//!
	//! Ogre frame listener callback.
	//!
	bool frameStarted(const Ogre::FrameEvent & event);

public slots:
	//!
	//! Triggers before redraw.
	//!
	void onPreRedraw();

private slots:
	//!
	//! Sets the visibility of the entity
	//!
	void setEntityVisibility();

protected slots:	// slots

	//!
    //! Processing function for animation input parameters
    //!
    virtual void updateAnimation ();

	//!
	//! Update a specific animation given by parameter
	//! \param parameter The animation parameter that is updated
	//!
	virtual void updateAnimation ( Parameter * parameter );

	//!
	//! Processing function for bone input parameters
	//!
    virtual void updateBone ();

	//!
	//! Processing function for bone attachment input parameters
	//!
	virtual void updateBoneAttachment();

	//!
	//! Update a specific bone given by parameter
	//! \param parameter The bone parameter that is updated
	//!
	virtual void updateBone ( Parameter* parameter );

	//!
	//! Processing function for pose input parameters
	//!
    virtual void updatePose ();

	//!
	//! Update a specific pose given by parameter
	//! \param parameter The pose parameter that is updated
	//!
	virtual void updatePose ( Parameter * parameter );

	//!
	//! Change function for the Geometry File parameter.
	//!
	virtual void geometryFileChanged ();

    //!
    //! Set interpolation mode.
    //!
    void setInterpolationMode ();

	//!
	//! Sets the internal config switches
	//!
	void setSwitches();

	//!
	//! Sets the animation multiplayer.
	//!
	void setAnimationMultiplayer();

protected:	// functions

	//!
    //! Initialize all animation states of this object (OGRE-specific).
    //!
    //! \return True if the animation states where successfully initialized, otherwise False.
    //!
    bool initAnimationStates ();
	
	//!
    //! Retrieve the names of all animation names, types and index numbers
    //!
    //! \return List with names, types and index numbers of animations.
    //!
    QStringList getAnimationNamesAndTypes (Ogre::Entity *entity);

	//!
    //! Returns the names of all bones in the skeleton for the mesh.
    //!
    //! \return The names of all bones in the skeleton for the mesh.
    //!
    QStringList getBoneNames ();

	//!
    //! Initialize a bunch of bones.
    //!
    //! \param boneNames The list of names of bones to initialize.
    //!
    virtual void initializeBones ( QStringList boneNames );

	//!
    //! Adds the given time value to the animation state with the given name in
    //! order to progess the animation (OGRE-specific).
    //!
    //! \param animationName The name of the animation state to progress.
    //! \param timeToAdd     The time to add to the animation state with the given name.
    //!
    virtual void progressAnimation ( const QString &animationName, float timeToAdd, float weight = 1.0 );

	//!
	//! Transforms a given bone using euler angles.
	//! 
	//! \param name The name of the bone.
	//! \param position Position of bone
	//! \param orientation Orientation of bone as rotation around x-/y-/z-axis
	//!
	virtual void transformBone( const QString &name, const Ogre::Vector3 &position, const Ogre::Vector3 &orientation );

	//!
	//! Transforms a given bone using a quaternion.
	//! 
	//! \param name The name of the bone.
	//! \param position Position of the bone
	//! \param orientation Orientation of bone as axis/angle rotation
	//!
	virtual void transformBone( const QString &name, const Ogre::Vector3& position, const Ogre::Quaternion &orientation );

	//!
    //! Checks the Ogre entity if it is animatable.
	//! \return True if the entity is animated
    //!
	bool isAnimated () const;

	//!
    //! Cleans all empty (identity) tracks from the given ogre skeleton
    //!
    void CleanEmptyTracks( Ogre::Skeleton * skeleton );

	//!
	//! Detaches all listeners from all bones.
	//!
	void DetachBoneListeners ();

	//!
	//! Update eyeRotation for ambient occlusion (Gaze)
	//! 
	void updateEyeRotation();

	//!
	//! Calculate bone occlusion for MarkerJoint_LUL9 & MarkerJoint_RUL9
	//! 
	void calculateBoneOcclusion();

	//!
	//! Decode a QVariantList of values into a rotation and a translation
	//! Supports Euler angles as well as Quaternions as input:
	//!   Euler:      [ rx, ry, rz, tx, ty, tz]
	//!   Quaternion: [ rx, ry, rz, rw, tx, ty, tz]
	//!
	void getOrientationAndTranslation( const QVariantList &values, Ogre::Vector3 &sumRotations, Ogre::Vector3 &sumTranslations );

private:
	//!
	//! Create bone parameters.
	//! 
	void createBoneParams();

	//!
	//! Create bone attachment parameters.
	//! 
	void createBoneAttachmentParams();

	//!
	//! Delete bone attachment scene nodes.
	//! 
	void deleteBoneAttachmentSceneNodes();

	//!
	//! Iterate through attachment map and update attachments.
	//! 
	void updateAllBoneAttachments();

	//!
	//! To make movable objects not visible, even with their children
	//!
	void makeObjectVisible(Ogre::SceneNode *sceneNode, int visible);

protected:	//data

	//!
    //! The switch for the auto pose triggering
    //!
	bool m_autoPose;

	//!
    //! The multiplayer for animation scaling
    //!
	float m_animationMultiplayer;

	//!
    //! Parameter group for Animations
    //!
    ParameterGroup *m_animationGroup;
	ParameterGroup *m_animationWeightGroup;

    //!
    //! Parameter group for skeletal animations
    //!
    ParameterGroup *m_skeletalGroup;
    ParameterGroup *m_skeletalWeightGroup;

    //!
    //! Parameter group for pose animations
    //!
    ParameterGroup *m_poseGroup;
    ParameterGroup *m_poseWeightGroup;

	//!
    //! Parameter group for morph animations
    //!
    ParameterGroup *m_morphGroup;
    ParameterGroup *m_morphWeightGroup;

    //!
    //! Parameter group for bone groups
    //!
    ParameterGroup *m_boneGroup;

	//!
	//! Parameter group bone attachment group
	//!
	ParameterGroup *m_boneAttachmentGroup;

    //!
    //! Animation progresses.
    //!
    QMap<QString, double> m_animProgresses;

    //!
    //! List of animation names.
    //!
    QStringList m_animationNames;

    //!
    //! List of animation names.
    //!
    QStringList m_weightNames;

    //!
    //! List of pose names.
    //!
    QStringList m_poseNames;

	//!
	//! Bone attachment map.
	//!
	QMap<QString, BoneAttachment *> m_boneAttachmentMap;

};

#endif
