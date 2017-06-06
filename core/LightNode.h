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
//! \file "LightNode.h"
//! \brief Header file for LightNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       17.02.2015 (last updated)
//!

#ifndef LIGHTNODE_H
#define LIGHTNODE_H

#include "ViewNode.h"
#include "OgreContainer.h"
#include "InstanceCounterMacros.h"


using namespace Frapper;

//!
//! Class representing nodes for lights in a 3D scene.
//!
class FRAPPER_CORE_EXPORT LightNode : public ViewNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the LightNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    LightNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the LightNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~LightNode ();

public: // functions

    //!
    //! Returns the scene node that contains the light which
    //! visually represents this node.
    //!
    //! \return Scene node containing the entity which visually represents this light.
    //!
    virtual Ogre::SceneNode * getSceneNode ();

protected: 

	//!
	//! Switches all default viewport lights on or off,
	//!
	//! \param value true for on, false for off.
	//!
	void switchHeadlights(const bool value);

	//!
	//! Create and register a new light.
	//!
	//! \param name The name of the light to be created.
	//!
	Ogre::Light *LightNode::createLight(const QString &name);

	//!
	//! Destroys all lights of the node.
	//!
	void destroyAllLights();


private slots: //

    //!
    //! Applies the currently set position for the node to the OGRE scene
    //! objects contained in this node.
    //!
    void applyPosition ();

    //!
    //! Applies the currently set orientation for the node to the OGRE scene
    //! objects contained in this node.
    //!
    void applyOrientation ();

protected: // data

    //!
    //! The scene manager.
    //!
    Ogre::SceneManager *m_sceneManager;

    //!
    //! The scene node the light is attached to.
    //!
    Ogre::SceneNode *m_sceneNode;

    //!
    //! The ogre scene node container.
    //!
    OgreContainer *m_ogreContainer;

    //!
    //! The list of ogre light node containers.
    //!
    QList<OgreContainer *> m_ogreLightContainers;

    //!
    //! The list of lights that this node represents.
    //!
    QList<Ogre::Light *> m_lightList;

    //!
    //! The entity that visually represents this light.
    //!
    Ogre::Entity *m_entity;

    //!
    //! The name of the output light parameter.
    //!
    QString m_outputLightName;
};

#endif
