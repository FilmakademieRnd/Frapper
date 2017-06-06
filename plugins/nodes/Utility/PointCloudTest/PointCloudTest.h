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
//! \file "PointCloudTest.h"
//! \brief Header file for PointCloudTest class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    0.0
//! \date       12.10.2011 (last updated)
//!
//!
#ifndef POINCLOUDTEST_H
#define POINCLOUDTEST_H

#include "Node.h"
#include "InstanceCounterMacros.h"
#include "OgreManager.h"
#include "Math.h"

#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
	#include <windows.h>
#endif

namespace PointCloudTestNode {
using namespace Frapper;

//!
//! Class representing AR
//!
class PointCloudTest : public Node
{
    Q_OBJECT
	ADD_INSTANCE_COUNTER

public: /// constructors and destructors

    //!
    //! Constructor of the ARNode class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    PointCloudTest ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the PointCloudTest class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual inline ~PointCloudTest ();

private slots:
	void onAnimation();

public: //methods



private: //methods

	void setOutputPoints();


private: //data

	int m_maxNumberOfPoints;
	int m_minNumberOfPoints;
	bool m_allowShrinkAndGrow;

	Parameter *m_ouputVertexBuffer;
	
	ParameterGroup *m_vertexBufferGroup;

	NumberParameter *m_pointPositionParameter;

	NumberParameter *m_pointColorParameter;

	QVector<float> m_pointPositionVector;
	QVector<float> m_pointColorVector;

	int m_sizeofLastDrawnMap;
};

} // namespace PointCloudTestNode 

#endif
