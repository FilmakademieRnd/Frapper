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

#ifndef TEXTUREGEOMETRYNODEABSTRACT_H
#define TEXTUREGEOMETRYNODEABSTRACT_H

#include "ViewNode.h"
#include "Ogre.h"
#include "OgreTools.h"

namespace Frapper {

//!
//! Class representing nodes generate a texture map of position data on a rendertarget texture.
//!
class FRAPPER_CORE_EXPORT TextureGeometryNodeAbstract : public ViewNode
{

    Q_OBJECT


public: // constructors and destructors

    //!
    //! Constructor of the TextureGeometry class.
    //!
    //! \param name The name to give the new TextureGeometry node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TextureGeometryNodeAbstract ( const QString &name, ParameterGroup *parameterRoot);

    //!
    //! Destructor of the TextureGeometry class.
    //!
    ~TextureGeometryNodeAbstract ();

	//!
    //! get the dummy manual object
    //!
	Ogre::ManualObject* getManualObject() { return m_dummyManualObject;}

	//!
    //! get the point count
    //!
	int getPointCount(bool dirty=0) { return m_pointCountParameter->getValue(dirty).toInt();}

	//!
    //! get the sceneNode
    //!
	Ogre::SceneNode* getSceneNode() { 

		return m_sceneNode;
	}

	//!
	//! set the points per primitive count
	//!
	void setPointsPerPrimitive(int ppp){m_pointsPerPrimitive=ppp;}

	//!
	//! Add an input texture target
	//!
	Parameter* addInputRenderTexture( const QString& parameterName);

	//!
	//! set inpupt texture to affect the texture unit state
	//!
	void addTusToParameter(Parameter* p, Ogre::TextureUnitState* t);
	
	//!
	//! create a copy of the material for this node instance
	//! \param name the name of the material to copy
	//!
	Ogre::MaterialPtr copyMaterial(Ogre::String name);

	//!
	//! set the material to be affected by count
	//!
	void setAffectedBycount(Ogre::MaterialPtr material);

	//!
	//! Creates an image from the given texture.
	//!
	//! \param texturePointer The texture from which to copy the image data.
	//! \return The image created from the given texture, or 0 if the given texture is invalid.
	//!
	void createImageFromTexture( const Ogre::TexturePtr texturePointer, Ogre::Image& outputImage);

	//!
	//! get the list of all the materials used in this node
	//! to later set common variables like resolution
	//!
	QList<Ogre::MaterialPtr>& getMaterials(){return m_materialsList;}

public slots: //

	//!
    //! set the pointcount value
    //!
	void setPointCount(int i);

private: // functions

	//!
	//! Initialize the empty texture
	//!
	void initEmptyTexture();

    //!
    //! Creates the dummy manual object to serve as a renderable object. 
    //!
    bool createDummyManualObject();

    //!
    //! Removes the dummy manual object that serves as a renderable object. 
    //!
    void destroyDummyManualObject();

private slots: //

	//!
    //! upadte inputs (preccessing function)
    //!
	void updateInput();

	//!
    //! Processing function of the input point count
	//! sets the count int the fragment and hull shader if they exist and have the parameter
    //!
	virtual void pointCountProcessingFunction();

	//!
    //! Processing function of the maximum point count parameter
	//! Here does nothing, but is implemented differently in the modify node and the shader node
    //!
	virtual void setMaxPointCountProcessingFunction();

	//!
	//! upadte the max output point count
	//!
	void setMaxOutPointCount();

	//!
	//! upadte the output point count
	//!
	void setOutPointCount();

private: // data

	//!
    //! OGRE scene manager.
    //!
    Ogre::SceneManager *m_sceneManager;

	//!
    //! OGRE scene node.
    //!
    Ogre::SceneNode *m_sceneNode;

    //!
    //! OGRE entity.
    //!
	Ogre::ManualObject *m_dummyManualObject;

	//!
	//! number of points per primitive for the geometry shader
	//! For every  points-per-primitive in the shader we need to create another "position" (primitive for the geometry shader)
	//! so each points-per-primitive count cycle we need to create another point or destroy one. This way we only have to change the material on the points,
	//! and dont have to recreate them if the count is still in the points-per-primitive range step.
	int m_pointsPerPrimitive;

	//!
	//! the old point count to compare with when updating primitive
	//!
	int m_oldPointCount;

	//!
	//! parameter for choosing the maximum number of points to generate
	//!
	EnumerationParameter *m_enumMaxPointParameter;

	//!
    //! point count parameter
    //!
	NumberParameter *m_pointCountParameter;
	
	//!
    //! list of materials used in this node
    //!
	QList<Ogre::MaterialPtr> m_materialsList;

	//!
    //! list of materials affected by the point count
    //!
	QList<Ogre::MaterialPtr> m_materialsAffectedByCount;

	//!
    //! max oint count out parameter
    //!
	NumberParameter *m_maxPointCountOutParameter;

	//!
	//! point count output parameter
	//! point count of this node
	//!
	NumberParameter *m_pointCountOutParameter;

	//!
    //! empty texture holder
    //!
	Ogre::TexturePtr m_emptyTexture;

	//!
	//! a map of input parameters and their affected tus's
	//!
	QMap<QString,QList<Ogre::TextureUnitState*>> m_inputTextureList;

};

}; //namespace
#endif
