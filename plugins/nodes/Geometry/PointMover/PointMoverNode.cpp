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
//! \file "PointMoverNode.h"
//! \brief Implementation file for PointMoverNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "PointMoverNode.h"
#include "OgreManager.h"
#include "OgreTools.h"

namespace PointMoverNode{
using namespace Frapper;

INIT_INSTANCE_COUNTER(PointMoverNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the PointMoverNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PointMoverNode::PointMoverNode ( const QString &name, ParameterGroup *parameterRoot ) :
	PointTextureMapNode(name,parameterRoot),
	m_pointSizeParameter(0),
	m_pointCountParameter(0)	{

	m_flatMaterialPtr = copyMaterial("MoveShaderMaterial");
	m_actualMaterialPtr = copyMaterial("PointSimpleMaterial");
	if(m_flatMaterialPtr.isNull()) Log::debug("mat is null");

	// add the input image
	Parameter *inputPosTexture = addInputRenderTexture("InPosition");

	// add the output image
	OutTextureTarget* outPositionTarget = addOutputRenderTargetImage(	this,
																createUniqueName("Position").c_str(),
																"OutPosition",
																512,
																Ogre::PF_R8G8B8A8,
																64,
																m_flatMaterialPtr,
																m_actualMaterialPtr		);

	// set inputs to affect output
	getParameter("OutPosition")->addAffectingParameter(inputPosTexture);

	// add an input (tus) to the output texture
	Ogre::TextureUnitState* tus = outPositionTarget->addInputTexture(inputPosTexture->getValue().value<Ogre::TexturePtr>(),m_flatMaterialPtr);

	// set the point count on the output to 1 (default)
	outPositionTarget->setPointCount(1);

	// add the tus to the list of tus's affected by the input
	addTusToParameter(inputPosTexture,tus);

#define DEBUG
#ifdef DEBUG
	//add the manual rtt update parameter
	Parameter *updateCommandParameter = Parameter::create("update",Frapper::Parameter::T_Command);
	parameterRoot->addParameter(updateCommandParameter);
	updateCommandParameter->setCommandFunction(SLOT(updateAllOutputs()));
	
#endif

	//add the pointScale parameter
	m_pointSizeParameter = new NumberParameter("pointSize",Frapper::Parameter::T_Int,2);
	m_pointSizeParameter->setNode(this);
	m_pointSizeParameter->setMaxValue(1000);
	m_pointSizeParameter->setMinValue(0);
	parameterRoot->addParameter(m_pointSizeParameter);
	m_pointSizeParameter->setChangeFunction(SLOT(setScaleParameter()));

	//add the pointCount parameter
	m_pointCountParameter = new NumberParameter("pointCount",Frapper::Parameter::T_UnsignedInt,1);
	m_pointCountParameter->setNode(this);
	m_pointCountParameter->setMaxValue(512*512);
	m_pointCountParameter->setMinValue(0);
	parameterRoot->addParameter(m_pointCountParameter);
	m_pointCountParameter->setChangeFunction(SLOT(setCountParameter()));

	//add the point move parameter
	m_pointMoveParameter = new NumberParameter("MoveVector",Frapper::Parameter::T_Float,0);
	m_pointMoveParameter->setSize(3);
	m_pointMoveParameter->setNode(this);
	parameterRoot->addParameter(m_pointMoveParameter);
	m_pointMoveParameter->setChangeFunction(SLOT(setMoveParameter()));

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the PointMoverNode class.
//!
PointMoverNode::~PointMoverNode ()
{

    emit viewNodeUpdated();

    DEC_INSTANCE_COUNTER
}

///
/// Public Slots
///

///
/// Protected Functions
///

///
/// Private Functions
///

///
/// Private Slots
///

//!
//! Sets the scenes shader parameter
//!
void PointMoverNode::setScaleParameter()
{
	Parameter* parameter = dynamic_cast<Parameter*>(sender());

	if (parameter) {
		const Ogre::String &name = parameter->getName().toStdString();
		const float value = parameter->getValue().toFloat();

		if (!m_actualMaterialPtr.isNull()){
			m_actualMaterialPtr->getTechnique(0)->getPass(0)->getGeometryProgramParameters()->setNamedConstant("radius",value);
		}

		triggerRedraw();
	}
}

//!
//! Sets the scenes shader parameter
//!
void PointMoverNode::setCountParameter()
{
	Parameter* parameter = dynamic_cast<Parameter*>(sender());

	if (parameter) {
		const Ogre::String &name = parameter->getName().toStdString();

		setPointCount(parameter->getValue().toInt());

		triggerRedraw();
	}
}

//!
//! Sets the scenes shader parameter
//!
void PointMoverNode::setMoveParameter()
{
	NumberParameter* parameter = dynamic_cast<NumberParameter*>(sender());

	if (parameter) {
		const Ogre::String &name = parameter->getName().toStdString();
		const Ogre::Vector3 value = parameter->getValue().value<Ogre::Vector3>();

		if (!m_flatMaterialPtr.isNull()){
			//m_flatMaterialPtr->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("move",value);
		}

		getTextureTarget(0)->update();

		triggerRedraw();
	}
}

} // end namespace
