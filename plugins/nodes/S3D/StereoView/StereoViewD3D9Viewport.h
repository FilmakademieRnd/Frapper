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
//! \file "StereoViewD3D9Viewport.h"
//! \brief Header file for StereoViewD3D9Viewport class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!

#ifndef STEREOVIEWD3D9VIEWPORT_H
#define STEREOVIEWD3D9VIEWPORT_H

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <QString>

namespace StereoViewNode {

typedef unsigned int uint;

// Enumerations
enum ImageTarget {
	LEFT_IMAGE,
	RIGHT_IMAGE
};

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class StereoViewD3D9Viewport
{

public: // constructors and destructors

    //!
    //! Constructor of the StereoViewD3D9Viewport class.
    //!
    //!
    StereoViewD3D9Viewport( HWND hWnd );

    //!
    //! Destructor of the StereoViewD3D9Viewport class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~StereoViewD3D9Viewport ();

	void updateSurface( ImageTarget target, IDirect3DSurface9* surface );
	void updateSurface( ImageTarget target, size_t srcWidth, size_t srcHeight, size_t srcRowPitchBytes, D3DFORMAT srcFormat, LPCVOID srcDataLeft );

	void renderFrame();

	void setStereo( bool enabled );
	bool getStereo() {return m_renderStereo;}

	void setSwapEyes( bool enabled );
	bool getSwapEyes() { return m_swapEyes; }

	void setFullscreen( bool enabled );
	bool getFullscreen() {return m_fullscreen;}

	int getHit() { return m_hit; };
	void setHit( int hit ) { m_hit = hit; };

protected: // events

protected: // functions

private: // functions

	void initD3D(HWND hWnd );  // set up and initializes Direct3D
	void initSurface();
	void resetBackbuffer( int backBufferWidth, int backBufferHeight, D3DFORMAT backbufferFormat, bool fullscreen = false);
	void changeResolution( size_t srcWidth, size_t srcHeight, D3DFORMAT srcFormat );

	void CheckForDxError(const char *file, int line, HRESULT hr);
	void WriteStereoHeader();

	void resetDevice();
	void resetContext();

private: //data

	int m_backbufferWidth;
	int m_backbufferHeight;
	D3DFORMAT m_backbufferFormat;

	//!
    //! The source stereo image
    //!
    IDirect3DSurface9 *m_imageSrc;

    //!
    //! The back buffer to write to
    //!
    IDirect3DSurface9 *m_backBuf;

	//!
	//! Pointer to our Direct3D interface
	//!
	IDirect3D9 *m_d3d;

	//!
	//! The pointer to the device class
	//!
	IDirect3DDevice9 *m_d3ddev;

	//!
	//! The font object used to render text
	//!
	ID3DXFont *m_font;

	//!
	//! Describes the presentation parameters.
	//!
	D3DPRESENT_PARAMETERS m_d3dpp;
	
	bool m_renderStereo;

	bool m_swapEyes;

	HWND m_windowHandle;

	bool m_fullscreen;

	int m_hit;

	static QString s_hitMessage;
};

} // end namespace

#endif // STEREOVIEWD3D9VIEWPORT_H
