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
//! \file "GeometryRenderNode.h"
//! \brief Header file for GeometryRenderNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       26.05.2009 (last updated)
//!

#ifndef GEOMETRYRENDERNODE_H
#define GEOMETRYRENDERNODE_H

#include "FrapperPrerequisites.h"
#include "RenderNode.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace Frapper {

//!
//! Base class for all render nodes.
//!
class FRAPPER_CORE_EXPORT GeometryRenderNode : public RenderNode
{

    Q_OBJECT

protected: // static constants

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

    //!
    //! The name of the background color parameter.
    //!
    static const QString BackgroundColorParameterName;


public: // constructors and destructors

    //!
    //! Constructor of the GeometryRenderNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    GeometryRenderNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the GeometryRenderNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~GeometryRenderNode ();

protected slots: //

    //!
	//! Processes the node's input data to generate the node's output image.
	//!
	virtual void processOutputImage ();

protected: // members

    //!
    //! The camera's scene node.
    //!
    Ogre::SceneNode *m_cameraSceneNode;

    //!
    //! The light's scene node.
    //!
    Ogre::SceneNode *m_lightSceneNode;

	//!
    //! The geometry's scene node.
    //!
    Ogre::SceneNode *m_geometrySceneNode;
};

} // end namespace Frapper

#endif
