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
//! \file "StereoViewWindow.h"
//! \brief Header file for StereoViewWindow class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!

#ifndef STEREOVIEWWINDOW_H
#define STEREOVIEWWINDOW_H

#include <QWidget>
#include "StereoViewD3D9Viewport.h"

#include "OgrePixelFormat.h"
#include "d3d9.h"

namespace StereoViewNode {

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class StereoViewWindow : public QWidget
{
    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the StereoViewWindow class.
    //!
    //!
    StereoViewWindow( int width = 400, int height = 300);

    //!
    //! Destructor of the StereoViewWindow class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~StereoViewWindow ();

	void UpdateImage( ImageTarget target, const Ogre::TexturePtr &texture );

public slots:
	void update();
	void setStereo( bool enabled);
	void setSwapEyes( bool enabled);
	void setD3D9Fullscreen( bool enabled);

signals:
	void hitChanged();

protected: // events

	void mouseDoubleClickEvent(QMouseEvent *e);


protected: // functions

	void resizeEvent( QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

private slots:

private: // functions
	
	D3DFORMAT getD3DFormat( Ogre::PixelFormat ogrePixelFormat);

	void toggleFullscreen();

	int m_windowWidth;
	int m_windowHeight;

	bool m_d3d9Fullscreen;

private: //data

	StereoViewD3D9Viewport* m_D3D9Viewport;

};

} // end namespace

#endif // STEREOVIEWWINDOW_H
