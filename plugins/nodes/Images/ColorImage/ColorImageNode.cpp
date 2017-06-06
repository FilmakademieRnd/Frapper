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
//! \file "ColorImageNode.cpp"
//! \brief Implementation file for ColorImageNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       28.04.2009 (last updated)
//!

#include "ColorImageNode.h"
#include "NumberParameter.h"
#include "Log.h"
#include <QtCore/QFile>

namespace ColorImageNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(ColorImageNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the ColorImageNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ColorImageNode::ColorImageNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ImageNode(name, parameterRoot, false)
{
    // get output image parameter
    Parameter *outputImageParameter = getParameter(m_outputImageName);
    if (outputImageParameter) {
        // set up parameter affections
        outputImageParameter->addAffectingParameter(getParameter("Width"));
        outputImageParameter->addAffectingParameter(getParameter("Height"));
        outputImageParameter->addAffectingParameter(getParameter("Color"));
        outputImageParameter->addAffectingParameter(getParameter("Alpha"));
        // set the processing function for the output image parameter
        outputImageParameter->setProcessingFunction(SLOT(createColorImage()));
    } else
        Log::error("Could not obtain output image parameter.", "ColorImageNode::ColorImageNode");

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the ColorImageNode class.
//!
ColorImageNode::~ColorImageNode ()
{
    DEC_INSTANCE_COUNTER
}


///
/// Private Slots
///


//!
//! Creates the color image for the output image parameter.
//!
//! \return True if creating the color image succeeded, otherwise False.
//!
void ColorImageNode::createColorImage ()
{
    // get parameter values needed for processing the output image
    unsigned int width = getUnsignedIntValue("Width");
    unsigned int height = getUnsignedIntValue("Height");
    QColor color = getColorValue("Color");
    Ogre::uint8 alpha = getUnsignedIntValue("Alpha");

    // make sure the width and height is valid and the alpha value doesn't exceed 255
    if (width == 0 || height == 0) {
        Log::error("The width or height for the requested image is 0.", "ColorImageNode::createColorImage");
        return;
    }
    if (alpha > 255)
        alpha = 255;

    //Log::debug(QString("Requested image resolution and color: %1x%2, %3").arg(width).arg(height).arg(color.name()), "ColorImageNode::createColorImage");

    // create an empty image with the requested resolution
    Ogre::PixelFormat pixelFormat = Ogre::PF_A8R8G8B8;
    size_t memorySize = Ogre::PixelUtil::getMemorySize(width, height, 1, pixelFormat);
    Ogre::uchar *imageData = OGRE_ALLOC_T(Ogre::uchar, memorySize, Ogre::MEMCATEGORY_GENERAL);
    Ogre::Image image;
    image.loadDynamicImage(imageData, width, height, 1, pixelFormat, true);

	
    // store the color values to use for the image
    Ogre::uint8 r = color.red();
    Ogre::uint8 g = color.green();
    Ogre::uint8 b = color.blue();
    Ogre::uint8 a = alpha;

    // fill the image with the constant color
    const Ogre::PixelBox &pixelBox = image.getPixelBox();
    Ogre::uint8 *pixelData = static_cast<Ogre::uint8 *>(pixelBox.data);
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x) {
            *pixelData++ = b;
            *pixelData++ = g;
            *pixelData++ = r;
            *pixelData++ = a;
        }

    setOutputImage(image);

}
} // namespace ColorImageNode
