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
//! \file "ViewPanel.cpp"
//! \brief Implementation file for ViewPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "ViewPanel.h"


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the ViewPanel class.
//!
//! \param viewPanelType The type of the view panel to create.
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
ViewPanel::ViewPanel ( ViewPanel::Type viewPanelType, QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    Panel(parent, flags)
{
    m_viewPanelType = viewPanelType;
}


///
/// Public Functions
///


//!
//! Returns the type of the view panel.
//!
//! \return The type of the view panel.
//!
ViewPanel::Type ViewPanel::getViewPanelType ()
{
    return m_viewPanelType;
}

QString ViewPanel::getPanelTypeName()
{
	return "Viewpanel";
}

} // end namespace Frapper