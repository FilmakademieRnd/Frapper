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
//! \file "TransformNode.cpp"
//! \brief Implementation file for TransformNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "TransformNode.h"
#include "Connection.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include "Log.h"

namespace TransformNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(TransformNode)

///
/// Protected Static Constants
///


//!
//! The name of the input geometry parameter.
//!
const QString TransformNode::InputGeometryParameterName = "Geometry to Transform";

//!
//! The name of the input lights parameter.
//!
const QString TransformNode::InputLightsParameterName = "Lights to Transform";

//!
//! The name of the input cameras parameter.
//!
const QString TransformNode::InputCamerasParameterName = "Cameras to Transform";

//!
//! The name of the output geometry parameter.
//!
const QString TransformNode::OutputGeometryParameterName = "Transformed Geometry";

//!
//! The name of the output lights parameter.
//!
const QString TransformNode::OutputLightsParameterName = "Transformed Lights";

//!
//! The name of the output cameras parameter.
//!
const QString TransformNode::OutputCamerasParameterName = "Transformed Cameras";


///
/// Constructors and Destructors
///


//!
//! Constructor of the TransformNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TransformNode::TransformNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ViewNode(name, parameterRoot),
    m_sceneManager(OgreManager::getSceneManager()),
    m_geometrySceneNode(0),
    m_geometryContainer(0),
    m_lightsSceneNode(0),
    m_lightsContainer(0),
    m_cameraSceneNode(0),
    m_cameraContainer(0)
{
    if (!m_sceneManager) {
        Log::error("The OGRE scene manager could not be obtained.", "TransformNode::TransformNode");
        return;
    }

    // create scene node and container for input geometry
    m_geometrySceneNode = m_sceneManager->createSceneNode(QString("%1_geometrySceneNode").arg(m_name).toStdString());
    m_geometryContainer = new OgreContainer(m_geometrySceneNode);
    m_geometrySceneNode->setUserAny(Ogre::Any(m_geometryContainer));

    // create scene node and container for input lights
    m_lightsSceneNode = m_sceneManager->createSceneNode(QString("%1_lightsSceneNode").arg(m_name).toStdString());
    m_lightsContainer = new OgreContainer(m_lightsSceneNode);
    m_lightsSceneNode->setUserAny(Ogre::Any(m_lightsContainer));

    // create scene node and container for input camera
    m_cameraSceneNode = m_sceneManager->createSceneNode(QString("%1_cameraSceneNode").arg(m_name).toStdString());
    m_cameraContainer = new OgreContainer(m_cameraSceneNode);
    m_cameraSceneNode->setUserAny(Ogre::Any(m_cameraContainer));

    // get transformation parameters
    Parameter *translateParameter = getParameter("Translate");
    Parameter *rotateParameter = getParameter("Rotate");
    Parameter *scaleParameter = getParameter("Scale");

	rotateParameter->setSelfEvaluating(true);

    // Set affections and processing functions of input and output parameters
    Parameter *inputGeometryParameter = getParameter(InputGeometryParameterName);
    Parameter *outputGeometryParameter = getParameter(OutputGeometryParameterName);
    if (inputGeometryParameter && outputGeometryParameter) {
        outputGeometryParameter->setProcessingFunction(SLOT(processOutputGeometry()));
        outputGeometryParameter->addAffectingParameter(inputGeometryParameter);
        outputGeometryParameter->setDirty(true);
        connect(outputGeometryParameter, SIGNAL(dirtied()), SLOT(checkViewFlag()));
    }
    Parameter *inputLightsParameter = getParameter(InputLightsParameterName);
    Parameter *outputLightsParameter = getParameter(OutputLightsParameterName);
    if (inputLightsParameter && outputLightsParameter) {
        outputLightsParameter->setProcessingFunction(SLOT(processOutputLights()));
        outputLightsParameter->addAffectingParameter(inputLightsParameter);
        outputLightsParameter->setDirty(true);
    }
    Parameter *inputCamerasParameter = getParameter(InputCamerasParameterName);
    Parameter *outputCamerasParameter = getParameter(OutputCamerasParameterName);
    if (inputCamerasParameter && outputCamerasParameter) {
        outputCamerasParameter->setProcessingFunction(SLOT(processOutputCameras()));
        outputCamerasParameter->addAffectingParameter(inputCamerasParameter);
        outputCamerasParameter->setDirty(true);
    }

    setChangeFunction("Translate", SLOT(applyTranslation()));
    setChangeFunction("Rotate", SLOT(applyRotation()));
    setChangeFunction("Scale", SLOT(applyScale()));
    setCommandFunction("Reset", SLOT(resetTransformation()));

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the TransformNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TransformNode::~TransformNode ()
{
    // destroy geometry scene node through its scene manager
	if (m_geometrySceneNode) {
        Ogre::SceneManager *sceneManager = m_geometrySceneNode->getCreator();
		if (sceneManager) {
            OgreTools::deepDeleteSceneNode(m_geometrySceneNode, sceneManager, true);
		}
    }

    // destroy lights scene node through its scene manager
    if (m_lightsSceneNode) {
        Ogre::SceneManager *sceneManager = m_lightsSceneNode->getCreator();
		if (sceneManager) {
			OgreTools::deepDeleteSceneNode(m_lightsSceneNode, sceneManager, true);
		}
    }

    // destroy camera scene node through its scene manager
    if (m_cameraSceneNode) {
        Ogre::SceneManager *sceneManager = m_cameraSceneNode->getCreator();
		if (sceneManager) { 
			OgreTools::deepDeleteSceneNode(m_cameraSceneNode, sceneManager, true);
		}
    }

    DEC_INSTANCE_COUNTER
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
Ogre::SceneNode * TransformNode::getSceneNode ()
{
    QVariant outputGeometryValue = getValue(OutputGeometryParameterName, true);

    if (outputGeometryValue.canConvert<Ogre::SceneNode *>()) 
        return outputGeometryValue.value<Ogre::SceneNode *>();

    return 0;
}


//!
//! Returns the radius of the bounding sphere surrounding this object.
//!
//! \return The radius of the bounding sphere surrounding this object.
//!
double TransformNode::getBoundingSphereRadius ()
{
    double scale = 1;
    return scale;
}


//!
//! Evaluates the given new connection for the node.
//!
//! \param connection The new connection to evaluate.
//!
void TransformNode::evaluateConnection ( Connection *connection )
{
}


///
/// Private Slots
///


//!
//! Processes the input geometry to generate the node's output geometry.
//!
void TransformNode::processOutputGeometry ()
{
    //if (!m_geometrySceneNode)
    //    return;

    //m_geometrySceneNode->removeAllChildren();

    //Parameter *geometryParameter = getParameter(InputGeometryParameterName);
    //QVariantList sceneNodes = geometryParameter->getValueList();

    //foreach(QVariant sceneNode, sceneNodes)
    //{
    //    // Check if Input parameter can be casted into an Ogre::SceneNode
    //    if (sceneNode.canConvert<Ogre::SceneNode*>())
    //    {
    //        Ogre::SceneNode *inputGeometrySceneNode = sceneNode.value<Ogre::SceneNode *>();
    //        m_geometrySceneNode->getParentSceneNode()->removeChild(inputGeometrySceneNode);
    //        m_geometrySceneNode->addChild(inputGeometrySceneNode);
    //    }
    //}

    //setValue(OutputGeometryParameterName, m_geometrySceneNode);
 

    if (!m_geometrySceneNode)
        return;

    // destroy all scene nodes and objects in the render scene
    OgreTools::deepDeleteSceneNode(m_geometrySceneNode, m_sceneManager);

    Parameter *geometryParameter = getParameter(InputGeometryParameterName);

    bool valid = false;
    foreach( QVariant parameterValue, geometryParameter->getValueList())
    {
        if( parameterValue.canConvert<Ogre::SceneNode *>() )
        {
            Ogre::SceneNode *inputGeometrySceneNode = parameterValue.value<Ogre::SceneNode *>();
            Ogre::SceneNode *inputGeometrySceneNodeCopy = 0;           

            OgreTools::deepCopySceneNode(inputGeometrySceneNode, inputGeometrySceneNodeCopy, QString("%1GeometrySceneNode").arg(m_name), m_sceneManager);
            if (inputGeometrySceneNodeCopy) {
                m_geometrySceneNode->addChild(inputGeometrySceneNodeCopy);
                valid = true;
            }
        }
    }
    if (valid)
        setValue(OutputGeometryParameterName, m_geometrySceneNode);

}


//!
//! Processes the input lights to generate the node's output lights.
//!
void TransformNode::processOutputLights ()
{
    if (!m_lightsSceneNode)
        return;

    Ogre::SceneNode *inputLightsSceneNode = getSceneNodeValue(InputLightsParameterName);
    OgreTools::deepDeleteSceneNode(m_lightsSceneNode, m_sceneManager);
    Ogre::SceneNode *inputLightsSceneNodeCopy = 0;
    OgreTools::deepCopySceneNode(inputLightsSceneNode, inputLightsSceneNodeCopy, QString("%1LightsSceneNode").arg(m_name), m_sceneManager);
    if (inputLightsSceneNodeCopy) {
        m_lightsSceneNode->addChild(inputLightsSceneNodeCopy);
        setValue(OutputLightsParameterName, m_lightsSceneNode);
    } else
        Log::error("No valid scene node in input lights parameter.", "TransformNode::processOutputLights");
}


//!
//! Processes the input cameras to generate the node's output cameras.
//!
void TransformNode::processOutputCameras ()
{
    if (!m_geometrySceneNode)
        return;

    Ogre::SceneNode *inputCameraSceneNode = getSceneNodeValue(InputCamerasParameterName);
    OgreTools::deepDeleteSceneNode(m_cameraSceneNode, m_sceneManager);
    Ogre::SceneNode *inputCameraSceneNodeCopy = 0;
    OgreTools::deepCopySceneNode(inputCameraSceneNode, inputCameraSceneNodeCopy, QString("%1CameraSceneNode").arg(m_name), m_sceneManager);
    if (inputCameraSceneNodeCopy) {
        m_cameraSceneNode->addChild(inputCameraSceneNodeCopy);
        setValue(OutputCamerasParameterName, m_cameraSceneNode);
    } else
        Log::error("No valid scene node in input cameras parameter.", "TransformNode::processOutputCameras");
}


//!
//! Applies the changed translation values to the scene nodes.
//!
void TransformNode::applyTranslation ()
{
    Ogre::Vector3 position = getVectorValue("Translate");

    // apply parameter value to scene nodes
    m_geometrySceneNode->setPosition(position);
    m_lightsSceneNode->setPosition(position);
    m_cameraSceneNode->setPosition(position);

    // update scene node copies through their containers
    m_geometryContainer->updateCopies();
    m_lightsContainer->updateCopies();
    m_cameraContainer->updateCopies();

    triggerRedraw();
}


//!
//! Applies the changed rotation values to the scene nodes.
//!
void TransformNode::applyRotation ()
{
    Ogre::Vector3 rotation = getVectorValue("Rotate");

    // decode parameter value
    Ogre::Quaternion xRotation = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(rotation.x)), Ogre::Vector3::UNIT_X);
    Ogre::Quaternion yRotation = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(rotation.y)), Ogre::Vector3::UNIT_Y);
    Ogre::Quaternion zRotation = Ogre::Quaternion(Ogre::Radian(Ogre::Degree(rotation.z)), Ogre::Vector3::UNIT_Z);
    Ogre::Quaternion orientation = xRotation * yRotation * zRotation;

    // apply parameter value to scene nodes
    m_geometrySceneNode->setOrientation(orientation);
    m_lightsSceneNode->setOrientation(orientation);
    m_cameraSceneNode->setOrientation(orientation);

    // update scene node copies through their containers
    m_geometryContainer->updateCopies();
    m_lightsContainer->updateCopies();
    m_cameraContainer->updateCopies();

    triggerRedraw();
}


//!
//! Applies the changed scale values to the scene nodes.
//!
void TransformNode::applyScale ()
{
    Ogre::Vector3 scale = getVectorValue("Scale");

    // apply parameter value to scene nodes
    m_geometrySceneNode->setScale(scale);
    m_lightsSceneNode->setScale(scale);
    m_cameraSceneNode->setScale(scale);

    // update scene node copies through their containers
    m_geometryContainer->updateCopies();
    m_lightsContainer->updateCopies();
    m_cameraContainer->updateCopies();

    triggerRedraw();
}

//!
//! Resets the transformation parameters to their default values.
//!
void TransformNode::resetTransformation ()
{
    Parameter *resetTransformationParameter = getParameter("Reset");
    if (!resetTransformationParameter) {
        Log::error("Could not obtain reset parameter.", "TransformNode::resetTransformation");
        return;
    }
    
    Parameter *translateParameter = getParameter("Translate");
    Parameter *rotateParameter = getParameter("Rotate");
    Parameter *scaleParameter = getParameter("Scale");
    if (!translateParameter || !rotateParameter || ! scaleParameter) {
        Log::error("Could not find transform parameter.", "TransformNode::resetTransformation");
        return;
    }
    
    translateParameter->reset();
    rotateParameter->reset();
    scaleParameter->reset();

}

} // namespace TransformNode 
