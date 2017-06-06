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
//! \file "LightNode.cpp"
//! \brief Implementation file for LightNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       17.02.2015 (last updated)
//!

#include "LightNode.h"
#include "SceneNodeParameter.h"
#include "EnumerationParameter.h"
#include "NumberParameter.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include "Log.h"

using namespace Frapper;

INIT_INSTANCE_COUNTER(LightNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the LightNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
LightNode::LightNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ViewNode(name, parameterRoot),
    m_sceneManager(OgreManager::getSceneManager()),
    m_sceneNode(0),
    m_ogreContainer(0),
    m_entity(0),
    m_outputLightName("Light")
{
	// preinitialize light list
	m_lightList.reserve(16);

	// create tansform parameter group
	ParameterGroup *transformGroup = new ParameterGroup("Transformation");
	parameterRoot->addParameter(transformGroup);

	// create switch for disabling lights
	Parameter *disableLight = new Parameter("Disable Light", Parameter::T_Bool, false);
	disableLight->setPinType(Parameter::PT_Input);
	disableLight->setSelfEvaluating(true);
	parameterRoot->addParameter(disableLight);

	// create transform parameter
	NumberParameter *position = new NumberParameter("Position", Parameter::T_Float, QVariant::fromValue<Ogre::Vector3>(Ogre::Vector3(.0,.0,.0)));
	position->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
	position->setMaxValue(250.0f);
	position->setMinValue(-250.0f);
	transformGroup->addParameter(position);
	position->setChangeFunction(SLOT(applyPosition()));

	// create transform parameter
	NumberParameter *orientation = new NumberParameter("Orientation", Parameter::T_Float, QVariant::fromValue<Ogre::Vector3>(Ogre::Vector3(.0,.0,.0)));
	orientation->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
	orientation->setMaxValue(180.0f);
	orientation->setMinValue(-180.0f);
	transformGroup->addParameter(orientation);
    orientation->setChangeFunction(SLOT(applyOrientation()));

    // create an output light parameter
    addOutputParameter(new LightParameter(m_outputLightName));

    // create OGRE scene node and scene node encapsulation
    m_sceneNode = OgreManager::createSceneNode(m_name);
    if (m_sceneNode) {
        m_ogreContainer = new OgreContainer(m_sceneNode);
        m_sceneNode->setUserAny(Ogre::Any(m_ogreContainer));
        //m_sceneNode->setScale(0.3, 0.3, -0.4);
    }

    setValue(m_outputLightName, m_sceneNode, true);

    // create a new OGRE entity to represent the light in the viewport
    QString entityName = QString("%1Entity").arg(m_name);
    m_entity = m_sceneManager->createEntity(entityName.toStdString(), "light.mesh");

    if (m_entity) {
        int numOfSubentities = m_entity->getNumSubEntities();
        for (int i = 0; i < numOfSubentities; ++i) {
            // create a new number parameter for the bone
            Ogre::SubEntity *subEntity = m_entity->getSubEntity(i);
            subEntity->setCustomParameter(0, Ogre::Vector4(0.0, 0.0, 0.0, 0.0));
        }
            m_sceneNode->attachObject(m_entity);
    }

    // initialize the light by calling the callback functions
    applyPosition();
    applyOrientation();

	switchHeadlights(false);

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the LightNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
LightNode::~LightNode ()
{
    OgreTools::deepDeleteSceneNode(m_sceneNode, m_sceneManager, true);
    setValue(m_outputLightName, (Ogre::SceneNode *) 0, true);

    DEC_INSTANCE_COUNTER

	if (s_numberOfInstances < 1)
		switchHeadlights(true);
}


///
/// Public Functions
///



//!
//! Returns the scene node that contains scene objects created or modified
//! by this node.
//!
//! \return The scene node containing objects created or modified by this node.
//!
Ogre::SceneNode * LightNode::getSceneNode ()
{
    Ogre::SceneNode *result = getSceneNodeValue(m_outputLightName, true);
    if (!result)
        Log::error("Could not obtain light scene node.", "LightNode::getSceneNode");
    return result;
}


///
/// Private Functions
///



//!
//! Switches all default viewport lights on or off,
//!
//! \param value true for on, false for off.
//!
void LightNode::switchHeadlights(const bool value)
{
	// disable default headlight
	Ogre::SceneManager::MovableObjectIterator objIter = m_sceneManager->getMovableObjectIterator("Light");
	while (objIter.hasMoreElements()) {
		Ogre::Light *light = static_cast<Ogre::Light *>(objIter.getNext());
		const QString lightName = QString::fromStdString(light->getName());
		if (lightName.contains("headLight")) {
			light->setVisible(value);
			light->setRenderingDistance(!value);
		}
	}
	triggerRedraw();
}


//!
//! Create and register a new light.
//!
//! \param name The name of the light to be created.
//!
Ogre::Light *LightNode::createLight(const QString &name)
{
	Ogre::Light *light = m_sceneManager->createLight(name.toStdString());
	m_sceneNode->attachObject(light);
	
	OgreContainer *ogreLightContainer = new OgreContainer(light);
    light->setUserAny(Ogre::Any(ogreLightContainer));
	
	m_ogreLightContainers.append(ogreLightContainer);
	m_lightList.append(light);

	return light;
}


//!
//! Destroys all lights of the node.
//!
void LightNode::destroyAllLights()
{
	foreach (Ogre::Light *light, m_lightList) {
		m_sceneNode->detachObject(light);
		OgreTools::deleteUserAnyFromMovableObject(light);
		m_sceneManager->destroyLight(light);
	}

	m_lightList.clear();
	m_ogreLightContainers.clear();
}


///
/// Private Slots
///


//!
//! Applies the currently set position for the node to the OGRE scene
//! objects contained in this node.
//!
void LightNode::applyPosition ()
{
    Ogre::Vector3 position = getVectorValue("Position");

    // apply the position to the light's scene node and all its copies
    m_sceneNode->setPosition(position);
	m_sceneNode->needUpdate();
	m_sceneNode->_update(true, false);
    m_ogreContainer->updateCopies();

    triggerRedraw();
}


//!
//! Applies the currently set orientation for the node to the OGRE scene
//! objects contained in this node.
//!
void LightNode::applyOrientation ()
{
    Ogre::Vector3 orientation = getVectorValue("Orientation");

    // decode the parameter's values
    Ogre::Radian xRadian = Ogre::Radian(Ogre::Degree(orientation.x));
    Ogre::Radian yRadian = Ogre::Radian(Ogre::Degree(orientation.y));
    Ogre::Radian zRadian = Ogre::Radian(Ogre::Degree(orientation.z));

    // apply the orientation to the light's scene node and all its copies
    m_sceneNode->resetOrientation();
    m_sceneNode->pitch(xRadian, Ogre::Node::TS_WORLD);
    m_sceneNode->yaw(yRadian, Ogre::Node::TS_WORLD);
    m_sceneNode->roll(zRadian, Ogre::Node::TS_WORLD);
	m_sceneNode->_update(true, false);
    m_ogreContainer->updateCopies();

    triggerRedraw();
}
