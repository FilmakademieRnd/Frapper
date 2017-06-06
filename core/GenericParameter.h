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
//! \file "GenericParameter.h"
//! \brief Header file for GenericParameter class.
//!
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       16.04.2010 (last updated)
//!

#ifndef GENERICPARAMETER_H
#define GENERICPARAMETER_H

#include "FrapperPrerequisites.h"
#include "Parameter.h"

namespace Frapper {

//!
//! Class representing parameters for numeric values.
//!
class FRAPPER_CORE_EXPORT GenericParameter : public Parameter
{

public: // constructors and destructors

    //!
    //! Constructor of the GenericParameter class.
    //!
    //! \param name The name of the parameter.
    //! \param type The type of the parameter's value(s).
    //! \param value The parameter's value.
    //! \param animated The parameter's animation status.
    //!
    GenericParameter ( const QString &name, const QVariant &value );

    //!
    //! Copy constructor of the GenericParameter class.
    //!
    //! \param parameter The parameter to copy.
    //! \param node The parent node.
    //!
    GenericParameter ( const GenericParameter &parameter, Node* node = 0  );

    //!
    //! Destructor of the FilenameParameter class.
    //!
    virtual ~GenericParameter ();

public: // functions

    //!
    //! Creates an exact copy of the parameter.
    //!
    //! \return An exact copy of the parameter.
    //!
    virtual AbstractParameter * clone ();

    //!
    //! Returns the name of the given parameter type.
    //!
    //! \param type The type for which to return the name.
    //! \return The name of the given parameter type.
    //!
    QString getTypeName ( Parameter::Type type ) const;

    //!
    //! Convenience function for setting the type info
    //!
    //! \param typeInfo The type info to set for the parameter.
    //!
    void setTypeInfo (const QString &typeInfo);

    //!
    //! Returns the parameter's type info string.
    //!
    //!
    //! \return The parameter's type info string
    //!
    QString getTypeInfo ( ) const;

private: // data

    //!
    //! The input method for numeric values of the parameter.
    //!
    QString m_typeInfo;
};

} // end namespace Frapper

#endif
