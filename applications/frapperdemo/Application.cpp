//!
//! \file "Application.cpp"
//! \brief Implementation file for Application class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "Application.h"
#include "WindowsDialog.h"
#include "WindowManager.h"
#include "CopyHandler.h"
#include "NetworkGraphicsView.h"
#include "NodeFactory.h"
#include "PanelFactory.h"
#include "WidgetFactory.h"
#include "OgreManager.h"
#include "Log.h"
#include <QSplashScreen>
#include <QBitmap>
#include <QStyleFactory>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressDialog>
#include <QtCore/QDateTime>
#include <QProcess>
#include <QTextstream>


// COLLADA
#define NO_BOOST
#include <dae.h>
#include <1.5/dom/domCOLLADA.h>

// the following include is required for Qt resources to be loaded
//#include "qrc_main.cxx"

///
/// Macro Definitions
///


//!
//! Macro definition for the default scene filename.
//!
#define DEFAULT_FILENAME tr("untitled.dae")


///
/// Constructors and Destructors
///


//!
//! Constructor of the Application class.
//!
//! \param organizationName The name of the organization that created the application. This name is used for e.g. storing the application's settings.
//! \param applicationName The name of the application.
//! \param argc The number of arguments passed to the application.
//! \param argv The list of arguments passed to the application.
//! \param aboutText The text to display in the application's About box.
//!
Application::Application ( const QString &organizationName, const QString &applicationName, int &argc, char **argv, const QString &aboutText /* = "" */ ) :
    QApplication(argc, argv),
    m_organizationName(organizationName),
    m_applicationName(applicationName),
    m_aboutText(aboutText),
    m_mainWindow(0),
    m_currentFilename(DEFAULT_FILENAME),
    m_modified(false),
    m_clearingScene(false),
    // MVC objects
    m_sceneModel(new SceneModel()),
    m_controller(new Controller(m_sceneModel)),
    // OGRE system objects
    m_ogreRoot(0),
    // additional data
    m_menuBarHeight(0),
    m_statusBarHeight(0),
    // menu system
    m_menuParentWidget(new QWidget()),
    // menus
    m_fileMenu(new QMenu(tr("&File"), m_menuParentWidget)),
    m_editMenu(new QMenu(tr("&Edit"), m_menuParentWidget)),
    m_createMenu(new QMenu(tr("&Create"), m_menuParentWidget)),
    m_animationMenu(new QMenu(tr("&Animation"), m_menuParentWidget)),
    m_viewMenu(new QMenu(tr("&View"), m_menuParentWidget)),
    m_windowMenu(new QMenu(tr("&Window"), m_menuParentWidget)),
    m_helpMenu(new QMenu(tr("&Help"), m_menuParentWidget)),
    // application actions
    m_aboutApplicationAction(new QAction(QIcon(":/applicationIcon"), QString("%1 %2").arg(tr("&About")).arg(m_applicationName), this)),
    m_aboutFrameworkAction(new QAction(QIcon(":/splash"), tr("About &Frapper"), this)),
    m_aboutOgreAction(new QAction(QIcon(":/ogreIcon"), tr("About &OGRE"), this)),
    m_aboutQtAction(new QAction(QIcon(":/qtIcon"), tr("About &Qt"), this)),
    m_quitAction(new QAction(tr("E&xit"), this)),
    // file actions
    m_newSceneAction(new QAction(QIcon(":/newIcon"), tr("&New"), this)),
    m_openSceneAction(new QAction(QIcon(":/openIcon"), tr("&Open..."), this)),
	m_reloadSceneAction(new QAction(QIcon(":/refreshIcon"), tr("&Reload"), this)),
    m_mergeSceneAction(new QAction(tr("&Merge..."), this)),
    m_saveSceneAction(new QAction(QIcon(":/saveIcon"), tr("&Save"), this)),
    m_saveSceneAsAction(new QAction(tr("Save &As..."), this)),
    m_closeSceneAction(new QAction(QIcon(":/closeIcon"), tr("&Close"), this)),
    // edit actions
    m_undoAction(new QAction(QIcon(":/undoIcon"), tr("&Undo"), this)),
    m_redoAction(new QAction(QIcon(":/redoIcon"), tr("&Redo"), this)),
    m_cutAction(new QAction(QIcon(":/cutIcon"), tr("Cu&t"), this)),
    m_copyAction(new QAction(QIcon(":/copyIcon"), tr("&Copy"), this)),
    m_pasteAction(new QAction(QIcon(":/pasteIcon"), tr("&Paste"), this)),
	m_duplicateAction(new QAction(QIcon(":/copyIcon"), tr("&Duplicate"), this)),
    m_deleteAction(new QAction(QIcon(":/deleteIcon"), tr("&Delete"), this)),
    m_selectAllAction(new QAction(QIcon(":/frameAllIcon"), tr("Select &All"), this)),
	m_groupAction(new QAction(QIcon(":/groupIcon"), tr("&Group"), this)),
	m_backdropAction(new QAction(QIcon(":/groupIcon"), tr("&BackDrop"), this)),
	m_ungroupAction(new QAction(QIcon(":/ungroupIcon"), tr("&Ungroup"), this)),
    m_preferencesAction(new QAction(QIcon(":/preferencesIcon"), tr("&Preferences..."), this)),
	m_ogreConfiguration(new QAction(QIcon(":/ogreIcon"), tr("&Ogre Configuration..."), this)),
	m_selectWorkingDirectoryAction(new QAction(QIcon(":/openIcon"), tr("&Select Working Directory..."), this)),

    // view actions
    m_showAction(new QAction(tr("Show"), this)),
    m_hideAction(new QAction(tr("Hide"), this)),
    m_statusBarAction(new QAction(tr("&Status Bar"), this)),
    // window actions
    m_applyDefaultLayoutAction(0),
    m_resetLayoutAction(0),
    m_newDesktopLayoutAction(0),
    m_saveAsNewDesktopLayoutAction(0),
    m_applyDefaultDesktopLayoutAction(0),
    m_applyViewportOnlyLayoutAction(0),
    m_applyDualScreen1DesktopLayoutAction(0),
    m_applyDualScreen2DesktopLayoutAction(0),
    m_saveDesktopLayoutAsNewAction(0),
    m_newWindowAction(new QAction(QIcon(":/addWindowIcon"), tr("&New"), this)),
    m_duplicateWindowAction(new QAction(QIcon(":/duplicateWindowIcon"), tr("&Duplicate"), this)),
    m_renameWindowAction(new QAction(QIcon(":/editWindowIcon"), tr("&Rename"), this)),
    m_closeWindowAction(new QAction(QIcon(":/removeWindowIcon"), tr("&Close"), this)),
    m_closeWindowAction2(new QAction(QIcon(":/removeWindowIcon"), tr("&Close Window"), this)),
    m_closeAdditionalWindowsAction(new QAction(QIcon(":/closeWindowsIcon"), tr("Close &All"), this)),
	m_openWindowLayoutAction(new QAction(QIcon(":/openIcon"), tr("Open Layout..."), this)),
	m_saveDefaultWindowLayoutAction(new QAction(QIcon(":/saveIcon"), tr("Save Default Layout"), this)),
	m_saveAsNewWindowLayoutAction(new QAction(QIcon(":/saveIcon"), tr("Save Layout As..."), this)),
    m_listWindowsAction(new QAction(QIcon(":/windowsIcon"), tr("&Windows..."), this)),
    // help actions
    m_documentationAction(new QAction(QIcon(":/helpIcon"), tr("&Documentation"), this))
{
    //setQuitOnLastWindowClosed(false);
    //setAttribute(Qt::AA_DontShowIconsInMenus);    // for a more professional look

    // create, configure and show the splash screen window
    QPixmap pixmap = QPixmap(":/splash");
    QSplashScreen splashScreen (pixmap);
    splashScreen.setAttribute(Qt::WA_DeleteOnClose);  // important: destroy the splash screen when it is closed
    splashScreen.setMask(pixmap.mask());
    splashScreen.show();    
	Qt::Alignment splashScreenAlignment = Qt::AlignHCenter | Qt::AlignBottom;
    QColor splashScreenColor (Qt::black);
    QString statusMessage;
    processEvents();

    // update status message
    statusMessage = "\nLoading icons...";
    Log::debug(statusMessage, "Application::Application");
    splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);
    
    // load icons
    Log::loadIcons();
    setWindowIcon(QIcon(":/applicationIcon"));

    // update status message
    statusMessage = "\nLoading settings...";
    Log::debug(statusMessage, "Application::Application");
    splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

    // load settings
    loadSettings();
    setCurrentFilename(m_currentFilename);

    // update status message
    statusMessage = "\nInitializing OGRE...";
    Log::debug(statusMessage, "Application::Application");
    splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

    // create and initialize OGRE system objects
	// Using absolute paths here is more robust against changes of current directory during runtime
	const Ogre::String applicationPath = QDir::currentPath().toStdString();
	const Ogre::String pluginFileName   = applicationPath + "/config/plugins.cfg";
	const Ogre::String configFileName   = applicationPath + "/config/ogre.cfg";
	const Ogre::String resourceFileName = applicationPath + "/config/resources.cfg";
	const Ogre::String logFileName      = applicationPath + "/logs/ogre.log";
	OgreManager::initialize( pluginFileName, configFileName, resourceFileName, logFileName);
    m_sceneModel->createSceneRoot();

    // add the application object as a log listener to OGRE's default log to receive OGRE log messages
    connect(this, SIGNAL(sendOgreMessage(const QString &, const QString &)), SLOT(processOgreMessage(const QString &, const QString &)), Qt::QueuedConnection);
    Ogre::Log *defaultLog = Ogre::LogManager::getSingletonPtr()->getDefaultLog();
    defaultLog->addListener(this);
	    	
	// update status message
	statusMessage = "\nLoading widget types...";
	Log::debug(statusMessage, "Application::Application");
	splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

	// parse the XML directory and look for widget type description files
	QDir widgetDir ("plugins/widgets");
	QFileInfoList fileInfoList = widgetDir.entryInfoList(QStringList() << "*.xml", QDir::Files);
	if (fileInfoList.size() > 0)
		for (int i = 0; i < fileInfoList.size(); ++i) {
			QFileInfo fileInfo = fileInfoList.at(i);
			WidgetFactory::registerType(fileInfo.absoluteFilePath());
		}
	else
		Log::warning(QString("No plugin files for widget types found in \"%1\".").arg(widgetDir.path()), "Application::Application");


	// update status message
	statusMessage = "\nLoading panel types...";
	Log::debug(statusMessage, "Application::Application");
	splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

	// parse the XML directory and look for panel type description files
    QDir panelDir ("plugins/panels");
    fileInfoList = panelDir.entryInfoList(QStringList() << "*.xml", QDir::Files);
    if (fileInfoList.size() > 0)
        for (int i = 0; i < fileInfoList.size(); ++i) {
            QFileInfo fileInfo = fileInfoList.at(i);
			PanelFactory::registerType(fileInfo.absoluteFilePath());
        }
    else
        Log::warning(QString("No plugin files for panel types found in \"%1\".").arg(panelDir.path()), "Application::Application");


	// update status message
	statusMessage = "\nLoading node types...";
	Log::debug(statusMessage, "Application::Application");
	splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

	// parse the XML directory and look for node type description files
	NodeFactory::initialize();
	QDir nodeDir ("plugins/nodes");
	fileInfoList = nodeDir.entryInfoList(QStringList() << "*.xml", QDir::Files);
	if (fileInfoList.size() > 0)
		for (int i = 0; i < fileInfoList.size(); ++i) {
			QFileInfo fileInfo = fileInfoList.at(i);
			NodeFactory::registerType(fileInfo.absoluteFilePath());
		}
	else
		Log::warning(QString("No XML description files for node types found in \"%1\".").arg(nodeDir.path()), "Application::Application");



	//
	// END EXPERIMENT
	// 

    // update status message
    statusMessage = "\nSetting up menu system...";
    Log::debug(statusMessage, "Application::Application");
    splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

    // set up the application's actions and menus
    setUpActions();
    setUpMenus();

    // set up scene model signal/slot connections
    connect(m_sceneModel, SIGNAL(modified()), SLOT(setModified()));
    connect(m_sceneModel, SIGNAL(selectionChanged(bool)), SLOT(updateActions(bool)));

    // update status message
    statusMessage = "\nCreating main window...";
    Log::debug(statusMessage, "Application::Application");
    splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

    // create the application's main window
    m_mainWindow = createWindow(Window::DefaultLayout, true);
    m_mainWindow->setWindowState(Qt::WindowMaximized);
    m_mainWindow->show();

    // store the heights of the main window's menu and status bars (will be used when determining the position and size of new windows)
    m_menuBarHeight = m_mainWindow->menuBar()->height();
    m_statusBarHeight = m_mainWindow->statusBar()->height();

    // process arguments
    if (argc > 0) {
        if (argc == 2) {
            // update status message
            statusMessage = "\nLoading scene file...";
            Log::debug(statusMessage, "Application::Application");
            splashScreen.showMessage(statusMessage, splashScreenAlignment, splashScreenColor);

            // use the second argument as the name of a scene file to load
            QString filename = QString(argv[1]);
			setCurrentFilename(filename);
            loadFile(filename);

            // home the view in all network graphics views
            foreach (NetworkGraphicsView *networkGraphicsView, m_mainWindow->findChildren<NetworkGraphicsView *>())
                networkGraphicsView->homeView();
        }
    } else
        showStatus(tr("Ready"));

    // close (and destroy) the splash screen once the main window is displayed
    splashScreen.finish(m_mainWindow);
}


//!
//! Destructor of the Application class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
Application::~Application ()
{
    delete m_controller;
    delete m_sceneModel;

    NodeFactory::freeResources();
	PanelFactory::freeResources();

    delete m_ogreRoot;

    // delete the menu parent widget so that all created menus are destroyed as well
    delete m_menuParentWidget;

    Log::debug("Application destroyed.", "Application::~Application");
}


///
/// Public Functions
///


//!
//! OGRE log message handler implementing the Ogre::LogListener interface.
//!
//! Is called whenever the log receives a message and is about to write it out.
//!
//! \param message The message to be logged.
//! \param lml The message level the log is using.
//! \param maskDebug If we are printing to the console or not.
//! \param logName The name of this log (so you can have several listeners for different logs, and identify them).
//!
#if(OGRE_VERSION >= 0x010800)
void Application::messageLogged ( const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName, bool &skipThisMessage )
#else
void Application::messageLogged ( const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName )
#endif
{
#if(OGRE_VERSION >= 0x010800)
	if (skipThisMessage)
		return;
#endif
    QString level;
    switch (lml) {
        case Ogre::LML_TRIVIAL:
            level = "trivial";
            break;
        case Ogre::LML_NORMAL:
            level = "normal";
            break;
        case Ogre::LML_CRITICAL:
            level = "critical";
            break;
    }
    
    //Log::addOgreLogMessage(QString("%1").arg(message.c_str()), level);
    sendOgreMessage(QString("%1").arg(message.c_str()), level);
}


//!
//! COLLADA error message handler.
//!
//! Is called when COLLADA produces an error message.
//!
//! \param msg The error message.
//!
void Application::handleError ( daeString msg )
{
    Log::error(msg, "COLLADA");
}


//!
//! COLLADA warning message handler.
//!
//! Is called when COLLADA produces a warning message.
//!
//! \param msg The warning message.
//!
void Application::handleWarning ( daeString msg )
{
    Log::warning(msg, "COLLADA");
}


///
/// Public Slots
///


//!
//! Slot that is called when a window of the application has been activated.
//!
//! \param window The window that was activated.
//!
void Application::handleWindowActivation ( Window *window )
{
    WindowManager::highlightWindowAction(window);

    // update file menu actions
    m_closeWindowAction2->setVisible(!window->isMainWindow());

    // update view menu actions
    m_statusBarAction->setChecked(window->statusBar()->isVisible());

    // update window menu actions
    m_renameWindowAction->setEnabled(!window->isMainWindow());
    m_closeWindowAction->setEnabled(!window->isMainWindow());
}


//!
//! Slot that is called when a window of the application has been renamed.
//!
//! \param window The window whose title was changed.
//!
void Application::handleWindowRenaming ( Window *window )
{
    WindowManager::updateWindowActionTitle(window);
}


//!
//! Slot that is called when a window of the application has been closed.
//!
//! \param window The window that was closed.
//! \param event The object containing details about the window close event.
//!
void Application::handleWindowClosing ( Window *window, QCloseEvent *event )
{
    // check if the window is the main window or one of the additional windows
    if (window->isMainWindow()) {
        // check if the scene needs to be saved
        if (!saveChangesIfNecessary(tr("Exit"))) {
            // tell Qt to ignore the close event
            event->ignore();
            return;
        }

        WindowManager::closeAdditionalWindows();
    } else
        WindowManager::unregisterWindow(window);

    // update window actions
    m_closeAdditionalWindowsAction->setEnabled(WindowManager::hasAdditionalWindows());
    m_listWindowsAction->setEnabled(WindowManager::hasAdditionalWindows());

    // tell Qt to accept the close event
    event->accept();
}


//!
//! Creates a new window for the given panel type at the given position
//! with the given size.
//!
//! \param panelTypeName The name of the panel type that the new window should contain.
//! \param position The position for the new window.
//! \param panelSize The size of the panel to be contained in the new window.
//!
void Application::createWindow ( const QString &panelTypeName, const QPoint &position, const QSize &panelSize )
{
    Window *newWindow = createWindow(panelTypeName, false);

    // set the window's position and size
    newWindow->move(position - QPoint(0, m_menuBarHeight));
    newWindow->resize(panelSize + QSize(0, m_menuBarHeight + m_statusBarHeight));

    newWindow->show();

	if (panelTypeName == Panel::getTypeName(Panel::T_Viewport)) {
		openViewportLayout(newWindow);	// to apply the Viewport parameters stored in the layout
		m_sceneModel->rebuildScene();		
	}
}


//!
//! Slot that is called when a ViewPanel-derived widget has been created for
//! one of the application's windows.
//!
//! \param viewPanel The ViewPanel-derived widget that has been created for the application.
//!
void Application::handleViewPanelCreation ( ViewPanel *viewPanel )
{
    m_controller->registerViewPanel(viewPanel);
}


//!
//! Updates the application's actions depending on whether objects in the
//! scene are selected.
//!
//! \param objectsSelected Flag that states whether objects in the scene are selected.
//!
void Application::updateActions ( bool objectsSelected )
{
    // update actions
    m_cutAction->setEnabled(objectsSelected);
    m_copyAction->setEnabled(objectsSelected);
    m_duplicateAction->setEnabled(objectsSelected);
    m_deleteAction->setEnabled(objectsSelected);
    m_showAction->setEnabled(objectsSelected);
    m_hideAction->setEnabled(objectsSelected);
}


//!
//! Displays the context menu for creating nodes at the given position.
//!
//! \param position The position at which to display the node context menu.
//!
void Application::showNodeContextMenu ( const QPoint &position )
{
    QMenu nodeContextMenu;
    nodeContextMenu.setObjectName("NodeContextMenu");
    nodeContextMenu.addActions(m_createMenu->actions());
    nodeContextMenu.exec(position);
}


///
/// Private Functions
///


//!
//! Sets up the application's actions.
//!
void Application::setUpActions ()
{
    /// Application actions

    // about application
    m_aboutApplicationAction->setStatusTip(tr("Display information about this application"));
    m_aboutApplicationAction->setMenuRole(QAction::AboutRole);
    connect(m_aboutApplicationAction, SIGNAL(triggered()), SLOT(showAboutApplication()));

    // about application framework
    m_aboutFrameworkAction->setStatusTip(tr("Display information about Frapper"));
    m_aboutFrameworkAction->setMenuRole(QAction::AboutRole);
    connect(m_aboutFrameworkAction, SIGNAL(triggered()), SLOT(showAboutFramework()));

    // about Ogre
    m_aboutOgreAction->setStatusTip(tr("Display information about OGRE"));
    m_aboutOgreAction->setMenuRole(QAction::AboutRole);
    connect(m_aboutOgreAction, SIGNAL(triggered()), SLOT(showAboutOgre()));

    // about Qt
    m_aboutQtAction->setStatusTip(tr("Display information about Qt"));
    m_aboutQtAction->setMenuRole(QAction::AboutRole);
    connect(m_aboutQtAction, SIGNAL(triggered()), SLOT(showAboutQt()));

    // quit
    m_quitAction->setShortcut(tr("Ctrl+Q"));
    m_quitAction->setStatusTip(tr("Exit the application"));
    m_quitAction->setMenuRole(QAction::QuitRole);
    connect(m_quitAction, SIGNAL(triggered()), SLOT(quitApplication()));

    /// Scene actions

    // new scene
    m_newSceneAction->setShortcut(tr("Ctrl+N"));
    m_newSceneAction->setStatusTip(tr("Create a new scene"));
    connect(m_newSceneAction, SIGNAL(triggered()), SLOT(newScene()));

    // open scene
    m_openSceneAction->setShortcut(tr("Ctrl+O"));
    m_openSceneAction->setStatusTip(tr("Open an existing scene from file"));
    connect(m_openSceneAction, SIGNAL(triggered()), SLOT(openScene()));

	// open layout
    m_openWindowLayoutAction->setStatusTip(tr("Open an existing layout from file"));
    connect(m_openWindowLayoutAction, SIGNAL(triggered()), SLOT(openLayout()));

    // reload scene
	m_reloadSceneAction->setShortcut(tr("Ctrl+R"));
    m_reloadSceneAction->setStatusTip(tr("Reload the previously opened scene file"));
    connect(m_reloadSceneAction, SIGNAL(triggered()), SLOT(reloadScene()));

    // reload scene
    m_mergeSceneAction->setStatusTip(tr("Open an existing scene from file and merge it with the current scene"));
    connect(m_mergeSceneAction, SIGNAL(triggered()), SLOT(mergeScene()));

    // save scene
    m_saveSceneAction->setShortcut(tr("Ctrl+S"));
    m_saveSceneAction->setStatusTip(tr("Save the scene under its current filename"));
    connect(m_saveSceneAction, SIGNAL(triggered()), SLOT(saveScene()));

    // save scene as
    m_saveSceneAsAction->setShortcut(tr("Ctrl+Shift+S"));
    m_saveSceneAsAction->setStatusTip(tr("Save the current scene under a new filename"));
    connect(m_saveSceneAsAction, SIGNAL(triggered()), SLOT(saveSceneAs()));

	// save new layout
	m_saveAsNewWindowLayoutAction->setStatusTip(tr("Save the current layout under a new filename"));
    connect(m_saveAsNewWindowLayoutAction, SIGNAL(triggered()), SLOT(saveLayoutAs()));

	// save default layout
	m_saveDefaultWindowLayoutAction->setStatusTip(tr("Save the current layout as default Layout"));
    connect(m_saveDefaultWindowLayoutAction, SIGNAL(triggered()), SLOT(saveDafaultLayout()));

    // close scene
    m_closeSceneAction->setShortcut(tr("Ctrl+W"));
    m_closeSceneAction->setStatusTip(tr("Close the current scene file"));
    connect(m_closeSceneAction, SIGNAL(triggered()), SLOT(closeScene()));

    /// Edit actions

    // undo
    //m_undoAction->setShortcut(tr("Ctrl+Z"));
    //m_undoAction->setStatusTip(tr("Undo the previously performed command"));
    //m_undoAction->setEnabled(false);
    //connect(m_undoAction, SIGNAL(triggered()), SLOT(undo()));

    // redo
    //m_redoAction->setShortcut(tr("Ctrl+Y"));
    //m_redoAction->setStatusTip(tr("Redo the previously undone command"));
    //m_redoAction->setEnabled(false);
    connect(m_redoAction, SIGNAL(triggered()), SLOT(redo()));

    // cut
    m_cutAction->setShortcut(tr("Ctrl+X"));
    m_cutAction->setStatusTip(tr("Copy the selected objects to the clipboard and delete them from the scene"));
    connect(m_cutAction, SIGNAL(triggered()), SLOT(cut()));

    // copy
    m_copyAction->setShortcut(tr("Ctrl+C"));
    m_copyAction->setStatusTip(tr("Copy the selected objects to the clipboard"));
    connect(m_copyAction, SIGNAL(triggered()), SLOT(copy()));

    // paste
    m_pasteAction->setShortcut(tr("Ctrl+V"));
    m_pasteAction->setStatusTip(tr("Paste the contents of the clipboard into the scene"));
    connect(m_pasteAction, SIGNAL(triggered()), SLOT(paste()));

	// duplicate
    m_duplicateAction->setShortcut(tr("Ctrl+D"));
    m_duplicateAction->setStatusTip(tr("Dublicates the selected Item"));
    connect(m_duplicateAction, SIGNAL(triggered()), SLOT(duplicate()));

    // delete
    //m_deleteAction->setShortcut(tr("Del")); // implemented in NetworkPanel -> NetworkGraphicsView
    m_deleteAction->setStatusTip(tr("Delete the currently selected objects"));
    connect(m_deleteAction, SIGNAL(triggered()), m_sceneModel, SLOT(deleteSelected()));

	// group
	m_groupAction->setShortcut(tr("Ctrl+G"));
    m_groupAction->setStatusTip(tr("Group the currently selected objects"));
    connect(m_groupAction, SIGNAL(triggered()), m_sceneModel, SLOT(groupSelected()));

	// backdrop
	m_backdropAction->setShortcut(tr("Ctrl+B"));
	m_backdropAction->setStatusTip(tr("Create a backdrop node"));
	connect(m_backdropAction, SIGNAL(triggered()), m_sceneModel, SLOT(backDrop()));

	// ungroup
	m_ungroupAction->setShortcut(tr("Ctrl+U"));
    m_ungroupAction->setStatusTip(tr("Ungroup the currently selected objects"));
    connect(m_ungroupAction, SIGNAL(triggered()), m_sceneModel, SLOT(ungroupSelected()));

    // select all
    m_selectAllAction->setShortcut(tr("Ctrl+A"));
    m_selectAllAction->setStatusTip(tr("Select all objects in the scene"));
    connect(m_selectAllAction, SIGNAL(triggered()), m_sceneModel, SLOT(selectAll()));

    // preferences
    m_preferencesAction->setShortcut(tr("Ctrl+,"));
    m_preferencesAction->setStatusTip(tr("Edit the application's options and settings"));
    m_preferencesAction->setMenuRole(QAction::PreferencesRole);
    connect(m_preferencesAction, SIGNAL(triggered()), SLOT(editPreferences()));

	// OGRE Configuration
	m_ogreConfiguration->setShortcut(tr("Ctrl+."));
	m_ogreConfiguration->setStatusTip(tr("Edit the OGRE configuration"));
	connect (m_ogreConfiguration, SIGNAL(triggered()), SLOT(editOgreConfiguration()));

	// Select Working Directory
	m_selectWorkingDirectoryAction->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_O );
	m_selectWorkingDirectoryAction->setStatusTip(tr("Select the working directory for this scene."));
	connect (m_selectWorkingDirectoryAction, SIGNAL(triggered()), SLOT(selectWorkingDirectory()));

    /// View actions

    // show
    m_showAction->setShortcut(tr("Shift+H"));
    m_showAction->setStatusTip(tr("Make the selected objects visible by considering them when evaluating the network"));
    connect(m_showAction, SIGNAL(triggered()), m_sceneModel, SLOT(evaluateSelectedObjects()));

    // hide
    m_hideAction->setShortcut(tr("Ctrl+H"));
    m_hideAction->setStatusTip(tr("Make the selected objects invisible by ignoring them when evaluating the network"));
    connect(m_hideAction, SIGNAL(triggered()), m_sceneModel, SLOT(ignoreSelectedObjects()));

    // status bar
    m_statusBarAction->setCheckable(true);
    m_statusBarAction->setChecked(true);
    m_statusBarAction->setStatusTip(tr("Toggle this window's status bar"));
    connect(m_statusBarAction, SIGNAL(triggered()), SLOT(toggleStatusBar()));

    /// Window actions

    // new window
    m_newWindowAction->setShortcut(tr("Ctrl+Shift+N"));
    m_newWindowAction->setStatusTip(tr("Open a new window with a default panel layout"));
    connect(m_newWindowAction, SIGNAL(triggered()), SLOT(newWindow()));

    // duplicate window
    m_duplicateWindowAction->setStatusTip(tr("Duplicate this window including its layout"));
    connect(m_duplicateWindowAction, SIGNAL(triggered()), SLOT(duplicateActiveWindow()));

    // rename window
    m_renameWindowAction->setStatusTip(tr("Edit this window's title"));
    connect(m_renameWindowAction, SIGNAL(triggered()), SLOT(renameActiveWindow()));

    // close window
    m_closeWindowAction->setStatusTip(tr("Close this window"));
    connect(m_closeWindowAction, SIGNAL(triggered()), SLOT(closeActiveWindow()));

    // close window (for the File menu)
    m_closeWindowAction2->setStatusTip(m_closeWindowAction->statusTip());
    connect(m_closeWindowAction2, SIGNAL(triggered()), SLOT(closeActiveWindow()));

    // close all additional windows
    m_closeAdditionalWindowsAction->setStatusTip(tr("Close all windows but the main window"));
    m_closeAdditionalWindowsAction->setEnabled(false);
    connect(m_closeAdditionalWindowsAction, SIGNAL(triggered()), SLOT(closeAdditionalWindows()));

    // list windows
    m_listWindowsAction->setStatusTip(tr("List the application's windows"));
    m_listWindowsAction->setEnabled(false);
    connect(m_listWindowsAction, SIGNAL(triggered()), SLOT(listWindows()));

    /// Help actions

    // documentation
    m_documentationAction->setShortcut(tr("F1"));
    m_documentationAction->setStatusTip(tr("Open the documentation's main page"));
    connect(m_documentationAction, SIGNAL(triggered()), SLOT(showDocumentation()));

}


//!
//! Sets up the application's menu objects.
//!
void Application::setUpMenus ()
{
    // file menu
    m_fileMenu->setObjectName("FileMenu");
    QAction *separatorAction1 = m_fileMenu->addSeparator();
    QAction *separatorAction2 = m_fileMenu->addSeparator();
    QAction *separatorAction3 = m_fileMenu->addSeparator();
    QAction *separatorAction4 = m_fileMenu->addSeparator();
    m_fileMenu->addActions(QList<QAction *>()
        << m_newSceneAction
        << separatorAction1
        << m_openSceneAction
        << m_reloadSceneAction
        << m_mergeSceneAction
        << separatorAction2
        << m_saveSceneAction
        << m_saveSceneAsAction
        << separatorAction3
        << m_closeSceneAction
        << separatorAction4
        << m_closeWindowAction2
        << m_quitAction
    );

    // edit menu
    m_editMenu->setObjectName("EditMenu");
    separatorAction1 = m_editMenu->addSeparator();
    separatorAction2 = m_editMenu->addSeparator();
    separatorAction3 = m_editMenu->addSeparator();
    separatorAction4 = m_editMenu->addSeparator();
    m_editMenu->addActions(QList<QAction *>()
        << m_undoAction
        << m_redoAction
        << separatorAction1
        << m_cutAction
        << m_copyAction
        << m_pasteAction
		<< m_duplicateAction
        << separatorAction2
        << m_deleteAction
        << separatorAction3
        << m_selectAllAction
		<< m_groupAction
		<< m_backdropAction
		<< m_ungroupAction
        << separatorAction4
        << m_preferencesAction
		<< m_ogreConfiguration
		<< m_selectWorkingDirectoryAction
    );

    // create menu
    m_createMenu->setObjectName("CreateMenu");

    // fill the create menu with node type category actions
    QMap<QString, QAction *> nodeTypeCategoryActions;
    for (unsigned int i = 0; i < NodeFactory::getNumCategories(); ++i) {
        QString categoryName = NodeFactory::getCategoryName(i);
        if (!categoryName.isEmpty()) {
            QString categoryIconName = NodeFactory::getCategoryIconName(i);
            QString categoryActionText = QString("&%1%2").arg(categoryName.left(1)).arg(categoryName.mid(1));
            if (categoryIconName.isEmpty())
                nodeTypeCategoryActions[categoryName] = m_createMenu->addAction(categoryActionText);
            else
                nodeTypeCategoryActions[categoryName] = m_createMenu->addAction(QIcon(categoryIconName), categoryActionText);
            nodeTypeCategoryActions[categoryName]->setMenu(new QMenu(m_menuParentWidget));
            nodeTypeCategoryActions[categoryName]->setEnabled(false);
        } else
            m_createMenu->addSeparator();
    }
    m_createMenu->addSeparator();

    // iterate over the list of node type names and create a menu action for each node type
    QStringList typeNames = NodeFactory::getTypeNames();
    for (int i = 0; i < typeNames.size(); ++i) {
        QString typeName = typeNames.at(i);
        QString categoryName = NodeFactory::getCategoryName(typeName);
        // check if the node type category action already exists
        if (nodeTypeCategoryActions.contains(categoryName)) {
            // enable the node type category action if it is disabled
            if (!nodeTypeCategoryActions[categoryName]->isEnabled())
                nodeTypeCategoryActions[categoryName]->setEnabled(true);
        } else
            Log::warning(QString("Node type category named \"%1\" could not be obtained.").arg(categoryName), "Application::setUpMenus");
        // create a submenu action for the node type
        QString iconName = ":/pluginIcon";
        if (NodeFactory::isErroneous(typeName))
            iconName = ":/pluginWarningIcon";
        QAction *creationAction = nodeTypeCategoryActions[categoryName]->menu()->addAction(QIcon(iconName), QString("&%1%2").arg(typeName.left(1)).arg(typeName.mid(1)));
        connect(creationAction, SIGNAL(triggered()), SLOT(creationActionTriggered()));
    }

    // animation menu
    m_animationMenu->setObjectName("AnimationMenu");
    m_animationMenu->addActions(m_sceneModel->getControlActions());

    // view menu
    m_viewMenu->setObjectName("ViewMenu");
    separatorAction1 = m_viewMenu->addSeparator();
    m_viewMenu->addActions(QList<QAction *>()
        << m_showAction
        << m_hideAction
        << separatorAction1
        << m_statusBarAction
    );

    // window menu
    m_windowMenu->setObjectName("WindowMenu");
    separatorAction1 = m_windowMenu->addSeparator();
    separatorAction2 = m_windowMenu->addSeparator();
    m_windowMenu->addActions(QList<QAction *>()
        << m_newWindowAction
        << m_duplicateWindowAction
        << m_renameWindowAction
        << m_closeWindowAction
        << m_closeAdditionalWindowsAction
        << separatorAction1
		<< m_openWindowLayoutAction
		<< m_saveAsNewWindowLayoutAction
		<< m_saveDefaultWindowLayoutAction
		<< separatorAction2
        << m_listWindowsAction

    );

    // help menu
    m_helpMenu->setObjectName("HelpMenu");
    separatorAction1 = m_helpMenu->addSeparator();
    m_helpMenu->addActions(QList<QAction *>()
        << m_documentationAction
        << separatorAction1
        //<< m_aboutApplicationAction
        << m_aboutFrameworkAction
        << m_aboutOgreAction
        << m_aboutQtAction
        //<< m_aboutColladaAction
    );

    // make the window menu known to the window manager so that it may update it
    // with actions that can be used for switching between the application's windows
    WindowManager::registerWindowMenu(m_windowMenu);
}


//!
//! Loads the application's settings.
//!
void Application::loadSettings ()
{
    // set up the application's style
#if QT_VERSION >= 0x050000
	QString styleName = "fusion";
#else
	QString styleName = "plastique";
#endif
    QStyle *style = QStyleFactory::create(styleName);
    setStyle(style);

	QFile file("resources/default.css");
	QString stylesheet;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		while (!in.atEnd()) {
			stylesheet += in.readLine(); 
		}
	}
	setStyleSheet(stylesheet);

    setPalette(style->standardPalette());

    //QSettings settings (m_organizationName, m_applicationName);
    //QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    //QSize size = settings.value("size", QSize(400, 400)).toSize();
    //resize(size);
    //move(pos);
}


//!
//! Saves the application's settings.
//!
void Application::saveSettings ()
{
    //QSettings settings (m_organizationName, m_applicationName);
    //settings.setValue("pos", pos());
    //settings.setValue("size", size());
}


//!
//! Sets the modified state of the scene.
//!
void Application::setModified ( bool modified )
{
    // skip this function if the scene is currently being cleared
    if (m_clearingScene)
        return;

    m_modified = modified;

    //if (m_mainWindow)
    //    m_mainWindow->setWindowModified(m_modified);
}


//!
//! Displays the given message in the status bar of the active window.
//!
void Application::showStatus ( const QString &message )
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    if (activeWindow)
        activeWindow->statusBar()->showMessage(message, 2000);
}


//!
//! Creates a new window.
//!
//! \param layoutName The name of the layout to apply to the new window.
//! \param isMainWindow Flag to control whether to create the application's main window.
//!
Window * Application::createWindow ( const QString &layoutName, bool isMainWindow /* = false */ )
{
    QString windowTitle;
    if (isMainWindow)
        windowTitle = tr("Main Window");

    Window *newWindow = new Window(this, m_applicationName, windowTitle, layoutName, isMainWindow);

    if (isMainWindow)
        connect(newWindow, SIGNAL(destroyed(QObject *)), SLOT(unregisterMainWindow()));

    addMenus(newWindow);

    WindowManager::registerWindow(newWindow);

    // update window actions
    m_closeAdditionalWindowsAction->setEnabled(!isMainWindow);
    m_listWindowsAction->setEnabled(!isMainWindow);

    return newWindow;
}


//!
//! Applies the application window's menus to the given window.
//!
//! \param targetWindow The window to add menus to.
//!
void Application::addMenus ( Window *targetWindow )
{
    // make sure the pointer to the window is valid
    if (!targetWindow)
        return;

    QMenuBar *menuBar = targetWindow->menuBar();
    menuBar->addMenu(m_fileMenu);
    menuBar->addMenu(m_editMenu);
    menuBar->addMenu(m_createMenu);
    menuBar->addMenu(m_animationMenu);
    menuBar->addMenu(m_viewMenu);
    menuBar->addMenu(m_windowMenu);
    menuBar->addSeparator();    // only affects the "motif" style
    menuBar->addMenu(m_helpMenu);

    // set up connection for displaying a node context menu for specific panels
    connect(targetWindow, SIGNAL(nodeContextMenuRequested(const QPoint &)), SLOT(showNodeContextMenu(const QPoint &)));
}


//!
//! Checks if the current scene was modified and if so, asks the user whether
//! the current scene should be saved.
//!
//! \param dialogTitle The title to use for the dialog window.
//!
bool Application::saveChangesIfNecessary ( const QString &dialogTitle )
{
    if (m_modified) {
        Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
        QMessageBox::StandardButton clickedButton = QMessageBox::warning(
                activeWindow,
                dialogTitle,
                tr("The current scene has been modified.") + "\n" +
                tr("Do you want to save your changes?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );

        if (clickedButton == QMessageBox::Save)
            return saveScene();
        else if (clickedButton == QMessageBox::Cancel)
            return false;
    }
    return true;
}


//!
//! Opens a scene from the current or another scene file.
//!
//! \param clear Flag to control whether to clear the current scene.
//! \param reload Flag to control whether to reload the current scene file.
//!
void Application::openScene ( bool clear, bool reload /* = false */ )
{
    // get name of file to open
    QString filename (m_currentFilename);
    if (!reload) {
        // open a file selection dialog
        Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
        filename = QFileDialog::getOpenFileName(activeWindow, tr("Open Scene File"), QFileInfo(filename).absoluteDir().path(), tr("COLLADA files (*.dae)"));
        if (filename.isEmpty())
            return;
    }

    // clear the scene if requested
    if (clear) {
        m_clearingScene = true;
        m_sceneModel->clear();
        m_clearingScene = false;
    }

	if (clear)
		setCurrentFilename(filename);

    // load the scene from file
    loadFile(filename);

    // home the view in all network graphics views
    foreach (NetworkGraphicsView *networkGraphicsView, m_mainWindow->findChildren<NetworkGraphicsView *>())
        networkGraphicsView->homeView();
}


//!
//! Opens an existing layout.
//!
void Application::openLayout ( QString filename /* = "" */ )
{
    // get name of file to open
    if (filename.isEmpty()) {
        // open a file selection dialog
        Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
        filename = QFileDialog::getOpenFileName(activeWindow, tr("Open Layout File"), QDir(m_currentFilename).path(), tr("Layout files (*.lt)"));		
    }

    if (!QFile::exists(filename)) {
        Log::error(QString("The file \"%1\" could not be found.").arg(filename), "Application::openLayout");
		m_currentLayoutFilename = "config/default.lt";
        return;
    }

	m_currentLayoutFilename = filename; 
	QSettings settings(filename, QSettings::IniFormat);
	QWidget *newLayout = m_mainWindow->setLayoutSettings(settings);
	if (newLayout != 0) {
		m_mainWindow->clearLayout();
		m_mainWindow->setCentralWidget(newLayout);
	}
	else
		Log::error(QString("Error in File \"%1\".").arg(filename), "Application::openLayout");
}

//!
//! Opens an existing layout to  be loaded into the Viewport panel (useful when Tear Off of the Viewport is done)
//!
void Application::openViewportLayout ( Window *window  )
{
	QSettings settings(m_currentLayoutFilename, QSettings::IniFormat);
	window->applyViewportLayout(window, settings);
}

//!
//! Loads the scene from the file with the given name.
//!
//! \param filename The name of the file to load.
//!
void Application::loadFile ( const QString &filename )
{
    // check if the file exists
    if (!QFile::exists(filename)) {
        Log::error(QString("The file \"%1\" could not be found.").arg(filename), "Application::loadFile");
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // load the scene from the scene file
    DAE dae;
    daeErrorHandler::setErrorHandler(this);

    daeElement *rootElement = dae.open(filename.toStdString());
    if (rootElement) {
		QString layoutFilename = filename;
        layoutFilename.replace(".dae", ".lt");
        openLayout(layoutFilename);
		m_sceneModel->setSceneFileName(filename);
        m_sceneModel->createScene(rootElement);
        dae.close(filename.toStdString());
        rootElement = 0;
        showStatus(QString(tr("File \"%1\" loaded")).arg(filename));
    } else
        Log::error(QString("The file \"%1\" could not be loaded.").arg(filename), "Application::loadFile");

    QApplication::restoreOverrideCursor();
}


//!
//! Saves the current scene to the file with the given name.
//!
//! \param filename The name of the file to save the scene to.
//! \return True if the file could be saved successfully, otherwise False.
//!
bool Application::saveFile ( const QString &filename )
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // obtain creation and modification dates
    QDateTime now = QDateTime::currentDateTime();
    QString xsdDateTimeFormat ("yyyy-MM-ddThh:mm:ssZ");
    QString creationDate = now.toString(xsdDateTimeFormat);
    QString modificationDate = now.toString(xsdDateTimeFormat);

    // create a new COLLADA document
    DAE dae;
    daeElement *rootElement = dae.add(filename.toStdString());

	if (!rootElement)
		return false;

    // create the asset elements
    daeElement *assetElement = rootElement->add("asset");
    daeElement *contributorElement = assetElement->add("contributor");
    daeElement *authoringToolElement = contributorElement->add("authoring_tool");
    authoringToolElement->setCharData("Filmakademie Application Framework");
    daeElement *createdElement = assetElement->add("created");
    createdElement->setCharData(creationDate.toStdString().c_str());
    daeElement *modifiedElement = assetElement->add("modified");
    modifiedElement->setCharData(modificationDate.toStdString().c_str());
    daeElement *unitElement = assetElement->add("unit");
    unitElement->setAttribute("meter", "0.010000");
    daeElement *upAxisElement = assetElement->add("up_axis");
    upAxisElement->setCharData("Y_UP");

    // create the visual scenes library elements
    daeElement *scenesLibraryElement = rootElement->add("library_visual_scenes");
    daeElement *visualSceneElement = scenesLibraryElement->add("visual_scene");
    visualSceneElement->setAttribute("id", "RootNode");
    visualSceneElement->setAttribute("name", "RootNode");
    m_sceneModel->createDaeElements(visualSceneElement);

    // create the scene elements
    daeElement *sceneElement = rootElement->add("scene");
	daeElement *extraElement = sceneElement->add("extra");
	m_sceneModel->createSceneDAEProperties(extraElement);
    daeElement *instanceVisualSceneElement = sceneElement->add("instance_visual_scene");
    instanceVisualSceneElement->setAttribute("url", "#RootNode");

    // save the document under the given name
    dae.write(filename.toStdString());

    QApplication::restoreOverrideCursor();

    setCurrentFilename(filename);
    showStatus(tr("File saved"));
    return true;
}


//!
//! Sets the name of the currently edited scene file.
//!
//! \param filename The name of the currently edited scene file.
//!
void Application::setCurrentFilename ( const QString &filename )
{
    m_currentFilename = filename;
    m_sceneModel->setName(filename);
	m_sceneModel->setSceneFileName(filename);
	if( filename == DEFAULT_FILENAME )
		m_sceneModel->setWorkingDirectory("");
    setModified(false);

    if (m_mainWindow) {
        QString shownFilename;
        if (m_currentFilename.isEmpty())
            shownFilename = DEFAULT_FILENAME;
        else
            shownFilename = QFileInfo(m_currentFilename).fileName();

        m_mainWindow->setWindowTitle(tr("%1[*] - %2").arg(shownFilename).arg(m_applicationName));
    }
}


//!
//! Displays an about dialog window with the given information text.
//!
//! \param title The title to use for the about dialog.
//! \param text The text to display in the about dialog.
//!
void Application::showAbout ( const QString &title, const QString &text )
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    
    QMessageBox::about(activeWindow, title, text);
}


///
/// Protected Events
///


//!
//! Handles application close events.
//!
//! \param event The object that contains details about the event.
//!
void Application::closeEvent ( QCloseEvent *event )
{
    if (saveChangesIfNecessary(tr("Closing Application"))) {
        saveSettings();
        event->accept();
    } else
        event->ignore();
}


///
/// Private Slots
///

//!
//! Slot processes all messages sent by Ogre. Executed in Frapper thread.
//! (Frapper log messages cannot be sent across threads.)
//!
void Application::processOgreMessage ( const QString &message, const QString &level )
{
    Log::addOgreLogMessage(message, level);
}

//!
//! Sets the modified state of the scene to true.
//!
void Application::setModified ()
{
    setModified(true);
}


//!
//! Unregisters the main window from the application.
//!
//! Is called when the main window has been destroyed.
//!
void Application::unregisterMainWindow ()
{
    m_mainWindow = 0;
}


///
/// Private Slots: Application actions
///


//!
//! Shows a dialog window with information about the application.
//!
void Application::showAboutApplication ()
{
    QString title (qobject_cast<QAction *>(sender())->text().remove('&'));
    showAbout(title, m_aboutText);
}


//!
//! Shows a dialog window with information about the application framework.
//!
void Application::showAboutFramework ()
{
    QString title (qobject_cast<QAction *>(sender())->text().remove('&'));
    QString text (
        "<h3>About Frapper</h3>"
        "This application was built using Frapper version 1.1a, a C++ toolkit for node-based, plugin-powered, panel-oriented, cross-platform 3D application development.<br>"
        "<div align=\"center\">"
        "<a href=\"http://www.filmakademie.de\"><img src=\":/filmakademieLogo\"></a><br>"
        "</div>"
        "The Application Framework has been developed at Filmakademie Baden-Wuerttemberg.<br>"
        "Authors: Simon Spielmann, Michael Bussler, Nils Zweiling, Stefan Habel<br>"
        "Copyright &copy; 2014 Filmakademie Baden-Wuerttemberg. All rights reserved.<br>"
        "<br>"
        "Visit <a href=\"http://research.animationsinstitut.de\">research.animationsinstitut.de</a>, <a href=\"http://sourceforge.net/projects/frapper\">sourceforge.net/projects/frapper</a>, <br> or <a href=\"http://www.filmakademie.de\">www.filmakademie.de</a> for more information."
    );
    showAbout(title, text);
}


//!
//! Shows a dialog window with information about OGRE.
//!
void Application::showAboutOgre ()
{
    QString title (qobject_cast<QAction *>(sender())->text().remove('&'));
    QString text (
        "<h3>About OGRE</h3>"
        "OGRE is an open-source Object-oriented Graphics Rendering Engine.<br>"
        "<br>"
        "Visit <a href=\"http://www.ogre3d.org\">www.ogre3d.org</a> for more information."
    );
    showAbout(title, text);
}


//!
//! Shows a dialog window with information about Qt.
//!
void Application::showAboutQt ()
{
    aboutQt();
}


//!
//! Closes the main window, thus ending the application.
//!
void Application::quitApplication ()
{
    WindowManager::closeAdditionalWindows();

    if (m_mainWindow)
        m_mainWindow->close();
}


///
/// Private Slots: Scene actions
///


//!
//! Creates a new scene.
//!
void Application::newScene ()
{
    if (!saveChangesIfNecessary(tr("New Scene")))
        return;

    m_clearingScene = true;
    m_sceneModel->clear();
    m_clearingScene = false;

    setCurrentFilename(DEFAULT_FILENAME);
}


//!
//! Opens an existing scene.
//!
void Application::openScene ()
{
    if (!saveChangesIfNecessary(tr("Open Scene")))
        return;

    openScene(true);
}


//!
//! Reloads the scene from the current scene file.
//!
void Application::reloadScene ()
{
    if (m_modified) {
        Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
        QMessageBox::StandardButton clickedButton = QMessageBox::warning(activeWindow,
                tr("Reload Scene"),
                tr("The current scene has been modified.") + "\n" +
                tr("Do you want to discard your changes and reload the scene?"),
                QMessageBox::Discard | QMessageBox::Cancel
        );
        if (clickedButton == QMessageBox::Cancel)
            return;
    }

    openScene(true, true);
}


//!
//! Merges the current scene with an existing scene from a file.
//!
void Application::mergeScene ()
{
    if (!saveChangesIfNecessary(tr("Merge Scene")))
        return;

    openScene(false);
}


//!
//! Saves the scene under the current name.
//!
bool Application::saveScene ()
{
    if (m_currentFilename.isEmpty())
        return saveSceneAs();
    else
        return saveFile(m_currentFilename);
}


//!
//! Saves the scene under a new name.
//!
bool Application::saveSceneAs ()
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    QString filename = QFileDialog::getSaveFileName(activeWindow, tr("Save Scene File"), QDir(m_currentFilename).path(), tr("COLLADA files (*.dae)"));
    if (filename.isEmpty())
        return false;

    return saveFile(filename);
}


//!
//! Saves the layout under a new name.
//!
bool Application::saveLayoutAs ()
{
	Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
	QString filename = QFileDialog::getSaveFileName(activeWindow, tr("Save Layout File"), QDir(m_currentFilename.remove(".dae")).path(), tr("Layout files (*.lt)"));
    if (filename.isEmpty())
        return false;

	QSettings settings(filename, QSettings::IniFormat);
	settings.clear();
	m_mainWindow->setLayoutIds(m_mainWindow);
	m_mainWindow->getLayoutSettings(settings, m_mainWindow);

	return true;
}


//!
//! Saves the default layout.
//!
bool Application::saveDafaultLayout ()
{
    QSettings settings("config/default.lt", QSettings::IniFormat);
	settings.clear();
	m_mainWindow->setLayoutIds(m_mainWindow);
	m_mainWindow->getLayoutSettings(settings, m_mainWindow);

	return true;
}


//!
//! Closes the scene.
//!
void Application::closeScene ()
{
    newScene();
}


///
/// Private Slots: Edit actions
///


//!
//! Undo the action that was performed last.
//!
void Application::undo ()
{
    Log::na("Application::undo");
}


//!
//! Redo the action that was undone last.
//!
void Application::redo ()
{
    Log::na("Application::redo");
}


//!
//! Copies the selected object to the clipboard and delete it from the scene.
//!
void Application::cut ()
{
    Log::na("Application::cut");
}


//!
//! Copies the selected object to the clipboard.
//!
void Application::copy ()
{
    QWidget *focusWidget = Application::focusWidget();
    if (focusWidget) {
        QWidget *parentWidget = focusWidget->parentWidget();
        if (parentWidget) {
            CopyHandler *copyHandler = dynamic_cast<CopyHandler *>(parentWidget);
            if (copyHandler)
                copyHandler->copy();
            else
                Log::warning("Copying is currently not available for the selected object/GUI element. (Debug information: The element's parent widget is not an implementation of CopyHandler.)", "Application::copy");
        } else
            Log::warning("Copying is currently not available for the selected object/GUI element.", "Application::copy");
    }
}


//!
//! Pastes the contents of the clipboard into the current scene.
//!
void Application::paste ()
{
    Log::na("Application::paste");
}


//!
//! Duplicates the selected Item.
//!
void Application::duplicate ()
{
	if (m_sceneModel)
		m_sceneModel->duplicateNode();
}



//!
//! Opens the preferences dialog window to edit the application's options.
//!
void Application::editPreferences ()
{

}

//!
//! Opens the OGRE Configuration dialog window.
//!
void Application::editOgreConfiguration ()
{	
	if (QFile::exists("frapperogreconfig.exe")){	
		QProcess *m_Process; 
		m_Process = new QProcess( this ); 
		if (m_Process) {	
			m_Process->start("frapperogreconfig.exe"); 
			m_Process->waitForFinished();
		}
		Log::info("Note that changes to the OGRE render configuration require a restart of the Frapper application.","Application::Application");
	}
	else 
		Log::error("File not found! OGRE render configuration requires 'ogreconfigapp.exe'.", "Application::Application");
}


void Application::selectWorkingDirectory()
{
	SceneModel::selectWorkingDirectory();
}


///
/// Private Slots: Create actions
///


//!
//! Slot that is called when one of the creation actions is triggered.
//!
void Application::creationActionTriggered ()
{
    QAction *creationAction = qobject_cast<QAction *>(sender());
    if (!creationAction)
        return;

    QString typeName = creationAction->text().replace("&", "");

    m_sceneModel->createObject(typeName);
}


///
/// Private Slots: View actions
///


//!
//! Toggles the active window's status bar.
//!
void Application::toggleStatusBar ()
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    if (activeWindow)
        activeWindow->statusBar()->setVisible(!activeWindow->statusBar()->isVisible());
}


///
/// Private Slots: Window actions
///


//!
//! Opens a new window with the default layout.
//!
void Application::newWindow ()
{
    Window *newWindow = createWindow(Window::DefaultLayout);
    newWindow->show();
}


//!
//! Duplicates the currently active window including its layout.
//!
void Application::duplicateActiveWindow ()
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    if (activeWindow)
        Log::na("Application::duplicateActiveWindow");
}


//!
//! Opens a dialog window for editing the active window's title.
//!
void Application::renameActiveWindow ()
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    if (activeWindow) {
        bool result;
        QString title = QInputDialog::getText(activeWindow, tr("Rename Window"), tr("Title of this window:"), QLineEdit::Normal, activeWindow->title(), &result);
        if (result && !title.isEmpty())
            activeWindow->setTitle(title);
    }
}


//!
//! Closes the currently active window.
//!
void Application::closeActiveWindow ()
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    if (activeWindow)
        activeWindow->close();
}


//!
//! Closes all of the application's windows but its main window.
//!
void Application::closeAdditionalWindows ()
{
    WindowManager::closeAdditionalWindows();
}


//!
//! Opens a dialog box listing all of the application's currently open windows.
//!
void Application::listWindows ()
{
    Window *activeWindow = qobject_cast<Window *>(this->activeWindow());
    WindowsDialog *windowsDialog = new WindowsDialog(WindowManager::getItemModel(), activeWindow);
    windowsDialog->setModal(true);
    windowsDialog->show();
}


///
/// Private Slots: Help actions
///


//!
//! If available, highlight the documentation panel in the current window, or
//! launch a new window containing only a documentation panel.
//!
void Application::showDocumentation ()
{
    // check if there already is a window with a documentation panel
    Window *documentationWindow = WindowManager::getDocumentationWindow();
    if (documentationWindow) {
        // activate the window with the documentation panel and show the documentation main page
        QApplication::setActiveWindow(documentationWindow);
        documentationWindow->showDocumentationMainPage();
    } else {
        // create a new window with only a documentation panel
        Window *newWindow = createWindow("Documentation", false);
        newWindow->show();
    }
}
