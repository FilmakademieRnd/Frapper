/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "ShadingTemplateNode.cpp"
//! \brief Implementation file for ShadingTemplateNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       30.03.2011 (last updated)
//!


#include "ShadingTemplateNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"

namespace ShadingTemplateNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the ShadingTemplateNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
ShadingTemplateNode::ShadingTemplateNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode(name, parameterRoot), 
    m_resX(0), 
    m_resY(0)
{

    // add ressource location
    const QString &resourceLocation = getStringValue("Resource Location");
    const QString &shadingName      = getStringValue("Shader Name");
    m_resourceGroupName = shadingName + "_Group";

    if ( resourceLocation != "")
    {
        Ogre::ResourceGroupManager *resourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();
        if( !resourceGroupManager->resourceGroupExists( m_resourceGroupName.toStdString()))
        {
            resourceGroupManager->addResourceLocation( resourceLocation.toStdString(), "FileSystem", m_resourceGroupName.toStdString());
            resourceGroupManager->initialiseResourceGroup( m_resourceGroupName.toStdString());
        }
    }
    
    // create shading
    if (m_viewport) {
        if (shadingName != "")
            m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, shadingName.toStdString(), 0);
    }
	
    // add listener
    if (m_compositor) {
        m_compositor->addListener(this);
    }
    setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));



    // set affections for all input map parameters
    ParameterGroup *inputMapsGroup = getParameterGroup("Input Maps");
    if (inputMapsGroup) {
        QList<Parameter*> inputMapParameters = inputMapsGroup->filterParameters("", true, true);
        for (int i = 0; i < inputMapParameters.size(); ++i) {
            Parameter *parameter = inputMapParameters[i];
            std::cout << parameter->getName().toStdString() << std::endl;
            if (parameter)
                addAffection(parameter->getName(), m_outputImageName);
        }
    }

    // set change function for all shader parameters
    unsigned int pass = 0;
    ParameterGroup *shaderParameterGroup = 0;
    while (shaderParameterGroup = getParameterGroup("Shader Parameters Pass " + QString::number(pass) )) {
        if (shaderParameterGroup) {
            QList<Parameter*> shaderParameters = shaderParameterGroup->filterParameters("", true, true);
            for (int i = 0; i < shaderParameters.size(); ++i) {
                Parameter *parameter = shaderParameters[i];
                if (parameter)
                    setChangeFunction(parameter->getName(), SIGNAL(triggerRedraw()));
            }
        }
        pass++;
    }

    m_resX = new NumberParameter( "Resolution X", Parameter::T_Int, QVariant(0));
    m_resY = new NumberParameter( "Resolution Y", Parameter::T_Int, QVariant(0));

    m_resX->setMaxValue(QVariant(4096));
    m_resX->setMinValue(QVariant(0));

    m_resY->setMaxValue(QVariant(4096));
    m_resY->setMinValue(QVariant(0));

    parameterRoot->addParameter(m_resX);
    parameterRoot->addParameter(m_resY);

    m_resX->setChangeFunction(SLOT(processOutputImage()));
    m_resY->setChangeFunction(SLOT(processOutputImage()));

}


//!
//! Destructor of the BlendNode class.
//!

//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ShadingTemplateNode::~ShadingTemplateNode ()
{
}


//!
//! Protected Functions
//!

//!
//! Callback when instance of this class is registered as Ogre::ShadingListener.
//! 
//! \param pass_id Id to identifiy current shading pass.
//! \param mat Material this pass is currently using.
//!
void ShadingTemplateNode::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    //set shader parameters
    Ogre::GpuProgramParametersSharedPtr fpParams = getShaderParameters(mat);

    // set change function for all shader parameters
    ParameterGroup *shaderParameterGroup = getParameterGroup("Shader Parameters Pass " + QString::number(pass_id));
    if (shaderParameterGroup) {
        QList<Parameter*> shaderParameters = shaderParameterGroup->filterParameters("", true, true);
        for (int i = 0; i < shaderParameters.size(); ++i) {
            Parameter *parameter = shaderParameters[i];
            if (parameter) {
                if (parameter->getType() == Parameter::T_Int)
                    setShaderParameter(fpParams, parameter->getName().toStdString(), parameter->getValue().toInt());
                else if (parameter->getType() == Parameter::T_Float)
                    setShaderParameter(fpParams, parameter->getName().toStdString(), parameter->getValue().toFloat());
				else if (parameter->getType() == Parameter::T_Color) {
					const QColor &color = parameter->getValue().value<QColor>();
					setShaderParameter(fpParams, parameter->getName().toStdString(), Ogre::Vector3(color.redF(), color.greenF(), color.blueF()));
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
void ShadingTemplateNode::reload ()
{
	// save the shading name and destroy shading
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

    // create the new shading with old name and location
	compositorName = getStringValue("Shader Name").toStdString();
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
void ShadingTemplateNode::processOutputImage ()
{
    size_t width = 16;
    size_t height = 16;

	if (m_compositor)
        m_compositor->setEnabled(false);

    const int resX = m_resX->getValue().toInt();
    const int resY = m_resY->getValue().toInt();

    // resize internal render texture
    ParameterGroup *inputMapsGroup = getParameterGroup("Input Maps");
    QList<Parameter*> inputMapParameters = inputMapsGroup->filterParameters("", true, true);
    for (int i = 0; i < inputMapParameters.size(); ++i) {
        Parameter *parameter = inputMapParameters[i];
        if (parameter) {
            Ogre::TexturePtr inputTexture = getTextureValue(parameter->getName());
            if (inputTexture.isNull())
                continue;

            // find the largest texture in chain
            if (i == 0) {
                width = ( resX > 16 ) ? resX : inputTexture->getWidth();
                height = ( resY > 16 ) ? resY : inputTexture->getHeight();
            }
			Ogre::MaterialPtr matPtr = m_compositor->getTechnique()->getTargetPass(i)->getPass(0)->getMaterial();
			setTexture(matPtr, getTextureValue(parameter->getName()), 0);
        }
    }
	resizeRenderTexture(width, height, Ogre::PF_R8G8B8A8);

    //enable shading (now that the input texture name was set)
    if (m_compositor)
        m_compositor->setEnabled(true);

    m_renderTexture->getBuffer()->getRenderTarget()->update();
    //setValue(m_outputImageName, m_renderTexture);
}
} // namespace ShadingTemplateNode
