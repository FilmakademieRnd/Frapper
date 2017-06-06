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
PositionBufferCreatorNode::PositionBufferCreatorNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryNode(name, parameterRoot, "PositionBuffer"),
    m_sceneNode(0),
    m_positionEntity(0),
	m_renderTargetTexture(0),
	m_renderTargetCamera(0)
{

	Parameter* scaleParameter = getParameter("PointSize");
	scaleParameter->setChangeFunction(SLOT(setScaleParameter()));

	createBufferObject();

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the PositionBufferCreatorNode class.
//!
PositionBufferCreatorNode::~PositionBufferCreatorNode ()
{
    destroyEntity();
    emit viewNodeUpdated();

    DEC_INSTANCE_COUNTER
}


///
/// Public Slots
///


///
/// Private Functions
///


//!
//! Loads animation mesh.
//!
//! \return True, if successful loading of ogre mesh
//! False, otherwise.
//!
bool PositionBufferCreatorNode::createBufferObject()
{
    // obtain the OGRE scene manager
    Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    if (!sceneManager) {
        Log::error("Could not obtain OGRE scene manager.", "PositionBufferCreatorNode::createBuffer");
        return false;
    }

    // create new scene node
    m_sceneNode = OgreManager::createSceneNode(m_name);
    if (!m_sceneNode) {
        Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "PositionBufferCreatorNode::loadMesh");
        return false;
    }
    setValue(m_outputGeometryName, m_sceneNode, true);

    // create a new OGRE entity for the buffer
	m_positionEntity = sceneManager->createManualObject((m_name + "_Entity").toStdString());
    m_sceneNode->attachObject(m_positionEntity);

	//create the rtt
	createRtt();

	// create the base point
	updateBuffer();
	
    return true;
}


//!
//! Removes the OGRE entity containing the mesh geometry from the scene and
//! destroys it along with the OGRE scene node.
//!
void PositionBufferCreatorNode::destroyEntity ()
{
    if (m_positionEntity) {
        // remove the entity from the scene node it is attached to
        Ogre::SceneNode *parentSceneNode = m_positionEntity->getParentSceneNode();
        if (parentSceneNode)
            parentSceneNode->detachObject(m_positionEntity);

        // destroy the entity through its scene manager
        Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
        if (sceneManager) {
			sceneManager->destroyManualObject(m_positionEntity);
            m_positionEntity = 0;
        }
    }

    if (m_sceneNode) {
        // destroy the scene node through its scene manager
        Ogre::SceneManager *sceneManager = m_sceneNode->getCreator();
        if (sceneManager) {
            sceneManager->destroySceneNode(m_sceneNode);
            m_sceneNode = 0;
            setValue(m_outputGeometryName, m_sceneNode, true);
        }
    }
}



///
/// Private Slots
///

//!
//! Sets the scenes shader parameter
//!
void PositionBufferCreatorNode::setScaleParameter()
{
	Parameter* parameter = dynamic_cast<Parameter*>(sender());

	if (parameter) {
		const Ogre::String &name = parameter->getName().toStdString();
		const float value = parameter->getValue().toFloat();

		Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("FlatViewShaderMaterial"));
		if (!matPtr.isNull()){
			updateRtt();
		}
			//matPtr->getTechnique(0)->getPass(0)->getGeometryProgramParameters()->setNamedConstant("radius",value);

		triggerRedraw();
	}
}

//!
//! Change function for the Geometry File parameter.
//!
void PositionBufferCreatorNode::updateBuffer()
{	
	m_positionEntity->clear();

	m_positionEntity->begin("FlatViewShaderMaterial", Ogre::RenderOperation::OT_POINT_LIST);
	QVector3D pos(0,0,0);
	m_positionEntity->position(
		pos.x(),
		pos.y(),
		pos.z() );

	m_positionEntity->end();

	updateRtt();
}

//!
//! create and setup the rtt texture
//!
void PositionBufferCreatorNode::createRtt(){

	Ogre::TexturePtr rtt_texture = Ogre::TextureManager::getSingleton().createManual(
			"RttTex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, 
			512, 512,
			0,
			Ogre::PF_R8G8B8A8,
			Ogre::TU_RENDERTARGET);
	m_renderTargetTexture = rtt_texture->getBuffer()->getRenderTarget();
	m_renderTargetCamera = OgreManager::getSceneManager()->createCamera(createUniqueName("rttCamera"));
	m_renderTargetCamera->setPosition(0.5,0,0);
	m_renderTargetCamera->lookAt(0,0,0);
	m_renderTargetCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	m_renderTargetCamera->setNearClipDistance(0.5);
	m_renderTargetCamera->setFarClipDistance(1.5);
	m_renderTargetTexture->addViewport(m_renderTargetCamera);
	m_renderTargetTexture->getViewport(0)->setClearEveryFrame(true);
	m_renderTargetTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
	m_renderTargetTexture->getViewport(0)->setOverlaysEnabled(false);

}


//!
//! update the rtt
//!
void PositionBufferCreatorNode::updateRtt(){

	triggerRedraw();
	m_renderTargetTexture->update();
	m_renderTargetTexture->writeContentsToFile("rtt.png");
}

} // end namespace
