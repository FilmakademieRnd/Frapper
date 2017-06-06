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
//! \file "OgreContainer.cpp"
//! \brief Implementation file for OgreContainer class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.04.2009 (last updated)
//!

#include "OgreContainer.h"

namespace Frapper {

INIT_INSTANCE_COUNTER(OgreContainer)
Q_DECLARE_METATYPE(QVector<float>);

///
/// Constructors and Destructor
///


//!
//! Constructor of the OgreContainer class.
//!
//! \param sceneNode The scene node to be contained in the container.
//!
OgreContainer::OgreContainer ( Ogre::SceneNode *sceneNode ) :
m_sceneNode(sceneNode),
m_entity(0),
m_manualObject(0),
m_light(0),
m_camera(0),
m_cameraWidth(0),
m_cameraHeight(0)
{
    INC_INSTANCE_COUNTER
}


//!
//! Constructor of the OgreContainer class.
//!
//! \param entity The entity to be contained in the container.
//!
OgreContainer::OgreContainer ( Ogre::Entity *entity ) :
m_sceneNode(0),
m_entity(entity),
m_manualObject(0),
m_light(0),
m_camera(0),
m_cameraWidth(0),
m_cameraHeight(0)
{
    INC_INSTANCE_COUNTER
}


//!
//! Constructor of the OgreContainer class.
//!
//! \param manualObject The manual object to be contained in the container.
//!
OgreContainer::OgreContainer ( Ogre::ManualObject *manualObject ) :
m_sceneNode(0),
m_entity(0),
m_manualObject(manualObject),
m_light(0),
m_camera(0),
m_cameraWidth(0),
m_cameraHeight(0)
{
    INC_INSTANCE_COUNTER
}


//!
//! Constructor of the OgreContainer class.
//!
//! \param light The light to be contained in the container.
//!
OgreContainer::OgreContainer ( Ogre::Light *light ) :
m_sceneNode(0),
m_entity(0),
m_manualObject(0),
m_light(light),
m_camera(0),
m_cameraWidth(0),
m_cameraHeight(0)
{
INC_INSTANCE_COUNTER
}


//!
//! Constructor of the OgreContainer class.
//!
//! \param camera The camera to be contained in the container.
//!
OgreContainer::OgreContainer ( Ogre::Camera *camera, const int width, const int height ) :
m_sceneNode(0),
m_entity(0),
m_manualObject(0),
m_light(0),
m_camera(camera),
m_cameraWidth(width),
m_cameraHeight(height)
{
    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the OgreContainer class.
//!
OgreContainer::~OgreContainer ()
{
    this->disconnect();
    DEC_INSTANCE_COUNTER
}


///
/// Public Functions
///


//!
//! Returns the scene node contained in this container.
//!
//! \return The scene node contained in this container.
//!
Ogre::SceneNode * OgreContainer::getSceneNode ()
{
    return m_sceneNode;
}


//!
//! Sets the scene node to be contained in this container.
//!
//! \param sceneNode The scene node to be contained in this container.
//!
void OgreContainer::setSceneNode ( Ogre::SceneNode *sceneNode )
{
    m_sceneNode = sceneNode;
}


//!
//! Returns the entity contained in this container.
//!
//! \return The entity contained in this container.
//!
Ogre::Entity * OgreContainer::getEntity ()
{
    return m_entity;
}


//!
//! Sets the entity to be contained in this container.
//!
//! \param entity The entity to be contained in this container.
//!
void OgreContainer::setEntity ( Ogre::Entity *entity )
{
    m_entity = entity;
}


//!
//! Returns the manual object contained in this container.
//!
//! \return The manual object contained in this container.
//!
Ogre::ManualObject* OgreContainer::getManualObject ()
{
    return m_manualObject;
}


//!
//! Sets the manual object to be contained in this container.
//!
//! \param manualObject The manual object to be contained in this container.
//!
void OgreContainer::setManualObject ( Ogre::ManualObject *manualObject )
{
    m_manualObject = manualObject;
}


//!
//! Returns the light contained in this container.
//!
//! \return The light contained in this container.
//!
Ogre::Light * OgreContainer::getLight ()
{
    return m_light;
}


//!
//! Sets the light to be contained in this container.
//!
//! \param light The light to be contained in this container.
//!
void OgreContainer::setLight ( Ogre::Light *light )
{
    m_light = light;
}


//!
//! Returns the camera contained in this container.
//!
//! \return The camera contained in this container.
//!
Ogre::Camera * OgreContainer::getCamera ()
{
    return m_camera;
}


//!
//! Returns the camera width contained in this container.
//!
//! \return The camera contained in this container.
//!
unsigned int OgreContainer::getCameraWidth() const
{
	return m_cameraWidth;
}


//!
//! Returns the camera height contained in this container.
//!
//! \return The camera contained in this container.
//!
unsigned int OgreContainer::getCameraHeight() const
{
	return m_cameraHeight;
}


//!
//! Sets the camera to be contained in this container.
//!
//! \param camera The camera to be contained in this container.
//!
void OgreContainer::setCamera ( Ogre::Camera *camera )
{
    m_camera = camera;
}


//!
//! Sets the camera width contained in this container.
//!
//! \param width The new width for the camera contained
//! in this container.
//!
void OgreContainer::setCameraWidth(const unsigned int width)
{
	m_cameraWidth = width;
}

//!
//! Sets the camera height contained in this container.
//!
//! \param height The new width for the camera contained
//! in this container.
//!
void OgreContainer::setCameraHeight(const unsigned int height)
{
	m_cameraHeight = height;
}

//!
//! Emits the signal to notify connected objects that the scene node
//! contained in this container has been updated.
//!
void OgreContainer::updateCopies ()
{
    emit sceneNodeUpdated();
}


//!
//! Emits the signal to notify connected objects that the animation state
//! of the entity contained in this container has been updated.
//!
//! \param name The name of the animation state that has been updated.
//! \param progress The progress value for the animation state that has been updated.
//!
void OgreContainer::updateCopies ( const QString &name, double progress, float weight )
{
    emit animationStateUpdated(name, progress, weight);
}


//!
//! Emits the signal to notify connected objects that the animation state
//! of the entity contained in this container has been updated.
//!
//! \param vertexBufferGroup The new vertex buffer for the manual object that has been updated.
//!
void OgreContainer::updateCopies ( ParameterGroup* vertexBufferGroup )
{
	emit vertexBufferUpdated(vertexBufferGroup);
}


//!
//! Emits the signal to notify connected objects that the bone transformation
//! of the entity contained in this container has been updated.
//!
//! \param name The name of the bone that has been updated.
//! \param tx Translation x axis.
//! \param ty Translation y axis.
//! \param tz Translation z axis.
//! \param rx Rotation x axis.
//! \param ry Rotation y axis.
//! \param rz Rotation z axis.
//!
void OgreContainer::updateCopies ( const QString &name, 
	const float &tx, const float &ty, const float &tz, 
	const float &rx, const float &ry, const float &rz )
{
    emit boneTransformUpdated(name, tx, ty, tz, rx, ry, rz);
}

//!
//! Emits the signal to notify connected objects that the bone transformation
//! of the entity contained in this container has been updated.
//!
//! \param name The name of the bone that has been updated.
//! \param tx Translation x axis.
//! \param ty Translation y axis.
//! \param tz Translation z axis.
//! \param rx Rotation x axis.
//! \param ry Rotation y axis.
//! \param rz Rotation z axis.
//!
void OgreContainer::updateCopies ( const QString &name, 
    const Ogre::Vector3 position, 
    const Ogre::Quaternion orientation )
{
    emit boneTransformUpdated(name, position, orientation);
}

///
/// Public Slots
///


//!
//! Synchronizes the scene node contained in this container to the scene
//! node contained in the OgreContainer calling this slot.
//!
void OgreContainer::updateSceneNode ()
{
    if (!m_sceneNode)
        return;

    OgreContainer *ogreContainer = dynamic_cast<OgreContainer *>(sender());
    if (ogreContainer) {
        Ogre::SceneNode *sceneNode = dynamic_cast<Ogre::SceneNode*>(ogreContainer->getSceneNode());
        if (sceneNode) {
            m_sceneNode->setPosition(sceneNode->getPosition());
            m_sceneNode->setOrientation(sceneNode->getOrientation());
            m_sceneNode->setScale(sceneNode->getScale());

            // notify connected objects that the scene node contained in this container has been updated
            emit sceneNodeUpdated();
        }
    }
}


//!
//! Synchronizes the camera contained in this container to the camera
//! contained in the OgreContainer calling this slot.
//!
void OgreContainer::updateCamera ()
{
	if (!m_camera)
		return;

	OgreContainer *ogreContainer = dynamic_cast<OgreContainer *>(sender());
	if (ogreContainer) {
		Ogre::Camera *camera = ogreContainer->getCamera();
		if (camera) {
			const Ogre::Real &oldFocalLength = m_camera->getFocalLength();
			const Ogre::Real &newFocalLength = camera->getFocalLength();
			if (oldFocalLength != newFocalLength)
				m_camera->setFocalLength(newFocalLength);

			const Ogre::Radian &oldFOVy = m_camera->getFOVy();
			const Ogre::Radian &newFOVy = camera->getFOVy();
			if (oldFOVy != newFOVy)
				m_camera->setFOVy(newFOVy);

			const Ogre::Real &oldNearClipDistance = m_camera->getNearClipDistance();
			const Ogre::Real &newNearClipDistance = camera->getNearClipDistance();
			if (oldNearClipDistance != newNearClipDistance)
				m_camera->setNearClipDistance(newNearClipDistance);

			const Ogre::Real &oldFarClipDistance = m_camera->getFarClipDistance();
			const Ogre::Real &newFarClipDistance = camera->getFarClipDistance();
			if (oldFarClipDistance != newFarClipDistance)
				m_camera->setFarClipDistance(newFarClipDistance);

			const Ogre::Real &oldAspectRatio = m_camera->getAspectRatio();
			const Ogre::Real &newAspectRatio = camera->getAspectRatio();
			if (oldAspectRatio != newAspectRatio)
				m_camera->setAspectRatio(newAspectRatio);
			
			/*m_camera->setAutoAspectRatio(camera->getAutoAspectRatio());
			m_camera->setCastShadows(camera->getCastsShadows());
			m_camera->setDirection(camera->getDirection());
			m_camera->setLodBias(camera->getLodBias());
			m_camera->setPolygonMode(camera->getPolygonMode());
			m_camera->setPolygonModeOverrideable(camera->getPolygonModeOverrideable());
			m_camera->setProjectionType(camera->getProjectionType());
			m_camera->setQueryFlags(camera->getQueryFlags());
			m_camera->setRenderingDistance(camera->getRenderingDistance());
			m_camera->setRenderQueueGroup(camera->getRenderQueueGroup());
			m_camera->setUseIdentityProjection(camera->getUseIdentityProjection());
			m_camera->setUseIdentityView(camera->getUseIdentityView());
			m_camera->setUseRenderingDistance(camera->getUseRenderingDistance());
			m_camera->setVisibilityFlags(camera->getVisibilityFlags());
			m_camera->setVisible(camera->getVisible());*/

			// notify connected objects that the scene node contained in this container has been updated
			emit sceneNodeUpdated();

			//// for DEBUG
			//Ogre::Matrix4 ptMat = m_camera->getProjectionMatrixRS();
			//Ogre::Vector3 farCorner = m_camera->getViewMatrix(true) * m_camera->getWorldSpaceCorners()[4];

			//std::cout << "Camera Container:" << std::endl;

			//for(int i = 0; i < 4; ++i) {
			//	// Test: Display matrix on console for testing
			//	std::cout << "    " << ptMat[i][0];
			//	std::cout << " \t " << ptMat[i][1];
			//	std::cout << " \t " << ptMat[i][2];
			//	std::cout << " \t " << ptMat[i][3] << std::endl;
			//}

			//std::cout << "Far Corner:" << farCorner << std::endl;
		}
	}
}

//!
//! Synchronizes the light contained in this container to the light
//! contained in the OgreContainer calling this slot.
//!
void OgreContainer::updateLight ()
{
	if (!m_light)
		return;

	OgreContainer *ogreContainer = dynamic_cast<OgreContainer *>(sender());
	if (ogreContainer) {
		Ogre::Light *light = ogreContainer->getLight();
		if (light) {
			m_light->setType(light->getType());
			m_light->setDiffuseColour(light->getDiffuseColour());
			m_light->setSpecularColour(light->getSpecularColour());
			m_light->setAttenuation(light->getAttenuationRange(), light->getAttenuationConstant(), light->getAttenuationLinear(), light->getAttenuationQuadric());
			m_light->setPosition(light->getPosition());
			m_light->setDirection(light->getDirection());
			if (m_light->getType() == Ogre::Light::LT_SPOTLIGHT)
				m_light->setSpotlightRange(light->getSpotlightInnerAngle(), light->getSpotlightOuterAngle(), light->getSpotlightFalloff());
			m_light->setPowerScale(light->getPowerScale());
			m_light->setCastShadows(light->getCastShadows());
			m_light->setVisible(light->isVisible());

			// notify connected objects that the scene node contained in this container has been updated
			emit sceneNodeUpdated();
		}
	}
}

//!
//! Updates the animation state with the given name for the entity
//! contained in this container to the given progress.
//!
//! \param name The name of the animation state to update.
//! \param progress The progress value for the animation state to update.
//!
void OgreContainer::updateAnimationState ( const QString &name, double progress, float weight /* = 1.0 */ )
{
    // check if an entity is contained in this container
    if (m_entity) {
        Ogre::AnimationState *animationState = m_entity->getAnimationState(name.toStdString());
        if (animationState) {
			static const Ogre::Real epsilon = 0.05f;
			const Ogre::Real oldTester = animationState->getTimePosition() * animationState->getWeight();  
			const Ogre::Real    tester = static_cast<float>(progress) * weight;

            if ((oldTester < epsilon && tester < epsilon))
                return;
            else if ((oldTester > epsilon && tester < epsilon)) {
                animationState->setWeight(weight);
                animationState->setTimePosition(progress);
                animationState->setEnabled(false);
            }
            else if ((oldTester < epsilon && tester > epsilon)) {
                animationState->setEnabled(true);
                animationState->setWeight(weight);
                animationState->setTimePosition(progress);
            }
            else {
                animationState->setWeight(weight);
                animationState->setTimePosition(progress);
            }
		
			// old reference, do not delete
			/*if ((oldProgress < epsilon && progress < epsilon) ||
				(oldWeight < epsilon && weight < epsilon))
                return;
            else if ((oldProgress > epsilon && progress < epsilon) ||
					 (oldWeight > epsilon && weight < epsilon)) {
                animationState->setWeight(weight);
                animationState->setTimePosition(progress);
                animationState->setEnabled(false);
				if (name.contains("Happy_Add_Wink", Qt::CaseInsensitive))
					std::cout << "disabled:" << name.toStdString() << std::endl;
            }
            else if ((oldProgress < epsilon && progress > epsilon) ||
					 (oldWeight < epsilon && weight > epsilon)) {
                animationState->setEnabled(true);
                animationState->setWeight(weight);
                animationState->setTimePosition(progress);
				if (name.contains("Happy_Add_Wink", Qt::CaseInsensitive))
					std::cout << "enabled:" << name.toStdString() << std::endl;
            }
            else {
                animationState->setWeight(weight);
                animationState->setTimePosition(progress);
            }*/

            // notify connected objects that the animation state has been updated
            emit animationStateUpdated(name, progress, weight);
        }
    }
}

//!
//! Transforms a given bone.
//! Necessary to update all existing copys! 
//! 
//! \param name The name of the bone.
//! \param tx Translation x axis.
//! \param ty Translation y axis.
//! \param tz Translation z axis.
//! \param rx Rotation x axis.
//! \param ry Rotation y axis.
//! \param rz Rotation z axis.
//!
void OgreContainer::updateBoneTransform ( const QString &name, 
	const float &tx, const float &ty, const float &tz, 
	const float &rx, const float &ry, const float &rz )
{
    if (m_entity->hasSkeleton()) {
        Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
        if (skeletonInstance->hasBone(name.toStdString())) {
            Ogre::Bone *bone = skeletonInstance->getBone(name.toStdString());
            bone->setManuallyControlled(true);
            bone->reset();
            bone->translate(tx, ty, tz);
            bone->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(rx));
            bone->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(ry));
            bone->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(rz));
            emit boneTransformUpdated(name, tx, ty, tz, rx, ry, rz);
        }
    }
}

//!
//! Transforms a given bone.
//! Necessary to update all existing copys! 
//! 
//! \param name The name of the bone.
//! \param position New position of bone.
//! \param orientation New orientation of bone.
//!
void OgreContainer::updateBoneTransform ( const QString &name, 
    const Ogre::Vector3 position,
    const Ogre::Quaternion orientation )
{
    if (m_entity->hasSkeleton()) {
        Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
        if (skeletonInstance->hasBone(name.toStdString())) {
            Ogre::Bone *bone = skeletonInstance->getBone(name.toStdString());
            bone->setManuallyControlled(true);
            bone->reset();
            bone->setPosition(position);
            bone->setOrientation(orientation);

            emit boneTransformUpdated(name, position, orientation);
        }
    }
}

//!
//! Signal to notify connected objects that the custom parameters
//! of the entity contained in this container has been updated.
//!
//! \param subentityName The name of the subentity that has been updated.
//! \param value the new value of the custom parameter
//!
void OgreContainer::updateCustomParameter ( const QString &name,
	const Ogre::Vector4 &value )
{
	m_entity->getSubEntity(name.toStdString())->setCustomParameter(0, value);

	emit customParameterUpdated(name, value);
}

//!
//! Updates the vertex buffers used by a manual object
//! Necessary to update all existing copys!
//!
void OgreContainer::updateVertexBuffer ( ParameterGroup* vertexBufferGroup )
{
	if (!vertexBufferGroup)
		return;

	m_manualObject->clear();
	
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

	m_manualObject->begin(materialName.toStdString(), renderOperation, materialGroupName.toStdString());
	const int code = (bool) posParameter * 1 + 
					 (bool) colParameter * 2 +
					 (bool) normParameter * 4 + 
					 (bool) uvParameter * 8;

	switch (code){
		case 1: {
			const QVector<float> &posList = posParameter->getValue().value<QVector<float>>();
			for(int i=0; i<posList.size(); i+=3) {
				m_manualObject->position(
					posList.at(i),
					posList.at(i+1),
					posList.at(i+2) );
			}
			break;
		}
		case 3: {
			const QVector<float> &posList = posParameter->getValue().value<QVector<float>>();
			const QVector<float> &colList = colParameter->getValue().value<QVector<float>>();
			for(int i=0; i<posList.size(); i+=3) {
				m_manualObject->position(
					posList.at(i),
					posList.at(i+1),
					posList.at(i+2) );
				m_manualObject->colour(
					colList.at(i),
					colList.at(i+1),
					colList.at(i+2) );
			}
			break;
		}
		case 7: {
			const QVector<float> &posList = posParameter->getValue().value<QVector<float>>();
			const QVector<float> &colList = colParameter->getValue().value<QVector<float>>();
			const QVector<float> &normList = normParameter->getValue().value<QVector<float>>();
			for(int i=0; i<posList.size(); i+=3) {
				m_manualObject->position(
					posList.at(i),
					posList.at(i+1),
					posList.at(i+2) );
				m_manualObject->colour(
					colList.at(i),
					colList.at(i+1),
					colList.at(i+2) );
				m_manualObject->normal(
					normList.at(i),
					normList.at(i+1),
					normList.at(i+2));
			}
			break;
		}
		case 15: {
			const QVector<float> &posList = posParameter->getValue().value<QVector<float>>();
			const QVector<float> &colList = colParameter->getValue().value<QVector<float>>();
			const QVector<float> &normList = normParameter->getValue().value<QVector<float>>();
			const QVector<float> &uvList = uvParameter->getValue().value<QVector<float>>();
			for(int i=0; i<posList.size(); i+=3) {
				m_manualObject->position(
					posList.at(i),
					posList.at(i+1),
					posList.at(i+2) );
				m_manualObject->colour(
					colList.at(i),
					colList.at(i+1),
					colList.at(i+2) );
				m_manualObject->normal(
					normList.at(i),
					normList.at(i+1),
					normList.at(i+2) );
				m_manualObject->textureCoord(
					uvList.at(i),
					uvList.at(i+1),
					uvList.at(i+2));
			}
			break;
		}
		default: {
			Log::error(QString("Max supported attributes size is 4"), "OgreContainer::updateVertexBuffer");
			break;
		}
	}
	m_manualObject->end();
	emit vertexBufferUpdated(vertexBufferGroup);
}

} // end namespace Frapper