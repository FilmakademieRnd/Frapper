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
//! \file "WindowAction.cpp"
//! \brief Implementation file for WindowAction class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "WindowAction.h"


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the WindowAction class.
//!
//! \param window The window that the action represents.
//! \param index The index of the action in the window menu.
//!
WindowAction::WindowAction ( Window *window, int index )
    : QAction(QString("&%1 %2").arg(index).arg(window->title()), window)
{
    m_window = window;
    m_index = index;
    connect(this, SIGNAL(triggered()), this, SLOT(activateWindow()));
}


///
/// Public Functions
///


//!
//! Returns the window that the action represents.
//!
//! \return The window that the action represents.
//!
Window * WindowAction::getWindow ()
{
    return m_window;
}


//!
//! Sets the text of the action.
//!
//! \param text The new text for the action.
//!
void WindowAction::setText ( const QString &text )
{
    QAction::setText(QString("&%1 %2").arg(m_index).arg(text));
}


///
/// Public Slots
///


//!
//! Slot that is called when the action is triggered.
//! Activates the window that the action represents.
//!
void WindowAction::activateWindow ()
{
    if (!m_window)
        return;

    // check if the window is minimized
    if (m_window->isMinimized())
        // restore the window by removing the WindowMinimized state and setting the WindowActive state
        m_window->setWindowState(m_window->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    else
        // activate the window
        QApplication::setActiveWindow(m_window);
}

} // end namespace Frapper