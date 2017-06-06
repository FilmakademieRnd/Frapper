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
//! \file "ColorCorrectionNode.cpp"
//! \brief Implementation file for ColorCorrectionNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       06.08.2013 (last updated)
//!

#include "ColorCorrectionNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

#define STEPS 256

namespace ColorCorrectionNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the ColorCorrectionNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ColorCorrectionNode::ColorCorrectionNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot),
		m_pixelFormat(Ogre::PF_R8G8B8A8)
{
    // create compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "ColorCorrectionCompositor", 0);

    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }
	
	// create lut parameters
	m_lutR = new NumberParameter("lutR", Parameter::T_Float, 0.0);
	m_lutG = new NumberParameter("lutG", Parameter::T_Float, 0.0);
	m_lutB = new NumberParameter("lutB", Parameter::T_Float, 0.0);
	m_lutA = new NumberParameter("lutA", Parameter::T_Float, 0.0);

	Key startKey(0.0f, 0.0f);
	Key endKey(255.0f, 1.0f);
	
	m_lutR->addKey(startKey);
	m_lutR->addKey(endKey);
	m_lutG->addKey(startKey);
	m_lutG->addKey(endKey);
	m_lutB->addKey(startKey);
	m_lutB->addKey(endKey);
	m_lutA->addKey(startKey);
	m_lutA->addKey(endKey);

	parameterRoot->addParameter(m_lutR);
	parameterRoot->addParameter(m_lutG);
	parameterRoot->addParameter(m_lutB);
	parameterRoot->addParameter(m_lutA);

	// create lut texture
	const Ogre::String lutTextureName = createUniqueName("LutTexture" + getName());
	m_lutTexture = Ogre::TextureManager::getSingleton().createManual(lutTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, STEPS, 1, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	

    // set affections and callback functions
    addAffection("Input Map", m_outputImageName);
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Output Pixel Format", SLOT(changeOutputPixelFormat()));
	
	setProcessingFunction(m_lutR->getName(), SLOT(updateLutTexture()));
	setProcessingFunction(m_lutG->getName(), SLOT(updateLutTexture()));
	setProcessingFunction(m_lutB->getName(), SLOT(updateLutTexture()));
	setProcessingFunction(m_lutA->getName(), SLOT(updateLutTexture()));
}


//!
//! Destructor of the ColorCorrectionNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ColorCorrectionNode::~ColorCorrectionNode ()
{
	if(!m_lutTexture.isNull() && Ogre::TextureManager::getSingleton().resourceExists(m_lutTexture->getName()))
		Ogre::TextureManager::getSingleton().remove(m_lutTexture->getName());
}


///
/// Protected Functions
///


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void ColorCorrectionNode::processOutputImage ()
{
    // obtain input image
    Ogre::TexturePtr inputTexture = getTextureValue("Input Map");

    if (inputTexture.isNull()) {
        // disable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(false);
        
        // render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_defaultTexture);
        Log::warning("Not all input images connected.", "ColorCorrectionNode::processOutputImage");
    }
	else if (!m_renderTexture.isNull()) {
		//set texture name
		Ogre::MaterialPtr mat = m_compositor->getTechnique()->getOutputTargetPass()->getPass(0)->getMaterial();
        setTexture(mat, inputTexture, 0);
        setTexture(mat, m_lutTexture, 1);
        // resize render texture if necessary
        m_renderWidth = inputTexture->getWidth();
        m_renderHeight = inputTexture->getHeight();
        resizeRenderTexture(m_renderWidth, m_renderHeight, m_pixelFormat);

        //enable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(true);

        //render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        //Log::info(m_name + " rendered", "ColorCorrectionNode::processOutputImage");
        setValue(m_outputImageName, m_renderTexture);

		updateLutTexture();
    }
}

//!
//! Changes the output pixel format.
//!
void ColorCorrectionNode::changeOutputPixelFormat ()
{
    if (m_renderTexture.isNull())
        return;

    // obtain the operator parameter
    EnumerationParameter *outputFormatParameter = getEnumerationParameter("Output Pixel Format");
    if (outputFormatParameter) {
        // get the value of the currently selected item in the enumeration
        QString value = outputFormatParameter->getCurrentValue();
        if (!value.isNull()) {
            if (value == "Float16")
				m_pixelFormat = Ogre::PF_FLOAT16_RGBA;
            else if (value == "RGBA8")
				m_pixelFormat = Ogre::PF_R8G8B8A8;
        }
    }
    processOutputImage();
    setValue(m_outputImageName, m_renderTexture, true);
}

//!
//! Creates a lookup table as a texture based on the key curves.
//!
void ColorCorrectionNode::updateLutTexture ()
{
	Ogre::HardwarePixelBufferSharedPtr lutBuffer = m_lutTexture->getBuffer();
	lutBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
	const Ogre::PixelBox &pixelBoxLut = lutBuffer->getCurrentLock();
	float *lutFloatPtr = static_cast<float*>(pixelBoxLut.data);

	const float lastPosR = m_lutR->getLastKeyPos()/STEPS;
	const float lastPosG = m_lutG->getLastKeyPos()/STEPS;
	const float lastPosB = m_lutB->getLastKeyPos()/STEPS;
	const float lastPosA = m_lutA->getLastKeyPos()/STEPS;

	for (float i=0; i<STEPS; ++i) {
		*lutFloatPtr++ = m_lutR->getKeyValueTime(i*lastPosR).toFloat();
		*lutFloatPtr++ = m_lutG->getKeyValueTime(i*lastPosG).toFloat();
		*lutFloatPtr++ = m_lutB->getKeyValueTime(i*lastPosB).toFloat();
		*lutFloatPtr++ = m_lutA->getKeyValueTime(i*lastPosA).toFloat();
	}

	lutBuffer->unlock();
}


} // namespace ColorCorrectionNode
