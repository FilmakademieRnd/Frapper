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
//! \file "HairRenderNode.h"
//! \brief Header file for HairRenderNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       11.10.2010 (last updated)
//!

#ifndef HAIRRENDERNODE_H
#define HAIRRENDERNODE_H

#include "GeometryRenderNode.h"
#include "OgreTools.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

#define LIGHT_DEBUG

namespace HairRenderNode {
using namespace Frapper;

class ShadowTargetListener : public Ogre::RenderTargetListener
{
public: 
	ShadowTargetListener();
	~ShadowTargetListener() {};

	void preRenderTargetUpdate (const Ogre::RenderTargetEvent &evt);
	void setDepthBias(const Ogre::Real bias);
	void setDepthOffset(const Ogre::Real offset);

private:
	Ogre::Pass *m_shadowCasterPass;
	Ogre::Real m_depthBias;
	Ogre::Real m_depthOffset;
};

//!
//! Class for a basic render node.
//!
class HairRenderNode : public GeometryRenderNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the HairRenderNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    HairRenderNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the HairRenderNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~HairRenderNode ();

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

	void setPassDepthBias();

	void setPassDepthOffset();

public slots: //

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

public:
	void redrawTriggered();


private: // functions

	//!
	//! Returns the greatest componen of the vector
	//!
	//! \return The greatest componen of the vector
	float greatestComponent(const Ogre::Vector3 &vec) const;

	//!
	//! Insterts a custom vertex shader to rendering pipeline
	//!
	void setCustomVertexShader();

	//!
	//! The less then function for light power sorting
	//!
	static bool greaterThan(const QPair<Ogre::Vector3, Ogre::Vector3> &d1, const QPair<Ogre::Vector3, Ogre::Vector3> &d2);

protected: // data

    //!
    //! The default scene light.
    //!
    Ogre::Light *m_sceneLight;
    
    //!
    //! Camera for RenderNode scene.
    //!
	Ogre::Camera *m_cameraCopy;

	//!
	//! The list containing the light positions
	//!
	QList<QPair<Ogre::Vector3, Ogre::Vector3>> m_lightPositions;

	//!
	//! The Pointer to the shadow caster render listener
	//!
	ShadowTargetListener *m_shadowListener;

	//!
	//! The scale value for the light positions
	//!
	float m_lightScale;


#ifdef LIGHT_DEBUG
	//!
    //! The name of the fifth image output parameter.
    //!
    QString m_outputSecondImageName;
#endif

};

} // namespace HairRenderNode 

#endif