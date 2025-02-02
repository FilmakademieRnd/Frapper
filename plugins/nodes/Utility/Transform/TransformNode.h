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
//! \file "TransformNode.h"
//! \brief Header file for TransformNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef TRANSFORMNODE_H
#define TRANSFORMNODE_H

#include "ViewNode.h"
#include "OgreContainer.h"
#include "InstanceCounterMacros.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace TransformNode {
using namespace Frapper;

//!
//! Class representing nodes that transform input data in 3D space.
//!
class TransformNode : public ViewNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

protected: // static constants

    //!
    //! The name of the input geometry parameter.
    //!
    static const QString InputGeometryParameterName;

    //!
    //! The name of the input lights parameter.
    //!
    static const QString InputLightsParameterName;

    //!
    //! The name of the input cameras parameter.
    //!
    static const QString InputCamerasParameterName;

    //!
    //! The name of the output geometry parameter.
    //!
    static const QString OutputGeometryParameterName;

    //!
    //! The name of the output lights parameter.
    //!
    static const QString OutputLightsParameterName;

    //!
    //! The name of the output cameras parameter.
    //!
    static const QString OutputCamerasParameterName;

public: // constructors and destructors

    //!
    //! Constructor of the TransformNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TransformNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the TransformNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TransformNode ();

public: // functions

    //!
    //! Returns the scene node that contains scene objects created or modified
    //! by this node.
    //!
    //! \return The scene node containing objects created or modified by this node.
    //!
    virtual Ogre::SceneNode * getSceneNode ();

    //!
    //! Returns the radius of the bounding sphere surrounding this object.
    //!
    //! \return The radius of the bounding sphere surrounding this object.
    //!
    virtual double getBoundingSphereRadius ();

    //!
    //! Evaluates the given new connection for the node.
    //!
    //! \param connection The new connection to evaluate.
    //!
    virtual void evaluateConnection ( Connection *connection );

public slots: //

    //!
    //! Processes the input geometry to generate the node's output geometry.
    //!
    void processOutputGeometry ();

    //!
    //! Processes the input lights to generate the node's output lights.
    //!
    void processOutputLights ();

    //!
    //! Processes the input cameras to generate the node's output cameras.
    //!
    void processOutputCameras ();

    ////! Applies the changed transform values to the scene nodes.
    ////!
    //void applyTransform ();

    //!
    //! Applies the changed translation values to the scene nodes.
    //!
    void applyTranslation ();

    //!
    //! Applies the changed rotation values to the scene nodes.
    //!
    void applyRotation ();

    //!
    //! Applies the changed scale values to the scene nodes.
    //!
    void applyScale ();
    
    //!
    //! Resets the transformation parameters to their default values.
    //!
    void resetTransformation ();

private: // variables

    //!
    //! The scene manager handling all OGRE objects.
    //!
    Ogre::SceneManager *m_sceneManager;

    //!
    //! The scene node to which input entities will be attached.
    //!
    Ogre::SceneNode *m_geometrySceneNode;

    //!
    //! The container for the scene node.
    //!
    OgreContainer *m_geometryContainer;

    //!
    //! The scene node to which input lights will be attached.
    //!
    Ogre::SceneNode *m_lightsSceneNode;

    //!
    //! The container for the lights.
    //!
    OgreContainer *m_lightsContainer;

    //!
    //! The scene node to which the input camera will be attached.
    //!
    Ogre::SceneNode *m_cameraSceneNode;

    //!
    //! The container for the camera.
    //!
    OgreContainer *m_cameraContainer;

};

} // namespace TransformNode 

#endif
