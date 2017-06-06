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
//! \file "ShadowMapHairRenderNode.h"
//! \brief Header file for ShadowMapHairRenderNode class.
//!
//! \author     Markus Rapp <markus.rapp@filmakademie.de>
//! \version    1.0
//! \date       11.12.2014 (last updated)
//!

#ifndef SHADOWMAPHAIRRENDERNODE_H
#define SHADOWMAPHAIRRENDERNODE_H

#include "GeometryRenderNode.h"
#include "OgreTools.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

#define LIGHT_DEBUG

namespace ShadowMapHairRenderNode {
using namespace Frapper;

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

protected: // data

	Ogre::Technique *m_shadowTechnique;
};

class ShadowListener : public Ogre::SceneManager::Listener
{
public: 
	
	ShadowListener() {};
	~ShadowListener() {};

	void shadowTextureCasterPreViewProj (Ogre::Light *light, Ogre::Camera *camera, size_t iteration);
};


//!
//! Class for a basic render node.
//!
class ShadowMapHairRenderNode : public GeometryRenderNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the ShadowMapHairRenderNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ShadowMapHairRenderNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ShadowMapHairRenderNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ShadowMapHairRenderNode ();

private slots:

	//!
	//! Loads an XML reference data file.
	//!
	//! Is called when the value of the Reference Data File parameter has
	//! changed.
	//!
	//! \return True if the reference data was successfully loaded from file, otherwise False.
	//!
	void loadLightDescriptionFile ();

	//!
    //! Sets the scene light scale value
    //!
    void setLightScale ();

	//!
    //! Sets the scenes shader parameter
    //!
	void setShaderParameter ();

	//!
	//! Sets the number of lights for the sadow calculation
	//!
	void setNbrLights();


public slots: //

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

public:
	
	void redrawTriggered();


private: // functions

	//!
	//! Set shadow hair shader material and caster material. Sets textures and variables of tessellation control and evaluation shader.
	//!
	void setHairShader();

	//!
	//! Returns the greatest componen of the vector
	//!
	//! \return The greatest componen of the vector
	float greatestComponent(const Ogre::Vector3 &vec) const;

	//!
	//! The less then function for light power sorting
	//!
	static bool greaterThan(const QPair<Ogre::Vector3, Ogre::Vector3> &d1, const QPair<Ogre::Vector3, Ogre::Vector3> &d2);

protected: // data

	//!
	//! The scale value for the light positions
	//!
	float m_lightScale;

    //!
    //! The default matrial switch listener.
    //!
	MaterialSwitcher *m_materialSwitchListener;

	//!
	//! The list containing the light positions
	//!
	QList<QPair<Ogre::Vector3, Ogre::Vector3>> m_lightPositions;

#ifdef LIGHT_DEBUG
	//!
    //! The name of the fifth image output parameter.
    //!
    QString m_outputSecondImageName;
#endif

};

} // namespace ShadowMapHairRenderNode 

#endif