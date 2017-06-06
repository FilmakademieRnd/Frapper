//!
//! \file "Application.h"
//! \brief Header file for Application class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#ifndef STEREOBOTTIC_H
#define STEREOBOTTIC_H

#include "FrapperPrerequisites.h"
#include "Window.h"
#include "Controller.h"
#include "SceneModel.h"
#include <QtGui/QApplication>
#include <QtGui/QSplashScreen>
#include <QtGui/QAction>

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

// COLLADA
#include <dae/daeErrorHandler.h>


using namespace Frapper;

//!
//! The application's main class.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph Application {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     Application [label="Application",fillcolor="grey75"];
//!     QApplication -> Application;
//!     QApplication [label="QApplication",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qapplication.html"];
//!     LogListener -> Application;
//!     LogListener [label="Ogre::LogListener",fillcolor="#f0a210",URL="http://www.ogre3d.org/docs/api/html/classOgre_1_1LogListener.html"];
//!     daeErrorHandler -> Application;
//!     daeErrorHandler [label="daeErrorHandler",fillcolor="#ffd284",URL="http://www.collada.org/mediawiki/index.php/DOM_guide:_Error_handling"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class StereoBottic : public QApplication, public Ogre::LogListener, public daeErrorHandler
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the StereoBottic class.
    //!
    //! \param organizationName The name of the organization that created the application. This name is used for e.g. storing the application's settings.
    //! \param applicationName The name of the application.
    //! \param argc The number of arguments passed to the application.
    //! \param argv The list of arguments passed to the application.
    //! \param aboutText The text to display in the application's About box.
    //!
    StereoBottic ( const QString &organizationName, const QString &applicationName, int &argc, char **argv, const QString &aboutText = "" );

    //!
    //! Destructor of the Application class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~StereoBottic ();

public: // functions

    //!
    //! Ogre log message handler implementing the Ogre::LogListener interface.
    //!
    //! Is called whenever the log receives a message and is about to write it out.
    //!
    //! \param message The message to be logged.
    //! \param lml The message level the log is using.
    //! \param maskDebug If we are printing to the console or not.
    //! \param logName The name of this log (so you can have several listeners for different logs, and identify them).
    //!
#if(OGRE_VERSION >= 0x010800)
    virtual void messageLogged ( const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName, bool &skipThisMessage );
#else
    virtual void messageLogged ( const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName );
#endif

    //!
    //! COLLADA error message handler.
    //!
    //! Is called when COLLADA produces an error message.
    //!
    //! \param msg The error message.
    //!
    virtual void handleError ( daeString msg );

    //!
    //! COLLADA warning message handler.
    //!
    //! Is called when COLLADA produces a warning message.
    //!
    //! \param msg The warning message.
    //!
    virtual void handleWarning ( daeString msg );

public slots: //

    //!
    //! Slot that is called when a window of the application has been
    //! activated.
    //!
    //! \param window The window that was activated.
    //!
    void handleWindowActivation ( Window *window );

    //!
    //! Slot that is called when a window of the application has been renamed.
    //!
    //! \param window The window whose title was changed.
    //!
    void handleWindowRenaming ( Window *window );

    //!
    //! Slot that is called when a window of the application has been closed.
    //!
    //! \param window The window that was closed.
    //! \param event The object containing details about the window close event.
    //!
    void handleWindowClosing ( Window *window, QCloseEvent *event );

    //!
    //! Creates a new window for the given panel type at the given position
    //! with the given size.
    //!
    //! \param panelTypeName The name of the panel type that the new window should contain.
    //! \param position The position for the new window.
    //! \param panelSize The size of the panel to be contained in the new window.
    //!
    void createWindow ( const QString &panelTypeName, const QPoint &position, const QSize &panelSize );

    //!
    //! Slot that is called when a ViewPanel-derived widget has been created
    //! for one of the application's windows.
    //!
    //! \param viewPanel The ViewPanel-derived widget that has been created for the application.
    //!
    void handleViewPanelCreation ( ViewPanel *viewPanel );

    //!
    //! Updates the application's actions depending on whether objects in the
    //! scene are selected.
    //!
    //! \param objectsSelected Flag that states whether objects in the scene are selected.
    //!
    void updateActions ( bool objectsSelected );

    //!
    //! Displays the context menu for creating nodes at the given position.
    //!
    //! \param position The position at which to display the node context menu.
    //!
    void showNodeContextMenu ( const QPoint &position );

protected: // events

    //!
    //! Handles application close events.
    //!
    //! \param event The object that contains details about the event.
    //!
    void closeEvent ( QCloseEvent *event );

private: // functions

    //!
    //! Sets up the application's actions.
    //!
    void setUpActions ();

    //!
    //! Sets up the application's menu objects.
    //!
    void setUpMenus ();

    //!
    //! Loads the application's settings.
    //!
    void loadSettings ();

    //!
    //! Saves the application's settings.
    //!
    void saveSettings ();

    //!
    //! Sets the modified state of the scene.
    //!
    void setModified ( bool modified );

    //!
    //! Displays the given message in the status bar of the active window.
    //!
    void showStatus ( const QString &message );

    //!
    //! Creates a new window.
    //!
    //! \param layoutName The name of the layout to apply to the new window.
    //! \param isMainWindow Flag to control whether to create the application's main window.
    //!
    Window * createWindow ( const QString &layoutName, bool isMainWindow = false );

    //!
    //! Applies the application window's menus to the given window.
    //!
    //! \param targetWindow The window to add menus to.
    //!
    void addMenus ( Window *targetWindow );

    //!
    //! Checks if the current scene was modified and if so, asks the user whether
    //! the current scene should be saved.
    //!
    //! \param dialogTitle The title to use for the dialog window.
    //!
    bool saveChangesIfNecessary ( const QString &dialogTitle );

    //!
    //! Opens a scene from the current or another scene file.
    //!
    //! \param clear Flag to control whether to clear the current scene.
    //! \param reload Flag to control whether to reload the current scene file.
    //!
    void openScene ( bool clear, bool reload = false );

    //!
    //! Loads the scene from the file with the given name.
    //!
    //! \param filename The name of the file to load.
    //!
    void loadFile ( const QString &filename );

    //!
    //! Saves the current scene to the file with the given name.
    //!
    //! \param filename The name of the file to save the scene to.
    //! \return True if the file could be saved successfully, otherwise False.
    //!
    bool saveFile ( const QString &filename );

    //!
    //! Sets the name of the currently edited scene file.
    //!
    //! \param filename The name of the currently edited scene file.
    //!
    void setCurrentFilename ( const QString &filename );

    //!
    //! Displays an about dialog window with the given information text.
    //!
    //! \param title The title to use for the about dialog.
    //! \param text The text to display in the about dialog.
    //!
    void showAbout ( const QString &title, const QString &text );

signals:
    
    //!
    //! Send Ogre messages. Called by ogre thread.
    //! (Frapper log messages cannot be sent across threads.)
    //!
    void sendOgreMessage ( const QString &message, const QString &level );

private slots: //
    
    //!
    //! Slot processes all messages sent by Ogre. Executed in Frapper thread.
    //! (Frapper log messages cannot be sent across threads.)
    //!
    void processOgreMessage  ( const QString &message, const QString &level );
    
    //!
    //! Sets the modified state of the scene to true.
    //!
    void setModified ();

    //!
    //! Unregisters the main window from the application.
    //!
    //! Is called when the main window has been destroyed.
    //!
    void unregisterMainWindow ();

private slots: // Application actions

    //!
    //! Shows a dialog window with information about the application.
    //!
    void showAboutApplication ();

    //!
    //! Shows a dialog window with information about the application framework.
    //!
    void showAboutFramework ();

    //!
    //! Shows a dialog window with information about Ogre.
    //!
    void showAboutOgre ();

    //!
    //! Shows a dialog window with information about Qt.
    //!
    void showAboutQt ();

    //!
    //! Closes the main window, thus ending the application.
    //!
    void quitApplication ();

private slots: // Scene actions

    //!
    //! Creates a new scene.
    //!
    void newScene ();

    //!
    //! Opens an existing scene.
    //!
    void openScene ();

	//!
	//! Opens an existing layout.
	//!
	void openLayout ( QString filename = "" );

    //!
    //! Reloads the scene from the current scene file.
    //!
    void reloadScene ();

    //!
    //! Merges the current scene with an existing scene from a file.
    //!
    void mergeScene ();

    //!
    //! Saves the scene under the current name.
    //!
    bool saveScene ();

    //!
    //! Saves the scene under a new name.
    //!
    bool saveSceneAs ();

	//!
	//! Saves the default layout.
	//!
	bool saveDafaultLayout ();

	//!
    //! Saves the layout under a new name.
    //!
    bool saveLayoutAs ();

    //!
    //! Closes the scene.
    //!
    void closeScene ();

private slots: // Edit actions

    //!
    //! Undo the action that was performed last.
    //!
    void undo ();

    //!
    //! Redo the action that was undone last.
    //!
    void redo ();

    //!
    //! Copies the selected object to the clipboard and delete it from the scene.
    //!
    void cut ();

    //!
    //! Copies the selected object to the clipboard.
    //!
    void copy ();

    //!
    //! Pastes the contents of the clipboard into the current scene.
    //!
    void paste ();

	//!
	//! Duplicates the selected Item.
	//!
	void duplicate ();

    //!
    //! Opens the preferences dialog window to edit the application's options.
    //!
    void editPreferences ();

	//!
    //! Opens the OGRE Configuration dialog window.
    //!
    void editOgreConfiguration ();	

private slots: // Create actions

    //!
    //! Slot that is called when one of the creation actions is triggered.
    //!
    void creationActionTriggered ();

private slots: // View actions

    //!
    //! Toggles the active window's status bar.
    //!
    void toggleStatusBar ();

private slots: // Tools actions


private slots: // Window actions

    //!
    //! Opens a new window with the default layout.
    //!
    void newWindow ();

    //!
    //! Duplicates the currently active window including its layout.
    //!
    void duplicateActiveWindow ();

    //!
    //! Opens a dialog window for editing the active window's title.
    //!
    void renameActiveWindow ();

    //!
    //! Closes the currently active window.
    //!
    void closeActiveWindow ();

    //!
    //! Closes all of the application's windows but its main window.
    //!
    void closeAdditionalWindows ();

    //!
    //! Opens a dialog box listing all of the application's currently open windows.
    //!
    void listWindows ();

private slots: // Help actions

    //!
    //! If available, highlight the documentation panel in the current window, or
    //! launch a new window containing only a documentation panel.
    //!
    void showDocumentation ();

private: // data

    //!
    //! The name of the organization that created the application.
    //!
    //! This name is used for e.g. storing the application's settings.
    //!
    QString m_organizationName;

    //!
    //! The name of the application.
    //!
    QString m_applicationName;

    //!
    //! The text to display in the application's About box.
    //!
    QString m_aboutText;

    //!
    //! A pointer to the application's main window.
    //!
    Window *m_mainWindow;

    //!
    //! The name of the file currently loaded.
    //!
    QString m_currentFilename;

    //!
    //! Flag that states whether the current scene has been modified.
    //!
    bool m_modified;

    //!
    //! Flag that states whether the current scene is being cleared.
    //!
    bool m_clearingScene;

    // MVC objects

    //!
    //! The model containing scene objects like meshes, lights and cameras.
    //!
    SceneModel *m_sceneModel;

    //!
    //! The controller for setting up model/view connections.
    //!
    Controller *m_controller;

    // OGRE system objects

    //!
    //! The root of the OGRE system.
    //!
    Ogre::Root *m_ogreRoot;

    // additional data

    //!
    //! The height of menu bars in the application's windows.
    //!
    int m_menuBarHeight;

    //!
    //! The height of status bars in the application's windows.
    //!
    int m_statusBarHeight;

private: // data: menu system

    //!
    //! The widget that will be used as parent for all created menus.
    //!
    //! By destroying this widget in the destructor all menus are destroyed as
    //! well.
    //!
    QWidget *m_menuParentWidget;

    // menus
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_createMenu;
    QMenu *m_animationMenu;
    QMenu *m_viewMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;

    // application actions
    QAction *m_aboutApplicationAction;
    QAction *m_aboutFrameworkAction;
    QAction *m_aboutOgreAction;
    QAction *m_aboutQtAction;
    QAction *m_quitAction;
    
    // file actions
    QAction *m_newSceneAction;
    QAction *m_openSceneAction;
    QAction *m_reloadSceneAction;
    QAction *m_mergeSceneAction;
    QAction *m_saveSceneAction;
    QAction *m_saveSceneAsAction;
    QAction *m_closeSceneAction;

    // edit actions
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_duplicateAction;
    QAction *m_deleteAction;
    QAction *m_groupAction;
    QAction *m_ungroupAction;
    QAction *m_selectAllAction;
    QAction *m_preferencesAction;
	QAction *m_ogreConfiguration;

    // view actions
    QAction *m_showAction;
    QAction *m_hideAction;
    QAction *m_statusBarAction;

    // window actions
    QAction *m_applyDefaultLayoutAction;
    QAction *m_resetLayoutAction;
    QAction *m_newDesktopLayoutAction;
    QAction *m_saveAsNewDesktopLayoutAction;
    QAction *m_applyDefaultDesktopLayoutAction;
    QAction *m_saveAsNewWindowLayoutAction;
    QAction *m_applyViewportOnlyLayoutAction;
    QAction *m_applyDualScreen1DesktopLayoutAction;
    QAction *m_applyDualScreen2DesktopLayoutAction;
    QAction *m_saveDesktopLayoutAsNewAction;
    QAction *m_newWindowAction;
    QAction *m_duplicateWindowAction;
    QAction *m_renameWindowAction;
    QAction *m_closeWindowAction;
    QAction *m_closeWindowAction2;
    QAction *m_closeAdditionalWindowsAction;
	QAction *m_openWindowLayoutAction;
	QAction *m_saveDefaultWindowLayoutAction;
    QAction *m_listWindowsAction;

    // help actions
    QAction *m_documentationAction;

};


#endif
