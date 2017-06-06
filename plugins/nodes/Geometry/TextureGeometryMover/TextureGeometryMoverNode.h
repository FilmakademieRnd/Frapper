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
//! \file "TextureGeometryMoverNode.h"
//! \brief Header file for TextureGeometryMoverNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef TEXTUREGEOMETRYMOVERNODE_H
#define TEXTUREGEOMETRYMOVERNODE_H

#include "TextureGeometryNode.h"
#include "Ogre.h"
#include "OgreTools.h"

namespace TextureGeometryMoverNode{
using namespace Frapper;

//!
//! Class representing nodes generate a texture map of position data on a rendertarget texture.
//!
class TextureGeometryMoverNode : public TextureGeometryNode
{

    Q_OBJECT
	ADD_INSTANCE_COUNTER


public: // constructors and destructors

    //!
    //! Constructor of the TextureGeometryMoverNode class.
    //!
    //! \param name The name to give the new TextureGeometryMoverNode node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
	//! \param outputImageName the name of the output image texture
    //!
    TextureGeometryMoverNode ( const QString &name, ParameterGroup *parameterRoot);

    //!
    //! Destructor of the TextureGeometryMoverNode class.
    //!
    ~TextureGeometryMoverNode ();

private slots: //

	//!
    //! update the count
    //!
	void setCountParameter();

	//!
    //! update the move
    //!
	void setMoveParameter();

private: // data

	//!
    //! The Pointer to the Parameter for changing point count
    //!
    NumberParameter *m_pointCountParameter;

	//!
    //! The Pointer to the Parameter for changing the move amount
    //!
    NumberParameter *m_pointMoveParameter;

	//!
    //! The Pointer to the flat material
    //!
	Ogre::MaterialPtr m_materialPtr;

};

} // end namespace

#endif
