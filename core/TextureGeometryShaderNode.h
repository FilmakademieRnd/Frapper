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
//! \brief Header file for TextureGeometry class.
//!
//! \author     Amit Rojtblat <amit.rojtblat@filmakademie.de>
//! \version    1.0
//! \date       22.02.2014 (last updated)
//!

#ifndef TEXTUREGEOMETRYSHADERNODE_H
#define TEXTUREGEOMETRYSHADERNODE_H

#include "TextureGeometryNodeAbstract.h"

namespace Frapper {

//!
//! Class representing nodes generate a texture map of position data on a rendertarget texture.
//!
class FRAPPER_CORE_EXPORT TextureGeometryShaderNode : public TextureGeometryNodeAbstract
{

    Q_OBJECT


public: // constructors and destructors

    //!
    //! Constructor of the TextureGeometry class.
    //!
    //! \param name The name to give the new TextureGeometry node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TextureGeometryShaderNode ( const QString &name, ParameterGroup *parameterRoot, int pointsPerPrimitive = 266240);

    //!
    //! Destructor of the TextureGeometry class.
    //!
    ~TextureGeometryShaderNode ();

	//!
	//! get the material
	//!
	Ogre::MaterialPtr getMaterial(){return m_materialPtr;}

	//!
	//! copy the material (creates a copy)
	//!
	void setMaterial(const Ogre::String matName);

private: // functions

private slots: // functions

private: // data

	//!
    //! The Pointer to the material to use for the shading
    //!
	Ogre::MaterialPtr m_materialPtr;


};

}; //namespace
#endif
