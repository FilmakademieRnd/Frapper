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
//! \file "CharonWorkflowIteratorNode.cpp"
//! \brief Header file for CharonWorkflowIteratorNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       12.08.2013 (created)
//! \date       12.08.2013 (last updated)

//!

#ifndef CHARONWORKFLOWITERATORNODE_H
#define CHARONWORKFLOWITERATORNODE_H

#include "Node.h"
#include <charon-core/ParameteredObject.h>

namespace CharonWorkflowIteratorNode {

template <typename T>
class NodeParameteredObject : public TemplatedParameteredObject<T> {

public:
	NodeParameteredObject(const std::string& name = "");

private:
	OutputSlot < std::vector<std::string> > filename;

};

//!
//! Base class for all render nodes.
//!
class CharonWorkflowIteratorNode : public Frapper::Node
{

	Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the CharonWorkflowIteratorNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	CharonWorkflowIteratorNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the CharonWorkflowIteratorNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CharonWorkflowIteratorNode ();


private slots: 

	//!
    //! Start the iteration node
    //!
	void startIteration();
	void connectSlots();
	void disconnectSlots( int connectionID );

private: // data

	ParameteredObject* m_parameteredObject;

};

} // end namespace

#endif
