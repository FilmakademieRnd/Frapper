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
//! \file "FiveViewCamera.cpp"
//! \brief Implementation file for FiveViewCamera class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       07.02.2012 (created)
//! \date       07.02.2012 (last updated)
//!

#include "FiveViewCamera.h"
#include "OgreManager.h"

namespace FiveViewCameraNode {
using namespace Frapper;


///
/// Constructors and Destructors
///

//!
//! Constructor of the FiveViewCamera class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
FiveViewCamera::FiveViewCamera ( const QString &name, ParameterGroup *parameterRoot ) :
    ViewNode( name, parameterRoot),
    m_sceneNode(0),
    m_ogreContainer(0),
    m_cameraSceneNode1(NULL), 
    m_cameraSceneNode2(NULL), 
    m_cameraSceneNode3(NULL), 
    m_cameraSceneNode4(NULL), 
    m_cameraSceneNode5(NULL), 
    m_camera1(NULL), 
    m_camera2(NULL), 
    m_camera3(NULL), 
    m_camera4(NULL), 
    m_camera5(NULL)
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
    m_outputCamera1= new CameraParameter( "Camera1");
    m_outputCamera1->setPinType(Parameter::PT_Output);
    m_outputCamera1->setNode(this);
    m_outputCamera1->setProcessingFunction( SLOT(ProcessCamera1()));
    m_outputCamera1->setDirty(true);
    m_outputCamera1->addAffectingParameter( cameraParameter );
    m_outputCamera1->addAffectingParameter(getParameter("Focal Length"));
    m_outputCamera1->addAffectingParameter(getParameter("Field of View"));

    parameterRoot->addParameter(m_outputCamera1);

    // create an output camera parameter
    m_outputCamera2= new CameraParameter( "Camera2");
    m_outputCamera2->setPinType(Parameter::PT_Output);
    m_outputCamera2->setNode(this);
    m_outputCamera2->setProcessingFunction( SLOT(ProcessCamera2()));
    m_outputCamera2->setDirty(true);
    m_outputCamera2->addAffectingParameter( cameraParameter );
    m_outputCamera2->addAffectingParameter(getParameter("Focal Length"));
    m_outputCamera2->addAffectingParameter(getParameter("Field of View"));
    parameterRoot->addParameter(m_outputCamera2);
	
    // create an output camera parameter
    m_outputCamera3= new CameraParameter( "Camera3");
    m_outputCamera3->setPinType(Parameter::PT_Output);
    m_outputCamera3->setNode(this);
    m_outputCamera3->setProcessingFunction( SLOT(ProcessCamera3()));
    m_outputCamera3->setDirty(true);
    m_outputCamera3->addAffectingParameter( cameraParameter );
    m_outputCamera3->addAffectingParameter(getParameter("Focal Length"));
    m_outputCamera3->addAffectingParameter(getParameter("Field of View"));
    parameterRoot->addParameter(m_outputCamera3);
	
    // create an output camera parameter
    m_outputCamera4= new CameraParameter( "Camera4");
    m_outputCamera4->setPinType(Parameter::PT_Output);
    m_outputCamera4->setNode(this);
    m_outputCamera4->setProcessingFunction( SLOT(ProcessCamera4()));
    m_outputCamera4->setDirty(true);
    m_outputCamera4->addAffectingParameter( cameraParameter );
    m_outputCamera4->addAffectingParameter(getParameter("Focal Length"));
    m_outputCamera4->addAffectingParameter(getParameter("Field of View"));
    parameterRoot->addParameter(m_outputCamera4);
	
    // create an output camera parameter
    m_outputCamera5= new CameraParameter( "Camera5");
    m_outputCamera5->setPinType(Parameter::PT_Output);
    m_outputCamera5->setNode(this);
    m_outputCamera5->setProcessingFunction( SLOT(ProcessCamera5()));
    m_outputCamera5->setDirty(true);
    m_outputCamera5->addAffectingParameter( cameraParameter );
    m_outputCamera5->addAffectingParameter(getParameter("Focal Length"));
    m_outputCamera5->addAffectingParameter(getParameter("Field of View"));
    parameterRoot->addParameter(m_outputCamera5);

    // Add processing functions for stereo parameter
    setChangeFunction( "Eye Distance", SLOT(AdjustCameras()));
    setChangeFunction( "Reverse Stereo", SLOT(AdjustCameras()));
    setChangeFunction( "Frustum Offset Multiplier", SLOT(AdjustCameras()));
    setChangeFunction( "Focal Length", SLOT(AdjustCameras()));
    setChangeFunction( "Field of View", SLOT(AdjustCameras()));

    this->setSaveable(true);
}


//!
//! Destructor of the FiveViewCameraNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
FiveViewCamera::~FiveViewCamera ()
{
    OgreTools::deepDeleteSceneNode(m_sceneNode, m_sceneManager, true);
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void FiveViewCamera::ProcessCamera1()
{

    OgreTools::deepDeleteSceneNode( m_cameraSceneNode1, m_sceneManager, true);
    m_cameraSceneNode1 = NULL;
    m_camera1 = 0;

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
                OgreTools::deepCopySceneNode( inputCamera, m_cameraSceneNode1, m_name+"1", m_sceneManager);
                OgreManager::relocateSceneNode(m_cameraSceneNode1, m_sceneNode);
                m_cameraContainer1 = OgreTools::getOgreContainer(m_cameraSceneNode1);
            }
            catch( Ogre::Exception& e)
            {
                Log::error(QString::fromStdString( "The camera's scene node could not be copied: " + e.getDescription()), "FiveViewCameraNode::processCamera");
                return;
            }

            // copy cameras
            m_camera1 = OgreTools::getFirstCamera(m_cameraSceneNode1);
            m_cameraContainer1->setCamera(m_camera1);


            m_outputCamera1->setValue(QVariant::fromValue<Ogre::SceneNode*>(m_cameraSceneNode1));
            m_outputCamera1->setDirty(false);

            AdjustCameras();
        }
    }
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void FiveViewCamera::ProcessCamera2()
{

    OgreTools::deepDeleteSceneNode( m_cameraSceneNode2, m_sceneManager, true);
    m_cameraSceneNode2 = NULL;
    m_camera2 = 0;

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
                OgreTools::deepCopySceneNode( inputCamera, m_cameraSceneNode2, m_name+"2", m_sceneManager);
                OgreManager::relocateSceneNode(m_cameraSceneNode2, m_sceneNode);
                m_cameraContainer2 = OgreTools::getOgreContainer(m_cameraSceneNode2);
            }
            catch( Ogre::Exception& e)
            {
                Log::error(QString::fromStdString( "The camera's scene node could not be copied: " + e.getDescription()), "FiveViewCameraNode::processCamera");
                return;
            }

            // copy cameras
            m_camera2 = OgreTools::getFirstCamera(m_cameraSceneNode2);
            m_cameraContainer2->setCamera(m_camera2);

            m_outputCamera2->setValue(QVariant::fromValue<Ogre::SceneNode*>(m_cameraSceneNode2));
            m_outputCamera2->setDirty(false);

            AdjustCameras();
        }
    }
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void FiveViewCamera::ProcessCamera3()
{

    OgreTools::deepDeleteSceneNode( m_cameraSceneNode3, m_sceneManager, true);
    m_cameraSceneNode3 = NULL;
    m_camera3 = 0;

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
                OgreTools::deepCopySceneNode( inputCamera, m_cameraSceneNode3, m_name+"3", m_sceneManager);
                OgreManager::relocateSceneNode(m_cameraSceneNode3, m_sceneNode);
                m_cameraContainer3 = OgreTools::getOgreContainer(m_cameraSceneNode3);
            }
            catch( Ogre::Exception& e)
            {
                Log::error(QString::fromStdString( "The camera's scene node could not be copied: " + e.getDescription()), "FiveViewCameraNode::processCamera");
                return;
            }

            // copy cameras
            m_camera3 = OgreTools::getFirstCamera(m_cameraSceneNode3);
            m_cameraContainer3->setCamera(m_camera3);

            m_outputCamera3->setValue(QVariant::fromValue<Ogre::SceneNode*>(m_cameraSceneNode3));
            m_outputCamera3->setDirty(false);

            AdjustCameras();
        }
    }
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void FiveViewCamera::ProcessCamera4()
{

    OgreTools::deepDeleteSceneNode( m_cameraSceneNode4, m_sceneManager, true);
    m_cameraSceneNode4 = NULL;
    m_camera4 = 0;

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
                OgreTools::deepCopySceneNode( inputCamera, m_cameraSceneNode4, m_name+"4", m_sceneManager);
                OgreManager::relocateSceneNode(m_cameraSceneNode4, m_sceneNode);
                m_cameraContainer4 = OgreTools::getOgreContainer(m_cameraSceneNode4);
            }
            catch( Ogre::Exception& e)
            {
                Log::error(QString::fromStdString( "The camera's scene node could not be copied: " + e.getDescription()), "FiveViewCameraNode::processCamera");
                return;
            }

            // copy cameras
            m_camera4 = OgreTools::getFirstCamera(m_cameraSceneNode4);
            m_cameraContainer4->setCamera(m_camera4);

            m_outputCamera4->setValue(QVariant::fromValue<Ogre::SceneNode*>(m_cameraSceneNode4));
            m_outputCamera4->setDirty(false);

            AdjustCameras();
        }
    }
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void FiveViewCamera::ProcessCamera5()
{

    OgreTools::deepDeleteSceneNode( m_cameraSceneNode5, m_sceneManager, true);
    m_cameraSceneNode5 = NULL;
    m_camera5 = 0;

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
                OgreTools::deepCopySceneNode( inputCamera, m_cameraSceneNode5, m_name+"5", m_sceneManager);
                OgreManager::relocateSceneNode(m_cameraSceneNode5, m_sceneNode);
                m_cameraContainer5 = OgreTools::getOgreContainer(m_cameraSceneNode5);
            }
            catch( Ogre::Exception& e)
            {
                Log::error(QString::fromStdString( "The camera's scene node could not be copied: " + e.getDescription()), "FiveViewCameraNode::processCamera");
                return;
            }

            // copy cameras
            m_camera5 = OgreTools::getFirstCamera(m_cameraSceneNode5);
            m_cameraContainer5->setCamera(m_camera5);

            m_outputCamera5->setValue(QVariant::fromValue<Ogre::SceneNode*>(m_cameraSceneNode5));
            m_outputCamera5->setDirty(false);

            AdjustCameras();
        }
    }
}

void FiveViewCamera::AdjustCameras( )
{
    float eyeDistance   = this->getValue("Eye Distance").toFloat();
    float frustumOffset = this->getValue("Frustum Offset Multiplier").toFloat();
    float focalLength   = this->getValue("Focal Length").toFloat();
    float fieldOfView   = this->getValue("Field of View").toFloat();
    bool reverseStereo  = this->getValue("Reverse Stereo").toBool();

    if( reverseStereo )
        eyeDistance *= -1.0f;

    if( m_camera1 )
    {
        m_camera1->setPosition( 1.0f * eyeDistance, 0.0f, 0.0f );
        m_camera1->setFrustumOffset( 1.0f * frustumOffset * eyeDistance, 0.0);
        m_camera1->setFocalLength( focalLength);
        m_camera1->setFOVy( Ogre::Radian( Ogre::Degree( fieldOfView)));
        m_cameraContainer1->updateCopies();
    }
    if( m_camera2 )
    {
        m_camera2->setPosition( 0.5f * eyeDistance, 0.0f, 0.0f );
        m_camera2->setFrustumOffset(  0.5f * frustumOffset * eyeDistance, 0.0);
        m_camera2->setFocalLength( focalLength);
        m_camera2->setFOVy( Ogre::Radian( Ogre::Degree( fieldOfView)));
        m_cameraContainer2->updateCopies();
    }
    if( m_camera3 )
    {
        m_camera3->setPosition(  0.0f * eyeDistance, 0.0f, 0.0f );
        m_camera3->setFrustumOffset(  0.0f * frustumOffset * eyeDistance, 0.0);
        m_camera3->setFocalLength( focalLength);
        m_camera3->setFOVy( Ogre::Radian( Ogre::Degree( fieldOfView)));
        m_cameraContainer3->updateCopies();
    }
    if( m_camera4 )
    {
        m_camera4->setPosition( -0.5f * eyeDistance, 0.0f, 0.0f );
        m_camera4->setFrustumOffset( -0.5f * frustumOffset * eyeDistance, 0.0);
        m_camera4->setFocalLength( focalLength);
        m_camera4->setFOVy( Ogre::Radian( Ogre::Degree( fieldOfView)));
        m_cameraContainer4->updateCopies();
    }
    if( m_camera5 )
    {
        m_camera5->setPosition( -1.0f * eyeDistance, 0.0f, 0.0f );
        m_camera5->setFrustumOffset( -1.0f * frustumOffset * eyeDistance, 0.0);
        m_camera5->setFocalLength( focalLength);
        m_camera5->setFOVy( Ogre::Radian( Ogre::Degree( fieldOfView)));
        m_cameraContainer5->updateCopies();
    }
}

//!
//! Returns the scene node that contains scene objects created or modified
//! by this node.
//!
//! \return The scene node containing objects created or modified by this node.
//!
Ogre::SceneNode * FiveViewCamera::getSceneNode ()
{
    return m_sceneNode;
}

} // namespace FiveViewCameraNode
