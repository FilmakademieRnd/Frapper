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
//! \file "TextureGeometry.h"
//! \brief Header file for TextureGeometry class.
//!
//! \author     Amit Rojtblat <amit.rojtblat@filmakademie.de>
//! \version    1.0
//! \date       22.02.2014 (last updated)
//!

#ifndef TEXTUREGEOMETRYNODE_H
#define TEXTUREGEOMETRYNODE_H

#include "TextureGeometryNodeAbstract.h"
#include "Ogre.h"
#include "OgreTools.h"

namespace Frapper {

//!
//! Class representing nodes generate a texture map of position data on a rendertarget texture.
//!
class FRAPPER_CORE_EXPORT TextureGeometryNode : public TextureGeometryNodeAbstract
{

    Q_OBJECT


public: // constructors and destructors

    //!
    //! Constructor of the TextureGeometry class.
    //!
    //! \param name The name to give the new TextureGeometry node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TextureGeometryNode ( const QString &name, ParameterGroup *parameterRoot);

    //!
    //! Destructor of the TextureGeometry class.
    //!
    ~TextureGeometryNode ();

	//!
	//! Creates a NEW Ogre::TexturePtr and assignes it to a new output parameter
	//!
	//! \param parentNode the node that will process this texture target
    //! \param name base name to give to the texture. Will be made unique and assigned to the class.
    //! \param resolution the resolution of the texture (uniform square)
	//! \param format the format of the OgreTexturePtr
	//! \param points per primitive how many points can the geo shader of this target produce per primitive for calculating amount of needed created points in the update function
	//! \param flatMaterial the material to use for calculating the output texture
	//! \param actualMaterial the material to use for actual screen shading
	//!
	Parameter* addOutputRenderTargetImage (	const QString& name,
										const QString& parameterName,
										int resolution, 
										Ogre::PixelFormat format, 
										int pointsPerPrimitive, 
										Ogre::MaterialPtr& material,
										Ogre::MultiRenderTarget* mrt=0);

	//!
	//! Add an input texture (textureUnitState) to output material
	//!
	Ogre::TextureUnitState* addInputTextureToOutputTarget(const Ogre::TexturePtr& t ,const Ogre::MaterialPtr& m);

	//!
	//! Get the image to display
	//!
	Ogre::TexturePtr getImage();

	//!
	//! get the camera
	//!
	Ogre::Camera* getCamera();

public slots: //

	//!
    //! upadte all outputs
    //!
	void updateAllOutputs();

private: // functions

	//!
    //! Removes all the output texture targets and their structs
    //!
    void destroyTextureTargets();

	//!
	//! Init the camera
	//!
	Ogre::Camera* initCamera();

	//!
	//! add render target to mrt
	//!
	int addRenderTargetToMRT(Ogre::MultiRenderTarget* mrt, Ogre::RenderTexture* target);

private slots: //

	//!
    //! upadte output  (preccessing function)
    //!
	void updateOutput();

	//!
    //! Processing function of the maximum point count parameter
	//! Here does nothing, but is implemented differently in the modify node and the shader node
    //!
	virtual void setMaxPointCountProcessingFunction();

private: // data

	//!
	//! the scene manager for this render target to hold the object in
	//!
	Ogre::SceneManager *m_PrivateSceneManager;

	//!
	//! the camera for the target rendering
	//!
	Ogre::Camera* m_camera;

	//!
	//! parameter for choosing which output image to display
	//!
	EnumerationParameter *m_enumImageParameter;

	//!
	//! a map of output parameters and their materials
	//!
	QMap<QString,Ogre::MaterialPtr> m_outputParameterToMaterialMap;

	//!
	//! a map of output parameters and their MRTs
	//!
	QMap<QString,Ogre::MultiRenderTarget*> m_outputParameterRenderTargetMap;

	//!
	//! a list of all mrts in the node
	//!
	QList<Ogre::MultiRenderTarget*> m_multiRenderTargetsList;
	
	//!
	//! Structure to hold all needed data of a texture output
	//!
	struct targetData{
		Ogre::TexturePtr texture;
		Ogre::RenderTexture* target;
		Ogre::MultiRenderTarget* mrt;
		int mrtBoundnumber;
		Parameter* parameter;
	};

	//!
	//! a map of output texture targets and their target data
	//!
	QMap<Ogre::RenderTexture*,targetData*> m_textureToTargetDataMap;

};

}; //namespace
#endif
