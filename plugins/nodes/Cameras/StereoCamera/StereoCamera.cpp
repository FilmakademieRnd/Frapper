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
//! \file "StereoCamera.cpp"
//! \brief Implementation file for StereoCamera class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       07.02.2012 (created)
//! \date       07.02.2012 (last updated)
//!

#include "StereoCamera.h"
#include "OgreManager.h"
#include "Node.h"

namespace StereoCameraNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the StereoCamera class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
StereoCamera::StereoCamera ( const QString &name, ParameterGroup *parameterRoot ) :
    ViewNode( name, parameterRoot),
	m_cameraL(NULL), 
	m_cameraR(NULL),
    m_sceneNode(0),
    m_cameraSceneNodeL(NULL), 
    m_cameraSceneNodeR(NULL),
	m_ogreContainer(0),
	m_cameraContainerL(NULL),
	m_cameraContainerR(NULL)
{

    m_sceneManager = OgreManager::getSceneManager();

    // create OGRE scene node and scene node encapsulation
    m_sceneNode = OgreManager::createSceneNode( m_name );
    if (m_sceneNode) {
        m_ogreContainer = new OgreContainer(m_sceneNode);
        m_sceneNode->setUserAny(Ogre::Any(m_ogreContainer));
        m_sceneNode->setScale(0.3f, 0.3f, -0.4f);
    }

    // create the camera input parameter - multiplicity 1
    CameraParameter *cameraParameter = new CameraParameter("CameraIn");
    cameraParameter->setMultiplicity(1);
    cameraParameter->setPinType(Parameter::PT_Input);
    parameterRoot->addParameter(cameraParameter);

    // create an output camera parameter
    m_outputCameraL = new CameraParameter( "Left Camera");
    m_outputCameraL->setPinType(Parameter::PT_Output);
    m_outputCameraL->setNode(this);
    m_outputCameraL->setProcessingFunction( SLOT(ProcessLeftCamera()));
    m_outputCameraL->setDirty(true);
    m_outputCameraL->addAffectingParameter( cameraParameter);
	m_outputCameraL->addAffectingParameter(getParameter("Focal Length"));
	m_outputCameraL->addAffectingParameter(getParameter("Field of View"));
    parameterRoot->addParameter(m_outputCameraL);

    m_outputCameraR = new CameraParameter( "Right Camera");
    m_outputCameraR->setPinType(Parameter::PT_Output);
    m_outputCameraR->setNode(this);
    m_outputCameraR->setProcessingFunction( SLOT(ProcessRightCamera()));
    m_outputCameraR->setDirty(true);
    m_outputCameraR->addAffectingParameter( cameraParameter);
	m_outputCameraR->addAffectingParameter(getParameter("Focal Length"));
	m_outputCameraR->addAffectingParameter(getParameter("Field of View"));
    parameterRoot->addParameter(m_outputCameraR);

    // Add processing functions for stereo parameter
	setChangeFunction( "Eye Distance", SLOT(AdjustCameras()));
	setChangeFunction( "Reverse Stereo", SLOT(AdjustCameras()));
	setChangeFunction( "Frustum Offset Multiplier", SLOT(AdjustCameras()));
	setChangeFunction( "Focal Length", SLOT(AdjustCameras()));
	setChangeFunction( "Field of View", SLOT(AdjustCameras()));

	this->setSaveable(true);
}


//!
//! Destructor of the StereoCameraNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
StereoCamera::~StereoCamera ()
{
    OgreTools::deepDeleteSceneNode(m_sceneNode, m_sceneManager, true);
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void StereoCamera::ProcessLeftCamera()
{

    OgreTools::deepDeleteSceneNode( m_cameraSceneNodeL, m_sceneManager, true);
    m_cameraSceneNodeL = NULL;
	m_cameraContainerL = NULL;
    m_cameraL = 0;

    // process input camera
    Ogre::SceneNode *inputCamera = getSceneNodeValue("CameraIn");

    if (inputCamera) 
    {
        // get camera from scene node
        Ogre::Camera *camera = OgreTools::getFirstCamera(inputCamera);

        if (camera) 
        {
            try
            {
                OgreTools::deepCopySceneNode( inputCamera, m_cameraSceneNodeL, m_name+"Left", m_sceneManager);
                OgreManager::relocateSceneNode(m_cameraSceneNodeL, m_sceneNode);
            }
            catch( Ogre::Exception& e)
            {
                Log::error(QString::fromStdString( "The camera's scene node could not be copied: " + e.getDescription()), "StereoCameraNode::processCamera");
                return;
            }

            // copy cameras
            m_cameraL = OgreTools::getFirstCamera(m_cameraSceneNodeL);
			m_cameraContainerL = OgreTools::getOgreContainer(m_cameraSceneNodeL);
			m_cameraContainerL->setCamera(m_cameraL);

            m_outputCameraL->setValue(QVariant::fromValue<Ogre::SceneNode*>(m_cameraSceneNodeL));
            m_outputCameraL->setDirty(false);

            AdjustCameras();
        }
    }
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void StereoCamera::ProcessRightCamera()
{

    OgreTools::deepDeleteSceneNode( m_cameraSceneNodeR, m_sceneManager, true);
    m_cameraSceneNodeR = NULL;
    m_cameraR = 0;

    // process input camera
    Ogre::SceneNode *inputCamera = getSceneNodeValue("CameraIn");

    if (inputCamera) 
    {
        // get camera from scene node
        Ogre::Camera *camera = OgreTools::getFirstCamera(inputCamera);

        if (camera) 
        {
            try
            {
                OgreTools::deepCopySceneNode( inputCamera, m_cameraSceneNodeR, m_name+"Right", m_sceneManager);
                OgreManager::relocateSceneNode(m_cameraSceneNodeR, m_sceneNode);
            }
            catch( Ogre::Exception& e)
            {
                Log::error(QString::fromStdString( "The camera's scene node could not be copied: " + e.getDescription()), "StereoCameraNode::processCamera");
                return;
            }

            // copy cameras
            m_cameraR = OgreTools::getFirstCamera(m_cameraSceneNodeR);
			m_cameraContainerR = OgreTools::getOgreContainer(m_cameraSceneNodeR);
			m_cameraContainerR->setCamera(m_cameraR);

            m_outputCameraR->setValue(QVariant::fromValue<Ogre::SceneNode*>(m_cameraSceneNodeR));
            m_outputCameraR->setDirty(false);

            AdjustCameras();
        }
    }
}

void StereoCamera::AdjustCameras( )
{
	float eyeDistance   = this->getValue("Eye Distance").toFloat();
	float frustumOffset = this->getValue("Frustum Offset Multiplier").toFloat();
	float focalLength   = this->getValue("Focal Length").toFloat();
	float fieldOfView   = this->getValue("Field of View").toFloat();
	bool reverseStereo  = this->getValue("Reverse Stereo").toBool();

	if( reverseStereo )
		eyeDistance *= -1.0f;

    if( m_cameraL )
    {
        m_cameraL->setPosition( -0.5f * eyeDistance, 0.0f, 0.0f );
        m_cameraL->setFrustumOffset( -0.5f * frustumOffset * eyeDistance, 0.0);
		m_cameraL->setFocalLength( focalLength);
		m_cameraL->setFOVy( Ogre::Radian( Ogre::Degree( fieldOfView)));
		m_cameraContainerL->updateCopies();

    }
    if( m_cameraR )
    {
        m_cameraR->setPosition(  0.5f * eyeDistance, 0.0f, 0.0f );
        m_cameraR->setFrustumOffset( 0.5f * frustumOffset * eyeDistance, 0.0);
		m_cameraR->setFocalLength( focalLength);
		m_cameraR->setFOVy( Ogre::Radian( Ogre::Degree( fieldOfView)));
		m_cameraContainerR->updateCopies();
    }
}

//!
//! Returns the scene node that contains scene objects created or modified
//! by this node.
//!
//! \return The scene node containing objects created or modified by this node.
//!
Ogre::SceneNode * StereoCamera::getSceneNode ()
{
    return m_sceneNode;
}

} // namespace StereoCameraNode
