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

#include "HairRenderNodeAlpha.h"
#include "GeometryRenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreManager.h"
#include <QtCore/QFile>

namespace HairRenderAlphaNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the HairRenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
HairRenderNode::HairRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryRenderNode(name, parameterRoot),
	m_lightScale(60)
{
	if (m_sceneManager) {
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
    setChangeFunction("blurSize", SLOT(setShaderParameter()));
	setChangeFunction("LiSPSMAdjust", SLOT(setShadowParameter()));
	setChangeFunction("CamLightAngle", SLOT(setShadowParameter()));
 
#ifdef LIGHT_DEBUG
	m_outputShadowImageName = QString("Light Map");
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
	addOutputParameter(Parameter::createImageParameter(m_outputShadowImageName));

	Parameter *outputImageParameterLight = getParameter(m_outputShadowImageName);
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
}


///
/// Public Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void HairRenderNode::processOutputImage ()
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
		initializeRenderTarget(m_camera, Ogre::ColourValue(1.0f,1.0f,1.0f), Ogre::PF_L8);

		//Ogre::FocusedShadowCameraSetup *setup = new Ogre::FocusedShadowCameraSetup();
		Ogre::LiSPSMShadowCameraSetup *setup = new Ogre::LiSPSMShadowCameraSetup();
		setup->setOptimalAdjustFactor(0.2);
		setup->setCameraLightDirectionThreshold(Ogre::Degree(30));
		Ogre::ShadowCameraSetupPtr shadowCameraSetup = Ogre::ShadowCameraSetupPtr(setup);
		m_sceneManager->setShadowCameraSetup(shadowCameraSetup);

		if (m_geometrySceneNode) {
			const int nbrLights = getIntValue("Number of Lights");

			m_sceneManager->setShadowTextureSelfShadow(true);
			m_sceneManager->setShadowTextureCasterMaterial("HairRenderAlpha/LightZ");
			m_sceneManager->setShadowTextureSettings(512, nbrLights, Ogre::PF_FLOAT16_GR);
			m_sceneManager->setShadowCasterRenderBackFaces(false);

			const unsigned numShadowRTTs = m_sceneManager->getShadowTextureCount();
			for (unsigned i = 0; i < numShadowRTTs; ++i) {
				Ogre::TexturePtr tex = m_sceneManager->getShadowTexture(i);
				Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
				vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
				vp->setClearEveryFrame(true);
			}
			setCustomVertexShader();
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

void HairRenderNode::redrawTriggered()
{
	RenderNode::redrawTriggered();
#ifdef LIGHT_DEBUG
	getParameter(m_outputShadowImageName)->setAuxDirty(false);
#endif
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
	Ogre::MaterialPtr shadowMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/Shadow"));
	Ogre::MaterialPtr shadowCastMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/LightZ"));

	Ogre::MaterialPtr shadowHairMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/ShadowHair"));
	Ogre::MaterialPtr shadowHairCastMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/LightZHair"));

	Ogre::MaterialPtr shadowLashMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/ShadowLash"));
	Ogre::MaterialPtr shadowLashCastMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/LightZLash"));

	Ogre::MaterialPtr shadowPoseMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/ShadowPose"));
	Ogre::MaterialPtr shadowPoseCastMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/LightZPose"));

	Ogre::MaterialPtr shadowAssMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/ShadowAss"));
	Ogre::MaterialPtr shadowAssCastMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/LightZAss"));

	Ogre::MaterialPtr shadowEyeMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("HairRenderAlpha/ShadowEye"));

	Ogre::SceneManager::MovableObjectIterator objIter = m_sceneManager->getMovableObjectIterator("Entity");
	while (objIter.hasMoreElements()) {
		Ogre::Entity *entity = static_cast<Ogre::Entity *>(objIter.getNext());
		//entity->setMaterial(shadowMat);
		entity->setCastShadows(false);
		const Ogre::MeshPtr mesh = entity->getMesh();
		Ogre::Mesh::SubMeshNameMap nameMap = mesh->getSubMeshNameMap();
		Ogre::Mesh::SubMeshNameMap::iterator nameMapIter;
		for (nameMapIter = nameMap.begin(); nameMapIter != nameMap.end(); ++nameMapIter) {
			const QString &subMeshName = nameMapIter->first.c_str();
			Ogre::SubMesh *subMesh = mesh->getSubMesh(nameMapIter->first);
			Ogre::SubEntity *subEntity = entity->getSubEntity(nameMapIter->second);
			if (subMeshName.contains("hair", Qt::CaseInsensitive)) {
				entity->setCastShadows(true);				
				subEntity->setMaterial(shadowHairMat);
				shadowHairCastMat->load();
				shadowHairMat->getBestTechnique()->setShadowCasterMaterial(shadowHairCastMat);
			}
			else if (subMeshName.contains("base", Qt::CaseInsensitive)){
				entity->setCastShadows(true);	
				subEntity->setMaterial(shadowPoseMat);
				shadowPoseCastMat->load();
				shadowPoseMat->getBestTechnique()->setShadowCasterMaterial(shadowPoseCastMat);
			}
			else if (subMeshName.contains("eye", Qt::CaseInsensitive)){
				entity->setCastShadows(true);	
				subEntity->setMaterial(shadowEyeMat);
				shadowAssCastMat->load();
				shadowEyeMat->getBestTechnique()->setShadowCasterMaterial(shadowAssCastMat);
			}
			else if (subMeshName.contains("lash", Qt::CaseInsensitive)){
				entity->setCastShadows(true);	
				subEntity->setMaterial(shadowLashMat);
				shadowLashCastMat->load();
				shadowLashMat->getBestTechnique()->setShadowCasterMaterial(shadowLashCastMat);
			}
			else if (subMeshName.contains("brow", Qt::CaseInsensitive) ||
					 subMeshName.contains("bg", Qt::CaseInsensitive) ||
					 subMeshName.contains("tess", Qt::CaseInsensitive)){
				subEntity->setVisible(false);
			}
			else if (subMeshName.contains("logo", Qt::CaseInsensitive) ||
					 subMeshName.contains("teeth", Qt::CaseInsensitive)) {
				entity->setCastShadows(false);
				subEntity->setMaterial(shadowAssMat);
			}
			else {
				entity->setCastShadows(true);	
				subEntity->setMaterial(shadowMat);
				shadowCastMat->load();
				shadowMat->getBestTechnique()->setShadowCasterMaterial(shadowCastMat);
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
	//processOutputImage();
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
//! Sets the number of lights for the shadow calculation
//!
void HairRenderNode::setNbrLights()
{
	if (m_lightSceneNode || m_lightPositions.empty())
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
		light->setSpotlightRange(Ogre::Degree(60), Ogre::Degree(80));
		light->setDirection(-pos);
		light->setDiffuseColour(length, length, length);
		light->setSpecularColour(0,0,0);
		light->setCastShadows(true);
		rootSceneNode->attachObject(light);
		if (count == nbrLights-1)
			break;
	}

	m_sceneManager->setShadowTextureCount(nbrLights);

	redrawTriggered();
}

//!
//! Sets the scenes shadow parameters
//!
void HairRenderNode::setShadowParameter()
{
	Ogre::LiSPSMShadowCameraSetup *setup = new Ogre::LiSPSMShadowCameraSetup();
	setup->setOptimalAdjustFactor(getFloatValue("LiSPSMAdjust")/10000.0f);
	setup->setCameraLightDirectionThreshold(Ogre::Degree(getFloatValue("CamLightAngle")));

	Ogre::ShadowCameraSetupPtr shadowCameraSetup = Ogre::ShadowCameraSetupPtr(setup);
	m_sceneManager->setShadowCameraSetup(shadowCameraSetup);

	redrawTriggered();
}

//!
//! Sets the scenes shader parameters
//!
void HairRenderNode::setShaderParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
    const float value = parameter->getValue().toFloat();

	setParameter("HairRenderAlpha/Shadow", name, value);
	setParameter("HairRenderAlpha/ShadowHair", name, value);
	setParameter("HairRenderAlpha/ShadowEye", name, value);
	setParameter("HairRenderAlpha/ShadowPose", name, value);
	setParameter("HairRenderAlpha/ShadowLash", name, value);
	//setParameter("HairRenderAlpha/ShadowAss", name, value);

	redrawTriggered();
}

//!
//! Sets the shader parameter in the shadow material
//!
//! \param matName The name of the material
//! \param paramName The name of the parameter to be set.
//! \param value The vamue of the parameter to be set.
//!
void HairRenderNode::setParameter(const Ogre::String &matName, const Ogre::String &paramName, const float value)
{
	const Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName(matName));
	if (!matPtr.isNull()) {
		Ogre::GpuProgramParametersSharedPtr params = matPtr->getTechnique(0)->getPass(1)->getFragmentProgramParameters();

		if (!params->_findNamedConstantDefinition(paramName))
			params = matPtr->getTechnique(0)->getPass(1)->getVertexProgramParameters();
		if (params->_findNamedConstantDefinition(paramName))
			params->setNamedConstant(paramName, value);
	}
}

} // namespace HairRenderAlphaNode 
