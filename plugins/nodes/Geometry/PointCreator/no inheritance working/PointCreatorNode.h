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
//! \file "PointCreatorNode.h"
//! \brief Header file for PointCreatorNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef PointCreatorNode_H
#define PointCreatorNode_H

#include "GeometryNode.h"
#include "OgreContainer.h"
#include "Ogre.h"
#include "OgreTools.h"


namespace PointCreatorNode {
	using namespace Frapper;

//!
//! Class representing nodes generate a texture map of position data on a rendertarget texture.
//!
	class PointCreatorNode : public ViewNode
{

    Q_OBJECT
		ADD_INSTANCE_COUNTER


public: // constructors and destructors

    //!
    //! Constructor of the PointCreatorNode class.
    //!
    //! \param name The name to give the new PointCreatorNode node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
	//! \param outputImageName the name of the output image texture
    //!
    PointCreatorNode ( const QString &name, ParameterGroup *parameterRoot, const QString &outputImageName = "Image");

    //!
    //! Destructor of the PointCreatorNode class.
    //!
    ~PointCreatorNode ();

	//!
    //! get the scene node for viewport rendering
    //!
	Ogre::SceneNode* getSceneNode();

public slots: //

    //!
    //! Redraw of ogre scene has been triggered.
    //!
    virtual void redrawTriggered ();


protected: // functions
	//!
	//! Sets the given texture as the output image in the output image parameter.
	//!
	//! \param texture The texture to use as output image
	//!
	void setOutputImage ( Ogre::TexturePtr texture ); ///from imageNode


private: // functions

    //!
    //! Creates the dummy manual object to serve as a renderable object. 
    //!
    bool createDummyManualObject();

    //!
    //! Removes the dummy manual object that serves as a renderable object. 
    //!
    void destroyDummyManualObject();

	//!
    //! create and setup the rtt texture
    //!
    void createRtt();



private slots: //

	//!
    //! Update the rtt
    //!
    void updateRtt();

	//!
    //! update the radius
    //!
	void setScaleParameter();

	//!
    //! update the count
    //!
	void setCountParameter();

private: // data

    //!
    //! OGRE scene node.
    //!
    Ogre::SceneNode *m_sceneNode;

	//!
    //! OGRE scene node.
    //!
    Ogre::SceneManager *m_sceneManager;

    //!
    //! OGRE entity.
    //!
	Ogre::ManualObject *m_dummyManualObject;

	//!
    //! The render texture name.
    //!
    Ogre::String m_renderTextureName;

	//!
    //! The rtt texture.
    //!
    Ogre::TexturePtr m_rttTexture;

	//!
    //! The flat materialPtr.
    //!
    Ogre::MaterialPtr m_flatMaterialPtr;

	//!
    //! The point materialPtr.
    //!
    Ogre::MaterialPtr m_pointMaterialPtr;

	//!
    //! The viewport of the rtt.
    //!
    Ogre::Viewport *m_viewport;

	//!
    //! OGRE camera for rtt
    //!
	Ogre::Camera *m_renderTargetCamera;


	//!
    //! The name of the image output parameter.
    //!
    QString m_outputImageName;

	//!
    //! The Pointer to the Parameter containing the main render target
    //!
    Parameter *m_outputParameter;

	//!
    //! The Pointer to the Parameter for changing point size
    //!
    NumberParameter *m_pointSizeParameter;

	//!
    //! The Pointer to the Parameter for changing point count
    //!
    NumberParameter *m_pointCountParameter;

	//!
    //! The current point count
    //!
    int m_currentPointCount;

	//!
    //! The old point count used for building the buffer.
	//! For every 64 points in the shader we need to create another "position" (primitive for the geometry shader)
	//! so each 64 point count cycle we need to create another point or destroy one. This way we only have to change the material on the points,
	//! and dont have to recreate them if the count is still in the %64 range.
    //!
    int m_lastCount;

};

} // end namespace

#endif
