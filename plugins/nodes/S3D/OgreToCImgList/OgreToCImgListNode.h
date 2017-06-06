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
//! \file "OgreToCImgListNode.cpp"
//! \brief Header file for OgreToCImgListNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!

#ifndef OGRETOCIMGNODE_H
#define OGRETOCIMGNODE_H

#include "ImageNode.h"
#include "GenericParameter.h"
#include <charon-core/ParameteredObject.h>
#include <charon-utils/CImg.h>

namespace OgreToCImgListNode {
	using namespace Frapper;

template <typename T>
class OgreToCImgListPO : public TemplatedParameteredObject<T> {

public:
	OgreToCImgListPO(const std::string& name = "");

private:
	OutputSlot < cimg_library::CImgList<T> > image_out;
};


//!
//! Base class for all render nodes.
//!
class OgreToCImgListNode : public Frapper::Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the OgreToCImgListNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	OgreToCImgListNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the OgreToCImgListNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~OgreToCImgListNode ();


private slots: 

	//!
	//! Connects Frapper in/out parameter with Charon in/out slots.
	//!
	void connectSlots ();

	//!
	//! Disonnects Frapper in/out parameter with Charon in/out slots.
	//!
	void disconnectSlots (int connectionID);
	//!
    //! Initiates the Charon internal processing chain.
    //!
    void processOutputSlot ();

	//!
    //! Canges the Charon plugins template type.
    //!
	void changeTemplateType ();

private: // functions
	
	template<typename T> 
	void prepareOutputSlot();

private: // data

	ParameteredObject *m_parameteredObject;

	int m_selectedType;
};

} // end namespace

#endif
