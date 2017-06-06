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
//! \file "StereoViewNode.cpp"
//! \brief Implementation file for StereoViewNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!


#include "StereoViewNode.h"

// access to Frapper Parameters
#include "Parameter.h"

// Ogre Pixelbuffer stuff
#include "OgrePixelFormat.h"
#include "OgreHardwareBuffer.h"
#include "OgreHardwarePixelBuffer.h"

namespace StereoViewNode {
	using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the StereoViewNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
StereoViewNode::StereoViewNode ( const QString &name, ParameterGroup *parameterRoot ) :
    CompositorNode( name, parameterRoot ),
	m_window(NULL),
	m_leftInputImageDirty(false),
	m_rightInputImageDirty(false),
	m_viewMode(INTERLACED)
{
	// create compositor and add register callback function
	if (m_viewport)
	{
		m_compositor = Ogre::CompositorManager::getSingleton().addCompositor(m_viewport, "InterlacedCompositor" );
		if (m_compositor) 
		{
			m_compositor->addListener(this);
		}
	}

	loadStereoTestImage();
	load3DVisionImage();

	// update is triggered by viewport
	setProcessingFunction( m_outputImageName, SLOT(processOutputImage()));

	// update output image if left image changed
	addAffection("Left Image", m_outputImageName);

	// update output image if right image changed
	addAffection("Right Image", m_outputImageName);

	// listen for image input parameters getting dirtied
	Parameter* leftInputImageParameter  = getParameter("Left Image");
	Parameter* rightInputImageParameter = getParameter("Right Image");
	Parameter* outputImageParameter = getParameter(m_outputImageName);
	connect( leftInputImageParameter,  SIGNAL(dirtied()), this, SLOT(leftImageDirty()));
	connect( rightInputImageParameter, SIGNAL(dirtied()), this, SLOT(rightImageDirty()));

	// listen to change of the View Mode parameter
	setChangeFunction("View Mode", SLOT(ViewModeChanged()));

	// setup command functions
	setCommandFunction("3D Vision Options > Open Stereo Viewer", SLOT(openStereoWindow()));

	// setup change functions
	setChangeFunction("3D Vision Options > Stereo", SLOT(enableStereo()));
	setChangeFunction("3D Vision Options > Swap Eyes", SLOT(swapEyes()));
	setChangeFunction("3D Vision Options > Use Direct3D9 Fullscreen", SLOT(setFullscreenMode()));

	// init default values
	ViewModeChanged();
}


//!
//! Destructor of the AqtBlendNode class.
//!

//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
StereoViewNode::~StereoViewNode ()
{
	if( m_window)
		delete m_window;

	if( m_compositor)
		m_compositor->setEnabled(false);
}

//!
//! Protected Events
//!

///
/// Private Slots
///


//!
//! Process the output image, which also updates the StereoView Window
//!
void StereoViewNode::processOutputImage()
{
	if( m_leftInputImageDirty && m_rightInputImageDirty )
	{
		// display test images per default
		Ogre::TexturePtr leftTexture  = m_testImageLeft;
		Ogre::TexturePtr rightTexture = m_testImageRight;

		// get image input parameters
		Parameter* leftImageInputParameter  = getParameter( "Left Image" );
		Parameter* rightImageInputParameter = getParameter( "Right Image" );

		if( leftImageInputParameter->isConnected() && rightImageInputParameter->isConnected())
		{
			// get input textures
			leftTexture  = leftImageInputParameter->getValue(true).value<Ogre::TexturePtr>();
			rightTexture = rightImageInputParameter->getValue(true).value<Ogre::TexturePtr>();
		}


		if( leftTexture.isNull() || rightTexture.isNull())
			return;

		if( m_viewMode == NVISION)
		{
			assert(m_window);
			m_window->UpdateImage( LEFT_IMAGE, leftTexture);
			m_window->UpdateImage( RIGHT_IMAGE, rightTexture);

			// update stereoview window
			m_window->update();
		}
		else /*if( m_viewMode == INTERLACED)*/
		{
			if (m_compositor)
				m_compositor->setEnabled( false);

			setupTextures( leftTexture, rightTexture);

			if (m_compositor)
				m_compositor->setEnabled( true);

			//render and set output
			setValue( m_outputImageName, m_renderTexture );
			getParameter(m_outputImageName)->setDirty(false);
		}

		m_leftInputImageDirty = false;
		m_rightInputImageDirty = false;
	}
}

void StereoViewNode::openStereoWindow()
{
	if( m_window)
	{
		m_window->show();
		m_window->update();
	}
}

void StereoViewNode::enableStereo()
{
	if( m_window)
		m_window->setStereo( getBoolValue("3D Vision Options > Stereo"));
}

void StereoViewNode::swapEyes()
{
	if( m_window)
		m_window->setSwapEyes( getBoolValue("3D Vision Options > Swap Eyes"));
}

void StereoViewNode::setFullscreenMode()
{
	if( m_window)
		m_window->setD3D9Fullscreen( getBoolValue("3D Vision Options > Use Direct3D9 Fullscreen"));
}

void StereoViewNode::notifyMaterialRender( Ogre::uint32 pass_id, Ogre::MaterialPtr &mat )
{
}

void StereoViewNode::leftImageDirty()
{
	m_leftInputImageDirty = true;
}

void StereoViewNode::rightImageDirty()
{
	m_rightInputImageDirty = true;
}

void StereoViewNode::updateView()
{
	leftImageDirty();
	rightImageDirty();
	processOutputImage();
}

void StereoViewNode::ViewModeChanged()
{
	EnumerationParameter* viewModeParameter = getEnumerationParameter("View Mode");
	if( viewModeParameter->getCurrentLiteral() == "NVIDIA 3D Vision")
	{
		m_viewMode = NVISION;
		
		if( m_compositor)
			m_compositor->setEnabled(false);

		// create the StereoViewWindow
		m_window = new StereoViewWindow( 1280, 720);

		// Init options
		enableStereo();
		swapEyes();
		setFullscreenMode();

		// update view whenever hit was changed
		connect( m_window, SIGNAL(hitChanged()), this, SLOT(updateView()));

		// Display 3D Vision image in frapper viewport
		setValue( m_outputImageName, m_NV3DImage );
		updateView();
	}
	else /*if( viewModeParameter->getCurrentLiteral() == "Interlaced")*/
	{
		m_viewMode = INTERLACED;

		if( m_window)
		{
			delete m_window;
			m_window = NULL;
		}
		
		// Set output resolution to native resolution of monitor
		// which displays the interlaced images in fullscreen
		resizeRenderTexture(1920, 1080); 

		if( m_compositor)
			m_compositor->setEnabled(false);

		setValue( m_outputImageName, m_renderTexture );

		updateView();
	}
	emit viewNodeUpdated();
}

void StereoViewNode::setupTextures( Ogre::TexturePtr &leftTexture, Ogre::TexturePtr &rightTexture )
{
	// setup textures
	Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName("Interlaced/InterlacedMaterial");

	if( !materialPtr.isNull() )
	{
		if( !leftTexture.isNull() )
			setTexture( materialPtr, leftTexture, 0);
		else
			setTexture( materialPtr, m_defaultTexture, 0);

		if( !rightTexture.isNull() )
			setTexture( materialPtr, rightTexture, 1);
		else
			setTexture( materialPtr, m_defaultTexture, 1);
	}
}

void StereoViewNode::load3DVisionImage()
{
	Ogre::Image defaultImage;
	defaultImage.load("3DVision.jpg", "General");

	m_NV3DImage = Ogre::TextureManager::getSingleton().loadImage( "3DVisionImage", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, defaultImage,
		Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_A8R8G8B8);
}

void StereoViewNode::loadStereoTestImage()
{
	Ogre::TextureManager& textureManager =  Ogre::TextureManager::getSingleton();
	Ogre::Image testImageLeft;
	testImageLeft.load("StereoTest_1920x1080_L.jpg", "General");
	m_testImageLeft = textureManager.loadImage( "StereoTest_1920x1080_L", 
												Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
												testImageLeft, Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_A8R8G8B8);

	Ogre::Image testImageRight;
	testImageRight.load("StereoTest_1920x1080_R.jpg", "General");
	m_testImageRight = textureManager.loadImage( "StereoTest_1920x1080_R", 
												Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
												testImageRight, Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_A8R8G8B8);
}

} // end namespace