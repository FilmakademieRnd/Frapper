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
//! \file "TextureGeometryCreatorNode.h"
//! \brief Implementation file for TextureGeometryCreatorNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "TextureGeometryCreatorNode.h"
#include "OgreManager.h"
#include "OgreTools.h"

namespace TextureGeometryCreatorNode{
using namespace Frapper;

INIT_INSTANCE_COUNTER(TextureGeometryCreatorNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the TextureGeometryCreatorNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TextureGeometryCreatorNode::TextureGeometryCreatorNode ( const QString &name, ParameterGroup *parameterRoot ) :
	TextureGeometryNode(name,parameterRoot),
	m_pointCountParameter(0)	{


	m_materialPtr = copyMaterial("FlatViewShaderMaterial");

	// add the output image
	addOutputRenderTargetImage(	createUniqueName("Position").c_str(),
							"outPosition",
							512,
							Ogre::PF_R8G8B8A8,
							64,
							m_materialPtr		);


	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the TextureGeometryCreatorNode class.
//!
TextureGeometryCreatorNode::~TextureGeometryCreatorNode ()
{

	//remove material copys
	//Ogre::MaterialManager::getSingleton().remove(m_materialPtr->getHandle());

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

} // end namespace
