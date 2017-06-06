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
//! \file "ViewingParameters.cpp"
//! \brief Implementation file for ViewingParameters class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       04.05.2009 (last updated)
//!

#include "ViewingParameters.h"
#include "OgreTools.h"
#include "Log.h"

#define MIN_VIEW_RADIUS 0.01

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the ViewingParameters class.
//!
ViewingParameters::ViewingParameters () :
    m_canvasWidth(1280),
    m_canvasHeight(720)
{
    reset();
    update();
}


//!
//! Destructor of the ViewingParameters class.
//!
ViewingParameters::~ViewingParameters ()
{
}


///
/// Public Functions
///


//!
//! Resets the viewing parameter's to the default view.
//!
void ViewingParameters::reset ()
{
    m_viewRadius = 25.0;
    m_viewAnglePhi = 60.0;  // elevation (90° means the camera is on ground level)
    m_viewAngleTheta = 45.0;  // rotation around Y axis

    m_center.x = 0.0;
    m_center.y = 0.0;
	m_center.z = 0.0;

    m_fieldOfView = Ogre::Radian(Ogre::Degree(35.0));
    m_nearClipping = 0.01f;
    m_farClipping = 1000.0f;
}


//!
//! Translates the camera by the given translation values.
//!
//! \param dx The translation to apply in X.
//! \param dy The translation to apply in Y.
//!
void ViewingParameters::translate ( float dx, float dy )
{
    Ogre::Vector3 direction = m_eye - m_center;
    direction.normalise();

    Ogre::Vector3 r = direction.crossProduct(m_up);

    Ogre::Vector3 moveVector;
    moveVector.x = m_viewRadius * (dx * r.x + dy * m_up.x);
    moveVector.y = m_viewRadius * (dx * r.y + dy * m_up.y);
    moveVector.z = m_viewRadius * (dx * r.z + dy * m_up.z);

    m_eye += moveVector;
    m_center += moveVector;
}


//!
//! Modifies the camera's viewing angles by the given angle values.
//!
//! \param phi The angle by which to rotate the viewing phi angle.
//! \param theta The angle by which to rotate the viewing theta angle.
//!
void ViewingParameters::rotate ( float phi, float theta )
{
    m_viewAngleTheta += Ogre::Degree(phi);
    m_viewAnglePhi -= Ogre::Degree(theta);

    // clamp Theta to -180..+180
    if (m_viewAngleTheta.valueDegrees() >= 180)
        m_viewAngleTheta -= Ogre::Degree(360);
    else if (m_viewAngleTheta.valueDegrees() <= -180)
        m_viewAngleTheta += Ogre::Degree(360);

    // clamp Phi to -180..+180
    if (m_viewAnglePhi.valueDegrees() >= 180)
        m_viewAnglePhi -= Ogre::Degree(360);
    else if (m_viewAnglePhi.valueDegrees() <= -180)
        m_viewAnglePhi += Ogre::Degree(360);
}


//!
//! Dollies the camera by the given delta value.
//!
//! \param delta The delta value to apply to the viewing radius.
//! \param wheel Flag to control whether to dolly by mouse wheel.
//!
void ViewingParameters::dolly ( float delta, bool wheel /* = false */ )
{
    if (wheel)
        m_viewRadius -= 10.0 * m_viewRadius / delta;
    else
        m_viewRadius += 1.7 * m_viewRadius * delta;

    if (m_viewRadius < MIN_VIEW_RADIUS)
        m_viewRadius = MIN_VIEW_RADIUS;
}


//!
//! Updates the viewing parameters according to the currently set viewing
//! angles.
//!
void ViewingParameters::update ()
{
    Ogre::Real phi = m_viewAnglePhi.valueRadians();
    Ogre::Real theta = m_viewAngleTheta.valueRadians();

    // calculate eye position using polar coordinates
    Ogre::Vector3 direction;
    direction.x = sin(phi) * cos(theta);
    direction.y = cos(phi);
    direction.z = sin(phi) * sin(theta);
    direction.normalise();
    m_eye = m_center + m_viewRadius * direction;

    // calculate up vector
    Ogre::Vector3 worldUp (0, 1, 0);
    if (phi < 0)
        worldUp.y = -worldUp.y;
    Ogre::Vector3 cross = worldUp.crossProduct(direction);
    cross.normalise();
    m_up = direction.crossProduct(cross);
    m_up.normalise();
}


//!
//! Applies the current viewing parameters to the given viewing parameters.
//!
//! \param viewingParameters The target instance to apply the viewing parameters to.
//!
void ViewingParameters::applyTo ( ViewingParameters *viewingParameters )
{
    viewingParameters->m_viewRadius = m_viewRadius;
    viewingParameters->m_viewAnglePhi = m_viewAnglePhi;
    viewingParameters->m_viewAngleTheta = m_viewAngleTheta;
    viewingParameters->m_center = m_center;

    viewingParameters->m_eye = m_eye;
    viewingParameters->m_up = m_up;

    viewingParameters->m_fieldOfView = m_fieldOfView;
    viewingParameters->m_nearClipping = m_nearClipping;
    viewingParameters->m_farClipping = m_farClipping;

    viewingParameters->m_canvasWidth = m_canvasWidth;
    viewingParameters->m_canvasHeight = m_canvasHeight;
}


//!
//! Applies the current viewing parameters to the parameters of the given
//! node.
//!
//! \param node The node to apply the viewing parameters to.
//!
void ViewingParameters::applyTo ( Node *node )
{
    if (!node) {
        Log::error("No node given to apply the viewing parameters to.", "ViewingParameters::applyTo");
        return;
    }

    // use a temporarily created scene node for calculating the node's orientation
    Ogre::SceneNode *dummySceneNode = new Ogre::SceneNode(0);
    dummySceneNode->setPosition(m_eye);
    dummySceneNode->setFixedYawAxis(true);
    dummySceneNode->lookAt(m_center, Ogre::Node::TS_WORLD);
    Ogre::Quaternion orientationQuaternion = dummySceneNode->getOrientation();
    Ogre::Radian yaw = orientationQuaternion.getYaw(false);
    Ogre::Radian roll = orientationQuaternion.getRoll(false);
    Ogre::Radian pitch = orientationQuaternion.getPitch(false);
    delete dummySceneNode;

    Ogre::Vector3 position = m_eye;
    Ogre::Vector3 orientation = Ogre::Vector3(pitch.valueDegrees(), yaw.valueDegrees(), roll.valueDegrees());

	Parameter* positionParameter = node->getParameter("Position");
	Parameter* orientationParamter = node->getParameter("Orientation");

    node->setValue("Orbit > Radius", m_viewRadius, true);
    node->setValue("Orbit > Azimuth angle", m_viewAnglePhi.valueDegrees(), true);
    node->setValue("Orbit > Zenith angle", m_viewAngleTheta.valueDegrees(), true);
	node->setValue("Orbit > Target Position", m_center, true);

	positionParameter->setValue( QVariant::fromValue<Ogre::Vector3>(position), true);
	orientationParamter->setValue( QVariant::fromValue<Ogre::Vector3>(orientation), true);

	orientationParamter->executeCommand();
}


//!
//! Applies the current viewing parameters to the given scene node.
//!
//! \param sceneNode The scene node to apply the viewing parameters to.
//! \param lookAtOrigin Flag to control whether the scene node should be oriented towards the coordinate system's origin.
//! \param center Flag to control whether to place the scene node at the coordinate system's origin.
//!
void ViewingParameters::applyTo ( Ogre::SceneNode *sceneNode, bool lookAtOrigin /* = false */, bool center /* = false */ )
{
    if (!sceneNode) {
        Log::error("No scene node given to apply the viewing parameters to.", "ViewingParameters::applyTo");
        return;
    }

    if (!lookAtOrigin) {
        if (!center)
            sceneNode->setPosition(m_eye);
        else {
            sceneNode->setPosition(m_center);
            Ogre::Real centerSize = m_viewRadius / 40;
            sceneNode->setScale(centerSize, centerSize, centerSize);
        }
    } else
        sceneNode->setPosition(4.0 * (m_eye - m_center).normalisedCopy());

    sceneNode->setFixedYawAxis(true, m_up);

    if (!lookAtOrigin)
        sceneNode->lookAt(m_center, Ogre::Node::TS_WORLD);
    else
        sceneNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TS_WORLD);

}


//!
//! Applies the current viewing parameters to the given camera.
//!
//! \param camera The camera to apply the viewing parameters to.
//!
void ViewingParameters::applyTo ( Ogre::Camera *camera )
{
    if (!camera) {
        Log::error("No camera given to apply the viewing parameters to.", "ViewingParameters::applyTo");
        return;
    }

    // apply the current canvas size to the given camera
    OgreContainer *cameraContainer = Ogre::any_cast<OgreContainer *>(((Ogre::MovableObject *) camera)->getUserAny());
    if (cameraContainer) {
		cameraContainer->setCameraWidth(m_canvasWidth);
		cameraContainer->setCameraHeight(m_canvasHeight);
        camera->setAspectRatio((float)(m_canvasWidth) / (float)m_canvasHeight);
    }

    camera->setFOVy(m_fieldOfView);
    camera->setNearClipDistance(m_nearClipping);
    camera->setFarClipDistance(m_farClipping);
}


//!
//! Applies the current viewing parameters from the given node to the this
//! viewing parameters.
//!
//! \param node The node to apply the viewing parameters from.
//!
void ViewingParameters::applyFrom ( Node *node )
{
    if (!node) {
        Log::error("No node given to apply the viewing parameters from.", "ViewingParameters::applyFrom");
        return;
    }

    m_viewRadius = node->getFloatValue("Orbit > Radius");
    m_viewAnglePhi = Ogre::Degree(node->getFloatValue("Orbit > Azimuth angle"));
    m_viewAngleTheta = Ogre::Degree(node->getFloatValue("Orbit > Zenith angle"));
	
	m_center = node->getVectorValue("Orbit > Target Position");
    update();
}


//!
//! Returns the width of the canvas stored in the viewing parameters.
//!
//! \return The width of the canvas stored in the viewing parameters.
//!
unsigned int ViewingParameters::getCanvasWidth () const
{
    return m_canvasWidth;
}


//!
//! Returns the height of the canvas stored in the viewing parameters.
//!
//! \return The height of the canvas stored in the viewing parameters.
//!
unsigned int ViewingParameters::getCanvasHeight () const
{
    return m_canvasHeight;
}


//!
//! Sets the size of the canvas to store in the viewing parameters.
//!
//! \param width The width of the canvas to store in the viewing parameters.
//! \param height The height of the canvas to store in the viewing parameters.
//!
void ViewingParameters::setCanvasSize ( unsigned int width, unsigned int height )
{
    m_canvasWidth = width;
    m_canvasHeight = height;
}

} // end namespace Frapper