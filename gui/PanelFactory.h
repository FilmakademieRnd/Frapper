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
//! \file "PanelFactory.h"
//! \brief Header file for PanelFactory class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#ifndef PANELFACTORY_H
#define PANELFACTORY_H

#include "FrapperPrerequisites.h"
#include "Panel.h"
#include "PanelTypeInterface.h"
#include <QtCore/QString>


namespace Frapper {

//!
//! Static class managing node types and Node object creation.
//!
class FRAPPER_GUI_EXPORT PanelFactory
{

public: // static functions

    //!
    //! Loads the XML description of a panel type from the file with the given
    //! name.
    //!
    //! \param filename The name of an XML file describing a panel type.
    //!
    static bool registerType ( const QString &filename );

    //!
    //! Creates a panel of the given type name with the given name.
    //!
    //! \param typeName The name of the type to use for the new panel.
    //! \param name The name to give to the new panel.
    //! \return A pointer to the new node.
    //!
    static Panel * createPanel ( const QString &typeName , QWidget *parent);

    //!
    //! Frees all resources that were used by private static data of the panel
    //! factory class.
    //!
    static void freeResources ();

public : // static data

	//!
	//! Map of existing panel types.
	//!
	static QMap<QString, PanelTypeInterface*> m_panelTypeMap;

	//!
	//! List of existing panel plugin Files
	//!
	static QStringList m_panelPluginFiles;

	//!
	//! List of existing panel plugin names
	//!
	static QStringList m_panelPluginNames;

	//!
	//! List of existing panel plugin icons
	//!
	static QStringList m_panelPluginIcons;
};

} // end namespace Frapper

#endif
