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
//! \file "MocapMeshNode.h"
//! \brief Header file for MocapMeshNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       22.03.2012 (last updated)
//!

#ifndef MOCAPMESHNODE_H
#define MOCAPMESHNODE_H 

#include "GeometryAnimationNode.h"

using namespace Frapper;

//!
//! Class representing nodes that can contain OGRE entities with animation.
//!
class MocapMeshNode : public GeometryAnimationNode
{
    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the MocapMeshNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    MocapMeshNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the MocapMeshNode class.
    //!
    ~MocapMeshNode ();

protected: // functions


    //!
    //! Adds the given time value to the animation state with the given name in
    //! order to progess the animation (OGRE-specific).
    //!
    //! \param aUnitName The name of the animation state to progress.
    //! \param timeToAdd The time to add to the animation state with the given name.
    //!
    void progressAnimation ( const QString &animationName, float time, float weight = 1.0 );

	////!
	////! Initialize a bunch of bones.
	////!
	////! \param boneNames The list of names of bones to initialize.
	////!
	virtual void initializeBones ( QStringList boneNames );

	//virtual void transformBone( const QString &name, const Ogre::Vector3& position, const Ogre::Quaternion &orientation );

public slots:
	void setBoneOrientation();

private slots:

	//!
	//! Called when bone blending options changes
	//!
	void boneBlendingChanged();

	void boneSetInitialOrientation( Ogre::Bone * bone, Ogre::Vector3 &leftInitialOrientation );

	void freeHandsChanged();

protected: // data

	//!
    //! The switch for enabling bone blending between tracking data and animations
    //!
    bool m_useBoneBlending;

    //!
    //! The number of frames to blend between bone position and animation
    //!
    float m_boneBlendingRange;

	float m_boneBlendingDeadZone;

    QStringList m_boneBlendingSkipAnimations;

	QStringMatcher handBoneMatcher;

	Ogre::Quaternion m_lefthandInitial;
	Ogre::Quaternion m_righthandInitial;

	Ogre::Quaternion m_leftHandCurrentInitial;
	Ogre::Quaternion m_rightHandCurrentInitial;

	Ogre::Bone* m_leftHandBone;
	Ogre::Bone* m_rightHandBone;
};

#endif
