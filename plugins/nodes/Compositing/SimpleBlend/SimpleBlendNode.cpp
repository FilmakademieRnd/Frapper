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
//! \file "SimpleBlendNode.cpp"
//! \brief Implementation file for SimpleBlendNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       16.10.2009 (last updated)
//!

#include "SimpleBlendNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace SimpleBlendNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the SimpleBlendNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
SimpleBlendNode::SimpleBlendNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot),
		m_pixelFormat(Ogre::PF_R8G8B8A8),
		m_operationValue(0)
{
    // create compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "SimpleBlendCompositor", 0);

    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    // set affections and callback functions
    addAffection("Input Map One", m_outputImageName);
    addAffection("Input Map Two", m_outputImageName);
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Operator", SLOT(changeOperationValue()));
    setChangeFunction("Output Pixel Format", SLOT(changeOutputPixelFormat()));
	setProcessingFunction("Linear Blend Factor", SIGNAL(triggerRedraw()));
}


//!
//! Destructor of the SimpleBlendNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
SimpleBlendNode::~SimpleBlendNode ()
{
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
void SimpleBlendNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	Ogre::TexturePtr inputTextureOne = getTextureValue("Input Map One");
    Ogre::TexturePtr inputTextureTwo = getTextureValue("Input Map Two");

	if (inputTextureOne.isNull() || inputTextureTwo.isNull())
		return;

    // Blend pass
    if (pass_id == 0)
    {	
	    Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);
        const Ogre::Real blendFactor = getFloatValue("Linear Blend Factor") / 100.f;
 
        //set shader parameters
	    setShaderParameter(fpParams, "blendFactor", blendFactor);
		setShaderParameter(fpParams, "op",  static_cast<Ogre::Real>(m_operationValue));
	}
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void SimpleBlendNode::processOutputImage ()
{
    // obtain input image
    Ogre::TexturePtr inputTextureOne = getTextureValue("Input Map One");
    Ogre::TexturePtr inputTextureTwo = getTextureValue("Input Map Two");

    if (inputTextureOne.isNull() && inputTextureTwo.isNull()) {
        // disable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(false);
        
        // render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_defaultTexture);
        Log::warning("Not all input images connected.", "SimpleBlendNode::processOutputImage");
    }
    else if (inputTextureTwo.isNull()) {
        setValue("Image", inputTextureOne);
        return;
    }
    else if (inputTextureOne.isNull()) {
        setValue("Image", inputTextureTwo);
        return;
    }
	else if (!m_renderTexture.isNull()) {
		Ogre::MaterialPtr mat = m_compositor->getTechnique()->getOutputTargetPass()->getPass(0)->getMaterial();
		//set texture name
        setTexture(mat, inputTextureOne, 0);
        setTexture(mat, inputTextureTwo, 1);
        // resize render texture if necessary
        m_renderWidth = inputTextureOne->getWidth();
        m_renderHeight = inputTextureOne->getHeight();
        resizeRenderTexture(m_renderWidth, m_renderHeight, m_pixelFormat);

        //enable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(true);

        //render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        //Log::info(m_name + " rendered", "SimpleBlendNode::processOutputImage");
        setValue(m_outputImageName, m_renderTexture);
    }
}

//!
//! Changes the output pixel format.
//!
void SimpleBlendNode::changeOutputPixelFormat ()
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
//! Changes the shader operation value.
//!
void SimpleBlendNode::changeOperationValue ()
{
	EnumerationParameter *parameter = dynamic_cast<EnumerationParameter *>(sender());
	if( parameter )
	{
		m_operationValue = parameter->getCurrentIndex();
		triggerRedraw();
	}
}
} // namespace SimpleBlendNode
