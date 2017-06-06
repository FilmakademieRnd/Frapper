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

//! \file "StereoViewWindow.cpp"
//! \brief Implementation file for StereoViewWindow class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!


#include "StereoViewWindow.h"

// includes, DX9
#include <assert.h>

#include "OgreTools.h"

// Ogre to D3D9 conversion
#include "OgrePixelFormat.h"
#include <QKeyEvent>

// Ogre_Rendersystem_D3D9
//#include <OgreD3D9HardwarePixelBuffer.h>
//#include <OgreD3D9Mappings.h>

namespace StereoViewNode{
	
///
/// Constructors and Destructors
///

//!
//! Constructor of the StereoViewWindow class.
//!
StereoViewWindow::StereoViewWindow( int width, int height ) : 
	QWidget(),
	m_windowWidth(width),
	m_windowHeight(height),
	m_d3d9Fullscreen(false)
{
	setWindowTitle("Stereo View Window");
	//sizePolicy().setHeightForWidth(true); // keep aspect ratio

	resize( width, height );
	//setUpdatesEnabled(false);

	m_D3D9Viewport = new StereoViewD3D9Viewport( this->winId() );

	show();
	update();
}


//!
//! Destructor of the StereoViewWindow class.
//!

StereoViewWindow::~StereoViewWindow ()
{
	delete m_D3D9Viewport;
}

//!
//! Public functions
//!

void StereoViewWindow::UpdateImage( ImageTarget target, const Ogre::TexturePtr &texture )
{
	if( texture.isNull() )
		return;

	Ogre::HardwarePixelBufferSharedPtr textureBuffer = texture->getBuffer();

	// if provided texture is a D3D9 texture, use it directly
	// TODO: need the d3d device here..
	//Ogre::D3D9HardwarePixelBuffer* pD3D9TextureBuffer = dynamic_cast<Ogre::D3D9HardwarePixelBuffer*>(textureBuffer.getPointer());
	//if( pD3D9TextureBuffer )
	//{
	//	m_D3D9Viewport->updateSurface( StereoViewD3D9Viewport::LEFT_IMAGE, pD3D9TextureBuffer->getSurface());
	//}

	// copy data from texture to surface
	size_t width = texture->getWidth();
	size_t height = texture->getHeight();

	Frapper::OgreTools::HardwareBufferLocker hbl( textureBuffer, Ogre::HardwareBuffer::HBL_READ_ONLY);
	const Ogre::PixelBox &pixBox = hbl.getCurrentLock();

	uint rowPitchBytes = pixBox.rowPitch * Ogre::PixelUtil::getNumElemBytes(pixBox.format);
	D3DFORMAT d3dFormat = getD3DFormat( pixBox.format);

	m_D3D9Viewport->updateSurface( target, pixBox.getWidth(), pixBox.getHeight(), rowPitchBytes, 
											d3dFormat, pixBox.data );

	m_windowWidth = textureBuffer->getWidth();
	m_windowHeight = textureBuffer->getHeight();
}

//int StereoViewWindow::heightForWidth( int w ) const
//{
//	return w * floor( round(m_backbufferHeight / (float)m_backbufferWidth) +.5f);
//}

void StereoViewWindow::resizeEvent( QResizeEvent *event )
{
	update();
}

void StereoViewWindow::keyPressEvent( QKeyEvent *event )
{
	QWidget::keyPressEvent(event);

	if( event->key() == Qt::Key_F )
	{
		toggleFullscreen();
	} 
	else if( event->key() == Qt::Key_S )
	{
		setStereo( !m_D3D9Viewport->getStereo());
	}
	else if( event->key() == Qt::Key_E )
	{
		setSwapEyes( !m_D3D9Viewport->getSwapEyes());
	}
	else if( event->key() == Qt::Key_Plus )
	{
		int hit = m_D3D9Viewport->getHit() +1;
		m_D3D9Viewport->setHit( hit );
		std::cout << "Hit : " << hit << std::endl;
		emit hitChanged();
	}
	else if( event->key() == Qt::Key_Minus )
	{
		int hit = m_D3D9Viewport->getHit() -1;
		m_D3D9Viewport->setHit( hit );
		std::cout << "Hit : " << hit << std::endl;
		emit hitChanged();
	}
	else if( event->key() == Qt::Key_Escape )
	{
		hide();
	}
}

void StereoViewWindow::setStereo( bool enabled )
{
	m_D3D9Viewport->setStereo( enabled);
	update();
}

void StereoViewWindow::setSwapEyes( bool enabled )
{
	m_D3D9Viewport->setSwapEyes( enabled);
	update();
}

void StereoViewWindow::update()
{
	if( isVisible() )
		m_D3D9Viewport->renderFrame();
}

D3DFORMAT StereoViewWindow::getD3DFormat( Ogre::PixelFormat ogrePixelFormat )
{
	switch(ogrePixelFormat)
	{
	case Ogre::PF_L8:
		return D3DFMT_L8;
	case Ogre::PF_L16:
		return D3DFMT_L16;
	case Ogre::PF_A8:
		return D3DFMT_A8;
	case Ogre::PF_A4L4:
		return D3DFMT_A4L4;
	case Ogre::PF_BYTE_LA:
		return D3DFMT_A8L8; // Assume little endian here
	case Ogre::PF_R3G3B2:
		return D3DFMT_R3G3B2;
	case Ogre::PF_A1R5G5B5:
		return D3DFMT_A1R5G5B5;
	case Ogre::PF_R5G6B5:
		return D3DFMT_R5G6B5;
	case Ogre::PF_A4R4G4B4:
		return D3DFMT_A4R4G4B4;
	case Ogre::PF_R8G8B8:
		return D3DFMT_R8G8B8;
	case Ogre::PF_A8R8G8B8:
		return D3DFMT_A8R8G8B8;
	case Ogre::PF_A8B8G8R8:
		return D3DFMT_A8B8G8R8;
	case Ogre::PF_X8R8G8B8:
		return D3DFMT_X8R8G8B8;
	case Ogre::PF_X8B8G8R8:
		return D3DFMT_X8B8G8R8;
	case Ogre::PF_A2B10G10R10:
		return D3DFMT_A2B10G10R10;
	case Ogre::PF_A2R10G10B10:
		return D3DFMT_A2R10G10B10;
	case Ogre::PF_FLOAT16_R:
		return D3DFMT_R16F;
	case Ogre::PF_FLOAT16_GR:
		return D3DFMT_G16R16F;
	case Ogre::PF_FLOAT16_RGBA:
		return D3DFMT_A16B16G16R16F;
	case Ogre::PF_FLOAT32_R:
		return D3DFMT_R32F;
	case Ogre::PF_FLOAT32_GR:
		return D3DFMT_G32R32F;
	case Ogre::PF_FLOAT32_RGBA:
		return D3DFMT_A32B32G32R32F;
	case Ogre::PF_SHORT_RGBA:
		return D3DFMT_A16B16G16R16;
	case Ogre::PF_SHORT_GR:
		return D3DFMT_G16R16;
	case Ogre::PF_DXT1:
		return D3DFMT_DXT1;
	case Ogre::PF_DXT2:
		return D3DFMT_DXT2;
	case Ogre::PF_DXT3:
		return D3DFMT_DXT3;
	case Ogre::PF_DXT4:
		return D3DFMT_DXT4;
	case Ogre::PF_DXT5:
		return D3DFMT_DXT5;
	case Ogre::PF_UNKNOWN:
	default:
		return D3DFMT_UNKNOWN;
	}
}

void StereoViewWindow::mouseDoubleClickEvent(QMouseEvent *e) 
{
	QWidget::mouseDoubleClickEvent(e);
	toggleFullscreen();
}

void StereoViewWindow::toggleFullscreen()
{
	if( m_d3d9Fullscreen )
	{
		// unset qt windowed fullscreen
		setWindowState( windowState() & ~Qt::WindowFullScreen);

		bool bFullscreen = m_D3D9Viewport->getFullscreen();

		if( bFullscreen)
		{
			m_D3D9Viewport->setFullscreen( false );
			resize(m_windowWidth/2, m_windowHeight/2);
			SetWindowPos( this->winId(), HWND_TOP, 0, 0, 800, 600, SWP_FRAMECHANGED | SWP_DRAWFRAME);
		}
		else
		{
			m_D3D9Viewport->setFullscreen( true );
			this->activateWindow();
			this->raise();
		}
		update();
	}
	else
	{
		// reset d3d9 viewport
		if( m_D3D9Viewport->getFullscreen())
			m_D3D9Viewport->setFullscreen(false);

		setWindowState( windowState() ^ Qt::WindowFullScreen); // toggle qt window fullscreen flag

		if( !isFullScreen()) 
		{
			resize(m_windowWidth/2, m_windowHeight/2);
			SetWindowPos( this->winId(), HWND_TOP, 0, 0, 800, 600, SWP_FRAMECHANGED | SWP_DRAWFRAME);
		}
	}
}

void StereoViewWindow::setD3D9Fullscreen( bool enabled )
{
	// set use nativ d3d9 fullscreen
	m_d3d9Fullscreen = enabled;
}

} // end namespace