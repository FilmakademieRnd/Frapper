/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "CameraNode.h"
//! \brief Header file for CameraNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef CAMERANODE_H
#define CAMERANODE_H

#include "ViewNode.h"
#include "OgreContainer.h"
#include "InstanceCounterMacros.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace CameraNode {
using namespace Frapper;

//!
//! Class representing nodes for cameras in a 3D scene.
//!
class CameraNode : public ViewNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the CameraNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    CameraNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the CameraNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CameraNode ();

public: // functions

    //!
    //! Returns the scene node that contains the camera which
    //! visually represents this node.
    //!
    //! \return Scene node containing the entity which visually represents this camera.
    //!
    virtual Ogre::SceneNode * getSceneNode ();

private: // functions

    //!
    //! Checks if the currently set render width and height corresponds to a
    //! render resolution preset, and if so, selects that preset.
    //!
    void checkForPreset ();

	//!
	//! Updates output parameters based on the frustum
	//!
	//! It should be called when a value of the frustum has changed
	//!
	void updateDependentOutputParameters ();

	//!
	//! Called when scene load is ready to prevent wrong recource setup in Ogre
	//!
	void loadReady ();

	//!
	//! For debugging!
	//!
	void writeOutCameraData();

private slots: //

    //!
    //! Applies the currently set transformation for the node to all copys in
	//! the current Ogre context.
    //!
    void updateTransform ();

	//!
    //! Applies the intrinsic and extrinsic parameters of the camera to all.
	//! copys in the current Ogre context.
    //! on frame change.
    //!
    void updateFrustum ();

    //!
    //! Updates the render resolution parameters according to the selected
    //! preset.
    //!
    //! Is called when the value of the render resolution preset parameter has
    //! changed.
    //!
    void renderResolutionPresetChanged ();

    //!
    //! Applies the changed render width to the output camera.
    //!
    //! Is called when the value of the render width parameter has changed.
    //!
    void renderWidthChanged ();

    //!
    //! Applies the changed render height to the output camera.
    //!
    //! Is called when the value of the render height parameter has changed.
    //!
    void renderHeightChanged ();

    //!
    //! Applies the changed focal length to the output camera.
    //!
    //! Is called when the value of the focal length parameter has changed.
    //!
    void focalLengthChanged ();

    //!
    //! Applies the changed field of view to the output camera.
    //!
    //! Is called when the value of the field of view parameter has changed.
    //!
    void fieldOfViewChanged ();

    //!
    //! Applies the changed near clipping plane to the output camera.
    //!
    //! Is called when the value of the near clipping plane parameter has
    //! changed.
    //!
    void nearClippingPlaneChanged ();

    //!
    //! Applies the changed far clipping plane to the output camera.
    //!
    //! Is called when the value of the far clipping plane parameter has
    //! changed.
    //!
    void farClippingPlaneChanged ();

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

    //!
    //! Applies the currently set quaternion orientation for the node to the OGRE scene
    //! objects contained in this node.
    //!
    void applyQOrientation ();

	//!
    //! Deletes the second orientation input parameter.
    //!
    void onConnect ();


protected: // data

    //!
    //! The image space projection matix
    //!
	QVariantList m_ptMatList;

	//!
    //! The world view projection matix
    //!
	QVariantList m_wMatList;

private: // data

    //!
    //! The scene manager.
    //!
    Ogre::SceneManager *m_sceneManager;

    //!
    //! The scene node the camera is attached to.
    //!
    Ogre::SceneNode *m_sceneNode;

    //!
    //! The ogre scene node container.
    //!
    OgreContainer *m_ogreContainer;

	//!
    //! The ogre camera node container.
    //!
	OgreContainer *m_ogreCameraContainer;

    //!
    //! The camera that this node represents.
    //!
    Ogre::Camera *m_camera;

    //!
    //! The entity that visually represents this camera.
    //!
    Ogre::Entity *m_entity;

    //!
    //! The name of the output camera parameter.
    //!
    QString m_outputCameraName;

};
} // namespace CameraNode

#endif
