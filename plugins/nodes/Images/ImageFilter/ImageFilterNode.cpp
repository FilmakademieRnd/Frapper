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
//! \file "ImageFilterNode.cpp"
//! \brief Implementation file for ImageFilterNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       28.04.2009 (last updated)
//!

#include "ImageFilterNode.h"
#include "NumberParameter.h"
#include "Log.h"
#include "OgreTools.h"

namespace ImageFilterNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(ImageFilterNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the ImageFilterNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ImageFilterNode::ImageFilterNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ImageNode(name, parameterRoot, true, "Filtered Image")
{
    // set affections and callback functions
    addAffection("Image to Filter", m_outputImageName);
    addAffection("Red", m_outputImageName);
    addAffection("Green", m_outputImageName);
    addAffection("Blue", m_outputImageName);
    addAffection("Alpha", m_outputImageName);
    addAffection("Color Correction > Reset", "Red");
    addAffection("Color Correction > Reset", "Green");
    addAffection("Color Correction > Reset", "Blue");
    addAffection("Color Correction > Reset", "Alpha");
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Red", SLOT(invalidateCache()));
    setChangeFunction("Green", SLOT(invalidateCache()));
    setChangeFunction("Blue", SLOT(invalidateCache()));
    setChangeFunction("Alpha", SLOT(invalidateCache()));
    setCommandFunction("Color Correction > Reset", SLOT(resetColorCorrection()));

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the ImageFilterNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ImageFilterNode::~ImageFilterNode ()
{
    DEC_INSTANCE_COUNTER
}


///
/// Private Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void ImageFilterNode::processOutputImage ()
{
    // obtain the image to filter
    Ogre::TexturePtr inputTexture = getTextureValue("Image to Filter");
    if (inputTexture.isNull()) {
        Log::warning("No input image connected.", "ImageFilterNode::processOutputImage");
        return;
    }

    //// check if the node should be evaluated and if not just pass the input texture on
    //if (!m_evaluate) {
    //    outputImageParameter->setTexture(inputTexture);
    //    outputImageParameter->setDirty(false);
    //    return;
    //}

    // get index of current frame
    int index = m_timeParameter->getValue().toInt();

    // clear the image cache if it has become invalid
    if (m_cacheInvalid) {
        clearImageCache();
        m_cacheInvalid = false;
    }

    // load the image from file or from cache (if activated)
    Ogre::TexturePtr texture;
    if ( m_cacheEnabled && isCached(index)) {
        // load the image from cache
        texture = getCachedImage(index);
    } else {
        // create the image by processing the input image
        size_t width = inputTexture->getWidth();
        size_t height = inputTexture->getHeight();
        size_t depth = inputTexture->getDepth();
        Ogre::PixelFormat pixelFormat = inputTexture->getFormat();

        size_t memorySize = Ogre::PixelUtil::getMemorySize(width, height, depth, pixelFormat);
        Ogre::uchar *imageData = OGRE_ALLOC_T(Ogre::uchar, memorySize, Ogre::MEMCATEGORY_GENERAL);
        Ogre::Image image;
        image.loadDynamicImage(imageData, width, height, depth, pixelFormat, true);

        // get values of affecting parameters
        int red = 255 * (float) getIntValue("Red") / 100;
        int green = 255 * (float) getIntValue("Green") / 100;
        int blue = 255 * (float) getIntValue("Blue") / 100;
        int alpha = 255 * (float) getIntValue("Alpha") / 100;

        // copy the input image to the output texture's buffer
		OgreTools::HardwareBufferLocker hbl( inputTexture->getBuffer(),  Ogre::HardwareBuffer::HBL_READ_ONLY);
        const Ogre::PixelBox &inputPixelBox = hbl.getCurrentLock();
        const Ogre::PixelBox &outputPixelBox = image.getPixelBox();
        Ogre::uint8 *inputPixelData = static_cast<Ogre::uint8 *>(inputPixelBox.data);
        Ogre::uint8 *outputPixelData = static_cast<Ogre::uint8 *>(outputPixelBox.data);
        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < width; ++x) {
                // input
                Ogre::uint8 b = *inputPixelData++;
                Ogre::uint8 g = *inputPixelData++;
                Ogre::uint8 r = *inputPixelData++;
                Ogre::uint8 a = *inputPixelData++;

                // processing
                r = qMin(qMax(r + red, 0), 255);
                g = qMin(qMax(g + green, 0), 255);
                b = qMin(qMax(b + blue, 0), 255);
                a = qMin(qMax(a + alpha, 0), 255);

                // output
                *outputPixelData++ = b;
                *outputPixelData++ = g;
                *outputPixelData++ = r;
                *outputPixelData++ = a;
            }
		}

		texture = createTextureFromImage(image);

		// write file to cache
		if (m_cacheEnabled)
			if( m_useGPUCache) // depending on current cache strategy, either cache the image or the texture
				cacheTexture( index, texture);
			else
				cacheImage( index, image);
    }

	setOutputImage(texture);
}


//!
//! Resets the color correction parameters to their default values.
//!
void ImageFilterNode::resetColorCorrection ()
{
    Parameter *resetColorCorrectionParameter = getParameter("Color Correction > Reset");
    if (!resetColorCorrectionParameter) {
        Log::error("Could not obtain reset parameter.", "ImageFilterNode::resetColorCorrection");
        return;
    }

    Parameter::List affectedParameters = resetColorCorrectionParameter->getAffectedParameters();
    for (int i = 0; i < affectedParameters.size(); ++i)
        affectedParameters[i]->reset();

    if (affectedParameters.size() > 0) {
        Parameter *parameter = dynamic_cast<Parameter *>(affectedParameters[0]);
        parameter->propagateDirty();
    }
}


//!
//! Marks the cache as being invalid so that it is cleared the next time
//! the output image is processed.
//!
void ImageFilterNode::invalidateCache ()
{
    m_cacheInvalid = true;
}

} // namespace ImageFilterNode
