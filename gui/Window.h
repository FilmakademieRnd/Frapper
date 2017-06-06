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
//! \file "Window.h"
//! \brief Header file for Window class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       08.06.2009 (last updated)
//!

#ifndef WINDOW_H
#define WINDOW_H

#include "FrapperPrerequisites.h"
#include "PanelFrame.h"
#include "ui_Window.h"
#include <QtCore/QSettings>


namespace Frapper {

//!
//! Class for the application's windows, both the main window and additional
//! windows.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph Window {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     Window [label="Window",fillcolor="grey75"];
//!     QMainWindow -> Window;
//!     QMainWindow [label="QMainWindow",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qmainwindow.html"];
//!     UiWindow -> Window [color="darkgreen"];
//!     UiWindow [label="Ui::Window",URL="class_ui_1_1_window.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_GUI_EXPORT Window : public QMainWindow, protected Ui::Window
{

    Q_OBJECT

public: // static constants

    //!
    //! The name of the default window layout.
    //!
    static const QString DefaultLayout;

    //!
    //! The name of the window layout with only a single view.
    //!
    static const QString SingleViewLayout;

    //!
    //! The name of the window layout with a fullscreen view.
    //!
    static const QString FullscreenViewLayout;

public: // constructors and destructors

    //!
    //! Constructor of the Window class.
    //!
    //! Creates a new application window using the given layout.
    //!
    //! \param parent The parent object (should be the Application object).
    //! \param applicationName The name of the application that the window will be a part of.
    //! \param title The title to give to the new window.
    //! \param layoutName The name of the layout to apply to the window.
    //! \param isMainWindow Flag that states whether the window to create is the application's main window.
    //!
    Window ( QObject *parent, const QString &applicationName, const QString &title, const QString &layoutName, bool isMainWindow = false );

    //!
    //! Destructor of the Window class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~Window ();

public: // functions

	//!
	//! Returns the window layout state
	//!
    //! \param settings The window layout settings state reference.
    //! \param rootChild The root widget.
    //!
	void getLayoutSettings (QSettings &settings, QObject *rootChild) const;

	//!
	//! Sets the window layout states
	//!
    //! \param The window layout settings state reference.
	//! \param rootChild The root widget.
    //!
	QWidget * setLayoutSettings (QSettings &settings, QString index = "0");

	//!
	//! Apply the viewport layout on the window
	//!
	//! \param The window reference - It is taken from the QObject.
	//! \param The settings to be applied to the Viewport.
	//!
	void applyViewportLayout(QObject *rootChild, QSettings &settings, QString index = "0");

	//!
	//! Sets the Viewport layout states
	//!
	//! \param The settings of the Viewport.
	//! \param The panelFrame where the settings will be set.
	//!
	void setViewportLayoutSettings(QSettings &settings, PanelFrame *panelFrame, QString index = "0");

	//!
	//! Insert unique IDs to the splitter and panel wedget names
	//!
	void setLayoutIds (QObject *rootChild);

    //!
    //! Returns whether the window is the application's main window.
    //!
    //! \return True if the window is the application's main window, otherwise False.
    //!
    bool isMainWindow ();

    //!
    //! Returns whether the window contains a documentation panel.
    //!
    //! \return True if the window contains a documentation panel, otherwise False.
    //!
    bool hasDocumentation ();

    //!
    //! Opens the documentation main page in the first documentation panel found in
    //! the window's layout.
    //!
    void showDocumentationMainPage ();

    //!
    //! Returns the window's title.
    //!
    //! \return The window's title.
    //!
    QString title ();

    //!
    //! Sets the window's title.
    //!
    //! \param title The title to set for the window.
    //!
    void setTitle ( const QString &title );

    //!
    //! Set fullscreen.
    //!
    void setFullscreen ( bool fullscreen );

	//!
    //! Deletes all layout widgets on contral widget
    //!
	void clearLayout ();

public slots: //

    //!
    //! Updates the window title according to the given panel type name if
    //! there is only one panel in the window.
    //!
    //! \param panelTypeName The name of the new panel type.
    //!
    void updateWindowTitle ( const QString &panelTypeName );

    //!
    //! Duplicates the given panel frame by creating a new window.
    //!
    //! \param panelFrame The panel frame to duplicate.
    //!
    void duplicatePanelFrame ( PanelFrame *panelFrame );

    //!
    //! Extracts the given panel frame to a new window.
    //!
    //! \param panelFrame The panel frame to extract to a new window.
    //!
    void extractPanelFrame ( PanelFrame *panelFrame );

    //!
    //! Closes and deletes the given panel.
    //!
    //! \param panelFrame The panel frame to close.
    //!
    void closePanelFrame ( PanelFrame *panelFrame );

signals: //

    //!
    //! Signal that is emitted when the window has become active.
    //!
    //! \param window The window that was activated.
    //!
    void windowActivated ( Window *window );

    //!
    //! Signal that is emitted when the window's title has changed.
    //!
    //! \param window The window whose title has been changed.
    //!
    void windowRenamed ( Window *window );

    //!
    //! Signal that is emitted when the window has been closed.
    //!
    //! \param window The window that has been closed.
    //! \param event The object containing details about the window close event.
    //!
    void windowClosed ( Window *window, QCloseEvent *event );

    //!
    //! Signal that is emitted when a new window should be created.
    //!
    //! \param panelTypeName The name of the panel type that the new window should contain.
    //! \param position The position for the new window.
    //! \param panelSize The size of the panel to be contained in the new window.
    //!
    void createWindowRequested ( const QString &panelTypeName, const QPoint &position, const QSize &panelSize );

    //!
    //! Signal that is emitted when a ViewPanel-derived widget has been created
    //! for the window.
    //!
    //! \param viewPanel The ViewPanel-derived widget that has been created for the window.
    //!
    void viewPanelCreated ( ViewPanel *viewPanel );

    //!
    //! Signal to notify connected objects that the context menu for creating
    //! new nodes should be displayed at the given position.
    //!
    //! \param position The position at which to display the node context menu.
    //!
    void nodeContextMenuRequested ( const QPoint &position );

protected: // events

    //!
    //! Event handler for context menu events.
    //!
    //! \param event The object that contains details about the event.
    //!
    void contextMenuEvent ( QContextMenuEvent *event );

    //!
    //! Event handler for widget state change events.
    //!
    //! \param event The object that contains details about the event.
    //!
    void changeEvent ( QEvent *event );

    //!
    //! Event handler for widget close events.
    //!
    //! \param event The object that contains details about the event.
    //!
    void closeEvent ( QCloseEvent *event );

private: // functions

    //!
    //! Applies the layout with the given name to the window.
    //! The given panel type is used only in specific layouts.
    //!
    //! \param layoutName The name of the layout to apply to the window.
    //! \param panelType A panel type that is used only in specific layout.
    //!
    void applyLayout ( const QString &layoutName, Panel::Type panelType = Panel::T_Viewport );

private slots:

    // window menu actions

    //!
    //! Apply the default layout on the window.
    //!
    void applyDefaultLayout ();

    //!
    //! Apply the viewport layout on the window, which means that only a viewport
    //! panel is displayed.
    //!
    void applyViewportOnlyLayout ();

private: // data

    //!
    //! The name of the application that this window is a part of.
    //!
    QString m_applicationName;

    //!
    //! Flag that states whether the window is the application's main window.
    //!
    bool m_isMainWindow;

    //!
    //! The window's title.
    //!
    QString m_title;

};

} // end namespace Frapper

#endif
