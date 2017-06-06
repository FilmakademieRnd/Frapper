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
//! \file "StereoCamera.h"
//! \brief Header file for StereoCamera class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       07.02.2012 (created)
//! \date       07.02.2012 (last updated)
//!

#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H

#include "OgreTools.h"
#include "ViewNode.h"
// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace StereoCameraNode {
using namespace Frapper;

//!
//! Class for a stereo renderer.
//!
class StereoCamera : public ViewNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the StereoCamera class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    StereoCamera ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the StereoCamera class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~StereoCamera ();

public:

    //!
    //! Returns the scene node that contains the cameras which
    //! visually represents this node.
    //!
    //! \return Scene node containing the entity which visually represents this camera.
    //!
    virtual Ogre::SceneNode * getSceneNode ();

public slots: //

    //!
    //!
    //!
    void AdjustCameras();

    void ProcessLeftCamera();
    void ProcessRightCamera();

private: // functions

    void DeleteCameraSceneNode( Ogre::SceneNode* cameraSceneNode );

protected: // data

    //!
    //! The cameras
    //!
    Ogre::Camera *m_cameraL, *m_cameraR;

    //!
    //! The cameras' scene nodes.
    //!
    Ogre::SceneNode *m_cameraSceneNodeL, *m_cameraSceneNodeR;

    //!
    //! The output camera parameter for left and right camera
    //!
    Parameter *m_outputCameraL, *m_outputCameraR;

    OgreContainer *m_cameraContainerL, *m_cameraContainerR;

    Ogre::SceneManager* m_sceneManager;
    Ogre::SceneNode* m_sceneNode;
    OgreContainer* m_ogreContainer;
};
} // namespace StereoCameraNode

#endif