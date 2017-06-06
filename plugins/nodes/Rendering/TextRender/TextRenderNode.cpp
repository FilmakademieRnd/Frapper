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
//! \file "TextRender.cpp"
//! \brief Implementation file for TextRender class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       07.02.2012 (created)
//! \date       07.02.2012 (last updated)
//!

#include "TextRenderNode.h"
#include "OgreManager.h"
#include "Node.h"
#include <QPainter>

namespace TextRenderNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the TextRender class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TextRenderNode::TextRenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ImageNode( name, parameterRoot)
{

    // Text input parameter is self-evaluating, thus new incoming 
    // text values are rendered to the output image immediately
    setProcessingFunction("Text", SLOT(updateOutputImage()));

    setChangeFunction("Position X", SLOT(updateOutputImage()));
    setChangeFunction("Position Y", SLOT(updateOutputImage()));

    // Change of resolution parameters triggers resolutionChanged() method
    setChangeFunction("Resolution > Width", SLOT(resolutionChanged()));
    setChangeFunction("Resolution > Height", SLOT(resolutionChanged()));
    
    // Change of font parameters triggers fontChanged() method
    setChangeFunction("Font Size", SLOT(fontChanged()));
    setChangeFunction("Font Name", SLOT(fontChanged()));
    setChangeFunction("Font Color", SLOT(fontChanged()));
    setChangeFunction("Bold", SLOT(fontChanged()));

    // Call change functions to intialize member variables
    resolutionChanged();
    fontChanged();

}

void TextRenderNode::updateOutputImage()
{
    if(!m_outputImage.isNull()) 
    {

        // Read Text input and position
        QString text = this->getStringValue("Text");
        int xPos = this->getIntValue("Position X");
        int yPos = this->getIntValue("Position Y");

        // clear image
        m_qImage.fill(Qt::transparent);

        // creating a new painter on each frame is recommended, 
        // see http://qt-project.org/doc/qt-4.8/qpainter.html#QPainter-2
        QPainter painter(&m_qImage);

        // setup painter
        painter.setFont(m_font);
        painter.setPen(m_color);

        // painter draws text to image
        painter.drawText(xPos, yPos+m_fontsize, text);

        // write pixel values to ogre texture
        Ogre::HardwarePixelBufferSharedPtr tgtBuffer = m_outputImage->getBuffer();
        tgtBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

        const Ogre::PixelBox &pixBox = tgtBuffer->getCurrentLock();

        uchar *tgtData = static_cast<uchar *>(pixBox.data);
        uchar *srcData = static_cast<uchar *>(m_qImage.bits());

        //copy data
        memcpy(tgtData, srcData, m_qImage.byteCount());

        tgtBuffer->unlock();
    }
}

//!
//! Destructor of the TextRenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TextRenderNode::~TextRenderNode ()
{
}

void TextRenderNode::resolutionChanged()
{
    // Read new resolution values
    m_width = getUnsignedIntValue("Resolution > Width");
    m_height = getUnsignedIntValue("Resolution > Height");

    // Resize the qimage
    m_qImage = QImage( QSize(m_width, m_height), QImage::Format_ARGB32);
    

    // destroy existing output texture and re-create it
    m_outputImage = getTextureValue(m_outputImageName);
    if (!m_outputImage.isNull()) {
        Ogre::TextureManager::getSingletonPtr()->remove( m_outputImage->getHandle());
        m_outputImage.setNull();
    }

    // create the output texture
    m_outputImage = Ogre::TextureManager::getSingletonPtr()->createManual( 
        QString(m_name + "OutputTexture").toStdString(),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
        Ogre::TEX_TYPE_2D, 
        m_width, m_height, 0, Ogre::PF_A8B8G8R8, Ogre::TU_DYNAMIC_WRITE_ONLY);

    setValue(m_outputImageName, m_outputImage, true);
}

void TextRenderNode::fontChanged()
{
    // Read new font values
    m_color = this->getColorValue("Font Color");
    m_fontsize = this->getIntValue("Font Size");
    QString fontfamily = this->getStringValue("Font Name");
    bool bold = this->getBoolValue("Bold");

    // update qfont
    m_font.setFamily(fontfamily);
    m_font.setPixelSize( m_fontsize);
    m_font.setBold(bold);
}

} // namespace TextRenderNode 
