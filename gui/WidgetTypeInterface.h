/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

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
//! \file "PanelFactoryInterface.h"
//! \brief Header file for PanelFactoryInterface class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       10.12.2009 (last updated)
//!

#ifndef WIDGETTYPEINTERFACE_H
#define WIDGETTYPEINTERFACE_H

#include "FrapperPrerequisites.h"
#include "WidgetPlugin.h"
#include <QtCore/QString>
#include <QWidget>

namespace Frapper {

	//!
	//! Interface for widget type classes.
	//!
	class FRAPPER_GUI_EXPORT WidgetTypeInterface
	{

	public: // functions

		//!
		//! Creates a widget of this type.
		//!
		//! \param parent The parent Widget.
		//! \param parameter The associated parameter
		//! \return A pointer to the new widget.
		//!
		virtual WidgetPlugin * createWidget ( QWidget *parent , ParameterPlugin * parameter) = 0;

	};

} // end namespace Frapper

using namespace Frapper;
Q_DECLARE_INTERFACE(WidgetTypeInterface, "de.filmakademie.frapper.Plugins.Widgets.WidgetTypeInterface/1.1");

#endif