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
//! \file "PainterNode.cpp"
//! \brief Implementation file for PainterNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       20.07.2009 (last updated)
//!

#include "PainterNode.h"
#include "NumberParameter.h"
#include "Log.h"

namespace PainterNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the PainterNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PainterNode::PainterNode ( const QString &name, ParameterGroup *parameterRoot ) :
    PainterPanel::PainterPanelNode( name, parameterRoot),
	m_ppItemsParameterGroup(0),
	m_outputImage(Ogre::TexturePtr())
{
    // set affections and callback functions
	Parameter* inputImageParameter = getParameter( "Input Image" );

	if( inputImageParameter )
	{
		// react on changes of the input image
		inputImageParameter->setSelfEvaluating(true);

		// update size of (output) mask if input changes
		addAffection( "Input Image", m_outputImageName );

		// process output image when requested by viewport
		setProcessingFunction( m_outputImageName, SLOT(processOutputImage()));
	}

	// manually reset mask
	setCommandFunction( "Reset Mask", SLOT( resetOutputImage() ));
	setCommandFunction( "Update Mask", SLOT( processOutputImage()));

	m_ppItemsParameterGroup = new Frapper::ParameterGroup("Painter Panel Items");
	getParameterRoot()->addParameter(m_ppItemsParameterGroup);

	// Need this extra parameter as m_timeParameter is initialized after the c'tor
	// The change function is called on scene load
	setChangeFunction("Update on frame change", SLOT(setOptions()));

	resetOutputImage();
}



//!
//! Destructor of the PainterNode class.
//!
PainterNode::~PainterNode ()
{
}


///
/// Public Slots
///


//!
//! Processes the node's input data to generate the node's output image.
//!
void PainterNode::processOutputImage()
{
	// update scene to current frame
	updateScene();

	// render scene to mask image
	renderSceneToImage(m_outputImage);
	
	// signal mask image has changed
	setOutputImage(m_outputImage);

	emit triggerRedraw();
}

Ogre::TexturePtr PainterNode::getBackground()
{
	Ogre::TexturePtr background = getTextureValue( "Input Image", true );
	return background;
}

Ogre::TexturePtr PainterNode::getMask()
{
	return Ogre::TexturePtr(); // always start with an empty mask;
}

Frapper::Node* PainterNode::getNode()
{
	return this;
}

Frapper::ParameterGroup* PainterNode::getItemParameterGroup()
{
	return m_ppItemsParameterGroup;
}

void PainterNode::resetOutputImage()
{
	Ogre::TexturePtr inputImagePtr = getTextureValue( "Input Image" );

	int width = 1920, height = 1080;

	if( !inputImagePtr.isNull() ) 
	{
		width = inputImagePtr->getWidth();
		height = inputImagePtr->getHeight();
	}

	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();

	if( !m_outputImage.isNull() )
		textureManager.remove( m_outputImage->getHandle() );

	m_outputImage = textureManager.createManual(
		createUniqueName("PainterNode_MaskTexture"), 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D,
		width, height, 0, 
		Ogre::PF_X8R8G8B8,
		Ogre::TU_DEFAULT );	

	setOutputImage( m_outputImage );
}

void PainterNode::onSceneChanged()
{
	// render scene to mask image
	//renderSceneToImage(m_outputImage);
}

void PainterNode::setOptions()
{
	if( getBoolValue("Update on frame change"))
	{
		// set affection of image to time parameter to handle update on frame change
		m_timeParameter->addAffectedParameter( getParameter(m_outputImageName));
	}
	else
	{
		m_timeParameter->removeAffectedParameter( getParameter(m_outputImageName));
	}

}

void PainterNode::setUpTimeDependencies( NumberParameter *timeParameter, NumberParameter *rangeParameter )
{
  Node::setUpTimeDependencies( timeParameter, rangeParameter);
	setOptions();
}

///
/// Private Functions
///

} // namespace PainterNode
