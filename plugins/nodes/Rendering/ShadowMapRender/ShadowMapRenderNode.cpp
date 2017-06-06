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
//! \file "ShadowMapRenderNode.cpp"
//! \brief Implementation file for ShadowMapRenderNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       11.10.2010 (last updated)
//!

#include "ShadowMapRenderNode.h"
#include "GeometryRenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreManager.h"
#include <QtCore/QFile>

namespace ShadowMapRenderNode {
using namespace Frapper;

void ShadowListener::shadowTextureCasterPreViewProj (Ogre::Light *light, Ogre::Camera *camera, size_t iteration)
{
	float range = light->getAttenuationRange();
    camera->setNearClipDistance(range * 0.01);
    camera->setFarClipDistance(range);
}

///
/// Constructors and Destructors
///

//!
//! Constructor of the ShadowMapRenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ShadowMapRenderNode::ShadowMapRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryRenderNode(name, parameterRoot)
{
	setValue(CameraLightToggleParameterName, false);
	if (m_sceneManager) {
		//ShadowListener *shadowListener = new ShadowListener();
		//m_sceneManager->addListener(shadowListener);
        m_sceneManager->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));
	}

	// Set the change function for light description file parameter.
    /*setChangeFunction("lightPower", SLOT(setShaderParameter()));
    setChangeFunction("vsmEpsilon", SLOT(setShaderParameter()));
    setChangeFunction("sampleRadius", SLOT(setShaderParameter()));
    setChangeFunction("blurSize", SLOT(setShaderParameter()));
    setChangeFunction("scaleBias", SLOT(setShaderParameter()));*/
 
	m_materialSwitchListener = new MaterialSwitcher();
	Ogre::MaterialManager::getSingleton().addListener( m_materialSwitchListener );	

#ifdef LIGHT_DEBUG
	m_outputSecondImageName = QString("Light Map");
#endif

#ifdef LIGHT_DEBUG
	addOutputParameter(Parameter::createImageParameter(m_outputSecondImageName));

	Parameter *outputImageParameterLight = getParameter(m_outputSecondImageName);
    if (outputImageParameterLight) {
        outputImageParameterLight->setProcessingFunction(SLOT(processOutputImage()));
		outputImageParameterLight->setAuxProcessingFunction(SLOT(redrawTriggered()));
        outputImageParameterLight->setDirty(true);
		// set up parameter affections
        outputImageParameterLight->addAffectingParameter(getParameter(InputGeometryParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(InputLightsParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(InputCameraParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(CameraLightToggleParameterName));
		outputImageParameterLight->addAffectingParameter(getParameter(BackgroundColorParameterName));
    }
#endif
}


//!
//! Destructor of the ShadowMapRenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ShadowMapRenderNode::~ShadowMapRenderNode ()
{
	m_sceneManager->setShadowTextureSelfShadow(false);
	m_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
	Ogre::MaterialManager::getSingleton().removeListener(m_materialSwitchListener);	
}


///
/// Public Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void ShadowMapRenderNode::processOutputImage ()
{
	m_sceneManager->setShadowTextureSelfShadow(false);
	m_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

	GeometryRenderNode::processOutputImage();

	// check if a camera light should be used instead of the input lights
	bool useCameraLight = getBoolValue(CameraLightToggleParameterName);

	// process input camera
	if (m_cameraSceneNode && m_geometrySceneNode) {

		if (m_lightSceneNode) {
			if (OgreTools::getFirstLight(m_lightSceneNode)) {
				m_lightSceneNode->setInheritScale(false);
				m_lightSceneNode->setInheritOrientation(false);
				// add the lights to the render scene
				// enable cast shadows
				Ogre::SceneNode::ObjectIterator lightIter = m_lightSceneNode->getAttachedObjectIterator();
				while (lightIter.hasMoreElements()) {
					Ogre::Light *light = dynamic_cast<Ogre::Light *>(lightIter.getNext());
					if (light) {
						light->setCastShadows(true);
						light->setAttenuation(200,1,0,0);
					}
				}
			}
		}

		// copy first camera
		m_camera = OgreTools::getFirstCamera(m_cameraSceneNode);
		m_camera->setFarClipDistance(2000.0f);
		m_camera->setNearClipDistance(.1f);

		// (re-)create the render texture
		initializeRenderTarget(m_camera, Ogre::ColourValue(1.0f,1.0f,1.0f));
		m_viewport->setMaterialScheme("Shadow");

		Ogre::FocusedShadowCameraSetup *setup = new Ogre::FocusedShadowCameraSetup();
		//Ogre::LiSPSMShadowCameraSetup *setup = new Ogre::LiSPSMShadowCameraSetup();
		//setup->setOptimalAdjustFactor(0.000002);
		//setup->setCameraLightDirectionThreshold(Ogre::Degree(30));
		Ogre::ShadowCameraSetupPtr shadowCameraSetup = Ogre::ShadowCameraSetupPtr(setup);
		m_sceneManager->setShadowCameraSetup(shadowCameraSetup);

		if (m_geometrySceneNode) {
			const int nbrLights = getIntValue("Number of Lights");

			m_sceneManager->setShadowTextureSelfShadow(true);
			m_sceneManager->setShadowTextureCasterMaterial("ShadowMapRender/LightZ");
			m_sceneManager->setShadowTextureSettings(512, nbrLights, Ogre::PF_FLOAT32_GR);
			m_sceneManager->setShadowCasterRenderBackFaces(false);

			const unsigned numShadowRTTs = m_sceneManager->getShadowTextureCount();
			for (unsigned i = 0; i < numShadowRTTs; ++i) {
				Ogre::TexturePtr tex = m_sceneManager->getShadowTexture(i);
				Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
				vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
				vp->setClearEveryFrame(true);
			}
			m_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		}

		// update render texture and render target
		setValue(m_outputImageName, m_renderTexture);
		getParameter(m_outputImageName)->setDirty(false);

#ifdef LIGHT_DEBUG
		setValue(m_outputSecondImageName, m_sceneManager->getShadowTexture(0));
		getParameter(m_outputSecondImageName)->setDirty(false);
#endif
	}

	RenderNode::redrawTriggered();
}


//private functions

//!
//! Returns the greatest componen of the vector
//!
//! \return The greatest componen of the vector

void ShadowMapRenderNode::redrawTriggered()
{
	RenderNode::redrawTriggered();

#ifdef LIGHT_DEBUG
	getParameter(m_outputSecondImageName)->setAuxDirty(false);
#endif
}

//!
//! Sets the scenes shader parameter
//!
void ShadowMapRenderNode::setShaderParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
    const float value = parameter->getValue().toFloat();

	Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("ShadowMapRender/Shadow"));
	Ogre::GpuProgramParametersSharedPtr params = matPtr->getTechnique(0)->getPass(1)->getFragmentProgramParameters();

	if (!params->_findNamedConstantDefinition(name))
		params = matPtr->getTechnique(0)->getPass(1)->getVertexProgramParameters();
	if (!params->_findNamedConstantDefinition(name))
		return;

	params->setNamedConstant(name, value);

	redrawTriggered();
}

///
/// Constructors and Destructors
///

//!
//! Constructor of the MaterialSwitcher class.
//!
MaterialSwitcher::MaterialSwitcher () :
    m_shadowTechnique(0)
{
	Ogre::MaterialPtr baseMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("ShadowMapRender/Shadow"));
	if (!baseMat.isNull()) {
	    baseMat->load();
	    m_shadowTechnique = baseMat->getBestTechnique();
    }
}

//!
//! Destructor of the MaterialSwitcher class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MaterialSwitcher::~MaterialSwitcher ()
{
}


///
/// Public Functions
///
	
Ogre::Technique* MaterialSwitcher::handleSchemeNotFound(unsigned short schemeIndex, 
														const Ogre::String& schemeName,
														Ogre::Material* originalMaterial, 
									                    unsigned short lodIndex,
														const Ogre::Renderable* rend)
{
		if (schemeName == "Shadow")
			return m_shadowTechnique;
		return NULL;
}

} // namespace ShadowMapRenderNode 
