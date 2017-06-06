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
//! \file "NormalFlattenNode.cpp"
//! \brief Implementation file for NormalFlattenNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       16.10.2009 (last updated)
//!

#include "NormalFlattenNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace NormalFlattenNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the NormalFlattenNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
NormalFlattenNode::NormalFlattenNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot)
{
    // create compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "NormalFlatteningCompositor", 0);

    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    // set affections and callback functions
    addAffection("Normal Map", m_outputImageName);
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Flattening Factor", SIGNAL(triggerRedraw())); 
}


//!
//! Destructor of the NormalFlattenNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
NormalFlattenNode::~NormalFlattenNode ()
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
void NormalFlattenNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	Ogre::TexturePtr inputTextureOne = getTextureValue("Normal Map");

	if (inputTextureOne.isNull())
		return;

    // Blend pass
    if (pass_id == 0)
    {
        //set shader parameters
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);
	    double blendFactor = getDoubleValue("Flattening Factor");
	    setShaderParameter(fpParams, "flatteningFactor", (float)blendFactor);

        //set texture name
        setTexture(mat, inputTextureOne, 0);
	}
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void NormalFlattenNode::processOutputImage ()
{
    // obtain input image
    Ogre::TexturePtr inputTextureOne = getTextureValue("Normal Map");

    if (inputTextureOne.isNull()) {
        // disable compositor (now that the input texture name was set)
        m_compositor->setEnabled(false);
        // render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        Log::info(m_name + " rendered", "NormalFlattenNode::processOutputImage");
        setValue("Image", m_defaultTexture);
        Log::warning("Not all input images connected.", "NormalFlattenNode::processOutputImage");
    }
    else if (!m_renderTexture.isNull()) {

		// restize render texture
        size_t width = inputTextureOne->getWidth();
        size_t height = inputTextureOne->getHeight();
        m_renderTexture->setWidth(width);
        m_renderTexture->setHeight(height);

        // enable compositor (now that the input texture name was set)
        m_compositor->setEnabled(true);

        // render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        Log::info(m_name + " rendered", "NormalFlattenNode::processOutputImage");
        setValue("Image", m_renderTexture);
    }
}


//!
//! Redraw of ogre scene has been triggered.
//!
void NormalFlattenNode::redrawTriggered ()
{
	if (m_renderTexture.isNull())
		return;

    Ogre::RenderTexture *renderTarget = m_renderTexture->getBuffer()->getRenderTarget();

    if (renderTarget)
        renderTarget->update();

}

} // namespace NormalFlattenNode 
