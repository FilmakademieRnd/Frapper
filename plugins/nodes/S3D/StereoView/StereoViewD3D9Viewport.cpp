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
//! \file "StereoViewD3D9Viewport.cpp"
//! \brief Implementation file for StereoViewD3D9Viewport class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!


#include "StereoViewD3D9Viewport.h"
#include "StereoImageHeader.h"

// includes, DX9
#include <DxErr.h>
#include <iostream>
#include <QKeyEvent>
#include <assert.h>

namespace StereoViewNode{

#define CheckHr(hr) CheckForDxError(__FILE__,__LINE__,hr)
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

QString StereoViewD3D9Viewport::s_hitMessage = QString("Current HIT: %1");

///
/// Constructors and Destructors
///

//!
//! Constructor of the StereoViewD3D9Viewport class.
//!
StereoViewD3D9Viewport::StereoViewD3D9Viewport( HWND hWnd ) :
	m_windowHandle(hWnd),
	m_fullscreen(false),
	m_imageSrc(NULL),
    m_backBuf(NULL),
	m_d3d(NULL),
	m_d3ddev(NULL),
	m_font(NULL),
	m_backbufferWidth(1920),
	m_backbufferHeight(1080),
	m_backbufferFormat(D3DFMT_A8R8G8B8),
	m_renderStereo( true ),
	m_swapEyes( false ),
	m_hit(0)
{
	// Init direct3D on given window handle
	initD3D( hWnd );

	// Init input images
	initSurface();

	// render to backbuffer
	renderFrame();
}


//!
//! Destructor of the StereoViewD3D9Viewport class.
//!

StereoViewD3D9Viewport::~StereoViewD3D9Viewport ()
{
	SAFE_RELEASE( m_imageSrc );
	SAFE_RELEASE( m_backBuf );
	SAFE_RELEASE( m_font );
	SAFE_RELEASE( m_d3ddev );    // close and release the 3D device
	SAFE_RELEASE( m_d3d );    // close and release Direct3D
}

//!
//! Private functions
//!

// this function initializes and prepares Direct3D for use
void StereoViewD3D9Viewport::initD3D( HWND hWnd)
{
	// create the Direct3D interface
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));    // clear out the struct for use
	m_d3dpp.Windowed = !m_fullscreen;		// windowed or fullscreen
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	m_d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	m_d3dpp.BackBufferFormat = m_backbufferFormat;  // set the back buffer format to 32 bit
	m_d3dpp.BackBufferWidth = m_backbufferWidth;
	m_d3dpp.BackBufferHeight = m_backbufferHeight;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	m_d3dpp.BackBufferCount = 1;

	if( m_fullscreen )
	{
		if( m_backbufferHeight == 1080 )
			m_d3dpp.FullScreen_RefreshRateInHz = 24; // set 1080p@24Hz
		else
			m_d3dpp.FullScreen_RefreshRateInHz = 60; // set 720p@60Hz
	}


	// create a device class using this information and information from the m_d3dpp stuct
	CheckHr( m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									hWnd,
									D3DCREATE_SOFTWARE_VERTEXPROCESSING,
									&m_d3dpp,
									&m_d3ddev));

	CheckHr( m_d3ddev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backBuf));

	CheckHr( D3DXCreateFont( m_d3ddev, 22, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_DONTCARE, "Arial", &m_font));
}


void StereoViewD3D9Viewport::initSurface()
{
	SAFE_RELEASE( m_imageSrc);

	// set up destRect
	RECT destRectL = {0, 0, m_backbufferWidth, m_backbufferHeight};

	// create the surface
	CheckHr( m_d3ddev->CreateOffscreenPlainSurface(m_backbufferWidth *2, m_backbufferHeight+1, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_imageSrc, NULL));

	WriteStereoHeader();
}

void StereoViewD3D9Viewport::updateSurface( ImageTarget target, IDirect3DSurface9* surface )
{
	D3DSURFACE_DESC srcDesc;
	CheckHr( surface->GetDesc(&srcDesc));
	
	bool resolutionChanged = false;
	if( m_backbufferWidth != srcDesc.Width || m_backbufferHeight != srcDesc.Height )
	{
		changeResolution( srcDesc.Width, srcDesc.Height, srcDesc.Format );
		resolutionChanged = true;
	}

	// set up src rect & destPoint
	RECT srcRect = { 0, 0, srcDesc.Width, srcDesc.Height };
	POINT destPoint = {0,0};
	
	// shift position of right image
	if( target == RIGHT_IMAGE )
	{
		destPoint.x = m_backbufferWidth;
	
		// set up HIT
		if( m_hit > 0 ) 
		{
			destPoint.x += m_hit;   // move destRect to left
			srcRect.right -= m_hit; // crop on right
		}
		else if( m_hit < 0)
		{
			srcRect.left -= m_hit; // crop on left, hit is negative
		}
	}

	// load left image
	CheckHr( m_d3ddev->UpdateSurface( surface, &srcRect, m_imageSrc, &destPoint));

	if( resolutionChanged)
		WriteStereoHeader();
}

void StereoViewD3D9Viewport::updateSurface( ImageTarget target, size_t srcWidth, size_t srcHeight, size_t srcRowPitchBytes, D3DFORMAT srcFormat, LPCVOID srcDataLeft )
{
	bool resolutionChanged = false;
	if( m_backbufferWidth != srcWidth || m_backbufferHeight != srcHeight )
	{
		changeResolution(srcWidth, srcHeight, srcFormat);
		resolutionChanged = true;
	}

	// set up srcRect and destRect
	RECT srcRect = { 0, 0, srcWidth, srcHeight };
	RECT dstRect;

	if( target == LEFT_IMAGE )
	{
		SetRect( &dstRect, 0, 0, m_backbufferWidth, m_backbufferHeight );
		
		if( m_hit > 0 ) {
			srcRect.left  += m_hit; // crop src on left
			dstRect.right -= m_hit; // move destRect to right to avoid scaling
		}
		else if( m_hit < 0) {
			srcRect.right += m_hit; // crop src on right
			dstRect.left  -= m_hit; // move destRect to left to avoid scaling
		}
	} 
	else /*if( target == RIGHT_IMAGE )*/
	{
		SetRect( &dstRect, m_backbufferWidth, 0, 2*m_backbufferWidth, m_backbufferHeight );

		if( m_hit > 0 ) {
			srcRect.right -= m_hit; // crop on right to avoid scaling
			dstRect.left  += m_hit; // move destRect to left
		}
		else if( m_hit < 0) {
			srcRect.left  -= m_hit; // crop on left to avoid scaling
			dstRect.right += m_hit; // crop on right, hit is negative!
		}
	}

	// load left image
	CheckHr( D3DXLoadSurfaceFromMemory( m_imageSrc, NULL, &dstRect, srcDataLeft, srcFormat, srcRowPitchBytes, NULL, &srcRect, D3DX_DEFAULT, 0));

	if( resolutionChanged)
		WriteStereoHeader();
}


void StereoViewD3D9Viewport::renderFrame()
{
	if( !m_d3ddev) return;

	// clear the window to a deep blue
    m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

	m_d3ddev->BeginScene();    // begins the 3D scene

	RECT destRect = { 0, 0, m_backbufferWidth, m_backbufferHeight};

    // Get the back buffer then Stretch the Surface on it.
    CheckHr( m_d3ddev->StretchRect( m_imageSrc, NULL, m_backBuf, &destRect, D3DTEXF_NONE));

	const char* hit_message = s_hitMessage.arg(m_hit).toStdString().c_str();
	RECT font_rect;

	// draw text for left eye
	SetRect( &font_rect, 0, 0, m_backbufferWidth, m_backbufferHeight);
	CheckHr( m_font->DrawText( NULL, hit_message, -1, &font_rect, DT_LEFT|DT_NOCLIP, 0x000000FF));
	
	// draw text for right eye
	SetRect( &font_rect, m_backbufferWidth/2, 0, m_backbufferWidth, m_backbufferHeight);
	CheckHr( m_font->DrawText( NULL, hit_message, -1, &font_rect, DT_LEFT|DT_NOCLIP, 0x000000FF));

    m_d3ddev->EndScene();    // ends the 3D scene
    m_d3ddev->Present(NULL, NULL, NULL, NULL);    // displays the created frame

}

void StereoViewD3D9Viewport::WriteStereoHeader()
{
	// Lock the stereo image
	D3DLOCKED_RECT lr;
	CheckHr( m_imageSrc->LockRect(&lr,NULL,0));

	// write stereo signature in the last raw of the stereo image
	LPNVSTEREOIMAGEHEADER pSIH =
		(LPNVSTEREOIMAGEHEADER)(((unsigned char *) lr.pBits) + (lr.Pitch * (m_backbufferHeight)));

	// Update the signature header values
	pSIH->dwSignature = m_renderStereo ? NVSTEREO_IMAGE_SIGNATURE : 0;
	pSIH->dwFlags = m_swapEyes ? SIH_SWAP_EYES : 0;
	pSIH->dwBPP = 32;
	pSIH->dwWidth = m_backbufferWidth*2;
	pSIH->dwHeight = m_backbufferHeight;

	// Unlock surface
	CheckHr( m_imageSrc->UnlockRect());	
}

void StereoViewD3D9Viewport::resetBackbuffer( int backbufferWidth, int backbufferHeight, D3DFORMAT backbufferFormat, bool fullscreen )
{
	m_backbufferWidth = backbufferWidth;
	m_backbufferHeight = backbufferHeight;
	m_backbufferFormat = backbufferFormat;

	m_d3dpp.BackBufferHeight = backbufferHeight;
	m_d3dpp.BackBufferWidth  = backbufferWidth;
	m_d3dpp.BackBufferFormat = backbufferFormat;

	SAFE_RELEASE( m_backBuf );
	SAFE_RELEASE( m_font );

	if( m_d3ddev)
		m_d3ddev->Reset( &m_d3dpp);

	CheckHr( m_d3ddev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backBuf));
	CheckHr( D3DXCreateFont( m_d3ddev, 22, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_DONTCARE, "Arial", &m_font));

}

void StereoViewD3D9Viewport::CheckForDxError( const char *file, int line, HRESULT hr )
{
    if (!FAILED(hr))
        return;

    // Get the direct X error and description
    char desc[1024];
    sprintf_s(desc, "(DX) %s - %s", DXGetErrorString(hr), DXGetErrorDescription(hr));

    // Output the file and line number in the correct format + the above DX error
    char buf[2048];
    sprintf_s(buf, "%s(%d) : Error: %s\n", file, line, desc);
    OutputDebugString( buf);

    // Cause the debugger to break here so we can fix the problem
    DebugBreak();
}

void StereoViewD3D9Viewport::setStereo( bool enabled )
{
	m_renderStereo = enabled;

	// reset device to allow setting the stereo flag
	resetDevice();

	// update viewport with new stereo settings
	renderFrame();
}	

void StereoViewD3D9Viewport::setSwapEyes( bool enabled )
{
	m_swapEyes = enabled;

	// write updated stereo header
	WriteStereoHeader();

	// update viewport with new settings
	renderFrame();
}

void StereoViewD3D9Viewport::resetDevice()
{
	// save current image content
	LPD3DXBUFFER pBuffer = NULL;
	D3DXSaveSurfaceToFileInMemory( &pBuffer, D3DXIFF_DDS, m_imageSrc, NULL, NULL);

	// get file information
	D3DXIMAGE_INFO pBufferInfo;
	D3DXGetImageInfoFromFileInMemory( pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), &pBufferInfo);

	// free image surface so that device can be reset
	SAFE_RELEASE(m_imageSrc);
	resetBackbuffer( m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight, m_d3dpp.BackBufferFormat );

	// create the image surface again
	CheckHr( m_d3ddev->CreateOffscreenPlainSurface( m_backbufferWidth *2, m_backbufferHeight+1, m_d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &m_imageSrc, NULL));

	// write image back to surface
	D3DXLoadSurfaceFromFileInMemory( m_imageSrc, NULL, NULL, pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), NULL, D3DX_FILTER_NONE, 0, &pBufferInfo);

	WriteStereoHeader();

	// free the image buffer
	pBuffer->Release();
}

void StereoViewD3D9Viewport::setFullscreen( bool enabled )
{
	if( m_fullscreen != enabled)
	{
		m_fullscreen = enabled;
		m_d3dpp.Windowed = !m_fullscreen;
		resetDevice();
	}
}

void StereoViewD3D9Viewport::changeResolution( size_t srcWidth, size_t srcHeight, D3DFORMAT srcFormat )
{
	SAFE_RELEASE(m_imageSrc);

	resetBackbuffer( srcWidth, srcHeight, srcFormat);

	// re-create the source image with new resolution
	CheckHr( m_d3ddev->CreateOffscreenPlainSurface( m_backbufferWidth *2, m_backbufferHeight+1, srcFormat, D3DPOOL_DEFAULT, &m_imageSrc, NULL));
}

} // end namespace
