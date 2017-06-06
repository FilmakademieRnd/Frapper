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
//! \file "PanelFactoryInterface.h"
//! \brief Header file for PanelFactoryInterface class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    0.21
//! \date       12.02.2009 (last updated)
//!

#ifndef PANELTYPEINTERFACE_H
#define PANELTYPEINTERFACE_H

#include "FrapperPrerequisites.h"
#include "Panel.h"
#include <QtCore/QString>


namespace Frapper {

//!
//! Interface for node type classes.
//!
class FRAPPER_GUI_EXPORT PanelTypeInterface
{

public: // functions

    //!
    //! Creates a panel of this type.
    //!
    //! \param parent The parent Widget.
    //! \return A pointer to the new panel.
    //!
    virtual Panel * createPanel ( QWidget *parent ) = 0;

};

} // end namespace Frapper

using namespace Frapper;
Q_DECLARE_INTERFACE(PanelTypeInterface, "de.filmakademie.frapper.Plugins.Panels.PanelTypeInterface/1.1");


#endif