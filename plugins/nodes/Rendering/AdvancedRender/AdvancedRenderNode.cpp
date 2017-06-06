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
//! \file "AdvancedRenderNode.cpp"
//! \brief Implementation file for AdvancedRenderNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Felix Bucella <felix.bucella@filmakademie.de>
//! \version    1.6
//! \date       17.05.2011 (last updated)
//!

#include "AdvancedRenderNode.h"
#include "GeometryRenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreManager.h"

namespace AdvancedRenderNode {
using namespace Frapper;

//#define USE_WRINKLEMAPPING

///
/// Constructors and Destructors
///


//!
//! Constructor of the AdvancedRenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
AdvancedRenderNode::AdvancedRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryRenderNode(name, parameterRoot),
	m_multiRenderTargetName(""),
	m_blendWeightTextureName(""),
	m_multiRenderTarget(0)
{
	if (m_sceneManager) {
#ifdef USE_WRINKLEMAPPING
		m_blendWeightTextureName = createUniqueName("BlendWeightTexture");
#endif
		m_multiRenderTargetName = createUniqueName("MRT");
	}

	m_materialSwitchListener = new MaterialSwitcher();
	Ogre::MaterialManager::getSingleton().addListener(m_materialSwitchListener);

	// manually create textue names we'll need
	m_outParameterNameList.append(QString("Normal Map"));
	m_outParameterNameList.append(QString("Depth Map"));
	m_outParameterNameList.append(QString("View Map"));
    m_outParameterNameList.append(QString("Object Id Map"));

	setChangeFunction("Enable Hardware Skinning", SLOT(EnableHardwareSkinning()));
	setChangeFunction("Entity has advanced render material", SLOT(UseEntityTechnique()));

	Parameter *outputImageParameter = getParameter(m_outputImageName);
	outputImageParameter->setProcessingFunction(SLOT(processOutputImage()));
	outputImageParameter->setDirty(true);

	// add image parameters for shading componet maps
	for (int i=0; i<m_outParameterNameList.size(); ++i) {
		const QString &name = m_outParameterNameList[i];
		outputImageParameter = Parameter::createImageParameter(name);
		addOutputParameter(outputImageParameter);
		outputImageParameter->setProcessingFunction(SLOT(processOutputImage()));
		outputImageParameter->setAuxProcessingFunction(SLOT(redrawTriggered()));
		outputImageParameter->setDirty(true);
		
		// set up parameter affections
        outputImageParameter->addAffectingParameter(getParameter(InputGeometryParameterName));
        outputImageParameter->addAffectingParameter(getParameter(InputLightsParameterName));
        outputImageParameter->addAffectingParameter(getParameter(InputCameraParameterName));
        outputImageParameter->addAffectingParameter(getParameter(CameraLightToggleParameterName));
		outputImageParameter->addAffectingParameter(getParameter(BackgroundColorParameterName));
		outputImageParameter->addAffectingParameter(getParameter("Render Target Multiplier"));
		
		// generate texture
		generateTexture(name, Ogre::TEX_TYPE_2D, m_renderWidth, m_renderHeight, 0, Ogre::PF_FLOAT32_RGB, Ogre::TU_RENDERTARGET);
	}

	setupTextures();

#ifdef WRINKLE_DEBUG
	m_outputWrinkleImageName = "WrinkleWeightMap";
	addOutputParameter(Parameter::createImageParameter(m_outputWrinkleImageName));

	Parameter *outputImageParameterWrinkle = getParameter(m_outputWrinkleImageName);
    if (outputImageParameterWrinkle) {
        outputImageParameterWrinkle->setProcessingFunction(SLOT(processOutputImage()));
		outputImageParameterWrinkle->setAuxProcessingFunction(SLOT(redrawTriggered()));
        outputImageParameterWrinkle->setDirty(true);
		// set up parameter affections
        outputImageParameterWrinkle->addAffectingParameter(getParameter(InputGeometryParameterName));
        outputImageParameterWrinkle->addAffectingParameter(getParameter(InputLightsParameterName));
        outputImageParameterWrinkle->addAffectingParameter(getParameter(InputCameraParameterName));
        outputImageParameterWrinkle->addAffectingParameter(getParameter(CameraLightToggleParameterName));
		outputImageParameterWrinkle->addAffectingParameter(getParameter(BackgroundColorParameterName));
    }
#endif
}


//!
//! Destructor of the AdvancedRenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
AdvancedRenderNode::~AdvancedRenderNode ()
{
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();

	Ogre::MaterialManager::getSingleton().removeListener(m_materialSwitchListener);

#ifdef USE_WRINKLEMAPPING
	if (textureManager.resourceExists(m_blendWeightTextureName)) {
		Ogre::RenderTexture* blendWeightTextureTarget = m_blendWeightTexture->getBuffer()->getRenderTarget();
		blendWeightTextureTarget->removeAllViewports();
		blendWeightTextureTarget->removeAllListeners();
		textureManager.remove(m_blendWeightTextureName);
	}
#endif

	if (m_multiRenderTarget) {
		m_multiRenderTarget->removeAllViewports();
		m_multiRenderTarget->removeAllListeners();
		Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget(m_multiRenderTargetName);
	}
}


///
/// Public Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void AdvancedRenderNode::processOutputImage ()
{
	GeometryRenderNode::processOutputImage();

	for (int i=0; i<m_outParameterNameList.size(); ++i)
		setValue(m_outParameterNameList[i], m_defaultTexture);

    // process input geometry
    Ogre::SceneNode *geometrySceneNode = getSceneNodeValue(InputGeometryParameterName);
    if (geometrySceneNode) {
		UseEntityTechnique();
	} else {
        Log::error("Could not obtain scene node from input geometry parameter.", "AdvancedRenderNode::processOutputImage");
		return;
	}

	// process input camera        
	if (m_cameraSceneNode) {
		setupTextures();
		rebindViewports(OgreTools::getFirstCamera(m_cameraSceneNode));

		// update render texture and render target
		setValue(m_outputImageName, m_renderTexture);

		// update add render textures and render targets
		QList<Ogre::TexturePtr> &textures = m_textureHash.values();
		for (int i=0; i<m_textureHash.size(); ++i)
			setValue(m_outParameterNameList[i], textures[i]);

#ifdef USE_WRINKLEMAPPING
		// Look for possibility to use the SaC-mapping and prepare this feature
		setupWrinkleMaterial();
#endif

		// undirty the output parameters to prevent multiple updates
		for (int i=0; i<m_outParameterNameList.size(); ++i)
			getParameter(m_outParameterNameList[i])->setDirty(false);
	}

	AdvancedRenderNode::redrawTriggered();
}


//!
//! Sets the material for software or hardware skinning.
//!
void AdvancedRenderNode::EnableHardwareSkinning()
{
	Parameter *parameter = getParameter("Enable Hardware Skinning");
	Ogre::MaterialPtr matPtr;
	if (parameter && parameter->getValue().toBool())
		matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("AdvancedRender/MRT_HS"));
	else
		matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("AdvancedRender/MRT"));
	
	if (!matPtr.isNull())
		m_materialSwitchListener->setMat(matPtr);
}


//!
//! Sets the material to the special entity render technique if avaiable.
//!
void AdvancedRenderNode::UseEntityTechnique()
{
	Parameter *parameter = getParameter("Entity has advanced render material");
	if (parameter && parameter->getValue().toBool()) {
		Ogre::SceneNode *geometrySceneNode = getSceneNodeValue(InputGeometryParameterName);
		Ogre::MovableObject *movableObject = OgreTools::getFirstMovableObject(geometrySceneNode);
		Ogre::ManualObject *manualObject = dynamic_cast<Ogre::ManualObject *>(movableObject);
		if (manualObject) {
			for (int i=0; i<manualObject->getNumSections(); ++i) {
				const Ogre::ManualObject::ManualObjectSection *section = manualObject->getSection(i);
				const Ogre::String &sectionMaterialName = section->getMaterialName();
				Ogre::MaterialPtr sectionMaterial = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName("AdvancedRender/" + sectionMaterialName);
				if (!sectionMaterial.isNull()) {
					m_materialSwitchListener->setMat(sectionMaterial);
				}
				break; // break loop if first AdvancedRender material has been found.
			}
		}
		QList<Ogre::Entity *> entityList; 
		OgreTools::getAllEntities(geometrySceneNode, entityList);
		foreach (Ogre::Entity *entity, entityList) {
			if (entity) {
				for (int i=0; i<entity->getNumSubEntities(); i++) {
					const Ogre::SubEntity *subEntity = entity->getSubEntity(i);
					if (subEntity) {
						// get source material
						const Ogre::MaterialPtr subMaterial = subEntity->getMaterial();
						// create new material
						Ogre::MaterialPtr newMaterial = Ogre::MaterialManager::getSingleton().create(createUniqueName(subMaterial->getName().c_str()), "AdvancedRenderNode");
						// copy details from source to new material
						subEntity->getMaterial()->copyDetailsTo(newMaterial);

						// search for source material path
						Ogre::String res_group;
						Ogre::ResourceGroupManager &resourceGroupMgr = Ogre::ResourceGroupManager::getSingleton();
						const Ogre::String &meshName = entity->getMesh()->getName();
						try {
							res_group = resourceGroupMgr.findGroupContainingResource(meshName);
						}
						catch (...) {
							continue;
						}
						const Ogre::FileInfoListPtr fileInfoList = resourceGroupMgr.findResourceFileInfo(res_group, meshName);
						Ogre::String filePath;

						if (!fileInfoList->empty())
							filePath = fileInfoList->at(0).archive->getName();
						else
							return;

						const QString renderSystemName = OgreManager::getRenderSystemName();
						if (renderSystemName != "UNKNOWN")
						{
							// compile Set Shader
							Ogre::String profileFlag;
							Ogre::HighLevelGpuProgramPtr arnFragmentProgram;

							if (renderSystemName.contains("3+")) {
								arnFragmentProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(newMaterial->getName() + "/FragmentProgram", "AdvancedRenderNode", "glsl", Ogre::GPT_FRAGMENT_PROGRAM);
								arnFragmentProgram->setSourceFile(filePath + "/arnFP.glsl");
								arnFragmentProgram->setParameter("syntax", "glsl400"); 
							}
							else {
								arnFragmentProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(newMaterial->getName() + "/FragmentProgram", "AdvancedRenderNode", "cg", Ogre::GPT_FRAGMENT_PROGRAM);
								if (renderSystemName.contains("11")) 		// DX11
									profileFlag = "ps_4_0";
								else if (renderSystemName.contains("9"))	// DX9
									profileFlag = "ps_2_0";
								else 										// GL2 and prv.
									profileFlag = "arbfp1";
								arnFragmentProgram->setSourceFile(filePath + "/arnFP.cg");
								arnFragmentProgram->setParameter("profiles", profileFlag);
								arnFragmentProgram->setParameter("entry_point", "main");
							}

							if (arnFragmentProgram->isSupported()) {
								arnFragmentProgram->load();
								if (arnFragmentProgram->isLoaded())
									newMaterial->getTechnique(0)->getPass(0)->setFragmentProgram(arnFragmentProgram->getName());
							}
							else {
								newMaterial = Ogre::MaterialManager::getSingleton().getByName("AdvancedRender/MRT");
							}
							m_materialSwitchListener->setMat(newMaterial);
						}
					}
				}

			}
		}
	}
	else
		EnableHardwareSkinning();
}


//!
//! Redraw of ogre scene has been triggered.
//!
void AdvancedRenderNode::redrawTriggered ()
{
	if (!m_renderTexture.isNull())
		if (isViewed() || m_outputParameter->isConnected()) {
			m_renderTexture->getBuffer()->getRenderTarget()->update();
			getParameter(m_outputImageName)->setAuxDirty(false);
		}

#ifdef USE_WRINKLEMAPPING
	if (!m_blendWeightTexture.isNull())
		m_blendWeightTexture->getBuffer()->getRenderTarget()->update();
#endif
	
	if (m_multiRenderTarget) {
		m_multiRenderTarget->update();
	
		for (int i=0; i<m_outParameterNameList.size(); ++i)
			getParameter(m_outParameterNameList[i])->setAuxDirty(false);
	}

	//Parameter* timeParameter = getTimeParameter();
	//if (timeParameter)
	//	std::cout << "Advanced Node:" << timeParameter->getValue().toInt() << std::endl; 
}


//!
//! Constructor of the MaterialSwitcher class.
//!
MaterialSwitcher::MaterialSwitcher () :
    m_technique(0),
	m_wrinkleMappingTechnique(0)
{
	Ogre::MaterialPtr mrtMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("AdvancedRender/MRT"));
	if(!mrtMat.isNull())
		setMat(mrtMat);
	else 
		Log::error("%1 Material not found.", "AdvancedRenderNode::MaterialSwitcher");

#ifdef USE_WRINKLEMAPPING
	// Material for SaC-mapping
	Ogre::MaterialPtr blendweightMat = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("Calculate_Blendweights"));
	if(!blendweightMat.isNull()){
		blendweightMat->load();
		m_wrinkleMappingTechnique = blendweightMat->getBestTechnique();
	}
#endif
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
		if (schemeName == "MRTPass")
            return m_technique;

#ifdef USE_WRINKLEMAPPING
		// for SaC-mapping
		if (schemeName == "blendWeightPass")	
			return m_wrinkleMappingTechnique;
#endif

		else return NULL;
}

//!
//! Sets the material for the MRTs.
//!
//! The new material to used for the MRT's.
//!
void MaterialSwitcher::setMat(Ogre::MaterialPtr mat)
{
    if (!mat.isNull()) {
	    mat->load();
	    m_technique = mat->getBestTechnique();
    }
}

//!
//! Check existing material for SaC-mapping shader and set the weight-map texture.
//! 
//! \return True when SaC-mapping material was found.
//!
bool AdvancedRenderNode::setupWrinkleMaterial ()
{
	Ogre::SceneNode *geometrySceneNode = getSceneNodeValue(InputGeometryParameterName);
	Ogre::Entity *myEntity = OgreTools::getFirstEntity(geometrySceneNode);

	Ogre::MeshPtr meshPtr; 

	if (myEntity){
		meshPtr = myEntity->getMesh();
	}
	
	if (!meshPtr.isNull()){

		Ogre::Mesh::SubMeshIterator subMeshIter = meshPtr->getSubMeshIterator();
		
		// find special Material for SaC-mapping and set weight-map as texture
		while (subMeshIter.hasMoreElements()) {
			Ogre::SubMesh* subMesh = subMeshIter.getNext();
			Ogre::String subMeshMaterialName = subMesh->getMaterialName();
			//Log::debug("Found Material - " + QString(subMeshMaterialName.c_str()));
			
			std::string::size_type loc = subMeshMaterialName.find("Wrinkles"); // name of the skin material of the character
			if( loc != std::string::npos ) {
				Ogre::MaterialPtr subMeshMaterial = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(subMeshMaterialName);
				if (!subMeshMaterial.isNull()){
					Log::debug("Found Wrinkles-Material !!!");

					m_blendWeightTexture->getBuffer()->getRenderTarget()->getViewport(0)->setMaterialScheme("blendWeightPass");

					// set weightmap as texture
					subMeshMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(m_blendWeightTextureName);
					Log::debug("Setup weight map " + QString(m_blendWeightTextureName.c_str()) + "as texture for \"Wrinkles\" material");
#ifdef WRINKLE_DEBUG
					setValue(m_outputWrinkleImageName, m_blendWeightTexture);
#endif
				}
				return true;
			}
		}
	}
	return false;
}

//!
//! Setup additional textures (blendweight, multi render target).
//!
void AdvancedRenderNode::setupTextures ()
{
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	Ogre::RenderTarget *blendWeightTextureTarget = 0;

#ifdef USE_WRINKLEMAPPING)
	// setup wrinkle weight map
	if (textureManager.resourceExists(m_blendWeightTextureName)) {
		blendWeightTextureTarget = m_blendWeightTexture->getBuffer()->getRenderTarget();
		blendWeightTextureTarget->removeAllViewports();
		blendWeightTextureTarget->removeAllListeners();
		textureManager.remove(m_blendWeightTextureName);
	}

	m_blendWeightTexture = Ogre::TextureManager::getSingleton().createManual(m_blendWeightTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 256, 256, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);
	blendWeightTextureTarget = m_blendWeightTexture->getBuffer()->getRenderTarget();
	blendWeightTextureTarget->setAutoUpdated(false);
#endif

	// setup multiple render target
	if(m_multiRenderTarget) {
		m_multiRenderTarget->removeAllViewports();
		m_multiRenderTarget->removeAllListeners();
		Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget(m_multiRenderTargetName);
	}

	m_multiRenderTarget = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget(m_multiRenderTargetName);
	m_multiRenderTarget->setAutoUpdated(false);

	size_t count = 0;
	QHash<QString, Ogre::TexturePtr>::iterator texHashIter;
	for (texHashIter = m_textureHash.begin(); texHashIter != m_textureHash.end(); ++texHashIter) {
		Ogre::RenderTexture *target = texHashIter.value()->getBuffer()->getRenderTarget();
		m_multiRenderTarget->bindSurface(count++, target);
	}
}


//!
//! Create new viewports and bind them to the additional textures (blendweight, multi render target).
//!
void AdvancedRenderNode::rebindViewports(Ogre::Camera *camera)
{
#ifdef USE_WRINKLEMAPPING)
	Ogre::RenderTexture *blendWeightTextureTarget = m_blendWeightTexture->getBuffer()->getRenderTarget();
	blendWeightTextureTarget->removeAllViewports();
	
	Ogre::Viewport *viewport = blendWeightTextureTarget->addViewport(camera);
	viewport->setMaterialScheme("blendWeightPass");
	viewport->setClearEveryFrame(true);
	viewport->setOverlaysEnabled(false);
	viewport->setSkiesEnabled(false);
	viewport->setBackgroundColour(Ogre::ColourValue::Black);
#endif

	m_multiRenderTarget->removeAllViewports();
	Ogre::Viewport *multiViewport = m_multiRenderTarget->addViewport(camera);
    multiViewport->setMaterialScheme("MRTPass");
    multiViewport->setClearEveryFrame(true);
    multiViewport->setOverlaysEnabled(false);
	multiViewport->setSkiesEnabled(false);
	multiViewport->setBackgroundColour(Ogre::ColourValue::Black);
}

} // namespace AdvancedRenderNode 
