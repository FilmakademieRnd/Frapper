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
//! \file "SceneLoaderNode.h"
//! \brief Header file for SceneLoaderNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       15.01.2012 (last updated)
//!

#ifndef SCENELOADERNODE_H
#define SCENELOADERNODE_H

#include "GeometryNode.h"
#include "OgreContainer.h"
#include "Ogre.h"
#include "OgreTools.h"
#include "DotSceneLoader.h"
#include <qvector3d.h>

namespace SceneLoaderNode {
using namespace Frapper;

//!
//! Class representing nodes that can load a scene with entities from a .scene file
//!
class SceneLoaderNode : public GeometryNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the SceneLoaderNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    SceneLoaderNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the SceneLoaderNode class.
    //!
    ~SceneLoaderNode ();


private: // functions

    //!
    //! Loads the animation mesh from file.
    //!
    //! \return True if the animation mesh was successfully loaded, otherwise False.
    //!
    bool loadScene ();

    //!
    //! Retrieves the numbers of vertices and triangles from the mesh and stores
    //! them in parameters of this node.
    //!
    void updateStatistics ();

private slots: //

    //!
    //! Change function for the Geometry File parameter.
    //!
    void sceneFileChanged ();

private: // data


    //!
    //! DotScene loader
    //! 
    DotSceneLoader* m_sceneLoader;

    //!
    //! OGRE scene node.
    //!
    Ogre::SceneNode *m_sceneNode;

    //!
    //!
    //! 
    Ogre::SceneNode* m_lightsSceneNode;

    //!
    //! Old resource group name.
    //!
    QString m_oldResourceGroupName;

};

} // namespace SceneLoaderNode
#endif
