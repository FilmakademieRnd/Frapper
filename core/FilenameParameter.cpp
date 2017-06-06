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
//! \file "FilenameParameter.cpp"
//! \brief Implementation file for FilenameParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       03.05.2009 (last updated)
//!

#include "FilenameParameter.h"

#include <QDir>
#include <QFileInfo>
#include <SceneModel.h>
#include <QMessageBox>
#include <QFileDialog>

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the FilenameParameter class.
//!
//! \param name The name of the parameter.
//! \param value The parameter's value.
//!
FilenameParameter::FilenameParameter ( const QString &name, const QVariant &value ) :
Parameter(name, Parameter::T_Filename, value),
m_filenameType(FT_Open),
m_fileNotFoundError(false)
{
}


//!
//! Copy constructor of the FilenameParameter class.
//!
//! \param parameter The parameter to copy.
//!
FilenameParameter::FilenameParameter ( const FilenameParameter &parameter ) :
Parameter(parameter),
m_filters(parameter.m_filters),
m_filenameType(parameter.m_filenameType)
{
}


//!
//! Destructor of the FilenameParameter class.
//!
FilenameParameter::~FilenameParameter ()
{
}

///
/// Public Static Functions
///

QString FilenameParameter::relativeFilePath( const QString &filePath )
{
	// check if given workflow file path is already absolute
	QFileInfo fi( filePath );
	if( !fi.isRelative() ) {

		// check if relative to working dir
		QString workingDir = SceneModel::getWorkingDirectory();
		if( workingDir != "" ) {

			QString wdStr = QFileInfo(workingDir).absoluteFilePath()+'/';

			if( filePath.contains( wdStr, Qt::CaseSensitive))
				return QString(filePath).remove( wdStr, Qt::CaseSensitive);
		}

		// check if relative to scene file dir
		QString sceneFile = SceneModel::getSceneFileName();
		if( sceneFile != "") {

			QString scStr = QFileInfo( sceneFile ).absolutePath()+'/';

			if( filePath.contains( scStr, Qt::CaseSensitive))
				return QString(filePath).remove( scStr, Qt::CaseSensitive);
		}
	}
	return filePath;
}

///
/// Public Functions
///


//!
//! Creates an exact copy of the parameter.
//!
//! \return An exact copy of the parameter.
//!
AbstractParameter * FilenameParameter::clone ()
{
    return new FilenameParameter(*this);
}


//!
//! Returns the filename filters to use when selecting a file in a file
//! browser dialog.
//!
//! \return The filters to use when selecting a file in a file dialog.
//!
QString FilenameParameter::getFilters () const
{
    return m_filters;
}


//!
//! Sets the filename filters to use when selecting a file in a file
//! browser dialog.
//!
//! \param filters The filters to use when selecting a file in a file dialog.
//!
void FilenameParameter::setFilters ( const QString &filters )
{
    m_filters = filters;
}


//!
//! Returns the type for the filename.
//!
//! \return The type for the filename.
//!
FilenameParameter::Type FilenameParameter::getType () const
{
    return m_filenameType;
}


//!
//! Sets the type for the filename.
//!
//! \param filenameType The type for the filename.
//!
void FilenameParameter::setType ( FilenameParameter::Type filenameType )
{
    m_filenameType = filenameType;
}

QVariant FilenameParameter::getValue( const bool triggerEvaluation /*= false */ )
{
	QVariant value = Parameter::getValue( triggerEvaluation );
	
	if( value.canConvert<QString>()) {
		bool fileNotFoundError = m_fileNotFoundError;
		QString result = absoluteFilePath( m_value.toString() );
		if( fileNotFoundError != m_fileNotFoundError )
			emit valueChanged(); // emit signal to update filename widget
		return QVariant( result );
	} else {
		return value;
	}
}

QString FilenameParameter::absoluteFilePath( const QString &filePath )
{
	m_fileNotFoundError = false;

	if( filePath.isEmpty())
		return QString();

	QFileInfo fileInfo( filePath );

	// check if filepath already absolute, e.g. not relative to working directory or scene
	if( !fileInfo.isRelative() ) {
		
		// check if file exists, set error flag if not
		m_fileNotFoundError = !fileInfo.exists();
		return filePath;
	}

	// check if file is relative to working dir
	QString workingDir = SceneModel::getWorkingDirectory();
	QFileInfo inWorkingDir( QDir(workingDir), filePath);

	// check if file exists in wd or has not been saved there yet
	// save files always relative to working directory
	if( inWorkingDir.exists() || m_filenameType == FT_Save)
		return inWorkingDir.absoluteFilePath();

	// check if file is relative to scene file
	QString sceneFile = SceneModel::getSceneFileName();

	if( sceneFile != "") {
		QDir sceneFileDir = QFileInfo( sceneFile).absoluteDir();
		QFileInfo inSceneFileDir( sceneFileDir, filePath );
		if( inSceneFileDir.exists() )
			return inSceneFileDir.absoluteFilePath();
	}

	// file could not be located -> set file not found flag
	m_fileNotFoundError = true;

	return filePath;
}

bool FilenameParameter::FileNotFoundError() const
{
	return m_fileNotFoundError;
}


} // end namespace Frapper