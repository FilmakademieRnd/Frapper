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
//! \file "SimpleRenderNode.h"
//! \brief Header file for SimpleRenderNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    0.1
//! \date       30.09.2010 (last updated)
//!

#ifndef SimpleRenderNode_H
#define SimpleRenderNode_H

#include "GeometryRenderNode.h"
#include "OgreTools.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace SimpleRenderNode {
using namespace Frapper;

//!
//! Forward declaration for a simple render node.
//!
class SimpleRenderNode;


//!
//! Class for a basic render node.
//!
class SimpleRenderNode : public GeometryRenderNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the SimpleRenderNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    SimpleRenderNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the SimpleRenderNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~SimpleRenderNode ();

};

} // namespace SimpleRenderNode 

#endif