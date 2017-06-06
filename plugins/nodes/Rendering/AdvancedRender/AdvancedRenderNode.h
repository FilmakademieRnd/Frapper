/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "AdvancedRenderNode.h"
//! \brief Header file for AdvancedRenderNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Felix Bucella <felix.bucella@filmakademie.de>
//! \version    1.0
//! \date       17.05.2011 (last updated)
//!

#ifndef ADVANCEDRENDERNODE_H
#define ADVANCEDRENDERNODE_H

#include "GeometryRenderNode.h"
#include "OgreTools.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

//#define WRINKLE_DEBUG

namespace AdvancedRenderNode {
using namespace Frapper;

//!
//! Forward declaration for a advanced render node.
//!
class AdvancedRenderNode;


//!
//! class to handle material switching without having to modify scene materials individually
//!
class MaterialSwitcher : public Ogre::MaterialManager::Listener
{

public: // constructors and destructors

    //!
    //! Constructor of the MaterialSwitcher class.
    //!
	MaterialSwitcher ();

    //!
    //! Destructor of the MaterialSwitcher class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~MaterialSwitcher ();

public: // methods
	
    //!
    //! Returns the width of the render target.
    //!
    //! \return The width of the render target.
    //!
	Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,
										  const Ogre::String& schemeName,
										  Ogre::Material* originalMaterial,
					                      unsigned short lodIndex,
										  const Ogre::Renderable* rend);

	//!
    //! Sets the material for the MRTs.
    //!
	void setMat(Ogre::MaterialPtr mat);

protected: // data

	Ogre::Technique *m_technique;
	Ogre::Technique *m_wrinkleMappingTechnique; // for SaC-mapping
};



//!
//! Class for a basic render node.
//!
class AdvancedRenderNode : public GeometryRenderNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the AdvancedRenderNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AdvancedRenderNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AdvancedRenderNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~AdvancedRenderNode ();


protected slots: //

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    virtual void processOutputImage ();

	//!
    //! Sets the material for software or hardware skinning.
    //!
	void EnableHardwareSkinning();

	//!
    //! Sets the material to the special entity render technique if avaiable.
    //!
	void UseEntityTechnique();

public:

	//!
    //! Redraw of ogre scene has been triggered.
    //!
    void redrawTriggered ();

private: // methods
		
	//!
	//! Check existing material for SaC-mapping shader and set the weight-map texture.
	//! 
	//! \return True when SaC-mapping material was found.
	//!
	bool setupWrinkleMaterial ();

	//!
	//! Setup additional textures (blendweight, multi render target).
	//!
	void setupTextures ();

	//!
	//! Create new viewports and bind them to the additional textures (blendweight, multi render target).
	//!
	void rebindViewports (Ogre::Camera *camera);

protected: // data

    //!
    //! The default scene light.
    //!
    Ogre::Light *m_sceneLight;

	//!
    //! The multi render target.
    //!
	Ogre::MultiRenderTarget *m_multiRenderTarget;

    //!
    //! The default matrial switch listener.
    //!
	MaterialSwitcher *m_materialSwitchListener;

	//!
	//! The advanced render material.
	//!
	QHash<Ogre::String, Ogre::MaterialPtr> m_renderMatHash;

	//!
    //! The wrinkle ID texture.
    //!
    Ogre::TexturePtr m_blendWeightTexture;

	//!
    //! The name of the object blend weight texture.
    //!
	Ogre::String m_blendWeightTextureName;

	//!
    //! The name of the multible render target.
    //!
	Ogre::String m_multiRenderTargetName;


#ifdef WRINKLE_DEBUG
	//!
    //! The name of the wrinkle weight image output parameter.
    //!
    QString m_outputWrinkleImageName;
#endif
};

} // namespace AdvancedRenderNode 

#endif