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
//! \file "ImageFileNode.cpp"
//! \brief Implementation file for ImageFileNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       20.07.2009 (last updated)
//!

#include "ImageFileNode.h"
#include "FilenameParameter.h"
#include "NumberParameter.h"
#include "Log.h"
#include <QtCore/QFile>
#include <QtCore/QDir>

namespace ImageFileNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(ImageFileNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the ImageFileNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ImageFileNode::ImageFileNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ImageNode(name, parameterRoot, true, "Loaded Image"),
	m_defaultSize(512, 512),
	m_scaleValue (100)
{
    // set affections and callback functions
    addAffection("Image File", m_outputImageName);
    addAffection("Use Image Sequence", m_outputImageName);
    addAffection("Start Frame", m_outputImageName);
    addAffection("Start Frame Offset", m_outputImageName);
	addAffection("Reset Image Size", m_outputImageName);
	addAffection("Scale in %", m_outputImageName);	
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Image File", SLOT(filenameChanged()));
    setChangeFunction("Use Image Sequence", SLOT(filenameChanged()));
	setChangeFunction("Scale in %", SLOT(resizeImageChanged()));	
    setChangeFunction("Missing Frame Color", SLOT(processDefaultImage()));
	setCommandFunction("Reset Image Size", SLOT(resetScalePressed()));

    processDefaultImage();

    INC_INSTANCE_COUNTER
    }


//!
//! Destructor of the ImageFileNode class.
//!
ImageFileNode::~ImageFileNode ()
{
    DEC_INSTANCE_COUNTER
}


///
/// Public Slots
///

//!
//! Handler function that is called when the value of the "Use Image Sequence"
//! parameter has been changed.
//!
void ImageFileNode::useImageSequenceToggled()
{	
    // update affection with global time parameter
    if (m_timeParameter) {
        Parameter *outputImageParameter = getParameter(m_outputImageName);
        bool useImageSequence = getBoolValue("Use Image Sequence");
        if (useImageSequence)
            m_timeParameter->addAffectedParameter(outputImageParameter);
        else
            m_timeParameter->removeAffectedParameter(outputImageParameter);
    }
}

//!
//! Handler function that is called when the value of the "Scale in %"
//! parameter has been changed.
//!
//! Sets the member variable "m_scaleValue" which is used in 
//! processOutputImage to calculate the new scale factor.
//!
void ImageFileNode::resizeImageChanged()
{
	// update m_scaleValue 
	NumberParameter *scaleParameter = getNumberParameter("Scale in %");
	if (scaleParameter) 
		m_scaleValue = getIntValue("Scale in %");		
	// clear cache
	clearImageCache();	
}

//!
//! Handler function that is called when the value of the "Reset Image Size"
//! button has been pressed.
//!
//! Sets the "Scale in %" back to 100%.
//!
void ImageFileNode::resetScalePressed()
{
	// reset "Scale in %" parameter to 100
	NumberParameter *scaleParameter2 = getNumberParameter("Scale in %");
	if (scaleParameter2) scaleParameter2->setValue(100);	
}

//!
//! Handler function that is called when the value of the image filename
//! parameter has been changed.
//!
//! Detects whether the selected image file is part of a sequence of image
//! files and sets the enabled state of parameters for image sequences
//! accordingly.
//!
void ImageFileNode::filenameChanged ()
{
    // get filename from filename parameter
    QString filename = getStringValue("Image File");
    if (filename == "") {
        Log::warning("Image filename has not been set yet.", "ImageFileNode::filenameChanged");
        return;
    }

    // check if the file exists
    if (!QFile::exists(filename)) {
        Log::error(QString("Image file \"%1\" not found.").arg(filename), "ImageFileNode::filenameChanged");
        return;
    }

    // split the absolute filename to path and base filename
    int lastSlashIndex = filename.lastIndexOf('/');
    QString path = "";
    if (lastSlashIndex > -1) {
        path = filename.mid(0, lastSlashIndex);
        filename = filename.mid(lastSlashIndex + 1);
    }

    // decode the filename into prefix, index string and suffix
    int index = 0;
	int firstIndex = 0;
	int lastIndex = 0;
	QString prefix ("");
	QString indexString ("");
	QString suffix ("");
	decodeFilename(filename, &index, &prefix, &indexString, &suffix);

	// the real file suffix
	QString fileSuffix;
	if (filename.endsWith(".png")) fileSuffix = ".png";
	else if (filename.endsWith(".bmp")) fileSuffix = ".bmp";
	else if (filename.endsWith(".jpg")) fileSuffix = ".jpg";	
	else if (filename.endsWith(".exr")) fileSuffix = ".exr";		
	else fileSuffix = "notSupporrted";	

	// calculate the number of images in the sequence
	int numberOfImages = 0;
	if (!prefix.isEmpty() || !suffix.isEmpty()) {
		// parse the directory that contains the file
		QDir directory (path);        
		QStringList files = directory.entryList(QStringList() << QString("%1*%2%3").arg(prefix).arg(suffix).arg(fileSuffix), QDir::Files, QDir::Name);
		numberOfImages = files.size();

        if (numberOfImages > 0) {
            // decode the indices of the first and last frames
            decodeFilename(files[0], &firstIndex);
            decodeFilename(files[files.size() - 1], &lastIndex);
        }
    }

    // enable or disable the image sequence parameters
    ParameterGroup *imageSequenceParameterGroup = getParameterGroup("Image Sequence");
    if (imageSequenceParameterGroup)
        imageSequenceParameterGroup->setEnabled(numberOfImages > 1);

    // update affection with global time parameter
    if (m_timeParameter) {
        Parameter *outputImageParameter = getParameter(m_outputImageName);
        bool useImageSequence = getBoolValue("Use Image Sequence");
        if (numberOfImages > 1 && useImageSequence)
            m_timeParameter->addAffectedParameter(outputImageParameter);
        else
            m_timeParameter->removeAffectedParameter(outputImageParameter);
    }

    // update sequence length parameter
    Parameter *sequenceLengthParameter = getParameter("Sequence Length");
    if (sequenceLengthParameter) {
        QString sequenceLength;
        if (numberOfImages > 1)
            sequenceLength = QString("%1 images").arg(numberOfImages);
        else
            sequenceLength = "The selected image file does not appear to be part of a sequence.";
        sequenceLengthParameter->setValue(sequenceLength);
    }

    // update start frame parameter
    NumberParameter *startFrameParameter = getNumberParameter("Start Frame");
    if (startFrameParameter) {
        startFrameParameter->setValue(firstIndex);
        startFrameParameter->setMinValue(firstIndex);
        startFrameParameter->setMaxValue(lastIndex);
    }

    // update end frame parameter
    NumberParameter *endFrameParameter = getNumberParameter("End Frame");
    if (endFrameParameter) {
        endFrameParameter->setValue(lastIndex);
        endFrameParameter->setMinValue(firstIndex);
        endFrameParameter->setMaxValue(lastIndex);
    }

	// update "Scale in %" parameter
    NumberParameter *scaleParameter = getNumberParameter("Scale in %");
	if (scaleParameter) scaleParameter->setValue(100);

    // notify connected objects that the node has changed
    emit nodeChanged();

    // clear the image cache
    clearImageCache();

	// obtain the OGRE resource group manager
    Ogre::ResourceGroupManager *resourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();
    if (!resourceGroupManager) {
        Log::error("The OGRE resource group manager could not be obtained.", "ImageFileNode::processOutputImage");
        return;
	}

	// add the image's location to the resource manager if it's not already included
    if (!resourceGroupManager->resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, filename.toStdString()))
        resourceGroupManager->addResourceLocation(path.toStdString(), "FileSystem");

	// determine initial image diemnsions
	Ogre::Image *iImage;
	iImage = new Ogre::Image();
	iImage->load(filename.toStdString(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_defaultSize = QSize(iImage->getWidth(), iImage->getHeight() );	
	setValue("Image Dimensions", QString("%1 x %2").arg(m_defaultSize.width()).arg(m_defaultSize.height()), true);    	
	delete iImage;
}


//!
//! Processes the node's input data to generate the node's output image.
//!
void ImageFileNode::processOutputImage ()
{
    // get parameter values needed for processing the output image
    QString filename = getStringValue("Image File");
    if (filename == "") {
        Log::warning("Image filename has not been set yet.", "ImageFileNode::processOutputImage");
        return;
    }
	
    // check if the file exists
    if (!QFile::exists(filename)) {
        Log::error(QString("Image file \"%1\" not found.").arg(filename), "ImageFileNode::processOutputImage");
        return;
    }

    // split the absolute filename to path and base filename
    int lastSlashIndex = filename.lastIndexOf('/');
    QString path = "";
    if (lastSlashIndex > -1) {
        path = filename.mid(0, lastSlashIndex);
        filename = filename.mid(lastSlashIndex + 1);
    }

	// the real file suffix
	QString fileSuffix;
	if (filename.endsWith(".png")) fileSuffix = ".png";
	else if (filename.endsWith(".bmp")) fileSuffix = ".bmp";
	else if (filename.endsWith(".jpg")) fileSuffix = ".jpg";
	else if (filename.endsWith(".exr")) fileSuffix = ".exr";	
	else {	
		Log::error("The image file format is not supported.", "ImageFileNode::processOutputImage");
        return;
    }

    // decode the filename into prefix, index string and suffix
    int index = 0;
	QString prefix ("");
    QString indexString ("");
    QString suffix ("");
    decodeFilename(filename, &index, &prefix, &indexString, &suffix);

    // update affection with global time parameter
    bool useImageSequence = getBoolValue("Use Image Sequence");
    if (useImageSequence && m_timeParameter) {

		index = m_timeParameter->getValue().toInt();
		const int offset = getIntValue("Start Frame Offset");
		int lastIndex = getIntValue("End Frame");
		int firstIndex = getIntValue("Start Frame");

		index += offset;

		index = (index < firstIndex) ? firstIndex : index;
        index = (index > lastIndex) ? lastIndex : index;

		filename = QString("%1%2%3%4").arg(prefix).arg(index, indexString.size(), 10, QChar('0')).arg(suffix).arg(fileSuffix);
	}

    // check if the file exists
    if (!QFile::exists(QString("%1/%2").arg(path, filename))) {
        //Log::error(QString("Image file \"%1/%2\" not found.").arg(path, filename), "ImageFileNode::processOutputImage");
		setOutputImage(m_defaultImage);
        return;
    }

    // load the image from file or from cache (if activated)
    Ogre::TexturePtr texture;	
    if (m_cacheEnabled && isCached(index)) {
        // load the image from cache
        texture = getCachedImage(index);
    } else {
        // load the image from file
        Ogre::Image image;
		QString filePath = path+'/'+filename;

		try{
			image.load( filePath.toStdString(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);	 // may throw
			// Resize Image			
			if (m_scaleValue != 100) {
				image.resize( 0.01*m_scaleValue*m_defaultSize.width(), 0.01*m_scaleValue*m_defaultSize.height() );
			}
			texture = createTextureFromImage(image);

			// write file to cache
			if (m_cacheEnabled)
				if( m_useGPUCache) // depending on current cache strategy, either cache the image or the texture
					cacheTexture( index, texture);
				else
					cacheImage( index, image);

		} catch( Ogre::Exception e) {
			Log::error("An error occurred while trying to load image from "+filePath+".", "ImageFileNode::processOutputImage");
			setOutputImage(texture);
			return;
		}
    }	

	setOutputImage( texture);

	// resize the default image to last loaded image size
	const unsigned int width  = texture->getWidth();
	const unsigned int height = texture->getHeight();
	if ( (width != m_defaultImage.getWidth()) || (height != m_defaultImage.getHeight()) ) {
		m_defaultImage.resize(width, height);
		processDefaultImage();
	}
}

//!
//! Processes the node's default image.
//!
void ImageFileNode::processDefaultImage ()
{
	unsigned int width = 320, height = 240;
	
	if( m_defaultImage.getWidth() > 0 && m_defaultImage.getHeight() > 0 )
	{
		width = m_defaultImage.getWidth();
		height = m_defaultImage.getHeight();
	}

    QColor color = getColorValue("Missing Frame Color");

    // create an empty image with the requested resolution
	Ogre::PixelFormat pixelFormat = Ogre::PF_R8G8B8;
    size_t memorySize = Ogre::PixelUtil::getMemorySize(width, height, 1, pixelFormat);
    Ogre::uchar *imageData = OGRE_ALLOC_T(Ogre::uchar, memorySize, Ogre::MEMCATEGORY_GENERAL);
    m_defaultImage = Ogre::Image();
    m_defaultImage.loadDynamicImage(imageData, width, height, 1, pixelFormat, true);

    // store the color values to use for the default image
    Ogre::uint8 r = color.red();
    Ogre::uint8 g = color.green();
    Ogre::uint8 b = color.blue();

    // fill the default image with the constant color
    const Ogre::PixelBox &pixelBox = m_defaultImage.getPixelBox();
    Ogre::uint8 *pixelData = static_cast<Ogre::uint8 *>(pixelBox.data);
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x) {
            *pixelData++ = b;
            *pixelData++ = g;
            *pixelData++ = r;
        }
}


///
/// Private Functions
///


//!
//! Decodes the given name of an image file from a sequence into its
//! prefix, index and suffix.
//!
//! \param filename The filename to decode.
//! \param index [out] The extracted index part of the filename converted to an integer number.
//! \param prefix [out] The extracted prefix part of the filename.
//! \param indexString [out] The extracted index part of the filename.
//! \param suffix [out] The extracted prefix part of the filename.
//!
void ImageFileNode::decodeFilename ( const QString &filename, int *index, QString *prefix /* = 0 */, QString *indexString /* = 0 */, QString *suffix /* = 0 */ )
{
    if (!index) {
        Log::error("Invalid pointer for index argument.", "ImageFileNode::decodeFilename");
        return;
    }

    // prepare results
    int indexResult = 0;
    QString prefixResult ("");
    QString indexStringResult ("");
    QString suffixResult ("");

    // decode filename
    QRegExp indexExpression ("(\\d+)(\\D*)(.bmp|.png|.jpg|.exr)");
	// QRegExp indexExpression ("(\\d+)(\\D*).(png|jpg)");
	//QRegExp indexExpression ("(\\d+)(\\D*).png");
    indexExpression.setMinimal(true);
    if (indexExpression.indexIn(filename) != -1) {
        indexStringResult = indexExpression.cap(1);
        if (indexExpression.captureCount() > 1)
            suffixResult = indexExpression.cap(2);
        prefixResult = filename.mid(0, filename.indexOf(indexStringResult));
        indexResult = indexStringResult.toInt();
    }

    // fill results
    *index = indexResult;
    if (prefix)
        *prefix = prefixResult;
    if (indexString)
        *indexString = indexStringResult;
    if (suffix)
        *suffix = suffixResult;
}
} // namespace ImageFileNode
