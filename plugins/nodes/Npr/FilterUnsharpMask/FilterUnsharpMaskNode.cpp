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
//! \file "FilterUnsharpMaskNode.cpp"
//! \brief Implementation file for FilterUnsharpMaskNode class.
//!
//! \author     Thomas Luft <thomas.luft@web.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "FilterUnsharpMaskNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace FilterUnsharpMaskNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the FilterUnsharpMaskNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
FilterUnsharpMaskNode::FilterUnsharpMaskNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot)
{
    // create compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "UnsharpMaskCompositor", 0);
    
    // add compositor listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    // set affections and callback functions
    addAffection("Input Map", m_outputImageName);
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Extent", SIGNAL(triggerRedraw()));
    setCommandFunction("Polish > Reload Shader", SLOT(reloadShader()));

}


//!
//! Destructor of the FilterUnsharpMaskNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
FilterUnsharpMaskNode::~FilterUnsharpMaskNode ()
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
void FilterUnsharpMaskNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    Ogre::TexturePtr inputTexture = getTextureValue("Input Map");
    if (inputTexture.isNull())
        return;

    //downscaled pass
    if (pass_id == 0000)
    {
        //set texture name
        setTexture(mat, inputTexture, 0);
    }

    //smooth downscaled pass
    if (pass_id == 0001)
    {
        //set gpu parameter
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);
        if (!fpParams.isNull())
        {
            static const double downscaleFactor = 0.125;
            setShaderParameter(fpParams, "pixelSize", Ogre::Vector4(1.0 / (double)(downscaleFactor * getWidth()), 1.0 / (double)(downscaleFactor * getHeight()), 0.0, 0.0));
            setShaderParameter(fpParams, "domainRadius", (Ogre::Real)(getDoubleValue("Extent") / 10.0 + 1.0));
        }
    }

    //indication pass
    if (pass_id == 0002)
    {
        //set gpu parameter
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);
        if (!fpParams.isNull())
        {
            setShaderParameter(fpParams, "pixelSize", Ogre::Vector4(1.0 / (double)getWidth(), 1.0 / (double)getHeight(), 0.0, 0.0));
        }

        //set texture name
        setTexture(mat, inputTexture, 0);
    }
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void FilterUnsharpMaskNode::processOutputImage ()
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
        resizeRenderTexture(width, height, Ogre::PF_R8G8B8A8);
        
        //enable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(true);

        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_renderTexture);
    }
}

//!
//! Reloads the shader.
//!
void FilterUnsharpMaskNode::reloadShader ()
{
    Ogre::MaterialPtr  mat = Ogre::MaterialManager::getSingleton().getByName("UnsharpMaskMaterial");
    if (!mat.isNull())
    {
        Ogre::GpuProgramPtr gpuPrg = mat->getTechnique(0)->getPass(0)->getFragmentProgram();
        if (!gpuPrg.isNull())
        {
            gpuPrg->reload();
            mat->getTechnique(0)->getPass(0)->setFragmentProgram(gpuPrg->getName(), true);
            Log::info("shader reloaded", "FilterUnsharpMaskNode::reloadShader");
            processOutputImage();
        }
    }
}

} // namespace FilterUnsharpMaskNode 
