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
//! \file "SceneNodeParameter.cpp"
//! \brief Implementation file for SceneNodeParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "SceneNodeParameter.h"
#include "Node.h"
#include "OgreManager.h"

namespace Frapper {

///
/// Public Static Data
///


//!
//! The default value for all scene node parameters.
//!
template <Parameter::Type T>
const QVariant SceneNodeParameter<T>::DefaultValue = QVariant::fromValue<Ogre::SceneNode *>(0);


///
/// Constructors and Destructors
///


//!
//! Constructor of the SceneNodeParameter class.
//!
//! \param name The name of the parameter.
//!
template <Parameter::Type T>
SceneNodeParameter<T>::SceneNodeParameter ( const QString &name ) :
    Parameter(name, T, QVariant::fromValue<Ogre::SceneNode *>(0))
{
    //createSceneNode();
}


//!
//! Copy constructor of the SceneNodeParameter class.
//!
//! \param parameter The parameter to copy.
//!
template <Parameter::Type T>
SceneNodeParameter<T>::SceneNodeParameter ( const SceneNodeParameter<T> &parameter ) :
    Parameter(parameter)
{
    m_defaultValue = QVariant(DefaultValue);
    //createSceneNode();
}


//!
//! Destructor of the SceneNodeParameter class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
template <Parameter::Type T>
SceneNodeParameter<T>::~SceneNodeParameter ()
{
    //destroySceneNode();
}


///
/// Public Functions
///


//!
//! Creates an exact copy of the parameter.
//!
//! \return An exact copy of the parameter.
//!
template <Parameter::Type T>
AbstractParameter * SceneNodeParameter<T>::clone ()
{
    return new SceneNodeParameter<T>(*this);
}


//!
//! Returns the OGRE scene node contained in this parameter.
//!
//! \return The OGRE scene node contained in this parameter.
//!
template <Parameter::Type T>
Ogre::SceneNode * SceneNodeParameter<T>::getSceneNode()
{
    Ogre::SceneNode *sceneNode = m_value.value<Ogre::SceneNode *>();
    return sceneNode;
}


///
/// Private Functions
///


//!
//! Creates the scene node that is contained in the scene node parameter.
//!
template <Parameter::Type T>
void SceneNodeParameter<T>::createSceneNode ()
{
    Ogre::SceneNode *sceneNode = OgreManager::createSceneNode(m_name);
    if (sceneNode)
        m_value = QVariant::fromValue<Ogre::SceneNode *>(sceneNode);
    else
        Log::error(QString("A scene node for parameter \"%1\" could not be created.").arg(m_name), "SceneNodeParameter<T>::createSceneNode");
}


//!
//! Destroys the scene node that is contained in the scene node parameter.
//!
template <Parameter::Type T>
void SceneNodeParameter<T>::destroySceneNode ()
{
    Ogre::SceneNode *sceneNode = m_value.value<Ogre::SceneNode *>();
    if (!sceneNode)
        return;

    Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    if (sceneManager) {
        sceneManager->destroySceneNode(sceneNode);
        m_value = QVariant::fromValue<Ogre::SceneNode *>(0);
    } else
        Log::error("The OGRE scene manager could not be obtained.", "SceneNodeParameter<T>::destroySceneNode");
}


//
// Protected Functions
//


///
/// Template Class Function Definitions
///


template const QVariant GeometryParameter::DefaultValue;
template GeometryParameter::SceneNodeParameter ( const QString &name );
template GeometryParameter::SceneNodeParameter ( const GeometryParameter &parameter );
template GeometryParameter::~SceneNodeParameter ();
template AbstractParameter * GeometryParameter::clone ();
template Ogre::SceneNode * GeometryParameter::getSceneNode ();
template void GeometryParameter::createSceneNode ();
template void GeometryParameter::destroySceneNode ();

template const QVariant LightParameter::DefaultValue;
template LightParameter::SceneNodeParameter ( const QString &name );
template LightParameter::SceneNodeParameter ( const LightParameter &parameter );
template LightParameter::~SceneNodeParameter ();
template AbstractParameter * LightParameter::clone ();
template Ogre::SceneNode * LightParameter::getSceneNode ();
template void LightParameter::createSceneNode ();
template void LightParameter::destroySceneNode ();

template const QVariant CameraParameter::DefaultValue;
template CameraParameter::SceneNodeParameter ( const QString &name );
template CameraParameter::SceneNodeParameter ( const CameraParameter &parameter );
template CameraParameter::~SceneNodeParameter ();
template AbstractParameter * CameraParameter::clone ();
template Ogre::SceneNode * CameraParameter::getSceneNode ();
template void CameraParameter::createSceneNode ();
template void CameraParameter::destroySceneNode ();

} // end namespace Frapper