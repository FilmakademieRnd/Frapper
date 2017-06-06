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
//! \file "ManualMesh.cpp"
//! \brief Implementation file for ManualMesh class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       01.12.2014 (last updated)
//!

#include "ManualMesh.h"
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif
#include <iostream>

float defaultPositionBuffer[3] = { 0.0f, 0.0f, 0.0f };

namespace Frapper {

	///
	/// Constructors and Destructors
	///

	//!
	//! Constructor of the ManualMesh class.
	//!
	ManualMesh::ManualMesh(const std::string& name, const std::string& resourcegroup) :
		m_rebuildNeeded(true)
	{
		std::cout << "Checkpoint Constr. Manual Mesh 1" << std::endl;
		m_mesh = Ogre::MeshManager::getSingleton().createManual(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		m_mesh->sharedVertexData = new Ogre::VertexData();
		m_subMesh = m_mesh->createSubMesh();
		m_renderOperationType = Ogre::RenderOperation::OT_POINT_LIST;
		std::cout << "Checkpoint Constr. Manual Mesh 2" << std::endl;
	}

	//!
	//! Destructor of the ManualMesh class.
	//!
	//! Defined virtual to guarantee that the destructor of a derived class
	//! will be called if the instance of the derived class is saved in a
	//! variable of its parent class type.
	//!
	ManualMesh::~ManualMesh()
	{
		std::cout << "Checkpoint Destr. Manual Mesh 1" << std::endl;
		Ogre::MeshManager::getSingleton().remove(m_mesh->getName());
		m_mesh.setNull();
		std::cout << "Checkpoint Destr. Manual Mesh 2" << std::endl;
	}

	void ManualMesh::setRenderOperationType(Ogre::RenderOperation::OperationType renderOperationType)
	{
		m_renderOperationType = renderOperationType;
		if (renderOperationType != m_subMesh->operationType)
			m_rebuildNeeded = true;
	}

	Ogre::MeshPtr& ManualMesh::getMesh()
	{
		return m_mesh;
	}

	void ManualMesh::setMaterialName(const Ogre::String& materialName, const std::string& resourceGroup)
	{
		m_subMesh->setMaterialName(materialName, resourceGroup);
	}

	void ManualMesh::clear()
	{
		// use default buffer with one vertex
		// using 0 vertices causes crash
		update(1, defaultPositionBuffer, 0, 0, 0);
	}

	void ManualMesh::update(const int numberOfVertices, float* positionArray, float* colorArray, float *normalArray, float *uvArray)
	{
		if (numberOfVertices != m_numberOfVertices) {
			m_numberOfVertices = numberOfVertices;
			prepareVertexBuffers(numberOfVertices, positionArray, normalArray, colorArray, uvArray);
			m_mesh->load();
		}
		else {
			updateVertexBuffer(m_positionBuffer, positionArray);

			if (colorArray != 0)
				updateColorBuffer(m_colorBuffer, colorArray);

			if (normalArray != 0)
				updateVertexBuffer(m_normalBuffer, normalArray);

			if (uvArray != 0)
				updateVertexBuffer(m_uvBuffer, uvArray);
		}	
	}

	void ManualMesh::prepareVertexBuffers(const int numberOfVertices, float *positionArray, float *normalArray, float *colorArray, float *uvArray)
	{
		m_mesh->sharedVertexData->vertexCount = numberOfVertices;

		Ogre::VertexDeclaration* decl = m_mesh->sharedVertexData->vertexDeclaration;
		decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

		m_positionBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3), m_mesh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
		updateVertexBuffer(m_positionBuffer, positionArray);
		Ogre::VertexBufferBinding* bind = m_mesh->sharedVertexData->vertexBufferBinding;
		bind->setBinding(0, m_positionBuffer);

		if (colorArray != NULL)
		{
			decl->addElement(1, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
			m_colorBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR),
				m_mesh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			updateColorBuffer(m_colorBuffer, colorArray);
			bind->setBinding(1, m_colorBuffer);
		}

		if (normalArray != NULL)
		{
			decl->addElement(2, 0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
			m_normalBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
				m_mesh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
		}

		if (uvArray != NULL)
		{
			decl->addElement(3, 0, Ogre::VET_FLOAT3, Ogre::VES_TEXTURE_COORDINATES);
			m_uvBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
				m_mesh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
		}
			
		m_subMesh->useSharedVertices = true;
		m_subMesh->operationType = m_renderOperationType;

		// TODO: NILS: set bounding box on adding vertices?!
		m_mesh->_setBounds(Ogre::AxisAlignedBox(-100, -100, -100, 100, 100, 100));
		m_mesh->_setBoundingSphereRadius(Ogre::Math::Sqrt(3 * 100 * 100));
		
		m_rebuildNeeded = false;
	}

	void ManualMesh::updateVertexBuffer(Ogre::HardwareVertexBufferSharedPtr& hwBuffer, float *srcBuffer)
	{ 
		unsigned int bufferSize = m_numberOfVertices * 3;
		hwBuffer->writeData(0, hwBuffer->getSizeInBytes(), srcBuffer, true);
	}

	void ManualMesh::updateColorBuffer(Ogre::HardwareVertexBufferSharedPtr& hwBuffer, float *srcBuffer)
	{
		Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
		Ogre::RGBA *convertedColorsArray = new Ogre::RGBA[m_numberOfVertices];

		for (int i = 0, k = 0; i < m_numberOfVertices * 3, k < m_numberOfVertices; i += 3, ++k)
		{
			renderSystem->convertColourValue(Ogre::ColourValue(srcBuffer[i], srcBuffer[i + 1], srcBuffer[i + 2]), &convertedColorsArray[k]);
		}
		m_colorBuffer->writeData(0, m_colorBuffer->getSizeInBytes(), convertedColorsArray, true);
		delete[] convertedColorsArray;
	}

} // end namespace Frapper
