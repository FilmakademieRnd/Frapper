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
//! \file "TextureGeometry.h"
//! \brief Implementation file for TextureGeometry class.
//!
//! \author     Amit Rojtblat <amit.rojtblat@filmakademie.de>
//! \version    1.0
//! \date       22.02.2014 (last updated)
//!

#include "TextureGeometryNode.h"
#include "OgreManager.h"
#include "OgreTools.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>

namespace Frapper{

///
/// Constructors and Destructors
///

//!
//! Constructor of the TextureGeometry class.
//!
//! \param name The name to give the new TextureGeometry node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TextureGeometryNode::TextureGeometryNode ( const QString &name, ParameterGroup *parameterRoot)
	:TextureGeometryNodeAbstract(name, parameterRoot),
	m_PrivateSceneManager(0),
	m_camera(0),
	m_enumImageParameter(0){

	// create the private scene manager
	m_PrivateSceneManager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC,name.toStdString()+"_privateSM");

	// add the dummy object
	Ogre::SceneNode *root =  m_PrivateSceneManager->getRootSceneNode();
	root->addChild(getSceneNode());

	//create enum parameter to choose which image to view
	m_enumImageParameter = new EnumerationParameter("outputDisplayImage_"+name,0);
	m_enumImageParameter->setNode(this);
	parameterRoot->addParameter(m_enumImageParameter);

}

//!
//! Destructor of the TextureGeometry class.
//!
TextureGeometryNode::~TextureGeometryNode ()
{

	//destroy all the render targets and their textures and cameras
	destroyTextureTargets();

}

///
/// Public functions
///

//!
//! Creates a new Ogre::TexturePtr and assignes it to an output parameter
//!
//! \param parentNode the node that will process this texture target
//! \param name base name to give to the texture. Will be made unique and
//! \param resolution the resolution of the texture (uniform square)
//! \param format the format of the OgreTexturePtr
//! \param points per primitive how many points can the geo shader of thi
//! \param flatMaterial the material to use for calculating the output te
//! \param actualMaterial the material to use for actual screen shading
//!
Parameter* TextureGeometryNode::addOutputRenderTargetImage (	const QString& name,
														const QString& parameterName,
														int resolution, 
														Ogre::PixelFormat format, 
														int pointsPerPrimitive, 
														Ogre::MaterialPtr& material,
														Ogre::MultiRenderTarget* mrt){

	// init the camera of the target
	if(!m_camera) initCamera();

	// init the texture target
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual(
													name.toStdString(),
													Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													Ogre::TEX_TYPE_2D, 
													resolution, resolution,
													0,
													format,
													Ogre::TU_RENDERTARGET);
	Ogre::RenderTexture *renderTexture = texture->getBuffer()->getRenderTarget();
	/*
	Ogre::TexturePtr texture2 = Ogre::TextureManager::getSingleton().createManual(
													name.toStdString()+"_MRTtest",
													Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
													Ogre::TEX_TYPE_2D, 
													resolution, resolution,
													0,
													format,
													Ogre::TU_RENDERTARGET);
	Ogre::RenderTexture *renderTexture2 = texture2->getBuffer()->getRenderTarget();
	*/
	renderTexture->setAutoUpdated(false);
	//renderTexture2->setAutoUpdated(false);

	
	Ogre::Root::getSingleton().getRenderSystem()->setDepthBufferFor(renderTexture);
	//Ogre::Root::getSingleton().getRenderSystem()->setDepthBufferFor(renderTexture2);

	//mrt test
	// init the texture target

	if(!mrt){
		mrt = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget("MRT_"+QString::number(m_multiRenderTargetsList.size()).toStdString());
		mrt->setAutoUpdated(false);
		m_multiRenderTargetsList.append(mrt);
	}

	int boundTargetId = addRenderTargetToMRT(mrt,renderTexture);

	//create the parameter
	Parameter* newTargetParameter = Parameter::createImageParameter(parameterName,texture);
	addOutputParameter(newTargetParameter);
	newTargetParameter->setProcessingFunction(SLOT(updateOutput()));

	// set the material for this output
	m_outputParameterToMaterialMap[newTargetParameter->getName()] = material;

	// set the mrt for the parameter
	m_outputParameterRenderTargetMap[newTargetParameter->getName()] = mrt;

	// add the image to the list of outputs to select from in the enumeration parameter
	QStringList values = m_enumImageParameter->getValues();
	QStringList literals = m_enumImageParameter->getLiterals();
	values.append(newTargetParameter->getName());
	literals.append(name);
	m_enumImageParameter->setValues(values);
	m_enumImageParameter->setLiterals(literals);

	// add the material to be affected by count
	setAffectedBycount(material);

	newTargetParameter->addAffectingParameter(getParameter("pointCount"));
	newTargetParameter->addAffectingParameter(getParameter("maxPointCount"));

	//setup the target data object and add to map
	targetData* newTargetData = new targetData;
	newTargetData->texture=texture;
	newTargetData->mrt=mrt;
	newTargetData->mrtBoundnumber=boundTargetId;
	newTargetData->parameter=newTargetParameter;
	newTargetData->target=renderTexture;
	m_textureToTargetDataMap[renderTexture]=newTargetData;

	return newTargetParameter;

}

//!
//! Add an input texture (textureUnitState) to output material
//!
Ogre::TextureUnitState* TextureGeometryNode::addInputTextureToOutputTarget(const Ogre::TexturePtr& t, const Ogre::MaterialPtr &m){
	Ogre::TextureUnitState *tus = m->getTechnique(0)->getPass(0)->createTextureUnitState();
	tus->setTexture(t);
	tus->setNumMipmaps(0);
	tus->setTextureFiltering(Ogre::TFO_NONE);

	return tus;
}

//!
//! Get the image to display
//!
Ogre::TexturePtr TextureGeometryNode::getImage(){

	
	// get the current name of the parameter
	QString outParameterName = m_enumImageParameter->getCurrentValue();

	// return the texture pointer
	Ogre::TexturePtr texture = getValue(outParameterName,true).value<Ogre::TexturePtr>();
	
	return getValue(outParameterName,true).value<Ogre::TexturePtr>();

}

//!
//! get the camera
//!
Ogre::Camera* TextureGeometryNode::getCamera(){

	//check if initiated already
	if(!m_camera) initCamera();
	return m_camera;
}

///
/// Protected Functions
///

///
/// public Slots
///

//!
//! upadte all outputs
//!
void TextureGeometryNode::updateAllOutputs(){

	QMap<QString,Ogre::MaterialPtr>::iterator iter;
	for(iter=m_outputParameterToMaterialMap.begin();iter!=m_outputParameterToMaterialMap.end();++iter){
		getParameter(iter.key())->propagateDirty();
	}

}

///
/// Private Functions
///

//!
//! Removes the OGRE entity containing the mesh geometry from the scene and
//! destroys it along with the OGRE scene node.
//!
void TextureGeometryNode::destroyTextureTargets(){

	//clear the render targets and remove the texture from resources and the material
	Ogre::TextureManager &texMgr = Ogre::TextureManager::getSingleton();
	Ogre::MaterialManager &matMgr = Ogre::MaterialManager::getSingleton();

	// loop all mrts and remove render targets and viewports
	QList<Ogre::MultiRenderTarget*>::iterator mrtIter;
	for(mrtIter=m_multiRenderTargetsList.begin();
		mrtIter!=m_multiRenderTargetsList.end();
		++mrtIter){
			for(int i=0;i<(*mrtIter)->getBoundSurfaceList().size();i++)
					(*mrtIter)->unbindSurface(i);
			(*mrtIter)->removeAllListeners();
			(*mrtIter)->removeAllViewports();
			(*mrtIter)->setActive(false);
			Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget((*mrtIter)->getName());
	}
	m_multiRenderTargetsList.clear();

	// iterate over all outputs
	QMap<QString,Ogre::MaterialPtr>::iterator iter;
	for(iter=m_outputParameterToMaterialMap.begin();
		iter!=m_outputParameterToMaterialMap.end();
		++iter){
			Parameter *param = getParameter(iter.key());
			Ogre::TexturePtr texture = param->getValue().value<Ogre::TexturePtr>();
			Ogre::MaterialPtr material = iter.value();

			// clear the targets and remove textures
			if(!texture.isNull()){
				Ogre::String texName = texture->getName();
				Ogre::RenderTexture *renderTexture = texture->getBuffer()->getRenderTarget();
				renderTexture->removeAllListeners();
				renderTexture->removeAllViewports();
				renderTexture->setActive(false);
				if(texMgr.resourceExists(texName))
					texMgr.remove(texName);
				Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget(texName);
			}

			//remove texture unit states
			material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

			//remove material
			matMgr.remove(material->getName());

	}

	// clear the map
	m_outputParameterToMaterialMap.clear();
	
}

//!
//! Init the camera
//!
Ogre::Camera* TextureGeometryNode::initCamera(){

	m_camera = m_PrivateSceneManager->createCamera(getName().toStdString()+"_RttCamera");
	m_camera->setPosition(1.0,0,0);
	m_camera->lookAt(0,0,0);
	m_camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	m_camera->setNearClipDistance(0.5);
	m_camera->setFarClipDistance(1.5);

	return m_camera;

}

//!
//! add render target to mrt
//!
int TextureGeometryNode::addRenderTargetToMRT(Ogre::MultiRenderTarget* mrt, Ogre::RenderTexture* target){

	// bind the targets
	int id = mrt->getBoundSurfaceList().size();
	mrt->bindSurface(id,target);

	// remove previus viewport to assign it again so the new targets wil update. this is stupid.
	mrt->removeAllViewports();
	Ogre::Viewport *mrtViewport = mrt->addViewport(m_camera);
	mrtViewport->setClearEveryFrame(true);
	mrtViewport->setOverlaysEnabled(false);
	mrtViewport->setBackgroundColour(Ogre::ColourValue(0,0,0,0));

	return id;
}

///
/// Private Slots
///

//!
//! upadte affected outputs
//!
void TextureGeometryNode::updateOutput(){

	// get the parameter
	Parameter* param = dynamic_cast<Parameter *>(sender());
	
	Ogre::MaterialPtr material = m_outputParameterToMaterialMap[param->getName()];
	getManualObject()->setMaterialName(0,material->getName());

	//update the mrt
	m_outputParameterRenderTargetMap[param->getName()]->update(true);

	//set all the related parameters clean
	QList<QString> relatedParams(m_outputParameterRenderTargetMap.keys(m_outputParameterRenderTargetMap[param->getName()]));
	QList<QString>::iterator iter;
	for(iter=relatedParams.begin();iter!=relatedParams.end();++iter){
		if(*iter!=param->getName())
			getParameter(*iter)->setDirty(false);
	}
}

//!
//! Processing function of the maximum point count parameter
//! Here does nothing, but is implemented differently in the modify node and the shader node
//!
void TextureGeometryNode::setMaxPointCountProcessingFunction(){

	// get the parameter
	EnumerationParameter* param = dynamic_cast<EnumerationParameter *>(sender());

	// get the new resolution
	int newResolution = param->getCurrentValue().toInt();

	// get the texture manager
	Ogre::TextureManager &texMgr = Ogre::TextureManager::getSingleton();

	// get all the mrts
	QList<Ogre::MultiRenderTarget*>::Iterator mrtIter;
	for(mrtIter=m_multiRenderTargetsList.begin();
		mrtIter!=m_multiRenderTargetsList.end();
		++mrtIter){

			//clear the mrt of all listeners and set unactive
			(*mrtIter)->removeAllListeners();
			(*mrtIter)->removeAllViewports();
			(*mrtIter)->setActive(false);

			// get all the targets of the mrt
			Ogre::MultiRenderTarget::BoundSufaceList boundSurfaces = (*mrtIter)->getBoundSurfaceList();
			QList<targetData> boundTargetDataList;
			Ogre::MultiRenderTarget::BoundSufaceList::iterator boundTextureIter;

			//convert target list to texture list and unbind the target
			for(boundTextureIter=boundSurfaces.begin();
				boundTextureIter!=boundSurfaces.end();
				++boundTextureIter){
					boundTargetDataList.append(*m_textureToTargetDataMap[*boundTextureIter]);
					(*mrtIter)->unbindSurface(m_textureToTargetDataMap[*boundTextureIter]->mrtBoundnumber);
					m_textureToTargetDataMap.remove(*boundTextureIter);
			}

			// create and connect the new targets
			QList<targetData>::iterator targetDataIter;
			for(targetDataIter=boundTargetDataList.begin();
				targetDataIter!=boundTargetDataList.end();
				++targetDataIter){
				
				//get current texture
				Ogre::TexturePtr currentTexture = (*targetDataIter).texture;
				
				//make sure texture exists
				if(!currentTexture.isNull()){
					//get the target data of the texture
					Parameter* param = (*targetDataIter).parameter;
					int boundID = (*targetDataIter).mrtBoundnumber;

					//get the texture name
					QString name = currentTexture->getName().c_str();
					//get the texture format
					Ogre::PixelFormat format = currentTexture->getFormat();

					Ogre::MultiRenderTarget* mrt = (*targetDataIter).mrt;

					//destroy old textures
					texMgr.remove(name.toStdString());

					// destroy the old target
					Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget((*targetDataIter).target->getName());

					//create a new texture with the new resolution
					Ogre::TexturePtr newTexture = Ogre::TextureManager::getSingleton().createManual(
														name.toStdString(),
														Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
														Ogre::TEX_TYPE_2D, 
														newResolution, newResolution,
														0,
														format,
														Ogre::TU_RENDERTARGET);
					Ogre::RenderTexture *newRenderTexture = newTexture->getBuffer()->getRenderTarget();

					newRenderTexture->setAutoUpdated(false);
	
					Ogre::Root::getSingleton().getRenderSystem()->setDepthBufferFor(newRenderTexture);

					//set the new texture to the mrt
					(*mrtIter)->bindSurface(boundID,newRenderTexture);
					
					//check depth buffer
					Ogre::DepthBuffer* dBuffer = m_outputParameterRenderTargetMap[param->getName()]->getDepthBuffer();
					
					(*mrtIter)->detachDepthBuffer();

					//set the parameter to the new target
					param->setValue(QVariant::fromValue<Ogre::TexturePtr>(newTexture));

					//update the data
					targetData* newTargetData = new targetData;
					newTargetData->mrt=(*mrtIter);
					newTargetData->mrtBoundnumber=boundID;
					newTargetData->parameter=param;
					newTargetData->target=newRenderTexture;
					newTargetData->texture=newTexture;
					m_textureToTargetDataMap[newRenderTexture] = newTargetData;
				}

				//set the viewport
				Ogre::Viewport *mrtViewport = (*mrtIter)->addViewport(m_camera);
				mrtViewport->setClearEveryFrame(true);
				mrtViewport->setOverlaysEnabled(false);
				mrtViewport->setBackgroundColour(Ogre::ColourValue(0,0,0,0));

				//set back to active
				(*mrtIter)->setActive(true);

			}//end of per mrt iteration

	}

	//set new resolution to shaders
	QList<Ogre::MaterialPtr>::iterator matIter;
	for(matIter = getMaterials().begin();
		matIter!= getMaterials().end();
		++matIter){

		// check if material is null
		if(!(*matIter).isNull()){
			//check if the fragment program has a resolution parameter
			if((*matIter)->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->_findNamedConstantDefinition("resolution")){
				(*matIter)->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("resolution",newResolution);
			}
		}
	}

	//set the max of the count parameter
	getNumberParameter("pointCount")->setMaxValue(newResolution*newResolution);

	//set the parameter clean
	param->setDirty(false);

	//updtae the panel to the new value
	forcePanelUpdate();

}

} // end namespace
