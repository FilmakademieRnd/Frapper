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
//! \file "AmbientOcclusionNode.cpp"
//! \brief Implementation file for AmbientOcclusionNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       23.10.2009 (last updated)
//!

#include "AmbientOcclusionNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace AmbientOcclusionNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the AqtBlendNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
AmbientOcclusionNode::AmbientOcclusionNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot)
{
    // create compositor
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "AmbientOcclusionCompositor", 0);

    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    addAffection("Normal Map", m_outputImageName);
    addAffection("Depth Map", m_outputImageName);
    addAffection("Radius", m_outputImageName);
    addAffection("Contrast", m_outputImageName);
	addAffection("Far Clipping Plane", m_outputImageName);
    addAffection("Far Corner", m_outputImageName);
    addAffection("Screen Space Projection Matix(4x4)", m_outputImageName);
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));
}


//!
//! Destructor of the AqtBlendNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
AmbientOcclusionNode::~AmbientOcclusionNode ()
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
void AmbientOcclusionNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    Ogre::TexturePtr inputTextureNormal = getTextureValue("Normal Map");
    Ogre::TexturePtr inputTextureDepth = getTextureValue("Depth Map");

    NumberParameter *farPlaneParameter = getNumberParameter("Far Clipping Plane");
    Parameter *farCornerParameter = getParameter("Far Corner");
    Parameter *MatrixParameter = getParameter("Screen Space Projection Matix(4x4)");

    if (inputTextureNormal.isNull() ||
        inputTextureDepth.isNull() ||
        !farCornerParameter->isConnected() ||
        !farPlaneParameter->isConnected() ||
        !MatrixParameter->isConnected()) {

            Log::warning("Not all inputs connected.", "AmbientOcclusionCompositorListener::notifyMaterialRender");
            return;
    }

    // ssao pass
    if (pass_id == 0)
    {
		Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat, SH_Frag);
		Ogre::GpuProgramParametersSharedPtr vpParams = getShaderParameters(mat, SH_Vert);

        if (fpParams.isNull())
            return;

        QVariantList ptMatList = getValue("Screen Space Projection Matix(4x4)", true).toList();
        Ogre::Vector3 farCorner = getVectorValue("Far Corner");
        float radius = getDoubleValue("Radius");
        float contrast = getDoubleValue("Contrast");
        float farPlane = getDoubleValue("Far Clipping Plane");

        Ogre::Matrix4 ptMat = Ogre::Matrix4(
			ptMatList[ 0].toDouble(),
            ptMatList[ 1].toDouble(),
            ptMatList[ 2].toDouble(),
            ptMatList[ 3].toDouble(),
            ptMatList[ 4].toDouble(),
            ptMatList[ 5].toDouble(),
            ptMatList[ 6].toDouble(),
            ptMatList[ 7].toDouble(),
            ptMatList[ 8].toDouble(),
            ptMatList[ 9].toDouble(),
            ptMatList[10].toDouble(),
            ptMatList[11].toDouble(),
            ptMatList[12].toDouble(),
            ptMatList[13].toDouble(),
            ptMatList[14].toDouble(),
            ptMatList[15].toDouble());

        // set shader parameters
		setShaderParameter(vpParams, "farCorner", farCorner);
        setShaderParameter(fpParams, "ptMat", ptMat);
        setShaderParameter(fpParams, "radius", radius);
        setShaderParameter(fpParams, "contrast", contrast);
        setShaderParameter(fpParams, "farPlane", farPlane);

        setTexture(mat, inputTextureNormal, 0);
        setTexture(mat, inputTextureDepth, 1);
    }
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void AmbientOcclusionNode::processOutputImage ()
{
    // obtain input image
    Ogre::TexturePtr inputTextureNormal = getTextureValue("Normal Map");
    Ogre::TexturePtr inputTextureDepth = getTextureValue("Depth Map");

	NumberParameter *farPlaneParameter = getNumberParameter("Far Clipping Plane");
	Parameter *farCornerParameter = getParameter("Far Corner");
	Parameter *MatrixParameter = getParameter("Screen Space Projection Matix(4x4)");

	if (!m_compositor)
		return;

    if (inputTextureNormal.isNull() || inputTextureDepth.isNull() || !farCornerParameter->isConnected() || !farPlaneParameter->isConnected() || !MatrixParameter->isConnected()) {
        // disable compositor (now that the input texture name was set)
        m_compositor->setEnabled(false);
        // render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        Log::info(m_name + " rendered", "AmbientOcclusionNode::processOutputImage");
        setValue("Image", m_defaultTexture);
        Log::warning("Not all input images connected.", "AmbientOcclusionNode::processOutputImage");
    }
    else if (!m_renderTexture.isNull()) {

		if (inputTextureNormal->getWidth() != inputTextureDepth->getWidth()  ||
			inputTextureNormal->getHeight() != inputTextureDepth->getHeight()) {
			Log::warning("Input textures have different resolutions.", "AmbientOcclusionNode::processOutputImage");
			return;
		}

		// resize render texture if necessary
        m_renderWidth = inputTextureNormal->getWidth();
        m_renderHeight = inputTextureNormal->getHeight();

		if (m_renderWidth != m_renderTexture->getWidth() || m_renderHeight != m_renderTexture->getHeight())
			resizeRenderTexture(m_renderWidth, m_renderHeight);	

        // enable compositor (now that the input texture name was set)
        m_compositor->setEnabled(true);
        // render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_renderTexture);
    }
}


//!
//! Redraw of ogre scene has been triggered.
//!
void AmbientOcclusionNode::redrawTriggered ()
{
	if (m_renderTexture.isNull())
		return;

    Ogre::RenderTexture *renderTarget = m_renderTexture->getBuffer()->getRenderTarget();
    if (renderTarget)
        renderTarget->update();
		//renderTarget->writeContentsToFile("Output_from_SSAO-Shader.png");

}

} // namespace AmbientOcclusionNode 
