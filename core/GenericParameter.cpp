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
//! \file "GenericParameter.cpp"
//! \brief Implementation file for GenericParameter class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       16.04.2010 (last updated)
//!

#include "GenericParameter.h"
#include "Node.h"

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the FilenameParameter class.
//!
//! \param name The name of the parameter.
//! \param type The type of the parameter's value(s).
//! \param value The parameter's value.
//! \param animated The parameter's animation status.
//!
GenericParameter::GenericParameter ( const QString &name, const QVariant &value) :
Parameter(name, Parameter::T_Generic, value),
m_typeInfo("NO TYPE")
{
}


//!
//! Copy constructor of the GenericParameter class.
//!
//! \param parameter The parameter to copy.
//! \param node The parent node.
//!
GenericParameter::GenericParameter ( const GenericParameter &parameter, Node* node /*= 0*/ ) :
Parameter(parameter)
{
    m_node = node;
    if (node)
        m_node->getTimeParameter()->addAffectedParameter(this);
}


//!
//! Destructor of the FilenameParameter class.
//!
GenericParameter::~GenericParameter ()
{
}


///
/// Public Functions
///


//!
//! Creates an exact copy of the parameter.
//!
//! \return An exact copy of the parameter.
//!
AbstractParameter * GenericParameter::clone ()
{
    return new GenericParameter(*this);
}

//!
//! Returns the name of the given parameter type.
//!
//! \param type The type for which to return the name.
//! \return The name of the given parameter type.
//!
QString GenericParameter::getTypeName ( Parameter::Type type ) const
{
    return QString("Generic: " + m_typeInfo);
}

//!
//! Convenience function for setting the type info
//!
//! \param typeInfo The type info to set for the parameter.
//!
void GenericParameter::setTypeInfo (const QString &typeInfo) 
{
    m_typeInfo = typeInfo;
}

//!
//! Returns the parameter's type info string.
//!
//!
//! \return The parameter's type info string
//!
QString GenericParameter::getTypeInfo ( ) const
{
    return m_typeInfo;
}

} // end namespace Frapper