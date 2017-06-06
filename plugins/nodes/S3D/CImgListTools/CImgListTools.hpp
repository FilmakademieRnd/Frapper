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
//! \file "CImgListTools.cpp"
//! \brief Implementation file for CImgListTools class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!


// Frapper
#include "CImgListTools.h"
#include "OgreTools.h"

// Ogre
#include <OgreBitwise.h>

// charon
#include <ParameteredObject.hxx>
#include <Slots.hxx>

template<typename T>
static void CImgListTools::copyImages( Slot* src, Slot* dst )
{
	InputSlot<cimg_library::CImgList<T>>* slotIn = dynamic_cast<InputSlot<cimg_library::CImgList<T>>*>(src);
	OutputSlot<cimg_library::CImgList<T>>* slotOut = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(dst);

	if( slotIn && slotOut ) 
	{
		try
		{
			slotOut->prepare();
			const cimg_library::CImgList<T> &imageListIn = (*slotIn)();
			cimg_library::CImgList<T>       &imageListOut = (*slotOut)();
			
			imageListOut.clear();
			//imageListOut.get_insert( imageListIn);

			imageListOut.assign(imageListIn);
		}
		catch (std::exception& e)
		{
			Frapper::Log::error( QString(e.what()), "CImgListTools::copyImages");
		}
	}
}

template<typename T>
static void CImgListTools::prepareImageList( Slot* src, Slot* dst )
{
	InputSlot<cimg_library::CImgList<T>>* slotIn = dynamic_cast<InputSlot<cimg_library::CImgList<T>>*>(src);
	OutputSlot<cimg_library::CImgList<T>>* slotOut = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(dst);

	if( slotIn && slotOut )
	{
		try
		{
			slotOut->prepare();
			const cimg_library::CImgList<T> &imageListIn = (*slotIn)();
			cimg_library::CImgList<T>       &imageListOut = (*slotOut)();

			if( imageListIn.size() != imageListOut.size()) {

				unsigned int size = imageListIn.size();
				if( size > 0 ){
					const cimg_library::CImg<T> firstImage = imageListIn[0];
					unsigned int width = firstImage.width();
					unsigned int height = firstImage.height();
					unsigned int depth = firstImage.depth();
					unsigned int dim = firstImage.spectrum();

					// init image list with zero-valued images
					imageListOut.assign( size, width, height, depth, dim, 0 );
				} else {
					imageListOut.clear();
				}
			}
		}
		catch (std::exception& e)
		{
			Frapper::Log::error( QString(e.what()), "CImgListTools::createImages");
		}
	}
}

template<typename T>
static void CImgListTools::resizeImageList( Slot* slot, size_t nImages, bool shrink )
{
	OutputSlot<cimg_library::CImgList<T>>* slotOut = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(slot);

	if( slotOut )
	{
		try
		{
			slotOut->prepare();
			cimg_library::CImgList<T>       &imageListOut = (*slotOut)();

			while ( imageListOut.size() < nImages ) {
				imageListOut.push_back( cimg_library::CImg<T>());
			}
			while( shrink && imageListOut.size() > nImages ) {
				imageListOut.pop_back();
			}
		}
		catch (std::exception& e)
		{
			Frapper::Log::error( QString(e.what()), "CImgListTools::createImages");
		}
	} 
	else
	{
		Frapper::Log::warning( "Only CImgLists in output slots can be resized!", "CImgListTools::resizeImageList" );
	}
}

template<typename T>
static void CImgListTools::clearImageList( Slot* slot )
{
	OutputSlot<cimg_library::CImgList<T>>* slotOut = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(slot);

	if( slotOut )
	{
		try
		{
			slotOut->prepare();
			cimg_library::CImgList<T>       &imageListOut = (*slotOut)();

			for( cimg_library::CImgList<T>::iterator listIter = imageListOut.begin(); listIter != imageListOut.end(); listIter++ )
			{
				listIter->fill(0);
			}
		}
		catch (std::exception& e)
		{
			Frapper::Log::error( QString(e.what()), "CImgListTools::createImages");
		}
	} 
	else
	{
		Frapper::Log::warning( "Only CImgLists in output slots can be resized!", "CImgListTools::resizeImageList" );
	}
}

template<typename T>
static void CImgListTools::clearImage( Slot* slot, unsigned int index )
{
	OutputSlot<cimg_library::CImgList<T>>* slotOut = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(slot);

	if( slotOut )
	{
		try
		{
			slotOut->prepare();
			cimg_library::CImgList<T>       &imageListOut = (*slotOut)();

			if( checkBounds(index, imageListOut )) {
				imageListOut[index].fill(0);
			}
		}
		catch (std::exception& e)
		{
			Frapper::Log::error( QString(e.what()), "CImgListTools::createImages");
		}
	} 
	else
	{
		Frapper::Log::warning( "Only CImgLists in output slots can be resized!", "CImgListTools::resizeImageList" );
	}
}

template<typename T>
static void CImgListTools::imageFromSlot (Ogre::TexturePtr& image, Slot *slot, unsigned int imageListIndex)
{
	try
	{
		OutputSlot<cimg_library::CImgList<T>>* pOutputSlot = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(slot);
		if( pOutputSlot)
		{
			const cimg_library::CImgList<T> &list = (*pOutputSlot)(); // throws
			imageToTexture(list, image, imageListIndex);
		}
		else
		{
			InputSlot<cimg_library::CImgList<T>>* pInputSlot = dynamic_cast<InputSlot<cimg_library::CImgList<T>>*>(slot);
			if( pInputSlot)
			{
				const cimg_library::CImgList<T> &list = (*pInputSlot)(); // throws
				imageToTexture(list, image, imageListIndex);
			}
		}
	}
	catch (std::exception& e)
	{
		Frapper::Log::error( QString(e.what()), "CImgListTools::imageFromSlot");
		return;
	}
}

template<typename T>
static void CImgListTools::imageToSlot ( Ogre::TexturePtr& image, Slot* slot, unsigned int imageListIndex )
{
	// prepare access to cimg_library::CImgList<T>
	OutputSlot<cimg_library::CImgList<T>>* pSlot = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>( slot );

	try
	{
		// get CImgList list
		cimg_library::CImgList<T> &list = (*pSlot)(); // may throw

		// replace selected frame in list with current image
		if( checkBounds( imageListIndex, list) )
			ogreTextureToCImg( image, list( imageListIndex ));
	}
	catch (std::exception& e)
	{
		Frapper::Log::error( QString(e.what()), "CImgListTools::ImageToSlot");
		return;
	}
}

template<typename T>
static void CImgListTools::imageToTexture( const cimg_library::CImgList<T>& list, Ogre::TexturePtr& image, unsigned int imageListIndex )
{
	if( list.empty())
		return;

	if( checkBounds( imageListIndex, list) )
	{
		cimg_library::CImg<T> c_img = list[imageListIndex];
		cImgToOgreTexture( c_img, image );
	}
}

template<typename T>
static void CImgListTools::cImgToOgreTexture( const cimg_library::CImg<T>& in, Ogre::TexturePtr& out )
{
	// buffer info
	const uint width    = in.width();
	const uint height   = in.height();
	const uint spectrum = in.spectrum();
	const uint depth    = in.depth();

	if (width + height < 2) {
		Frapper::Log::error(QString("Input Image not valid!"), "CImgListTools::BlitData");
		return;
	}

	// re-/create ogre image buffer
	Ogre::String textureName = out->getName();
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	if (textureManager.resourceExists( textureName )) {
		textureManager.remove( textureName );
	}

	Ogre::PixelFormat format;

	if( spectrum == 1)
		format = Ogre::PF_L8;
	else if( spectrum == 3)
		format = Ogre::PF_X8R8G8B8;
	else
		format = Ogre::PF_A8R8G8B8;

	out = textureManager.createManual(
		textureName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D,
		width, height, 0, 
		format,
		Ogre::TU_DEFAULT );


	Frapper::OgreTools::HardwareBufferLocker hbl( out->getBuffer(),  Ogre::HardwareBuffer::HBL_DISCARD);

	// buffer data
	const Ogre::PixelBox &pixBox = hbl.getCurrentLock();

	uchar* pixelData = static_cast<uchar*>(pixBox.data);
	ulong pxDataIndex = 0;
	ulong pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes( format ); 

	// copy loops
	for(uint y = 0; y < height; ++y) 
	{
		for(uint x = 0; x < width; ++x) 
		{
			if( spectrum == 1) // treat as mask
			{
				Ogre::uint8 val = static_cast<Ogre::uint8>(Ogre::Bitwise::floatToFixed(copyToFloat<T>(in(x,y,0))/255.0f, 8));
				*(static_cast<Ogre::uint8*>(pixelData + pxDataIndex)) = val;
			}
			else // copy all available channels
			{
				Ogre::ColourValue col(0,0,0,1);
				for(uint c = 0; c < spectrum; ++c) 
					col[c] = copyToFloat<T>(in(x,y,c))/255.0f;
				Ogre::PixelUtil::packColour( col, format, static_cast<void*> (pixelData + pxDataIndex));
			}
			pxDataIndex += pxDataIndexStep;
		}
	}
}

template<typename T>
static void CImgListTools::ogreTextureToCImg( Ogre::TexturePtr& in, cimg_library::CImg<T>& out )
{
	if( in.isNull())
		return;

	size_t w = in->getWidth();
	size_t h = in->getHeight();
	Ogre::PixelFormat format = in->getFormat();

	size_t pixDataSize = Ogre::PixelUtil::getMemorySize( w, h, 1, format);

	uchar* pixData = OGRE_ALLOC_T( uchar, pixDataSize, Ogre::MEMCATEGORY_GENERAL);
	Ogre::PixelBox pixBox( w, h, 1, format, pixData);
	in->getBuffer(0, 0)->blitToMemory(pixBox);

	// pointers, step size and offset to access pixel data
	ulong pxDataIndex = 0;
	ulong pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes ( format ); 

	// create corresponding cimg
	out.resize( w, h, 1, 3 ); // Just to be sure. Normally, the format doesn't change

	// copy loops
	for( size_t y = 0; y < h; ++y) 
	{
		for( size_t x = 0; x < w; ++x) 
		{
			Ogre::ColourValue cv;
			Ogre::PixelUtil::unpackColour( &cv, format, static_cast<void*> (pixData + pxDataIndex));

			for( size_t c = 0; c < 3; ++c)
				out(x,y,c) = copyFromFloat<T>( cv[c] * 255.0f);

			pxDataIndex += pxDataIndexStep;
		}
	}

	OGRE_FREE( pixData, Ogre::MEMCATEGORY_GENERAL);
}

template<typename T>
static size_t CImgListTools::getListSize( Slot* slot )
{
	try
	{
		OutputSlot<cimg_library::CImgList<T>>* pOutputSlot = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(slot);
		if( pOutputSlot)
		{
			const cimg_library::CImgList<T> &list = (*pOutputSlot)(); // throws
			return list.size();
		}
		else
		{
			InputSlot<cimg_library::CImgList<T>>* pInputSlot = dynamic_cast<InputSlot<cimg_library::CImgList<T>>*>(slot);
			if( pInputSlot)
			{
				const cimg_library::CImgList<T> &list = (*pInputSlot)(); // throws
				return list.size();
			}
		}
	}
	catch (std::exception& e)
	{
		Frapper::Log::error( QString(e.what()), "CImgListTools::getListSize");
		return 0;
	}
}


template<typename T>
const static bool CImgListTools::checkBounds( unsigned int& index, const cimg_library::CImgList<T>& list )
{
	if( list.size() == 0)
		return false;
	else if( index < 0 )
		return false;
	else if( index >= list.size() )
		return false;
	else
		return true;
}

template<> inline static float CImgListTools::copyToFloat<int> ( int data )       { return Ogre::Bitwise::fixedToFloat(data, 32); }
template<> inline static float CImgListTools::copyToFloat<float> ( float data )   { return data; }
template<> inline static float CImgListTools::copyToFloat<double> ( double data ) { return static_cast<float> (data); }

template<> inline static int    CImgListTools::copyFromFloat<int>   ( float val ) { return static_cast<int>(val); }
template<> inline static float  CImgListTools::copyFromFloat<float> ( float val ) { return val; }
template<> inline static double CImgListTools::copyFromFloat<double>( float val ) { return static_cast<double>(val); }
