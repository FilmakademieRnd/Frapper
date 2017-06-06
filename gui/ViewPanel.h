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
//! \file "ViewPanel.h"
//! \brief Header file for ViewPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       28.04.2009 (last updated)
//!

#ifndef VIEWPANEL_H
#define VIEWPANEL_H

#include "FrapperPrerequisites.h"
#include "Panel.h"


namespace Frapper {

//!
//! Base class for all view panel classes.
//!
class FRAPPER_GUI_EXPORT ViewPanel : public Panel
{

    Q_OBJECT

public: // nested enumerations

    //!
    //! Nested enumeration for the different types of view panels.
    //!
    enum Type {
        T_Documentation,
        T_Viewport,
        T_NetworkEditor,
        T_HierarchyEditor,
        T_ParameterEditor,
        T_CurveEditor,
		T_Timeline,
		T_PluginPanel,
        T_PainterPanel,
        T_NumTypes
    };

public: // constructors and destructors

    //!
    //! Constructor of the ViewPanel class.
    //!
    //! \param viewPanelType The type of view panel to create.
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    ViewPanel ( Type viewPanelType, QWidget *parent = 0, Qt::WindowFlags flags = 0 );

public: // functions

    //!
    //! Returns the type of the view panel.
    //!
    //! \return The type of the view panel.
    //!
    Type getViewPanelType ();

	virtual QString getPanelTypeName();

private: // data

    //!
    //! The type of the view panel.
    //!
    Type m_viewPanelType;

};

} // end namespace Frapper

#endif