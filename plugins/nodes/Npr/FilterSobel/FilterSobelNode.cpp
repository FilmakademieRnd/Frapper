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
//! \file "FilterSobelNode.cpp"
//! \brief Implementation file for FilterSobelNode class.
//!
//! \author     Thomas Luft <thomas.luft@web.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann sspielma@filmakademie.de>
//! \version    1.0
//! \date       19.11.2012 (last updated)
//!


#include "FilterSobelNode.h"
#include "ImageNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace FilterSobelNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the FilterSobelNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
FilterSobelNode::FilterSobelNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot)
{
    // create compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "SobelCompositor", 0);
    
    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    addAffection("Input Map", m_outputImageName);
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Monochrome", SIGNAL(triggerRedraw()));
    setChangeFunction("Extent", SIGNAL(triggerRedraw()));
    setChangeFunction("Threshold", SIGNAL(triggerRedraw()));
}


//!
//! Destructor of the FilterSobelNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
FilterSobelNode::~FilterSobelNode ()
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
void FilterSobelNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    Ogre::TexturePtr inputTexture = getTextureValue("Input Map");
    if (inputTexture.isNull())
        return;
    
    // Smooth horizontal
    if (pass_id == 0000)
    {
        //set shader parameters
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);
        if (!fpParams.isNull())
        {
            setShaderParameter(fpParams, "pixelSize", Ogre::Vector4(1.0 / getWidth(), 1.0 / getHeight(), (double)rand()/(double)RAND_MAX, (double)rand()/(double)RAND_MAX));
            setShaderParameter(fpParams, "domainRadius", (Ogre::Real) (getDoubleValue("Extent") / 10.0));
        }

        //set texture name
        setTexture(mat, inputTexture, 0);
    }

    // Smooth vertical
    if (pass_id == 0001)
    {
        //set shader parameters
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);
        if (!fpParams.isNull())
        {
            setShaderParameter(fpParams, "pixelSize", Ogre::Vector4(1.0 / (double) getWidth(), 1.0 / (double) getHeight(), (double)rand()/(double)RAND_MAX, (double)rand()/(double)RAND_MAX));
            setShaderParameter(fpParams, "domainRadius", (Ogre::Real) (getDoubleValue("Extent") / 10.0));
        }
    }

    // Sobel
    if (pass_id == 0002)
    {
        //set shader parameters
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);
        if (!fpParams.isNull())
        {
            setShaderParameter(fpParams, "pixelSize", Ogre::Vector4(1.0 / (double) getWidth(), 1.0 / (double) getHeight(), (double)rand()/(double)RAND_MAX, (double)rand()/(double)RAND_MAX));
            setShaderParameter(fpParams, "threshold", (Ogre::Real) getDoubleValue("Threshold"));
            setShaderParameter(fpParams, "monochrome", (int) getBoolValue("Monochrome"));
        }
    }
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void FilterSobelNode::processOutputImage ()
{
     // obtain input image
    Ogre::TexturePtr inputTexture = getTextureValue("Input Map");
    if (inputTexture.isNull()) {
        //disable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(false);

        //render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_defaultTexture);
        Log::warning("No input image connected.", "FilterSobelNode::processOutputImage");
    }
    else if (!m_renderTexture.isNull()) {
        //resize render texture
        size_t width = inputTexture->getWidth();
        size_t height = inputTexture->getHeight();
		//resizeRenderTexture(width, height, Ogre::PF_FLOAT32_RGBA);

		///////////////////////////////////////////////////////////
		// save the compositor name and destroy compositor
		m_compositor->setEnabled(false);
		Ogre::Compositor *compositor = m_compositor->getCompositor();
		Ogre::String compositorName;
		if (compositor) {
			compositorName = compositor->getName();
			m_compositor->removeListener(this);
			Ogre::CompositorManager::getSingleton().removeCompositorChain(m_viewport);		
			m_compositor = 0;
		}

		RenderNode::resizeRenderTexture(width, height, Ogre::PF_FLOAT32_RGBA, 0);

		// create the new compositor with old name and location
		m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, compositorName, 0);
		if (m_compositor) {
			m_compositor->addListener(this);
			m_compositor->setEnabled(true);
		}

		RenderNode::redrawTriggered();


		///////////////////////////////////////////////////////
        
        //////enable compositor (now that the input texture name was set)
        ////if (m_compositor)
        ////    m_compositor->setEnabled(true);

        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_renderTexture);
    }
}

} // namespace FilterSobelNode 
