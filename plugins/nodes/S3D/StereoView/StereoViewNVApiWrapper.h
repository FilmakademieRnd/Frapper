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
//! \file "StereoViewNVApiWrapper.h"
//! \brief Header file for StereoViewNVApiWrapper class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!

#ifndef STEREOVIEWNVAPIWRAPPER_H
#define STEREOVIEWNVAPIWRAPPER_H

#include "nvapi.h"

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class StereoViewNVApiWrapper
{

public: // constructors and destructors

	static void enableStereo();
	static void disableStereo();

private: // functions

	static void PrintError(NvAPI_Status status);
	static void initNVAPI();

private: //data

	static bool s_initialized;

};
#endif // STEREOVIEWNVAPIWRAPPER_H
