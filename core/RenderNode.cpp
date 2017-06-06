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
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "RenderNode.cpp"
//! \brief Implementation file for RenderNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.3
//! \date       22.03.2014 (last updated)
//!

#include "RenderNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreManager.h"

namespace Frapper {

///
/// Static protected variables
///


//!
//! The default gamma.
//!
float RenderNode::DefaultGamma = 2.2f;

//!
//! The default resolution multiplier literals.
//!
QStringList RenderNode::resLiterals = QStringList() << "4x" << "2x" << "none" << "1/2" << "1/4" << "1/8";

//!
//! The default resolution multiplier values.
//!
QStringList RenderNode::resValues = QStringList() << "4.0" << "2.0" << "1.0" << "0.5" << "0.25" << "0.125";


///
/// Constructors and Destructors
///


//!
//! Constructor of the RenderNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
RenderNode::RenderNode ( const QString &name, ParameterGroup *parameterRoot ) :
ViewNode(name, parameterRoot),
m_renderWidth(DefaultRenderWidth),
m_renderHeight(DefaultRenderHeight),
m_pixelFormat(Ogre::PF_R8G8B8),
m_fsaa(4),
m_targetMultiplier(1),
m_currentId(0),
m_sceneManager(0),
m_viewport(0),
m_camera(0),
m_outputImageName("Image"),
m_renderTextureName(Ogre::String(""))
{    
	Ogre::Root *ogreRoot = OgreManager::getRoot();
    if (ogreRoot) {
        // create new Ogre::SceneManager to handle the temporary render scene
        m_sceneManager = ogreRoot->createSceneManager(Ogre::ST_GENERIC, createUniqueName("SceneManager_RenderNode"));
        if (m_sceneManager) {
            m_camera = m_sceneManager->createCamera(createUniqueName("TmpCamera"));
            m_camera->setAspectRatio((float)m_renderWidth / (float)m_renderHeight);
            m_renderTextureName = createUniqueName("RenderTexture_" + getName());
            initializeRenderTarget(m_camera);
        }
    }

    // create default texture
    Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
    m_defaultTexture = textureManager.getByName("MissingCameraTexture");

    if (m_defaultTexture.isNull()) {
        Ogre::Image defaultImage;
        defaultImage.load("DefaultRenderImage.png", "General");
        m_defaultTexture = textureManager.loadImage("MissingCameraTexture", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, defaultImage,
													Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_A8R8G8B8);
    }

	// add the output image parameter
    addOutputParameter(Parameter::createImageParameter(m_outputImageName, m_defaultTexture));

    // setup default output image parameter
    m_outputParameter = getParameter(m_outputImageName);
    m_outputParameter->setAuxProcessingFunction(SLOT(redrawTriggered()));

	// target parameter group
	ParameterGroup *targetParameters = new ParameterGroup("Target Parameters");
	parameterRoot->addParameter(targetParameters);

    // setup the render target reder target mltipliers 
    EnumerationParameter *multiplierList = new EnumerationParameter("Render Target Multiplier", 0);
    targetParameters->addParameter(multiplierList);
    multiplierList->setLiterals(resLiterals);
    multiplierList->setValues(resValues);
	multiplierList->setValue( (int) 2);
	multiplierList->setChangeFunction(SLOT(setTargetMultiplier()));
    multiplierList->setDirty(true);
}


//!
//! Destructor of the RenderNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
RenderNode::~RenderNode ()
{
    finalizeRenderTargets();

    Ogre::Root *ogreRoot = OgreManager::getRoot();

    if (ogreRoot) {
        OgreTools::deepDeleteSceneNode(m_sceneManager->getRootSceneNode(), m_sceneManager);
        m_sceneManager->destroyAllCameras();
        ogreRoot->destroySceneManager(m_sceneManager);
    }
}

///
/// Private Methods
///



//!
//! Generates a Ogre RenderTexture by using the createManual(...) function.
//!
//! \return The unique name of the new render texture.
//!
Ogre::String RenderNode::generateTexture(const QString &name, Ogre::TextureType texType, Ogre::uint width, Ogre::uint height, int num_mips, Ogre::PixelFormat format, int usage)
{
    // create unique name for the reder texture
    Ogre::String uniqueRenderTextureName = createUniqueName(name);
    // convert Ogre::String to QString
    QString qtUniqueRenderTextureName = QString::fromStdString(uniqueRenderTextureName);
    // create render Texture
    Ogre::TexturePtr newTex = Ogre::TextureManager::getSingleton().createManual(uniqueRenderTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, texType, width*m_targetMultiplier, height*m_targetMultiplier, num_mips, format, usage, 0, false, 0);
    newTex->getBuffer()->getRenderTarget()->setAutoUpdated(false);
    m_textureHash[qtUniqueRenderTextureName] = newTex;
    return uniqueRenderTextureName;
}

//!
//! Create a viewport for all RenderTextures (RenderTargets) in m_textureHash.
//!
//!	\param cam Camera to create a Ogre::Viewport.
//!	
void RenderNode::initializeRenderTarget(Ogre::Camera* cam, Ogre::ColourValue bkColor /*= Ogre::ColourValue(0,0,0)*/, Ogre::PixelFormat format /*= Ogre::PF_R8G8B8*/, bool clearFrame /*= true*/, int fsaa /*= 4*/)
{
    Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
    m_camera = cam;

    if (!textureManager.resourceExists(m_renderTextureName)) {
        m_renderTexture = Ogre::TextureManager::getSingleton().createManual(m_renderTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			Ogre::TEX_TYPE_2D,  m_renderWidth*m_targetMultiplier, m_renderHeight*m_targetMultiplier, 0, format, Ogre::TU_RENDERTARGET, 0, false, fsaa);
        m_renderTexture->getBuffer()->getRenderTarget()->setAutoUpdated(false);
        m_renderTexture->setGamma(DefaultGamma);

        m_viewport = m_renderTexture->getBuffer()->getRenderTarget()->addViewport(cam);
        m_viewport->setOverlaysEnabled(false);
    }
    else
        resizeRenderTexture(m_renderWidth, m_renderHeight, format, fsaa);
    // to be shure that bkColour is set to correct value
    m_viewport->setClearEveryFrame(clearFrame);
    m_viewport->setBackgroundColour(bkColor);
}

//!
//! Create a viewport for all RenderTextures (RenderTargets) in m_textureHash.
//!
//!	\param cam Camera to create a Ogre::Viewport.
//!	
void RenderNode::initializeRenderTargetsInHash(Ogre::Camera* cam /*= 0*/, bool clearFrame /*= true*/)
{
    Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
    QHash<QString, Ogre::TexturePtr>::iterator iterTexHash;
    QHash<QString, Ogre::Viewport*>::iterator iterViewHash;
    Ogre::TexturePtr texture;
    Ogre::Viewport *viewport = 0;

    if (!cam)
        cam = m_camera;

    for (iterTexHash=m_textureHash.begin(); iterTexHash!=m_textureHash.end(); ++iterTexHash) {
        // create viewports for all render targets in the m_textureHash
        iterViewHash = m_viewportHash.find(iterTexHash.key());
        texture = iterTexHash.value();

        if (iterViewHash != m_viewportHash.end())
            texture->getBuffer()->getRenderTarget()->removeAllViewports();

        viewport = m_viewportHash.insert(iterTexHash.key(), texture->getBuffer()->getRenderTarget()->addViewport(cam)).value();
        viewport->setClearEveryFrame(clearFrame);
        viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
        //viewport->setBackgroundColour(OgreManager::convertColor(getColorValue(BackgroundColorParameterName)));
        viewport->setOverlaysEnabled(false);
        viewport->setSkiesEnabled(false);
    }
}

//!
//!	Initializes all textures
//!
//!	\param cam The scene cam for the new render target.
//!
void RenderNode::initializeAllTargets(Ogre::Camera* cam)
{
    initializeRenderTarget(cam);
    initializeRenderTargetsInHash(cam);
}

//!
//!	Resizes all textures
//!
//!	\param width The width of new render texture.
//!	\param height The height of new render texture.
//!
void RenderNode::resizeAllTargets(int width, int height)
{
    resizeRenderTexture(width, height);
    resizeAllTexturesInHash(width, height);
}

//!
//!	Resizes all textures in m_textureHash.
//!
//!	\param width The width of new render texture.
//!	\param height The height of new render texture.
//!
void RenderNode::resizeAllTexturesInHash(int width, int height)
{
    QHash<QString, Ogre::TexturePtr>::iterator iterTexHash;
    Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();

    width *= m_targetMultiplier;
    height *= m_targetMultiplier;

    for (iterTexHash = m_textureHash.begin(); iterTexHash != m_textureHash.end(); ++iterTexHash) {
        Ogre::TexturePtr texture = iterTexHash.value();

        // save parameters of the texture
        const Ogre::String textureName = texture->getName();
        const Ogre::String &resourceGroupManager = texture->getGroup();
        const Ogre::TextureType texType = texture->getTextureType();
        const size_t num_mips = texture->getNumMipmaps();
        const Ogre::PixelFormat format = texture->getFormat();
        const int usage = texture->getUsage();

        bool clearFrame = false;
        if (m_viewportHash.contains(QString::fromStdString(textureName))) {
            Ogre::Viewport *viewport0 = m_viewportHash[QString::fromStdString(textureName)];
            clearFrame = viewport0->getClearEveryFrame();
        }

        removeRenderTexture(texture);

        // create new render texture
        texture = textureManager.createManual(textureName, resourceGroupManager, texType, width, height, num_mips, format, usage);
        Ogre::RenderTexture *target = texture->getBuffer()->getRenderTarget();
        target->setAutoUpdated(false);
        m_textureHash[QString::fromStdString(textureName)] = texture;

        if (m_viewportHash.contains(QString::fromStdString(textureName))) {
            // add new viwport to render texture
            Ogre::Viewport *viewport0 = target->addViewport(m_camera);
            viewport0->setClearEveryFrame(true);
            viewport0->setBackgroundColour(Ogre::ColourValue(0,0,0));
            viewport0->setOverlaysEnabled(false);
            viewport0->setSkiesEnabled(false);
            m_viewportHash[QString::fromStdString(textureName)] = viewport0;
        }
    }
}

//!
//!	Resizes the main render texture (m_renderTexture).
//!
//!	\param width The width of new render texture.
//!	\param height The height of new render texture.
//!
void RenderNode::resizeRenderTexture(int width, int height, Ogre::PixelFormat pixelFormat /*= Ogre::PF_R8G8B8 */, int fsaa /*= 4*/)
{
    Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();

    // delete the render texture
    if (!textureManager.resourceExists(m_renderTextureName))
        return;

    width *= m_targetMultiplier;
    height *= m_targetMultiplier;

    const Ogre::String &resourceGroupManager = m_renderTexture->getGroup();
    Ogre::TextureType texType = m_renderTexture->getTextureType();
    size_t num_mips = m_renderTexture->getNumMipmaps();
    int usage = m_renderTexture->getUsage();
    //int fsaa = m_renderTexture->getFSAA();
    Ogre::Viewport *viewport0 = m_renderTexture->getBuffer()->getRenderTarget()->getViewport(0);
    const bool clearFrame = viewport0->getClearEveryFrame();
    const Ogre::ColourValue &bkColour = viewport0->getBackgroundColour();

    removeRenderTexture(m_renderTexture);

    // (re-)create the render texture
	m_renderTextureName = createUniqueName("RenderTexture_" + getName());
    m_renderTexture = textureManager.createManual(m_renderTextureName, resourceGroupManager, texType, width, height, num_mips, pixelFormat, usage, 0, false, fsaa);
    m_renderTexture->setGamma(((float) DefaultGamma)/10.0f);
    Ogre::RenderTexture *target = m_renderTexture->getBuffer()->getRenderTarget();
    target->setAutoUpdated(false);
    m_renderWidth = width;
    m_renderHeight = height;
	m_pixelFormat = pixelFormat;
	m_fsaa = fsaa;

    // add new viwport to render texture
    m_viewport = target->addViewport(m_camera);
    m_viewport->setClearEveryFrame(clearFrame);
    m_viewport->setBackgroundColour(bkColour);
    m_viewport->setOverlaysEnabled(false);
}


//!
//! Clean up all textures
//!
void RenderNode::finalizeRenderTargets()
{
    // finalize main render target
    Ogre::CompositorManager::getSingleton().removeCompositorChain(m_viewport);
    removeRenderTexture(m_renderTexture);

    // finalize add render targets
    QHash<QString, Ogre::TexturePtr>::iterator iterTexHash;
    QHash<QString, Ogre::Viewport*>::iterator iterViewHash;

    for (iterTexHash = m_textureHash.begin(); iterTexHash != m_textureHash.end(); ++iterTexHash) {
        Ogre::CompositorManager::getSingleton().removeCompositorChain(m_viewportHash[iterTexHash.key()]);
        removeRenderTexture(iterTexHash.value());
    }
    m_textureHash.clear();
    m_viewportHash.clear();
}

void RenderNode::removeRenderTexture(Ogre::TexturePtr &texture)
{
    Ogre::TextureManager &texMgr = Ogre::TextureManager::getSingleton();
    if (!texture.isNull()) {
        const Ogre::String &texName = texture->getName();
        Ogre::RenderTexture *target = texture->getBuffer()->getRenderTarget();
        target->removeAllListeners();
        target->removeAllViewports();
        if (texMgr.resourceExists(texName))
            texMgr.remove(texName);
        else
            Log::error("Texture: " + QString(texName.c_str()) + "to be removed from Ogre::TextureManager could not be found!", "RenderNode");
    }
}


//!
//! Assign texture to texture slot.
//!
//! \param material The material.
//! \param texture Pointer to the texture.
//! \param slot The texture slot id.
//!
void RenderNode::setTexture( const Ogre::MaterialPtr &material, const Ogre::TexturePtr &texture, const unsigned int slot ) const
{
    if (!material.isNull() && !texture.isNull()) {
        Ogre::Technique *technique = material->getTechnique(0);
        if (!technique)
            return;
        Ogre::Pass *pass = technique->getPass(0);
        if (!pass)
            return;
        unsigned short numPasses = pass->getNumTextureUnitStates();
        if (slot >= numPasses)
            return;
        Ogre::TextureUnitState *textureUnitState = pass->getTextureUnitState(slot);
        textureUnitState->setTextureName(texture->getName());
    }
}


//!
//! Get shader parameters.
//!
//! \param mat The material.
//! \param type The specific shader stage.
//! \param tecId The id for the technique.
//! \param passId The id for the pass.
//!
const Ogre::GpuProgramParametersSharedPtr RenderNode::getShaderParameters( 
	Ogre::MaterialPtr &material, const ShaderType type /*= SH_Frag*/, const unsigned short tecId /*= 0*/, const unsigned short passId /*= 0*/ ) const
{
	Ogre::GpuProgramParametersSharedPtr returnvalue;
	if (!material.isNull()) { 
		const Ogre::Pass *pass = material->getTechnique(tecId)->getPass(passId);
		if (pass) {
			switch (type) {
			case SH_Vert: 
				returnvalue = pass->getVertexProgramParameters();
				break;
			case SH_Frag: 
				returnvalue = pass->getFragmentProgramParameters();
				break;
			case SH_Geo: 
				returnvalue = pass->getGeometryProgramParameters();
				break;
			default: 
				returnvalue = pass->getFragmentProgramParameters();
				break;
			}
		}
	}
	return returnvalue;
}


//!
//! Creates a unique name from the given object name by adding the Node
//! number.
//!
const Ogre::String RenderNode::createUniqueName ( const QString &objectName )
{
	return Node::createUniqueName(objectName + QString("_%1").arg(m_currentId++));
}

///
/// Public Methods
///


//!
//! Returns the image that is generated by this node.
//!
//! \return The image that is generated by this node.
//!
Ogre::TexturePtr RenderNode::getImage ()
{
    Parameter *parameter = getParameter(m_outputImageName);
    if (parameter)
        parameter->propagateAuxDirty();
    return getTextureValue(m_outputImageName, true);
}

//!
//! Returns the image that is generated by this node.
//!
//! \return The image that is generated by this node.
//!
void RenderNode::updateImage () const
{
	if (m_outputParameter) {
        m_outputParameter->propagateAuxDirty();
		m_outputParameter->propagateEvaluation();
	}
}

//!
//! Returns the width of the render target.
//!
//! \return The width of the render target.
//!
unsigned int RenderNode::getWidth() const
{
    return m_renderWidth;
}

//!
//! Returns the height of the render target.
//!
//! \return The height of the render target.
//!
unsigned int RenderNode::getHeight() const
{
    return m_renderWidth;
}


///
/// Public Slots
///


//!
//! Redraw of ogre scene has been triggered.
//!
void RenderNode::redrawTriggered ()
{
    if	(!m_renderTexture.isNull() &&
		(isViewed() || m_outputParameter->isConnected()))
        m_renderTexture->getBuffer()->getRenderTarget()->update();

	m_outputParameter->setAuxDirty(false);

    Ogre::RenderTexture *texture; 
    foreach (Ogre::TexturePtr ptr, m_textureHash) {
		texture = ptr->getBuffer()->getRenderTarget();
        if (!ptr.isNull() && texture)
            texture->update();
    }

	for (int i=0; i<m_outParameterNameList.size(); ++i)
		getParameter(m_outParameterNameList[i])->setAuxDirty(false);

	/*Parameter* timeParameter = getTimeParameter();
	if (timeParameter)
		std::cout << "Render Node:" << timeParameter->getValue().toInt() << std::endl; */
}


//!
//! Set the multiplier for the render targets.
//!
void RenderNode::setTargetMultiplier()
{
    EnumerationParameter* parameter = static_cast<EnumerationParameter*>(sender());
    m_targetMultiplier = parameter->getCurrentValue().toFloat();
	m_outputParameter->propagateDirty();
}

//!
//! Force to rebuild the render target. 
//! HACK to restore damaged render targets in GL3+ render system
//! 
void RenderNode::rebuildRendertargets ()
{
	resizeRenderTexture(m_renderWidth, m_renderHeight, m_pixelFormat, m_fsaa);
    m_renderTexture->getBuffer()->getRenderTarget()->update();
    setValue(m_outputImageName, m_renderTexture, true);
}

} // end namespace Frapper