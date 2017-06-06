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
//! \file "NetworkPanel.h"
//! \brief Implementation file for NetworkPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "NetworkPanel.h"


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the NetworkPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
NetworkPanel::NetworkPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_NetworkEditor, parent, flags),
		m_networkGraphicsView(new NetworkGraphicsView(this))
{
    setupUi(this);

    // add the network graphics view to the panel's layout
    ui_vboxLayout->addWidget(m_networkGraphicsView);

    // set up action connections
    connect(ui_homeAction, SIGNAL(triggered()), m_networkGraphicsView, SLOT(homeView()));
    connect(ui_frameAllAction, SIGNAL(triggered()), m_networkGraphicsView, SLOT(frameAll()));
    connect(ui_frameSelectedAction, SIGNAL(triggered()), m_networkGraphicsView, SLOT(frameSelected()));
    connect(ui_displayAllPinsAction, SIGNAL(triggered()), m_networkGraphicsView, SLOT(displayAllPins()));
    connect(ui_displayConnectedPinsAction, SIGNAL(triggered()), m_networkGraphicsView, SLOT(displayConnectedPins()));
    connect(ui_displayPinsCollapsedAction, SIGNAL(triggered()), m_networkGraphicsView, SLOT(displayPinsCollapsed()));
    connect(ui_scrollbarsAction, SIGNAL(toggled(bool)), m_networkGraphicsView, SLOT(toggleScrollbars(bool)));

    // initialize scroll bars
    if (ui_scrollbarsAction->isChecked())
        ui_scrollbarsAction->toggle();
    else
        m_networkGraphicsView->toggleScrollbars(false);
}


//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
NetworkPanel::~NetworkPanel ()
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
void NetworkPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
    mainToolBar->addAction(ui_homeAction);
    mainToolBar->addAction(ui_frameSelectedAction);
    mainToolBar->addAction(ui_frameAllAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(ui_displayAllPinsAction);
    mainToolBar->addAction(ui_displayConnectedPinsAction);
    mainToolBar->addAction(ui_displayPinsCollapsedAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(ui_scrollbarsAction);
}


//!
//! Returns the graphics view that is used to display the model's graphics
//! scene.
//!
//! \return The graphics view that is used to display the model's graphics scene.
//!
NetworkGraphicsView * NetworkPanel::getNetworkGraphicsView ()
{
    return m_networkGraphicsView;
}


///
/// Protected Events
///


//!
//! Handles key press events for the widget.
//!
//! \param event The description of the key event.
//!
void NetworkPanel::keyPressEvent ( QKeyEvent *event )
{
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    switch (event->key()) {
        case Qt::Key_A:
            ui_frameAllAction->trigger();
            break;
        case Qt::Key_F:
            ui_frameSelectedAction->trigger();
            break;
        case Qt::Key_H:
        case Qt::Key_Home:
            ui_homeAction->trigger();
            break;
        case Qt::Key_T:
            ui_scrollbarsAction->toggle();
            break;
        default:
            ViewPanel::keyPressEvent(event);
    }
}

} // end namespace Frapper