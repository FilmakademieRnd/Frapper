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
//! \file "MocapMesh.h"
//! \brief Implementation file for MocapMesh class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       22.03.2012 (last updated)
//!

#include "MocapMeshNode.h"

using namespace Frapper;

#define USE_ANIMATION_WEIGHTS

///
/// Constructors and Destructors
///


//!
//! Constructor of the MocapMeshNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
MocapMeshNode::MocapMeshNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryAnimationNode(name, parameterRoot ),
	m_useBoneBlending(true),
    m_boneBlendingRange(0),
	m_lefthandInitial(Ogre::Quaternion::IDENTITY),
	m_righthandInitial(Ogre::Quaternion::IDENTITY),
	m_leftHandCurrentInitial(Ogre::Quaternion::IDENTITY),
	m_rightHandCurrentInitial(Ogre::Quaternion::IDENTITY),
	m_leftHandBone(0),
	m_rightHandBone(0)
{
	// Bone blending
    setChangeFunction("Use bone blending", SLOT(boneBlendingChanged()));
    setChangeFunction("Bone blending range", SLOT(boneBlendingChanged()));
	setChangeFunction("Bone blending dead zone", SLOT(boneBlendingChanged()));
    setChangeFunction("Bone blending skip animations", SLOT(boneBlendingChanged()));
    
	setCommandFunction("Init Bones", SLOT(setBoneOrientation()));
	setChangeFunction("HandLeft Orientation", SLOT(setBoneOrientation()));
	setChangeFunction("HandRight Orientation", SLOT(setBoneOrientation()));

	//setChangeFunction("Free Hands", SLOT(freeHandsChanged()));

    boneBlendingChanged();

	handBoneMatcher.setPattern( "Hand" );
	handBoneMatcher.setCaseSensitivity( Qt::CaseSensitive );
}


//!
//! Destructor of the MocapMeshNode class.
//!
MocapMeshNode::~MocapMeshNode ()
{
}


///
/// Public Slots
///

//!
//! Adds the given time value to the animation state with the given name in
//! order to progess the animation (OGRE-specific).
//!
//! \param aUnitName The name of the animation state to progress.
//! \param timeToAdd The time to add to the animation state with the given name.
//!
void MocapMeshNode::progressAnimation( const QString &animationName, float time, float weight /*= 1.0 */ )
{
    Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
	
	const Ogre::String &animStateName = animationName.toStdString();
	
	if (!m_entity->hasAnimationState(animStateName))
		return;
	
	Ogre::AnimationState *animState = m_entity->getAnimationState(animStateName);
    const float mult = getDoubleValue("mult");

    if (animState) {
        float animLength = animState->getLength();
        Ogre::Real pos = mult * time * animLength;

        QList<Ogre::Bone*> bonesToFade;
        QList<Ogre::Vector3> bonePositions;
        QList<Ogre::Quaternion> boneOrientations;

        // manually controlled bones might exist - reset them
        if ( pos > 0 && skeletonInstance->hasAnimation( animationName.toStdString())) 
        {
            Ogre::Animation *animation = skeletonInstance->getAnimation(animationName.toStdString());
            Ogre::Animation::NodeTrackIterator trackIter = animation->getNodeTrackIterator();
            while (trackIter.hasMoreElements()) {
                Ogre::NodeAnimationTrack* track = trackIter.getNext();
                Ogre::Bone *bone = skeletonInstance->getBone(track->getHandle());
                if (bone->isManuallyControlled())
                {
                    if( m_useBoneBlending )
                    {
                        // Store current bone position and orientation for fading
                        bonesToFade.append(bone);
                        bonePositions.append(bone->getPosition());
                        boneOrientations.append(bone->getOrientation());
                    }

                    bone->reset();
					m_entityContainer->updateCopies( QString::fromStdString(bone->getName()), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                }
            }
        }

        // update all copies created from the entity through the entity container
        if (m_entityContainer) {
            m_entityContainer->updateAnimationState(animationName, pos, weight);
        }

        if( m_useBoneBlending && !m_boneBlendingSkipAnimations.contains(animationName)) 
        {
            if( time < m_boneBlendingRange )
            {
				if( time < m_boneBlendingDeadZone )
				{
					for( int i=0; i<bonesToFade.size(); i++)
					{
						Ogre::Bone* bone = bonesToFade[i];

						bone->setPosition( bonePositions[i] );
						bone->setOrientation( boneOrientations[i] );

						m_entityContainer->updateCopies( QString::fromStdString(bone->getName()), bonePositions[i], boneOrientations[i] );
					}
				}

                const float alpha = time / m_boneBlendingRange;
                for( size_t i=0; i<bonesToFade.size(); i++)
                {
                    Ogre::Bone* bone = bonesToFade[i];
                    const Ogre::Vector3 position = alpha * bone->getPosition() + (1-alpha) * bonePositions[i];
                    const Ogre::Quaternion orientation = Ogre::Quaternion::nlerp( alpha, boneOrientations[i], bone->getOrientation(), true);

                    bone->setPosition( position );
                    bone->setOrientation( orientation );

                    m_entityContainer->updateCopies( QString::fromStdString(bone->getName()), position, orientation );
                }
            } 
            else if( (1-time) < m_boneBlendingRange)
            {
				const float alpha = (1-time) / m_boneBlendingRange;
                for( size_t i=0; i<bonesToFade.size(); i++)
                {
                    Ogre::Bone* bone = bonesToFade[i];
                    const Ogre::Vector3 position = alpha * bone->getPosition() + (1-alpha) * bonePositions[i];
                    const Ogre::Quaternion orientation = Ogre::Quaternion::nlerp( alpha, boneOrientations[i], bone->getOrientation(), true);

                    bone->setPosition( position );
                    bone->setOrientation( orientation );

                    m_entityContainer->updateCopies( QString::fromStdString(bone->getName()), position, orientation );
                }
            }
        }
    }
}

//!
//! Initialize a bunch of bones.
//!
//! \param boneNames The list of names of bones to initialize.
//!
void MocapMeshNode::initializeBones ( QStringList boneNames )
{

	//GeometryAnimationNode::initializeBones(boneNames);

	if( getBoolValue("Free Hands"))
	{

		Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();


		if (skeletonInstance->hasBone( "HandLeft" )) 
		{
			Ogre::Bone *bone = skeletonInstance->getBone( "HandLeft" );
			bone->setManuallyControlled(true);
			bone->setInheritOrientation(false);
			bone->reset();
			m_lefthandInitial = bone->getOrientation();
			m_leftHandCurrentInitial = bone->getOrientation();
			m_leftHandBone = bone;
		}

		if (skeletonInstance->hasBone( "HandRight" )) 
		{
			Ogre::Bone *bone = skeletonInstance->getBone( "HandRight" );
			bone->setManuallyControlled(true);
			bone->setInheritOrientation(false);
			bone->reset();
			m_righthandInitial = bone->getOrientation();
			m_rightHandCurrentInitial = bone->getOrientation();
			m_rightHandBone = bone;
		}
	}
}

void MocapMeshNode::boneBlendingChanged()
{
    m_useBoneBlending = getBoolValue("Use bone blending");
    m_boneBlendingRange = getFloatValue("Bone blending range");
	m_boneBlendingDeadZone = getFloatValue("Bone blending dead zone");
	QString skipAnimations = getStringValue("Bone blending skip animations");
    m_boneBlendingSkipAnimations = skipAnimations.split(",");
}

//void MocapMeshNode::transformBone( const QString &name, const Ogre::Vector3& position, const Ogre::Quaternion &orientation )
//{
//	if (m_entity->hasSkeleton()) 
//	{
//		const std::string &stdName = name.toStdString();
//		Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
//		if (skeletonInstance->hasBone(stdName)) 
//		{
//			Ogre::Bone *bone = skeletonInstance->getBone(stdName);
//
//			bone->setManuallyControlled(true);
//
//			bone->reset();
//			bone->translate( position);
//			bone->rotate( orientation, Ogre::Node::TS_WORLD );
//
//			Ogre::Quaternion local_orientation = bone->convertWorldToLocalOrientation( orientation );
//
//			m_entityContainer->updateCopies(name, position, local_orientation);
//		}
//	}
//}

void MocapMeshNode::boneSetInitialOrientation( Ogre::Bone * bone, Ogre::Vector3 &leftInitialOrientation )
{
	bone->setManuallyControlled(true);
	bone->setInheritOrientation(false);

	bone->reset();

	Ogre::Degree dx( leftInitialOrientation.x );
	Ogre::Degree dy( leftInitialOrientation.y );
	Ogre::Degree dz( leftInitialOrientation.z );

	bone->yaw(dy);
	bone->pitch(dx);
	bone->roll(dz);

	bone->setInitialState();
}

void MocapMeshNode::setBoneOrientation()
{
	if( getBoolValue("Free Hands") && m_entity && m_entity->hasSkeleton())
	{
		Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();

		if (skeletonInstance->hasBone( "HandLeft" )) 
		{
			Ogre::Bone *bone = skeletonInstance->getBone( "HandLeft" );

			QVariant value = getValue( "HandLeft Orientation" );
			if (value.canConvert<Ogre::Vector3>())
			{
				Ogre::Vector3 rotation = value.value<Ogre::Vector3>();
				Ogre::Degree dx( rotation.x );
				Ogre::Degree dy( rotation.y );
				Ogre::Degree dz( rotation.z );

				bone->setOrientation(m_lefthandInitial);

				bone->yaw(dy);
				bone->pitch(dx);
				bone->roll(dz);

				bone->setInitialState();
				m_leftHandCurrentInitial = bone->getInitialOrientation();
			}
		}

		if (skeletonInstance->hasBone( "HandRight" )) 
		{
			Ogre::Bone *bone = skeletonInstance->getBone( "HandRight" );

			QVariant value = getValue( "HandRight Orientation" );
			if (value.canConvert<Ogre::Vector3>())
			{
				Ogre::Vector3 rotation = value.value<Ogre::Vector3>();
				Ogre::Degree dx( rotation.x );
				Ogre::Degree dy( rotation.y );
				Ogre::Degree dz( rotation.z );

				bone->setOrientation(m_righthandInitial);

				bone->yaw(dy);
				bone->pitch(dx);
				bone->roll(dz);

				bone->setInitialState();
				m_rightHandCurrentInitial = bone->getInitialOrientation();
			}
		}
	}
}

void MocapMeshNode::freeHandsChanged()
{

}
