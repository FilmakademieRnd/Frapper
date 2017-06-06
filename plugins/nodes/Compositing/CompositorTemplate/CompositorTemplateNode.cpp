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
//! \file "CompositorTemplateNode.cpp"
//! \brief Implementation file for CompositorTemplateNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.2
//! \date       21.12.2012 (last updated)
//!


#include "CompositorTemplateNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace CompositorTemplateNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the CompositorTemplateNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
CompositorTemplateNode::CompositorTemplateNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot)
{
    // create compositor
    if (m_viewport) {
        const QString &compositorName = getStringValue("Compositor Name");
        if (compositorName != "")
            m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, compositorName.toStdString(), 0);
    }
	
    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));

    // set affections for all input map parameters
    ParameterGroup *inputMapsGroup = getParameterGroup("Input Maps");
    if (inputMapsGroup) {
		const Parameter::List& inputMapParameters = inputMapsGroup->getParameterList();
        for (int i = 0; i < inputMapParameters.size(); ++i) {
			const Parameter *parameter = dynamic_cast<Parameter *>(inputMapParameters.at(i));
            if (parameter)
                addAffection(parameter->getName(), m_outputImageName);
        }
    }

    // set change function for all shader parameters
    unsigned int pass = 0;
    ParameterGroup *shaderParameterGroup = 0;
    while (shaderParameterGroup = getParameterGroup("Shader Parameters Pass " + QString::number(pass) )) {
        if (shaderParameterGroup) {
			const Parameter::List& shaderParameters = shaderParameterGroup->getParameterList();
            for (int i = 0; i < shaderParameters.size(); ++i) {
                Parameter *parameter = dynamic_cast<Parameter *>(shaderParameters.at(i));
                if (parameter) {
					setProcessingFunction(parameter->getName(), SIGNAL(triggerRedraw()));
				}
            }
        }
        pass++;
    }

	// target parameter group
	ParameterGroup *targetParameters = getParameterGroup("Target Parameters");

	// resolution parameters
    NumberParameter *resolutionParameter = new NumberParameter( "Resolution", Parameter::T_Float, 0.0f);
	QVariantList resolutionValueList = QVariantList() << 0.0f << 0.0f;
	
    resolutionParameter->setMaxValue(4096);
    resolutionParameter->setMinValue(0);

	resolutionParameter->setValues(resolutionValueList);
    targetParameters->addParameter(resolutionParameter);
    resolutionParameter->setChangeFunction(SLOT(processOutputImage()));

	//dd - Preset Values
	setChangeFunction("Preset", SLOT(renderPresetChanged()));
	setProcessingFunction("Preset", SLOT(renderPresetChanged()));
	setProcessingFunction("Preset Index", SLOT(changePresetValue()));
}


//!
//! Destructor of the BlendNode class.
//!

//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CompositorTemplateNode::~CompositorTemplateNode ()
{
}


//!
//! Protected Functions
//!

//!
//! Callback when instance of this class is registered as Ogre::CompositorListener.
//! 
//! \param pass_id Id to identifiy current compositor pass.
//! \param mat Material this pass is currently using.
//!
void CompositorTemplateNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	CompositorNode::notifyMaterialRender(pass_id, mat);	

	Ogre::GpuProgramParametersSharedPtr vpParams, fpParams;

	//set shader parameters
	const Ogre::Technique *technique0 = mat->getTechnique(0);
	if (technique0) {
		const Ogre::Pass *pass0 = mat->getTechnique(0)->getPass(0);
		if (pass0) {
			if (pass0->hasFragmentProgram()) {
				fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
				// set change function for all shader parameters
				ParameterGroup *shaderParameterGroup = getParameterGroup("Shader Parameters Pass " + QString::number(pass_id));
				if (shaderParameterGroup) {
					Parameter *parameter = 0;
					const Parameter::List& shaderParameters = shaderParameterGroup->getParameterList();
					foreach (AbstractParameter *absParameter, shaderParameters) {
						parameter = static_cast<Parameter *>(absParameter);
						const Ogre::String &parameterName = parameter->getName().toStdString();
						const QVariant &value = parameter->getValue();
						switch ( parameter->getType() ) {
						case Parameter::T_Int:
							setShaderParameter(fpParams, parameterName, value.toInt());
							break;
						case Parameter::T_Float:
							setShaderParameter(fpParams, parameterName, value.toFloat());
							break;
						case Parameter::T_Color:
							const QColor &color = value.value<QColor>();
							setShaderParameter(fpParams, parameterName, Ogre::Vector3(color.redF(), color.greenF(), color.blueF()));
							break;
						}
					}
				}
			}
		}
	}
}

///
/// Protected Slots
///

///
/// Reload resources.
///
void CompositorTemplateNode::reload ()
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

    // get the resource location and reload the resources
    if (m_resourceGroupName != "") {
        Ogre::FileInfoListPtr fileInfoVector = Ogre::ResourceGroupManager::getSingleton().listResourceFileInfo(m_resourceGroupName.toStdString(), false);

        if (fileInfoVector->size() > 0) {
            Ogre::FileInfo fileInfo = fileInfoVector->at(0);
            Ogre::String archiveName = fileInfo.archive->getName();
            OgreTools::destroyResourceGroup(m_resourceGroupName);
            OgreTools::createResourceGroup(m_resourceGroupName, QString::fromStdString(archiveName));
        }
    }

    // create the new compositor with old name and location
	compositorName = getStringValue("Compositor Name").toStdString();
    m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, compositorName, 0);
    if (m_compositor) {
        m_compositor->addListener(this);
		processOutputImage();
		emit viewNodeUpdated();
    }
}

///
/// Private Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void CompositorTemplateNode::processOutputImage ()
{
    size_t width = 16;
    size_t height = 16;

	if (m_compositor)
        m_compositor->setEnabled(false);
	else
		return;

	const QVariantList &resList = getParameter("Resolution")->getValue().toList();;
	const float resX = resList[0].toFloat();
    const float resY = resList[1].toFloat();

    // resize internal render texture
    ParameterGroup *inputMapsGroup = getParameterGroup("Input Maps");
	const Parameter::List& inputMapParameters = inputMapsGroup->getParameterList();
    for (int i = 0; i < inputMapParameters.size(); ++i) {
		const Parameter *parameter = dynamic_cast<Parameter *>(inputMapParameters.at(i));
        if (parameter) {
            Ogre::TexturePtr inputTexture = getTextureValue(parameter->getName(), true);
            if (inputTexture.isNull())
				inputTexture = m_defaultTexture;

            // find the largest texture in chain
            if (i == 0) {
                width = ( resX > 16 ) ? resX : inputTexture->getWidth();
                height = ( resY > 16 ) ? resY : inputTexture->getHeight();
            }
			Ogre::MaterialPtr matPtr;
			if( m_compositor->getTechnique()->getNumTargetPasses() > i && 
				m_compositor->getTechnique()->getTargetPass(i)->getNumPasses() > 0) {
					matPtr = m_compositor->getTechnique()->getTargetPass(i)->getPass(0)->getMaterial();
			} else if( i == 0 ) {
					matPtr = m_compositor->getTechnique()->getOutputTargetPass()->getPass(0)->getMaterial();
			}
			setTexture(matPtr, inputTexture, 0);
        }
    }
	resizeRenderTexture(width, height, Ogre::PF_R8G8B8A8);

    //enable compositor (now that the input texture name was set)
    if (m_compositor)
        m_compositor->setEnabled(true);

    m_renderTexture->getBuffer()->getRenderTarget()->update();
    setValue(m_outputImageName, m_renderTexture);
}

//!
//! Changes the Shader parameter values according to the predefined values
//!
void CompositorTemplateNode::renderPresetChanged()
{
	ParameterGroup *shaderPassGroup = getParameterGroup("Shader Parameters Pass 0");
	const Parameter::List& shaderParameters = shaderPassGroup->getParameterList(); 

	EnumerationParameter *compositorParams = getEnumerationParameter("Preset");
	if (!compositorParams ) {
		Log::error("Compositor preset values could not be obtained.", "CompositorTemplateNode::renderPresetChanged");
		return;
	}

	QString values = compositorParams->getCurrentValue();
	QStringList valuesList = values.split(" x ");

	if (valuesList.size() == shaderParameters.size())
	{
		int index = 0;
		foreach(AbstractParameter *p, shaderParameters)
		{
			Parameter *param = dynamic_cast<Frapper::Parameter*>(p);
			param->setValue(QVariant(valuesList.at(index)));
			index++;
		}
	}
}

//!
//! Changes the predefined values according to the Preset Index
//!
void CompositorTemplateNode::changePresetValue()
{
	int index = getValue("Preset Index").toInt();
	EnumerationParameter *compositorParams = getEnumerationParameter("Preset");
	compositorParams->setValue(QVariant(index), true);

	renderPresetChanged();
}



} // namespace CompositorTemplateNode
