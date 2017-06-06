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
//! \file "EnumerationParameter.h"
//! \brief Header file for EnumerationParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de> 
//! \version    1.0
//! \date       30.04.2009 (last updated)
//!

#ifndef ENUMERATIONPARAMETER_H
#define ENUMERATIONPARAMETER_H

#include "FrapperPrerequisites.h"
#include "Parameter.h"

namespace Frapper {

//!
//! Class representing parameters for enumeration values.
//!
class FRAPPER_CORE_EXPORT EnumerationParameter : public Parameter
{

public: // constructors and destructors

    //!
    //! Constructor of the EnumerationParameter class.
    //!
    //! \param name The name of the parameter.
    //! \param value The parameter's value.
    //!
    EnumerationParameter ( const QString &name, const QVariant &value );

    //!
    //! Copy constructor of the EnumerationParameter class.
    //!
    //! \param parameter The parameter to copy.
    //!
    EnumerationParameter ( const EnumerationParameter &parameter );

    //!
    //! Destructor of the EnumerationParameter class.
    //!
    virtual ~EnumerationParameter ();

public: // functions

    //!
    //! Creates an exact copy of the parameter.
    //!
    //! \return An exact copy of the parameter.
    //!
    virtual AbstractParameter * clone ();

    //!
    //! Returns the index of the item currently selected in the enumeration.
    //!
    //! Convenience function that calls getValue().toInt() on the parameter.
    //!
    //! \return The index of the item currently selected in the enumeration.
    //!
    int getCurrentIndex ();

    //!
    //! Returns the literal of the item currently selected in the enumeration.
    //!
    //! \return The literal of the item currently selected in the enumeration, or an invalid QString when no item is currently selected.
    //!
    QString getCurrentLiteral ();

    //!
    //! Returns the value of the item currently selected in the enumeration.
    //!
    //! \return The value of the item currently selected in the enumeration, or an invalid QString when no item is currently selected.
    //!
    QString getCurrentValue ();

    //!
    //! Returns the list of string literals of the enumeration.
    //!
    //! \return The list of string literals of the enumeration.
    //!
    QStringList getLiterals () const;

    //!
    //! Sets the list of string literals for the enumeration.
    //!
    //! \param literals The list of string literals for the enumeration.
    //!
    void setLiterals ( QStringList literals );

    //!
    //! Returns the list of values of the enumeration.
    //!
    //! \return The list of values of the enumeration.
    //!
    QStringList getValues () const;

    //!
    //! Sets the list of values for the enumeration.
    //!
    //! \param values The list of values for the enumeration.
    //!
    void setValues ( QStringList values );

private: // data

    //!
    //! The list of string literals of the enumeration.
    //!
    QStringList m_literals;

    //!
    //! The list of values of the enumeration.
    //!
    QStringList m_values;

};

} // end namespace Frapper

#endif
