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
//! \file "HierarchyPanel.h"
//! \brief Implementation file for HierarchyPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "HierarchyPanel.h"
#include <QMenu>
#include <QStandardItemModel>
#include <QItemSelectionModel>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the HierarchyPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
HierarchyPanel::HierarchyPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_HierarchyEditor, parent, flags)
{
    setupUi(this);

    QMenu *filterMenu = new QMenu("Filter Messages", this);
    filterMenu->addAction(ui_geometryAction);
    filterMenu->addAction(ui_camerasAction);
    filterMenu->addAction(ui_lightsAction);
    filterMenu->addAction(ui_imagesAction);
    filterMenu->addAction(ui_materialsAction);
    filterMenu->addSeparator();
    filterMenu->addAction(ui_resetFilterAction);

    ui_filterAction->setMenu(filterMenu);
    ui_filterAction->setIcon(QIcon(":/resetFilterIcon"));
}


//!
//! Destructor of the HierarchyPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
HierarchyPanel::~HierarchyPanel ()
{

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
void HierarchyPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
    mainToolBar->addAction(ui_filterAction);
}


//!
//! Returns the tree view that is used to display the scene objects.
//!
//! \return The tree view that is used to display the scene objects.
//!
QTreeView * HierarchyPanel::getTreeView ()
{
    return sceneTreeView;
}

} // end namespace Frapper