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
//! \file "SceneLoaderNode.h"
//! \brief Implementation file for SceneLoaderNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       15.01.2012 (last updated)
//!

#include "SceneLoaderNode.h"
#include "ParameterGroup.h"
#include "NumberParameter.h"
#include "SceneNodeParameter.h"
#include "FilenameParameter.h"
#include "OgreManager.h"
#include "OgreTools.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include "Node.h"

namespace SceneLoaderNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(SceneLoaderNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the SceneLoaderNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
SceneLoaderNode::SceneLoaderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryNode(name, parameterRoot, "Mesh"),
    m_sceneNode(0),
    m_sceneLoader(NULL),
    m_lightsSceneNode(NULL),
    m_oldResourceGroupName("")
{
    m_sceneLoader = new DotSceneLoader();

    // Teach .scene loader which userDataReferences to parse and store in UserAny
    m_sceneLoader->userDataAttributes.push_back("MOVE");
    m_sceneLoader->userDataAttributes.push_back("JUMP");
    m_sceneLoader->userDataAttributes.push_back("MOVEJUMP");

    m_sceneLoader->userDataAttributes.push_back("PLATFORM");
    m_sceneLoader->userDataAttributes.push_back("OBJECT");
    m_sceneLoader->userDataAttributes.push_back("VISIBILITY");
    m_sceneLoader->userDataAttributes.push_back("CHECKPOINT");
    //m_sceneLoader->userDataAttributes.push_back("OBSTACLE");
    //m_sceneLoader->userDataAttributes.push_back("ENEMY");

    m_sceneLoader->userDataAttributes.push_back("START");
    m_sceneLoader->userDataAttributes.push_back("END");
    m_sceneLoader->userDataAttributes.push_back("TYPE");
    m_sceneLoader->userDataAttributes.push_back("LENGTH");
    
    // set affections and functions
    addAffection(       "Scene File", m_outputGeometryName);
    setCommandFunction( "Scene File", SLOT(sceneFileChanged()));
    setChangeFunction(  "Scene File", SLOT(sceneFileChanged()));
}


//!
//! Destructor of the SceneLoaderNode class.
//!
SceneLoaderNode::~SceneLoaderNode ()
{
    delete m_sceneLoader;
    OgreTools::deepDeleteSceneNode(m_sceneNode, OgreManager::getSceneManager(), true);
    OgreTools::destroyResourceGroup(m_oldResourceGroupName);
}

///
/// Public Functions
///


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
//!            False, otherwise.
//!
bool SceneLoaderNode::loadScene()
{
    QString sceneFileName = this->getStringValue("Scene File");

    if( sceneFileName == "" )
    {
        Frapper::Log::debug(QString("Scene file has not been set yet. (\"%1\")").arg(this->m_name), "SceneLoaderNode::loadScene");
        return false;
    }

    QFileInfo sceneFile(sceneFileName);

    // check if the file exists
    if (!sceneFile.exists()) {
        Frapper::Log::error(QString("Scene file \"%1\" not found.").arg(sceneFile.completeBaseName()), "SceneLoaderNode::loadScene");
        return false;
    }

    if (sceneFile.suffix() != "scene") {
        Frapper::Log::error("The file has to be an OGRE scene file.", "SceneLoaderNode::loadScene");
        return false;
    }


    // obtain the OGRE scene manager
    Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    if (!sceneManager) {
        Frapper::Log::error("Could not obtain OGRE scene manager.", "SceneLoaderNode::loadScene");
        return false;
    }

    // destroy an existing scene node and create a new one
    if( m_sceneNode )
    {
        // delete parameters
        Ogre::SceneNode::ChildNodeIterator itChild = m_sceneNode->getChildIterator();
        while ( itChild.hasMoreElements() )
        {
            Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
            this->removeOutputParameter(this->getParameter( QString::fromStdString(pChildNode->getName()).remove("Node")));
        }

        OgreTools::deepDeleteSceneNode(m_sceneNode, OgreManager::getSceneManager(), true );
    }

    // delete lights scene node and output parameter
    if( m_lightsSceneNode )
    {
        this->removeOutputParameter( this->getParameter("LightsSceneNode"));
        OgreTools::deepDeleteSceneNode(m_lightsSceneNode, OgreManager::getSceneManager(), true);
    }

    // create a root scene node for this frapper node with container 
    m_sceneNode = OgreManager::createSceneNode( QString::fromStdString( createUniqueName(this->m_name)));
    
    if (!m_sceneNode) {
        Frapper::Log::error(QString("Scene node for node \"%1\" could not be created.").arg(this->m_name), "SceneLoaderNode::loadScene");
        return false;
    }
    m_sceneNode->setUserAny(Ogre::Any(new OgreContainer(m_sceneNode)));

    // destroy old resource group and generate new one
    Ogre::String resourceGroupName = createUniqueName("ResourceGroup_" + sceneFile.fileName() + "_Level");
    OgreTools::destroyResourceGroup(m_oldResourceGroupName);
    m_oldResourceGroupName = QString::fromStdString(resourceGroupName);
    OgreTools::createResourceGroup(QString::fromStdString(resourceGroupName), sceneFile.path());

    m_sceneLoader->parseDotScene(sceneFileName.toStdString(), resourceGroupName, sceneManager, m_sceneNode);   

    Frapper::Log::info(QString("Scene file \"%1\" loaded.").arg(sceneFile.completeBaseName()), "SceneLoaderNode::loadScene");

    // Add an output parameter for each child of the loaded scene
    Ogre::SceneNode::ChildNodeIterator itChild = m_sceneNode->getChildIterator();
    while ( itChild.hasMoreElements() )
    {
        // Add OgreContainer to immediate child nodes
        Ogre::SceneNode* pChildNode = dynamic_cast<Ogre::SceneNode*>(itChild.getNext());
        if( pChildNode )
        {
            OgreContainer *container = new OgreContainer(pChildNode);
            pChildNode->setUserAny(Ogre::Any(container));

            // Add an output parameter for each and every node
            GeometryParameter* sceneNodeParameter = new GeometryParameter(QString::fromStdString(pChildNode->getName()).remove("Node"));
            sceneNodeParameter->setValue(QVariant::fromValue<Ogre::SceneNode *>(pChildNode)) ;
            this->addOutputParameter(sceneNodeParameter);
        }
    }


    // get all lights from the input scene and attach to lights scene node
    QList<Ogre::Light*> lightsList;
    OgreTools::getAllLights( m_sceneNode, lightsList);

    if( !lightsList.isEmpty() )
    {
        // create lights scene node
        m_lightsSceneNode = OgreManager::createSceneNode( QString::fromStdString( createUniqueName("LightsSceneNode")));
        m_lightsSceneNode->setUserAny(Ogre::Any(new OgreContainer(m_lightsSceneNode)));

        // add lights
        int i = 0;
        foreach( Ogre::Light* light, lightsList )
        {
            Ogre::Light* lightCopy = dynamic_cast<Ogre::Light*>(OgreTools::cloneMovableObject( light, "SceneLightCopy"+QString::number(i++), OgreManager::getSceneManager()));
            if( lightCopy )
            {
                lightCopy->setPosition( light->getDerivedPosition());
                lightCopy->setDirection( light->getDerivedDirection());
                m_lightsSceneNode->attachObject(lightCopy);
            }
        }


        // Add an output parameter for the lights scene node
        LightParameter* lightSceneNodeParameter = new LightParameter( "LightsSceneNode" );
        lightSceneNodeParameter->setValue( QVariant::fromValue<Ogre::SceneNode*>(m_lightsSceneNode));
        this->addOutputParameter(lightSceneNodeParameter);

    }



    this->setValue(m_outputGeometryName, m_sceneNode);
    emit viewNodeUpdated();

    return true;
}


//!
//! Retrieves the numbers of vertices and triangles from the mesh and stores
//! them in parameters of this node.
//!
void SceneLoaderNode::updateStatistics ()
{
}

///
/// Private Slots
///

//!
//! Change function for the Geometry File parameter.
//!
void SceneLoaderNode::sceneFileChanged ()
{
    // load new mesh and skeleton
    loadScene();
}

} // namespace SceneLoaderNode

