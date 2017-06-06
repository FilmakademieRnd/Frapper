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
//! \file "CurveEditorDataNode.cpp"
//! \brief Implementation file for CurveEditorDataNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       03.07.2009 (last updated)
//!

#include "CurveEditorDataNode.h"

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the CurveEditorDataNode class.
//!
//! \param name The name for the new node.
//!
CurveEditorDataNode::CurveEditorDataNode ( const QString &name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
m_parameterGroup(0)
{
}


//!
//! Destructor of the CurveEditorDataNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CurveEditorDataNode::~CurveEditorDataNode ()
{
    // the m_databaseGroup does not have to be destroyed here, as it is contained
    // in the node's parameter tree, which is destroyed in Node::~Node
}


///
/// Public Functions
///


ParameterGroup *CurveEditorDataNode::getCurveEditorGoup() const
{
	if (m_parameterGroup)
		return m_parameterGroup;
	else
		return getParameterRoot();
}

void CurveEditorDataNode::setCurveEditorGoup(ParameterGroup *parameterGroup)
{
    m_parameterGroup = parameterGroup;
}

} // end namespace Frapper