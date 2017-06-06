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
//! \file "CompositorNode.cpp"
//! \brief Implementation file for CompositorNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       26.05.2009 (last updated)
//!

#include "CompositorNode.h"
#include "Parameter.h"
#include "SceneNodeParameter.h"
#include "OgreTools.h"

namespace Frapper {

INIT_INSTANCE_COUNTER(CompositorNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the CompositorNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
CompositorNode::CompositorNode ( const QString &name, ParameterGroup *parameterRoot ) :
RenderNode(name, parameterRoot),
m_compositor(0),
m_resourceGroupName("")
{
	m_resourceGroupName = getStringValue("Resource Group Name");

	m_uvTransformMat = Ogre::Matrix4(1.0f,0.0f,0.0f,0.0f,
									 0.0f,1.0f,0.0f,0.0f,
									 0.0f,0.0f,1.0f,0.0f,
									 0.0f,0.0f,0.0f,1.0f);

    // set the reload functionality
    Parameter *reloadParameter = getParameter("Reload");
    if (reloadParameter) {
        // set the command function for the reset parameter
        reloadParameter->setCommandFunction(SLOT(reload()));
    } else
        Log::error("Could not obtain reload parameter.", "CompositorNode::CompositorNode");

	// add uv transform parameters
	NumberParameter *position = new NumberParameter("Position", Parameter::T_Float, 0.0f);
	NumberParameter *rotation = new NumberParameter("Rotation", Parameter::T_Float, 0.0f);
	NumberParameter *scale = new NumberParameter("Scale", Parameter::T_Float, 0.0f);
	QVariantList positionValueList = QVariantList() << 0.0f << 0.0f;
	QVariantList scaleValueList = QVariantList() << 1.0f << 1.0f;

	position->setValues(positionValueList);
	scale->setValues(scaleValueList);

	position->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
	scale->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
	rotation->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);

	position->setPinType(Parameter::PT_Input);
	scale->setPinType(Parameter::PT_Input);
	rotation->setPinType(Parameter::PT_Input);

	position->setSelfEvaluating(true);
	scale->setSelfEvaluating(true);
	rotation->setSelfEvaluating(true);

	position->setMinValue(-1);
	scale->setMinValue(0.1);
	rotation->setMinValue(0);

	position->setMaxValue(1);
	scale->setMaxValue(10);
	rotation->setMaxValue(360);

	position->setStepSize(0.01);
	scale->setStepSize(0.01);
	
	position->setSize(2);
	scale->setSize(2);

	// target parameter group
	ParameterGroup *targetParameters = getParameterGroup("Target Parameters");

	targetParameters->addParameter(position);
	targetParameters->addParameter(rotation);
	targetParameters->addParameter(scale);

	position->setProcessingFunction(SLOT(setUVTransform()));
	rotation->setProcessingFunction(SLOT(setUVTransform()));
	scale->setProcessingFunction(SLOT(setUVTransform()));

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the CompositorNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CompositorNode::~CompositorNode ()
{
    if (m_compositor) {
        m_compositor->setEnabled(false);
        m_compositor->removeListener(this);
    }
    DEC_INSTANCE_COUNTER
}

///
/// Protected Slots
///

///
/// Reload resources.
///
void CompositorNode::reload ()
{
    // save the compositor name and destroy compositor
    Ogre::String compositorName;
    if (m_compositor) {
        m_compositor->setEnabled(false);
        Ogre::Compositor *compositor = m_compositor->getCompositor();
        if (compositor) {
            compositorName = compositor->getName();
            m_compositor->removeListener(this);
            Ogre::CompositorManager::getSingleton().removeCompositorChain(m_viewport);		
            m_compositor = 0;
        }
    }

    if (m_resourceGroupName != "") {

        //TODO: NILZ: Do resource group checking.
        Ogre::FileInfoListPtr fileInfoVector = Ogre::ResourceGroupManager::getSingleton().listResourceFileInfo(m_resourceGroupName.toStdString(), false);

        if (fileInfoVector->size() > 0) {
            Ogre::FileInfo fileInfo = fileInfoVector->at(0);
            Ogre::String archiveName = fileInfo.archive->getName();
            OgreTools::destroyResourceGroup(m_resourceGroupName);
            OgreTools::createResourceGroup(m_resourceGroupName, QString::fromStdString(archiveName));
        }
    }

    // create the new compositor with old name and location
	const QString newCompositorName = createUniqueName(getStringValue("Compositor Name")).c_str();
    if (newCompositorName != "")
        compositorName = newCompositorName.toStdString();
    m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, compositorName, 0);
    if (m_compositor) {
        m_compositor->addListener(this);
        m_compositor->setEnabled(true);
        triggerRedraw();
    }
}


//!
//! Protected Functions
//!

//!
//!	Resizes the main render texture (m_renderTexture).
//!
//!	\param width The width of render texture.
//!	\param height The height of render texture.
//!
void CompositorNode::resizeRenderTexture(int width, int height, Ogre::PixelFormat pixelFormat /* = Ogre::PF_R8G8B8 */)
{
    if (!m_compositor || m_renderTexture.isNull() || (m_renderTexture->getWidth() == width && m_renderTexture->getHeight() == height 
        && m_renderTexture->getFormat() == pixelFormat))
        return;

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

    RenderNode::resizeRenderTexture(width, height, pixelFormat, 0);

    // create the new compositor with old name and location
    m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, compositorName, 0);
    if (m_compositor) {
        m_compositor->addListener(this);
        m_compositor->setEnabled(true);
    }

    RenderNode::redrawTriggered();
}

//!
//! Callback when instance of this class is registered as Ogre::CompositorListener.
//! 
//! \param pass_id Id to identifiy current compositor pass.
//! \param mat Material this pass is currently using.
//!
void CompositorNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	Ogre::GpuProgramParametersSharedPtr vpParams;

	//set shader parameters
	const Ogre::Technique *technique0 = mat->getTechnique(0);
	if (technique0) {
		const Ogre::Pass *pass0 = mat->getTechnique(0)->getPass(0);
		if (pass0) {
			if (pass0->hasVertexProgram()) {
				vpParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
				if (vpParams->_findNamedConstantDefinition("transformMatrix"))
					vpParams->setNamedConstant("transformMatrix", m_uvTransformMat);
			}
		}
	}
}

//!
//! Called when scene load is ready to prevent wrong recource setup in Ogre
//!
void CompositorNode::loadReady ()
{
	setUVTransform();
}

//!
//! Sets uv transform parameters.
//!
void CompositorNode::setUVTransform ()
{
	QVariantList &position = getParameter("Position")->getValue().toList();
	QVariantList &scale = getParameter("Scale")->getValue().toList();
	float rotation = getParameter("Rotation")->getValue().toFloat()/180.0f*Ogre::Math::PI;
	
	const Ogre::Matrix4 centerMat		= Ogre::Matrix4(1.0f, 0.0f, 0.0f, 0.5f,
													    0.0f, 1.0f, 0.0f, 0.5f,
													    0.0f, 0.0f, 1.0f, 0.0f,
														0.0f, 0.0f, 0.0f, 1.0f);

	const Ogre::Matrix4 translationMat	= Ogre::Matrix4(1.0f, 0.0f, 0.0f, position[0].toFloat(),
													    0.0f, 1.0f, 0.0f, position[1].toFloat(),
														0.0f, 0.0f, 1.0f, 0.0f,
													    0.0f, 0.0f, 0.0f, 1.0f);

	const Ogre::Matrix4 rotationMat		= Ogre::Matrix4(Ogre::Math::Cos(rotation), Ogre::Math::Sin(rotation), 0.0f, 0.0f,
													   -Ogre::Math::Sin(rotation), Ogre::Math::Cos(rotation), 0.0f, 0.0f,
													    0.0f, 0.0f, 1.0f, 0.0f,
													    0.0f, 0.0f, 0.0f, 1.0f);
	
	const Ogre::Matrix4 scaleMat		= Ogre::Matrix4(1.0f/scale[0].toFloat(), 0.0f, 0.0f, 0.0f,
													    0.0f, 1.0f/scale[1].toFloat(), 0.0f, 0.0f,
													    0.0f, 0.0f, 1.0f, 0.0f,
													    0.0f, 0.0f, 0.0f, 1.0f);

	m_uvTransformMat					=  centerMat * scaleMat * rotationMat * centerMat.inverse() *  translationMat;

	triggerRedraw();
}

//!
//! Force to rebuild the render target. 
//! HACK to restore damaged render targets in GL3+ render system
//!
void CompositorNode::rebuildRendertargets ()
{
	resizeRenderTexture(m_renderWidth, m_renderHeight, m_pixelFormat);
	m_renderTexture->getBuffer()->getRenderTarget()->update();
	setValue(m_outputImageName, m_renderTexture, true);
}

} // end namespace Frapper