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
//! \file "PointCloudViewerNode.h"
//! \brief Implementation file for PointCloudViewerNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "PointCloudViewerNode.h"
#include "OgreManager.h"
#include "OgreTools.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>

namespace PointCloudViewerNode {
	using namespace Frapper;

INIT_INSTANCE_COUNTER(PointCloudViewerNode)
Q_DECLARE_METATYPE(QVector<float>);

///
/// Constructors and Destructors
///


//!
//! Constructor of the PointCloudViewerNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PointCloudViewerNode::PointCloudViewerNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryNode(name, parameterRoot, "PointCloud"),
    m_sceneNode(0),
    m_pointCloud(0)
#ifndef TEST_MANUAL_MESH
	,m_manualObjContainer(0)
#endif
{
	std::cout << "Checkpoint Constr. Point Cloud 1" << std::endl;
	Parameter* vertexBuffer = getParameter("VertexBuffer");
	vertexBuffer->setProcessingFunction(SLOT(updatePointCloud()));
	//vertexBuffer->setSelfEvaluating(true);
	vertexBuffer->addAffectedParameter(getParameter(m_outputGeometryName));

	Parameter* scaleParameter = getParameter("PointSize");
	scaleParameter->setChangeFunction(SLOT(setShaderParameter()));

	INC_INSTANCE_COUNTER
	createObject();
	std::cout << "Checkpoint Constr. Point Cloud 2" << std::endl;
}


//!
//! Destructor of the PointCloudViewerNode class.
//!
PointCloudViewerNode::~PointCloudViewerNode ()
{
	std::cout << "Checkpoint Destr. Point Cloud 1" << std::endl;
    destroyEntity();
#ifdef TEST_MANUAL_MESH
	if (m_pointCloud)
		delete m_pointCloud;
#endif
    emit viewNodeUpdated();
	std::cout << "Checkpoint Destr. Point Cloud 2" << std::endl;
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
bool PointCloudViewerNode::createObject ()
{
    // obtain the OGRE scene manager
    Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    if (!sceneManager) {
        Log::error("Could not obtain OGRE scene manager.", "PointCloudViewerNode::loadMesh");
        return false;
    }

    // create new scene node
    m_sceneNode = OgreManager::createSceneNode(m_name);
    if (!m_sceneNode) {
        Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "PointCloudViewerNode::loadMesh");
        return false;
    }
    setValue(m_outputGeometryName, m_sceneNode, true);

#ifdef TEST_MANUAL_MESH
	m_pointCloud = new ManualMesh((m_name + "_Points").toStdString(), "General");
	m_entity = sceneManager->createEntity((m_name + "_Points").toStdString(), m_pointCloud->getMesh());
	m_pointCloud->setMaterialName("PointCloudMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_entity->setMaterialName("PointCloudMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_sceneNode->attachObject(m_entity);
#else
	m_pointCloud = sceneManager->createManualObject((m_name + "_Points").toStdString());
    m_sceneNode->attachObject(m_pointCloud);

	m_pointCloud->setUseIdentityProjection(true);
    m_pointCloud->setUseIdentityView(true);

	// create a container for the entity
    m_manualObjContainer = new OgreContainer(m_pointCloud);
    m_pointCloud->setUserAny(Ogre::Any(m_manualObjContainer));
#endif	

    updateStatistics();
    return true;
}


//!
//! Retrieves the numbers of vertices and triangles from the mesh and stores
//! them in parameters of this node.
//!
void PointCloudViewerNode::updateStatistics ()
{
	// ToDo!!!
    unsigned int numVertices = 0;
    unsigned int numTriangles = 0;

    setValue("Number of Vertices", QString("%L1").arg(numVertices), true);
    setValue("Number of Triangles", QString("%L1").arg(numTriangles), true);
}


//!
//! Removes the OGRE entity containing the mesh geometry from the scene and
//! destroys it along with the OGRE scene node.
//!
void PointCloudViewerNode::destroyEntity ()
{
    if (m_pointCloud) {
        // remove the entity from the scene node it is attached to
#ifdef TEST_MANUAL_MESH
		Ogre::SceneNode *parentSceneNode = m_entity->getParentSceneNode();
        if (parentSceneNode)
            parentSceneNode->detachObject(m_entity);

        // destroy the entity through its scene manager
        Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
        if (sceneManager) {
			sceneManager->destroyEntity(m_entity);
            m_entity = 0;
        }
#else
		Ogre::SceneNode *parentSceneNode = m_pointCloud->getParentSceneNode();
		if (parentSceneNode)
			parentSceneNode->detachObject(m_pointCloud);

		// destroy the entity through its scene manager
		Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
		if (sceneManager) {
			sceneManager->destroyManualObject(m_pointCloud);
			m_entity = 0;
		}
#endif
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
void PointCloudViewerNode::setShaderParameter()
{
	Parameter* parameter = dynamic_cast<Parameter*>(sender());

	if (parameter) {
		const Ogre::String &name = parameter->getName().toStdString();
		const float value = parameter->getValue().toFloat();

		Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("PointCloudMaterial"));
		if (!matPtr.isNull())
			matPtr->getTechnique(0)->getPass(0)->setPointSize(value);

		triggerRedraw();
	}
}

//!
//! Change function for the Geometry File parameter.
//!
void PointCloudViewerNode::updatePointCloud ()
{	
	Parameter* vertexBuffer = dynamic_cast<Parameter*>(sender());
	ParameterGroup* vertexBufferGroup = vertexBuffer->getValue().value<ParameterGroup *>();
	


#ifdef TEST_MANUAL_MESH
	if (!vertexBufferGroup)
		m_pointCloud->clear();
	else
		updateManualMesh(vertexBufferGroup);
#else
	if (!vertexBufferGroup)
		return;
	m_manualObjContainer->updateVertexBuffer(vertexBufferGroup);
#endif
}

#ifdef TEST_MANUAL_MESH
//!
//! Updates the manual mesh.  
//!
void PointCloudViewerNode::updateManualMesh(ParameterGroup* vertexBufferGroup)
{
	if (!vertexBufferGroup)
		return;
	
	NumberParameter* posParameter = static_cast<NumberParameter*>(vertexBufferGroup->getParameter("pos"));
	NumberParameter* colParameter = static_cast<NumberParameter*>(vertexBufferGroup->getParameter("col"));
	NumberParameter* normParameter = static_cast<NumberParameter*>(vertexBufferGroup->getParameter("norm"));
	NumberParameter* uvParameter = static_cast<NumberParameter*>(vertexBufferGroup->getParameter("uv"));
	
	Ogre::RenderOperation::OperationType renderOperation = Ogre::RenderOperation::OT_POINT_LIST;
	Parameter* opParameter = vertexBufferGroup->getParameter("op");
	if (opParameter) {
		QString opString = opParameter->getValue().value<QString>();
		if (opString == "triangles")
			renderOperation = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	}
	
	QString materialName = "PointCloudMaterial";
	Parameter* matParameter = vertexBufferGroup->getParameter("mat");
	if (matParameter) {
		materialName = matParameter->getValue().value<QString>();
	}

	QString materialGroupName = QString::fromStdString(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Parameter* matGroupParameter = vertexBufferGroup->getParameter("matGroup");
	if (matGroupParameter) {
		materialGroupName = matGroupParameter->getValue().value<QString>();
	}

	

	const int code = (bool) posParameter * 1 + 
					 (bool) colParameter * 2 +
					 (bool) normParameter * 4 + 
					 (bool) uvParameter * 8;

	QVector<float> &posList = posParameter->getValue().value<QVector<float>>();
	unsigned int numberOfVertices = posList.size() / 3;
	
	if (numberOfVertices == 0)
		return;

	m_pointCloud->setMaterialName(materialName.toStdString(), materialGroupName.toStdString());
	m_entity->setMaterialName(materialName.toStdString(), materialGroupName.toStdString());
	
	m_pointCloud->setRenderOperationType(renderOperation);

	switch (code){
		case 1: {
			m_pointCloud->update(numberOfVertices, &posList[0], 0, 0, 0);
			break;
		}
		case 3: {
			QVector<float> &colList = colParameter->getValue().value<QVector<float>>();
			m_pointCloud->update(numberOfVertices, &posList[0], &colList[0], 0, 0);
			break;
		}
		case 7: {
			QVector<float> &colList = colParameter->getValue().value<QVector<float>>();
			QVector<float> &normList = normParameter->getValue().value<QVector<float>>();
			m_pointCloud->update(numberOfVertices, &posList[0], &colList[0], &normList[0], 0);
			break;
		}
		case 15: {
			QVector<float> &colList = colParameter->getValue().value<QVector<float>>();
			QVector<float> &normList = normParameter->getValue().value<QVector<float>>();
			QVector<float> &uvList = uvParameter->getValue().value<QVector<float>>();
			m_pointCloud->update(numberOfVertices, &posList[0], &colList[0], &normList[0], &uvList[0]);
			break;
		}
		default: {
			Log::error(QString("Max supported attributes size is 4"), "OgreContainer::updateVertexBuffer");
			break;
		}
	}
}
#endif

} // end namespace
