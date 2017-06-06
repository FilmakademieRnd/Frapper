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
//! \file "TextureGeometry.h"
//! \brief Implementation file for TextureGeometry class.
//!
//! \author     Amit Rojtblat <amit.rojtblat@filmakademie.de>
//! \version    1.0
//! \date       22.02.2014 (last updated)
//!

#include "TextureGeometryShaderNode.h"
#include "OgreManager.h"
#include "OgreTools.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>

namespace Frapper{

///
/// Constructors and Destructors
///


//!
//! Constructor of the TextureGeometry class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TextureGeometryShaderNode::TextureGeometryShaderNode ( const QString &name, ParameterGroup *parameterRoot, int pointsPerPrimitive ) :
    TextureGeometryNodeAbstract(name, parameterRoot){
}


//!
//! Destructor of the TextureGeometry class.
//!
TextureGeometryShaderNode::~TextureGeometryShaderNode ()
{
}

///
/// Public functions
///

//!
//! copy the material (creates a copy)
//!
void TextureGeometryShaderNode::setMaterial(const Ogre::String matName){

	m_materialPtr = copyMaterial(matName);
	setAffectedBycount(m_materialPtr);

	getManualObject()->setMaterialName(0,m_materialPtr->getName());

}

///
/// Protected Functions
///

///
/// public Slots
///

///
/// Private Functions
///

///
/// Private Slots
///

} // end namespace
