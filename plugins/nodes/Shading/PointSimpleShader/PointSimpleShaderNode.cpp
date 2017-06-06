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
//! \file "PointSimpleShaderNode.h"
//! \brief Implementation file for PointSimpleShaderNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "PointSimpleShaderNode.h"
#include "OgreManager.h"
#include "OgreTools.h"

namespace PointSimpleShaderNode{
using namespace Frapper;

INIT_INSTANCE_COUNTER(PointSimpleShaderNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the PointSimpleShaderNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PointSimpleShaderNode::PointSimpleShaderNode ( const QString &name, ParameterGroup *parameterRoot ) :
	PointTextureMapNode(name,parameterRoot),
	m_pointSizeParameter(0)	{

	//m_flatMaterialPtr = copyMaterial("FlatViewShaderMaterial");
	m_actualMaterialPtr = copyMaterial("PointSimpleMaterial");

	// add the input image
	Parameter *inputPosTexture = addInputRenderTexture("InPosition");

	// add the tus to the material
	Ogre::TextureUnitState *tus = m_actualMaterialPtr->getTechnique(0)->getPass(0)->createTextureUnitState();
	tus->setTexture(inputPosTexture->getValue().value<Ogre::TexturePtr>());
	tus->setNumMipmaps(0);
	tus->setTextureFiltering(Ogre::TFO_NONE);
	addTusToParameter(inputPosTexture,tus);

	//set the material on the dummy object
	getManualObject()->setMaterialName(0,m_actualMaterialPtr->getName());

	//add the pointScale parameter
	m_pointSizeParameter = new NumberParameter("pointSize",Frapper::Parameter::T_Int,2);
	m_pointSizeParameter->setNode(this);
	m_pointSizeParameter->setMaxValue(1000);
	m_pointSizeParameter->setMinValue(0);
	parameterRoot->addParameter(m_pointSizeParameter);
	m_pointSizeParameter->setProcessingFunction(SLOT(setScaleParameter()));

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the PointSimpleShaderNode class.
//!
PointSimpleShaderNode::~PointSimpleShaderNode ()
{

	//remove material copys
	Ogre::MaterialManager::getSingleton().remove(m_flatMaterialPtr->getHandle());
	Ogre::MaterialManager::getSingleton().remove(m_actualMaterialPtr->getHandle());

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
void PointSimpleShaderNode::setScaleParameter()
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

} // end namespace
