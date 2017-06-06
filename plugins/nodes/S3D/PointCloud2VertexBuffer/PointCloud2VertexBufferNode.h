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
//! \file "pointCloud2VertexBufferNode.cpp"
//! \brief Header file for pointCloud2VertexBufferNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       04.10.2011 (last updated)
//!

#ifndef POINTCLOUD2VERTEXBUFFERNODE_H
#define POINTCLOUD2VERTEXBUFFERNODE_H

#include "Node.h"
#include <hekate/Correspondence.hxx>
#include <ParameteredObject.hxx>
#include <Slots.hxx>

#include "../CharonInterface/CharonInterface.h"

template <typename T>
class PointCloud2VertexBuffer : public TemplatedParameteredObject<T> {

public:
	PointCloud2VertexBuffer(const std::string& name = "");

private:
	InputSlot < typename hekate::CorrespondenceList<T> > in;
};


//!
//! Base class for all render nodes.
//!
class pointCloud2VertexBufferNode : public Frapper::Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the pointCloud2VertexBufferNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	pointCloud2VertexBufferNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the pointCloud2VertexBufferNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~pointCloud2VertexBufferNode ();


private slots:

	//!
    //! Connects Frapper in/out parameter with Charon in/out slots.
    //!
    void connectSlots ();

	//!
	//! Disonnects Frapper in/out parameter with Charon in/out slots.
	//!
	void disconnectSlots ( int connectionID );

	//!
    //! Initiates the Charon internal processing chain.
    //!
    void processOutputData ();

	//!
    //! Changes the Charon plugins template type.
    //!
	void changeTemplateType ();

	//!
	//! delete a connection
	//!
	void deleteConnection( Frapper::Connection *connection );

private: // functions

	//!
	//! Copys the Ogre render target data into a Vigra<T> image.
	//!
	//! \param srcSlot The source buffer as a Charon slot.
	//! \param destBuffer The destination buffer as a Ogre render target.
	//!
	template<typename T>
	void CopyData (Slot *srcSlot);

private: // data

	CharonInterface* m_charonInterface;

	Frapper::Parameter *m_ouputVertexBuffer;
	
	Frapper::ParameterGroup *m_vertexBufferGroup;

	Frapper::NumberParameter *m_pointPositionParameter;

	Frapper::NumberParameter *m_pointColorParameter;
};



#endif
