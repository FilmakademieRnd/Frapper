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
//! \file "AmbientOcclusionNodePlugin.h"
//! \brief Header file for AmbientOcclusionNodePlugin class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \version    1.0
//! \date       23.10.2009 (last updated)
//!

#ifndef AMBIENTOCCLUSIONNODEPLUGIN_H
#define AMBIENTOCCLUSIONNODEPLUGIN_H

#include "NodeTypeInterface.h"

namespace AmbientOcclusionNode {
using namespace Frapper;

//!
//! Plugin class for creating AmbientOcclusionNode objects.
//!
class AmbientOcclusionNodePlugin : public QObject, public NodeTypeInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "de.filmakademie.Nodes.NodeTypeInterface/1.1" FILE "metadata.json")
#endif
    Q_INTERFACES(Frapper::NodeTypeInterface)

public: // functions

    //!
    //! Creates a node of this node type.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //! \return A pointer to the new node.
    //!
    virtual Node * createNode ( const QString &name, ParameterGroup *parameterRoot );

};

} // namespace AmbientOcclusionNode 

#endif