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
//! \file "PositionBufferCreatorNode.h"
//! \brief Implementation file for PositionBufferCreatorNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "PositionBufferCreatorNode.h"
#include "OgreManager.h"
#include "OgreTools.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QVector3D>

namespace PositionBufferCreatorNode {
	using namespace Frapper;

INIT_INSTANCE_COUNTER(PositionBufferCreatorNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the PositionBufferCreatorNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PositionBufferCreatorNode::PositionBufferCreatorNode ( const QString &name, ParameterGroup *parameterRoot, const QString &outputImageName ) :
    ViewNode(name, parameterRoot),
    m_sceneNode(0),
	m_sceneManager(0),
    m_dummyManualObject(0),
	m_textureRenderTarget(0),
	m_viewport(0),
	m_renderTargetCamera(0),
	m_outputImageName(outputImageName),
	m_outputParameter(0)

{

	// add the output image parameter
	m_outputParameter = Parameter::createImageParameter(m_outputImageName);
    addOutputParameter( m_outputParameter);
    m_outputParameter->setAuxProcessingFunction(SLOT(redrawTriggered()));

	Ogre::Root *ogreRoot = OgreManager::getRoot();
    if (ogreRoot) {
        // create new Ogre::SceneManager to handle the temporary render scene
        m_sceneManager = ogreRoot->createSceneManager(Ogre::ST_GENERIC, createUniqueName("SceneManager_PositionBufferCreatorNode"));
        if (m_sceneManager) {
			createRtt();
			createDummyManualObject();
        }
		else
            Log::error("could not create sceneManager!", "PositionBufferCreatorNode");
    }

	// create default texture
    Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	m_defaultTexture = textureManager.createManual(createUniqueName("defaultTexture"),
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, 
			512, 512,
			0,
			Ogre::PF_R8G8B8A8,
			Ogre::TU_RENDERTARGET);

	//Parameter* scaleParameter = getParameter("PointSize");
	//scaleParameter->setChangeFunction(SLOT(setScaleParameter()));

	setValue(m_outputImageName, m_defaultTexture);

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the PositionBufferCreatorNode class.
//!
PositionBufferCreatorNode::~PositionBufferCreatorNode ()
{
	//clear the render target and remove the texture from resources
	Ogre::TextureManager &texMgr = Ogre::TextureManager::getSingleton();
    if (!m_rttTexture.isNull()) {
        m_textureRenderTarget->removeAllListeners();
        m_textureRenderTarget->removeAllViewports();
        if (texMgr.resourceExists(m_renderTextureName))
            texMgr.remove(m_renderTextureName);
        else
            Log::error("Texture: " + QString(m_renderTextureName.c_str()) + "to be removed from Ogre::TextureManager could not be found!", "PositionBufferCreatorNode");
    }

	//destroy the dummy manual object
    destroyDummyManualObject();

    emit viewNodeUpdated();

    DEC_INSTANCE_COUNTER
}

///
/// Public Slots
///

//!
//! Redraw of ogre scene has been triggered.
//!
void PositionBufferCreatorNode::redrawTriggered()
{
	Log::debug("redrawTriggered","");

    if	(!m_rttTexture.isNull() &&
		(isViewed() || m_outputParameter->isConnected()))
        updateRtt();

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
void PositionBufferCreatorNode::setOutputImage( Ogre::TexturePtr texture )
{
	if (texture.isNull()) {
		Log::error("Invalid texture given.", "ImageNode::setOutputImage");
		return;
	}
	// update the output image parameter
	setValue(m_outputImageName, texture, true);
	//setDirty(m_outputImageName, false);
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
bool PositionBufferCreatorNode::createDummyManualObject()
{

    // create new scene node
    m_sceneNode = OgreManager::createSceneNode(m_name);
    if (!m_sceneNode) {
        Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "PositionBufferCreatorNode::createDummyManualObject");
        return false;
    }

    // create a new OGRE entity for the buffer
	m_dummyManualObject = m_sceneManager->createManualObject((m_name + "_Entity").toStdString());
    m_sceneNode->attachObject(m_dummyManualObject);

	// create the base point
	updateDummyManualObject();

    return true;
}


//!
//! Removes the OGRE entity containing the mesh geometry from the scene and
//! destroys it along with the OGRE scene node.
//!
void PositionBufferCreatorNode::destroyDummyManualObject ()
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

    if (m_sceneNode) {
        // destroy the scene node through its scene manager
        Ogre::SceneManager *sceneManager = m_sceneNode->getCreator();
        if (sceneManager) {
            sceneManager->destroySceneNode(m_sceneNode);
            m_sceneNode = 0;
            //setValue(m_outputGeometryName, m_sceneNode, true);
        }
    }
}



///
/// Private Slots
///

//!
//! Change function for the Geometry File parameter.
//!
void PositionBufferCreatorNode::updateDummyManualObject()
{	
	m_dummyManualObject->clear();

	m_dummyManualObject->begin("FlatViewShaderMaterial", Ogre::RenderOperation::OT_POINT_LIST);
	QVector3D pos(0,0,0);
	m_dummyManualObject->position(
		pos.x(),
		pos.y(),
		pos.z() );

	m_dummyManualObject->end();

	Log::debug("here");
	triggerRedraw();
}

//!
//! create and setup the rtt texture
//!
void PositionBufferCreatorNode::createRtt(){

	m_renderTextureName = createUniqueName("RttTexuture");
	m_rttTexture = Ogre::TextureManager::getSingleton().createManual(
			m_renderTextureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, 
			512, 512,
			0,
			Ogre::PF_R8G8B8A8,
			Ogre::TU_RENDERTARGET);
	m_textureRenderTarget = m_rttTexture->getBuffer()->getRenderTarget();
	m_renderTargetCamera = OgreManager::getSceneManager()->createCamera(createUniqueName("rttCamera"));
	m_renderTargetCamera->setPosition(0.5,0,0);
	m_renderTargetCamera->lookAt(0,0,0);
	m_renderTargetCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	m_renderTargetCamera->setNearClipDistance(0.5);
	m_renderTargetCamera->setFarClipDistance(1.5);
	m_viewport = m_textureRenderTarget->addViewport(m_renderTargetCamera);
	m_textureRenderTarget->getViewport(0)->setClearEveryFrame(true);
	m_textureRenderTarget->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
	m_textureRenderTarget->getViewport(0)->setOverlaysEnabled(false);

	setOutputImage(m_rttTexture);
}


//!
//! update the rtt
//!
void PositionBufferCreatorNode::updateRtt(){

	m_textureRenderTarget->update();
	m_textureRenderTarget->writeContentsToFile("rtt.png");
}

} // end namespace
