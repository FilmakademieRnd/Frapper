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
//! \file "TridelityNode.cpp"
//! \brief Implementation file for TridelityNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       16.10.2009 (last updated)
//!

#include "TridelityNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace TridelityNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

const int TridelityNode::lookupX = 5;
const int TridelityNode::lookupY = 15;

// lookup table screentype 0
const int TridelityNode::lookupML[5][15] = 
{{ 0, 0, 4, 3, 3, 2, 1, 1, 0, 4, 4, 3, 2, 2, 1 },
 { 3, 3, 2, 1, 1, 0, 4, 4, 3, 2, 2, 1, 0, 0, 4 },
 { 1, 1, 0, 4, 4, 3, 2, 2, 1, 0, 0, 4, 3, 3, 2 },
 { 4, 4, 3, 2, 2, 1, 0, 0, 4, 3, 3, 2, 1, 1, 0 },
 { 2, 2, 1, 0, 0, 4, 3, 3, 2, 1, 1, 0, 4, 4, 3 }};

// lookup table screentype 1
const int TridelityNode::lookupMV[5][15] = 
{{ 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3 },
 { 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0 },
 { 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2 },
 { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 },
 { 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1 }};

//!
//! Constructor of the TridelityNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TridelityNode::TridelityNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot),
    m_screenType(0)
{
    // create compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "TridelityNodeCompositor" );

    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    // set affections and callback functions
    this->addAffection("Input Map 0", m_outputImageName);
    this->addAffection("Input Map 1", m_outputImageName);
    this->addAffection("Input Map 2", m_outputImageName);
    this->addAffection("Input Map 3", m_outputImageName);
    this->addAffection("Input Map 4", m_outputImageName);

    m_resX = this->getNumberParameter("Resolution X");
    m_resX->setChangeFunction(SLOT(processOutputImage()));

    m_resY = this->getNumberParameter("Resolution Y");
    m_resY->setChangeFunction(SLOT(processOutputImage()));

    this->setChangeFunction("Screen Type", SLOT(processOutputImage()));
    this->setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));

}


//!
//! Destructor of the TridelityNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TridelityNode::~TridelityNode ()
{
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "TridelityNodeCompositor", false );
}


///
/// Protected Functions
///

//!
//! Callback when instance of this class is registered as Ogre::CompositorListener.
//! 
//! \param pass_id Id to identifiy current compositor pass.
//! \param mat Material this pass is currently using.
//!
void TridelityNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void TridelityNode::processOutputImage()
{
    // resize render lookupTexture if necessary
    m_renderWidth  = m_resX->getValue().toInt();
    m_renderHeight = m_resY->getValue().toInt();
    m_screenType   = getEnumerationParameter("Screen Type")->getCurrentIndex();

    if (m_compositor)
        m_compositor->setEnabled( false);

    SetupTextures();

    if( m_renderWidth  < 16 ) m_renderWidth  = 16;
    if( m_renderHeight < 16 ) m_renderHeight = 16;

    resizeRenderTexture( m_renderWidth, m_renderHeight );

    //enable compositor (now that the input lookupTexture name was set)
    if (m_compositor)
        m_compositor->setEnabled( true);

    //render and set output
    Log::info(m_name + " rendered", "TridelityNode::processOutputImage");
	setValue( m_outputImageName, m_renderTexture );
	getParameter(m_outputImageName)->setDirty(false);

	emit viewNodeUpdated(); 
}


//!
//! Redraw of ogre scene has been triggered.
//!
void TridelityNode::redrawTriggered ()
{
	RenderNode::redrawTriggered();
}

void TridelityNode::SetupTextures()
{
    Ogre::TexturePtr inputTexture[6];
    inputTexture[0] = getTextureValue("Input Map 0", true);

    if( !inputTexture[0].isNull() )
    {
        if( m_renderWidth == 0 )
        {
            m_renderWidth = inputTexture[0]->getWidth();
        }
        if( m_renderHeight == 0 )
        {
            m_renderHeight = inputTexture[0]->getHeight();
        }
    }

    inputTexture[1] = getTextureValue("Input Map 1", true);
    inputTexture[2] = getTextureValue("Input Map 2", true);
    inputTexture[3] = getTextureValue("Input Map 3", true);
    inputTexture[4] = getTextureValue("Input Map 4", true);
    inputTexture[5] = CreateLookupTexture();

    Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName("TridelityNode/TridelityNodeMaterial");

    if( !materialPtr.isNull() )
    {
        // set input textures
        for( int i=0; i < 6; i++ )
        {
            if( !inputTexture[i].isNull() )
               setTexture( materialPtr, inputTexture[i], i);
            else
               setTexture( materialPtr, m_defaultTexture, i);
        }
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(materialPtr);
        setShaderParameter( fpParams, "screen_type", m_screenType );
    }   
}

// Create the lookupTexture
Ogre::TexturePtr TridelityNode::CreateLookupTexture()
{
    if( m_renderWidth < 16 || m_renderHeight < 16)
        return m_defaultTexture;

    Ogre::TextureManager& tm = Ogre::TextureManager::getSingleton();

    // delete old lookup texture
    if( !m_lookupTexture.isNull() ) 
    {
        m_lookupTexture->unload();
        m_lookupTexture.setNull();
        tm.unload("LookupTex"+m_name.toStdString());
        tm.remove("LookupTex"+m_name.toStdString());
    }

    m_lookupTexture = Ogre::TextureManager::getSingleton().createManual(
            "LookupTex"+m_name.toStdString(),                    // name
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D,              // type
            m_renderWidth, m_renderHeight,  // width & height
            0,                              // number of mipmaps
            Ogre::PF_FLOAT32_R,             // pixel format
            Ogre::TU_STATIC );

    // Get the pixel buffer
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_lookupTexture->getBuffer();

    // Lock the pixel buffer and get a pixel box
    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
    const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();

    float* pDest = static_cast<float*>(pixelBox.data);

    // Fill in the lookup table. 
    if (m_screenType == 0) {
        for(size_t y = 0; y < m_renderHeight; y++) {
            for (size_t x = 0; x < m_renderWidth; x++) {
                *pDest++ = (float) lookupML[ x%lookupX ][ y%lookupY ];
            }
        }
    } else if (m_screenType == 1) {
        for(size_t y = 0; y < m_renderHeight; y++) {
            for (size_t x = 0; x < m_renderWidth; x++) {
                *pDest++ = (float) lookupMV[ x%lookupX ][ y%lookupY ];
            }
        }
    }

    // Unlock the pixel buffer
    pixelBuffer->unlock();
    return m_lookupTexture;
}
} // namespace TridelityNode
