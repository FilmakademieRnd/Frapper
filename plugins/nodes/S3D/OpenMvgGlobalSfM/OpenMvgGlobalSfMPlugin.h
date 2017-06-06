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
//! \file "OpenMvgGlobalSfMPlugin.h"
//! \brief Header file for OpenMvgGlobalSfM class.
//!
//! \author     Jonas Trottnow <jtrottno@filmakademie.de>
//! \version    1.0
//! \date       03.02.2014 (last updated)
//!

#ifndef OpenMvgGlobalSfMPLUGIN_H
#define OpenMvgGlobalSfMPLUGIN_H

#include "NodeTypeInterface.h"
#include "Node.h"
#include "OpenMvgGlobalSfM.h"

namespace OpenMvgGlobalSfM {
using namespace Frapper;

//!
//! Plugin class for creation of OpenMvgGlobalSfM node.
//!
class OpenMvgGlobalSfMPlugin : public QObject, public Frapper::NodeTypeInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "de.filmakademie.Nodes.NodeTypeInterface/1.1" FILE "metadata.json")
#endif
    Q_INTERFACES(Frapper::NodeTypeInterface)

public: /// methods

    Node * createNode ( const QString &name, ParameterGroup *parameterRoot );


};

} // namespace OpenMvgGlobalSfM

#endif
