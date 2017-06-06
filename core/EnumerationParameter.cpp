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
//! \file "EnumerationParameter.cpp"
//! \brief Implementation file for EnumerationParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       20.05.2009 (last updated)
//!

#include "EnumerationParameter.h"

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the EnumerationParameter class.
//!
//! \param name The name of the parameter.
//! \param value The parameter's value.
//!
EnumerationParameter::EnumerationParameter ( const QString &name, const QVariant &value ) :
Parameter(name, Parameter::T_Enumeration, value)
{
}


//!
//! Copy constructor of the EnumerationParameter class.
//!
//! \param parameter The parameter to copy.
//!
EnumerationParameter::EnumerationParameter ( const EnumerationParameter &parameter ) :
Parameter(parameter)
{
    m_literals = parameter.m_literals;
    m_values = parameter.m_values;
}


//!
//! Destructor of the EnumerationParameter class.
//!
EnumerationParameter::~EnumerationParameter ()
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
AbstractParameter * EnumerationParameter::clone ()
{
    return new EnumerationParameter(*this);
}


//!
//! Returns the index of the item currently selected in the enumeration.
//!
//! Convenience function that calls getValue().toInt() on the parameter.
//!
//! \return The index of the item currently selected in the enumeration.
//!
int EnumerationParameter::getCurrentIndex ()
{
    return getValue().toInt();
}


//!
//! Returns the literal of the item currently selected in the enumeration.
//!
//! \return The literal of the item currently selected in the enumeration.
//!
QString EnumerationParameter::getCurrentLiteral ()
{
    int index = getCurrentIndex();
    if (index >= 0 && index < m_literals.size())
        return m_literals[index];
    else
        return QString();
}


//!
//! Returns the value of the item currently selected in the enumeration.
//!
//! \return The value of the item currently selected in the enumeration, or an invalid QString when no item is currently selected.
//!
QString EnumerationParameter::getCurrentValue ()
{
    int index = getCurrentIndex();
    if (index >= 0 && index < m_literals.size())
        return m_values[index];
    else
        return QString();
}


//!
//! Returns the list of string literals of the enumeration.
//!
//! \return The list of string literals of the enumeration, or an invalid QString when no item is currently selected.
//!
QStringList EnumerationParameter::getLiterals () const
{
    return m_literals;
}


//!
//! Sets the list of string literals for the enumeration.
//!
//! \param literals The list of string literals for the enumeration.
//!
void EnumerationParameter::setLiterals ( QStringList literals )
{
    m_literals = literals;
}


//!
//! Returns the list of values of the enumeration.
//!
//! \return The list of values of the enumeration.
//!
QStringList EnumerationParameter::getValues () const
{
    return m_values;
}


//!
//! Sets the list of values for the enumeration.
//!
//! \param values The list of values for the enumeration.
//!
void EnumerationParameter::setValues ( QStringList values )
{
    m_values = values;
}

} // end namespace Frapper