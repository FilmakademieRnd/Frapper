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
//! \file "Panel.cpp"
//! \brief Implementation file for Panel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "Panel.h"
#include "Log.h"
#include <QApplication>
#include <QMouseEvent>


namespace Frapper {

INIT_INSTANCE_COUNTER(Panel)


///
/// Global Variables with File Scope
///


//!
//! List of strings of names of panel types.
//!
static const char *PanelTypeNames[Panel::T_NumTypes] = {
    "Viewport",
    "Network Editor",
    "Hierarchy Editor",
    "Parameter Editor",
    "Curve Editor",
    "Timeline",
	"History",
    "Log",
    "Documentation"
};


//!
//! List of strings of names of icons for panel types.
//!
static const char *PanelTypeIconNames[Panel::T_NumTypes] = {
    ":/viewportIcon",
    ":/networkEditorIcon",
    ":/hierarchyEditorIcon",
    ":/parameterEditorIcon",
    ":/curveEditorIcon",
    ":/timelineIcon",
	":/historyIcon",
    ":/logIcon",
    ":/helpIcon"
};


///
/// Public Static Functions
///


//!
//! Returns the name of the given panel type.
//!
//! \param panelType The panel type for which to return the name.
//! \return The name of the given panel type.
//!
QString Panel::getTypeName ( Panel::Type panelType )
{
    return QString(PanelTypeNames[panelType]);
}


//!
//! Returns the icon name for the given panel type.
//!
//! \param panelType The panel type for which to return the icon name.
//! \return The name of the icon for the given panel type, or an empty string if no icon for the panel type is available.
//!
QString Panel::getTypeIconName ( Type panelType )
{
    return QString(PanelTypeIconNames[panelType]);
}


///
/// Constructors and Destructors
///


//!
//! Constructor of the Panel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
Panel::Panel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    QFrame(parent, flags)
{
    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the Panel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
Panel::~Panel ()
{
    DEC_INSTANCE_COUNTER
}


///
/// Public Functions
///


//!
//! Fills the given tool bars in a panel frame with actions for the panel.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void Panel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
    // the default implementation is to do nothing here
}

//!
//! Gets the panel parameters. For load and save functionality.
//! 
QMap<QString, QString> Panel::getPanelParameters ()
{
    return m_parameterMap;
}

//!
//! Sets the panel parameters. For load and save functionality.
//! 
void Panel::setPanelParameters ( const QMap<QString, QString> &parameterMap )
{
}

//!
//! Connects the panel with the scene.
//!
void Panel::registerControl(NodeModel *nodeModel, SceneModel *sceneModel)
{
	// nothing
}

} // end namespace Frapper