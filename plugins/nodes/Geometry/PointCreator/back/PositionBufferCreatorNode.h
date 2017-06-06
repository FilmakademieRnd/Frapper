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
//! \file "PositionBufferCreatorNode.h"
//! \brief Header file for PositionBufferCreatorNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef POSITIONBUFFERCREATORNODE_H
#define POSITIONBUFFERCREATORNODE_H

#include "GeometryNode.h"
#include "OgreContainer.h"
#include "Ogre.h"
#include "OgreTools.h"


namespace PositionBufferCreatorNode {
	using namespace Frapper;

//!
//! Class representing nodes that can contain OGRE entities with animation.
//!
class PositionBufferCreatorNode : public GeometryNode
{

    Q_OBJECT
		ADD_INSTANCE_COUNTER


public: // constructors and destructors

    //!
    //! Constructor of the PositionBufferCreatorNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    PositionBufferCreatorNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the PositionBufferCreatorNode class.
    //!
    ~PositionBufferCreatorNode ();


private: // functions

    //!
    //! Loads the animation mesh from file.  
    //!
    bool createBufferObject();

    //!
    //! Removes the OGRE entity containing the mesh geometry from the scene and
    //! destroys it along with the OGRE scene node.
    //!
    void destroyEntity();

	//!
    //! create and setup the rtt texture
    //!
    void createRtt();



private slots: //

    //!
    //! Change function for the Buffer update.
    //!
    void updateBuffer();

	//!
    //! Cupdate the rtt
    //!
    void updateRtt();

	//!
	//! Sets the scenes shader parameter
	//!
	void setScaleParameter();

private: // data

    //!
    //! OGRE scene node.
    //!
    Ogre::SceneNode *m_sceneNode;

    //!
    //! OGRE entity.
    //!
	Ogre::ManualObject *m_positionEntity;

	//!
    //! OGRE rtt texture for holding data
    //!
	Ogre::RenderTexture *m_renderTargetTexture;

	//!
    //! OGRE camera for rtt
    //!
	Ogre::Camera *m_renderTargetCamera;

};

} // end namespace

#endif
