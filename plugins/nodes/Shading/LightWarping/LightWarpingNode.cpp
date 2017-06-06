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
//! \file "LightWarpingNode.cpp"
//! \brief Implementation file for LightWarpingNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.1
//! \date       27.09.2010 (last updated)
//!


#include "LightWarpingNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include <QtCore/QFile>

namespace LightWarpingNode {
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
LightWarpingNode::LightWarpingNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot)
{
    // Create compositor.
    if (m_viewport)
        m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "LightWarpingCompositor", 0);

    // Add listener.
    if (m_compositor) {
        m_compositor->addListener(this);
    }

    // Set the affections.
    addAffection("Input Map Normal", m_outputImageName);
    addAffection("Input Map Hessian", m_outputImageName);
    addAffection("Input Map View", m_outputImageName);

    // Set the processing function.
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));

    // Set the change function for light description file parameter.
    setChangeFunction("Light Description File", SLOT(loadLightDescriptionFile()));

    // Set the change function for all sliders.
    setChangeFunction("cmax", SIGNAL(triggerRedraw()));
    setChangeFunction("exponent", SIGNAL(triggerRedraw()));
    setChangeFunction("fresnel", SIGNAL(triggerRedraw()));
    setChangeFunction("diffuse", SIGNAL(triggerRedraw()));
    setChangeFunction("specular", SIGNAL(triggerRedraw()));
    setChangeFunction("contrast", SIGNAL(triggerRedraw()));
    setChangeFunction("envsize", SIGNAL(triggerRedraw()));
    setChangeFunction("lightscale", SIGNAL(triggerRedraw()));
    setChangeFunction("enable", SIGNAL(triggerRedraw()));
    setChangeFunction("aniso", SIGNAL(triggerRedraw()));
}


//!
//! Destructor of the AqtBlendNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
LightWarpingNode::~LightWarpingNode ()
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
void LightWarpingNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    // Fetch all input textures and parameters.
    Ogre::TexturePtr inputTextureNormal = getTextureValue("Input Map Normal");
    Ogre::TexturePtr inputTextureView = getTextureValue("Input Map View");
    Ogre::TexturePtr inputTextureHessian = getTextureValue("Input Map Hessian");
    Ogre::TexturePtr inputTextureLightDirs = getTextureValue("Input Map Lights");
    Ogre::TexturePtr inputTextureLightPows = getTextureValue("Input Map Powers");

    // Not all required inputs are connected.
    if (inputTextureNormal.isNull() || inputTextureView.isNull() || inputTextureHessian.isNull() )
        return;

    // Lightwarping pass
    if (pass_id == 0)
    {
        // Set shader parameters
        Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);

        // Associated node not set.
        if (fpParams.isNull())
            return;

        // Get all shader parameters from node.
        float nbLights = getDoubleValue("Number Of Lights");
        float cmax = getDoubleValue("cmax");
        float exponent = getDoubleValue("exponent");
        float fresnel = getDoubleValue("fresnel");
        Ogre::Vector3 diffuse = getVectorValue("diffuse");
        Ogre::Vector3 specular = getVectorValue("specular");
        float contrast = getDoubleValue("contrast");
        float envsize = getDoubleValue("envsize");
        float lightscale = getDoubleValue("lightscale");
        bool enable = getBoolValue("enable");
        bool aniso = getBoolValue("aniso");

        // Set all shader parameters.
        //setShaderParameter(fpParams, "wMat", wMat);
        setShaderParameter(fpParams, "nbLights", nbLights);
        setShaderParameter(fpParams, "cmax", cmax);
        setShaderParameter(fpParams, "exponent", exponent);
        setShaderParameter(fpParams, "fresnel", fresnel);
        setShaderParameter(fpParams, "diffuse", diffuse);
        setShaderParameter(fpParams, "specular", specular);
        setShaderParameter(fpParams, "contrast", contrast);
        setShaderParameter(fpParams, "envsize", envsize);
        setShaderParameter(fpParams, "lightscale", lightscale);
        setShaderParameter(fpParams, "enable", (float) enable);
        setShaderParameter(fpParams, "aniso", (float) aniso);

        // Set the texture names.
        setTexture(mat, inputTextureNormal, 0);
        setTexture(mat, inputTextureView, 1);
        setTexture(mat, inputTextureHessian, 2);
        setTexture(mat, inputTextureLightDirs, 3);
        setTexture(mat, inputTextureLightPows, 4);
    }
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void LightWarpingNode::processOutputImage ()
{
    // Obtain input textures.
    Ogre::TexturePtr inputTextureNormal = getTextureValue("Input Map Normal");
    Ogre::TexturePtr inputTextureView = getTextureValue("Input Map View");

    if (inputTextureNormal.isNull() || inputTextureView.isNull()) {
        // Disable compositor (now that the input texture name was set).
        if (m_compositor) 
            m_compositor->setEnabled(false);
        
        // Render and set output.
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_defaultTexture);
        Log::warning("Not all input images connected.", "LightWarpingNode::processOutputImage");
    }
    else if (!m_renderTexture.isNull()) {
        // resize render texture if necessary
        m_renderWidth = inputTextureNormal->getWidth();
        m_renderHeight = inputTextureNormal->getHeight();
        resizeRenderTexture(m_renderWidth, m_renderHeight);

        // Enable compositor (now that the input texture name was set)
        if (m_compositor)
            m_compositor->setEnabled(true);
        
        // Render and set output
        m_renderTexture->getBuffer()->getRenderTarget()->update();
        setValue("Image", m_renderTexture);
    }
}

//!
//! Loads an XML reference data file.
//!
//! Is called when the value of the Reference Data File parameter has
//! changed.
//!
//! \return True if the reference data was successfully loaded from file, otherwise False.
//!
bool LightWarpingNode::loadLightDescriptionFile ( )
{
    QString path = getStringValue("Light Description File");
    bool result = false;
    QFile inFile(path);

	float* pDestDirs = 0;
	float* pDestPows = 0;

   Ogre::HardwarePixelBufferSharedPtr pixelBufferDirs;
   Ogre::HardwarePixelBufferSharedPtr pixelBufferPows;

    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		
		if (!inFile.atEnd()) {
			QString line = inFile.readLine();
			int length = line.section(" ", 1, 1).toInt();

			if(!m_lightDirsTexture.isNull() && Ogre::TextureManager::getSingleton().resourceExists(m_lightDirsTexture->getName()))
				Ogre::TextureManager::getSingleton().remove(m_lightDirsTexture->getName());
			
			if(!m_lightPowsTexture.isNull() && Ogre::TextureManager::getSingleton().resourceExists(m_lightPowsTexture->getName()))
				Ogre::TextureManager::getSingleton().remove(m_lightPowsTexture->getName());
			
			Ogre::String lightDirsTextureName = createUniqueName("LightDirsTexture_LightWarpingNode");
			Ogre::String lightPowsTextureName = createUniqueName("LightPowsTexture_LightWarpingNode");
			
			m_lightDirsTexture = Ogre::TextureManager::getSingleton().createManual(lightDirsTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_1D, length, 1, 0, Ogre::PF_FLOAT32_RGB, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			m_lightPowsTexture = Ogre::TextureManager::getSingleton().createManual(lightPowsTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_1D, length, 1, 0, Ogre::PF_FLOAT32_RGB, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
        }
		
		if (!m_lightDirsTexture.isNull() && !m_lightDirsTexture.isNull()) {
			// Get the pixel buffer
			pixelBufferDirs = m_lightDirsTexture->getBuffer();
			pixelBufferPows = m_lightPowsTexture->getBuffer();

			// Lock the pixel buffer and get a pixel box
			pixelBufferDirs->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
			pixelBufferPows->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
			const Ogre::PixelBox& pixelBoxDirs = pixelBufferDirs->getCurrentLock();
			const Ogre::PixelBox& pixelBoxPows = pixelBufferPows->getCurrentLock();
			pDestDirs = static_cast<float*>(pixelBoxDirs.data);
			pDestPows = static_cast<float*>(pixelBoxPows.data);
		}

		while (!inFile.atEnd()) {
			QString line = inFile.readLine();
			QString prefix = line.section(" ", 0, 0);
			if (prefix == "Dir:") {
				*pDestDirs++ = (float) line.section(" ", 1, 1).toDouble();
				*pDestDirs++ = (float) line.section(" ", 2, 2).toDouble();
				*pDestDirs++ = (float) line.section(" ", 3, 3).toDouble();
			}
			else if (prefix == "Pow:") {
				*pDestPows++ = (float) line.section(" ", 1, 1).toDouble();
				*pDestPows++ = (float) line.section(" ", 2, 2).toDouble();
				*pDestPows++ = (float) line.section(" ", 3, 3).toDouble();
			}
			
		}

		// Unlock the pixel buffer
		pixelBufferDirs->unlock();
		pixelBufferPows->unlock();
        
    } else
        Log::error(QString("Document import failed. \"%1\" not found.").arg(path), "LightDescriptionNode::loadReferenceDataFile");

    setValue("Input Map Lights", m_lightDirsTexture);
    setValue("Input Map Powers", m_lightPowsTexture);

    inFile.close();
    return result;
}


//!
//! Redraw of ogre scene has been triggered.
//!
void LightWarpingNode::redrawTriggered ()
{
	if (m_renderTexture.isNull())
		return;

    Ogre::RenderTexture *renderTarget = m_renderTexture->getBuffer()->getRenderTarget();
    if (renderTarget)
        renderTarget->update();
}

} // namespace LightWarpingNode 
