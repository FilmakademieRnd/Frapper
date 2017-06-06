/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "HairSingleScatterRenderNode.cpp"
//! \brief Implementation file for HairSingleScatterRenderNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       11.10.2010 (last updated)
//!

#include "HairSingleScatterRenderNode.h"
#include "GeometryRenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreManager.h"
#include <QtCore/QFile>

namespace HairSingleScatterRenderNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the HairSingleScatterRenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
HairSingleScatterRenderNode::HairSingleScatterRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    GeometryRenderNode(name, parameterRoot),
	m_lightScale(20)
{
	if (m_sceneManager) {
        m_sceneManager->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));
	}

	// Set the change function for light description file parameter.
    setChangeFunction("Light Description File", SLOT(loadLightDescriptionFile()));
    setChangeFunction("Light Scale Pos", SLOT(setLightScale()));
	setChangeFunction("Number of Lights", SLOT(setNbrLights()));
	setChangeFunction("lightPower", SLOT(setShaderParameter()));
    setChangeFunction("sampleRadius", SLOT(setShaderParameter()));
    setChangeFunction("blurSize", SLOT(setShaderParameter()));
	setChangeFunction("hairRootWidth", SLOT(setShaderParameter()));
	setChangeFunction("hairTipWidth", SLOT(setShaderParameter()));
	setChangeFunction("rootColor", SLOT(setShaderColor()));
	setChangeFunction("tipColor", SLOT(setShaderColor()));
	setChangeFunction("diffuseColor", SLOT(setShaderColor()));
	setChangeFunction("diffuseStrength", SLOT(setShaderParameter()));
	setChangeFunction("reflectColor", SLOT(setShaderColor()));
	setChangeFunction("reflectStrength", SLOT(setShaderParameter()));
	setChangeFunction("reflectWidth", SLOT(setShaderParameter()));
	setChangeFunction("scatterColor", SLOT(setShaderColor()));
	setChangeFunction("scatterStrength", SLOT(setShaderParameter()));
	setChangeFunction("scatterWidth", SLOT(setShaderParameter()));
	setChangeFunction("transmitColor", SLOT(setShaderColor()));
	setChangeFunction("transmitStrength", SLOT(setShaderParameter()));
	setChangeFunction("transmitWidth", SLOT(setShaderParameter()));

	Parameter *outputImageParameter = getParameter(m_outputImageName);
    if (outputImageParameter) {
        outputImageParameter->setProcessingFunction(SLOT(processOutputImage()));
        outputImageParameter->setDirty(true);
		// set up parameter affections
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::InputGeometryParameterName));
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::InputLightsParameterName));
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::InputCameraParameterName));
        outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::CameraLightToggleParameterName));
		outputImageParameter->addAffectingParameter(getParameter(GeometryRenderNode::BackgroundColorParameterName));
		outputImageParameter->addAffectingParameter(getParameter("Light Description File"));
    }
}


//!
//! Destructor of the HairSingleScatterRenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
HairSingleScatterRenderNode::~HairSingleScatterRenderNode ()
{
}


///
/// Public Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void HairSingleScatterRenderNode::processOutputImage ()
{
	GeometryRenderNode::processOutputImage();

	if (m_cameraSceneNode) {
		// obtain the backgrond color
		QColor bgColor = getColorValue(BackgroundColorParameterName);
		// (re-)create the render texture
		if (bgColor.alphaF() < 1.0f)
			initializeRenderTarget(m_camera, Ogre::ColourValue(bgColor.redF(), bgColor.greenF(), bgColor.blueF(), bgColor.alphaF()), Ogre::PF_R8G8B8, true, 8);
		else
			initializeRenderTarget(m_camera, Ogre::ColourValue(bgColor.redF(), bgColor.greenF(), bgColor.blueF()), Ogre::PF_R8G8B8, true, 8);

		// update render texture and render target
		setValue(m_outputImageName, m_renderTexture);

		// undirty the output parameter to prevent multiple updates
		getParameter(m_outputImageName)->setDirty(false);
	}

	if (m_geometrySceneNode) {
		setCustomVertexShader();
	}
}


//private functions

//!
//! Returns the greatest componen of the vector
//!
//! \return The greatest componen of the vector

void HairSingleScatterRenderNode::redrawTriggered()
{
	RenderNode::redrawTriggered();
}

float HairSingleScatterRenderNode::greatestComponent(const Ogre::Vector3 &vec) const
{
	float returnvalue = vec.x;
	if (returnvalue < vec.y)
		returnvalue = vec.y;
	if (returnvalue < vec.z)
		returnvalue = vec.z;
	return returnvalue;
}

//!
//! Insterts a custom vertex shader to rendering pipeline
//!
void HairSingleScatterRenderNode::setCustomVertexShader()
{
	Ogre::SceneManager::MovableObjectIterator objIter = m_sceneManager->getMovableObjectIterator("Entity");
	while (objIter.hasMoreElements()) {
		Ogre::Entity *entity = static_cast<Ogre::Entity *>(objIter.getNext());
		const Ogre::MeshPtr mesh = entity->getMesh();
		Ogre::Mesh::SubMeshNameMap nameMap = mesh->getSubMeshNameMap();
		Ogre::Mesh::SubMeshNameMap::iterator nameMapIter;
		for (nameMapIter = nameMap.begin(); nameMapIter != nameMap.end(); ++nameMapIter) {
			const QString &subMeshName = nameMapIter->first.c_str();
			if (subMeshName.contains("hair", Qt::CaseInsensitive)) {
				Ogre::SubMesh *subMesh = mesh->getSubMesh(nameMapIter->first);
				Ogre::SubEntity *subEntity = entity->getSubEntity(nameMapIter->second);
				
				subEntity->setMaterialName("hairSingleScatter/singleScatterHair");
			}
		}
	}
}

//!
//! The less then function for light power sorting
//!
bool HairSingleScatterRenderNode::greaterThan(const QPair<Ogre::Vector3, Ogre::Vector3> &d1, const QPair<Ogre::Vector3, Ogre::Vector3> &d2)
{
	return d1.second.length() > d2.second.length();
}

//protected functions

//!
//! Loads an XML reference data file.
//!
//! Is called when the value of the Reference Data File parameter has
//! changed.
//!
//! \return True if the reference data was successfully loaded from file, otherwise False.
//!
void HairSingleScatterRenderNode::loadLightDescriptionFile ()
{
    QString path = getStringValue("Light Description File");
    QFile inFile(path);

	m_lightPositions.clear();

	// parse the file and fill the pos/pow pair into a list
    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QPair<Ogre::Vector3, Ogre::Vector3> newData(Ogre::Vector3(0,0,0), Ogre::Vector3(0,0,0));
		while (!inFile.atEnd()) {
			QString line = inFile.readLine();
			QString prefix = line.section(" ", 0, 0);
			if (prefix == "Dir:") {
				Ogre::Vector3 pos(
					line.section(" ", 1, 1).toFloat(),
					line.section(" ", 2, 2).toFloat(),
					line.section(" ", 3, 3).toFloat() );
				newData.first = pos;
			}
			else if (prefix == "Pow:") {
				Ogre::Vector3 pow(
					line.section(" ", 1, 1).toFloat(),
					line.section(" ", 2, 2).toFloat(),
					line.section(" ", 3, 3).toFloat() );
				newData.second = pow;
			}
			if (newData.first.length() != 0 && newData.second.length() != 0) {
				m_lightPositions.append(newData);
				newData = QPair<Ogre::Vector3, Ogre::Vector3>(Ogre::Vector3(0,0,0), Ogre::Vector3(0,0,0));
			}
		}
    } else
        Log::error(QString("Document import failed. \"%1\" not found.").arg(path), "LightDescriptionNode::loadReferenceDataFile");
    
	inFile.close();

	// sort the list: greatest pow first
	qSort(m_lightPositions.begin(), m_lightPositions.end(), greaterThan);

	NumberParameter *nbrLights = getNumberParameter("Number of Lights");
	nbrLights->setMaxValue(m_lightPositions.length());
	processOutputImage();
}

//!
//! Sets the scene light scale value
//!
void HairSingleScatterRenderNode::setLightScale ()
{
	m_lightScale = getDoubleValue("Light Scale Pos");
	setNbrLights();
}

//!
//! Sets the number of lights for the sadow calculation
//!
void HairSingleScatterRenderNode::setNbrLights()
{
	if (m_lightSceneNode || m_lightPositions.empty())
		return;

	Ogre::SceneNode *rootSceneNode = m_sceneManager->getRootSceneNode();

	const int nbrLights = getIntValue("Number of Lights");
	m_sceneManager->destroyAllLights();

	unsigned int count = 0;
	const float max = m_lightPositions.first().second.length();
	for (QList<QPair<Ogre::Vector3, Ogre::Vector3>>::iterator iter = m_lightPositions.begin(); iter != m_lightPositions.end(); ++iter, ++count) {
		const Ogre::Vector3 pos = iter->first * m_lightScale;
		const Ogre::Vector3 pow = iter->second / max;
		const float length = (pow).length();
		Ogre::Light *light = m_sceneManager->createLight("Light" + QString::number(count).toStdString() );
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(pos);
		//light->setSpotlightRange(Ogre::Degree(100), Ogre::Degree(120));
		//light->setAttenuation(100, 1, 1, 1);
		//light->setDirection(-pos);
		light->setDiffuseColour(pow.x, pow.y, pow.z);
		//light->setDiffuseColour(length, length, length);		
		light->setSpecularColour(0,0,0);
		if (m_lightSceneNode)
			m_lightSceneNode->attachObject(light);
		if (count == nbrLights-1) break;
	}

	redrawTriggered();
}

//!
//! Sets the scenes shader parameter
//!


void HairSingleScatterRenderNode::setShaderParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
    const float value = parameter->getValue().toFloat();
	
	
	Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("hairSingleScatter/singleScatterHair"));
	Ogre::GpuProgramParametersSharedPtr params = matPtr->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	if (!params->_findNamedConstantDefinition(name))
		params = matPtr->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	if (params->_findNamedConstantDefinition(name))
		params->setNamedConstant(name, value);

	/*params = matPtr->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	if (params->_findNamedConstantDefinition(name))
		params->setNamedConstant(name, value);*/
	
	redrawTriggered();
}


void HairSingleScatterRenderNode::setShaderColor()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const QColor color = parameter->getValue().value<QColor>();
	Ogre::ColourValue oColor;
	
	oColor.r = color.redF();
	oColor.g = color.greenF();
	oColor.b = color.blueF();

	Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName("hairSingleScatter/singleScatterHair"));
	Ogre::GpuProgramParametersSharedPtr params = matPtr->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	if (!params->_findNamedConstantDefinition(name))
		params = matPtr->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	if (params->_findNamedConstantDefinition(name))
		params->setNamedConstant(name, oColor);

	/*params = matPtr->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	if (params->_findNamedConstantDefinition(name))
		params->setNamedConstant(name, oColor);*/

	redrawTriggered();
}

} // namespace HairSingleScatterRenderNode 
