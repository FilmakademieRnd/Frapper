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
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "WidgetFactory.h"
//! \brief Header file for WidgetFactory class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       10.12.2009 (last updated)
//!

#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H

#include "FrapperPrerequisites.h"
#include "WidgetTypeInterface.h"
#include "WidgetPlugin.h"
#include <QtCore/QString>


namespace Frapper {

//!
//! Static class managing node types and Node object creation.
//!
class FRAPPER_GUI_EXPORT WidgetFactory
{
public: // static functions

	
    //!
    //! Loads the XML description of a widget type from the file with the given
    //! name.
    //!
    //! \param filename The name of an XML file describing a widget type.
    //!
    static bool registerType ( const QString &filename );

	//!
	//! Returns a widget
	//!
	//! \param call Identification of the widget
	//! \param parameter Associated parameter
	//! \param parent Parent widget
	//! \return The created widget
	//!
	static WidgetPlugin * createWidget ( QString call ,  ParameterPlugin * parameter , QWidget *parent);

private: // static data

	//!
	//! List of Parameter names
	//!
	static QStringList m_WidgetNames;

	//!
	//! List of Widget Call names
	//!
	static QStringList m_WidgetCall;

	//!
	//! Map of existing widget types.
	//!
	static QMap<QString, WidgetTypeInterface*> m_widgetTypeMap;

};

} // end namespace Frapper

#endif