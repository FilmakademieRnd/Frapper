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
//! \file "image2VertexBufferNode.cpp"
//! \brief Header file for image2VertexBufferNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       04.10.2011 (last updated)
//!

#ifndef IMAGE2VERTEXBUFFERNODE_H
#define IMAGE2VERTEXBUFFERNODE_H

#include "Node.h"

namespace Image2VertexBufferNode {
	using namespace Frapper;

//!
//! Base class for all render nodes.
//!
class image2VertexBufferNode : public Frapper::Node
{

    Q_OBJECT


public: // constructors and destructors

    //!
    //! Constructor of the image2VertexBufferNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	image2VertexBufferNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the image2VertexBufferNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~image2VertexBufferNode ();

private: // structures

	struct BlitInfo {
		BlitInfo () : dx(0), dy(0), dt(0), copyFunc(0)  {};
		int dx, dy, dt;
		float (*copyFunc)(void *, unsigned int);
	};

private slots:

	//!
    //! Set the node's value for depth scaling.
    //!
	void setDepthScale();

	//!
    //! Processes the node's input image to generate the node's output data.
    //!
    void processOutputData ();


private: // functions

	//!
	//! Copys the Ogre render target depth buffer into a QList of floats.
	//!
	//! \param scrBuffer The source buffer from the Ogre render target.
	//! \param destList The destination buffer as a QList of floats.
	//!
	void copyImage2PosList(Ogre::HardwarePixelBufferSharedPtr srcBuffer, QVector<float> &destList);

	//!
	//! Copys the Ogre render target color buffer into a QList of floats.
	//!
	//! \param scrBuffer The source buffer from the Ogre render target.
	//! \param destList The destination buffer as a QList of floats.
	//!
	void copyImage2ColList(Ogre::HardwarePixelBufferSharedPtr srcBuffer, QVector<float> &destList);

	//!
    //! Figures out the infos needed to access the data in a Ogre reder target
    //! \param scrBuffer The source buffer from the Ogre render target.
    //!
	BlitInfo createBlitInfo (Ogre::HardwarePixelBufferSharedPtr srcBuffer);

	//!
	//! Static function used by BlitData to convert the data stored in the source buffer.
	//!
	//! \param data The source buffer from the Ogre render target.
	//! \param pos The index to the buffer.
	//!
	inline static float fixedToFloat(void *data, const unsigned int pos);

	//!
	//! Static function used by BlitData to convert the data stored in the source buffer.
	//!
	//! \param data The source buffer from the Ogre render target.
	//! \param pos The index to the buffer.
	//!
	inline static float halfToFloat(void *data, const unsigned int pos);

	//!
	//! Static function used by BlitData to convert the data stored in the source buffer.
	//!
	//! \param data The source buffer from the Ogre render target.
	//! \param pos The index to the buffer.
	//!
	inline static float floatToFloat(void *data, const unsigned int pos);

private: // data

	//! The parameter containing the vertex buffer data
	Frapper::Parameter *m_ouputVertexBuffer;
	
	//! The group containing the vertex buffer parameters
	Frapper::ParameterGroup *m_vertexBufferGroup;

	//! The Parameter containing the vertex buffers position data
	Frapper::NumberParameter *m_pointPositionParameter;

	//! The Parameter containing the vertex buffers color data
	Frapper::NumberParameter *m_pointColorParameter;

	//! The scale for the depth values
	float m_depthScale;
};

} // end namespace

#endif
