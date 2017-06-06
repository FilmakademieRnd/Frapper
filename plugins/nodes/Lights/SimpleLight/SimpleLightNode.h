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
//! \file "SimpleLightNode.h"
//! \brief Header file for SimpleLightNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       17.02.2015 (last updated)
//!

#ifndef SIMPLELIGHTNODE_H
#define SIMPLELIGHTNODE_H

#include "LightNode.h"
#include "OgreContainer.h"
#include "InstanceCounterMacros.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace SimpleLightNode {
using namespace Frapper;

//!
//! Class representing nodes for lights in a 3D scene.
//!
class SimpleLightNode : public LightNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the SimpleLightNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    SimpleLightNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the SimpleLightNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~SimpleLightNode ();


private slots: //

    //!
    //! Applies all additional parameters of the light node.
    //!
    //! Is called when any of the additional parameters of the light node has been
    //! changed
    //!
    void applyParameters ();

};

} // namespace SimpleLightNode

#endif
