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
//! \file "PatchMatchPlugin.cpp"
//! \brief 
//!
//! \author		Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       03.02.2014 (last updated)
//!

#include "PatchMatchPlugin.h"
#include "Node.h"

#include <QtCore/QtPlugin>

namespace PatchMatchNode {
using namespace Frapper;

//!
//! Creates a node of this node type.
//!
//! \param name The name for the new node.
//! \return A pointer to the new node.
//!
Node * PatchMatchPlugin::createNode ( const QString &name, ParameterGroup *parameterRoot )
{
	return new PatchMatchNode(name, parameterRoot);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PatchMatchplugin, PatchMatchPlugin)
#endif

} // namespace PatchMatch
