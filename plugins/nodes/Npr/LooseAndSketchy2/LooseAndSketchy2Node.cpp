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
//! \file "LooseAndSketchy2Node.cpp"
//! \brief Implementation file for LooseAndSketchy2Node class.
//!
//! \author     Marc Spicker
//! \version    1.0
//! \date       03.02.2015 (last updated)
//!


#include "LooseAndSketchy2Node.h"
#include "ImageNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include <random>

namespace LooseAndSketchy2Node {
	using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the LooseAndSketchy2Node class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
LooseAndSketchy2Node::LooseAndSketchy2Node ( const QString &name, ParameterGroup *parameterRoot ) :
    RenderNode(name, parameterRoot)
{
    Parameter *outputParameter = getParameter(m_outputImageName);
    if (outputParameter) {
        // set up parameter affections
        outputParameter->addAffectingParameter(getParameter("Input Map"));

        // set the processing function for the output image parameter
        outputParameter->setProcessingFunction(SLOT(processOutputImage()));
		setChangeFunction("numberSeeds", SIGNAL(triggerRedraw()));
		setChangeFunction("seedingThreshold", SIGNAL(triggerRedraw()));
		setChangeFunction("numberSteps", SIGNAL(triggerRedraw()));
		setChangeFunction("stepSize", SIGNAL(triggerRedraw()));
		setChangeFunction("minSteps", SIGNAL(triggerRedraw()));
		setChangeFunction("strokeContourCurvature", SIGNAL(triggerRedraw()));
		setChangeFunction("strokeThickness", SIGNAL(triggerRedraw()));
    } else
        Log::error("Could not obtain output image parameter.", "LooseAndSketchy2Node::LooseAndSketchy2Node");

}

//!
//! Destructor of the LooseAndSketchy2Node class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
LooseAndSketchy2Node::~LooseAndSketchy2Node ()
{
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void LooseAndSketchy2Node::processOutputImage ()
{
	// obtain input image
	Ogre::TexturePtr inputTexture = getTextureValue("Input Map");
	
	if (inputTexture.isNull()) {
		//Log::warning("No input image connected.", "LooseAndSketchy2Node::processOutputImage");
		inputTexture = m_defaultTexture;
	}	

    if (!m_renderTexture.isNull()) {
        size_t width = inputTexture->getWidth();
        size_t height = inputTexture->getHeight();
		resizeRenderTexture(width, height, Ogre::PF_A8R8G8B8);
		resizeRenderTexture(width, height, Ogre::PF_A8R8G8B8);
		m_viewport->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
    }

    progressLooseAndSketchy2();		
}

///
/// Private Methods
///

void LooseAndSketchy2Node::progressLooseAndSketchy2()
{
    OgreTools::deepDeleteSceneNode(m_sceneManager->getRootSceneNode(), m_sceneManager);
    Ogre::TexturePtr inputTexture = getTextureValue("Input Map");
    if (inputTexture.isNull())
    {
        //Log::warning("No input image connected.", "LooseAndSketchy2Node::progressLooseAndSketchyOffline");
        //return;

		inputTexture = m_defaultTexture;
    }
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("LooseAndSketchy2/LnS2Material");
	Ogre::TextureUnitState *tus = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	tus->setTextureName(inputTexture->getName());

	Ogre::GpuProgramParametersSharedPtr fpParams = material->getTechnique(0)->getPass(0)->getGeometryProgramParameters();
	setShaderParameter(fpParams, "seedingThreshold", getFloatValue("seedingThreshold"));
	setShaderParameter(fpParams, "numberSteps", getIntValue("numberSteps"));
	setShaderParameter(fpParams, "stepSize",getIntValue("stepSize"));
	setShaderParameter(fpParams, "minSteps", getIntValue("minSteps"));
	setShaderParameter(fpParams, "strokeContourCurvature", getFloatValue("strokeContourCurvature"));
	setShaderParameter(fpParams, "strokeThickness", getIntValue("strokeThickness"));

    computeLooseAndSketchy2();

    renderLooseAndSketchy2();

    m_renderTexture->getBuffer()->getRenderTarget()->update();
    setValue("Image", m_renderTexture); 
}

void LooseAndSketchy2Node::computeLooseAndSketchy2()
{
	m_seeds.clear();

	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0.f, 1.f);

	for (int i = 0; i < getIntValue("numberSeeds"); ++i)
	{
		m_seeds.push_back(Vector2d(distribution(generator), distribution(generator)));
	}
}


void LooseAndSketchy2Node::renderLooseAndSketchy2()
{
    Ogre::SceneNode *sceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
	Ogre::ManualObject *manualObj = m_sceneManager->createManualObject(std::string("points"));
	manualObj->setUseIdentityProjection(true);
	manualObj->setUseIdentityView(true);
  
	manualObj->begin("LooseAndSketchy2/LnS2Material", Ogre::RenderOperation::OT_POINT_LIST);

	for (std::vector<Vector2d>::const_iterator iter = m_seeds.begin(); iter != m_seeds.end(); ++iter)
	{
		manualObj->position(iter->x * 2.0 - 1.0, iter->y * 2.0 - 1.0, 0.0);
	}

	manualObj->end();

	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	manualObj->setBoundingBox(aabInf);

	// Render just before overlays
	manualObj->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
	sceneNode->attachObject(manualObj);
}

//!
//! Redraw of ogre scene has been triggered.
//!
void LooseAndSketchy2Node::redrawTriggered ()
{
    progressLooseAndSketchy2();
}

//!
//! Private Helper Methods
//!

//!
//! Force to rebuild the render target. 
//! HACK to restore damaged render targets in GL3+ render system
//! 
void LooseAndSketchy2Node::rebuildRendertargets ()
{
	m_renderTexture->getBuffer()->getRenderTarget()->update();
	setValue(m_outputImageName, m_renderTexture);
}

} // end namespace
