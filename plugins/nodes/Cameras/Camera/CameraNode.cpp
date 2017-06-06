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
//! \file "CameraNode.cpp"
//! \brief Implementation file for CameraNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "CameraNode.h"
#include "SceneNodeParameter.h"
#include "EnumerationParameter.h"
#include "NumberParameter.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include "Log.h"

namespace CameraNode {
using namespace Frapper;


INIT_INSTANCE_COUNTER(CameraNode)
Q_DECLARE_METATYPE(Ogre::Quaternion)

///
/// Static Constants
///

//!
//! Matrix to transform clip-space into image-space.
//!
static const Ogre::Matrix4 s_CLIP_SPACE_TO_IMAGE_SPACE(
0.5,    0,    0,  0.5,
0,   -0.5,    0,  0.5,
0,      0,    1,    0,
0,      0,    0,    1);

///
/// Constructors and Destructors
///


//!
//! Constructor of the CameraNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
CameraNode::CameraNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ViewNode(name, parameterRoot),
    m_sceneManager(OgreManager::getSceneManager()),
    m_sceneNode(0),
    m_ogreContainer(0),
    m_camera(0),
    m_entity(0),
    m_outputCameraName("Camera")
{
    // calculate the render resolution's aspect ratio based on the width and height set in the camera.xml file
    unsigned int renderWidth = getUnsignedIntValue("Render Resolution > Render Width");
    unsigned int renderHeight = getUnsignedIntValue("Render Resolution > Render Height");
    if (renderHeight == 0) {
        Log::error("Render height is 0.", "CameraNode::CameraNode");
        return;
    }
    float aspectRatio = (float) renderWidth / renderHeight;
    setValue("Render Resolution > Aspect Ratio", QString("%1").arg(aspectRatio), true);

    // create an output camera parameter
    CameraParameter *outputCameraParameter = new CameraParameter(m_outputCameraName);
    outputCameraParameter->setPinType(Parameter::PT_Output);
	parameterRoot->addParameter(outputCameraParameter);
    outputCameraParameter->addAffectingParameter(getParameter("Render Resolution > Preset"));
    outputCameraParameter->addAffectingParameter(getParameter("Render Resolution > Render Width"));
    outputCameraParameter->addAffectingParameter(getParameter("Render Resolution > Render Height"));
    outputCameraParameter->addAffectingParameter(getParameter("Render Resolution > Aspect Ratio"));
    outputCameraParameter->addAffectingParameter(getParameter("Focal Length"));
    outputCameraParameter->addAffectingParameter(getParameter("Field of View"));
    outputCameraParameter->addAffectingParameter(getParameter("Horizontal Aperture"));
    outputCameraParameter->addAffectingParameter(getParameter("Near Clipping Plane"));
    outputCameraParameter->addAffectingParameter(getParameter("Far Clipping Plane"));
	outputCameraParameter->addAffectingParameter(getParameter("Image Space Projection Matix(4x4)"));
	outputCameraParameter->addAffectingParameter(getParameter("World Matix(4x4)"));
	outputCameraParameter->addAffectingParameter(getParameter("Far Corner"));

    // create OGRE scene node and scene node encapsulation
    m_sceneNode = OgreManager::createSceneNode(m_name);
    if (m_sceneNode) {
        m_ogreContainer = new OgreContainer(m_sceneNode);
        m_sceneNode->setUserAny(Ogre::Any(m_ogreContainer));
        m_sceneNode->setScale(0.3f, 0.3f, -0.4f);
    }
    setValue(m_outputCameraName, m_sceneNode, true);

    // create an OGRE camera object
    QString cameraName = QString("%1Camera").arg(m_name);
    m_camera = m_sceneManager->createCamera(cameraName.toStdString());

	if (m_camera) {
		m_ogreCameraContainer = new OgreContainer(m_camera, renderWidth, renderHeight);
        dynamic_cast<Ogre::MovableObject *>(m_camera)->setUserAny(Ogre::Any(m_ogreCameraContainer));
    }

    // create a new OGRE entity to represent the camera in the viewport
	Ogre::String test = m_sceneNode->getName();
    QString entityName = QString("%1Entity").arg(m_name);
    m_entity = m_sceneManager->createEntity(entityName.toStdString(), "camera.mesh");

    if (m_entity) {
        int numOfSubentities = m_entity->getNumSubEntities();
        for (int i = 0; i < numOfSubentities; ++i) {
            // create a new number parameter for the bone
            Ogre::SubEntity *subEntity = m_entity->getSubEntity(i);
            subEntity->setCustomParameter(0, Ogre::Vector4(0.0, 0.0, 0.0, 0.0));
        }
    }

    // attach the camera and the entity to the scene node
    if (m_sceneNode) {
        if (m_camera)
            m_sceneNode->attachObject(m_camera);
        if (m_entity)
            m_sceneNode->attachObject(m_entity);
    }

    // set up parameter callback functions
    setProcessingFunction("Position", SLOT(applyPosition()));
	setProcessingFunction("Orientation", SLOT(applyOrientation()));  
	setProcessingFunction("OrientationQ", SLOT(applyQOrientation()));  
    //setProcessingFunction("Target Position", SLOT(applyPosition()));   /investigate SEIM!!!
    //setProcessingFunction("Target Orientation", SLOT(applyOrientation()));

	setChangeFunction("Position", SLOT(applyPosition()));
	setChangeFunction("Orientation", SLOT(applyOrientation()));  
	setChangeFunction("OrientationQ", SLOT(applyQOrientation()));  
	//setChangeFunction("Target Position", SLOT(applyPosition()));
	//setChangeFunction("Target Orientation", SLOT(applyOrientation()));

	setChangeFunction("Position", SIGNAL(triggerRedraw()));
	setChangeFunction("Orientation", SIGNAL(triggerRedraw()));
	setChangeFunction("OrientationQ", SIGNAL(triggerRedraw()));
	setChangeFunction("Target Position", SIGNAL(triggerRedraw()));
	setChangeFunction("Target Orientation", SIGNAL(triggerRedraw()));

	setChangeFunction("Render Resolution > Preset", SLOT(renderResolutionPresetChanged()));
    setChangeFunction("Render Resolution > Render Width", SLOT(renderWidthChanged()));
    setChangeFunction("Render Resolution > Render Height", SLOT(renderHeightChanged()));
    setChangeFunction("Focal Length", SLOT(focalLengthChanged()));
    setChangeFunction("Field of View", SLOT(fieldOfViewChanged()));
    setChangeFunction("Near Clipping Plane", SLOT(nearClippingPlaneChanged()));
    setChangeFunction("Far Clipping Plane", SLOT(farClippingPlaneChanged()));

	// prevent double connection for orientaton, jo!
	setOnConnectFunction("Orientation", SLOT(onConnect()));
	setOnConnectFunction("OrientationQ", SLOT(onConnect()));

    // initialize the camera by calling the callback functions
    //applyPosition();
    //applyOrientation();
    renderWidthChanged();
    renderHeightChanged();
    focalLengthChanged();
    fieldOfViewChanged();
    nearClippingPlaneChanged();
    farClippingPlaneChanged();

    this->setSaveable(true);
}


//!
//! Destructor of the CameraNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CameraNode::~CameraNode ()
{
    OgreTools::deepDeleteSceneNode(m_sceneNode, m_sceneManager, true);
}


///
/// Public Functions
///


//!
//! Returns the scene node that contains scene objects created or modified
//! by this node.
//!
//! \return The scene node containing objects created or modified by this node.
//!
Ogre::SceneNode * CameraNode::getSceneNode ()
{
    Ogre::SceneNode *result = getSceneNodeValue(m_outputCameraName, true);
    if (!result)
        Log::error("Could not obtain camera scene node.", "CameraNode::getSceneNode");
    return result;
}


///
/// Private Functions
///


//!
//! Checks if the currently set render width and height corresponds to a
//! render resolution preset, and if so, selects that preset.
//!
void CameraNode::checkForPreset ()
{
    EnumerationParameter *renderResolutionPresetParameter = getEnumerationParameter("Render Resolution > Preset");
    NumberParameter *renderWidthParameter = getNumberParameter("Render Resolution > Render Width");
    NumberParameter *renderHeightParameter = getNumberParameter("Render Resolution > Render Height");
    if (!renderResolutionPresetParameter || !renderWidthParameter || !renderHeightParameter) {
        Log::error("Render resoulution preset, width and/or render height parameter could not be obtained.", "CameraNode::checkForPreset");
        return;
    }

    unsigned int width = renderWidthParameter->getValue().toUInt();
    unsigned int height = renderHeightParameter->getValue().toUInt();

    QStringList values = renderResolutionPresetParameter->getValues();
    int index = 0;
    foreach (QString value, values) {
        QStringList parts = value.split(" x ");
        if (parts.size() == 2) {
            unsigned int presetWidth = parts[0].toUInt();
            unsigned int presetHeight = parts[1].toUInt();

            if (presetWidth == width && presetHeight == height) {
                renderResolutionPresetParameter->setValue(index);
                return;
            }
        }
        ++index;
    }

    // select the custom render resolution item
    QStringList literals = renderResolutionPresetParameter->getLiterals();
    int customIndex = literals.indexOf("Custom");
    renderResolutionPresetParameter->setValue(customIndex);
}


///
/// Private Slots
///

//!
//! Applies the currently set transformation for the node to all copys in
//! the current Ogre context.
//!
void CameraNode::updateTransform ()
{
    m_ogreContainer->updateCopies();
}

//!
//! Applies the intrinsic and extrinsic parameters of the camera to all.
//! copys in the current Ogre context.
//! on frame change.
//!
void CameraNode::updateFrustum ()
{
	updateDependentOutputParameters();
	m_ogreCameraContainer->updateCopies();
	triggerRedraw();
}

//!
//! Applies the currently set position for the node to the OGRE scene
//! objects contained in this node.
//!
void CameraNode::applyPosition ()
{
	Parameter *parameter = dynamic_cast<Frapper::NumberParameter *>(sender());
	QVariant value = parameter->getValue();
    
	if (!value.canConvert<Ogre::Vector3>())
		return;

	Ogre::Vector3 position = value.value<Ogre::Vector3>();

	// apply the position to the camera's scene node and all its copies
	m_sceneNode->setPosition(position);

	if( getBoolValue("lookAtEnabled")) 
	{
		// from target position and orientation, rotate camera in local space
		float azimuth = this->getDoubleValue("Azimuth angle");
		this->m_sceneNode->rotate( Ogre::Vector3(1,0,0), Ogre::Degree(azimuth), Ogre::Node::TS_LOCAL);
		float zenith = this->getDoubleValue("Zenith angle");
		this->m_sceneNode->rotate( Ogre::Vector3(0,1,0), Ogre::Degree(zenith), Ogre::Node::TS_LOCAL);   

		// shift camera along the z-axis
		float radius = this->getDoubleValue("Radius");
		this->m_sceneNode->translate( 0.0f, 0.0f, radius, Ogre::Node::TS_LOCAL );
	}

    updateTransform();
}


//!
//! Applies the currently set orientation for the node to the OGRE scene
//! objects contained in this node.
//!
void CameraNode::applyOrientation ()
{
	Parameter *parameter = dynamic_cast<Frapper::NumberParameter *>(sender());
	QVariant value = parameter->getValue();

	if (!value.canConvert<Ogre::Vector3>())
		return;

	Ogre::Vector3 rotation = value.value<Ogre::Vector3>();

	Ogre::Radian xRadian;
	Ogre::Radian yRadian;
	Ogre::Radian zRadian;

	bool useRadians = getBoolValue("useRadians");

	if (useRadians) {
		// decode the parameter's values
		xRadian = Ogre::Radian(rotation.x);
		yRadian = Ogre::Radian(rotation.y);
		zRadian = Ogre::Radian(rotation.z);
	} else {
		xRadian = Ogre::Radian(Ogre::Degree(rotation.x));
		yRadian = Ogre::Radian(Ogre::Degree(rotation.y));
		zRadian = Ogre::Radian(Ogre::Degree(rotation.z));
	}

	// apply the orientation to the camera's scene node
	m_sceneNode->resetOrientation();
	m_sceneNode->pitch(xRadian, Ogre::Node::TS_WORLD);
	m_sceneNode->yaw(yRadian, Ogre::Node::TS_WORLD);
	m_sceneNode->roll(zRadian, Ogre::Node::TS_WORLD);

    updateTransform();
}


//!
//! Applies the currently set quaternion orientation for the node to the OGRE scene
//! objects contained in this node.
//!
void CameraNode::applyQOrientation ()
{
	QVariant value = getValue( "OrientationQ" );

	if (!value.canConvert<Ogre::Quaternion>())
		return;

	Ogre::Quaternion orientation = value.value<Ogre::Quaternion>();
    m_sceneNode->setOrientation(orientation);
    
	updateTransform();
}


//!
//! Deletes the second orientation input parameter.
//!
void CameraNode::onConnect ()
{
	const QString &parameterName = dynamic_cast<Parameter *>(sender())->getName();

	if (parameterName == "Orientation") {
		Parameter *qParameter = getParameter("OrientationQ");
		if (qParameter->isConnected()) {
			QList<Connection *> &connections = qParameter->getConnectionMap().values();
			foreach (Connection *connection, connections) {
				deleteConnection(connection);
			}
		}
	}

	else if (parameterName == "OrientationQ") {
		Parameter *parameter = getParameter("Orientation");
		if (parameter->isConnected()) {
			QList<Connection *> &connections = parameter->getConnectionMap().values();
			foreach (Connection *connection, connections) {
				deleteConnection(connection);
			}
		}
	}
}


//!
//! Updates the render resolution parameters according to the selected
//! preset.
//!
//! Is called when the value of the render resolution preset parameter has
//! changed.
//!
void CameraNode::renderResolutionPresetChanged ()
{
    // obtain the render resolution preset parameter
    EnumerationParameter *renderResolutionPresetParameter = getEnumerationParameter("Render Resolution > Preset");
    if (!renderResolutionPresetParameter) {
        Log::error("Render resolution preset parameter could not be obtained.", "CameraNode::renderResolutionPresetChanged");
        return;
    }

    // get the value of the currently selected item in the enumeration
    QString value = renderResolutionPresetParameter->getCurrentValue();
    if (value.isNull())
        //> no value is available for the selected render resolution preset
        return;

    // split the enumeration parameter's value into parts
    QStringList values = value.split(" x ");
    if (values.size() == 2) {
        // decode the enumeration parameter's value into width and height and calculate the aspect ratio
        unsigned int width = values[0].toUInt();
        unsigned int height = values[1].toUInt();
        float aspectRatio = float(width) / height;

        // apply width, height and aspect ratio to the respective parameters
        setValue("Render Resolution > Render Width", width);
		renderWidthChanged();
        setValue("Render Resolution > Render Height", height);
		renderHeightChanged ();
        setValue("Render Resolution > Aspect Ratio", QString("%1").arg(aspectRatio));
    } else
        Log::debug("The render resolution preset's value does not contain a width and height.", "CameraNode::renderResolutionPresetChanged");

	updateFrustum();
}


//!
//! Applies the changed render width to the output camera.
//!
//! Is called when the value of the render width parameter has changed.
//!
void CameraNode::renderWidthChanged ()
{
    unsigned int width = getUnsignedIntValue("Render Resolution > Render Width");

    // apply the changed render width to the output camera
	OgreContainer *cameraContainer = Ogre::any_cast<OgreContainer *>(((Ogre::MovableObject *) m_camera)->getUserAny());
	if (cameraContainer) {
		cameraContainer->setCameraWidth(width);

		NumberParameter *renderHeightParameter = getNumberParameter("Render Resolution > Render Height");
		if (!renderHeightParameter)
			return;
		unsigned int height = renderHeightParameter->getValue().toUInt();

		// check if the aspect ratio is to be kept
		bool keepAspectRatio = getBoolValue("Keep Aspect Ratio");
		float aspectRatio = getValue("Aspect Ratio").toFloat();
		if (keepAspectRatio) {
			// calculate the height based on the current aspect ratio
			if (aspectRatio == 0) {
				Log::error("Aspect ratio is 0.", "CameraNode::renderWidthChanged");
				return;
			}
			height = (float) width / aspectRatio + 0.5;
			cameraContainer->setCameraHeight(height);
			renderHeightParameter->setValue(QVariant(height), true);
		} else {
			// calculate the aspect ratio based on the render
			aspectRatio = (float) width / height;
			setValue("Render Resolution > Aspect Ratio", QString("%1").arg(aspectRatio), true);
		}
		m_camera->setAspectRatio(aspectRatio);
	}

    checkForPreset();
	updateFrustum();
}


//!
//! Applies the changed render height to the output camera.
//!
//! Is called when the value of the render height parameter has changed.
//!
void CameraNode::renderHeightChanged ()
{
    unsigned int height = getUnsignedIntValue("Render Resolution > Render Height");

    // apply the changed render height to the output camera
	OgreContainer *cameraContainer = Ogre::any_cast<OgreContainer *>(((Ogre::MovableObject *) m_camera)->getUserAny());
	if (cameraContainer) {
		cameraContainer->setCameraHeight(height);

		NumberParameter *renderWidthParameter = getNumberParameter("Render Resolution > Render Width");
		if (!renderWidthParameter)
			return;
		unsigned int width = renderWidthParameter->getValue().toUInt();

		// check if the aspect ratio is to be kept
		bool keepAspectRatio = getBoolValue("Keep Aspect Ratio");
		float aspectRatio = getValue("Aspect Ratio").toFloat();
		if (keepAspectRatio) {
			// calculate the width based on the current aspect ratio
			if (aspectRatio == 0) {
				Log::error("Aspect ratio is 0.", "CameraNode::renderHeightChanged");
				return;
			}
			width = (float) height * aspectRatio + 0.5;
			cameraContainer->setCameraWidth(width);
			renderWidthParameter->setValue(QVariant(width), true);
		} else {
			// calculate the aspect ratio based on the render
			aspectRatio = (float) width / height;
			setValue("Render Resolution > Aspect Ratio", QString("%1").arg(aspectRatio), true);
		}
		m_camera->setAspectRatio(aspectRatio);
	}

    checkForPreset();
	updateFrustum();
}


//!
//! Applies the changed focal length to the output camera.
//!
//! Is called when the value of the focal length parameter has changed.
//!
void CameraNode::focalLengthChanged ()
{
    NumberParameter *focalLengthParameter = getNumberParameter("Focal Length");
	if (m_camera && focalLengthParameter) {
        m_camera->setFocalLength(focalLengthParameter->getValue().toFloat());
	}
	updateFrustum();
}


//!
//! Applies the changed field of view to the output camera.
//!
//! Is called when the value of the field of view parameter has changed.
//!
void CameraNode::fieldOfViewChanged ()
{
    NumberParameter *fieldOfViewParameter = getNumberParameter("Field of View");
	if (m_camera && fieldOfViewParameter) {
		m_camera->setFOVy(Ogre::Degree(fieldOfViewParameter->getValue().toFloat()));
	}
	updateFrustum();
}


//!
//! Applies the changed near clipping plane to the output camera.
//!
//! Is called when the value of the near clipping plane parameter has
//! changed.
//!
void CameraNode::nearClippingPlaneChanged ()
{
    NumberParameter *nearClippingPlaneParameter = getNumberParameter("Near Clipping Plane");
	if (m_camera && nearClippingPlaneParameter) {
		float nearClipParam = nearClippingPlaneParameter->getValue().toFloat();
		if (nearClipParam == 0)
			nearClipParam = 0.001f;
        m_camera->setNearClipDistance(nearClipParam);
	}
	updateFrustum();
}


//!
//! Applies the changed far clipping plane to the output camera.
//!
//! Is called when the value of the far clipping plane parameter has
//! changed.
//!
void CameraNode::farClippingPlaneChanged ()
{
    NumberParameter *farClippingPlaneParameter = getNumberParameter("Far Clipping Plane");
	if (m_camera && farClippingPlaneParameter) {
        m_camera->setFarClipDistance(farClippingPlaneParameter->getValue().toFloat());
	}
	updateFrustum();
}


//!
//! Updates output parameters based on the frustum
//!
//! It should be called when a value of the frustum has changed
//!
void CameraNode::updateDependentOutputParameters ()
{
	if (m_camera) {
		Ogre::Matrix4 wMat;
		m_camera->getWorldTransforms(&wMat);
		// get projection matrix
		Ogre::Matrix4 ptMat = s_CLIP_SPACE_TO_IMAGE_SPACE * m_camera->getProjectionMatrixRS();

		// set matrix as otput
		m_ptMatList.clear();
		m_wMatList.clear();

		//std::cout << "Camera NODE:" << std::endl;
		for(int i = 0; i < 4; ++i) {
			m_ptMatList.append(QVariant::fromValue(ptMat[i][0]));
			m_ptMatList.append(QVariant::fromValue(ptMat[i][1]));
			m_ptMatList.append(QVariant::fromValue(ptMat[i][2]));
			m_ptMatList.append(QVariant::fromValue(ptMat[i][3]));

			m_wMatList.append(QVariant::fromValue(wMat[i][0]));
			m_wMatList.append(QVariant::fromValue(wMat[i][1]));
			m_wMatList.append(QVariant::fromValue(wMat[i][2]));
			m_wMatList.append(QVariant::fromValue(wMat[i][3]));
		}
		setValue("Screen Space Projection Matix(4x4)", m_ptMatList, true);
		setValue("World Matix(4x4)", m_wMatList, true);

		// prepare output parameters		
		Ogre::Vector3 farCorner = m_camera->getViewMatrix(true) * m_camera->getWorldSpaceCorners()[4];
		setValue("Far Corner", farCorner, true);
		
		// just for debugging
		//writeOutCameraData();
	}
}

//!
//! Hack! Called when scene load is ready to prevent wrong recource setup in Ogre
//!
void CameraNode::loadReady ()
{
	Frapper::Parameter *width = getParameter("Render Width");
	if (width)
	 width->propagateDirty();
}

//!
//! For debugging!
//!
void CameraNode::writeOutCameraData()
{
	Ogre::Matrix4 ptMat = m_camera->getProjectionMatrixRS();
	Ogre::Vector3 farCorner = m_camera->getViewMatrix(true) * m_camera->getWorldSpaceCorners()[4];

	std::cout << "Camera NODE:" << std::endl;

	for(int i = 0; i < 4; ++i) {
		// Test: Display matrix on console for testing
		std::cout << "    " << ptMat[i][0];
		std::cout << " \t " << ptMat[i][1];
		std::cout << " \t " << ptMat[i][2];
		std::cout << " \t " << ptMat[i][3] << std::endl;
	}

	std::cout << "Far Corner:" << farCorner << std::endl;
	std::cout << "Far Corner:" << farCorner << std::endl;

}

} // namespace CameraNode
