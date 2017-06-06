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
//! \file "FrapperPrerequisites.h"
//! \brief Defines and includes needed for the whole project.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       10.08.2009 (last updated)
//!

#ifndef FRAPPERPREREQUISITES_H
#define FRAPPERPREREQUISITES_H

#include <QtPlugin>

//!
//! The Frapper namespace.
//!
#define BEIGN_NAMESPACE_FRAPPER { namespace Frapper {
#define END_NAMESPACE_FRAPPER } 

//!
//! The number of stages that will be available to contain nodes.
//!
#define NUMBER_OF_STAGES 2

//!
//! Qt plugin system
//!
#ifndef Q_PLUGIN_METADATA
#define Q_PLUGIN_METADATA(x)
#endif

#include "FrapperPlatform.h"

#endif