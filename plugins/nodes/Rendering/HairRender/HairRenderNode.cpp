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
//! \file "HairRenderNode.cpp"
//! \brief Implementation file for HairRenderNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       11.10.2010 (last updated)
//!

#include "HairRenderNode.h"
#include "GeometryRenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreManager.h"
#include <QtCore/QFile>

#define CLIPPING_STEPS 1
#define SHADOWTEX_SIZE 1024

namespace HairRenderNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


ShadowTargetListener::ShadowTargetListener() :
	m_depthBias(0.0f),
	m_depthOffset(0.0f)
{
	m_shadowCasterPass = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRender/OpacityMap"))->getTechnique(0)->getPass(0);
}

void ShadowTargetListener::preRenderTargetUpdate (const Ogre::RenderTargetEvent &evt)
{
	Ogre::Viewport *vieport = evt.source->getViewport(0);
	Ogre::GpuProgramParametersSharedPtr fParams = m_shadowCasterPass->getFragmentProgramParameters();

	if (!fParams->_findNamedConstantDefinition("iterPass"))
		return;

	vieport->setBackgroundColour(Ogre::ColourValue(0,0,0,1));

	vieport->setClearEveryFrame(true);
	m_shadowCasterPass->setDepthWriteEnabled(true);
	m_shadowCasterPass->setDepthFunction(Ogre::CMPF_LESS_EQUAL);
	m_shadowCasterPass->setSceneBlending(Ogre::SBT_REPLACE);
	m_shadowCasterPass->setDepthBias(0);
	fParams->setNamedConstant("iterPass", 0.0f);
	vieport->update();

	vieport->setClearEveryFrame(false, Ogre::FBT_DEPTH);
	m_shadowCasterPass->setDepthWriteEnabled(false);
	m_shadowCasterPass->setDepthFunction(Ogre::CMPF_GREATER_EQUAL);
	m_shadowCasterPass->setSceneBlending(Ogre::SBT_ADD);

	for (unsigned int i=1; i<4; ++i) {
		m_shadowCasterPass->setDepthBias(m_depthBias*i + m_depthOffset);
		fParams->setNamedConstant("iterPass", Ogre::Real(i));
		vieport->update();
	}
}

void ShadowTargetListener::setDepthBias(const Ogre::Real bias)
{
	m_depthBias = bias;
}

void ShadowTargetListener::setDepthOffset(const Ogre::Real offset)
{
	m_depthOffset = offset;
}

//!
//! Constructor of the HairRenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
HairRenderNode::HairRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryRenderNode(name, parameterRoot),
	m_shadowListener(0)
{
	if (m_sceneManager) {
        m_sceneManager->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));
	}

	// Set the change function for light description file parameter.
    setChangeFunction("Light Description File", SLOT(loadLightDescriptionFile()));
    setChangeFunction("Light Scale Pos", SLOT(setLightScale()));
	setChangeFunction("Number of Lights", SLOT(setNbrLights()));
    setChangeFunction("lightPower", SLOT(setShaderParameter()));
    setChangeFunction("sampleRadius", SLOT(setShaderParameter()));
    setChangeFunction("blurSize", SLOT(setShaderParameter()));
    setChangeFunction("scaleBias", SLOT(setPassDepthBias()));
    setChangeFunction("biasOffset", SLOT(setPassDepthOffset()));
 
#ifdef LIGHT_DEBUG
	m_outputSecondImageName = QString("Light Map");
#endif

	Parameter *outputImageParameter = getParameter(m_outputImageName);
    if (outputImageParameter) {
        outputImageParameter->setProcessingFunction(SLOT(processOutputImage()));
        outputImageParameter->setDirty(true);
		// set up parameter affections
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::InputGeometryParameterName));
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::InputLightsParameterName));
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::InputCameraParameterName));
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::CameraLightToggleParameterName));
		outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::BackgroundColorParameterName));
		outputImageParameter->addAffectingParameter(getParameter("Light Description File"));
    }

#ifdef LIGHT_DEBUG
	addOutputParameter(Parameter::createImageParameter(m_outputSecondImageName));

	Parameter *outputImageParameterLight = getParameter(m_outputSecondImageName);
    if (outputImageParameterLight) {
        outputImageParameterLight->setProcessingFunction(SLOT(processOutputImage()));
		outputImageParameterLight->setAuxProcessingFunction(SLOT(redrawTriggered()));
        outputImageParameterLight->setDirty(true);
		// set up parameter affections
        outputImageParameterLight->addAffectingParameter(getParameter(GeometryRenderNode::InputGeometryParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(GeometryRenderNode::InputLightsParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(GeometryRenderNode::InputCameraParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(GeometryRenderNode::CameraLightToggleParameterName));
		outputImageParameterLight->addAffectingParameter(getParameter(GeometryRenderNode::BackgroundColorParameterName));
    }
#endif
}


//!
//! Destructor of the HairRenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
HairRenderNode::~HairRenderNode ()
{
	m_sceneManager->setShadowTextureSelfShadow(false);
	m_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    if (m_sceneManager)
        // destroy all scene nodes and objects in the render scene
        OgreTools::deepDeleteSceneNode(m_sceneManager->getRootSceneNode(), m_sceneManager);
}


///
/// Public Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void HairRenderNode::processOutputImage ()
{
	if (m_lightPositions.empty()) {
		Log::error("Light description file not loaded.", "HairRenderNode::processOutputImage");
		return;
	}
	
    Parameter *tempImageParameter = getParameter(m_outputImageName);
    if (!tempImageParameter) {
        Log::error("Could not obtain output image parameter.", "HairRenderNode::processOutputImage");
        return;
    }

	m_sceneManager->setShadowTextureSelfShadow(false);
	m_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

    // check if a camera light should be used instead of the input lights
    bool useCameraLight = getBoolValue(CameraLightToggleParameterName);

    // get OGRE texture manager
    Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();

    // destroy all scene nodes and objects in the render scene
    OgreTools::deepDeleteSceneNode(m_sceneManager->getRootSceneNode(), m_sceneManager);

    if (m_cameraSceneNode) {
        OgreContainer *sceneNodeContainer = Ogre::any_cast<OgreContainer *>(m_cameraSceneNode->getUserAny());
        if (sceneNodeContainer)
            delete sceneNodeContainer;
        OgreTools::deepDeleteSceneNode(m_cameraSceneNode, m_sceneManager);
        m_cameraSceneNode->removeAndDestroyAllChildren();
        m_sceneManager->destroySceneNode(m_cameraSceneNode);
        m_cameraSceneNode = 0;
		m_cameraCopy = 0;
    }

    // destroy cameras, lights and entities
    m_sceneManager->destroyAllCameras();
    m_sceneManager->destroyAllEntities();
    m_sceneManager->destroyAllLights();

    // process input geometry
    Ogre::SceneNode *geometrySceneNode = getSceneNodeValue(InputGeometryParameterName);
    if (geometrySceneNode) {
        // duplicate the input geometry
        Ogre::SceneNode *geometrySceneNodeCopy = 0;
        OgreTools::deepCopySceneNode(geometrySceneNode, geometrySceneNodeCopy, m_name, m_sceneManager);
        // add the geometry to the render scene
        m_sceneManager->getRootSceneNode()->addChild(geometrySceneNodeCopy);
    } else
        Log::error("Could not obtain scene node from input geometry parameter.", "HairRenderNode::processOutputImage");

    // process input lights
    if (!useCameraLight) {
#ifdef LIGHT_PARAMETER_AVAILABLE
        // process input lights
        Ogre::SceneNode *lightsSceneNode = getSceneNodeValue(InputLightsParameterName);
        if (lightsSceneNode) {
             duplicate the input lights
            Ogre::SceneNode *lightsSceneNodeCopy = 0;
            OgreTools::deepCopySceneNode(lightsSceneNode, lightsSceneNodeCopy, m_name, m_sceneManager);
             add the lights to the render scene
            m_sceneManager->getRootSceneNode()->addChild(lightsSceneNodeCopy);
        } else
            Log::error("Could not obtain scene node from input lights parameter.", "HairRenderNode::processOutputImage");
#endif
    }

    // process input camera
    Ogre::SceneNode *cameraSceneNode = getSceneNodeValue(InputCameraParameterName);
	Ogre::RenderTexture *renderTargetLight;
	renderTargetLight = 0;

	setValue(m_outputImageName, m_defaultTexture);
#ifdef LIGHT_DEBUG
	setValue(m_outputSecondImageName, m_defaultTexture);
#endif
	
	if (cameraSceneNode) {
        // get the first camera attached to the input camera scene node
        Ogre::Camera *camera = OgreTools::getFirstCamera(cameraSceneNode);
        
		if (camera) {
			// duplicate the input camera
			delete m_cameraSceneNode; 
			m_cameraSceneNode = 0;
			OgreTools::deepCopySceneNode(cameraSceneNode, m_cameraSceneNode, m_name, m_sceneManager);
            if (!m_cameraSceneNode) {
                Log::error("The camera's scene node could not be copied.", "HairRenderNode::processOutputImage");
                return;
            }

            if (useCameraLight && m_lightPositions.empty()) {
                // set up names for camera light objects
                QString cameraLightName = QString("%1CameraLight").arg(m_name);
                //QString cameraLightSceneNodeName = QString("%1SceneNode").arg(cameraLightName);
                // create camera light objects
                Ogre::Light *cameraLight = m_sceneManager->createLight(cameraLightName.toStdString());
                //Ogre::SceneNode *cameraLightSceneNode = m_cameraSceneNode->createChildSceneNode(cameraLightSceneNodeName.toStdString());
                m_cameraSceneNode->attachObject(cameraLight);
            }
			else if (useCameraLight)	
				setNbrLights();

			// process input light
			Ogre::SceneNode *lightSceneNode = getSceneNodeValue(InputLightsParameterName);
			if (lightSceneNode) {
				// get the lights attached to the input light scene node
				Ogre::Light *light = OgreTools::getFirstLight(lightSceneNode);
				if (light) {
					// duplicate the input camera
					m_lightSceneNode = 0;
					OgreTools::deepCopySceneNode(lightSceneNode, m_lightSceneNode, m_name, m_sceneManager);
					if (!m_lightSceneNode) {
						Log::error("The light's scene node could not be copied.", "HairRenderNode::processOutputImage");
						return;
					}
					// set visibility of light gometry representation to false
					Ogre::Entity *entity = OgreTools::getFirstEntity(m_lightSceneNode);
					if (entity)
						entity->setVisible(false);
					// add the lights to the render scene
					m_sceneManager->getRootSceneNode()->addChild(m_lightSceneNode);
				}
			}

            // retrieve render resolution values from custom parameter
/*            const Ogre::Any &customData = dynamic_cast<Ogre::MovableObject *>(camera)->getUserAny();
            if (!customData.isEmpty()) {
                CameraInfo *cameraInfo = Ogre::any_cast<CameraInfo *>(customData);
                if (cameraInfo) {
                    m_renderWidth = cameraInfo->width;
                    m_renderHeight = cameraInfo->height;
                }
            }*/	        

			// copy first camera
			m_cameraCopy = OgreTools::getFirstCamera(m_cameraSceneNode);
			m_cameraCopy->setFarClipDistance(1000.0f);
			m_cameraCopy->setNearClipDistance(.1f);

            // (re-)create the render texture
			initializeRenderTarget(m_cameraCopy, Ogre::ColourValue(1,1,1), Ogre::PF_R8G8B8A8);

			//Ogre::FocusedShadowCameraSetup *setup = new Ogre::FocusedShadowCameraSetup();
			//Ogre::LiSPSMShadowCameraSetup *setup = new Ogre::LiSPSMShadowCameraSetup();
			//setup->setOptimalAdjustFactor(0);
			//Ogre::ShadowCameraSetupPtr shadowCameraSetup = Ogre::ShadowCameraSetupPtr(setup);
			//m_sceneManager->setShadowCameraSetup(shadowCameraSetup);

			if (geometrySceneNode) {
				const int nbrLights = getIntValue("Number of Lights");

				m_sceneManager->setShadowTextureSelfShadow(true);
				//m_sceneManager->setShadowTextureCasterMaterial("HairRender/LightZ");
				m_sceneManager->setShadowTextureSettings(SHADOWTEX_SIZE, nbrLights*CLIPPING_STEPS, Ogre::PF_FLOAT16_RGBA);
				m_sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, CLIPPING_STEPS);
				m_sceneManager->setShadowCasterRenderBackFaces(true);

				m_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
				setCustomVertexShader();

				if (!m_shadowListener)
					m_shadowListener = new ShadowTargetListener();
				
				const unsigned int numShadowRTTs = m_sceneManager->getShadowTextureCount();
				for (unsigned i = 0; i < numShadowRTTs; ++i) {
					Ogre::RenderTarget *target = m_sceneManager->getShadowTexture(i)->getBuffer()->getRenderTarget();
					Ogre::Viewport *vp = target->getViewport(0);
					target->addListener(m_shadowListener);
					vp->setClearEveryFrame(true);
					vp->setAutoUpdated(false);
					vp->update();
				}
			}

			// update render texture and render target
			setValue(m_outputImageName, m_renderTexture);
			// undirty the output parameters to prevent multiple updates
			getParameter(m_outputImageName)->setDirty(false);
#ifdef LIGHT_DEBUG
			setValue(m_outputSecondImageName, m_sceneManager->getShadowTexture(0));
			getParameter(m_outputSecondImageName)->setDirty(false);
#endif
		}
    }
	else
		Log::error("First object attached to scene node contained in input camera parameter is not a camera.", "HairRenderNode::processOutputImage");

		RenderNode::redrawTriggered();
}


//private functions

//!
//! Returns the greatest componen of the vector
//!
//! \return The greatest componen of the vector

void HairRenderNode::redrawTriggered()
{
	RenderNode::redrawTriggered();
}

float HairRenderNode::greatestComponent(const Ogre::Vector3 &vec) const
{
	float returnvalue = vec.x;
	if (returnvalue < vec.y)
		returnvalue = vec.y;
	if (returnvalue < vec.z)
		returnvalue = vec.z;
	return returnvalue;
}

//!
//! Insterts a custom vertex shader to rendering pipeline
//!
void HairRenderNode::setCustomVertexShader()
{
	Ogre::SceneManager::MovableObjectIterator objIter = m_sceneManager->getMovableObjectIterator("Entity");
	while (objIter.hasMoreElements()) {
		Ogre::Entity *entity = static_cast<Ogre::Entity *>(objIter.getNext());
		entity->setCastShadows(false);
		const Ogre::MeshPtr mesh = entity->getMesh();
		Ogre::Mesh::SubMeshNameMap nameMap = mesh->getSubMeshNameMap();
		Ogre::Mesh::SubMeshNameMap::iterator nameMapIter;
		for (nameMapIter = nameMap.begin(); nameMapIter != nameMap.end(); ++nameMapIter) {
			const QString &subMeshName = nameMapIter->first.c_str();
			if (subMeshName.contains("hair", Qt::CaseInsensitive)) {
				Ogre::SubMesh *subMesh = mesh->getSubMesh(nameMapIter->first);
				Ogre::SubEntity *subEntity = entity->getSubEntity(nameMapIter->second);
				entity->setCastShadows(true);
				
				Ogre::MaterialPtr shadowHairMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRender/ShadowHair"));
				subEntity->setMaterial(shadowHairMat);
				Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRender/OpacityMap"));
				matPtr->load();
				matPtr->setTransparencyCastsShadows(true);
				shadowHairMat->getBestTechnique()->setShadowCasterMaterial(matPtr);
				shadowHairMat->setTransparencyCastsShadows(true);
			}
		}
	}
}

//!
//! The less then function for light power sorting
//!
bool HairRenderNode::greaterThan(const QPair<Ogre::Vector3, Ogre::Vector3> &d1, const QPair<Ogre::Vector3, Ogre::Vector3> &d2)
{
	return d1.second.length() > d2.second.length();
}

//protected functions

//!
//! Loads an XML reference data file.
//!
//! Is called when the value of the Reference Data File parameter has
//! changed.
//!
//! \return True if the reference data was successfully loaded from file, otherwise False.
//!
void HairRenderNode::loadLightDescriptionFile ()
{
    QString path = getStringValue("Light Description File");
    QFile inFile(path);

	m_lightPositions.clear();

	// parse the file and fill the pos/pow pair into a list
    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QPair<Ogre::Vector3, Ogre::Vector3> newData(Ogre::Vector3(0,0,0), Ogre::Vector3(0,0,0));
		while (!inFile.atEnd()) {
			QString line = inFile.readLine();
			QString prefix = line.section(" ", 0, 0);
			if (prefix == "Dir:") {
				Ogre::Vector3 pos(
					line.section(" ", 1, 1).toFloat(),
					line.section(" ", 2, 2).toFloat(),
					line.section(" ", 3, 3).toFloat() );
				newData.first = pos;
			}
			else if (prefix == "Pow:") {
				Ogre::Vector3 pow(
					line.section(" ", 1, 1).toFloat(),
					line.section(" ", 2, 2).toFloat(),
					line.section(" ", 3, 3).toFloat() );
				newData.second = pow;
			}
			if (newData.first.length() != 0 && newData.second.length() != 0) {
				m_lightPositions.append(newData);
				newData = QPair<Ogre::Vector3, Ogre::Vector3>(Ogre::Vector3(0,0,0), Ogre::Vector3(0,0,0));
			}
		}
    } else
        Log::error(QString("Document import failed. \"%1\" not found.").arg(path), "LightDescriptionNode::loadReferenceDataFile");
    
	inFile.close();

	// sort the list: greatest pow first
	qSort(m_lightPositions.begin(), m_lightPositions.end(), greaterThan);

	NumberParameter *nbrLights = getNumberParameter("Number of Lights");
	nbrLights->setMaxValue(m_lightPositions.length());
	processOutputImage();
}

//!
//! Sets the scene light scale value
//!
void HairRenderNode::setLightScale ()
{
	m_lightScale = getDoubleValue("Light Scale Pos");
	setNbrLights();
}

//!
//! Sets the number of lights for the sadow calculation
//!
void HairRenderNode::setNbrLights()
{
	const int nbrLights = getIntValue("Number of Lights");

	m_sceneManager->destroyAllLights();

	unsigned int count = 0;
	const float max = greatestComponent(m_lightPositions.first().second);
	for (QList<QPair<Ogre::Vector3, Ogre::Vector3>>::iterator iter = m_lightPositions.begin(); iter != m_lightPositions.end(); ++iter, ++count) {
		const Ogre::Vector3 pos = iter->first * m_lightScale;
		const float length = (iter->second / max).length();
		Ogre::Light *light = m_sceneManager->createLight("Light" + QString::number(count).toStdString() );
		light->setType(Ogre::Light::LT_SPOTLIGHT);
		light->setPosition(pos);
		light->setSpotlightRange(Ogre::Degree(70), Ogre::Degree(90));
		light->setAttenuation(100, 1, 1, 1);
		light->setDirection(-pos);
		light->setDiffuseColour(length, length, length);
		light->setSpecularColour(0,0,0);
		light->setCastShadows(true);
		if (m_lightSceneNode)
			m_lightSceneNode->attachObject(light);
		if (count == nbrLights-1) break;
	}

	m_sceneManager->setShadowTextureCount(nbrLights);
	redrawTriggered();
}

//!
//! Sets the scenes shader parameter
//!
void HairRenderNode::setShaderParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
    const float value = parameter->getValue().toFloat();
	
	Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRender/ShadowHair"));
	Ogre::GpuProgramParametersSharedPtr params = matPtr->getTechnique(0)->getPass(1)->getFragmentProgramParameters();

	if (!params->_findNamedConstantDefinition(name))
		params = matPtr->getTechnique(0)->getPass(1)->getVertexProgramParameters();
	if (params->_findNamedConstantDefinition(name))
		params->setNamedConstant(name, value);

	redrawTriggered();
}

void HairRenderNode::setPassDepthBias()
{
	if (!m_shadowListener)
		return;

	Parameter* parameter = static_cast<Parameter*>(sender());
	const float value = parameter->getValue().toFloat();

	m_shadowListener->setDepthBias(Ogre::Real(value));
	redrawTriggered();
}

void HairRenderNode::setPassDepthOffset()
{
	if (!m_shadowListener)
		return;

	Parameter* parameter = static_cast<Parameter*>(sender());
	const float value = parameter->getValue().toFloat();

	m_shadowListener->setDepthOffset(Ogre::Real(value));
	redrawTriggered();
}

} // namespace HairRenderNode 
