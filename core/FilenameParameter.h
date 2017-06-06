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
//! \file "FilenameParameter.h"
//! \brief Header file for FilenameParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       03.05.2009 (last updated)
//!

#ifndef FILENAMEPARAMETER_H
#define FILENAMEPARAMETER_H

#include "FrapperPrerequisites.h"
#include "Parameter.h"

namespace Frapper {

//!
//! Class representing parameters for filenames.
//!
class FRAPPER_CORE_EXPORT FilenameParameter : public Parameter
{

public: // nested enumeration

    //!
    //! Nested enumeration of different filename types.
    //!
    enum Type {
        FT_Open,
        FT_Save,
		FT_Dir
    };

public: // constructors and destructors

    //!
    //! Constructor of the FilenameParameter class.
    //!
    //! \param name The name of the parameter.
    //! \param value The parameter's value.
    //!
    FilenameParameter ( const QString &name, const QVariant &value );

    //!
    //! Copy constructor of the FilenameParameter class.
    //!
    //! \param parameter The parameter to copy.
    //!
    FilenameParameter ( const FilenameParameter &parameter );

    //!
    //! Destructor of the FilenameParameter class.
    //!
    virtual ~FilenameParameter ();

public: // static functions

	//!
	//! Converts the given (absolute) (file) path to an relative (file) path
	//!
	//! \param path The (file) path to convert to a relative (file) path
	//! \return The relative (file) path
	//!
	static QString relativeFilePath ( const QString &filePath );

public: // functions

    //!
    //! Creates an exact copy of the parameter.
    //!
    //! \return An exact copy of the parameter.
    //!
    virtual AbstractParameter * clone ();

    //!
    //! Returns the filename filters to use when selecting a file in a file
    //! browser dialog.
    //!
    //! \return The filters to use when selecting a file in a file dialog.
    //!
    QString getFilters () const;

    //!
    //! Sets the filename filters to use when selecting a file in a file
    //! browser dialog.
    //!
    //! \param filters The filters to use when selecting a file in a file dialog.
    //!
    void setFilters ( const QString &filters );

    //!
    //! Returns the type for the filename.
    //!
    //! \return The type for the filename.
    //!
    Type getType () const;

    //!
    //! Sets the type for the filename.
    //!
    //! \param filenameType The type for the filename.
    //!
    void setType ( Type filenameType );

	//!
	//! Get the filename value of the the parameter with absolute path. 
	//! Relative file paths are converted to absolute file paths. 
	//!
	//! \sa FilenameParameter::FileNotFoundError()
	//!
	//! \return The filename value stored in this parameter with absolute path
	//!			or relative path, if file could not be located
	//!
	virtual QVariant getValue( const bool triggerEvaluation = false );

	//!
	//! Returns if there was an error locating the (relative) file
	//!
	//! \return Returns true, if there was an error locating the (relative) file
	//!			Returns false, otherwise.
	//!
	bool FileNotFoundError() const;


private: //functions

	//!
	//! Converts the given (relative) (file) path to an absolute (file) path
	//!
	//! \param path The (file) path to convert to an absolute (file) path
	//! \return The absolute (file) path or empty string if file could not be located
	//!
	QString absoluteFilePath ( const QString &filePath );

private: // data

    //!
    //! The filename filters to use when selecting a file in a file browser
    //! dialog.
    //!
    QString m_filters;

    //!
    //! The type of the filename.
    //!
    Type m_filenameType;

	//!
	//! This flag is set if the given file could not be located
	//!
	bool m_fileNotFoundError;
};

} // end namespace Frapper

#endif
