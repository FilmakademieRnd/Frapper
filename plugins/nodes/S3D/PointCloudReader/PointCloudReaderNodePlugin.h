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
//! \file "PointCloudReaderNodePlugin.h"
//! \brief Header file for PointCloudReaderNodePlugin class.
//!
//! \author     Volker Helzle <sspielma@filmakademie.de>
//! \version    1.0
//! \date       30.07.2014 (last updated)
//!

#ifndef POINTCLOUDREADERNODEPLUGIN_H
#define POINTCLOUDREADERNODEPLUGIN_H

#include "NodeTypeInterface.h"
#include "ParameterGroup.h"
#include "Node.h"


//!
//! Plugin class for creating PointCloudReaderNode objects.
//!
class PointCloudReaderNodePlugin : public QObject, public Frapper::NodeTypeInterface
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
    virtual Frapper::Node * createNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

};


#endif