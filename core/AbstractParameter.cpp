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
//! \file "AbstractParameter.cpp"
//! \brief Implementation file for AbstractParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       24.03.2010 (last updated)
//!

#include "AbstractParameter.h"
#include "Node.h"

namespace Frapper {

///
/// Constructors and Destructors
///

//!
//! Constructor of the AbstractParameter class.
//!
//! \param name The name of the parameter.
//!
AbstractParameter::AbstractParameter ( const QString &name ) :
    m_name(name),
    m_enabled(true),
    m_node(0)
{
}


//!
//! Copy constructor of the AbstractParameter class.
//!
//! \param parameter The parameter to copy.
//!
AbstractParameter::AbstractParameter ( const AbstractParameter &parameter, Node* node /*= 0*/ ) :
    m_name(parameter.m_name),
    m_enabled(parameter.m_enabled),
    m_node(node)
{
}


//!
//! Destructor of the AbstractParameter class.
//!
AbstractParameter::~AbstractParameter ()
{
}


///
/// Public Functions
///


//!
//! Returns the name of the parameter or parameter group.
//!
//! \return The name of the parameter or parameter group.
//!
const QString &AbstractParameter::getName () const
{
    return m_name;
}


//!
//! Returns whether the controls for editing the parameter or parameter
//! group in the UI should be enabled.
//!
//! \return True if the controls should be enabled, otherwise False.
//!
bool AbstractParameter::isEnabled () const
{
    return m_enabled;
}


//!
//! Sets whether the controls for editing the parameter or parameter group
//! in the UI should be enabled.
//!
//! \param enabled The value for the parameter's enabled flag.
//!
void AbstractParameter::setEnabled ( bool enabled, bool propagate /*= true*/ )
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        if (propagate)
            emit enabledChanged();
    }
}


//!
//! Sets name of the parameter.
//!
//! \param name The parameter name.
//!
void AbstractParameter::setName ( const QString &name )
{
    m_name = name;
}


//!
//! Returns the node to which this parameter or parameter group belongs.
//!
//! \return The node to which the parameter or parameter group belongs.
//!
Node * AbstractParameter::getNode () const
{
    return m_node;
}


//!
//! Sets the node to which this parameter or parameter group belongs.
//!
//! \param node The node to which the parameter or parameter group belongs.
//!
void AbstractParameter::setNode ( Node *node )
{
    m_node = node;
}

} // end namespace Frapper