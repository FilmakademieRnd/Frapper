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
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "CompositorNode.h"
//! \brief Header file for CompositorNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       26.05.2009 (last updated)
//!

#ifndef COMPOSITORNODE_H
#define COMPOSITORNODE_H

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
class FRAPPER_CORE_EXPORT CompositorNode : public RenderNode, public Ogre::CompositorInstance::Listener
{
    Q_OBJECT
	ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the CompositorNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    CompositorNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the CompositorNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CompositorNode ();

public slots:
	//!
	//! Force to rebuild the render target. 
	//! 
	virtual void rebuildRendertargets ();

protected slots:

    //!
    //! Reload resources.
    //!
    virtual void reload ();

	//!
	//! Sets uv transform parameters.
	//!
	virtual void setUVTransform ();


protected: // functions

	//!
	//!	Resizes the main render texture (m_renderTexture).
	//!
	//!	\param width The width of render texture.
	//!	\param height The height of render texture.
    //!
    virtual void resizeRenderTexture(int width, int height, Ogre::PixelFormat pixelFormat = Ogre::PF_R8G8B8);

    //!
    //! Callback when instance of this class is registered as Ogre::CompositorListener.
    //! 
    //! \param pass_id Id to identifiy current compositor pass.
    //! \param mat Material this pass is currently using.
    //!
    virtual void notifyMaterialRender ( Ogre::uint32 pass_id, Ogre::MaterialPtr &mat );

private: // functions
	
	//!
	//! Called when scene load is ready to prevent wrong recource setup in Ogre
	//!
	virtual void loadReady ();

protected: // data

    //!
    //! The compositor to use for rendering.
    //!
    Ogre::CompositorInstance *m_compositor;

    //!
    //! The resource group name.
    //!
    QString m_resourceGroupName;

	//!
    //! The matric for the uv transformation.
    //!
	Ogre::Matrix4 m_uvTransformMat;
};

} // end namespace Frapper

#endif
