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
//! \file "OgreTools.cpp"
//! \brief Implementation file for OgreTools class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!


#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreTagPoint.h"

namespace Frapper {

///
/// Public Static Functions
///

Ogre::Entity* OgreTools::cloneEntity( Ogre::Entity *entity, const QString &name, Ogre::SceneManager *sceneManager )
{
	Ogre::Entity* entityCopy = sceneManager->createEntity(name.toStdString(), entity->getMesh());

	// Copy material settings
	for (int i=0; i<entity->getNumSubEntities(); i++) {
		const Ogre::SubEntity *subEntity = entity->getSubEntity(i);
		Ogre::SubEntity *subEntityCopy = entityCopy->getSubEntity(i);
		
		if (subEntity->hasCustomParameter(0))
			subEntityCopy->setCustomParameter(0, subEntity->getCustomParameter(0));

		subEntityCopy->setMaterialName(subEntity->getMaterialName());
	}

	Ogre::AnimationStateSet *animationStateSet =  entity->getAllAnimationStates();
	Ogre::AnimationStateSet *animationStateSetCopy = entityCopy->getAllAnimationStates();

	//  SEIM OGRE_BUG:
	//  because of nullpointer exeption during deletion of animation states in the entity copy
	if(animationStateSet) {
		Ogre::AnimationStateIterator animIter = animationStateSet->getAnimationStateIterator();
		while(animIter.hasMoreElements()) {
			Ogre::AnimationState *animState = animIter.getNext();
			Ogre::AnimationState *animStateCopy = animationStateSetCopy->getAnimationState(animState->getAnimationName());
			if(animStateCopy)
				animStateCopy->setLoop(animState->getLoop());
		}
	}

	//  SEIM OGRE_BUG:
	//  original code
	/*if (animationStateSet)
	{
		OGRE_DELETE animationStateSet;
		animationStateSet = OGRE_NEW Ogre::AnimationStateSet(*entity->getAllAnimationStates());
	}*/

	// share skeleton
	if (entity->hasSkeleton()) {
		QMap<Ogre::String, Ogre::Node::Listener *> listenerMap;
		Ogre::Skeleton *skeleton = entity->getSkeleton();
		Ogre::Skeleton::BoneIterator skeletonIter = skeleton->getBoneIterator();

		while(skeletonIter.hasMoreElements()) {
			const Ogre::Bone *bone = skeletonIter.getNext();
			Ogre::Node::Listener *listener = bone->getListener();
			if (listener)
				listenerMap.insert(bone->getName(), listener);
		}

		entity->shareSkeletonInstanceWith(entityCopy);
		entityCopy->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

		const Ogre::Skeleton *skeletonCopy = entityCopy->getSkeleton();
		QMap<Ogre::String, Ogre::Node::Listener *>::iterator listenerMapIter = listenerMap.begin();
		while (listenerMapIter != listenerMap.end()) {
			skeletonCopy->getBone(listenerMapIter.key())->setListener(listenerMapIter.value());
			listenerMapIter++;
		}
	}

	return entityCopy;
}


//!
//! Clones an Ogre::MovableObject.
//!
//! Is needed because OGRE does not provide clone functions for cameras and
//! lights.
//!
//! \param movableObject The object to clone.
//! \param name The name to use for the object.
//! \param sceneManager The scene manager to use for creating the object.
//! \return The cloned object.
//!
Ogre::MovableObject * OgreTools::cloneMovableObject ( Ogre::MovableObject *movableObject, const QString &name, Ogre::SceneManager *sceneManager /* =  0 */ )
{
    // make sure the given object is valid
    if (!movableObject) {
        Log::error("The given movable object is invalid.", "OgreTools::cloneMovableObject");
        return 0;
    }

    // make sure a valid scene manager is available
    if (!sceneManager)
        sceneManager = movableObject->_getManager();
    if (!sceneManager) {
        Log::error("No valid scene manager available.", "OgreTools::cloneMovableObject");
        return 0;
    }

    Ogre::MovableObject *result = 0;
    Ogre::String typeName = movableObject->getMovableType();

    try
    {
        if (typeName == "Entity") {
			// TODO: NILS: introduced new instancing method in order to propagate mesh to other nodes
			Ogre::Entity *entity = dynamic_cast<Ogre::Entity *>(movableObject);			
			
			// disabled because it uses the wrong scene manager 
			//Ogre::Entity *entityCopy = entity->clone(name.toStdString());
			
			Ogre::Entity *entityCopy = cloneEntity(entity, name, sceneManager);		

			// in case visibility has been changed in the original entity
			entityCopy->setVisibilityFlags(entity->getVisibilityFlags());

			result = dynamic_cast<Ogre::MovableObject *>(entityCopy);
        }
		else if (typeName == "ManualObject")
		{
			Ogre::ManualObject *manualObj = dynamic_cast<Ogre::ManualObject *>(movableObject);
			Ogre::ManualObject *manualObjCopy = sceneManager->createManualObject(name.toStdString());

			// in case visibility has been changed in the original entity
			manualObjCopy->setVisibilityFlags(manualObj->getVisibilityFlags());
			
			for (unsigned int i=0; i<manualObj->getNumSections(); ++i) {
				Ogre::ManualObject::ManualObjectSection *section = manualObj->getSection(i);

				Ogre::VertexData *vertexData = section->getRenderOperation()->vertexData;
				
				const size_t vertexElementCount = vertexData->vertexDeclaration->getElementCount();
				const Ogre::String &materialName = section->getMaterialName();
				const Ogre::RenderOperation::OperationType operationType = section->getRenderOperation()->operationType;
				const Ogre::HardwareVertexBufferSharedPtr bufferPtr = vertexData->vertexBufferBinding->getBuffer(0);
				
				void* vertexBuffer0Data = bufferPtr->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
				float* vertexBuffer0PosFloat = static_cast<float*>(vertexBuffer0Data);
				Ogre::uchar* vertexBuffer0PosChar = static_cast<Ogre::uchar*>(vertexBuffer0Data);

				manualObjCopy->begin(materialName, operationType);
				switch (vertexElementCount) {
					case 1: {
						// getVertexSize is the size in bytes of the vertex. Vertex = 3 values * 4 bytes, but we need is just the number of values
						for(int jf=0; jf<vertexData->vertexCount*(bufferPtr->getVertexSize()/4); jf+=3) {
							float f1 = vertexBuffer0PosFloat[jf];
							float f2 = vertexBuffer0PosFloat[jf+1];
							float f3 = vertexBuffer0PosFloat[jf+2];
							manualObjCopy->position(
								vertexBuffer0PosFloat[jf],
								vertexBuffer0PosFloat[jf+1],
								vertexBuffer0PosFloat[jf+2] );
						}
						break;
					}
					case 2: {
						for(int j=0, jf=0; j<vertexData->vertexCount*bufferPtr->getVertexSize()-16; j+=16, jf+=4) {
							manualObjCopy->position(
								vertexBuffer0PosFloat[jf],
								vertexBuffer0PosFloat[jf+1],
								vertexBuffer0PosFloat[jf+2] );
							manualObjCopy->colour(
								static_cast<float>(vertexBuffer0PosChar[j+12]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+13]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+14]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+15]/255.f) );
						}
						break;
					}
					case 3: {
						for(int j=0, jf=0; j<vertexData->vertexCount*bufferPtr->getVertexSize()-28; j+=28, jf+=7) {
							manualObjCopy->position(
								vertexBuffer0PosFloat[jf],
								vertexBuffer0PosFloat[jf+1],
								vertexBuffer0PosFloat[jf+2] );
							manualObjCopy->colour(
								static_cast<float>(vertexBuffer0PosChar[j+12]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+13]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+14]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+15]/255.f) );
							manualObjCopy->normal(
								vertexBuffer0PosFloat[jf+4],
								vertexBuffer0PosFloat[jf+5],
								vertexBuffer0PosFloat[jf+6]);
						}
						break;
					}
					case 4: {
						for(int j=0, jf=0; j<vertexData->vertexCount*bufferPtr->getVertexSize()-40; j+=40, jf+=10) {
							manualObjCopy->position(
								vertexBuffer0PosFloat[jf],
								vertexBuffer0PosFloat[jf+1],
								vertexBuffer0PosFloat[jf+2] );
							manualObjCopy->normal(
								vertexBuffer0PosFloat[jf+3],
								vertexBuffer0PosFloat[jf+4],
								vertexBuffer0PosFloat[jf+5]);
							manualObjCopy->colour(
								static_cast<float>(vertexBuffer0PosChar[j+12]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+13]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+14]/255.f),
								static_cast<float>(vertexBuffer0PosChar[j+15]/255.f) );
							manualObjCopy->textureCoord(
								vertexBuffer0PosFloat[jf+7],
								vertexBuffer0PosFloat[jf+8],
								vertexBuffer0PosFloat[jf+9]);
						}
						break;
					}
					default: {
						Log::error(QString("Max supported attributes size is 4"), "OgreTools::cloneMovableObject");
						break;
					}
				}  // end switch block
				manualObjCopy->end();
				bufferPtr->unlock();
			}

			// create a new container for the cloned manual object
			OgreContainer *manualObjCopyContainer = new OgreContainer(manualObjCopy);
			manualObjCopy->setUserAny(Ogre::Any(manualObjCopyContainer));

			if (!manualObj->getUserAny().isEmpty()) 
            {
                try
                {
                    OgreContainer *manualObjContainer = Ogre::any_cast<OgreContainer *>(manualObj->getUserAny());
                    if (manualObjContainer) 
                    {
						QObject::connect( manualObjContainer, SIGNAL(vertexBufferUpdated(ParameterGroup *)), manualObjCopyContainer, SLOT(updateVertexBuffer(ParameterGroup *)));
                    }
                }
                catch (Ogre::Exception& e)
                {
					Frapper::Log::error(QString::fromStdString( "Ogre::any_cast to OgreContainer for entity "+manualObj->getName()+" failed: "+e.getDescription()), "OgreTools::cloneMoveableObject");
                }
            }

			result = dynamic_cast<Ogre::MovableObject *>(manualObjCopy);
		}
        else if (typeName == "Light") 
        {
            // clone light
            Ogre::Light *light = dynamic_cast<Ogre::Light *>(movableObject);
            Ogre::Light *lightCopy = sceneManager->createLight(name.toStdString());
            lightCopy->setType(light->getType());
            lightCopy->setDiffuseColour(light->getDiffuseColour());
            lightCopy->setSpecularColour(light->getSpecularColour());
            lightCopy->setAttenuation(light->getAttenuationRange(), light->getAttenuationConstant(), light->getAttenuationLinear(), light->getAttenuationQuadric());
            lightCopy->setPosition(light->getPosition());
            lightCopy->setDirection(light->getDirection());
			lightCopy->setVisible(light->getVisible());
            if (lightCopy->getType() == Ogre::Light::LT_SPOTLIGHT)
                lightCopy->setSpotlightRange(light->getSpotlightInnerAngle(), light->getSpotlightOuterAngle(), light->getSpotlightFalloff());
            lightCopy->setPowerScale(light->getPowerScale());
            lightCopy->setCastShadows(light->getCastShadows());

            // create a new container for the cloned light
            OgreContainer *lightCopyContainer = new OgreContainer(lightCopy);
            lightCopy->setUserAny(Ogre::Any(lightCopyContainer));
            if (!light->getUserAny().isEmpty()) 
            {
                try
                {
                    OgreContainer *lightContainer = Ogre::any_cast<OgreContainer *>(light->getUserAny());
                    if (lightContainer && lightContainer->getLight())
                        QObject::connect(lightContainer, SIGNAL(sceneNodeUpdated()), lightCopyContainer, SLOT(updateLight()));
                }
                catch (Ogre::Exception& e)
                {
                    Frapper::Log::error(QString::fromStdString( "Ogre::any_cast to OgreContainer for light "+light->getName()+" failed: "+e.getDescription()), "OgreTools::cloneMoveableObject");
                }
            }
            result = dynamic_cast<Ogre::MovableObject *>(lightCopy);
        } 
        else if (typeName == "Camera") 
        {
            // clone camera
            Ogre::Camera *camera = dynamic_cast<Ogre::Camera *>(movableObject);
            Ogre::Camera *cameraCopy = sceneManager->createCamera(name.toStdString());
            cameraCopy->setAspectRatio(camera->getAspectRatio());
            cameraCopy->setAutoAspectRatio(camera->getAutoAspectRatio());
            cameraCopy->setCastShadows(camera->getCastsShadows());
            cameraCopy->setDirection(camera->getDirection());
            cameraCopy->setFocalLength(camera->getFocalLength());
            cameraCopy->setFOVy(camera->getFOVy());

            cameraCopy->setLodBias(camera->getLodBias());
            cameraCopy->setNearClipDistance(camera->getNearClipDistance());
            cameraCopy->setFarClipDistance(camera->getFarClipDistance());
            cameraCopy->setOrientation(camera->getOrientation());
            cameraCopy->setPolygonMode(camera->getPolygonMode());
            cameraCopy->setPolygonModeOverrideable(camera->getPolygonModeOverrideable());
            cameraCopy->setPosition(camera->getPosition());
            cameraCopy->setProjectionType(camera->getProjectionType());
            cameraCopy->setQueryFlags(camera->getQueryFlags());
            cameraCopy->setRenderingDistance(camera->getRenderingDistance());
            cameraCopy->setRenderQueueGroup(camera->getRenderQueueGroup());
            cameraCopy->setUseIdentityProjection(camera->getUseIdentityProjection());
            cameraCopy->setUseIdentityView(camera->getUseIdentityView());
            cameraCopy->setUseRenderingDistance(camera->getUseRenderingDistance());
            cameraCopy->setVisibilityFlags(camera->getVisibilityFlags());
            cameraCopy->setVisible(camera->getVisible());
         
			/*cameraCopy->setWindow(...);
			cameraCopy->setCustomParameter(0, camera->getCustomParameter(0));
			cameraCopy->setAutoTracking(...);
			cameraCopy->setCullingFrustum(camera->getCullingFrustum());
			cameraCopy->setCustomParameter(...);
			cameraCopy->setCustomProjectionMatrix(..);
			cameraCopy->setCustomViewMatrix(..);
			cameraCopy->setDebugDisplayEnabled(...);
			cameraCopy->setDefaultQueryFlags(...);
			cameraCopy->setDefaultVisibilityFlags(...);
			cameraCopy->setFixedYawAxis(...);
			Ogre::Real left, right, top, bottom;
			camera->getFrustumExtents(left, right, top, bottom);
			cameraCopy->setFrustumExtents(left, right, top, bottom);
			cameraCopy->setFrustumOffset(camera->getFrustumOffset());
			cameraCopy->setListener(camera->getListener());
			cameraCopy->setLodCamera(camera->getLodCamera());
			cameraCopy->setOrthoWindow(...);
			cameraCopy->setOrthoWindowHeight(...);
			cameraCopy->setOrthoWindowWidth(...);
			cameraCopy->setRenderSystemData(camera->getRenderSystemData());
			cameraCopy->setUserAny(camera->getUserAny());*/

			Ogre::MovableObject *cameraMo = dynamic_cast<Ogre::MovableObject *>(camera);
			Ogre::MovableObject * cameraCopyMo = dynamic_cast<Ogre::MovableObject *>(cameraCopy);
			if (!cameraMo->getUserAny().isEmpty()) 
			{
				try
				{
					OgreContainer *cameraContainer = Ogre::any_cast<OgreContainer *>(cameraMo->getUserAny());
					OgreContainer *cameraCopyContainer = new OgreContainer(cameraCopy, cameraContainer->getCameraWidth(), cameraContainer->getCameraHeight());
					cameraCopyMo->setUserAny(Ogre::Any(cameraCopyContainer));
					if (cameraContainer && cameraContainer->getCamera())
						QObject::connect(cameraContainer, SIGNAL(sceneNodeUpdated()), cameraCopyContainer, SLOT(updateCamera()));
				}
				catch (Ogre::Exception& e)
				{
					Frapper::Log::error(QString::fromStdString( "Ogre::any_cast to OgreContainer for light "+camera->getName()+" failed: "+e.getDescription()), "OgreTools::cloneMoveableObject");
				}
			}

			result = dynamic_cast<Ogre::MovableObject *>(cameraCopy);
		}
	}
	catch (Ogre::Exception& e)
	{
		Log::error(QString::fromStdString("Could not clone movable object "+movableObject->getName()+" of type "+typeName+": "+e.getDescription()), "OgreTools::cloneMovableObject");
		return NULL;
	}

	return result;
}


//!
//! Creates a deep copy of the given scene node.
//!
//! \param sceneNode The scene node to copy.
//! \param sceneNodeCopy The scene node to add the copied objects to (will be created if 0).
//! \param namePrefix The prefix to use for names of copied objects.
//! \param sceneManager The scene manager to use for creating the object.
//!
void OgreTools::deepCopySceneNode ( Ogre::SceneNode *sceneNode, Ogre::SceneNode *&sceneNodeCopy, const QString &namePrefix, Ogre::SceneManager *sceneManager /* = 0 */ )
{
    // make sure the given scene node is valid
    if (!sceneNode) {
        Log::error("The given scene node is invalid.", "OgreTools::deepCopySceneNode");
        return;
    }

    // make sure a valid scene manager is available
    if (!sceneManager)
        sceneManager = sceneNode->getCreator();
    if (!sceneManager) {
        Log::error("No valid scene manager available.", "OgreTools::deepCopySceneNode");
        return;
    }

    // create the target scene node if it doesn't exist yet
    if (!sceneNodeCopy) {
        QString sceneNodeCopyName = QString("%1_%2Copy").arg(namePrefix).arg(sceneNode->getName().c_str());
        sceneNodeCopy = copySceneNode(sceneNode, sceneNodeCopyName, sceneManager);
        if (!sceneNodeCopy) {
            Log::error("The scene node could not be copied.", "OgreTools::deepCopySceneNode");
            return;
        }
    }

    // iterate over the list of attached objects
    Ogre::SceneNode::ObjectIterator objectIterator = sceneNode->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements()) {
        Ogre::MovableObject *movableObject = objectIterator.getNext();
        if (movableObject) {
            QString entityCopyName = QString("%1_%2Copy").arg(namePrefix).arg(movableObject->getName().c_str());
            Ogre::MovableObject *movableObjectCopy = cloneMovableObject(movableObject, entityCopyName, sceneManager);
            if (movableObjectCopy)
                sceneNodeCopy->attachObject(movableObjectCopy);
        }
    }

    // iterate over the list of child nodes
    Ogre::SceneNode::ChildNodeIterator childNodeIterator = sceneNode->getChildIterator();
    while (childNodeIterator.hasMoreElements() ) {
        Ogre::SceneNode *childSceneNode = (Ogre::SceneNode *) childNodeIterator.getNext();
        QString childSceneNodeCopyName = QString("%1_%2Copy").arg(namePrefix).arg(childSceneNode->getName().c_str());
        Ogre::SceneNode *childSceneNodeCopy = copySceneNode(childSceneNode, childSceneNodeCopyName, sceneManager);
        if (childSceneNodeCopy) {
            sceneNodeCopy->addChild(childSceneNodeCopy);
            deepCopySceneNode(childSceneNode, childSceneNodeCopy, namePrefix, sceneManager);
        }
    }
}


//!
//! Creates a copy of the given scene node.
//!
//! \param sceneNode The scene node to copy.
//! \param name The name to use for the copied scene node.
//! \param sceneManager The scene manager to use for creating the scene node.
//! \return A copy of the given scene node.
//!
Ogre::SceneNode * OgreTools::copySceneNode ( Ogre::SceneNode *sceneNode, const QString &name, Ogre::SceneManager *sceneManager /* = 0 */ )
{
    // make sure the given scene node is valid
    if (!sceneNode) {
        Log::error("The given scene node is invalid.", "OgreTools::copySceneNode");
        return 0;
    }

    // make sure a valid scene manager is available
    if (!sceneManager)
        sceneManager = sceneNode->getCreator();
    if (!sceneManager) {
        Log::error("No valid scene manager available.", "OgreTools::copySceneNode");
        return 0;
    }

    // check if a scene node of the given name already exists
    if (sceneManager->hasSceneNode(name.toStdString())) {
        Log::error(QString("The scene manager already contains a scene node named \"%1\".").arg(name), "OgreTools::copySceneNode");
        return 0;
    }

    // create the scene node copy
    Ogre::SceneNode *sceneNodeCopy = sceneManager->createSceneNode(name.toStdString());
    if (!sceneNodeCopy) {
        Log::error("The scene node copy could not be created.", "OgreTools::copySceneNode");
        return 0;
    }

    // create a container for the scene node copy
    OgreContainer *sceneNodeCopyContainer = new OgreContainer(sceneNodeCopy);
    sceneNodeCopy->setUserAny(Ogre::Any(sceneNodeCopyContainer));

    if (!sceneNode->getUserAny().isEmpty()) 
    {
        try
        {
            OgreContainer *sceneNodeContainer = Ogre::any_cast<OgreContainer *>(sceneNode->getUserAny());
            if (sceneNodeContainer)
            {
                // connect update signal of scene node with update slot of copy
                if(sceneNodeContainer->getSceneNode())
                    QObject::connect(sceneNodeContainer, SIGNAL(sceneNodeUpdated()), sceneNodeCopyContainer, SLOT(updateSceneNode()));

                // connect update signal of light with update slot of copy
                if(sceneNodeContainer->getLight())
                    QObject::connect(sceneNodeContainer, SIGNAL(sceneNodeUpdated()), sceneNodeCopyContainer, SLOT(updateLight()));

                // connect update signal of camera with update slot of copy
                if(sceneNodeContainer->getCamera())
                    QObject::connect(sceneNodeContainer, SIGNAL(sceneNodeUpdated()), sceneNodeCopyContainer, SLOT(updateCamera()));
            }
        }
        catch (Ogre::Exception& e)
        {
            QString sceneNodeName = QString::fromStdString(sceneNode->getName());
            Frapper::Log::error("Ogre::any_cast to OgreContainer for "+sceneNodeName+" failed:"+QString::fromStdString(e.getFullDescription()), "OgreTools::copySceneNode");
        }
    }


    // copy parameters from scene node to scene node copy
    //sceneNodeCopy->setAutoTracking(...);
    //sceneNodeCopy->setCustomParameter(...);
    //sceneNodeCopy->setDebugDisplayEnabled(...);
    //sceneNodeCopy->setDirection(...);
    //sceneNodeCopy->setFixedYawAxis(...);
    sceneNodeCopy->setInheritOrientation(sceneNode->getInheritOrientation());
    sceneNodeCopy->setInheritScale(sceneNode->getInheritScale());
    //sceneNodeCopy->setInitialState(...);
    //sceneNodeCopy->setInSceneGraph(...);
    sceneNodeCopy->setListener(sceneNode->getListener());
    sceneNodeCopy->setOrientation(sceneNode->getOrientation());
    //sceneNodeCopy->setParent(...);

#if (OGRE_VERSION < 0x010700)
    sceneNodeCopy->setPolygonModeOverrideable(sceneNode->getPolygonModeOverrideable());
#endif

    sceneNodeCopy->setPosition(sceneNode->getPosition());
    //sceneNodeCopy->setRenderSystemData(...);
    sceneNodeCopy->setScale(sceneNode->getScale());

#if (OGRE_VERSION < 0x010700)
    sceneNodeCopy->setUseIdentityProjection(sceneNode->getUseIdentityProjection());
    sceneNodeCopy->setUseIdentityView(sceneNode->getUseIdentityView());
#endif

    //sceneNodeCopy->getUserAny(...);
    //sceneNodeCopy->setVisible(...);
    return sceneNodeCopy;
}

//!
//! Deletes a whole scene node tree (including attached objects).
//!
//! \param sceneNode The scene node containing the tree to delete.
//! \param sceneManager The scene manager to use for destroying the scene node.
//! \param deleteRoot Flag to control whether to delete the given scene node.
//!
void OgreTools::deepDeleteSceneNode ( Ogre::SceneNode *sceneNode, Ogre::SceneManager *sceneManager /* = 0 */, bool deleteRoot /* = false */ )
{
    // make sure the given scene node is valid
    if (!sceneNode) {
        Log::error("The given scene node is invalid.", "OgreTools::deepDeleteSceneNode");
        return;
    }

    // make sure a valid scene manager is available
    if (!sceneManager)
        sceneManager = sceneNode->getCreator();
    if (!sceneManager) {
        Log::error("No valid scene manager available.", "OgreTools::deepDeleteSceneNode");
        return;
    }	

	// iterate over the list of attached objects
	Ogre::SceneNode::ObjectIterator objectIterator = sceneNode->getAttachedObjectIterator();
	while (objectIterator.hasMoreElements()) {
		Ogre::String objectName = "";
		try
		{
			Ogre::MovableObject *movableObject = objectIterator.getNext();
			objectName = movableObject->getName();

			// Delete also attached children
			Ogre::Entity *entity = dynamic_cast<Ogre::Entity *>(movableObject);
			if (entity)
			{
				Ogre::Entity::ChildObjectListIterator childIter = entity->getAttachedObjectIterator();
				while( childIter.hasMoreElements() )
				{
					Ogre::MovableObject *movableChild = childIter.getNext();				
					sceneManager->destroyMovableObject(movableChild);
				}
			}
			
			sceneNode->detachObject(movableObject);
			deleteUserAnyFromMovableObject(movableObject);

			Ogre::Camera *camera = dynamic_cast<Ogre::Camera *>(movableObject);
			if( camera ) {
				sceneManager->destroyCamera(camera);
				continue;
			}

			Ogre::Light *light = dynamic_cast<Ogre::Light *>(movableObject);
			if( light ) {
				sceneManager->destroyLight(light);
				continue;
			}

			Ogre::InstancedEntity *instancedEntity = dynamic_cast<Ogre::InstancedEntity *>(movableObject);
			if (instancedEntity) {
				sceneManager->destroyInstancedEntity(instancedEntity);
			}

			if( entity ) {
				if (entity->getMesh()->hasSkeleton() && entity->sharesSkeletonInstance()) {
					entity->stopSharingSkeletonInstance();
					entity->detachAllObjectsFromBone();
				}
				sceneManager->destroyEntity(entity);
				continue;
			}

			//Ogre::SceneManager *manager = movableObject->_getManager();
			//if (manager) {
			//	manager->destroyMovableObject(movableObject);
			//	continue;
			//}

			sceneManager->destroyMovableObject(movableObject);
		}
		catch ( Ogre::Exception& e) {
			Frapper::Log::error( QString::fromStdString("Deleting object "+objectName+" failed: " + e.getDescription()), "OgreTools::deepDeleteSceneNode");
		}
    }

    // iterate over the list of child nodes
    Ogre::SceneNode::ChildNodeIterator childNodeIterator = sceneNode->getChildIterator();
    while (childNodeIterator.hasMoreElements()) {
        Ogre::SceneNode *childSceneNode = dynamic_cast<Ogre::SceneNode *>(childNodeIterator.getNext());
        if (childSceneNode)
            deepDeleteSceneNode(childSceneNode, sceneManager, true);
    }


    // destroy all child nodes of the given scene node
    sceneNode->removeAndDestroyAllChildren();

    // check if the given scene node should be destroyed as well
    if (deleteRoot) {
        deleteUserAnyFromSceneNode(sceneNode);
        sceneManager->destroySceneNode(sceneNode);
        sceneNode = NULL;
    }
 }


//!
//! Returns the first entity attached to the given scene node.
//!
//! \param sceneNode The scene node to return the entity from.
//! \return The first entity attached to the given scene node.
//!
Ogre::Entity * OgreTools::getFirstEntity ( Ogre::SceneNode *sceneNode )
{
    return findFirstObject<Ogre::Entity *>(sceneNode);
}

//!
//! Returns the first camera attached to the given scene node.
//!
//! \param sceneNode The scene node to return the camera from.
//! \return The first camera attached to the given scene node.
//!
Ogre::Camera * OgreTools::getFirstCamera ( Ogre::SceneNode *sceneNode )
{
    return findFirstObject<Ogre::Camera *>(sceneNode);
}

//!
//! Returns the first light attached to the given scene node.
//!
//! \param sceneNode The scene node to return the light from.
//! \return The first light attached to the given scene node.
//!
Ogre::Light * OgreTools::getFirstLight ( Ogre::SceneNode *sceneNode )
{
    return findFirstObject<Ogre::Light *>(sceneNode);
}

//!
//! Returns the first movable object attached to the given scene node.
//!
//! \param sceneNode The scene node to return the light from.
//! \return The first light attached to the given scene node.
//!
Ogre::MovableObject* OgreTools::getFirstMovableObject( Ogre::SceneNode *sceneNode )
{
    return findFirstObject<Ogre::MovableObject*>(sceneNode);
}

///
/// Private Static Functions
///


//!
//! Returns the first movable object of the given type name contained in
//! the given scene node.
//!
//! \param sceneNode The scene node to find the first object in.
//! \param typeName The name of the type of objects to look for.
//! \return The first movable object of the given type name contained in the given scene node.
//!
template <typename T>
T OgreTools::findFirstObject ( Ogre::SceneNode *sceneNode )
{
    // make sure the given scene node is valid
    if (!sceneNode) {
        Log::error("The given scene node is invalid.", "OgreTools::findFirstObject");
        return 0;
    }

    // iterate over the list of attached objects
    Ogre::SceneNode::ObjectIterator objectIterator = sceneNode->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements()) {
        T object = dynamic_cast<T>(objectIterator.getNext());
        if(object) return object;
    }

    // iterate over the list of child nodes
    Ogre::SceneNode::ChildNodeIterator childIter = sceneNode->getChildIterator();
    while( childIter.hasMoreElements() ) {
        Ogre::SceneNode *childSceneNode = (Ogre::SceneNode *) childIter.getNext();
        return findFirstObject<T>( childSceneNode );
    }
    return 0;
}

void OgreTools::getAllLights( Ogre::SceneNode* sceneNode, QList<Ogre::Light*>& lightsList )
{
    findAllObjects<Ogre::Light*>( sceneNode, lightsList);
}
void OgreTools::getAllCameras( Ogre::SceneNode* sceneNode, QList<Ogre::Camera*>& camerasList )
{
    findAllObjects<Ogre::Camera*>( sceneNode, camerasList);
}
void OgreTools::getAllEntities( Ogre::SceneNode* sceneNode, QList<Ogre::Entity*>& entityList )
{
    findAllObjects<Ogre::Entity*>( sceneNode, entityList);
}

//!
//! Returns all movable objects of the given type contained in
//! the given scene node and its ancestors.
//!
//! \param sceneNode The scene node to find all objects in.
//! \return The first movable object of the given type name contained in the given scene node.
//!
template <typename T>
static void OgreTools::findAllObjects ( Ogre::SceneNode *sceneNode, QList<T>& objectsList )
{
    // make sure the given scene node is valid
    if (!sceneNode) {
        Log::error("The given scene node is invalid.", "OgreTools::findAllObjects");
    }

    // iterate over the list of attached objects
    Ogre::SceneNode::ObjectIterator objectIterator = sceneNode->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements()) 
    {
        T object = dynamic_cast<T>(objectIterator.getNext());
        if ( object )
            objectsList.append(object);
    }

    // iterate over the list of child nodes
    Ogre::SceneNode::ChildNodeIterator childIter = sceneNode->getChildIterator();
    while( childIter.hasMoreElements() ) {
        Ogre::SceneNode *childSceneNode = dynamic_cast<Ogre::SceneNode*>(childIter.getNext());
        findAllObjects<T>( childSceneNode, objectsList );
    }
}

//!
//! Destroy the node's Ogre resource group.
//!
void OgreTools::destroyResourceGroup ( const QString &name )
{
    Ogre::StringVector groupNames = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
    if (std::find(groupNames.begin(), groupNames.end(), name.toStdString()) != groupNames.end()) {
        Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(name.toStdString());
    }
}

//!
//! Create the node's Ogre resource group.
//!
void OgreTools::createResourceGroup ( const QString &name, const QString &path )
{
    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(name.toStdString());
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path.toStdString(), "FileSystem", name.toStdString());
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(name.toStdString());
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(name.toStdString());
}

void OgreTools::getSceneNodesByName( Ogre::SceneNode* root, QString name, QList<Ogre::SceneNode*>& sceneNodes )
{
    // make sure the given scene node is valid
    if (!root)
        return;

    if( QString::fromStdString(root->getName()).contains(name, Qt::CaseInsensitive) )
    {
        sceneNodes.append(root);
    }

    // iterate over the list of child nodes
    Ogre::SceneNode::ChildNodeIterator childNodeIterator = root->getChildIterator();
    while (childNodeIterator.hasMoreElements() ) 
    {
        Ogre::SceneNode *child = (Ogre::SceneNode *) childNodeIterator.getNext();
        getSceneNodesByName( child, name, sceneNodes);
    }
}

OgreContainer* OgreTools::getOgreContainer( Ogre::SceneNode* sceneNode )
{
    if( !sceneNode )
    {
        Frapper::Log::error( "OgreTools::getOgreContainer called with undefined scene node!", "OgreTools::getOgreContainer");
        return NULL;
    }

    OgreContainer* result = NULL;

    const Ogre::Any& sceneNodeAny = sceneNode->getUserAny();
    if( sceneNodeAny.isEmpty())
    {
        result = new OgreContainer( sceneNode );
        sceneNode->setUserAny( Ogre::Any( result ));
    }
    else
    {
        try 
        {
            result = Ogre::any_cast<OgreContainer*>(sceneNodeAny);
        }
        catch( Ogre::Exception& e)
        {
            Frapper::Log::error( QString::fromStdString("Ogre::Any_cast for scene node "+sceneNode->getName()+" failed: "+e.getDescription()),"OgreTools::getOgreContainer" );
        }
    }
    return result;
}


 template <typename T>
 void OgreTools::deleteUserAnyFrom( T t )
 {
     if( !t)
         return;

     QString objectName = QString::fromStdString(t->getName());

     // delete user any, if any
     const Ogre::Any& userAny = t->getUserAny();

     if (!userAny.isEmpty()) 
     {
         if( userAny.getType() == typeid( OgreContainer* ) )
         {
             try {
                 OgreContainer *ogreContainer = Ogre::any_cast<OgreContainer *>(userAny);
                 delete ogreContainer;
             }
             catch (Ogre::Exception& e ) {
                 Frapper::Log::error("Ogre::any_cast to OgreContainer* for movableObject \""+objectName+"\" failed:"+QString::fromStdString( e.getDescription() ), "OgreTools::deleteUserAnyFrom");
             }
         }
     }
 }
 
void OgreTools::deleteUserAnyFromSceneNode( Ogre::SceneNode *sceneNode )
{
    deleteUserAnyFrom<Ogre::SceneNode*>(sceneNode);
}

void OgreTools::deleteUserAnyFromMovableObject( Ogre::MovableObject *movableObject )
{
    deleteUserAnyFrom<Ogre::MovableObject*>(movableObject);
}

void OgreTools::OgreTextureToQImage( const Ogre::TexturePtr& src, QImage &dst )
{
    if( src.isNull() )
        return;

	size_t w = src->getWidth();
	size_t h = src->getHeight();

	if( src->getFormat() == Ogre::PF_L8 ) {

		size_t pixDataSize = Ogre::PixelUtil::getMemorySize( w, h, 1, Ogre::PF_L8 );
		uchar* pixData = OGRE_ALLOC_T( uchar, pixDataSize, Ogre::MEMCATEGORY_GENERAL);
		Ogre::PixelBox pixBox( w, h, 1, Ogre::PF_L8, pixData);
		src->getBuffer(0, 0)->blitToMemory(pixBox);

		dst = QImage( w, h, QImage::Format_ARGB32);
		uchar* pPixData = pixData;

		// copy from pixData to dst image
		for( size_t y = 0; y < h; ++y ) 
		{
			QRgb* dstColor = (QRgb*)dst.scanLine(y);
			for( size_t x = 0; x < w; ++x) {
				uchar srcColor = *pPixData;
				dstColor[x] = qRgb( srcColor, srcColor, srcColor );
				pPixData++;
			}
		}

		OGRE_FREE( pixData, Ogre::MEMCATEGORY_GENERAL);

	} else {

		dst = QImage( w, h, QImage::Format_ARGB32);
		Ogre::PixelBox pixBox( w, h, 1, Ogre::PixelFormat::PF_A8R8G8B8, dst.bits());
		src->getBuffer(0, 0)->blitToMemory(pixBox);
	}
}

void OgreTools::OgreTextureToQImageMask( const Ogre::TexturePtr& src, QImage &dst, QRgb maskColor /*= 0*/, QRgb outColor /*= 0 */ )
{
	QImage inter;
	OgreTextureToQImage( src, inter );

	if( inter.isNull() )
		return;

	bool useColor = outColor != 0;

	dst = QImage( inter.width(), inter.height(), QImage::Format_ARGB32);

	if( inter.format() == QImage::Format_ARGB32 || 
		inter.format() == QImage::Format_ARGB32_Premultiplied )
	{
		QRgb maskColorRGB = ( maskColor & RGB_MASK );

		// copy loops
		for( size_t y = 0; y < inter.height(); ++y) 
		{
			QRgb* srcCol = (QRgb*) inter.scanLine(y);
			QRgb* dstCol = (QRgb*) dst.scanLine(y);

			for( size_t x = 0; x < inter.width(); ++x) 
			{
				if( (srcCol[x] & RGB_MASK ) == maskColorRGB ) 
				{
					dstCol[x] = 0; // black & transparent
				} 
				else 
				{
					if( useColor )
						dstCol[x] = outColor; // non-black pixel -> set to input color
					else
						dstCol[x] = srcCol[x];
				}
			}
		}
	}
	else
	{
		for( size_t y = 0; y < inter.height(); ++y) 
		{
			uchar* srcCol = inter.scanLine(y);
			QRgb* dstCol = (QRgb*)dst.scanLine(y);

			for( size_t x = 0; x < inter.width(); ++x) 
			{
				if( srcCol[x] == maskColor ) 
				{
					dstCol[x] = 0; // black & transparent
				} 
				else 
				{
					if( useColor ) {
						dstCol[x] = outColor;
					} else {
						dstCol[x] = qRgba( srcCol[x], srcCol[x], srcCol[x], 255 );
					}
				}
			}
		}
	}
}

void OgreTools::QImageToOgreTexture( const QImage &src, Ogre::TexturePtr& dst, const Ogre::PixelFormat& format /*= Ogre::PF_A8R8G8B8*/ )
{
    if (src.isNull() || dst.isNull())
        return;

	if( ! (src.format() == QImage::Format_ARGB32 || src.format() == QImage::Format_ARGB32_Premultiplied )){
		Frapper::Log::error(QString("Function currently only supports QImages of format QImage::Format_ARGB32!"), "OgreTools::QImageToOgreTexture");
		return;
	}

	// buffer info
	const uint width    = src.width();
	const uint height   = src.height();

	Ogre::String textureName = dst->getName();
	if( width  != dst->getWidth()  ||
		height != dst->getHeight() ||
		format != dst->getFormat() )
	{
		// re-/create ogre image buffer
		Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
		if (textureManager.resourceExists( textureName )) {
			textureManager.remove( textureName );
		}

		dst = textureManager.createManual(
			textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			Ogre::TEX_TYPE_2D, width, height, 0, format, Ogre::TU_DYNAMIC_WRITE_ONLY );
	}

	HardwareBufferLocker hbl( dst->getBuffer(),  Ogre::HardwareBuffer::HBL_DISCARD);

	// buffer data
	const Ogre::PixelBox &pixBox = hbl.getCurrentLock();

	uchar* pixelData = static_cast<uchar*>(pixBox.data);
	ulong pxDataIndex = 0;
	ulong pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes( format ); 

	const uchar* qImageData = src.constBits();
	ulong qxDataIndex = 0;
	ulong qxDataIndexStep = 4;  // at least for QImage::Format_ARGB32..


	// copy loops
	for(uint y = 0; y < height; ++y) 
	{
		const QRgb* col = ( const QRgb*) src.scanLine(y);

		for(uint x = 0; x < width; ++x) 
		{
			if( format == Ogre::PF_L8 ) // treat as mask
			{
				*(static_cast<Ogre::uint8*>(pixelData + pxDataIndex)) = static_cast<Ogre::uint8>( qGray(col[x]));
			}
			else // copy all available channels
			{
				QColor qcolor = QColor::fromRgba(col[x]);
				Ogre::PixelUtil::packColour( qcolor.redF(), qcolor.greenF(), qcolor.blueF(), 1.0f, format, static_cast<void*> (pixelData + pxDataIndex));
			}
			pxDataIndex += pxDataIndexStep;
		}
	}
}
} // end namespace Frapper