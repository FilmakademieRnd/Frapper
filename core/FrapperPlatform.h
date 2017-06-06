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
//! \brief Platform dependet configurations.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       10.08.2009 (last updated)
//!

#ifndef FRAPPERPLATFORM_H
#define FRAPPERPLATFORM_H

#ifdef _WIN32
  #ifdef frappercore_EXPORTS
    #define FRAPPER_CORE_EXPORT __declspec(dllexport)
  #else
    #define FRAPPER_CORE_EXPORT __declspec(dllimport)
  #endif
  #ifdef frappergui_EXPORTS
    #define FRAPPER_GUI_EXPORT __declspec(dllexport)
  #else
    #define FRAPPER_GUI_EXPORT __declspec(dllimport)
  #endif
#else
  #define FRAPPER_CORE_EXPORT
#endif

#endif