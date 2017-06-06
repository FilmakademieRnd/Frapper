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
//! \file "PointCloudTest.cpp"
//! \brief AR
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    0.1
//! \date       12.1.2011 (last updated)
//!

#include "PointCloudTest.h"
#include "Log.h"

namespace PointCloudTestNode {
using namespace Frapper;
using namespace std;

Q_DECLARE_METATYPE(QVector<float>)
INIT_INSTANCE_COUNTER(PointCloudTest)


///
/// Public Constructors
///

//!
//! Constructor of the PointCloudTest class.
//!
//! \param name The name to give the new mesh node.
//!
PointCloudTest::PointCloudTest ( QString name, ParameterGroup *parameterRoot ) :
	Node(name, parameterRoot),
	m_maxNumberOfPoints(1000),
	m_minNumberOfPoints(500),
	m_allowShrinkAndGrow(false)

{
	INC_INSTANCE_COUNTER

	// get output parameter group
	m_vertexBufferGroup = new ParameterGroup("VertexBufferGroup");
	m_pointPositionParameter = new NumberParameter("pos", Parameter::T_Float, 0.0f);
	m_vertexBufferGroup->addParameter(m_pointPositionParameter);
	m_pointColorParameter = new NumberParameter("col", Parameter::T_Float, 0.0f);
	m_vertexBufferGroup->addParameter(m_pointColorParameter);
	m_pointColorParameter->setVisible(false);
	m_pointPositionParameter->setVisible(false);
	m_ouputVertexBuffer = Parameter::createGroupParameter("VertexBuffer", m_vertexBufferGroup);
	m_ouputVertexBuffer->setPinType(Parameter::PT_Output);
	parameterRoot->addParameter(m_ouputVertexBuffer);

	Parameter* animationParameter = getParameter("Animate");
   	animationParameter->setChangeFunction(SLOT(onAnimation()));

	setOutputPoints();
}

//!
//! Destructor of the ARNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PointCloudTest::~PointCloudTest ()
{
   DEC_INSTANCE_COUNTER
}

///
/// Public Slots
///

///
/// Private Slots
///
void PointCloudTest::onAnimation()
{
	Parameter* maxNumberOfPointsParameter = getParameter("MaxNumberOfPoints");	
	if (maxNumberOfPointsParameter) {
		m_maxNumberOfPoints = maxNumberOfPointsParameter->getValue().toInt();
	}

	Parameter* minNumberOfPointsParameter = getParameter("MinNumberOfPoints");
	if (minNumberOfPointsParameter) {
		m_minNumberOfPoints = minNumberOfPointsParameter->getValue().toInt();
	}

	Parameter* allowShrinkAndGrowParameter = getParameter("AllowShrinkAndGrow");
	if (allowShrinkAndGrowParameter) {
		m_allowShrinkAndGrow = allowShrinkAndGrowParameter->getValue().toBool();
	}

	setOutputPoints();
}


///
/// Public Methods
///

///
/// Private Methods
///



void PointCloudTest::setOutputPoints()
{
	m_pointPositionVector.clear();
	m_pointColorVector.clear();
	for(size_t i=0; i < m_maxNumberOfPoints; i++)
	{
		float x = ((float)rand()) / RAND_MAX;
		float y = ((float)rand()) / RAND_MAX;
		float z = ((float)rand()) / RAND_MAX;
		m_pointPositionVector.append(x);
		m_pointPositionVector.append(y);
		m_pointPositionVector.append(z);
		m_pointColorVector.append(((float)rand())/(float)RAND_MAX);	//R
		m_pointColorVector.append(((float)rand())/(float)RAND_MAX);	//G
		m_pointColorVector.append(((float)rand())/(float)RAND_MAX);	//B
	}

	m_pointPositionParameter->setSize(m_pointPositionVector.size());
	m_pointColorParameter->setSize(m_pointColorVector.size());

	m_pointPositionParameter->setValue(QVariant::fromValue(m_pointPositionVector), true);
	m_pointColorParameter->setValue(QVariant::fromValue(m_pointColorVector), true);

	m_ouputVertexBuffer->propagateDirty();
}

} // namespace PointCloudTestNode 
