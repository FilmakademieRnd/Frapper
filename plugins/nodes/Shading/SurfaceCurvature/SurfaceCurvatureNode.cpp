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
//! \file "SurfaceCurvatureNode.cpp"
//! \brief Implementation file for SurfaceCurvatureNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       02.11.2009 (last updated)
//!

#include "SurfaceCurvatureNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace SurfaceCurvatureNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the SurfaceCurvatureNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
SurfaceCurvatureNode::SurfaceCurvatureNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot)
{
    // create second compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "SurfaceCurvatureCompositor", 0);

    // add listener to first compositor
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    // set the affections and callback functions
    addAffection("Input Map Normal", m_outputImageName);
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
    setChangeFunction("Magnitude", SIGNAL(triggerRedraw()));
    setChangeFunction("Show Color", SIGNAL(triggerRedraw()));
}


//!
//! Destructor of the SurfaceCurvatureNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
SurfaceCurvatureNode::~SurfaceCurvatureNode ()
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
void SurfaceCurvatureNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    Ogre::TexturePtr inputTextureNormal = getTextureValue("Input Map Normal");

    if (inputTextureNormal.isNull())
        return;

    // Data hessian map pass
    if (pass_id == 0)
    {
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);

        double magnitude = getDoubleValue("Magnitude");
        bool showcolor = getBoolValue("Show Color");

        //set shader parameters
        setShaderParameter(fpParams, "width", (Ogre::Real) inputTextureNormal->getWidth());
        setShaderParameter(fpParams, "height", (Ogre::Real) inputTextureNormal->getHeight());
        setShaderParameter(fpParams, "cmax", (Ogre::Real) magnitude);
        setShaderParameter(fpParams, "hsv", (Ogre::Real) showcolor);

        //set texture name
        setTexture(mat, inputTextureNormal, 0);
    }
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void SurfaceCurvatureNode::processOutputImage ()
{
    // obtain input image
    Ogre::TexturePtr inputTextureNormal = getTextureValue("Input Map Normal");

    // check input
    if (inputTextureNormal.isNull()) {
        //disable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(false);

        //render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_defaultTexture);
        Log::warning("Not all input images connected.", "SurfaceCurvatureNode::processOutputImage");
    }
    else {
        // resize render texture if necessary
        m_renderWidth = inputTextureNormal->getWidth();
        m_renderHeight = inputTextureNormal->getHeight();
        resizeRenderTexture(m_renderWidth, m_renderHeight);

        //enable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(true);

        //render and set output
        setValue(m_outputImageName, m_renderTexture);
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_renderTexture);
    }
}

} // namespace SurfaceCurvatureNode 

