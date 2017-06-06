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
//! \file "AnimatableMeshHairNode.cpp"
//! \brief Implementation file for AnimatableMeshHairNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Markus Rapp <markus.rapp@filmakademie.de>
//! \version    2.0
//! \date       06.10.2014 (last updated)
//!

//#define WRITE_PREPARE_HAIR_LOG

#include "AnimatableMeshHairNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include <QtCore/QFile>
#include "Helper.h"

#include "OgreTagPoint.h"

namespace AnimatableMeshHairNode {
using namespace Frapper;


#define SMALL_NUMBER 0.00001

///
/// Initialization of non-integral static const class members
///

const int AnimatableMeshHairNode::m_numStrandVariables = 1024;
const float AnimatableMeshHairNode::PIG = 180.0f*3.141593f;

///
/// Constructors and Destructors
///


//!
//! Constructor of the AnimatableMeshHairNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
AnimatableMeshHairNode::AnimatableMeshHairNode ( const QString &name, ParameterGroup *parameterRoot ) :
	GeometryAnimationNode(name, parameterRoot)
{
	// Set the change function for hair gemoetry
	setChangeFunction("Distance Root Vertex to Scalp Vertex", SLOT(changeRootVertexToScalpVertexDistanceParameter()));

	setChangeFunction("Hair LOD Factor", SLOT(changeHairLODParameter()));
	setChangeFunction("g_backFaceCullingScale", SLOT(changeShaderParameter()));
	
	setChangeFunction("Screen Space Adaptive LOD", SLOT(changeShaderLODParameter()));
	setChangeFunction("Segment Screen Size", SLOT(changeSegmentScreenSizeParameter()));
	setChangeFunction("Bend Angle Meaningful", SLOT(changeBendAngleMeaningfulParameter()));
	setChangeFunction("Bend Angle Max Tessellation", SLOT(changeBendAngleMaxTessellationParameter()));

	setChangeFunction("g_tessellationFactor", SLOT(changeShaderParameter()));
	setChangeFunction("g_detailFactor", SLOT(changeShaderParameter()));
	setChangeFunction("g_rootRadius", SLOT(changeShaderParameter()));
	setChangeFunction("g_tipRadius", SLOT(changeShaderParameter()));
	setChangeFunction("g_clumpWidth", SLOT(changeShaderParameter()));
	setChangeFunction("g_strandWidth", SLOT(changeShaderParameter()));
	setChangeFunction("g_strandTaperingStart", SLOT(changeShaderParameter()));
	setChangeFunction("g_curlyHairScale", SLOT(changeShaderParameter()));
	setChangeFunction("g_deviationHairScale", SLOT(changeShaderParameter()));
	setChangeFunction("g_thinning", SLOT(changeThinningParameter()));
	
	setChangeFunction("g_clumpTransitionLength", SLOT(changeShaderParameter()));
	setChangeFunction("g_maxDistance", SLOT(changeShaderParameter()));
	setChangeFunction("g_maxAngle", SLOT(changeShaderParameter()));
	
	setChangeFunction("Render Scalp Mesh", SLOT(changeScalpMeshVisibility()));
	setChangeFunction("Render Guide Strands", SLOT(changeGuideHairsVisibility()));

	// Set the change function for hair shading
	setChangeFunction("rootColor", SLOT(changeFragmentShaderColor()));
	setChangeFunction("tipColor", SLOT(changeFragmentShaderColor()));
	setChangeFunction("diffuseColor", SLOT(changeDiffuseColor()));
	setChangeFunction("diffuseStrength", SLOT(changeDiffuseStrength()));
	setChangeFunction("colorGradientWidth", SLOT(changeFragmentShaderParameter()));
	setChangeFunction("colorGradientShift", SLOT(changeFragmentShaderParameter()));

	setChangeFunction("reflectColor", SLOT(changeReflectColor()));
	setChangeFunction("reflectStrength", SLOT(changeReflectStrength()));

	setChangeFunction("reflectWidth", SLOT(changeFragmentShaderParameter()));
	setChangeFunction("scatterColor", SLOT(changeScatterColor()));
	setChangeFunction("scatterStrength", SLOT(changeScatterStrength()));
	setChangeFunction("scatterWidth", SLOT(changeFragmentShaderParameter()));

	setChangeFunction("transmitColor", SLOT(changeTransmitColor()));
	setChangeFunction("transmitStrength", SLOT(changeTransmitStrength()));
	
	setChangeFunction("transmitWidth", SLOT(changeFragmentShaderParameter()));

	setChangeFunction("reflectShift", SLOT(changeReflectShift()));
	setChangeFunction("transmitShift", SLOT(changeTransmitShift()));
	setChangeFunction("scatterShift", SLOT(changeScatterShift()));

	setChangeFunction("lightPower", SLOT(changeFragmentShaderParameter()));

	setChangeFunction("Hair Style Initial Position", SLOT(changeInitialHairStylePosition()) );

	m_tessellationFactor = 64.0f;
	m_strandWidthLOD = 0.0f;
	m_NumberMaxOfHairSegments = -1;
	m_curlTextureSizeXY = 0.0f;
	m_deviationsTextureSizeXY = 0.0f;

	m_strandLength = NULL;
	m_thinning = 0.5f;

	m_coordinateFramesTextureSizeXY = 0.0f;
	m_scalpNormalsTextureSizeXY = 16.0f;
	m_scalpNormalIndicesTextureSizeXY = 16.0f;

	m_bendAngleMaxTessellation = Ogre::Math::DegreesToRadians(45.0f);;		// 45 degrees
	m_bendAngleMeaningful = Ogre::Math::DegreesToRadians(5.0f);					// 5 degrees

	m_rootVertexToScalpVertexDistance = 0.01f;

	m_scalpMeshSubEntity = NULL;

	// shading
	m_reflectColor = Ogre::ColourValue(0.9,0.9,0.9);
	m_reflectStrength = 3.0;
	m_transmitColor = Ogre::ColourValue(0.7,0.4,0.1);
	m_transmitStrength = 3.0;
	m_scatterColor = Ogre::ColourValue(0.5, 0.2, 0.05);
	m_scatterStrength = 3.0;
	m_diffuseColor = Ogre::ColourValue(0.55, 0.4, 0.24);
	m_diffuseStrength = 8.0;

	m_boneFound = false;
}


//!
//! Destructor of the AqtBlendNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
AnimatableMeshHairNode::~AnimatableMeshHairNode ()
{
	// destroy resources
	if( !m_hairMesh.isNull() )
	{
		Ogre::MeshManager::getSingleton().remove( m_hairMeshName );
	}

	if(m_strandLength)
	{
		delete [] m_strandLength;
	}

	// destroy textures
	Ogre::TextureManager& textureManager = Ogre::TextureManager::getSingleton();
	if(!m_hairCoordTex.isNull() && textureManager.resourceExists(m_hairCoordTex->getName()))
		textureManager.remove(m_hairCoordTex->getName());

	if(!m_hairTangentsTex.isNull() && textureManager.resourceExists(m_hairTangentsTex->getName()))
		textureManager.remove(m_hairTangentsTex->getName());

	if(!m_strandCoordinatesTex.isNull() && textureManager.resourceExists(m_strandCoordinatesTex->getName()))
		textureManager.remove(m_strandCoordinatesTex->getName());

	if(!m_hairIndexTex.isNull() && textureManager.resourceExists(m_hairIndexTex->getName()))
		textureManager.remove(m_hairIndexTex->getName());

	if(!m_hairIndexEndTex.isNull() && textureManager.resourceExists(m_hairIndexEndTex->getName()))
		textureManager.remove(m_hairIndexEndTex->getName());

	if(!m_hairCurlDeviationTex.isNull() && textureManager.resourceExists(m_hairCurlDeviationTex->getName()))
		textureManager.remove(m_hairCurlDeviationTex->getName());

	if(!m_hairDeviationsTex.isNull() && textureManager.resourceExists(m_hairDeviationsTex->getName()))
		textureManager.remove(m_hairDeviationsTex->getName());

	if(!m_strandLengthTex.isNull() && textureManager.resourceExists(m_strandLengthTex->getName()))
		textureManager.remove(m_strandLengthTex->getName());

	// todo: check why heap corruption if circular texture is removed
	//if(!m_randomCircularCoordinatesTex.isNull() && textureManager.resourceExists(m_randomCircularCoordinatesTex->getName()))
	//	textureManager.remove(m_randomCircularCoordinatesTex->getName());

	if(!m_coordinateFramesTex.isNull() && textureManager.resourceExists(m_coordinateFramesTex->getName()))
		textureManager.remove(m_coordinateFramesTex->getName());

	if(!m_maxDistanceAngleTex.isNull() && textureManager.resourceExists(m_maxDistanceAngleTex->getName()))
		textureManager.remove(m_maxDistanceAngleTex->getName());

	if(!m_scalpNormalsTex.isNull() && textureManager.resourceExists(m_scalpNormalsTex->getName()))
		textureManager.remove(m_scalpNormalsTex->getName());

	if(!m_scalpNormalIndicesTex.isNull() && textureManager.resourceExists(m_scalpNormalIndicesTex->getName()))
		textureManager.remove(m_scalpNormalIndicesTex->getName());

	if(!m_strandBendTex.isNull() && textureManager.resourceExists(m_strandBendTex->getName()))
		textureManager.remove(m_strandBendTex->getName());
}


//!
//! Changes the hair shader parameters.
//!
//! Is called when a GUI parameter has changed.
//!
void AnimatableMeshHairNode::changeShaderParameter(){

	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	if (m_grundMat.isNull())
		return;
	
	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuHullParam = pass->getTessellationHullProgramParameters();
	Ogre::GpuProgramParametersSharedPtr gpuDomainParam = pass->getTessellationDomainProgramParameters();
	//Ogre::GpuProgramParametersSharedPtr gpuGeometryParam = pass->getGeometryProgramParameters();

	//Ogre::MaterialPtr matPtr = Ogre::MaterialPtr(Ogre::MaterialManager::getSingleton().getByName(""));
	//Ogre::GpuProgramParametersSharedPtr params = m_grundMat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	// hull shader
	if (gpuHullParam->_findNamedConstantDefinition(name))
		gpuHullParam->setNamedConstant(name, value);

	// domain shader
	if(gpuDomainParam->_findNamedConstantDefinition(name))
		gpuDomainParam->setNamedConstant(name, value);

	// geometry shader
	//if (gpuGeometryParam->_findNamedConstantDefinition(name))
	//	gpuGeometryParam->setNamedConstant(name, value);
}


//!
//! Changes the hair fragment shader parameters.
//!
//! Is called when a GUI parameter has changed.
//!
void AnimatableMeshHairNode::changeFragmentShaderParameter(){

	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	if(m_grundMat.isNull())
		return;
	
	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	// fragment shader
	if (gpuFragmentParam->_findNamedConstantDefinition(name))
		gpuFragmentParam->setNamedConstant(name, value);
}


//!
//! Sets the scenes shader color in fragment shader
//!
void AnimatableMeshHairNode::changeFragmentShaderColor()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const QColor color = parameter->getValue().value<QColor>();
	Ogre::ColourValue oColor;
	
	oColor.r = color.redF();
	oColor.g = color.greenF();
	oColor.b = color.blueF();

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition(name))
		gpuFragmentParam->setNamedConstant(name, oColor);
}


//!
//! Changes scalp mesh visibility
//!
//! Is called when a GUI parameter has changed.
//!
void AnimatableMeshHairNode::changeScalpMeshVisibility()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const bool visible = parameter->getValue().toBool();

	if( m_scalpMeshSubEntity)
	{
		m_scalpMeshSubEntity->setVisible(visible);
	}
}


//!
//! Changes guide hairs visibility
//!
//! Is called when a GUI parameter has changed.
//!
void AnimatableMeshHairNode::changeGuideHairsVisibility()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const bool visible = parameter->getValue().toBool();
	if( m_entity )
	{
		const Ogre::MeshPtr mesh = m_entity->getMesh();
		Ogre::Mesh::SubMeshNameMap nameMap = mesh->getSubMeshNameMap();
		Ogre::Mesh::SubMeshNameMap::iterator nameMapIter;

		for ( nameMapIter = nameMap.begin(); nameMapIter != nameMap.end(); ++nameMapIter ) 
		{
			const QString &subMeshName = nameMapIter->first.c_str();
			Ogre::SubMesh *subMesh = mesh->getSubMesh(nameMapIter->first);
			Ogre::SubEntity *subEntity = m_entity->getSubEntity(nameMapIter->second);

			if( subMeshName.contains(m_hairGuideName, Qt::CaseInsensitive) )
			{
				subEntity->setVisible(visible);
			}
		}
	}
}

//!
//! Changes thinning parameter
//!
//! Thinning parameter is changed in shader and for deviations texture.
//!
void AnimatableMeshHairNode::changeThinningParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();
	
	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuDomainParam = pass->getTessellationDomainProgramParameters();

	// domain shader
	if(gpuDomainParam->_findNamedConstantDefinition(name))
		gpuDomainParam->setNamedConstant(name, value);

	m_thinning = value;

	//createDeviantStrands((m_NumberMaxOfHairSegments+1)*64);
	//setTexture(m_grundMat, m_hairDeviationsTex, 3, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
}


//!
//! Changes hair lod dependent on a slider with a value between 0.0 and 2.0
//!
void AnimatableMeshHairNode::changeHairLODParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	if(m_grundMat.isNull())
		return;
	
	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuHullParam = pass->getTessellationHullProgramParameters();
	Ogre::GpuProgramParametersSharedPtr gpuDomainParam = pass->getTessellationDomainProgramParameters();

	float detailFactor = 1.0f;
	if( value > 1.0 )
	{
		detailFactor = 1.0 + 63.0*(value-1.0);
		m_tessellationFactor = 64.0;
		m_strandWidthLOD = 1.0;
	}
	else
	{
		m_tessellationFactor = 1.0 + 63.0*value;

		float fNumberHair = value;
		if( fNumberHair < 0.02f )
		{
			fNumberHair = 0.02f;
		}

		m_strandWidthLOD = 1.0/fNumberHair;
	}

	// hull shader
	if (gpuHullParam->_findNamedConstantDefinition("g_detailFactor"))
		gpuHullParam->setNamedConstant("g_detailFactor", detailFactor);
	if (gpuHullParam->_findNamedConstantDefinition("g_tessellationFactor"))
		gpuHullParam->setNamedConstant("g_tessellationFactor", m_tessellationFactor);

	// domain shader
	if(gpuDomainParam->_findNamedConstantDefinition("g_strandWidthLOD"))
		gpuDomainParam->setNamedConstant("g_strandWidthLOD", m_strandWidthLOD);

	// update patches per tessellated strand and tessellation factor
	setValue("g_tessellationFactor", QString("%L1").arg(m_tessellationFactor), true);
	setValue("g_detailFactor", QString("%L1").arg(detailFactor), true);
}


//!
//! Changes segment screen size parameter
//!
void AnimatableMeshHairNode::changeSegmentScreenSizeParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	float screenSpaceTessellationScale = 1.0f/value;

	if(m_grundMat.isNull())
		return;

	// hull shader
	Ogre::GpuProgramParametersSharedPtr gpuHullParam = m_grundMat->getTechnique(0)->getPass(0)->getTessellationHullProgramParameters();
	if (gpuHullParam->_findNamedConstantDefinition("g_screenSpaceTessellationScale"))
		gpuHullParam->setNamedConstant("g_screenSpaceTessellationScale", screenSpaceTessellationScale);
}


//!
//! Change m_bendAngleMeaningful
//!
void AnimatableMeshHairNode::changeBendAngleMeaningfulParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	m_bendAngleMeaningful = Ogre::Math::DegreesToRadians(value);
}



void AnimatableMeshHairNode::changeBendAngleMaxTessellationParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	m_bendAngleMaxTessellation = Ogre::Math::DegreesToRadians(value);
}

//!
//! Change m_bendAngleMaxTessellation
//!
void AnimatableMeshHairNode::changeRootVertexToScalpVertexDistanceParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	m_rootVertexToScalpVertexDistance = value;
}

//!
//! Change between manual LOD and screen space adaptive LOD
//!
void AnimatableMeshHairNode::changeShaderLODParameter()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const bool value = parameter->getValue().toBool();

	if(m_grundMat.isNull())
		return;

	Ogre::GpuProgramParametersSharedPtr gpuHullParam = m_grundMat->getTechnique(0)->getPass(0)->getTessellationHullProgramParameters();
	
	if( value )
	{
		if (gpuHullParam->_findNamedConstantDefinition("g_screenSpaceAdpativeTessellation"))
			gpuHullParam->setNamedConstant("g_screenSpaceAdpativeTessellation", 1.0f);
	}
	else
	{
		if (gpuHullParam->_findNamedConstantDefinition("g_screenSpaceAdpativeTessellation"))
			gpuHullParam->setNamedConstant("g_screenSpaceAdpativeTessellation", 0.0f);
	}
}


//!
//! Change reflect color variable of hair
//!
void AnimatableMeshHairNode::changeReflectColor()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const QColor color = parameter->getValue().value<QColor>();
	
	m_reflectColor.r = color.redF();
	m_reflectColor.g = color.greenF();
	m_reflectColor.b = color.blueF();

	changeSpecularLobesColorsR();
}


//!
//! Change reflect color variable of hair
//!
void AnimatableMeshHairNode::changeReflectStrength()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	m_reflectStrength = value;

	changeSpecularLobesColorsR();
}


//!
//! Change specular lobes color for R
//!
void AnimatableMeshHairNode::changeSpecularLobesColorsR()
{
	Ogre::ColourValue oColor = 0.25f * m_reflectColor * m_reflectStrength;

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition("_AM_c_R"))
	{
		gpuFragmentParam->setNamedConstant("_AM_c_R", oColor);
	}
}


//!
//!	Change transmit color for hair
//!
void AnimatableMeshHairNode::changeTransmitColor()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const QColor color = parameter->getValue().value<QColor>();
	
	m_transmitColor.r = color.redF();
	m_transmitColor.g = color.greenF();
	m_transmitColor.b = color.blueF();

	changeSpecularLobesColorsTT();
}


//!
//! Change transmit strength for hair
//!
void AnimatableMeshHairNode::changeTransmitStrength()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	m_transmitStrength = value;

	changeSpecularLobesColorsTT();
}


//!
//! Change specular lobes color for TT
//!
void AnimatableMeshHairNode::changeSpecularLobesColorsTT()
{
	Ogre::ColourValue oColor = 0.25f * m_transmitColor * m_transmitStrength;

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition("_AM_c_TT"))
		gpuFragmentParam->setNamedConstant("_AM_c_TT", oColor);
}


//!
//! Change scatter color for hair
//!
void AnimatableMeshHairNode::changeScatterColor()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const QColor color = parameter->getValue().value<QColor>();
	
	m_scatterColor.r = color.redF();
	m_scatterColor.g = color.greenF();
	m_scatterColor.b = color.blueF();

	changeSpecularLobesColorsTRT();
}

//!
//! Change scatter strength for hair
//!
void AnimatableMeshHairNode::changeScatterStrength()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	m_scatterStrength = value;

	changeSpecularLobesColorsTRT();
}


//!
//! Change specular lobes for TRT
//!
void AnimatableMeshHairNode::changeSpecularLobesColorsTRT()
{
	Ogre::ColourValue oColor = 0.25f * m_scatterColor * m_scatterStrength;

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition("_AM_c_TRT"))
		gpuFragmentParam->setNamedConstant("_AM_c_TRT", oColor);
}


//!
//! Change diffuse color for hair
//!
void AnimatableMeshHairNode::changeDiffuseColor()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const QColor color = parameter->getValue().value<QColor>();
	
	m_diffuseColor.r = color.redF();
	m_diffuseColor.g = color.greenF();
	m_diffuseColor.b = color.blueF();

	changeDiffuseColorStrengthHair();
}


//!
//! Change diffuse strength for hair
//!
void AnimatableMeshHairNode::changeDiffuseStrength()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	m_diffuseStrength = value;

	changeDiffuseColorStrengthHair();
}


//!
//! Change diffuse color for hair, which combines strength and color
//!
void AnimatableMeshHairNode::changeDiffuseColorStrengthHair()
{
	Ogre::ColourValue oColor = 0.08f * m_diffuseColor * m_diffuseStrength;

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition("_AM_d"))
		gpuFragmentParam->setNamedConstant("_AM_d", oColor);
}


//!
//! Change reflect angular shift of specular lob R
//!
void AnimatableMeshHairNode::changeReflectShift()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	const float _AM_s_R = value / (PIG);

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition("_AM_s_R"))
		gpuFragmentParam->setNamedConstant("_AM_s_R", _AM_s_R);
}


//!
//! Change transmit angular shift of specular lob R
//!
void AnimatableMeshHairNode::changeTransmitShift()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	const float _AM_s_TT = value / (PIG);

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition("_AM_s_TT"))
		gpuFragmentParam->setNamedConstant("_AM_s_TT", _AM_s_TT);
}


//!
//! Change scatter angular shift of specular lob R
//!
void AnimatableMeshHairNode::changeScatterShift()
{
	Parameter* parameter = static_cast<Parameter*>(sender());
	const Ogre::String &name = parameter->getName().toStdString();
	const float value = parameter->getValue().toFloat();

	const float _AM_s_TRT = value / (PIG);

	if(m_grundMat.isNull())
		return;

	Ogre::Technique* technique = m_grundMat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = pass->getFragmentProgramParameters();

	if (gpuFragmentParam->_findNamedConstantDefinition("_AM_s_TRT"))
		gpuFragmentParam->setNamedConstant("_AM_s_TRT", _AM_s_TRT);
}


//!
//! Change initial position of hair style
//!
void AnimatableMeshHairNode::changeInitialHairStylePosition()
{
	if(m_boneFound)
	{
		const Ogre::Vector3 value = getVectorValue("Hair Style Initial Position");

		m_entity->detachObjectFromBone(m_hairEntity);
		m_entity->attachObjectToBone(m_boneNameToAssignToHair, m_hairEntity, Ogre::Quaternion::IDENTITY, value);
		m_entity->getAttachedObjectIterator().getNext();
	}
}


//!
//! Loads the animation mesh from file.
//!
//! \return True if the animation mesh was successfully loaded, otherwise False.
//!
bool AnimatableMeshHairNode::loadMesh ()
{
	QString filename = getStringValue("Geometry File");
	if (filename == "") {
		Log::debug(QString("Geometry file has not been set yet. (\"%1\")").arg(m_name), "GeometryAnimationNode::loadMesh");
		return false;
	}

	bool singleStrandHair = getBoolValue("Single Strand");

	// obtain the OGRE scene manager
	Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
	if (!sceneManager) {
		Log::error("Could not obtain OGRE scene manager.", "GeometryAnimationNode::loadMesh");
		return false;
	}

	// destroy an existing OGRE entity for the mesh
	destroyEntity();

	// create new scene node
	m_sceneNode = OgreManager::createSceneNode(m_name);
	if (!m_sceneNode) {
		Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "GeometryAnimationNode::loadMesh");
		return false;
	}
	setValue(m_outputGeometryName, m_sceneNode, true);

	// check if the file exists
	if (!QFile::exists(filename)) {
		Log::error(QString("Mesh file \"%1\" not found.").arg(filename), "GeometryAnimationNode::loadMesh");
		return false;
	}

	// split the absolute filename to path and base filename
	int lastSlashIndex = filename.lastIndexOf('/');
	QString path = "";
	if (lastSlashIndex > -1) {
		path = filename.mid(0, lastSlashIndex);
		filename = filename.mid(lastSlashIndex + 1);
	}
	if (!filename.endsWith(".mesh")) {
		Log::error("The geometry file has to be an OGRE mesh file.", "GeometryAnimationNode::loadMesh");
		return false;
	}

	// destroy old resource group and generate new one
	QString resourceGroupName = QString::fromStdString(createUniqueName("ResourceGroup_" + filename + "_AnimatableMeshHair"));
	OgreTools::destroyResourceGroup(m_oldResourceGroupName);
	m_oldResourceGroupName = resourceGroupName;
	OgreTools::createResourceGroup(resourceGroupName, path);

	int debugFloatCounter = 0;

	// create a new OGRE entity for the mesh file
	m_entity = sceneManager->createEntity(m_name.toStdString(), filename.toStdString(), resourceGroupName.toStdString());

	if ( m_entity ) {
		const Ogre::MeshPtr mesh = m_entity->getMesh();
		const Ogre::Mesh::SubMeshNameMap &nameMap = mesh->getSubMeshNameMap();
		const int numOfSubentities = m_entity->getNumSubEntities();

		// Create unique name for hair-texture
		Ogre::String hairCoordName = createUniqueName("HairCoordTexture_AnimatableMeshHairNode");
		Ogre::String hairTangentsName = createUniqueName("HairTangentsTexture_AnimatableMeshHairNode");

		// Number of guide hairs should be equal than "m_entity->getNumSubEntities() - 1" after loop below;
		m_NumberOfGuideHairs = 0;

		// Number of segments per guide-hair should be equal for all guide strands
		m_NumberMaxOfHairSegments = -1;

		// total number of values (number of vertices of all guide hairs)
		m_NumberOverallHairGuideVertices = 0;

		// Index of vertex buffer
		const int bufferIdx = 0;

		size_t hairIndexCount = 0;
		size_t hairGrowthMeshIndexCount = 0;
		Ogre::SubMesh *meshShapeSubMesh = NULL;

		std::vector<std::vector<Ogre::Vector3>> hairStrands;

		// Loop to figure out the number of segments per guide strand
		// and check for all guides have the same amount of segments.
		// In addition the number of guide-hairs is counted

		// Get scalp mesh name
		bool scalpMeshFound = false;
		const QString scalpMeshName = getStringValue("Scalp Mesh Name");
		// Get hair guides name
		m_hairGuideName = getStringValue("Hair Guides Name");

		for (Ogre::Mesh::SubMeshNameMap::const_iterator &nameMapIter = nameMap.begin(); nameMapIter != nameMap.end(); ++nameMapIter) {
			const QString &subMeshName = nameMapIter->first.c_str();
			Ogre::SubMesh *subMesh = mesh->getSubMesh(nameMapIter->first);

			if( subMeshName.contains(scalpMeshName, Qt::CaseInsensitive) ) { 
				scalpMeshFound = true;

				hairGrowthMeshIndexCount = subMesh->indexData->indexCount / 3;
				meshShapeSubMesh = subMesh;
				m_scalpMeshSubEntity =  m_entity->getSubEntity(nameMapIter->second);

				if(!getBoolValue("Render Scalp Mesh"))
					m_scalpMeshSubEntity->setVisible(false);
			}

			// Check for working only on hair geo and not on the groth-mesh
			if (subMeshName.contains(m_hairGuideName, Qt::CaseInsensitive)) {
				m_NumberOfGuideHairs++;

				// Number of vertices in current submesh
				const int numOfGuideVertices = subMesh->vertexData->vertexBufferBinding->getBuffer(bufferIdx)->getNumVertices();
				// Number of segments (calculated from number of vertices minus the start/end vertex)
				const int tempNumOfGuideHairSegments = numOfGuideVertices - 1;

				m_NumberOverallHairGuideVertices += numOfGuideVertices;

				// This is done only for the first hair submesh
				//if (m_NumberMaxOfHairSegments < 0)
				// save maximum number of hair segments
				if(m_NumberMaxOfHairSegments < tempNumOfGuideHairSegments) {
					// Number of vertices in current submesh
					m_NumberMaxOfHairSegments = tempNumOfGuideHairSegments;
				}
			}
		}
		setValue("Number of Hair Segments", QString("%L1").arg(m_NumberMaxOfHairSegments), true);


		if(!scalpMeshFound) {
			Log::error("Scalp mesh: '" + scalpMeshName + "' has not been found!", "GeometryAnimationNode::loadMesh");
			return false;
		}

		if(m_NumberOfGuideHairs == 0) {
			Log::error("Guide hairs containing name: '" + m_hairGuideName + "' have not been found!", "GeometryAnimationNode::loadMesh");
			return false;
		}

		setValue("Number of Guide Hairs", QString("%L1").arg(m_NumberOfGuideHairs), true);


		// Search for textures
		//int lastDotIndex = filename.lastIndexOf('.');
		//QString meshFilename = filename.mid(0, lastDotIndex);
		//QString textureFilenamePrefix = path + "/" + meshFilename;
		//QString textureFilename = textureFilenamePrefix + "_HairCoordTex.png";

		//m_hairCoordTex = Ogre::TextureManager::getSingleton().load(textureFilename.toStdString(), "General");


		// Calculate size of texture. (The texture for storing vertex positions)
		// The texture has to be quadratic and its length has to be always an number power of 2
		const int texturesizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)m_NumberOverallHairGuideVertices)));
		setValue("Texture Size", QString("%L1").arg(texturesizeXY), true);

		// Break if size calculation failed (due to a bug in NextPow2)
		if (texturesizeXY <= 0)
			return false;


		// Create ogre texture manual for hair vertex data
		m_hairCoordTex = Ogre::TextureManager::getSingleton().createManual(
			hairCoordName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			texturesizeXY,
			texturesizeXY,
			0,
			Ogre::PF_FLOAT32_RGBA,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		// Pixel Buffer of the texture for guide hair vertex positions
		Ogre::HardwarePixelBufferSharedPtr hairPixelBuffer = m_hairCoordTex->getBuffer(0,0);

		// Lock the pixel buffer in a way that we can write to it
		hairPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
		const Ogre::PixelBox &pb = hairPixelBuffer->getCurrentLock();
		Ogre::Real* pFloat = 0;
		pFloat = static_cast<Ogre::Real*>(pb.data);
		int writeVertexPos = 0;


		// Create ogre texture manual for hair vertex tangents data
		m_hairTangentsTex = Ogre::TextureManager::getSingleton().createManual(
			hairTangentsName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			texturesizeXY,
			texturesizeXY,
			0,
			Ogre::PF_FLOAT32_RGBA,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		// Pixel Buffer of the texture for guide hair vertex tangets
		Ogre::HardwarePixelBufferSharedPtr hairTangentsPixelBuffer = m_hairTangentsTex->getBuffer(0,0);

		// Lock the pixel buffer in a way that we can write to it
		hairTangentsPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
		const Ogre::PixelBox &pixelBoxHairTangents = hairTangentsPixelBuffer->getCurrentLock();
		Ogre::Real* pFloatTangents = 0;
		pFloatTangents = static_cast<Ogre::Real*>(pixelBoxHairTangents.data);
		int writeTangentsPos = 0;

		//size_t hairMeshIndexCount = 0;			// how many indices are needed for the final hair
		int indexTexturesizeXY = 0;

		if( meshShapeSubMesh ) {
			// guide hair strand data for creation of index buffer
			std::vector<GuideHairIndexData> guideHairIndexData;

			const bool renderGuideStrands = getBoolValue("Render Guide Strands");

			// Read vertex data, search for corresponding guide strand and save guide strand to texture
			// --------------------------
			// Get the format (vertex declaration) of the vertices in the buffer
			const Ogre::VertexDeclaration* meshShapeDecl = meshShapeSubMesh->vertexData->vertexDeclaration;

			// Get the number of the attributes (elements) in the buffer
			int numOfElementsInBuffer = meshShapeDecl->getElementCount();

			// Get Vertex Buffer
			Ogre::HardwareVertexBufferSharedPtr meshShapeVBuffer = meshShapeSubMesh->vertexData->vertexBufferBinding->getBuffer(bufferIdx);

			// Lock vertex-buffer to access content
			unsigned char* meshShapeVertexBufferPtr = static_cast<unsigned char*>(meshShapeVBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			Ogre::Real* pMeshShapeVertex;
			Ogre::Real* pMeshShapeNormal;

			const size_t numVertices = meshShapeSubMesh->vertexData->vertexBufferBinding->getBuffer(bufferIdx)->getNumVertices();

			std::list<Ogre::Vector3> normalList;		// index list of scalp mesh normals

			// Read mesh vertex data to texture buffer
			for (size_t v = 0; v < numVertices; ++v) {
				// Get elements
				const Ogre::VertexDeclaration::VertexElementList &elemsList = meshShapeDecl->findElementsBySource(bufferIdx);
				Ogre::VertexDeclaration::VertexElementList::const_iterator i, iend;

				// Loop through elements
				for ( i = elemsList.begin(); i != elemsList.end(); ++i ) {

					if ( i->getSemantic() == Ogre::VertexElementSemantic::VES_POSITION ) {
						i->baseVertexPointerToElement(meshShapeVertexBufferPtr, &pMeshShapeVertex);

						Ogre::Vector3 meshVertex( pMeshShapeVertex[0], pMeshShapeVertex[1], pMeshShapeVertex[2]);

						bool guideStrandFound = false;

						// Loop through all submeshes again the to write guide strand vertex positions into the texture
						for (Ogre::Mesh::SubMeshNameMap::const_iterator nameMapIter = nameMap.begin(); nameMapIter != nameMap.end(); ++nameMapIter)  {
							const QString &subMeshName = nameMapIter->first.c_str();
							Ogre::SubMesh *subMesh = mesh->getSubMesh(nameMapIter->first);
							Ogre::SubEntity *subEntity = m_entity->getSubEntity(nameMapIter->second);

							// write positions to texture
							if( subMeshName.contains(m_hairGuideName, Qt::CaseInsensitive) ) {
								// Get the format (vertex declaration) of the vertices in the buffer
								const Ogre::VertexDeclaration* decl = subMesh->vertexData->vertexDeclaration;

								// Get the number of the attributes (elements) in the buffer
								int numOfElementsInBuffer = decl->getElementCount();

								// Get Vertex Buffer
								Ogre::HardwareVertexBufferSharedPtr vBuffer = subMesh->vertexData->vertexBufferBinding->getBuffer(bufferIdx);

								// Lock vertex-buffer to access content
								unsigned char* vertexBufferPtr = static_cast<unsigned char*>(vBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

								Ogre::Real* pReal;

								// check first vertex if it is the right one we search for
								// get first vertex
								// Get elements
								Ogre::VertexDeclaration::VertexElementList elemsList = decl->findElementsBySource(bufferIdx);
								Ogre::VertexDeclaration::VertexElementList::const_iterator i, iend;

								// Loop through elements
								for ( i = elemsList.begin(); i != elemsList.end(); ++i ) {
									if ( i->getSemantic() == Ogre::VertexElementSemantic::VES_POSITION )
										i->baseVertexPointerToElement(vertexBufferPtr, &pReal);
								}

								// check if distance of the vertices is small enough
								Ogre::Vector3 guideRootVertex(pReal[0], pReal[1], pReal[2]);
								float distance = meshVertex.distance(guideRootVertex);

								if ( distance <= m_rootVertexToScalpVertexDistance ) {
									// check if vertices are the same
									//if(	pReal[0] == pMeshShapeVertex[0] &&
									//	pReal[1] == pMeshShapeVertex[1] &&
									//	pReal[2] == pMeshShapeVertex[2] )
									guideStrandFound = true;

									// found correct guide strand
									const size_t numVertices = subMesh->vertexData->vertexBufferBinding->getBuffer(bufferIdx)->getNumVertices();
									const float currentNumOfGuideHairSegments = (float)(numVertices - 1);

									// save guide hair data
									GuideHairIndexData currentGuideHairIndexData;
									currentGuideHairIndexData.rootTexturePosition = writeVertexPos/4;
									currentGuideHairIndexData.strandSize = (float)(numVertices-1);
									guideHairIndexData.push_back(currentGuideHairIndexData);

									// Indices we need to render hair
									//hairMeshIndexCount = hairMeshIndexCount + numVertices;

									std::list<Ogre::Vector3> currentStrand;

									// needed to save hair strand vector data and use for coordinate frame calculation
									std::vector<Ogre::Vector3> currentStrandVector;

									// Loop to write guid hair vertex data to texture buffer
									for (size_t v = 0; v < numVertices; ++v) {
										// Get elements
										Ogre::VertexDeclaration::VertexElementList elemsList = decl->findElementsBySource(bufferIdx);
										Ogre::VertexDeclaration::VertexElementList::const_iterator i, iend;

										// Loop through elements
										for ( i = elemsList.begin(); i != elemsList.end(); ++i ) {

											if ( i->getSemantic() == Ogre::VertexElementSemantic::VES_POSITION ) {
												i->baseVertexPointerToElement(vertexBufferPtr, &pReal);

#ifdef WRITE_PREPARE_HAIR_LOG
												std::cout << "Write to texture Position " << writeVertexPos << " -> " << pReal[0] << " " << pReal[1] << " " << pReal[2] << " 1.0" << std::endl;
#endif
												// write vertex
												pFloat[writeVertexPos++] = pReal[0];
												pFloat[writeVertexPos++] = pReal[1];
												pFloat[writeVertexPos++] = pReal[2];
												pFloat[writeVertexPos++] = (float)(numVertices-1-v);			// position distance to to hair tip

												Ogre::Vector3 currentStrandVertex(pReal[0], pReal[1], pReal[2]);
												currentStrand.push_back( currentStrandVertex );
												currentStrandVector.push_back( currentStrandVertex );
											}

										}
										vertexBufferPtr += vBuffer->getVertexSize();
									}

									// Unlock vertex buffer
									vBuffer->unlock();

									// add hair strand to vector
									hairStrands.push_back( currentStrandVector );

									// calculate tangents
									std::list<Ogre::Vector3>::const_iterator iCurrent;
									std::list<Ogre::Vector3>::const_iterator iPrevious = currentStrand.begin();
									std::list<Ogre::Vector3>::const_iterator iNext = currentStrand.begin();
									++iNext;

									for (iCurrent = currentStrand.begin(); iCurrent != currentStrand.end(); ++iCurrent) {
										Ogre::Vector3 tangent;

										// 1st tangents
										if( iPrevious == currentStrand.begin() ) {
											// first element
											tangent = 0.5 * ( *iNext - *iCurrent );
											++iNext;
										}
										else {
											Ogre::Vector3 previousVertex = *iPrevious;

											if( iNext == currentStrand.end() ) {
												// last element
												tangent = 0.5 * ( *iCurrent - *iPrevious );
											}
											else {
												tangent = 0.5 * ( *iNext - *iPrevious );
												++iNext;
											}
										}

										// write tangent to texture
										pFloatTangents[writeTangentsPos++] = tangent.x;
										pFloatTangents[writeTangentsPos++] = tangent.y;
										pFloatTangents[writeTangentsPos++] = tangent.z;
										pFloatTangents[writeTangentsPos++] = currentNumOfGuideHairSegments;

										iPrevious = iCurrent;
									}
									break;
								}

								// Unlock vertex buffer
								vBuffer->unlock();
							}
							if(!renderGuideStrands) {
								// do not render guide hair
								subEntity->setVisible(false);
							}
						}

						if( !guideStrandFound ) {
							Log::error("Hair mesh file error: no guide strand found for scalp mesh vertex.", "AnimatableMeshHairNode::loadMesh");
							return false;
						}
					}
					else if ( i->getSemantic() == Ogre::VertexElementSemantic::VES_NORMAL ) {
						i->baseVertexPointerToElement(meshShapeVertexBufferPtr, &pMeshShapeNormal);
						Ogre::Vector3 normal(pMeshShapeNormal[0], pMeshShapeNormal[1], pMeshShapeNormal[2]);
						normalList.push_back(normal);
					}

				}
				meshShapeVertexBufferPtr += meshShapeVBuffer->getVertexSize();
			}

			if(normalList.empty()) {
				// normal were not found -> saved in an extra buffer
				// Get Vertex Buffer
				size_t bufferCount = meshShapeSubMesh->vertexData->vertexBufferBinding->getBufferCount();

				for(size_t bufferIdx = 1; normalList.empty() && bufferIdx < bufferCount ; bufferIdx++) {
					Ogre::HardwareVertexBufferSharedPtr meshShapeVBuffer = meshShapeSubMesh->vertexData->vertexBufferBinding->getBuffer(bufferIdx);

					// Lock vertex-buffer to access content
					unsigned char* meshShapeVertexBufferPtr = static_cast<unsigned char*>(meshShapeVBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

					Ogre::Real* pMeshShapeVertex;
					Ogre::Real* pMeshShapeNormal;

					const size_t numVertices = meshShapeSubMesh->vertexData->vertexBufferBinding->getBuffer(bufferIdx)->getNumVertices();

					// Read mesh vertex data to texture buffer
					for (size_t v = 0; v < numVertices; ++v) {
						// Get elements
						Ogre::VertexDeclaration::VertexElementList elemsList = meshShapeDecl->findElementsBySource(bufferIdx);
						Ogre::VertexDeclaration::VertexElementList::iterator i, iend;

						// Loop through elements
						for ( i = elemsList.begin(); i != elemsList.end(); ++i ) {

							if ( i->getSemantic() == Ogre::VertexElementSemantic::VES_NORMAL ) {
								i->baseVertexPointerToElement(meshShapeVertexBufferPtr, &pMeshShapeNormal);
								Ogre::Vector3 normal(pMeshShapeNormal[0], pMeshShapeNormal[1], pMeshShapeNormal[2]);
								normalList.push_back(normal);
							}
						}

						meshShapeVertexBufferPtr += meshShapeVBuffer->getVertexSize();
					}
					// Unlock vertex buffer
					meshShapeVBuffer->unlock();
				}
			}

			// --------------------------
			// calculate index data texture
			// --------------------------
			std::list<Ogre::Vector3> indexDataList;			// start of guide strand segment
			std::list<Ogre::Vector3> indexEndDataList;		// end of guide strand segment
			std::list<Ogre::Vector3> indexNormalList;		// index list of scalp mesh normals

			// read scalp mesh index data
			Ogre::IndexData* index_data = meshShapeSubMesh->indexData;

			size_t numTris = index_data->indexCount / 3;
			unsigned short* pShort;
			unsigned int* pInt;
			Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
			bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

			if (use32bitindexes) 
				pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			else
				pShort = static_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			if(use32bitindexes) {
				for(size_t k = 0; k < numTris; ++k) {
					unsigned int vindex1 = *pInt++;
					unsigned int vindex2 = *pInt++;
					unsigned int vindex3 = *pInt++;

					GuideHairIndexData indexData1 = guideHairIndexData.at(vindex1);
					GuideHairIndexData indexData2 = guideHairIndexData.at(vindex2);
					GuideHairIndexData indexData3 = guideHairIndexData.at(vindex3);

					int shortestStrandSize = indexData1.strandSize;

					if( indexData2.strandSize < shortestStrandSize )
						shortestStrandSize = indexData2.strandSize;

					if( indexData3.strandSize < shortestStrandSize )
						shortestStrandSize = indexData3.strandSize;

					for(size_t i=0; i < shortestStrandSize; i++) {
						// start index
						Ogre::Vector3 indexVector;

						indexVector.x = indexData1.rootTexturePosition + ceil(i * ( ( indexData1.strandSize ) / shortestStrandSize ) );
						indexVector.y = indexData2.rootTexturePosition + ceil(i * ( ( indexData2.strandSize ) / shortestStrandSize ) );
						indexVector.z = indexData3.rootTexturePosition + ceil(i * ( ( indexData3.strandSize ) / shortestStrandSize ) );

						indexDataList.push_back(indexVector);

						// end index
						Ogre::Vector3 indexVectorEnd;

						indexVectorEnd.x = indexData1.rootTexturePosition + ceil((i+1) * ( ( indexData1.strandSize ) / shortestStrandSize ) );
						indexVectorEnd.y = indexData2.rootTexturePosition + ceil((i+1) * ( ( indexData2.strandSize ) / shortestStrandSize ) );
						indexVectorEnd.z = indexData3.rootTexturePosition + ceil((i+1) * ( ( indexData3.strandSize ) / shortestStrandSize ) );

						indexEndDataList.push_back(indexVectorEnd);

						// index for normals
						Ogre::Vector3 indexNormalVector(vindex1, vindex2, vindex3);
						indexNormalList.push_back(indexNormalVector);
					}
				}
			}
			else {
				for(size_t k = 0; k < numTris; ++k) {
					unsigned int vindex1 = *pShort++;
					unsigned int vindex2 = *pShort++;
					unsigned int vindex3 = *pShort++;

					GuideHairIndexData indexData1 = guideHairIndexData.at(vindex1);
					GuideHairIndexData indexData2 = guideHairIndexData.at(vindex2);
					GuideHairIndexData indexData3 = guideHairIndexData.at(vindex3);

					float shortestStrandSize = indexData1.strandSize;

					if( indexData2.strandSize < shortestStrandSize )
						shortestStrandSize = indexData2.strandSize;

					if( indexData3.strandSize < shortestStrandSize )
						shortestStrandSize = indexData3.strandSize;

					for(size_t i=0; i < shortestStrandSize; i++) {
						// start index
						Ogre::Vector3 indexVector;

						indexVector.x = indexData1.rootTexturePosition + ceil(i * ( ( indexData1.strandSize ) / shortestStrandSize ) );
						indexVector.y = indexData2.rootTexturePosition + ceil(i * ( ( indexData2.strandSize ) / shortestStrandSize ) );
						indexVector.z = indexData3.rootTexturePosition + ceil(i * ( ( indexData3.strandSize ) / shortestStrandSize ) );

						indexDataList.push_back(indexVector);

						// end index
						Ogre::Vector3 indexVectorEnd;

						indexVectorEnd.x = indexData1.rootTexturePosition + ceil((i+1) * ( ( indexData1.strandSize ) / shortestStrandSize ) );
						indexVectorEnd.y = indexData2.rootTexturePosition + ceil((i+1) * ( ( indexData2.strandSize ) / shortestStrandSize ) );
						indexVectorEnd.z = indexData3.rootTexturePosition + ceil((i+1) * ( ( indexData3.strandSize ) / shortestStrandSize ) );

						indexEndDataList.push_back(indexVectorEnd);

						// index for normals
						Ogre::Vector3 indexNormalVector(vindex1, vindex2, vindex3);
						indexNormalList.push_back(indexNormalVector);
					}
				}
			}

			createScalpMeshNormalTexture(normalList, indexNormalList);

			ibuf->unlock();
			// --------------------------

			// Unlock vertex buffer
			meshShapeVBuffer->unlock();


			// Index texture for multi strand interpolation
			//--------------------------------------
			hairIndexCount = indexDataList.size();
			indexTexturesizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)hairIndexCount)));

			Ogre::String hairIndexName = createUniqueName("HairIndexTexture_AnimatableMeshHairNode");
			// Create ogre texture manual for hair strands index data
			m_hairIndexTex = Ogre::TextureManager::getSingleton().createManual(
				hairIndexName,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				Ogre::TEX_TYPE_2D,
				indexTexturesizeXY,
				indexTexturesizeXY,
				0,
				Ogre::PF_FLOAT32_RGB,
				Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

			// Pixel Buffer of the texture for guide hair vertex tangets
			Ogre::HardwarePixelBufferSharedPtr hairIndexPixelBuffer = m_hairIndexTex->getBuffer(0,0);

			// Lock the pixel buffer in a way that we can write to it
			hairIndexPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
			const Ogre::PixelBox &pixelBoxHairIndex = hairIndexPixelBuffer->getCurrentLock();
			Ogre::Real* pFloatIndices = 0;
			pFloatIndices = static_cast<Ogre::Real*>(pixelBoxHairIndex.data);
			int writeIndexPos = 0;
			//--------------------------------------

			// write index data
			for (std::list<Ogre::Vector3>::iterator iterIndexData = indexDataList.begin(); iterIndexData != indexDataList.end(); ++iterIndexData) {
				pFloatIndices[writeIndexPos++] = (*iterIndexData).x;
				pFloatIndices[writeIndexPos++] = (*iterIndexData).y;
				pFloatIndices[writeIndexPos++] = (*iterIndexData).z;
			}

			hairIndexPixelBuffer->unlock();

			// Index end texture for multi strand interpolation
			//--------------------------------------
			Ogre::String hairIndexEndName = createUniqueName("HairIndexEndTexture_AnimatableMeshHairNode");
			// Create ogre texture manual for hair strands index data
			m_hairIndexEndTex = Ogre::TextureManager::getSingleton().createManual(
				hairIndexEndName,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				Ogre::TEX_TYPE_2D,
				indexTexturesizeXY,
				indexTexturesizeXY,
				0,
				Ogre::PF_FLOAT32_RGB,
				Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

			// Pixel Buffer of the texture for guide hair vertex tangets
			Ogre::HardwarePixelBufferSharedPtr hairIndexEndPixelBuffer = m_hairIndexEndTex->getBuffer(0,0);

			// Lock the pixel buffer in a way that we can write to it
			hairIndexEndPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
			const Ogre::PixelBox &pixelBoxHairIndexEnd = hairIndexEndPixelBuffer->getCurrentLock();
			Ogre::Real* pFloatIndicesEnd = 0;
			pFloatIndicesEnd = static_cast<Ogre::Real*>(pixelBoxHairIndexEnd.data);
			int writeIndexEndPos = 0;
			//--------------------------------------

			// write index end data
			for (std::list<Ogre::Vector3>::iterator iterIndexEndData = indexEndDataList.begin(); iterIndexEndData != indexEndDataList.end(); ++iterIndexEndData) {
				pFloatIndicesEnd[writeIndexEndPos++] = (*iterIndexEndData).x;
				pFloatIndicesEnd[writeIndexEndPos++] = (*iterIndexEndData).y;
				pFloatIndicesEnd[writeIndexEndPos++] = (*iterIndexEndData).z;
			}

			hairIndexEndPixelBuffer->unlock();
		}

		// Unlock the buffer again (frees it for use by the GPU)
		hairPixelBuffer->unlock();
		hairTangentsPixelBuffer->unlock();

		// remove all guide hairs and the scalp mesh when both are disabled
		bool renderGuideStrands = getBoolValue("Render Guide Strands");
		bool renderScalpMesh = getBoolValue("Render Scalp Mesh");

		if( !renderGuideStrands && !renderScalpMesh ) {
			Ogre::Mesh::SubMeshNameMap nameMapCopy = mesh->getSubMeshNameMap();
			for (Ogre::Mesh::SubMeshNameMap::iterator nameMapIter = nameMapCopy.begin(); nameMapIter != nameMapCopy.end(); ++nameMapIter) {
				const QString &subMeshName = nameMapIter->first.c_str();

				if( subMeshName.contains(m_hairGuideName, Qt::CaseInsensitive) ||
					subMeshName.contains(scalpMeshName, Qt::CaseInsensitive) ) {
						mesh->destroySubMesh(nameMapIter->first);
				}
			}
			m_scalpMeshSubEntity = NULL;
		}

		m_entity->_initialise(true);

#ifdef WRITE_PREPARE_HAIR_LOG
		std::cout << "texturesizeXY = " << texturesizeXY << " \^ 2 = " << (texturesizeXY * texturesizeXY)
			<< "\nBuffersize in Byte: " << hairPixelBuffer->getSizeInBytes() << " / 4 = " << (hairPixelBuffer->getSizeInBytes() / 4)
			<< std::endl;
#endif

		// Write out the Texture for debugging (doesn't work at the current ogre version 1.9)
		//hairPixelBuffer->getRenderTarget()->writeContentsToFile("test.tiff");


		// Create buffer for random generated strand barycentric coodinates, which are used for multi-strand interpolation
		const int numStrandVariables = 1024;//size of Interpolated hair variables. this is used for hair thickness, interpolation coordinates

		// Create unique name for hair-texture
		Ogre::String strandCoordinatesName = createUniqueName("StrandCoordinates_AnimatableMeshHairNode");
		// Create ogre texture manual
		m_strandCoordinatesTex = Ogre::TextureManager::getSingleton().createManual(
			strandCoordinatesName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_1D,
			numStrandVariables,
			1,
			0,
			Ogre::PF_FLOAT32_RGB,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		// Pixel Buffer of the texture for guide hair vertex positions
		Ogre::HardwarePixelBufferSharedPtr strandCoordinatesBuffer = m_strandCoordinatesTex->getBuffer(0,0);

		// Lock the pixel buffer in a way that we can write to it
		strandCoordinatesBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
		const Ogre::PixelBox &strandCoordinatesPixelBox = strandCoordinatesBuffer->getCurrentLock();
		Ogre::Real* pStrandCoordinatesFloat = 0;
		pStrandCoordinatesFloat = static_cast<Ogre::Real*>(strandCoordinatesPixelBox.data);

		int writePosition = 0;
		float coord1 = 1;
		float coord2 = 0;

		for( int i=0; i < numStrandVariables; i++) {
			coord1 = Ogre::Math::RangeRandom(0.0, 1.0);
			coord2 = Ogre::Math::RangeRandom(0.0, 1.0);

			// sum has to be smaller than one to be useful for barycentric coordinates
			if(coord1 + coord2 > 1) {
				coord1 = 1.0 - coord1;
				coord2 = 1.0 - coord2;
			}

			pStrandCoordinatesFloat[writePosition++] = coord1;
			pStrandCoordinatesFloat[writePosition++] = coord2;
			pStrandCoordinatesFloat[writePosition++] = 1.0f - coord1 - coord2;
		}

		// unlock buffer
		strandCoordinatesBuffer->unlock();

		if(singleStrandHair) {
			// curly hair
			createCurlDeviations(m_NumberOverallHairGuideVertices, m_NumberOfGuideHairs);

			// deviant hair strand
			createStrandLengths();
			createDeviantStrands((m_NumberMaxOfHairSegments+1)*64);
		}
		else {
			// curly hair
			createCurlDeviations(hairIndexCount, hairGrowthMeshIndexCount);

			// deviant hair strand
			createStrandLengths();
			createDeviantStrands(m_NumberMaxOfHairSegments*64);
		}

		createRandomCircularCoordinates();
		createCoordinateFrames(hairStrands);
		createMaxDistanceAngleTexture(meshShapeSubMesh, hairStrands);
		createHairStrandBendTexture(hairStrands);

		// Create custom mesh with empty vertex buffer for hair rendering
		//-----------------------------------------------------------------
		m_hairMeshName = createUniqueName("CustomHairMesh");
		if( !m_hairMesh.isNull() )
			Ogre::MeshManager::getSingleton().remove( m_hairMeshName );

		m_hairMesh = Ogre::MeshManager::getSingleton().createManual(m_hairMeshName, "General");
		Ogre::SubMesh *subMesh = m_hairMesh->createSubMesh();

		// create the vertex data structure
		// m_hairMesh->sharedVertexData = NULL;				// test if it works
		m_hairMesh->sharedVertexData = new Ogre::VertexData;
		m_hairMesh->sharedVertexData->vertexCount = 0;

		// declare how the vertices will be represented
		Ogre::VertexDeclaration *decl = m_hairMesh->sharedVertexData->vertexDeclaration;
		size_t offset = 0;

		// create the vertex buffer
		Ogre::HardwareVertexBufferSharedPtr vertexBuffer = 
			Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			offset, 
			m_hairMesh->sharedVertexData->vertexCount, 
			Ogre::HardwareBuffer::HBU_STATIC);

		size_t indexCountBuffer = 0;

		if( singleStrandHair ) {
			//indexCountBuffer = hairMeshIndexCount * 3;
			indexCountBuffer = m_NumberOverallHairGuideVertices * 3;
		}
		else {
			//indexCountBuffer = hairIndexCount * 3 * (m_NumberMaxOfHairSegments+1);
			indexCountBuffer = hairIndexCount *3;
		}

		// create the index buffer
		Ogre::HardwareIndexBufferSharedPtr indexBuffer = 
			Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
			Ogre::HardwareIndexBuffer::IT_32BIT,
			indexCountBuffer,							// index count
			Ogre::HardwareBuffer::HBU_STATIC);

		// attach the buffers to the mesh
		m_hairMesh->sharedVertexData->vertexBufferBinding->setBinding(0, vertexBuffer);
		subMesh->useSharedVertices = true;
		subMesh->indexData->indexBuffer = indexBuffer;
		subMesh->indexData->indexCount = indexCountBuffer;
		subMesh->indexData->indexStart = 0;

		/* set the bounds of the mesh */
		m_hairMesh->_setBounds(Ogre::AxisAlignedBox(-1, -1, -1, 1, 1, 1));

		/* notify the mesh that we're all ready */
		m_hairMesh->load();

		// Create an entity/scene node based on hair mesh, e.g.
		Ogre::String customHairEntityName = createUniqueName("CustomHairEntity");
		sceneManager->destroyEntity(customHairEntityName);

		m_hairEntity = sceneManager->createEntity(customHairEntityName, m_hairMeshName, "General");
		
		Ogre::MaterialPtr matPtr;
		
		if( singleStrandHair )
		{
			matPtr = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("SingleStrandHair"));
		}
		else
		{

			matPtr = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("MultiStrandHair"));			
		}

		//m_grundMat = matPtr->clone(createUniqueName("Material"));	// Save material

		if(m_grundMat.isNull())
			m_grundMat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().create(createUniqueName(matPtr->getName().c_str()), "General"));

		matPtr->copyDetailsTo(m_grundMat);			//Save material
		m_hairEntity->setMaterial(m_grundMat);		// Set material

		m_boneFound = false;
		m_boneNameToAssignToHair = getStringValue("Assigned Bone Name").toStdString();

		Ogre::SkeletonInstance* characterSkeleton = m_entity->getSkeleton();
		if(characterSkeleton) {
			if( characterSkeleton->hasBone( m_boneNameToAssignToHair ) ) {
				// attach new generated hair to head bone
				// hair style will rotate and translate with head position
				Ogre::Vector3 hairInitialPosition = getVectorValue("Hair Style Initial Position");
				Ogre::TagPoint* tagPoint = m_entity->attachObjectToBone(m_boneNameToAssignToHair, m_hairEntity, Ogre::Quaternion::IDENTITY, hairInitialPosition);

				m_boneFound = true;
			}
		}

		if(!m_boneFound) {
			// attach hair without bone assignment
			m_sceneNode->attachObject(m_hairEntity);
		}

		//m_sceneNode->attachObject(m_hairEntity);

		// Save material
		//m_grundMat = m_hairEntity->getSubEntity(0)->getMaterial();

#ifdef WRITE_PREPARE_HAIR_LOG
		Ogre::String matName = m_grundMat->getName();
		std::cout << "Grundmesh Material= " << matName << std::endl;
#endif

		//-----------------------------------------------------------------


		// Set shader parameters
		//setTexture(m_grundMat, m_hairCoordTex, 0, Ogre::TextureUnitState::BT_TESSELLATION_HULL);
		// Texture with TextureUnitState BT_TESSELLATION_HULL need to be set with unit stat BT_TESSELLATION_DOMAIN
		// even if they are not used in the Domain/Tessellation Evaluation Shader
		// Textures for single and multi strand interpolation
		setTexture(m_grundMat, m_hairCoordTex,					0, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_hairTangentsTex,				1, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_hairCurlDeviationTex,			2, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_hairDeviationsTex,				3, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_strandLengthTex,				4, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_randomCircularCoordinatesTex,	5, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_coordinateFramesTex,			6, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);

		// Texture only for multi strand interpolation
		setTexture(m_grundMat, m_hairIndexTex,			7, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_hairIndexEndTex,		8, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_strandCoordinatesTex,	9, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_maxDistanceAngleTex,	10, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_scalpNormalIndicesTex,	11, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_scalpNormalsTex,		12, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);
		setTexture(m_grundMat, m_strandBendTex,			13, Ogre::TextureUnitState::BT_TESSELLATION_DOMAIN);


		// create filename
		//int lastDotIndex = filename.lastIndexOf('.');
		//QString meshFilename = filename.mid(0, lastDotIndex);
		//QString textureFilenamePrefix = path + "/" + meshFilename;

		//textureFilename = textureFilenamePrefix + "_HairCoordTex.png";
		//saveTextureToFile(m_hairCoordTex, textureFilename.toStdString());

		//Ogre::GpuProgramParametersSharedPtr gpuHullParam = m_grundMat->getTechnique(0)->getPass(0)->getTesselationHullProgramParameters();

		if(m_grundMat.isNull())
			return false;

		Ogre::Technique* technique = m_grundMat->getTechnique(0);
		Ogre::Pass* pass = technique->getPass(0);
		Ogre::GpuProgramParametersSharedPtr gpuHullParam = pass->getTessellationHullProgramParameters();
		Ogre::GpuProgramParametersSharedPtr gpuDomainParam = pass->getTessellationDomainProgramParameters();
		Ogre::GpuProgramParametersSharedPtr gpuGeoParam = pass->getGeometryProgramParameters();

		//if (gpuHullParam->_findNamedConstantDefinition("g_detailFactor"))
		//	gpuHullParam->setNamedConstant("g_detailFactor", (float) m_NumberMaxOfHairSegments);
		if (gpuHullParam->_findNamedConstantDefinition("g_textureSize"))
			gpuHullParam->setNamedConstant("g_textureSize", (float) texturesizeXY);
		if (gpuHullParam->_findNamedConstantDefinition("g_textureSizeIndices"))
			gpuHullParam->setNamedConstant("g_textureSizeIndices", (float) indexTexturesizeXY);
		if (gpuHullParam->_findNamedConstantDefinition("g_coordinateFramesTextureSize"))
			gpuHullParam->setNamedConstant("g_coordinateFramesTextureSize", m_coordinateFramesTextureSizeXY );
		if (gpuHullParam->_findNamedConstantDefinition("g_maxDistanceAngleTextureSize"))
			gpuHullParam->setNamedConstant("g_maxDistanceAngleTextureSize", m_maxDistanceAngleTextureSizeXY );
		if (gpuHullParam->_findNamedConstantDefinition("g_scalpNormalsTextureSize"))
			gpuHullParam->setNamedConstant("g_scalpNormalsTextureSize", m_scalpNormalsTextureSizeXY );

		// set domain shader variables
		if (gpuDomainParam->_findNamedConstantDefinition("g_textureSize"))
			gpuDomainParam->setNamedConstant("g_textureSize", (float) texturesizeXY);

		//if (gpuDomainParam->_findNamedConstantDefinition("g_numberOfGuideStrands"))
		//	gpuDomainParam->setNamedConstant("g_numberOfGuideStrands", (float) m_NumberOfGuideHairs);

		if (gpuDomainParam->_findNamedConstantDefinition("g_totalStrandVertices"))
			gpuDomainParam->setNamedConstant("g_totalStrandVertices", (float) ( m_NumberMaxOfHairSegments+1 ) );

		if (gpuDomainParam->_findNamedConstantDefinition("g_curlTextureSize"))
			gpuDomainParam->setNamedConstant("g_curlTextureSize", m_curlTextureSizeXY );

		if (gpuDomainParam->_findNamedConstantDefinition("g_deviationsTextureSize"))
			gpuDomainParam->setNamedConstant("g_deviationsTextureSize", m_deviationsTextureSizeXY );

		if (gpuDomainParam->_findNamedConstantDefinition("g_numStrandVariables"))
			gpuDomainParam->setNamedConstant("g_numStrandVariables", m_numStrandVariables );

		if (gpuDomainParam->_findNamedConstantDefinition("g_thinning"))
			gpuDomainParam->setNamedConstant("g_thinning", m_thinning );

		if (gpuDomainParam->_findNamedConstantDefinition("g_maxDistanceAngleTextureSize"))
			gpuDomainParam->setNamedConstant("g_maxDistanceAngleTextureSize", m_maxDistanceAngleTextureSizeXY );

		// set geometry shader variables
		if (gpuGeoParam->_findNamedConstantDefinition("g_numStrandVariables"))
			gpuGeoParam->setNamedConstant("g_numStrandVariables", m_numStrandVariables );


		// END OF HAIR SPECIFIC STUFF !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


		bool SaC = getBoolValue("useSaC");
		const int updatedNumOfSubentities = m_entity->getNumSubEntities();

		for (int i = 0; i < updatedNumOfSubentities; ++i) {
			// create a new number parameter for the bone
			Ogre::SubEntity *subEntity = m_entity->getSubEntity(i);
			subEntity->setCustomParameter(0, Ogre::Vector4(0.0, 0.0, 0.0, 0.0));

			const Ogre::String subMeshMaterialName = subEntity->getMaterialName();
			std::string::size_type loc = subMeshMaterialName.find("Face");
			if( loc != std::string::npos && SaC ) {
				const Ogre::Mesh::SubMeshNameMap &subMeshNameMap = m_entity->getMesh()->getSubMeshNameMap();
				writeDataToVertices(m_entity->getSubEntity(i)->getSubMesh());
			}
		}

		// set cumulative blend mode instead of Ogre::ANIMBLEND_AVERAGE which is default
		if (m_entity->hasSkeleton()) {
			Ogre::Skeleton *skeleton = m_entity->getSkeleton();
			skeleton->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

			if( getBoolValue("Clean empty Skeleton Animation tracks on load")) {
				skeleton->optimiseAllAnimations(false);
				CleanEmptyTracks(skeleton);
			}
		}

		m_sceneNode->attachObject(m_entity);

		// create a container for the entity
		m_entityContainer = new OgreContainer(m_entity);
		m_entity->setUserAny(Ogre::Any(m_entityContainer));
	}

	updateStatistics();
	Log::info(QString("Mesh file \"%1\" loaded.").arg(filename), "GeometryAnimationNode::loadMesh");
	return true;
}


///
/// Protected Functions
///

//!
//! Assign texture to texture slot.
//!
//! \param material The material.
//! \param texture Pointer to the texture.
//! \param slot The texture slot id.
//! \param bindingType Defines to which shader the texture unit is bind
//!
void AnimatableMeshHairNode::setTexture( Ogre::MaterialPtr &material, Ogre::TexturePtr texture, unsigned int slot, Ogre::TextureUnitState::BindingType bindingType )
{
    if (!material.isNull() && !texture.isNull()) {
        Ogre::Technique *technique = material->getTechnique(0);
        if (!technique)
            return;
        Ogre::Pass *pass = technique->getPass(0);
        if (!pass)
            return;
        unsigned short numPasses = pass->getNumTextureUnitStates();
        if (slot >= numPasses)
            return;
		Ogre::TextureUnitState *textureUnitState = pass->getTextureUnitState(slot);
		textureUnitState->setBindingType(bindingType);
		textureUnitState->setTexture(texture);
		//textureUnitState->setTextureName(texture->getName());

        //material->getTechnique(0)->getPass(0)->getTextureUnitState(slot)->setTextureName(texture->getName());
    }
}


//!
//! Saves texture to a file
//!
//! \param texture The texture pointer.
//! \param FileName Name of the created file
//! \param pixelFormat Pixel format of the texture
//!
void AnimatableMeshHairNode::saveTextureToFile(Ogre::TexturePtr texture, const Ogre::String& fileName, Ogre::PixelFormat pixelFormat /*= Ogre::PF_FLOAT32_RGBA*/)
{
	//// Declare buffer
	//const size_t buffSize = (texture->getWidth() * texture->getHeight() * 4);
	//unsigned char *data = OGRE_ALLOC_T(unsigned char,buffSize,Ogre::MEMCATEGORY_GENERAL);

	//// Clear buffer
	//memset(data, 0, buffSize);

	//// Setup Image with correct settings
	//Ogre::Image image;
	//image.loadDynamicImage(data, texture->getWidth(), texture->getHeight(), 1, texture->getFormat(), true);

	//// Copy Texture buffer contents to image buffer
	//Ogre::HardwarePixelBufferSharedPtr buffer = texture->getBuffer();      
	//const Ogre::PixelBox destBox = image.getPixelBox();
	//buffer->blitToMemory(destBox);

	//// Save to disk!
	//image.save(fileName);

	Ogre::HardwarePixelBufferSharedPtr readbuffer;
	readbuffer = texture->getBuffer(0, 0);
	readbuffer->lock( Ogre::HardwareBuffer::HBL_NORMAL );
	const Ogre::PixelBox &readrefpb = readbuffer->getCurrentLock();    
	uchar *readrefdata = static_cast<uchar*>(readrefpb.data);        
 
	Ogre::Image img;
	img = img.loadDynamicImage (readrefdata, texture->getWidth(),
	texture->getHeight(), texture->getFormat());    
	img.save(fileName);
 
	readbuffer->unlock();

}


//!
//! Box Muller Transformation
//!
//! \param var1 Result 1 of box muller transformation
//! \param var2 Result 2 of box muller transformation
//!
void AnimatableMeshHairNode::BoxMullerTransform(float& var1, float& var2)
{
	float unifVar1 = Ogre::Math::RangeRandom(0.0, 1.0);
	float unifVar2 = Ogre::Math::RangeRandom(0.0, 1.0);
	float temp = sqrt(-2*log(unifVar1));
	var1 = temp*cos(2*Ogre::Math::PI*unifVar2);
	var2 = temp*sin(2*Ogre::Math::PI*unifVar2);
}


//!
//! Procedural creation of curly hair deviations
//!
//! \param totalHairGuideVertexNumber Vertex number of all hair guides combined
//! \param numberOfGuideHairs Number of used guide hairs
//!
void AnimatableMeshHairNode::createCurlDeviations(int totalHairGuideVertexNumber, int numberOfGuideHairs)
{
	// Texture for curl diviation
	//--------------------------------------
	size_t curlDeviationsCount = totalHairGuideVertexNumber*64;
	int curlTexturesizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)curlDeviationsCount)));
	m_curlTextureSizeXY = (float)curlTexturesizeXY;

	Ogre::String hairCurlDiviationName = createUniqueName("HairCurlDiviations_AnimatableMeshHairNode");
	// Create ogre texture manual for hair strands index data
	m_hairCurlDeviationTex = Ogre::TextureManager::getSingleton().createManual(
			hairCurlDiviationName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			curlTexturesizeXY,
			curlTexturesizeXY,
			0,
			Ogre::PF_FLOAT32_RGB,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr hairCurlDiviationsIndexPixelBuffer = m_hairCurlDeviationTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	hairCurlDiviationsIndexPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBoxHairCurlDiviationsIndex = hairCurlDiviationsIndexPixelBuffer->getCurrentLock();
	Ogre::Real* pFloatCurlDiviations = 0;
	pFloatCurlDiviations = static_cast<Ogre::Real*>(pixelBoxHairCurlDiviationsIndex.data);
	int writeCurlDiviationPos = 0;
	//--------------------------------------

	// Create curl deviations for curly hair
	int numCVs = 13;		// 13
	float sd = 1.5;			// 1.5
	//if(g_useShortHair)
	//{
	//	numCVs = 10;
	//	sd = 1.2f;
	//}
	int numSegments = numCVs - 3;
	int numVerticesPerSegment = ceil( ((m_NumberMaxOfHairSegments+1)*64) /float(numSegments) );
	float uStep = 1.0/numVerticesPerSegment;
	Ogre::Vector2* CVs = new Ogre::Vector2[numCVs];
	CVs[0] = Ogre::Vector2(0,0);
	CVs[1] = Ogre::Vector2(0,0);
	CVs[2] = Ogre::Vector2(0,0);
	CVs[3] = Ogre::Vector2(0,0);
	float x,y;
	int index = 0;
	int lastIndex = 0;

	Ogre::Matrix4 basisMatrix = Ogre::Matrix4
	(
	-1/6.0f,	3/6.0f,		-3/6.0f,	1/6.0f,
	3/6.0f,		-6/6.0f,	0,			4/6.0f,
	-3/6.0f,	3/6.0f,		3/6.0f,		1/6.0f,
	1/6.0f,		0,			0,			0
	);

	//Ogre::Vector2* curlDeviations = new Ogre::Vector2[curlDeviationsCount];
	for (int i = 0; i < numberOfGuideHairs; i++) 
	{
		//create the random CVs
		for(int j=4;j<numCVs;j++)
		{
			BoxMullerTransform(x,y);
			x *= sd;
			y *= sd; 
			CVs[j] = Ogre::Vector2(x,y);
		}

		//create the points
		for(int s=0;s<numSegments;s++)
		{
			for(float u=0;u<1.0;u+=uStep)
			{
				Ogre::Vector4 basis;
				
				//vectorMatrixMultiply(&basis,basisMatrix,float4(u * u * u, u * u, u, 1));
				basis = basisMatrix * Ogre::Vector4(u*u*u, u*u, u, 1);
				Ogre::Vector2 position = Ogre::Vector2(0,0);
				for (int c = 0; c < 4; ++c) 
					position += basis[c] * CVs[s+c];
				if( index-lastIndex < (m_NumberMaxOfHairSegments+1)*64 )
				{
					//curlDeviations[index++] = position;
					// write to texture
					pFloatCurlDiviations[writeCurlDiviationPos++] = position.x;
					pFloatCurlDiviations[writeCurlDiviationPos++] = position.y;
					pFloatCurlDiviations[writeCurlDiviationPos++] = 0.0f;
					index++;
				}
			}
		}

		lastIndex = index;
	}

	hairCurlDiviationsIndexPixelBuffer->unlock();
}


//!
//! Procedural creation of hair strand deviations
//!
//! \param tessellatedMasterStrandLengthMax Maximum length of tessellated guide strand
//!
void AnimatableMeshHairNode::createDeviantStrands(size_t tessellatedMasterStrandLengthMax)
{
	//size_t tessellatedMasterStrandLengthMax = (m_NumberMaxOfHairSegments+1)*64;

	// Texture for diviations
	//--------------------------------------
	//size_t deviationsCount = totalHairGuideVertexNumber*64;		// number of vertices * maximumNumber of interpolated hair strands * maximum tessellation of a single segment
	size_t deviationsCount = m_numStrandVariables*tessellatedMasterStrandLengthMax;		// size of interpolated hair variables * maximum hair segment size
	int deviationsTexturesizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)deviationsCount)));
	m_deviationsTextureSizeXY = (float)deviationsTexturesizeXY;

	Ogre::String hairDiviationName = createUniqueName("HairDiviations_AnimatableMeshHairNode");
	
	if(!m_hairDeviationsTex.isNull() && Ogre::TextureManager::getSingleton().resourceExists(m_hairDeviationsTex->getName()))
		Ogre::TextureManager::getSingleton().remove(m_hairDeviationsTex->getName());
	
	// Create ogre texture manual for hair strands index data
	m_hairDeviationsTex = Ogre::TextureManager::getSingleton().createManual(
			hairDiviationName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			deviationsTexturesizeXY,
			deviationsTexturesizeXY,
			0,
			Ogre::PF_FLOAT32_RGB,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr hairDiviationsIndexPixelBuffer = m_hairDeviationsTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	hairDiviationsIndexPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBoxHairDiviationsIndex = hairDiviationsIndexPixelBuffer->getCurrentLock();
	Ogre::Real* pFloatDiviations = 0;
	pFloatDiviations = static_cast<Ogre::Real*>(pixelBoxHairDiviationsIndex.data);
	int writeDiviationPos = 0;
	//--------------------------------------

	//deviant strands-------------------------------------------------------------------------
	//using bsplines:
	int numCVs = 7;
	int numSegments = numCVs - 3;
	int numVerticesPerSegment = ceil( tessellatedMasterStrandLengthMax / float(numSegments) );
	float uStep = 1.0/numVerticesPerSegment;
	Ogre::Vector2* CVs = new Ogre::Vector2[numCVs];
	CVs[0] = Ogre::Vector2(0,0);
	CVs[1] = Ogre::Vector2(0,0);
	float x,y;
	int index = 0;
	int lastIndex = 0;

	Ogre::Matrix4 basisMatrix = Ogre::Matrix4
	(
		-1/6.0f,  3/6.0f, -3/6.0f,  1/6.0f,
		3/6.0f, -6/6.0f,      0,  4/6.0f,
		-3/6.0f,  3/6.0f,  3/6.0f,  1/6.0f,
		1/6.0f,      0,       0,      0
	);

	float sdScale = 1.0f;
	//if(g_useShortHair) sdScale = 0.6f;

	for (int i = 0; i < m_numStrandVariables; i++) 
	{
		float randomChoice = Ogre::Math::RangeRandom(0.0, 1.0);
		if(randomChoice>0.6)
		{
			float maxLength = (1.0-m_thinning) + m_thinning*m_strandLength[i%m_numStrandVariables];//between 0 and 1
			//float maxLength = 1.0;
			//float maxLength = Ogre::Math::RangeRandom(0.0, 1.0);
			int maxCV = floor(maxLength*numSegments)+ 2;//can do 1 here with sd=1.2;

			//create the random CVs
			for(int j=2;j<numCVs;j++)
			{
				float sd;

				if(randomChoice > 0.95)//make some very stray hair
					sd = 1.2f;
				else if(randomChoice > 0.8)
					sd = 0.8f;
				else //make some lesser stray hair that are more deviant near the ends
				{
					if(maxLength>((numCVs-1)/numCVs) && j==numCVs-1)    
						sd = 100.0f;
					else if(j>=maxCV)
						sd = 4.0f;
					else 
						sd = 0.12f;
				}			 

				BoxMullerTransform(x,y);
				x *= sd * sdScale;
				y *= sd * sdScale; 
				CVs[j] = Ogre::Vector2(x,y);
			}

			//create the points
			for(int s=0;s<numSegments;s++)
			{
				for(float u=0;u<1.0;u+=uStep)
				{  
					Ogre::Vector4 basis;
					//vectorMatrixMultiply(&basis,basisMatrix, Ogre::Vector4(u * u * u, u * u, u, 1));
					basis = basisMatrix * Ogre::Vector4(u * u * u, u * u, u, 1);
					Ogre::Vector2 position = Ogre::Vector2(0,0);
					for (int c = 0; c < 4; ++c) 
						position += basis[c] * CVs[s+c];
					if( index-lastIndex < (m_NumberMaxOfHairSegments+1)*64 )
					{
						// write to texture
						pFloatDiviations[writeDiviationPos++] = position.x;
						pFloatDiviations[writeDiviationPos++] = position.y;
						pFloatDiviations[writeDiviationPos++] = 0.0f;
						index++;
					}
				}
			}
		}
		else
		{
			// no deviations
			for(unsigned int j=0;j<tessellatedMasterStrandLengthMax;j++)
			{
				// write to texture
				pFloatDiviations[writeDiviationPos++] = 0.0f;
				pFloatDiviations[writeDiviationPos++] = 0.0f;
				pFloatDiviations[writeDiviationPos++] = 0.0f;
				index++;
			}
		}

		lastIndex = index;
	}

	hairDiviationsIndexPixelBuffer->unlock();
}


//!
//! Strand length calculation for hair thinning
//!
void AnimatableMeshHairNode::createStrandLengths()
{
	// create texture
	//--------------------------------------
	int texturesizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)m_numStrandVariables)));

	Ogre::String textureName = createUniqueName("HairStrandLengths_AnimatableMeshHairNode");
	// Create ogre texture manual for hair strands index data
	m_strandLengthTex = Ogre::TextureManager::getSingleton().createManual(
			textureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_1D,
			m_numStrandVariables,
			1,
			0,
			Ogre::PF_FLOAT32_R,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_strandLengthTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
	Ogre::Real* pFloat = 0;
	pFloat = static_cast<Ogre::Real*>(pixelBox.data);
	int writePos = 0;
	//--------------------------------------
	
	
	// calculate data and write to texture and array
	m_strandLength = new float[m_numStrandVariables];

	float minLength = 0.2f;

	for (int i = 0; i < m_numStrandVariables; ++i)
	{
		m_strandLength[i] = Ogre::Math::RangeRandom(0.0, 1.0) * (1-minLength) + minLength;
		pFloat[writePos++] = m_strandLength[i];
	}

	pixelBuffer->unlock();
}


//!
//! Random circular coordinates for use with single strand rendering of interpolated hair strands
//!
void AnimatableMeshHairNode::createRandomCircularCoordinates()
{
	// create texture
	//--------------------------------------
	Ogre::String textureName = createUniqueName("HairRandomCircularCoordinates_AnimatableMeshHairNode");

	if(!m_randomCircularCoordinatesTex.isNull() && Ogre::TextureManager::getSingleton().resourceExists(m_randomCircularCoordinatesTex->getName()))
		Ogre::TextureManager::getSingleton().remove(m_randomCircularCoordinatesTex->getName());

	// Create ogre texture manual for hair strands index data
	m_randomCircularCoordinatesTex = Ogre::TextureManager::getSingleton().createManual(
			textureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_1D,
			m_numStrandVariables,
			1,
			0,
			Ogre::PF_FLOAT32_RGB,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_randomCircularCoordinatesTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
	Ogre::Real* pFloat = 0;
	pFloat = static_cast<Ogre::Real*>(pixelBox.data);
	int writePos = 0;
	//--------------------------------------


	//if(g_clumpCoordinates)
	//	delete[] g_clumpCoordinates;
	//g_clumpCoordinates = new D3DXVECTOR2[g_NumStrandVariables];
	//create two coordinates that lie inside the unit circle
	int c = 0;
	float coord1 = 0;
	float coord2 = 0;

	pFloat[writePos++] = coord1;
	pFloat[writePos++] = coord2;
	pFloat[writePos++] = 0.0f;
	c++;

	while(c<m_numStrandVariables)
	{
		//distributed with a gaussian distribution
		BoxMullerTransform(coord1,coord2);
		float sd = 0.25;
		coord1 *= sd;
		coord2 *= sd;


		if( sqrt( (coord1*coord1) + (coord2*coord2)) < 1 )
		{
			pFloat[writePos++] = coord1;
			pFloat[writePos++] = coord2;
			pFloat[writePos++] = 0.0f;
			c++;
		}
	}

	pixelBuffer->unlock();
	//g_StrandCircularCoordinates
}


//!
//! Creation of coordinate frames per guide hair vertex. One coordinate frame consists of 3 vectors
//!
//! \param hairStrands Vector of guide hair strands
//!
void AnimatableMeshHairNode::createCoordinateFrames(std::vector<std::vector<Ogre::Vector3>> hairStrands)
{
	size_t coordinateFramesCount = m_NumberOverallHairGuideVertices * 3 * 3;	//3 vec3s per coordinate frame    -- CVs = control vertices

	float* coordinateFrames = new float[coordinateFramesCount]; //3 vec3s per coordinate frame    -- CVs = control vertices
	int index = 0;
	int CFNum = 0;
	for(int i=0; i < coordinateFramesCount; i++)
	{
		coordinateFrames[i] = 0;
	}

	for(int s=0; s < int(hairStrands.size()); s++)
	{
		for(int i=0;i<int(hairStrands.at(s).size())-1;i++)
		{
			Ogre::Vector3 x(hairStrands.at(s).at(i+1).x - hairStrands.at(s).at(i).x,
							hairStrands.at(s).at(i+1).y - hairStrands.at(s).at(i).y,
							hairStrands.at(s).at(i+1).z - hairStrands.at(s).at(i).z);
			x.normalise();
				
			Ogre::Vector3 y;
			Ogre::Vector3 z;

			if(i==0)
			{	
				// TODO: calculate root tangent
				//note: we could also load here the tangent vector of the root on the scalp rather than a random vector
				Ogre::Vector3 randVec( Ogre::Math::RangeRandom(0.0, 1.0), Ogre::Math::RangeRandom(0.0, 1.0), Ogre::Math::RangeRandom(0.0, 1.0) );
				randVec.normalise();
				z = x.crossProduct(randVec);
				z.normalise();
				y = x.crossProduct(z);
				y.normalise();
			}
			else
			{

				Ogre::Vector3 xRotationVector;
				Ogre::Vector3 prevX(coordinateFrames[(CFNum-1)*9 + 0],
									coordinateFrames[(CFNum-1)*9 + 1],
									coordinateFrames[(CFNum-1)*9 + 2] );
				Ogre::Vector3 prevY(coordinateFrames[(CFNum-1)*9 + 3],
									coordinateFrames[(CFNum-1)*9 + 4],
									coordinateFrames[(CFNum-1)*9 + 5] );
				Ogre::Vector3 prevZ(coordinateFrames[(CFNum-1)*9 + 6],
									coordinateFrames[(CFNum-1)*9 + 7],
									coordinateFrames[(CFNum-1)*9 + 8] );


				xRotationVector = x.crossProduct(prevX);
				float sinTheta = xRotationVector.length();
				float theta = asin(sinTheta);
				xRotationVector.normalise();

				rotateVector(xRotationVector, theta, prevY, y);
				y.normalise();
				rotateVector(xRotationVector, theta, prevZ, z);
				z.normalise();

				//do a gram-Schmidt orthogonalization on these vectors, just to be sure
	//#if 0
	//			Ogre::Vector3 tempX = x;
	//			Ogre::Vector3 tempY = y;
	//			Ogre::Vector3 tempZ = z;
	//			GramSchmidtOrthoNormalize( tempX,tempY,tempZ );

	//			if( vecLength(x-tempX) > SMALL_NUMBER)
	//				numTimes++;
	//			if( vecLength(y-tempY) > SMALL_NUMBER)
	//				numTimes++;
	//			if( vecLength(z-tempZ) > SMALL_NUMBER)
	//				numTimes++;
	//#endif				
			

			}

			coordinateFrames[CFNum*9+0] = x.x;
			coordinateFrames[CFNum*9+1] = x.y;
			coordinateFrames[CFNum*9+2] = x.z;
			coordinateFrames[CFNum*9+3] = y.x;
			coordinateFrames[CFNum*9+4] = y.y;
			coordinateFrames[CFNum*9+5] = y.z;
			coordinateFrames[CFNum*9+6] = z.x;
			coordinateFrames[CFNum*9+7] = z.y;
			coordinateFrames[CFNum*9+8] = z.z;
			CFNum++;
		}
	
		//coordinate axis for last vertex in strand are undefined; setting them to zero
		for(int k=0;k<9;k++)
			coordinateFrames[CFNum*9+k] = coordinateFrames[(CFNum-1)*9+k];

		CFNum++;
	}

	// create texture
	//--------------------------------------
	size_t coordinateFrameTextureCount = m_NumberOverallHairGuideVertices * 3;
	int textureSizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)coordinateFrameTextureCount)));
	m_coordinateFramesTextureSizeXY = (float)textureSizeXY;

	Ogre::String textureName = createUniqueName("HairCoordinateFrames_AnimatableMeshHairNode");

	if(!m_coordinateFramesTex.isNull() && Ogre::TextureManager::getSingleton().resourceExists(m_coordinateFramesTex->getName()))
		Ogre::TextureManager::getSingleton().remove(m_coordinateFramesTex->getName());

	// Create ogre texture manual for hair strands index data
	m_coordinateFramesTex = Ogre::TextureManager::getSingleton().createManual(
			textureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			textureSizeXY,
			textureSizeXY,
			0,
			Ogre::PF_FLOAT32_RGB,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_coordinateFramesTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
	Ogre::Real* pFloat = 0;
	pFloat = static_cast<Ogre::Real*>(pixelBox.data);
	int writePos = 0;
	//--------------------------------------

	// write data to texture
	for(int i=0; i < coordinateFramesCount; i++)
	{
		pFloat[writePos++] = coordinateFrames[i];
	}

	pixelBuffer->unlock();

	if( coordinateFrames )
	{
		delete [] coordinateFrames;
	}
}


//!
//! Rotate a vector
//!
//! note: this method is only valid when the axis is perpendicular to prevVec
//!
//! \param rotationAxis axis to rotate 
//! \param theta angle of rotation
//! \param prevVec input vector
//! \param newVec result vector
//!
void AnimatableMeshHairNode::rotateVector(const Ogre::Vector3& rotationAxis,float theta,const Ogre::Vector3& prevVec, Ogre::Vector3& newVec)
{
	Ogre::Vector3 axisDifference;
	axisDifference = rotationAxis - prevVec;

	if( axisDifference.length()<SMALL_NUMBER || theta < SMALL_NUMBER )
	{
		newVec = prevVec;
		return;
	}

	float c = cos(theta);
	float s = sin(theta);
	float t = 1 - c;
	float x = rotationAxis.x;
	float y = rotationAxis.y;
	float z = rotationAxis.z;

	Ogre::Matrix4 rotationMatrix(
		t*x*x + c,		t*x*y - s*z,	t*x*z + s*y, 0,
		t*x*y + s*z,	t*y*y + c,		t*y*z - s*x, 0,
		t*x*z - s*y,	t*y*z + s*x,	t*z*z + c  , 0,
		0          ,	0          ,	0          , 1);

	newVec = rotationMatrix * prevVec;
}


//!
//! Creation of texture which hold the maximum/angle between hair strands of one face.
//!
//! \param scalpSubMesh Pointer to scalp sub mesh
//! \param hairStrands
//!
void AnimatableMeshHairNode::createMaxDistanceAngleTexture(Ogre::SubMesh *scalpSubMesh, std::vector<std::vector<Ogre::Vector3>> hairStrands)
{
	// Get Index Buffer
	Ogre::HardwareIndexBufferSharedPtr scalpMeshIBuffer = scalpSubMesh->indexData->indexBuffer;
	
	size_t numTris = scalpSubMesh->indexData->indexCount / 3;

	// create texture
	//--------------------------------------
	int textureSizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)numTris)));
	m_maxDistanceAngleTextureSizeXY = (float)textureSizeXY;

	Ogre::String textureName = createUniqueName("MaximumDistanceAngle_AnimatableMeshHairNode");

	// Create ogre texture manual for hair strands index data
	m_maxDistanceAngleTex = Ogre::TextureManager::getSingleton().createManual(
			textureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			textureSizeXY,
			textureSizeXY,
			0,
			Ogre::PF_FLOAT32_RGBA,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_maxDistanceAngleTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
	Ogre::Real* pFloat = 0;
	pFloat = static_cast<Ogre::Real*>(pixelBox.data);
	int writePos = 0;
	//--------------------------------------	

	std::vector<Ogre::Vector3> hairStrand1;
	std::vector<Ogre::Vector3> hairStrand2;
	std::vector<Ogre::Vector3> hairStrand3;
	
	// lock index buffer
	unsigned int* pInt;
	unsigned short* pShort;
	bool use32bitindexes = (scalpMeshIBuffer->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
				
	if (use32bitindexes) 
		pInt = static_cast<unsigned int*>(scalpMeshIBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
	else
		pShort = static_cast<unsigned short*>(scalpMeshIBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

	for(size_t i=0; i<numTris ; i++)
	{
		if(use32bitindexes)
		{
			// get indices of current triangle
			unsigned int index1 = *pInt++;
			unsigned int index2 = *pInt++;
			unsigned int index3 = *pInt++;

			// get hair strands
			hairStrand1 = hairStrands.at(index1);
			hairStrand2 = hairStrands.at(index2);
			hairStrand3 = hairStrands.at(index3);
		}
		else
		{
			// get indices of current triangle
			unsigned short index1 = *pShort++;
			unsigned short index2 = *pShort++;
			unsigned short index3 = *pShort++;

			// get hair strands
			hairStrand1 = hairStrands.at(index1);
			hairStrand2 = hairStrands.at(index2);
			hairStrand3 = hairStrands.at(index3);
		}

		float maximumDistance = 0.0f;
		int maximumDistancePosition = 0;
		float maximumAngle = 0.0f;
		int maximumAnglePosition = 0;

		// TODO: check for different sizes and handle them
		for(int j=0; j<int(hairStrand1.size())-1; j++)
		{
			// distance
			float currentMaximumDistance = calculateMaximumDistance(hairStrand1.at(j), 
																	hairStrand2.at(j),
																	hairStrand3.at(j));

			if( currentMaximumDistance > maximumDistance )
			{
				maximumDistance = currentMaximumDistance;
				maximumDistancePosition = j;
			}

			// angle
			if( j < int(hairStrand1.size())-1 )
			{
				// ignore angle of last segment
				float currentMaximumAngle = calculateMaximumAngle(
					hairStrand1.at(j+1) - hairStrand1.at(j),
					hairStrand2.at(j+1) - hairStrand2.at(j),
					hairStrand3.at(j+1) - hairStrand3.at(j) );

				if( currentMaximumAngle > maximumAngle )
				{
					maximumAngle = currentMaximumAngle;
					maximumAnglePosition = j;
				}
			}
		}

		// write to texture
		pFloat[writePos++] = maximumDistance;
		pFloat[writePos++] = maximumDistancePosition;
		pFloat[writePos++] = maximumAngle;
		pFloat[writePos++] = maximumAnglePosition;
	}


	// unlock index buffer
	scalpMeshIBuffer->unlock();

	// unlock texture pixel buffer
	pixelBuffer->unlock();
}


//!
//! Calculate maximum distance between those vectors
//!
//! \param v1 Input vector 1
//! \param v2 Input vector 2
//! \param v3 Input vector 3
//!
//! \return Maximum distance between those vectors
//!
float AnimatableMeshHairNode::calculateMaximumDistance(Ogre::Vector3 v1, Ogre::Vector3 v2, Ogre::Vector3 v3)
{
	float distance12 = v1.distance(v2);
	float distance13 = v1.distance(v3);
	float distance23 = v2.distance(v3);

	float maximumDistance = distance12;
	if( distance13 > distance23 )
	{
		if( distance13 > maximumDistance )
		{
			maximumDistance = distance13;
		}
	}
	else
	{
		if( distance23 > maximumDistance )
		{
			maximumDistance = distance23;
		}
	}

	return maximumDistance;
}


//!
//! Calculate maximum angle between those vectors
//!
//! \param v1 Input vector 1
//! \param v2 Input vector 2
//! \param v3 Input vector 3
//!
//! \return Maximum angle between input vectors
//!
float AnimatableMeshHairNode::calculateMaximumAngle(Ogre::Vector3 tangent1, Ogre::Vector3 tangent2, Ogre::Vector3 tangent3)
{
	// check angle
	float angle12 = tangent1.angleBetween(tangent2).valueDegrees();
	float angle13 = tangent1.angleBetween(tangent3).valueDegrees();
	float angle23 = tangent2.angleBetween(tangent3).valueDegrees();

	float maximumAngle = angle12;
	if( angle13 > angle23 )
	{
		if( angle13 > maximumAngle )
		{
			maximumAngle = angle13;
		}
	}
	else
	{
		if( angle23 > maximumAngle )
		{
			maximumAngle = angle23;
		}
	}

	return maximumAngle; 
}


//!
//! Creation of texture, which holds scalp mesh normals
//!
//! \param normalList List of scalp mesh normals
//! \param indexNormalList Index list of scalp mesh normals
//!
void AnimatableMeshHairNode::createScalpMeshNormalTexture(std::list<Ogre::Vector3> normalList,std::list<Ogre::Vector3> indexNormalList)
{
	// create normal texture
	//--------------------------------------
	int textureSizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)normalList.size())));
	m_scalpNormalsTextureSizeXY = (float)textureSizeXY;

	Ogre::String textureName = createUniqueName("ScalpNormals_AnimatableMeshHairNode");

	// Create ogre texture manual for hair strands index data
	m_scalpNormalsTex = Ogre::TextureManager::getSingleton().createManual(
			textureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			textureSizeXY,
			textureSizeXY,
			0,
			Ogre::PF_FLOAT32_RGB,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_scalpNormalsTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
	Ogre::Real* pFloat = 0;
	pFloat = static_cast<Ogre::Real*>(pixelBox.data);
	int writePos = 0;
	//--------------------------------------

	// write index data
	for (std::list<Ogre::Vector3>::iterator iterData = normalList.begin(); iterData != normalList.end(); ++iterData)
	{
		pFloat[writePos++] = (*iterData).x;
		pFloat[writePos++] = (*iterData).y;
		pFloat[writePos++] = (*iterData).z;
	}

	pixelBuffer->unlock();

	// create normal index texture
	//--------------------------------------
	int indicesTextureSizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)indexNormalList.size())));
	m_scalpNormalIndicesTextureSizeXY = (float)indicesTextureSizeXY;

	Ogre::String indicesTextureName = createUniqueName("ScalpNormalIndices_AnimatableMeshHairNode");

	// Create ogre texture manual for hair strands index data
	m_scalpNormalIndicesTex = Ogre::TextureManager::getSingleton().createManual(
			indicesTextureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			indicesTextureSizeXY,
			indicesTextureSizeXY,
			0,
			Ogre::PF_FLOAT32_RGB,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr pixelBufferIndices = m_scalpNormalIndicesTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	pixelBufferIndices->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBoxIndices = pixelBufferIndices->getCurrentLock();
	Ogre::Real* pFloatIndices = 0;
	pFloatIndices = static_cast<Ogre::Real*>(pixelBoxIndices.data);
	int writePosIndices = 0;
	//--------------------------------------

	// write index data
	for (std::list<Ogre::Vector3>::iterator iterData = indexNormalList.begin(); iterData != indexNormalList.end(); ++iterData)
	{
		pFloatIndices[writePosIndices++] = (*iterData).x;
		pFloatIndices[writePosIndices++] = (*iterData).y;
		pFloatIndices[writePosIndices++] = (*iterData).z;
	}

	pixelBufferIndices->unlock();

}


//!
//! Creation of texture, which determines how the hair strand is bended. (Used to calculate detail tessellation factor at run-time)
//!
//! \param hairStrands Vector of guide hair strands
//!
void AnimatableMeshHairNode::createHairStrandBendTexture(std::vector<std::vector<Ogre::Vector3>> hairStrands)
{
	// create texture
	//--------------------------------------
	int textureSizeXY = Helpers::MathH::NextPow2(std::ceil(std::sqrt((float)m_NumberOverallHairGuideVertices)));
	//m_strandBendTextureSizeXY = (float)textureSizeXY;

	Ogre::String textureName = createUniqueName("StrandBendTexture_AnimatableMeshHairNode");

	// Create ogre texture manual for hair strands index data
	m_strandBendTex = Ogre::TextureManager::getSingleton().createManual(
			textureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			textureSizeXY,
			textureSizeXY,
			0,
			Ogre::PF_FLOAT32_R,
			Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	// Pixel Buffer of the texture for guide hair vertex tangets
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_strandBendTex->getBuffer(0,0);

	// Lock the pixel buffer in a way that we can write to it
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox &pixelBox = pixelBuffer->getCurrentLock();
	Ogre::Real* pFloat = 0;
	pFloat = static_cast<Ogre::Real*>(pixelBox.data);
	int writePos = 0;
	//--------------------------------------	


	for(int i=0; i<int(hairStrands.size()) ; i++)
	{
		for(int j=0; j<int(hairStrands.at(i).size())-1; j++)
		{
			float angle = 0;
			float tessellationScale = 0.0;

			if( j==0 )
			{
				// first segment
				Ogre::Vector3 vector0 = hairStrands.at(i).at(j);
				Ogre::Vector3 vector1 = hairStrands.at(i).at(j+1);
				Ogre::Vector3 vector2 = hairStrands.at(i).at(j+2);

				Ogre::Vector3 tangent01 = vector1 - vector0;
				tangent01.normalise();
				Ogre::Vector3 tangent12 = vector2 - vector1;
				tangent12.normalise();
				angle = tangent01.angleBetween(tangent12).valueRadians();
			}
			else if( j == int(hairStrands.at(i).size())-2 )
			{
				// last segment
				Ogre::Vector3 vector0 = hairStrands.at(i).at(j-1);
				Ogre::Vector3 vector1 = hairStrands.at(i).at(j);
				Ogre::Vector3 vector2 = hairStrands.at(i).at(j+1);

				Ogre::Vector3 tangent01 = vector1 - vector0;
				tangent01.normalise();
				Ogre::Vector3 tangent12 = vector2 - vector1;
				tangent12.normalise();
				angle = tangent01.angleBetween(tangent12).valueRadians();
			}
			else
			{
				// first angle
				Ogre::Vector3 vector0 = hairStrands.at(i).at(j-1);
				Ogre::Vector3 vector1 = hairStrands.at(i).at(j);
				Ogre::Vector3 vector2 = hairStrands.at(i).at(j+1);

				Ogre::Vector3 tangent01 = vector1 - vector0;
				tangent01.normalise();
				Ogre::Vector3 tangent12 = vector2 - vector1;
				tangent12.normalise();
				float angle0 = tangent01.angleBetween(tangent12).valueRadians();

				// second angle
				vector0 = hairStrands.at(i).at(j);
				vector1 = hairStrands.at(i).at(j+1);
				vector2 = hairStrands.at(i).at(j+2);

				tangent01 = vector1 - vector0;
				tangent01.normalise();
				tangent12 = vector2 - vector1;
				tangent12.normalise();
				float angle1 = tangent01.angleBetween(tangent12).valueRadians();

				if( angle0 > angle1 )
				{
					angle = angle0;
				}
				else
				{
					angle = angle1;
				}
			}

			// between than 45 degrees and 315 degrees
			if( angle > m_bendAngleMaxTessellation )
			{
				tessellationScale = 1.0f;
			}
			else if( angle < m_bendAngleMeaningful )
			{
				tessellationScale = 0.0f;
			}
			else
			{
				tessellationScale = (angle- m_bendAngleMeaningful) / (m_bendAngleMaxTessellation - m_bendAngleMeaningful);
			}

			// write to texture
			pFloat[writePos++] = tessellationScale;
		}

	}

	// unlock texture pixel buffer
	pixelBuffer->unlock();
}

} // namespace AnimatableMeshHairNode 
