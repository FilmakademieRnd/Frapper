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
//! \file "WindowManager.h"
//! \brief Header file for WindowManager class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    0.51
//! \date       12.02.2009 (last updated)
//!

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "FrapperPrerequisites.h"
#include "Window.h"
#include <QtCore/QList>
#include <QStandardItemModel>
#include <QMenu>


namespace Frapper {

//!
//! Static class for managing the application's windows.
//!
class FRAPPER_GUI_EXPORT WindowManager
{

public: // static functions

    //!
    //! Returns the item model that contains the list of the application's
    //! windows.
    //!
    //! \return The item model of the application's windows.
    //!
    static QStandardItemModel * getItemModel ();

    //!
    //! Registers the given menu with the window manager so that it updates it
    //! with a list of actions that can be used to switch between the windows
    //! registered with the window manager.
    //!
    //! \param windowMenu The menu to fill with window actions.
    //!
    static void registerWindowMenu ( QMenu *windowMenu );

    //!
    //! Registers the given window with the window manager.
    //!
    //! \param window The window to register with the window manager.
    //!
    static void registerWindow ( Window *window );

    //!
    //! Unregisters the given window with the window manager.
    //!
    //! \param window The window to unregister with the window manager.
    //!
    static void unregisterWindow ( Window *window );

    //!
    //! Returns the first window from the list of registered windows that
    //! contains a documentation panel, or 0 if no window in the application
    //! contains a documentation panel.
    //!
    //! \return The first window containing a documentation panel, or 0.
    //!
    static Window * getDocumentationWindow ();

    //!
    //! Highlights the action that is associated with the given window.
    //! Is called when a window of the application has been activated.
    //!
    //! \param window The window that was activated.
    //!
    static void highlightWindowAction ( Window *window );

    //!
    //! Updates the titles of the item and action that correspond to the given
    //! window.
    //! Is called when a window title has been changed.
    //!
    //! \param window The window whose title has been changed.
    //!
    static void updateWindowActionTitle ( Window *window );

    //!
    //! Returns whether additional windows have been registered with the window
    //! manager.
    //!
    //! \return True if additional windows have been registered with the window manager, otherwise False.
    //!
    static bool hasAdditionalWindows ();

    //!
    //! Closes all registered windows but the main window.
    //!
    static void closeAdditionalWindows ();

private: // static data

    //!
    //! The item model that contains the list of the application's windows.
    //!
    static QStandardItemModel s_windowItemModel;

    //!
    //! The root item in the item model that contains the list of windows.
    //!
    static QStandardItem *s_rootItem;

    //!
    //! The menu to update with actions that can be used for switching between
    //! the windows that are registered with the window manager.
    //!
    static QMenu *s_windowMenu;

};

} // end namespace Frapper

#endif
