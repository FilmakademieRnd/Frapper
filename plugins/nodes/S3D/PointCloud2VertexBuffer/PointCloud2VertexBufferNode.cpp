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
//! \brief Implementation file for pointCloud2VertexBufferNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       04.10.2011 (last updated)
//!


#include "pointCloud2VertexBufferNode.h"
//#include <QtGui/QProgressDialog>
#include <typeinfo>
#include "GenericParameter.h"

#include "../CharonInterface/CharonInterface.hpp"

Q_DECLARE_METATYPE(Slot *);
Q_DECLARE_METATYPE(QVector<float>);

///
/// Constructors and Destructors
///
template <typename T>
PointCloud2VertexBuffer<T>::PointCloud2VertexBuffer(const std::string& name) :
	TemplatedParameteredObject<T>("PointCloud2VertexBuffer2", name, "doc")
{
	this->_addInputSlot(in, "pointCloud2D", "pointCloud2D", "CorrespondenceList");
}


//!
//! Constructor of the pointCloud2VertexBufferNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
pointCloud2VertexBufferNode::pointCloud2VertexBufferNode ( const QString &name, Frapper::ParameterGroup *parameterRoot ) :
	Node(name, parameterRoot)
{
	m_charonInterface = new CharonInterface(name);
	connect( m_charonInterface, SIGNAL(requestDeleteConnection(Frapper::Connection*)), this, SLOT(deleteConnection(Frapper::Connection*)));

	Frapper::EnumerationParameter *templateTypeParameter = new Frapper::EnumerationParameter("Template Type", 0);
	parameterRoot->addParameter(templateTypeParameter);
	templateTypeParameter->setLiterals(QStringList() << "double" << "float" << "int");
	templateTypeParameter->setValues(QStringList() << "0" << "1" << "2");
	templateTypeParameter->setChangeFunction(SLOT(changeTemplateType()));

	m_vertexBufferGroup = new Frapper::ParameterGroup("VertexBufferGroup");
	m_pointPositionParameter = new Frapper::NumberParameter("pos", Frapper::Parameter::T_Float, 0.0f);
	m_vertexBufferGroup->addParameter(m_pointPositionParameter);
	m_pointColorParameter = new Frapper::NumberParameter("col", Frapper::Parameter::T_Float, 0.0f);
	m_vertexBufferGroup->addParameter(m_pointColorParameter);
	m_pointColorParameter->setVisible(false);
	m_pointPositionParameter->setVisible(false);
	m_ouputVertexBuffer = Frapper::Parameter::createGroupParameter("VertexBuffer", m_vertexBufferGroup);
	m_ouputVertexBuffer->setPinType(Frapper::Parameter::PT_Output);
	parameterRoot->addParameter(m_ouputVertexBuffer);

	changeTemplateType();
	
	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the pointCloud2VertexBufferNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
pointCloud2VertexBufferNode::~pointCloud2VertexBufferNode ()
{
	DEC_INSTANCE_COUNTER;
}


// private slots


//!
//! Connects Frapper in/out parameter with Charon in/out slots.
//!
void pointCloud2VertexBufferNode::connectSlots ()
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	m_charonInterface->connectSlots(inParameter);
}


//!
//! Disonnects Frapper in/out parameter with Charon in/out slots.
//!
void pointCloud2VertexBufferNode::disconnectSlots ( int connectionID )
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	m_charonInterface->disconnectSlots(inParameter, connectionID);
}


//!
//! Initiates the Charon internal processing chain.
//!
void pointCloud2VertexBufferNode::processOutputData ()
{
	Slot* slot = 0;
	try {
		// trigger evaluation of input
		Frapper::Parameter* inputMap = getParameter("pointCloud2D");
		if( inputMap->isConnected() && !m_charonInterface->executed() ){
			m_charonInterface->processWorkflow();
		}
		slot = m_charonInterface->getSlot("pointCloud2D");

		const int selectedType = getEnumerationParameter("Template Type")->getCurrentIndex();
		switch (selectedType) {
		case 0: CopyData<double>(slot);
			break;
		case 1: CopyData<float>(slot);
			break;
		case 2: CopyData<int>(slot);
			break;
		default : CopyData<double>(slot);
		}

	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), "pointCloud2VertexBufferNode::processOutputData");
		return;
	}
}


//!
//! Copys the Ogre render target data into a Vigra<T> image.
//!
//! \param srcSlot The source buffer as a Charon slot.
//! \param destBuffer The destination buffer as a Ogre render target.
//!
template<typename T>
void pointCloud2VertexBufferNode::CopyData (Slot *srcSlot)
{
	QVector<float> vertices;
	QVector<float> colors;

	InputSlot<hekate::CorrespondenceList<T>> *input = dynamic_cast<InputSlot<hekate::CorrespondenceList<T>> *>(srcSlot);

	const hekate::CorrespondenceList<T> &corrs = (*input)();
	const size_t listSize = corrs.size();

	if (listSize < 1) {
		Frapper::Log::error(QString("Input Data not valid!"), "pointCloud2VertexBufferNode::CopyData");
		return;
	}

	auto itPtCloud = corrs.begin();
	auto itPtCloudEnd = corrs.end();

	vertices.reserve(listSize);
	colors.reserve(listSize);
	
	for(; itPtCloud != itPtCloudEnd; itPtCloud++) {
		vertices.append( (float)itPtCloud->x() );
		vertices.append( (float)itPtCloud->y() );
		vertices.append( (float)itPtCloud->z() );
		colors.append( (float)itPtCloud->R / 255.f);
		colors.append( (float)itPtCloud->G / 255.f);
		colors.append( (float)itPtCloud->B / 255.f);
	}
	
	m_pointPositionParameter->setSize(vertices.size());
	m_pointColorParameter->setSize(colors.size());

	m_pointPositionParameter->setValue(QVariant::fromValue(vertices));
	m_pointColorParameter->setValue(QVariant::fromValue(colors));
	m_ouputVertexBuffer->setDirty(true);
}


//!
//! Canges the Charon plugins template type.
//!
void pointCloud2VertexBufferNode::changeTemplateType ()
{
	m_charonInterface->setPO(NULL);

	Frapper::ParameterGroup *parameterRoot = getParameterRoot();
	Frapper::EnumerationParameter *enumParameter = getEnumerationParameter("Template Type");
	Frapper::GenericParameter *inputVBParameter = dynamic_cast<Frapper::GenericParameter *>(getParameter("pointCloud2D"));

	const std::string objectName = "PointCloud2VB";
	//const QString typeString = enumParameter->getCurrentLiteral();
	const int selectedType = getEnumerationParameter("Template Type")->getCurrentIndex();
	const QString& typeStr = getEnumerationParameter("Template Type")->getCurrentLiteral();

	if (inputVBParameter)
		parameterRoot->removeParameter(inputVBParameter, false);

	ParameteredObject* po = 0;
	switch (selectedType) {
		default :
		case 0:
			po = new PointCloud2VertexBuffer<double>(objectName);				
			break;
		case 1:
			po = new PointCloud2VertexBuffer<float>(objectName);
			break;
		case 2:
			po = new PointCloud2VertexBuffer<int>(objectName);
			break;
	}
	m_charonInterface->setPO(po);

	Slot *inSlot = m_charonInterface->getSlot("pointCloud2D");
	inputVBParameter = new Frapper::GenericParameter("pointCloud2D", 0);

	parameterRoot->addParameter(inputVBParameter);
	inputVBParameter->setPinType(Frapper::Parameter::PT_Input);
	inputVBParameter->setTypeInfo("CorrespondenceList<"+typeStr+">");
	inputVBParameter->setDirty(true);
	//inputVBParameter->setSelfEvaluating(true);
	inputVBParameter->addAffectedParameter(m_ouputVertexBuffer);
	inputVBParameter->setValue(QVariant::fromValue<Slot *>(inSlot), false);
	inputVBParameter->setOnConnectFunction(SLOT(connectSlots()));
	inputVBParameter->setOnDisconnectFunction(SLOT(disconnectSlots(int)));
	inputVBParameter->setProcessingFunction(SLOT(processOutputData()));

	emit nodeChanged();
	emit selectDeselectObject(Frapper::Node::getName());
}

void pointCloud2VertexBufferNode::deleteConnection( Frapper::Connection *connection )
{
	Frapper::Node::deleteConnection(connection);
}
