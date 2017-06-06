/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "PainterPanelExports.h"
//! \brief DLL import/export declaration for win32
//!
//! \author     Michael Bu�ler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       14.10.2013 (created)
//!

#ifndef PAINTERPANELEXPORTS_H
#define PAINTERPANELEXPORTS_H

#ifdef _WIN32
	#ifdef painterpanel_EXPORTS
		#define PAINTER_PANEL_EXPORT __declspec(dllexport)
	#else
		#define PAINTER_PANEL_EXPORT __declspec(dllimport)
	#endif
#else
	#define PAINTER_PANEL_EXPORT
#endif

#endif
