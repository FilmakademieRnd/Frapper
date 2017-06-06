/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "PointCreatorNode.h"
//! \brief Implementation file for PointCreatorNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "PointCreatorNode.h"
#include "OgreManager.h"
#include "OgreTools.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QVector3D>

namespace PointCreatorNode {
	using namespace Frapper;

INIT_INSTANCE_COUNTER(PointCreatorNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the PointCreatorNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PointCreatorNode::PointCreatorNode ( const QString &name, ParameterGroup *parameterRoot, const QString &outputImageName ) :
    ViewNode(name, parameterRoot),
    m_sceneNode(0),
	m_sceneManager(0),
    m_dummyManualObject(0),
	m_viewport(0),
	m_renderTargetCamera(0),
	m_outputImageName(outputImageName),
	m_outputParameter(0),
	m_pointSizeParameter(0),
	m_pointCountParameter(0)

{

	//setup the material pointers
	m_flatMaterialPtr = Ogre::MaterialManager::getSingletonPtr()->getByName("FlatViewShaderMaterial");
	m_pointMaterialPtr = Ogre::MaterialManager::getSingletonPtr()->getByName("PointSimpleMaterial");

	// add the output image parameter
	m_outputParameter = Parameter::createImageParameter(m_outputImageName);
    addOutputParameter( m_outputParameter);
    m_outputParameter->setAuxProcessingFunction(SLOT(redrawTriggered()));

#define DEBUG
#ifdef DEBUG
	//add the manual rtt update parameter
	Parameter *updateCommandParameter = Parameter::create("update",Frapper::Parameter::T_Command);
	parameterRoot->addParameter(updateCommandParameter);
	updateCommandParameter->setCommandFunction(SLOT(updateRtt()));
	
#endif

	//add the pointScale parameter
	m_pointSizeParameter = new NumberParameter("pointSize",Frapper::Parameter::T_Int,2);
	m_pointSizeParameter->setNode(this);
	m_pointSizeParameter->setMaxValue(1000);
	m_pointSizeParameter->setMinValue(0);
	parameterRoot->addParameter(m_pointSizeParameter);
	m_pointSizeParameter->setChangeFunction(SLOT(setScaleParameter()));

	//add the pointCount parameter
	m_pointCountParameter = new NumberParameter("pointCount",Frapper::Parameter::T_UnsignedInt,1);
	m_pointCountParameter->setNode(this);
	m_pointCountParameter->setMaxValue(512*512);
	m_pointCountParameter->setMinValue(0);
	parameterRoot->addParameter(m_pointCountParameter);
	m_pointCountParameter->setChangeFunction(SLOT(setCountParameter()));

	m_lastCount = m_currentPointCount = 1;

	Ogre::Root *ogreRoot = OgreManager::getRoot();
	m_sceneManager = OgreManager::getSceneManager();
	if (ogreRoot&&m_sceneManager) {
		// create new scene node
		m_sceneNode = OgreManager::createSceneNode(m_name);
		createRtt();
		createDummyManualObject();
		updateRtt();
    } else
            Log::error("could not create sceneManager!", "PointCreatorNode");


	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the PointCreatorNode class.
//!
PointCreatorNode::~PointCreatorNode ()
{
	//clear the render target and remove the texture from resources
	Ogre::TextureManager &texMgr = Ogre::TextureManager::getSingleton();
    if (!m_rttTexture.isNull()) {
         m_rttTexture->getBuffer()->getRenderTarget()->removeAllListeners();
         m_rttTexture->getBuffer()->getRenderTarget()->removeAllViewports();
        if (texMgr.resourceExists(m_renderTextureName))
            texMgr.remove(m_renderTextureName);
        else
            Log::error("Texture: " + QString(m_renderTextureName.c_str()) + "to be removed from Ogre::TextureManager could not be found!", "PointCreatorNode");
    }

	//destroy the dummy manual object
    destroyDummyManualObject();

    emit viewNodeUpdated();

    DEC_INSTANCE_COUNTER
}

//!
//! get the scene node for viewport rendering
//!
Ogre::SceneNode* PointCreatorNode::getSceneNode(){
	
	return m_sceneNode;
}

///
/// Public Slots
///

//!
//! Redraw of ogre scene has been triggered.
//!
void PointCreatorNode::redrawTriggered()
{

    if	(!m_rttTexture.isNull() &&
		(isViewed() || m_outputParameter->isConnected()))
        //m_rttTexture->getBuffer()->getRenderTarget()->update();

	getParameter(m_outputImageName)->setAuxDirty(false);

}

///
/// Protected Functions
///

//!
//! Sets the given texture as the output image in the output image parameter.
//!
//! \param texture The texture to use as output image
//!
void PointCreatorNode::setOutputImage( Ogre::TexturePtr texture )
{
	if (texture.isNull()) {
		Log::error("Invalid texture given.", "PointCreatorNode::setOutputImage");
		return;
	}
	// update the output image parameter
	setValue(m_outputImageName, texture, true);
}

///
/// Private Functions
///

//!
//! creates the dummy manual object
//!
//! \return True, if successful loading of ogre mesh
//! False, otherwise.
//!
bool PointCreatorNode::createDummyManualObject()
{

    if (!m_sceneNode) {
        Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "PointCreatorNode::createDummyManualObject");
        return false;
    }

    // create a new OGRE entity for the buffer
	m_dummyManualObject = m_sceneManager->createManualObject((m_name + "_Entity").toStdString());
	m_dummyManualObject->begin(m_pointMaterialPtr->getName(), Ogre::RenderOperation::OT_POINT_LIST);
	m_dummyManualObject->position(0,0,0);
	m_dummyManualObject->end();
    m_sceneNode->attachObject(m_dummyManualObject);

    return true;
}


//!
//! Removes the OGRE entity containing the mesh geometry from the scene and
//! destroys it along with the OGRE scene node.
//!
void PointCreatorNode::destroyDummyManualObject ()
{
    if (m_dummyManualObject) {
        // remove the entity from the scene node it is attached to
        Ogre::SceneNode *parentSceneNode = m_dummyManualObject->getParentSceneNode();
        if (parentSceneNode)
            parentSceneNode->detachObject(m_dummyManualObject);

        // destroy the entity through its scene manager
        Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
        if (sceneManager) {
			sceneManager->destroyManualObject(m_dummyManualObject);
            m_dummyManualObject = 0;
        }
    }

	if (m_renderTargetCamera) {
        // remove the entity from the scene node it is attached to
        Ogre::SceneNode *parentSceneNode = m_renderTargetCamera->getParentSceneNode();
        if (parentSceneNode)
            parentSceneNode->detachObject(m_renderTargetCamera);

        // destroy the entity through its scene manager
        Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
        if (sceneManager) {
			sceneManager->destroyCamera(m_renderTargetCamera);
            m_renderTargetCamera = 0;
        }
    }

    if (m_sceneNode) {
        // destroy the scene node through its scene manager
        Ogre::SceneManager *sceneManager = m_sceneNode->getCreator();
        if (sceneManager) {
            sceneManager->destroySceneNode(m_sceneNode);
            m_sceneNode = 0;
        }
    }
}



///
/// Private Slots
///

//!
//! Sets the scenes shader parameter
//!
void PointCreatorNode::setScaleParameter()
{
	Parameter* parameter = dynamic_cast<Parameter*>(sender());

	if (parameter) {
		const Ogre::String &name = parameter->getName().toStdString();
		const float value = parameter->getValue().toFloat();

		if (!m_pointMaterialPtr.isNull()){
			m_pointMaterialPtr->getTechnique(0)->getPass(0)->getGeometryProgramParameters()->setNamedConstant("radius",value);
		}

		triggerRedraw();
	}
}

//!
//! Sets the scenes shader parameter
//!
void PointCreatorNode::setCountParameter()
{
	Parameter* parameter = dynamic_cast<Parameter*>(sender());

	if (parameter) {
		const Ogre::String &name = parameter->getName().toStdString();
		m_currentPointCount = parameter->getValue().toInt();

		if (!m_flatMaterialPtr.isNull()){
			m_flatMaterialPtr->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("count",m_currentPointCount);
		}

		updateRtt();
		triggerRedraw();
	}
}

//!
//! create and setup the rtt texture
//!
void PointCreatorNode::createRtt(){

	m_renderTextureName = createUniqueName("RttTexuture");
	m_rttTexture = Ogre::TextureManager::getSingleton().createManual(
			m_renderTextureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, 
			512, 512,
			0,
			Ogre::PF_R8G8B8A8,
			Ogre::TU_RENDERTARGET);

	m_renderTargetCamera = m_sceneManager->createCamera(createUniqueName("rttCamera"));
	m_sceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));
	m_sceneNode->attachObject(m_renderTargetCamera);
	m_renderTargetCamera->setPosition(1.0,0,0);
	m_renderTargetCamera->lookAt(0,0,0);
	m_renderTargetCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	m_renderTargetCamera->setNearClipDistance(0.5);
	m_renderTargetCamera->setFarClipDistance(1.5);
	m_viewport = m_rttTexture->getBuffer()->getRenderTarget()->addViewport(m_renderTargetCamera);
	m_viewport->setClearEveryFrame(true);
	m_viewport->setBackgroundColour(Ogre::ColourValue::Black);
	m_viewport->setOverlaysEnabled(false);

	Ogre::TextureUnitState *textureUnitState = m_pointMaterialPtr->getTechnique(0)->getPass(0)->createTextureUnitState(m_renderTextureName);
	textureUnitState->setNumMipmaps(0);
	textureUnitState->setTextureFiltering(Ogre::TFO_NONE);

	setOutputImage(m_rttTexture);
}


//!
//! update the rtt
//!
void PointCreatorNode::updateRtt(){

#ifdef DEBUG
	Log::debug("rttUpdate");
#endif

	Ogre::SceneNode *root =  m_sceneManager->getRootSceneNode();

	if(!m_view)root->addChild(m_sceneNode);

	bool recreate = false;

	//compare point counts
	if(m_currentPointCount!=m_lastCount){

		//chack point count %64 step
		int lastCountSection = m_lastCount/65;
		int currentCountSection = m_currentPointCount/65;

		//set old count
		m_lastCount = m_currentPointCount;

		if(lastCountSection!=currentCountSection) recreate = true;
	}
	
	m_dummyManualObject->setMaterialName(0,m_flatMaterialPtr->getName());

	m_rttTexture->getBuffer()->getRenderTarget()->update();
#ifdef DEBUG
	m_rttTexture->getBuffer()->getRenderTarget()->writeContentsToFile("rtt.png");
#endif

	if (recreate){
		m_dummyManualObject->clear();

		m_dummyManualObject->begin(m_pointMaterialPtr->getName(), Ogre::RenderOperation::OT_POINT_LIST);
		for(int i=0;i<(m_currentPointCount/65)+1;i++){
			m_dummyManualObject->position(0,0,0);
		}

		m_dummyManualObject->end();
	} else
		m_dummyManualObject->setMaterialName(0,m_pointMaterialPtr->getName());

	if(!m_view)root->removeChild(m_sceneNode);

}

} // end namespace
