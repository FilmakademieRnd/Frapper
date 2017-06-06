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
//! \file "ImageSaverNode.cpp"
//! \brief Implementation file for ImageSaverNode class.
//!
//! \author     Christopher Lutz <clutz@animationsnippets.com>
//! \version    1.0
//! \date       15.01.2010
//!

#include "ImageSaverNode.h"
#include "Log.h"

namespace ImageSaverNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(ImageSaverNode)

///
/// Protected Static Constants
///

//!
//! number of image files
//!
const unsigned int ImageSaverNode::NumImages = 4;


///
/// Constructors and Destructors
///


//!
//! Constructor of the ImageSaverNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
//! /todo Christopher: Add user interface for saving options (e.g. compression quality, bit depth, alpha options etc.)
//!
ImageSaverNode::ImageSaverNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_currentFrame(0)

{
	for (unsigned int i = 1; i <= NumImages; i++) {

		// set filename type to 'Save' for all filenames
		FilenameParameter *filenameParameter = getFilenameParameter(QString("Filename %1").arg(i));
		if (filenameParameter) {
			filenameParameter->setType(FilenameParameter::FT_Save);
		} else
			Log::error(QString("Could not obtain parameter 'Filename %1'.").arg(i), "ImageSaverNode::ImageSaverNode");
	}

	// notify this node and save all input images if frame has changed
	connect(this, SIGNAL(frameChanged(int)), SLOT(saveAllImages()));

    // set the 'Save Current Frame' functionality
    Parameter *saveCurrentFrameParameter = getParameter("Save Current Frame");
    if (saveCurrentFrameParameter) {
        // set the command function for the parameter 'Save Current Frame'
        saveCurrentFrameParameter->setCommandFunction(SLOT(saveAllImages()));
    } else
        Log::error("Could not obtain Parameter 'Save Current Frame'.", "ImageSaverNode::ImageSaverNode");


    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the ImageSaverNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ImageSaverNode::~ImageSaverNode ()
{
    DEC_INSTANCE_COUNTER
}


///
/// Private Slots
///

//!
//! Saves the a texture to a file.
//!
//! NOTE: This code snippet is based on the function Ogre::Texture::convertToImage()
//! that is intruduced with OGRE v.17 RC1
//!
//! /todo Christopher: Once FRAPPER is migrated to OGRE v1.7, use
//! Ogre::Texture::convertToImage() instead to avoid duplicate code
//!
void ImageSaverNode::saveImage(Ogre::TexturePtr tex, std::string filename)
{
	size_t numMips = 1;
	size_t dataSize = Ogre::Image::calculateSize(numMips,
		tex->getNumFaces(), tex->getWidth(), tex->getHeight(), tex->getDepth(), tex->getFormat());

	void* pixData = OGRE_MALLOC(dataSize, Ogre::MEMCATEGORY_GENERAL);
	// if there are multiple faces and mipmaps we must pack them into the data
	// faces, then mips
	void* currentPixData = pixData;
	for (size_t face = 0; face < tex->getNumFaces(); ++face)
	{
		for (size_t mip = 0; mip < numMips; ++mip)
		{
			size_t mipDataSize = Ogre::PixelUtil::getMemorySize(tex->getWidth(),
				tex->getHeight(), tex->getDepth(), tex->getFormat());

			Ogre::PixelBox pixBox(tex->getWidth(), tex->getHeight(), tex->getDepth(), tex->getFormat(), currentPixData);
			tex->getBuffer(face, mip)->blitToMemory(pixBox);

			currentPixData = (void*)((char*)currentPixData + mipDataSize);

		}
	}

	// load, and tell Image to delete the memory when it's done.
	Ogre::Image destImage;
	destImage.loadDynamicImage((Ogre::uchar*)pixData, tex->getWidth(), tex->getHeight(), tex->getDepth(),
		tex->getFormat(), true, tex->getNumFaces(), numMips - 1);

	destImage.save(filename);

    //OGRE_FREE( pixData, Ogre::MEMCATEGORY_GENERAL);

}

//!
//! Saves the node's input images to file.
//!
void ImageSaverNode::saveAllImages()
{
	// if 'Eval' flag is not set, do nothing
	if (!m_evaluate)
		return;

    // obtain the parameter 'File Format'
    EnumerationParameter *fileFormatParameter = getEnumerationParameter("File Format");
    if (!fileFormatParameter) {
        Log::error("Could not obtain parameter 'File Format'.", "ImageSaverNode::processOutputImage");
        return;
    }

	// get the value of the currently selected file format in the enumeration
    QString fileFormat = fileFormatParameter->getCurrentValue();
	if (fileFormat.isNull()) {
        Log::error("No value set for parameter 'File Format'.", "ImageSaverNode::processOutputImage");
        return;
	}

	// get the value of the parameter 'Offset'
    NumberParameter *offsetParameter = getNumberParameter("Offset");
	if (!offsetParameter) {
        Log::error("Could not obtain parameter 'Offset'.", "ImageSaverNode::processOutputImage");
        return;
	}

	// get index of current frame (with addition of the offset)
    int currentFrame = m_timeParameter->getValue().toInt() + offsetParameter->getValue().toInt();
    //int currentFrame = m_currentFrame;

	// go through all input images and save them to their respective files
	for (unsigned int i = 1; i <= NumImages; i++) {

		// obtain the image to save
		Ogre::TexturePtr inputTexture = getTextureValue(QString("Image %1").arg(i), true);
		if (inputTexture.isNull()) {
			Log::debug(QString("No value set for input image %1.").arg(i), "ImageSaverNode::processOutputImage");
			continue;
		}

		// obtain the base filename
		QString filename = getStringValue(QString("Filename %1").arg(i));
		if (filename == "") {
			//Log::debug(QString("No value set for parameter 'Filename %1'.").arg(i), "ImageSaverNode::processOutputImage");
			continue;
		}

		// split the absolute filename to path and base filename
		int lastSlashIndex = filename.lastIndexOf('/');
		QString path = "";
		if (lastSlashIndex > -1) {
			path = filename.mid(0, lastSlashIndex + 1);
			filename = filename.mid(lastSlashIndex + 1);
		}

		//  delete extension from filename
		int lastDotIndex = filename.lastIndexOf('.');
		if (lastDotIndex > -1) {
			filename = filename.mid(0, lastDotIndex);
		}

		// add frame number to filename
		filename += QString("%1").arg(QString("%1").arg(currentFrame), 5, '0');

		// add extension
		filename += fileFormat;

		// save image
		saveImage(inputTexture, QString(path + filename).toStdString());
        m_currentFrame++;
		Log::info(QString(path + filename), "ImageSaverNode::saveAllImages");
	}	
}

} // namespace ImageSaverNode 
