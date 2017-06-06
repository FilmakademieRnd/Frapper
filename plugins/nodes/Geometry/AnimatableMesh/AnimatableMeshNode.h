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
//! \file "AnimatableMeshNode.h"
//! \brief Header file for AnimatableMeshNode class.
//!
//! \author     Volker Helzle <volker.helzle@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       26.11.2012 (last updated)
//!

#ifndef ANIMATABLEMESHNODE_H
#define ANIMATABLEMESHNODE_H

#include "GeometryAnimationNode.h"

namespace AnimatableMeshNode {
using namespace Frapper;

//!
//! Class representing nodes that can contain OGRE entities with animation.
//!
class AnimatableMeshNode : public GeometryAnimationNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the AnimatableMeshNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AnimatableMeshNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AnimatableMeshNode class.
    //!
    ~AnimatableMeshNode ();

};

} // namespace AnimatableMeshNode
#endif
