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
//! \file "CameraInputNodePlugin.h"
//! \brief Header file for CameraInputNode class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    0.0
//! \date       24.01.2011 (last updated)
//!

#ifndef CAMERAINPUTNODEPLUGIN_H
#define CAMERAINPUTNODEPLUGIN_H

#include "QtGui/QColor"
#include "QtCore/QVariant"
#include "QtGui/QStandardItem"

#include "NodeTypeInterface.h"
#include "Node.h"
#include "CameraInputNode.h"

namespace CameraInputNode {
using namespace Frapper;

//!
//! Plugin class for creation of CameraInputNode node.
//!
class CameraInputNodePlugin : public QObject, public NodeTypeInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "de.filmakademie.Nodes.NodeTypeInterface/1.1" FILE "metadata.json")
#endif
    Q_INTERFACES(Frapper::NodeTypeInterface)

public: /// methods

    Node * createNode ( const QString &name, ParameterGroup *parameterRoot );


};

} // namespace CameraInputNode

#endif
