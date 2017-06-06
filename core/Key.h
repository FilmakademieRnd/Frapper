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
//! \file "Key.h"
//! \brief Header file for Key class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    2.0
//! \date       04.12.2012 (last updated)
//!

#include "FrapperPrerequisites.h"
#include <QtCore/QVariant>

#ifndef KEY_H
#define KEY_H

namespace Frapper {


//!
//! The data structure for a key.
//!
class FRAPPER_CORE_EXPORT Key
{
public:
	//!
    //! Nested enumeration for the different interpolation types
    //!
    enum KeyType {
        KT_Step = 0,
        KT_Linear,
        KT_Bezier
    };

	//!
	//! The key's type.
	//!
	KeyType type;

public:
	//!
	//! The key's index tangent index and tangent value.
	//!
	float index, tangentIndex, tangentValue;

	//!
	//! The paraent parameter of the key.
	//!
	AbstractParameter *parent;

	//!
	//! The key's value.
	//!
	QVariant keyValue;

public:
	inline Key()
    {
    }

	inline Key( const float index, const QVariant value, KeyType type = KT_Linear, AbstractParameter *parent = 0 )
		: index( index ), keyValue( value ), type( type ), tangentIndex(index), tangentValue(value.toFloat()), parent(parent) 
    {
    }

	inline Key( const float index, const QVariant value, const float tangentIndex, const float tangentValue, KeyType type = KT_Bezier, AbstractParameter *parent = 0 )
		: index( index ), keyValue( value ), type( type ), tangentIndex( tangentIndex ), tangentValue( tangentValue ), parent(parent) 
    {
    }

	inline Key operator * ( const float fScalar ) const
    {
        return Key(
            index,
			keyValue.toFloat() * fScalar,
            tangentIndex,
			tangentValue,
			type);
    }

	inline Key operator / ( const float fScalar ) const
    {
		if (fScalar != 0.f) {
			return Key(
				index,
				keyValue.toFloat() / fScalar,
				tangentIndex,
				tangentValue,
				type);
		}
		else
			return *this;
    }

	inline Key& operator *= ( const float fScalar )
    {
		keyValue.setValue<float>(keyValue.toFloat() * fScalar);
        return *this;
    }

	inline Key& operator /= ( const float fScalar )
    {
        if (fScalar != 0.f)
			keyValue.setValue<float>(keyValue.toFloat() / fScalar);
		return *this;
    }

	inline friend std::ostream& operator <<
        ( std::ostream& o, const Key& k )
    {
		o << "Key(" << k.index << ", " << k.keyValue.toString().toStdString() << ", " << k.tangentIndex << ", " << k.tangentValue << ")";
        return o;
    }
};

} // end namespace Frapper

#endif