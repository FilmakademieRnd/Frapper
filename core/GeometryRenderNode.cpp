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
//! \file "GeometryRenderNode.cpp"
//! \brief Implementation file for GeometryRenderNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       26.05.2009 (last updated)
//!

#include "GeometryRenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreManager.h"
#include "OgreContainer.h"

namespace Frapper {

///
/// Protected Static Constants
///


//!
//! The name of the input geometry parameter.
//!
const QString GeometryRenderNode::InputGeometryParameterName = "Geometry to Render";

//!
//! The name of the input lights parameter.
//!
const QString GeometryRenderNode::InputLightsParameterName = "Lights";

//!
//! The name of the input camera parameter.
//!
const QString GeometryRenderNode::InputCameraParameterName = "Camera";

//!
//! The name of the camera light toggle parameter.
//!
const QString GeometryRenderNode::CameraLightToggleParameterName = "Use camera light instead of input lights";

//!
//! The name of the background color parameter.
//!
const QString GeometryRenderNode::BackgroundColorParameterName = "Background Color";


///
/// Constructors and Destructors
///


//!
//! Constructor of the GeometryRenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
GeometryRenderNode::GeometryRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
RenderNode(name, parameterRoot),
m_cameraSceneNode(0),
m_lightSceneNode(0),
m_geometrySceneNode(0)
{
    // create the mandatory geometry input parameter - multiplicity 1
    GeometryParameter *geometryParameter = new GeometryParameter(InputGeometryParameterName);
    geometryParameter->setMultiplicity(1);
    geometryParameter->setPinType(Parameter::PT_Input);
    geometryParameter->setSelfEvaluating(true);
    parameterRoot->addParameter(geometryParameter);

    // create the mandatory light input parameter - multiplicity *
    LightParameter *lightsParameter = new LightParameter(InputLightsParameterName);
    lightsParameter->setMultiplicity(Parameter::M_OneOrMore);
    lightsParameter->setPinType(Parameter::PT_Input);
    lightsParameter->setSelfEvaluating(true);
    parameterRoot->addParameter(lightsParameter);

    // create the mandatory camera input parameter - multiplicity 1
    CameraParameter *cameraParameter = new CameraParameter(InputCameraParameterName);
    geometryParameter->setMultiplicity(1);
    cameraParameter->setPinType(Parameter::PT_Input);
    parameterRoot->addParameter(cameraParameter);
    geometryParameter->setSelfEvaluating(true);

    // create the background color parameter
    Parameter *backgroundColorParameter = new Parameter(BackgroundColorParameterName, Parameter::T_Color, QColor(Qt::black));
    parameterRoot->addParameter(backgroundColorParameter, true);

    // create the camera light toggle parameter
    Parameter *cameraLightToggleParameter = new Parameter(CameraLightToggleParameterName, Parameter::T_Bool, true);
    parameterRoot->addParameter(cameraLightToggleParameter, true);

    // set up parameter affections for output image parameter
    Parameter *outputImageParameter = getParameter(m_outputImageName);
    if (outputImageParameter) {
		outputImageParameter->setProcessingFunction(SLOT(processOutputImage()));
        outputImageParameter->setDirty(true);
        outputImageParameter->addAffectingParameter(geometryParameter);
        outputImageParameter->addAffectingParameter(lightsParameter);
        outputImageParameter->addAffectingParameter(cameraParameter);
        outputImageParameter->addAffectingParameter(cameraLightToggleParameter);
        outputImageParameter->addAffectingParameter(backgroundColorParameter);
    }
	outputImageParameter->setSelfEvaluating(true);
}


//!
//! Destructor of the GeometryRenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
GeometryRenderNode::~GeometryRenderNode ()
{
    // destroy all scene nodes and objects in the render scene
	OgreTools::deepDeleteSceneNode(m_sceneManager->getRootSceneNode(), m_sceneManager);
}

//!
//! Processes the node's input data to generate the node's output image.
//!
void GeometryRenderNode::processOutputImage ()
{
	Ogre::SceneNode *rootSceneNode = m_sceneManager->getRootSceneNode();

	// destroy all scene nodes and objects in the render scene
	OgreTools::deepDeleteSceneNode(rootSceneNode, m_sceneManager);

	// reset output image to default
	setValue(m_outputImageName, m_defaultTexture);

	// process input geometry
	m_geometrySceneNode = getSceneNodeValue(InputGeometryParameterName);
	if (m_geometrySceneNode) {
		// duplicate the input geometry
		Ogre::SceneNode *geometrySceneNodeCopy = 0;
		OgreTools::deepCopySceneNode(m_geometrySceneNode, geometrySceneNodeCopy, m_name, m_sceneManager);
		// add the geometry to the render scene
		rootSceneNode->addChild(geometrySceneNodeCopy);
		m_geometrySceneNode = geometrySceneNodeCopy;
	} 
	else {
		Log::error("Could not obtain scene node from input geometry parameter.", "GeometryRenderNode::processOutputImage");
		return;
	}

	// process input camera
	m_cameraSceneNode = getSceneNodeValue(InputCameraParameterName);
	if (m_cameraSceneNode) {
		// get the first camera attached to the input camera scene node
		m_camera = OgreTools::getFirstCamera(m_cameraSceneNode);
		if (m_camera) {
			// duplicate the input camera
			Ogre::SceneNode *cameraSceneNodeCopy = 0;
			OgreTools::deepCopySceneNode(m_cameraSceneNode, cameraSceneNodeCopy, m_name, m_sceneManager);
			if (!cameraSceneNodeCopy) {
				Log::error("The camera's scene node could not be copied.", "GeometryRenderNode::processOutputImage");
				return;
			}
			// set visibility of light gometry representation to false
			Ogre::Entity *entity = OgreTools::getFirstEntity(cameraSceneNodeCopy);
			if (entity)
				entity->setVisible(false);
			// add the lights to the render scene
			rootSceneNode->addChild(cameraSceneNodeCopy);
			m_cameraSceneNode = cameraSceneNodeCopy;

			// process input light
			Parameter *lightParameter = getParameter(InputLightsParameterName);
			bool noInputLight = true;
			foreach( QVariant parameterValue, lightParameter->getValueList()) {
				if( parameterValue.canConvert<Ogre::SceneNode *>() ) {
					Ogre::SceneNode *inputLightSceneNode = parameterValue.value<Ogre::SceneNode *>();
					// get the lights attached to the input light scene node
					Ogre::Light *light = OgreTools::getFirstLight(inputLightSceneNode);
					if (light) {
						// duplicate the input camera
						Ogre::SceneNode *LightSceneNodeCopy = 0;
						OgreTools::deepCopySceneNode(inputLightSceneNode, LightSceneNodeCopy, m_name, m_sceneManager);
						if (!LightSceneNodeCopy) {
							Log::error("The light's scene node could not be copied.", "GeometryRenderNode::processOutputImage");
							return;
						}
						// set visibility of light gometry representation to false
						entity = OgreTools::getFirstEntity(LightSceneNodeCopy);
						if (entity)
							entity->setVisible(false);
						// add the lights to the render scene
						rootSceneNode->addChild(LightSceneNodeCopy);
						m_lightSceneNode = LightSceneNodeCopy;
						noInputLight = false;
					}
				}
			}
			// if no input light
			if (noInputLight && getBoolValue(CameraLightToggleParameterName)) {
				// set up names for camera light objects
				QString cameraLightName = QString("%1CameraLight").arg(m_name);
				// create camera light objects
				Ogre::Light *cameraLight = m_sceneManager->createLight(cameraLightName.toStdString());
				cameraSceneNodeCopy->attachObject(cameraLight);
			}

			// copy first camera
			m_camera = OgreTools::getFirstCamera(cameraSceneNodeCopy);
			OgreTools::getOgreContainer(cameraSceneNodeCopy)->setCamera(m_camera);

			// retrieve render resolution values from custom parameter
			const Ogre::Any &customData = dynamic_cast<Ogre::MovableObject *>(m_camera)->getUserAny();
			if (!customData.isEmpty()) {
				OgreContainer *cameraContainer = Ogre::any_cast<OgreContainer *>(customData);
				if (cameraContainer)
					resizeAllTargets(cameraContainer->getCameraWidth(), cameraContainer->getCameraHeight());
			}
			// obtain the backgrond color
			QColor bgColor = getColorValue(BackgroundColorParameterName);

			// (re-)create the render texture
			if (bgColor.alphaF() < 1.0f)
				initializeRenderTarget(m_camera, Ogre::ColourValue(bgColor.redF(), bgColor.greenF(), bgColor.blueF(), bgColor.alphaF()), Ogre::PF_R8G8B8A8);
			else
				initializeRenderTarget(m_camera, Ogre::ColourValue(bgColor.redF(), bgColor.greenF(), bgColor.blueF()));

			// update render texture and render target
			setValue(m_outputImageName, m_renderTexture, false);

			// undirty the output parameter to prevent multiple updates
			getParameter(m_outputImageName)->setDirty(true);
		}
	}
	else {
		m_camera = 0;
		Log::error("First object attached to scene node contained in input camera parameter is not a camera.", "GeometryRenderNode::processOutputImage");
	}
	RenderNode::redrawTriggered();
}


} // end namespace Frapper