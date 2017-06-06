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
//! \file "StereoViewNVApiWrapper.cpp"
//! \brief Implementation file for StereoViewNVApiWrapper class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!


#include "StereoViewNVApiWrapper.h"

#include "nvapi.h"
#include "NvApiDriverSettings.h"
#include <stdio.h>

///
/// Private Static Data
///

bool StereoViewNVApiWrapper::s_initialized = false;

//!
//! Public functions
//!

void StereoViewNVApiWrapper::enableStereo()
{
	if( !s_initialized)
		initNVAPI();

	NvAPI_Status status = NvAPI_Stereo_Enable();
	if (status != NVAPI_OK)
	{
		PrintError(status);
		return;
	}
}

void StereoViewNVApiWrapper::disableStereo()
{
	if( !s_initialized)
		initNVAPI();

	NvAPI_Status status = NvAPI_Stereo_Disable();
	if (status != NVAPI_OK)
	{
		PrintError(status);
		return;
	}
}

//!
//! Private functions
//!

void StereoViewNVApiWrapper::initNVAPI()
{
	NvAPI_Status status = NvAPI_Initialize();
	if (status != NVAPI_OK)
	{
		PrintError(status);
		return;
	}
}


void StereoViewNVApiWrapper::PrintError(NvAPI_Status status)
{
	NvAPI_ShortString szDesc = {0};
	NvAPI_GetErrorMessage(status, szDesc);
	printf(" NVAPI error: %s\n", szDesc);
}
