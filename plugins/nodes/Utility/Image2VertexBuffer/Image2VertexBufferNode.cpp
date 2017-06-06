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
//! \brief Implementation file for image2VertexBufferNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       04.10.2011 (last updated)
//!

#include "image2VertexBufferNode.h"
#include "GenericParameter.h"
#include <OgreBitwise.h>

namespace Image2VertexBufferNode {
	using namespace Frapper;

Q_DECLARE_METATYPE(QVector<float>);
//!
//! Constructor of the image2VertexBufferNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
image2VertexBufferNode::image2VertexBufferNode ( const QString &name, Frapper::ParameterGroup *parameterRoot ) :
	Node(name, parameterRoot)
{
	const QString vBufferName = "VertexBuffer";

	m_vertexBufferGroup = new Frapper::ParameterGroup("VertexBufferGroup");
	m_pointPositionParameter = new Frapper::NumberParameter("pos", Frapper::Parameter::T_Float, 0.0f);
	m_vertexBufferGroup->addParameter(m_pointPositionParameter);
	m_pointColorParameter = new Frapper::NumberParameter("col", Frapper::Parameter::T_Float, 0.0f);
	m_vertexBufferGroup->addParameter(m_pointColorParameter);
	m_pointColorParameter->setVisible(false);
	m_pointPositionParameter->setVisible(false);
	m_ouputVertexBuffer = Frapper::Parameter::createGroupParameter(vBufferName, m_vertexBufferGroup);
	m_ouputVertexBuffer->setPinType(Frapper::Parameter::PT_Output);
	parameterRoot->addParameter(m_ouputVertexBuffer);

	Parameter* depthParameter = getParameter("Depth Scale");
	m_depthScale = depthParameter->getValue().toFloat();
	depthParameter->setChangeFunction(SLOT(setDepthScale()));

	addAffection("Input Map Color", vBufferName);
    addAffection("Input Map Depth", vBufferName);
	m_ouputVertexBuffer->setProcessingFunction(SLOT(processOutputData()));
	m_ouputVertexBuffer->setSelfEvaluating(false);
    m_ouputVertexBuffer->setDirty(true);
	
	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the image2VertexBufferNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
image2VertexBufferNode::~image2VertexBufferNode ()
{
	DEC_INSTANCE_COUNTER;
}


// private slots


//!
//! Set the node's value for depth scaling.
//!
void image2VertexBufferNode::setDepthScale ()
{
	Parameter* parameter = dynamic_cast<Parameter *>(sender());

	if (parameter) {
		m_depthScale = parameter->getValue().toFloat();
		processOutputData();
		m_ouputVertexBuffer->propagateDirty();
	}
}

//!
//! Processes the node's input image to generate the node's output data.
//!
void image2VertexBufferNode::processOutputData ()
{
	QVector<float> pointPositionList, pointColorList;
	Ogre::HardwarePixelBufferSharedPtr bufferCol, bufferPos;

    Ogre::TexturePtr inputTextureDepth = getTextureValue("Input Map Depth");
	Ogre::TexturePtr inputTextureColor = getTextureValue("Input Map Color");

	if( (!inputTextureDepth.isNull() && !inputTextureColor.isNull()) &&
		(inputTextureDepth->getHeight() == inputTextureColor->getHeight()) &&
		(inputTextureDepth->getWidth() == inputTextureColor->getWidth()) ) {

		bufferPos = inputTextureDepth->getBuffer();
		copyImage2PosList(bufferPos, pointPositionList);

		bufferCol = inputTextureColor->getBuffer();
		copyImage2ColList(bufferCol, pointColorList);

	}
	else if (!inputTextureDepth.isNull() && inputTextureColor.isNull()) {
		bufferPos = inputTextureDepth->getBuffer();
		copyImage2PosList(bufferPos, pointPositionList);

		pointColorList.reserve(pointPositionList.size());
		for (int i=0; i<pointPositionList.size(); ++i)
			pointColorList << 0.0f;
	}

	m_pointPositionParameter->setSize(pointPositionList.size());
	m_pointColorParameter->setSize(pointColorList.size());

	m_pointPositionParameter->setValue(QVariant::fromValue(pointPositionList));
	m_pointColorParameter->setValue(QVariant::fromValue(pointColorList));

	m_ouputVertexBuffer->setDirty(true);
}

//!
//! Copys the Ogre render target depth buffer into a QList of floats.
//!
//! \param scrBuffer The source buffer from the Ogre render target.
//! \param destList The destination buffer as a QList of floats.
//!
void image2VertexBufferNode::copyImage2PosList(Ogre::HardwarePixelBufferSharedPtr srcBuffer, QVector<float> &destList)
{
	srcBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);

	// buffer info
	    BlitInfo blitInfo = createBlitInfo(srcBuffer);

	if (blitInfo.copyFunc == 0) {
		Frapper::Log::error(QString("No supported tesxture format!"), "image2VertexBufferNode::copyImage2ColList");
		srcBuffer->unlock();
		return;
	}

	void *vData = srcBuffer->getCurrentLock().data;
	destList.reserve(blitInfo.dx*blitInfo.dy*3);

	// copy loops
	int count = 0;
	for(int y=0, count=0; y<blitInfo.dy; ++y) {
		for(int x=0; x<blitInfo.dx; ++x, count+=blitInfo.dt) {
			destList.append(static_cast<float>(x));
			destList.append(static_cast<float>(y));
			destList.append(blitInfo.copyFunc(vData, count)*m_depthScale);
		}
	}

	srcBuffer->unlock();
}

//!
//! Copys the Ogre render target color buffer into a QList of floats.
//!
//! \param scrBuffer The source buffer from the Ogre render target.
//! \param destList The destination buffer as a QList of floats.
//!
void image2VertexBufferNode::copyImage2ColList(Ogre::HardwarePixelBufferSharedPtr srcBuffer, QVector<float> &destList)
{
	srcBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
    BlitInfo blitInfo = createBlitInfo(srcBuffer);

	if (blitInfo.copyFunc == 0) {
		Frapper::Log::error(QString("No supported tesxture format!"), "image2VertexBufferNode::copyImage2ColList");
		srcBuffer->unlock();
		return;
	}

	void *vData = srcBuffer->getCurrentLock().data;
	destList.reserve(blitInfo.dx*blitInfo.dy*3);

	// copy loops
	for(int y=0, count=0; y<blitInfo.dy; ++y) {
		for(int x=0; x<blitInfo.dx; ++x, count+=blitInfo.dt) 
		{
			destList.append(blitInfo.copyFunc(vData, (count+2)));
			destList.append(blitInfo.copyFunc(vData, (count+1)));
			destList.append(blitInfo.copyFunc(vData, (count)));
		}
	}

	srcBuffer->unlock();
}

//!
//! Figures out the infos needed to access the data in a Ogre reder target
//! \param scrBuffer The source buffer from the Ogre render target.
//!
image2VertexBufferNode::BlitInfo image2VertexBufferNode::createBlitInfo(Ogre::HardwarePixelBufferSharedPtr srcBuffer)
{
	BlitInfo returnvalue;

	// buffer info
	const Ogre::PixelBox &pixBox = srcBuffer->getCurrentLock();
	const Ogre::PixelFormat format = pixBox.format;
	returnvalue.dx = pixBox.getWidth();
	returnvalue.dy = pixBox.getHeight();
	returnvalue.dt = Ogre::PixelUtil::getComponentCount(format);

	if (format == Ogre::PF_X8B8G8R8 || format == Ogre::PF_X8R8G8B8)
		returnvalue.dt+=1;

	const int nbrBits = Ogre::PixelUtil::getNumElemBits(format) / returnvalue.dt;

	if (nbrBits == 8) 
		returnvalue.copyFunc = &image2VertexBufferNode::fixedToFloat;
	else if (nbrBits == 16) 
		returnvalue.copyFunc = &image2VertexBufferNode::halfToFloat;
	else if (nbrBits == 32) 
		returnvalue.copyFunc = &image2VertexBufferNode::floatToFloat;
	else 
		return BlitInfo();

	return returnvalue;
}


//!
//! Static function used by BlitData to convert the data stored in the source buffer.
//!
//! \param data The source buffer from the Ogre render target.
//! \param pos The index to the buffer.
//!
inline float image2VertexBufferNode::fixedToFloat(void *data, const unsigned int pos)
{
	Ogre::uint8 *iData = static_cast<Ogre::uint8 *>(data);
	return Ogre::Bitwise::fixedToFloat(iData[pos], 8);
}


//!
//! Static function used by BlitData to convert the data stored in the source buffer.
//!
//! \param data The source buffer from the Ogre render target.
//! \param pos The index to the buffer.
//!
inline float image2VertexBufferNode::halfToFloat(void *data, const unsigned int pos)
{
	Ogre::uint16 *iData = static_cast<Ogre::uint16 *>(data);
	return Ogre::Bitwise::halfToFloat(iData[pos]);
}


//!
//! Static function used by BlitData to convert the data stored in the source buffer.
//!
//! \param data The source buffer from the Ogre render target.
//! \param pos The index to the buffer.
//!
inline float image2VertexBufferNode::floatToFloat(void *data, const unsigned int pos)
{
	float *iData = static_cast<float *>(data);
	return iData[pos];
}

} // end namespace
