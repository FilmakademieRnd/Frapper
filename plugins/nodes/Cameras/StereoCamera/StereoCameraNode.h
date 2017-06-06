/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

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
//! \file "StereoCameraNode.h"
//! \brief Header file for StereoCameraNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       07.02.2012 (created)
//! \date       07.02.2012 (last updated)
//!

#ifndef STEREOCAMERANODE_H
#define STEREOCAMERANODE_H

#include "ImageNode.h"
#include "OgreTools.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace StereoCameraNode {
using namespace Frapper;

//!
//! Class for a stereo render node.
//! Although its a render node, it does not implement the interface of RenderNode.h,
//! as the StereoCameraNode uses the default Scene Manager provided by the OgreManager,
//! and therefore uses the standard frapper rendering context.
//!
class StereoCameraNode : public ImageNode
{

    Q_OBJECT

protected: // static constants

    //!
    //! The name of the background color parameter.
    //!
    static const QString BackgroundColorParameterName;

    //!
    //! The default render width.
    //!
    static const int DefaultRenderWidth = 1200;

    //!
    //! The default render height.
    //!
    static const int DefaultRenderHeight = 720;

    //!
    //! The default gamma.
    //!
    static float DefaultGamma;

    //!
    //! The default resolution multiplier literals.
    //!
    static QStringList resLiterals;

    //!
    //! The default resolution multiplier values.
    //!
    static QStringList resValues;

    //!
    //! The name of the input geometry parameter.
    //!
    static const QString InputGeometryParameterName;

    //!
    //! The name of the input lights parameter.
    //!
    static const QString InputLightsParameterName;

    //!
    //! The name of the input camera parameter.
    //!
    static const QString InputCameraParameterName;

    //!
    //! The name of the camera light toggle parameter.
    //!
    static const QString CameraLightToggleParameterName;


public: // constructors and destructors

    //!
    //! Constructor of the StereoCameraNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    StereoCameraNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the StereoCameraNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~StereoCameraNode ();

public: // methods

    //!
    //! Returns the width of the render target.
    //!
    //! \return The width of the render target.
    //!
    unsigned int getWidth() const;

    //!
    //! Returns the height of the render target.
    //!
    //! \return The height of the render target.
    //!
    unsigned int getHeight() const; 


public slots: //

    //!
    //! Redraw of ogre scene has been triggered.
    //!
    virtual void redrawLeftTriggered ();
    virtual void redrawRightTriggered ();


protected: // methods

    //!
    //! Create a viewport for all RenderTextures (RenderTargets) in m_textureHash.
    //!
    //!	\param cam Camera to create a Ogre::Viewport.
    //!	
    void initializeRenderTargets( Ogre::ColourValue bkColor = Ogre::ColourValue(0,0,0), Ogre::PixelFormat format = Ogre::PF_R8G8B8, bool clearFrame = true);

    //!
    //! Clean up all textures
    //!
    void finalizeRenderTargets();

    //!
    //! Clean up the render texture
    //!
    void removeRenderTexture(Ogre::TexturePtr &texture);


protected: // data

    //!
    //! The scene manager for building the scene to render.
    //!
    Ogre::SceneManager *m_sceneManager;


    //!
    //! The viewport.
    //!
    Ogre::Viewport *m_viewportL, *m_viewportR;


    //!
    //! The renderer's render texture.
    //!
    Ogre::TexturePtr m_renderTextureL, m_renderTextureR;

    //!
    //! The render texture name.
    //!
    Ogre::String m_renderTextureNameL, m_renderTextureNameR;

    //!
    //! The width of the image to render.
    //!
    int m_renderWidth;

    //!
    //! The height of the image to render.
    //!
    int m_renderHeight;

    //!
    //! The multiplier for the render target size.
    //!
    float m_targetMultiplier;

    //!
    //! The default texture.
    //!
    Ogre::TexturePtr m_defaultTexture;

};
} // namespace StereoCameraNode

#endif