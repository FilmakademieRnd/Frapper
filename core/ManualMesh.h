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
//! \file "ManualMesh.h"
//! \brief Header file for ManualMesh class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    0.7
//! \date       01.12.2014 (last updated)
//!

#ifndef MANUALMESH_H
#define MANUALMESH_H

#include "FrapperPrerequisites.h"

#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace Frapper {

//!
//! This class is generates an Ogre Mesh manually. This way Ogre::Entity can be
//! used in order to create instances. This is not possible using Ogre::ManualObject.
//!
class FRAPPER_CORE_EXPORT ManualMesh
{

public:

    //!
    //! Constructor of the ManualMesh class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	ManualMesh(const std::string& name, const std::string& resourcegroup);

    //!
    //! Destructor of the ManualMesh class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ManualMesh ();

	void setRenderOperationType(Ogre::RenderOperation::OperationType renderOperationType);
	Ogre::MeshPtr& getMesh();
	void setMaterialName(const Ogre::String& materialName, const std::string& resourceGroup);

	void clear();
	void update(const int numberOfPoints, float *positionArray, float *colorArray, float *normalArray, float *uvArray);
	void prepareVertexBuffers(const int numberOfPoints, float *positionArray, float *colorArray, float *normalArray, float *uvArray);
	void updateVertexBuffer(Ogre::HardwareVertexBufferSharedPtr& hwBuffer, float *srcBuffer);
	void updateColorBuffer(Ogre::HardwareVertexBufferSharedPtr& hwBuffer, float *srcBuffer);

private:
	bool m_rebuildNeeded;

	unsigned int m_numberOfVertices;
	Ogre::MeshPtr m_mesh;
	Ogre::SubMesh* m_subMesh;
	
	Ogre::RenderOperation::OperationType m_renderOperationType;

	Ogre::HardwareVertexBufferSharedPtr m_positionBuffer;
	Ogre::HardwareVertexBufferSharedPtr m_normalBuffer;
	Ogre::HardwareVertexBufferSharedPtr m_colorBuffer;
	Ogre::HardwareVertexBufferSharedPtr m_uvBuffer;
};

} // end namespace Frapper

#endif