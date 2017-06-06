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
//! \file "FiveViewCamera.h"
//! \brief Header file for FiveViewCamera class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       07.02.2012 (created)
//! \date       07.02.2012 (last updated)
//!

#ifndef FIVEVIEWCAMERA_H
#define FIVEVIEWCAMERA_H

#include "OgreTools.h"
#include "ViewNode.h"
// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace FiveViewCameraNode {
using namespace Frapper;

//!
//! Class for five view cameras
//!
class FiveViewCamera : public ViewNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the FiveViewCamera class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    FiveViewCamera ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the FiveViewCamera class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~FiveViewCamera ();

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

    void ProcessCamera1();
    void ProcessCamera2();
    void ProcessCamera3();
    void ProcessCamera4();
    void ProcessCamera5();

private: // functions

    void DeleteCameraSceneNode( Ogre::SceneNode* cameraSceneNode );

protected: // data

    //!
    //! The cameras
    //!
    Ogre::Camera *m_camera1, *m_camera2, *m_camera3, *m_camera4, *m_camera5;

    //!
    //! The cameras' scene nodes.
    //!
    Ogre::SceneNode *m_cameraSceneNode1, *m_cameraSceneNode2, *m_cameraSceneNode3, *m_cameraSceneNode4, *m_cameraSceneNode5;

    //!
    //! The output camera parameter for left and right camera
    //!
    Parameter *m_outputCamera1, *m_outputCamera2, *m_outputCamera3, *m_outputCamera4, *m_outputCamera5;

    OgreContainer *m_cameraContainer1, *m_cameraContainer2, *m_cameraContainer3, *m_cameraContainer4, *m_cameraContainer5;

    Ogre::SceneManager* m_sceneManager;
    Ogre::SceneNode* m_sceneNode;
    OgreContainer* m_ogreContainer;
};

} // namespace FiveViewCameraNode
#endif
