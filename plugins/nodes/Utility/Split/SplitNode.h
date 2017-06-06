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
//! \file "SplitNode.h"
//! \brief Header file for SplitNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       14.01.2013 (last updated)
//!

#ifndef SPLITNODE_H
#define SPLITNODE_H

#include "Node.h"


namespace SplitNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class SplitNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the SplitNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    SplitNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the SplitNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~SplitNode ();

private slots: //

	//!
    //! Creates the input parameters for the node.
    //!
	void setupParameters(Parameter *sourceParameter);

	//!
    //! Sets the parameter values.
    //!
	void setParameters();

	//!
    //! Sets the multiplier used for scaling the value.
    //!
	void cleanupParameters();

private:  // data
	ParameterGroup *m_outGroup;
	Parameter *m_inParameter;
};

} // namespace SplitNode 

#endif
