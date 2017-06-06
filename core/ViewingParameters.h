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
//! \file "ViewingParameters.h"
//! \brief Header file for SceneModel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       03.05.2009 (last updated)
//!

#ifndef VIEWINGPARAMETERS_H
#define VIEWINGPARAMETERS_H

#include "Node.h"
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace Frapper {

//!
//! Class representing a collection of parameters for viewing a 3D scene.
//!
class FRAPPER_CORE_EXPORT ViewingParameters
{

public: // constructors and destructors

    //!
    //! Constructor of the ViewingParameters class.
    //!
    ViewingParameters ();

    //!
    //! Destructor of the ViewingParameters class.
    //!
    ~ViewingParameters ();

public: // functions

    //!
    //! Resets the viewing parameter's to the default view.
    //!
    void reset ();

    //!
    //! Translates the camera by the given translation values.
    //!
    //! \param dx The translation to apply in X.
    //! \param dy The translation to apply in Y.
    //!
    void translate ( const float dx, const float dy );

    //!
    //! Modifies the camera's viewing angles by the given angle values.
    //!
    //! \param dPhi The angle by which to rotate the viewing phi angle.
    //! \param dTheta The angle by which to rotate the viewing theta angle.
    //!
    void rotate ( const float dPhi, const float dTheta );

    //!
    //! Dollies the camera by the given delta value.
    //!
    //! \param delta The delta value to apply to the viewing radius.
    //! \param wheel Flag to control whether to dolly by mouse wheel.
    //!
    void dolly ( const float delta,  bool wheel = false );

    //!
    //! Updates the viewing parameters according to the currently set viewing
    //! angles.
    //!
    void update ();

    //!
    //! Applies the current viewing parameters to the given viewing parameters.
    //!
    //! \param viewingParameters The target instance to apply the viewing parameters to.
    //!
    void applyTo ( ViewingParameters *viewingParameters );

    //!
    //! Applies the current viewing parameters to the parameters of the given
    //! node.
    //!
    //! \param node The node to apply the viewing parameters to.
    //!
    void applyTo ( Node *node );

    //!
    //! Applies the current viewing parameters to the given scene node.
    //!
    //! \param sceneNode The scene node to apply the viewing parameters to.
    //! \param lookAtOrigin Flag to control whether the scene node should be oriented towards the coordinate system's origin.
    //! \param center Flag to control whether to place the scene node at the coordinate system's origin.
    //!
    void applyTo ( Ogre::SceneNode *sceneNode, bool lookAtOrigin = false, bool center = false );

    //!
    //! Applies the current viewing parameters to the given camera.
    //!
    //! \param camera The camera to apply the viewing parameters to.
    //!
    void applyTo ( Ogre::Camera *camera );

    //!
    //! Applies the current viewing parameters from the given node to the this
    //! viewing parameters.
    //!
    //! \param node The node to apply the viewing parameters from.
    //!
    void applyFrom ( Node *node );

    //!
    //! Returns the width of the canvas stored in the viewing parameters.
    //!
    //! \return The width of the canvas stored in the viewing parameters.
    //!
    unsigned int getCanvasWidth () const;

    //!
    //! Returns the height of the canvas stored in the viewing parameters.
    //!
    //! \return The height of the canvas stored in the viewing parameters.
    //!
    unsigned int getCanvasHeight () const;

    //!
    //! Sets the size of the canvas to store in the viewing parameters.
    //!
    //! \param width The width of the canvas to store in the viewing parameters.
    //! \param height The height of the canvas to store in the viewing parameters.
    //!
    void setCanvasSize ( const unsigned int width, const unsigned int height );

private: // data

	//!
    //! The width of the canvas stored in the viewing parameters.
    //!
    unsigned int m_canvasWidth;

    //!
    //! The height of the canvas stored in the viewing parameters.
    //!
    unsigned int m_canvasHeight;

    //!
    //! Spherical coordinate Rho used to calculate the eye point of the camera.
    //!
    Ogre::Real m_viewRadius;

    //!
    //! The camera's near clipping plane value.
    //!
    Ogre::Real m_nearClipping;

    //!
    //! The camera's far clipping plane value.
    //!
    Ogre::Real m_farClipping;

    //!
    //! Spherical coordinate Phi used to calculate the eye point of the camera.
    //!
    Ogre::Degree m_viewAnglePhi;

    //!
    //! Spherical coordinate Theta used to calculate the eye point of the camera.
    //!
    Ogre::Degree m_viewAngleTheta;
	
    //!
    //! The camera's field of view setting.
    //!
    Ogre::Radian m_fieldOfView;

    //!
    //! The camera's position in 3D space.
    //!
    Ogre::Vector3 m_eye;

    //!
    //! The position to look at in 3D space.
    //!
    Ogre::Vector3 m_center;

    //!
    //! The viewing up vector in 3D space.
    //!
    Ogre::Vector3 m_up;
};

} // end namespace Frapper

#endif
