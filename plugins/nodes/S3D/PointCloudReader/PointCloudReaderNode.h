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
//! \file "PointCloudReaderNode.cpp"
//! \brief Header file for PointCloudReaderNode class.
//!
//! \author     Volker Helzle <sspielma@filmakademie.de>
//! \version    1.0
//! \date       30.07.2014 (last updated)
//!

#ifndef POINTCLOUDREADERNODE_H
#define POINTCLOUDREADERNODE_H

#include "Node.h"

//!
//! Base class for all render nodes.
//!
class PointCloudReaderNode : public Frapper::Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PointCloudReaderNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	PointCloudReaderNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the PointCloudReaderNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PointCloudReaderNode ();

public slots:

	//!
	//! Loads the point cloud file from the selected file.
	//!
	//! Is called when the point cloud file has been changed.
	//!
	void pcFileChanged ();

	//!
	//! Trigger point cloud file reload.
	//!
	void triggerReload ();


private: // data	

	Frapper::Parameter *m_ouputVertexBuffer;
	
	Frapper::ParameterGroup *m_vertexBufferGroup;

	Frapper::NumberParameter *m_pointPositionParameter;

	Frapper::NumberParameter *m_pointColorParameter;

protected: // functions

	//!
	//! Loads the point cloud file.
	//!
	bool loadPointCloudFile ();
};



#endif
