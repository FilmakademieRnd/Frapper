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
//! \file "ShadowMapHairRenderNode.cpp"
//! \brief Implementation file for ShadowMapHairRenderNode class.
//!
//! \author     Markus Rapp <markus.rapp@filmakademie.de>
//! \version    1.0
//! \date       11.12.2014 (last updated)
//!

#include "ShadowMapHairRenderNode.h"
#include "GeometryRenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreManager.h"
#include <QtCore/QFile>

namespace ShadowMapHairRenderNode {
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
//! Constructor of the ShadowMapHairRenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ShadowMapHairRenderNode::ShadowMapHairRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryRenderNode(name, parameterRoot),
	m_lightScale(30)
{
	setValue(CameraLightToggleParameterName, false);
	if (m_sceneManager) {
		//ShadowListener *shadowListener = new ShadowListener();
		//m_sceneManager->addListener(shadowListener);
        m_sceneManager->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));
	}

	// Set the change function for light description file parameter.
    setChangeFunction("Light Description File", SLOT(loadLightDescriptionFile()));
    setChangeFunction("Light Scale Pos", SLOT(setLightScale()));
	setChangeFunction("Number of Lights", SLOT(setNbrLights()));
    setChangeFunction("lightPower", SLOT(setShaderParameter()));
    setChangeFunction("vsmEpsilon", SLOT(setShaderParameter()));
    setChangeFunction("sampleRadius", SLOT(setShaderParameter()));
    setChangeFunction("blurSize", SLOT(setShaderParameter()));
    setChangeFunction("scaleBias", SLOT(setShaderParameter()));
 
	m_materialSwitchListener = new MaterialSwitcher();
	Ogre::MaterialManager::getSingleton().addListener( m_materialSwitchListener );	

#ifdef LIGHT_DEBUG
	m_outputSecondImageName = QString("Light Map");
#endif

	Parameter *outputImageParameter = getParameter(m_outputImageName);
    if (outputImageParameter) {
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
        outputImageParameterLight->addAffectingParameter(getParameter(InputGeometryParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(InputLightsParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(InputCameraParameterName));
        outputImageParameterLight->addAffectingParameter(getParameter(CameraLightToggleParameterName));
		outputImageParameterLight->addAffectingParameter(getParameter(BackgroundColorParameterName));
    }
#endif
}


//!
//! Destructor of the ShadowMapHairRenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ShadowMapHairRenderNode::~ShadowMapHairRenderNode ()
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
void ShadowMapHairRenderNode::processOutputImage ()
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
						light->setAttenuation(50,1,0,0);
					}
				}
			}
		}
		else if (useCameraLight || m_lightPositions.empty()) {
			// set up names for camera light objects
			QString cameraLightName = QString("%1CameraLight").arg(m_name);
			// create camera light objects
			m_sceneManager->destroyAllLights();
			Ogre::Light *cameraLight = m_sceneManager->createLight(cameraLightName.toStdString());
			cameraLight->setType(Ogre::Light::LT_SPOTLIGHT);
			cameraLight->setSpotlightRange(Ogre::Degree(80), Ogre::Degree(90), 1);
			cameraLight->setDirection(-m_cameraSceneNode->getPosition());
			cameraLight->setCastShadows(true);
			m_cameraSceneNode->attachObject(cameraLight);
		}
		else
			setNbrLights();

		// copy first camera
		m_camera = OgreTools::getFirstCamera(m_cameraSceneNode);
		m_camera->setFarClipDistance(2000.0f);
		m_camera->setNearClipDistance(.1f);

		// (re-)create the render texture
		initializeRenderTarget(m_camera, Ogre::ColourValue(1.0f,1.0f,1.0f), Ogre::PF_R8G8B8);
		m_viewport->setMaterialScheme("Shadow");

		Ogre::FocusedShadowCameraSetup *setup = new Ogre::FocusedShadowCameraSetup();
		//Ogre::LiSPSMShadowCameraSetup *setup = new Ogre::LiSPSMShadowCameraSetup();
		//setup->setOptimalAdjustFactor(0.2);
		//setup->setCameraLightDirectionThreshold(Ogre::Degree(30));
		Ogre::ShadowCameraSetupPtr shadowCameraSetup = Ogre::ShadowCameraSetupPtr(setup);
		m_sceneManager->setShadowCameraSetup(shadowCameraSetup);

		if (m_geometrySceneNode) {
			const int nbrLights = getIntValue("Number of Lights");

			m_sceneManager->setShadowTextureSelfShadow(true);
			m_sceneManager->setShadowTextureCasterMaterial("ShadowMapHairRender/HairLightZ");
			m_sceneManager->setShadowTextureSettings(512, nbrLights, Ogre::PF_FLOAT32_GR);
			m_sceneManager->setShadowCasterRenderBackFaces(false);

			const unsigned numShadowRTTs = m_sceneManager->getShadowTextureCount();
			for (unsigned i = 0; i < numShadowRTTs; ++i) {
				Ogre::TexturePtr tex = m_sceneManager->getShadowTexture(i);
				Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
				vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
				vp->setClearEveryFrame(true);
			}

			setHairShader();

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
//! Set shadow hair shader material and caster material. Sets textures and variables of tessellation control and evaluation shader.
//!
void ShadowMapHairRenderNode::setHairShader()
{
	Ogre::MaterialPtr shadowHairMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("ShadowMapHairRender/HairShadow"));
	Ogre::MaterialPtr shadowHairCastMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("ShadowMapHairRender/HairLightZ"));

	Ogre::SceneManager::MovableObjectIterator objIter = m_sceneManager->getMovableObjectIterator("Entity");
	while (objIter.hasMoreElements()) {
		Ogre::Entity *entity = static_cast<Ogre::Entity *>(objIter.getNext());

		const QString &entityName = entity->getName().c_str();
		if(entityName.contains("CustomHairEntity", Qt::CaseInsensitive))
		{
			// set shadow map hair shader
			if(entity->getNumSubEntities() > 0)
			{
				Ogre::SubEntity *subEntity = entity->getSubEntity(0);

				if(subEntity)
				{
					Ogre::Pass *passShadowHairMat0 = shadowHairMat->getTechnique(0)->getPass(0);
					Ogre::Pass *passShadowHairMat1 = shadowHairMat->getTechnique(0)->getPass(1);
					Ogre::Pass *passShadowHairCastMat = shadowHairCastMat->getBestTechnique(0)->getPass(0);
					Ogre::Pass *passHairMaterial = subEntity->getMaterial()->getTechnique(0)->getPass(0);

					// shader attributes
					Ogre::GpuProgramParametersSharedPtr gpuHairHullParam = passHairMaterial->getTessellationHullProgramParameters();
					Ogre::GpuProgramParametersSharedPtr gpuHairDomainParam = passHairMaterial->getTessellationDomainProgramParameters();

					// Set texture pointers of shadow hair materials
					Ogre::Pass::TextureUnitStateIterator texUnitStateIter = passHairMaterial->getTextureUnitStateIterator();
					int texturePosition = 0;
					while (texUnitStateIter.hasMoreElements()) {
						Ogre::TextureUnitState* texUnitState = texUnitStateIter.getNext();
						const Ogre::TexturePtr &texturePtr = texUnitState->_getTexturePtr();
						
						passShadowHairMat0->getTextureUnitState(texturePosition + 1)->setTexture(texturePtr);
						passShadowHairMat1->getTextureUnitState(texturePosition + 1)->setTexture(texturePtr);
						passShadowHairCastMat->getTextureUnitState(texturePosition)->setTexture(texturePtr);

						texturePosition++;
					}

					// Set hull/control parameters
					Ogre::GpuProgramParametersSharedPtr gpuHullParamHairMat0 = passShadowHairMat0->getTessellationHullProgramParameters();
					Ogre::GpuProgramParametersSharedPtr gpuHullParamHairMat1 = passShadowHairMat1->getTessellationHullProgramParameters();
					Ogre::GpuProgramParametersSharedPtr gpuHullParamHairCastMat = passShadowHairCastMat->getTessellationHullProgramParameters();

					Ogre::GpuConstantDefinitionIterator gpuHairHullConstIter = gpuHairHullParam->getConstantDefinitionIterator();
					while( gpuHairHullConstIter.hasMoreElements()) {
						const Ogre::String& paramName = gpuHairHullConstIter.peekNextKey();
						Ogre::GpuConstantDefinition gpuHairHullConst = gpuHairHullConstIter.getNext();

						switch(gpuHairHullConst.constType)
						{
						case Ogre::GpuConstantType::GCT_FLOAT1:
							{
								float value = *gpuHairHullParam->getFloatPointer(gpuHairHullConst.physicalIndex);

								if (gpuHullParamHairMat0->_findNamedConstantDefinition(paramName))
								{
									gpuHullParamHairMat0->setNamedConstant(paramName, value);
								}
								if (gpuHullParamHairMat1->_findNamedConstantDefinition(paramName))
								{
									gpuHullParamHairMat1->setNamedConstant(paramName, value);
								}
								if (gpuHullParamHairCastMat->_findNamedConstantDefinition(paramName))
								{
									gpuHullParamHairCastMat->setNamedConstant(paramName, value);
								}
							}
							break;
						case Ogre::GpuConstantType::GCT_INT1:
							{
								int value = *gpuHairHullParam->getIntPointer(gpuHairHullConst.physicalIndex);

								if (gpuHullParamHairMat0->_findNamedConstantDefinition(paramName))
								{
									gpuHullParamHairMat0->setNamedConstant(paramName, value);
								}
								if (gpuHullParamHairMat1->_findNamedConstantDefinition(paramName))
								{
									gpuHullParamHairMat1->setNamedConstant(paramName, value);
								}
								if (gpuHullParamHairCastMat->_findNamedConstantDefinition(paramName))
								{
									gpuHullParamHairCastMat->setNamedConstant(paramName, value);
								}
							}
							break;
						default:
							break;
						}
					}

					// Set domain/evaluation parameters
					Ogre::GpuProgramParametersSharedPtr gpuDomainParamHairMat0 = passShadowHairMat0->getTessellationDomainProgramParameters();
					Ogre::GpuProgramParametersSharedPtr gpuDomainParamHairMat1 = passShadowHairMat0->getTessellationDomainProgramParameters();
					Ogre::GpuProgramParametersSharedPtr gpuDomainParamHairCastMat = passShadowHairMat0->getTessellationDomainProgramParameters();
					
					Ogre::GpuConstantDefinitionIterator gpuHairDomainConstIter = gpuHairDomainParam->getConstantDefinitionIterator();
					while( gpuHairDomainConstIter.hasMoreElements()) {
						const Ogre::String& paramName = gpuHairDomainConstIter.peekNextKey();
						Ogre::GpuConstantDefinition gpuHairDomainConst = gpuHairDomainConstIter.getNext();

						switch(gpuHairDomainConst.constType)
						{
						case Ogre::GpuConstantType::GCT_FLOAT1:
							{
								float value = *gpuHairDomainParam->getFloatPointer(gpuHairDomainConst.physicalIndex);

								if (gpuDomainParamHairMat0->_findNamedConstantDefinition(paramName))
								{
									gpuDomainParamHairMat0->setNamedConstant(paramName, value);
								}
								if (gpuDomainParamHairMat1->_findNamedConstantDefinition(paramName))
								{
									gpuDomainParamHairMat1->setNamedConstant(paramName, value);
								}
								if (gpuDomainParamHairCastMat->_findNamedConstantDefinition(paramName))
								{
									gpuDomainParamHairCastMat->setNamedConstant(paramName, value);
								}
							}
							break;
						case Ogre::GpuConstantType::GCT_INT1:
							{
								int value = *gpuHairDomainParam->getIntPointer(gpuHairDomainConst.physicalIndex);

								if (gpuDomainParamHairMat0->_findNamedConstantDefinition(paramName))
								{
									gpuDomainParamHairMat0->setNamedConstant(paramName, value);
								}
								if (gpuDomainParamHairMat1->_findNamedConstantDefinition(paramName))
								{
									gpuDomainParamHairMat1->setNamedConstant(paramName, value);
								}
								if (gpuDomainParamHairCastMat->_findNamedConstantDefinition(paramName))
								{
									gpuDomainParamHairCastMat->setNamedConstant(paramName, value);
								}
							}
							break;
						default:
							break;
						}
					}

					// Set caster and shadow material
					entity->setCastShadows(true);
					subEntity->setMaterial(shadowHairMat);
					shadowHairCastMat->load();
					shadowHairMat->getBestTechnique()->setShadowCasterMaterial(shadowHairCastMat);
				}
			}
		}
	}
}

//!
//! Returns the greatest componen of the vector
//!
//! \return The greatest componen of the vector

void ShadowMapHairRenderNode::redrawTriggered()
{
	RenderNode::redrawTriggered();

#ifdef LIGHT_DEBUG
	getParameter(m_outputSecondImageName)->setAuxDirty(false);
#endif
}

float ShadowMapHairRenderNode::greatestComponent(const Ogre::Vector3 &vec) const
{
    return qMax<float>( qMax<float>(vec.x, vec.y), vec.z);
}


//!
//! The less then function for light power sorting
//!
bool ShadowMapHairRenderNode::greaterThan(const QPair<Ogre::Vector3, Ogre::Vector3> &d1, const QPair<Ogre::Vector3, Ogre::Vector3> &d2)
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
void ShadowMapHairRenderNode::loadLightDescriptionFile ()
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
void ShadowMapHairRenderNode::setLightScale ()
{
	m_lightScale = getDoubleValue("Light Scale Pos");
	setNbrLights();
}

//!
//! Sets the number of lights for the sadow calculation
//!
void ShadowMapHairRenderNode::setNbrLights()
{
	if (m_lightSceneNode)
		return;

	if (m_lightPositions.empty())
		return;

	Ogre::SceneNode *rootSceneNode = m_sceneManager->getRootSceneNode();

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
		light->setAttenuation(100.f, .5f, 1.f, 1.f);
		light->setDirection(-pos);
		light->setDiffuseColour(length, length, length);
		light->setSpecularColour(0,0,0);
		light->setCastShadows(true);
		rootSceneNode->attachObject(light);
		if (count == nbrLights-1) break;
	}

	m_sceneManager->setShadowTextureCount(nbrLights);
	redrawTriggered();
}

//!
//! Sets the scenes shader parameter
//!
void ShadowMapHairRenderNode::setShaderParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
    const float value = parameter->getValue().toFloat();

	Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("ShadowMapHairRender/HairShadow"));
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
	Ogre::MaterialPtr baseMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("ShadowMapHairRender/HairShadow"));
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

} // namespace ShadowMapHairRenderNode 
