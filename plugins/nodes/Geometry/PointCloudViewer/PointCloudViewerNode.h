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
//! \file "PointCloudViewerNode.h"
//! \brief Header file for PointCloudViewerNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef ANIMATABLEMESHNODE_H
#define ANIMATABLEMESHNODE_H

#include "GeometryNode.h"
#include "ManualMesh.h"
#include "OgreContainer.h"
#include "Ogre.h"
#include "OgreTools.h"

#define TEST_MANUAL_MESH

namespace PointCloudViewerNode {
	using namespace Frapper;

//!
//! Class representing nodes that can contain OGRE entities with animation.
//!
class PointCloudViewerNode : public GeometryNode
{

    Q_OBJECT
        ADD_INSTANCE_COUNTER


public: // constructors and destructors

    //!
    //! Constructor of the PointCloudViewerNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    PointCloudViewerNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the PointCloudViewerNode class.
    //!
    ~PointCloudViewerNode ();


private: // functions

    //!
    //! Loads the animation mesh from file.  
    //!
    bool createObject ();
	
#ifdef TEST_MANUAL_MESH
	//!
	//! Updates the manual mesh.  
	//!
	void updateManualMesh(ParameterGroup* vertexBufferGroup);
#endif

    //!
    //! Retrieves the numbers of vertices and triangles from the mesh and stores
    //! them in parameters of this node.
    //!
    void updateStatistics ();

    //!
    //! Removes the OGRE entity containing the mesh geometry from the scene and
    //! destroys it along with the OGRE scene node.
    //!
    void destroyEntity ();


private slots: //

    //!
    //! Change function for the Geometry File parameter.
    //!
    void updatePointCloud ();

	//!
	//! Sets the scenes shader parameter
	//!
	void setShaderParameter();


private: // data

    //!
    //! OGRE scene node.
    //!
    Ogre::SceneNode *m_sceneNode;


#ifdef TEST_MANUAL_MESH
	//!
	//! OGRE entity.
	//!
	Ogre::Entity *m_entity;

	//!
	//! Manual object.
	//!
	ManualMesh *m_pointCloud;

#else
	//!
	//! OGRE manual object.
	//!
	Ogre::ManualObject *m_pointCloud;

	//!
	//! The container for the manual object.
	//!
	OgreContainer *m_manualObjContainer;
#endif


};

} // end namespace

#endif
