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
//! \file "WindowManager.cpp"
//! \brief Implementation file for WindowManager class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "WindowManager.h"
#include "WindowItem.h"
#include "WindowAction.h"


namespace Frapper {

///
/// Private Static Data
///


//!
//! The item model that contains the list of the application's windows.
//!
QStandardItemModel WindowManager::s_windowItemModel;

//!
//! The root item in the item model that contains the list of windows.
//!
QStandardItem *WindowManager::s_rootItem = s_windowItemModel.invisibleRootItem();

//!
//! The menu to update with actions that can be used for switching between
//! the windows that are registered with the window manager.
//!
QMenu *WindowManager::s_windowMenu;


///
/// Public Static Functions
///


//!
//! Returns the item model that contains the list of the application's
//! windows.
//!
//! \return The item model of the application's windows.
//!
QStandardItemModel * WindowManager::getItemModel ()
{
    return &s_windowItemModel;
}


//!
//! Registers the given menu with the window manager so that it updates it
//! with a list of actions that can be used to switch between the windows
//! registered with the window manager.
//!
//! \param windowMenu The menu to fill with window actions.
//!
void WindowManager::registerWindowMenu ( QMenu *windowMenu )
{
    s_windowMenu = windowMenu;
}


//!
//! Registers the given window with the window manager.
//!
void WindowManager::registerWindow ( Window *window )
{
    // add the given window to the list of managed windows
    WindowItem *windowItem = new WindowItem(window);
    s_rootItem->appendRow(windowItem);

    // check if a window menu has been registered
    if (s_windowMenu) {
        int index = s_rootItem->rowCount();
        // add a new action for the window to the window menu
        QList<QAction *> actions = s_windowMenu->actions();
        if (actions.size() > 0) {
            WindowAction *windowAction = new WindowAction(window, index);
            QAction *lastAction = actions.at(actions.size() - 1);
            s_windowMenu->insertAction(lastAction, windowAction);
        }
    }
}


//!
//! Unregisters the given window with the window manager.
//!
void WindowManager::unregisterWindow ( Window *window )
{
    // iterate over the list of registered windows
    for (int row = 0; row < s_rootItem->rowCount(); ++row) {
        WindowItem *windowItem = dynamic_cast<WindowItem *>(s_rootItem->child(row));
        if (windowItem && windowItem->getWindow() == window) {
            // remove the window item from the model
            s_rootItem->removeRow(row);
            return;
        }
    }
}



//!
//! Returns the first window from the list of registered windows that
//! contains a documentation panel, or 0 if no window in the application
//! contains a documentation panel.
//!
//! \return The first window containing a documentation panel, or 0.
//!
Window * WindowManager::getDocumentationWindow ()
{
    // iterate over the list of registered windows
    for (int row = 0; row < s_rootItem->rowCount(); ++row) {
        WindowItem *windowItem = dynamic_cast<WindowItem *>(s_rootItem->child(row));
        if (windowItem && windowItem->getWindow()->hasDocumentation())
            return windowItem->getWindow();
    }
    return 0;
}


//!
//! Highlights the action that is associated with the given window.
//! Is called when a window of the application has been activated.
//!
//! \param window The window that was activated.
//!
void WindowManager::highlightWindowAction ( Window *window )
{
    // check if a window menu has been registered
    if (!s_windowMenu)
        return;

    // iterate over the list of window actions
    foreach (QAction *action, s_windowMenu->actions()) {
        WindowAction *windowAction = qobject_cast<WindowAction *>(action);
        if (windowAction) {
            // set the bold state of the window action's font accordingly
            QFont font (windowAction->font());
            font.setBold(windowAction->getWindow() == window);
            windowAction->setFont(font);
        }
    }
}


//!
//! Updates the titles of the item and action that correspond to the given
//! window.
//! Is called when a window title has been changed.
//!
//! \param window The window whose title has been changed.
//!
void WindowManager::updateWindowActionTitle ( Window *window )
{
    // iterate over the list of registered windows
    for (int row = 0; row < s_rootItem->rowCount(); ++row) {
        WindowItem *windowItem = dynamic_cast<WindowItem *>(s_rootItem->child(row));
        // update the window item that represents the given window
        if (windowItem && windowItem->getWindow() == window)
            windowItem->setText(window->title());
    }

    // iterate over the list of window actions
    foreach (QAction *action, s_windowMenu->actions()) {
        WindowAction *windowAction = qobject_cast<WindowAction *>(action);
        // update the window action that represents the given window
        if (windowAction && windowAction->getWindow() == window)
            windowAction->setText(window->title());
    }
}


//!
//! Returns whether additional windows have been registered with the window
//! manager.
//!
//! \return True if additional windows have been registered with the window manager, otherwise False.
//!
bool WindowManager::hasAdditionalWindows ()
{
    return s_rootItem->rowCount() > 1;
}


//!
//! Closes all registered windows but the main window.
//!
void WindowManager::closeAdditionalWindows ()
{
    // iterate over the list of registered windows beginning with the second
    // and build a list of windows to close
    // (do not close them directly, as that would change the model which would
    // cause the loop to be left too early)
    QList<Window *> windowsToClose;
    for (int row = 1; row < s_rootItem->rowCount(); ++row) {
        WindowItem *windowItem = dynamic_cast<WindowItem *>(s_rootItem->child(row));
        if (windowItem)
            windowsToClose.append(windowItem->getWindow());
    }

    // iterate over the list of windows to close and close them
    // (this will cause unregisterWindow() to be called, changing the model)
    for (int i = 0; i < windowsToClose.size(); ++i)
        windowsToClose.at(i)->close();
}

} // end namespace Frapper