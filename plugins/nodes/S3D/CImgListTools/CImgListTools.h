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
//! \file "MaskEditNode.cpp"
//! \brief Header file for MaskEditNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!

#ifndef MASKEDIT_H
#define MASKEDIT_H

#include <charon-core/ParameteredObject.h>
#include <charon-core/Slots.h>
#include <charon-utils/CImg.h>
#include <QString>
#include "OgreTexture.h"

//!
//! Base class for all render nodes.
//!
class CImgListTools
{

public: // functions

	//!
	//! Copy images an from input slot to an output slot
	//!
	//! \param src The slot to read the CImgList from
	//! \param dst The slot to write the CImgList to
	template<typename T>
	static void copyImages( Slot* src, Slot* dst );

	//!
	//! Create list of empty (black )images according to properties of input images
	//!
	//! \param src The slot to read the CImgList from
	//! \param dst The slot to write the CImgList to
	template<typename T>
	static void prepareImageList( Slot* src, Slot* dst );

	//!
	//! Create list of empty (black )images according to properties of input images
	//!
	//! \param src The slot to read the CImgList from
	//! \param dst The slot to write the CImgList to
	template<typename T>
	static void resizeImageList( Slot* slot, size_t nImages, bool shrink = false );

	//!
	//! Create list of empty (black )images according to properties of input images
	//!
	//! \param src The slot to read the CImgList from
	//! \param dst The slot to write the CImgList to
	template<typename T>
	static void clearImageList( Slot* slot );

	//!
	//! Create list of empty (black )images according to properties of input images
	//!
	//! \param src The slot to read the CImgList from
	//! \param dst The slot to write the CImgList to
	template<typename T>
	static void clearImage( Slot* slot, unsigned int index );

	//!
	//! Reads the actual image from the given slot and writes it to the given ogre texture
	//!
	//! \param image The Ogre::TexturePtr to write the image to
	//! \param srcSlot The slot to read the image from
	//!
	template<typename T> 
	static void imageFromSlot ( Ogre::TexturePtr& image, Slot *slot, unsigned int imageListIndex);

	//!
	//! Reads the actual image from the given ogre texture and writes it to the given slot
	//!
	//! \param image The Ogre::TexturePtr to read the image from
	//! \param srcSlot The slot to write the image to
	//!
	template<typename T>
	static void imageToSlot ( Ogre::TexturePtr& image, Slot* outputSlot, unsigned int imageListIndex );

	template<typename T>
	static void imageToTexture( const cimg_library::CImgList<T>& list, Ogre::TexturePtr& image, unsigned int imageListIndex );
	
	//!
	//! Copies an ogre texture to a cimg
	//!
	//! \param in An ogre texture
	//! \param out A CImg<T>
	//!
	template<typename T>
	static void ogreTextureToCImg( Ogre::TexturePtr& in, cimg_library::CImg<T>& out );

	//!
	//! Copies a cimg to an ogre texture
	//!
	//! \param in A CImg<T>
	//! \param out An ogre texture
	//!
	template<typename T>
	static void cImgToOgreTexture( const cimg_library::CImg<T>& in, Ogre::TexturePtr& out );

	//!
	//! Checks if index is inside bounds of an image list
	//!
	//! \param index Image index to check
	//! \param slot The the slot (input or output) that contains an CImgList<T>
	//!
	template<typename T>
	static size_t getListSize( Slot* slot );

private:
	template<typename T>
	const static bool checkBounds( unsigned int& index, const cimg_library::CImgList<T>& list );

	//!
	//! Static function used by BlitData to convert the data stored in the source buffer.
	//!
	//! \param data The source data
	//!
	template<typename T>
	inline static float copyToFloat( T data);

	//!
	//! Static function used by BlitData to convert the data stored in the source buffer.
	//!
	//! \param data The source data
	//!
	template<typename T>
	inline static T copyFromFloat( float val);
};

#endif

