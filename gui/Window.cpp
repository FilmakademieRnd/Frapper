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
//! \file "Window.cpp"
//! \brief Implementation file for Window class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       08.06.2009 (last updated)
//!

#include "Window.h"
#include "ViewPanel.h"
#include "Log.h"
#include <QSplitter>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QtCore/QDebug>
#include <QtCore/QByteArray>
#include <iostream>


namespace Frapper {

///
/// Static Constants
///


//!
//! The name of the default window layout.
//!
const QString Window::DefaultLayout = "Default";

//!
//! The name of the window layout with only a single view.
//!
const QString Window::SingleViewLayout = "Single View";

//!
//! The name of the window layout with a fullscreen view.
//!
const QString Window::FullscreenViewLayout = "Fullscreen View";



///
/// Static Variables
///

//!
//! The ID of the splitter widgets
//!
unsigned int splitterID = 0;

//!
//! The ID of the panelFrame widgets
//!
unsigned int panelFrameID = 0;

///
/// Constructors and Destructors
///


//!
//! Constructor of the Window class.
//!
//! Creates a new application window using the layout with the given name.
//!
//! \param parent The parent object (should be the Application object).
//! \param applicationName The name of the application that the window will be a part of.
//! \param title The title to give to the new window.
//! \param layoutName The name of the layout to apply to the window.
//! \param isMainWindow Flag that states whether the window to create is the application's main window.
//!
Window::Window ( QObject *parent, const QString &applicationName, const QString &title, const QString &layoutName, bool isMainWindow /* = false */ ) :
    QMainWindow(),
    m_applicationName(applicationName),
    m_title(title),
    m_isMainWindow(isMainWindow)
{
    setupUi(this);
    setWindowIcon(QIcon(":/applicationIcon"));

    if (m_isMainWindow)
        setWindowTitle(m_applicationName);
    else
        setTitle(title);

    // set up external signal/slot connections
    connect(this, SIGNAL(windowActivated(Window *)), parent, SLOT(handleWindowActivation(Window *)));
    connect(this, SIGNAL(windowRenamed(Window *)), parent, SLOT(handleWindowRenaming(Window *)));
    connect(this, SIGNAL(windowClosed(Window *, QCloseEvent *)), parent, SLOT(handleWindowClosing(Window *, QCloseEvent *)));
    connect(this, SIGNAL(createWindowRequested(const QString &, const QPoint &, const QSize &)), parent, SLOT(createWindow(const QString &, const QPoint &, const QSize &)));
    connect(this, SIGNAL(viewPanelCreated(ViewPanel *)), parent, SLOT(handleViewPanelCreation(ViewPanel *)));


	Panel::Type panelType = Panel::T_Uninitialized;

	// check if dynamic panel and decode panel type from type name
	if( layoutName.startsWith("_pt_")) {

		int start = 4;
		int end = layoutName.indexOf( '_', start);
		if( end > start ) {
			bool ok = false;
			int type = layoutName.mid(start, end-start).toInt(&ok);
			if(ok) {
				panelType = (Panel::Type) type;
			}
		}
	} else {
		// check if the given layout name is the name of a panel
		int i = 0;
		while (panelType == Panel::T_Uninitialized && i < Panel::T_NumTypes)
			if (layoutName == Panel::getTypeName((Panel::Type) i))
				panelType = (Panel::Type) i;
			else
				++i;
	}

    // check if a single panel should be displayed in the window
    if (panelType != Panel::T_Uninitialized)
        applyLayout(Window::SingleViewLayout, panelType);
    else
		applyLayout(layoutName);
		
    setAttribute(Qt::WA_DeleteOnClose);
}


//!
//! Destructor of the Window class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
Window::~Window ()
{
    Log::debug(QString("Window \"%1\" destroyed.").arg(m_title), "Window::~Window");
}


///
/// Public Functions
///


//!
//! Returns the window layout state
//!
//! \param settings The window layout settings state reference.
//! \param rootChild The root widget.
//!
void Window::getLayoutSettings (QSettings &settings, QObject *rootChild) const
{
	QSplitter *splitter;
	PanelFrame *panelFrame;
	const QObjectList &children = rootChild->children();

	foreach (QObject *child, children) {
		if (splitter = dynamic_cast<QSplitter*>(child)) {
			settings.beginGroup(splitter->objectName());
			settings.setValue("type", "Splitter");
			settings.setValue("states", splitter->saveState());
			settings.setValue("left", splitter->widget(0)->objectName());
			settings.setValue("right", splitter->widget(1)->objectName());
			settings.endGroup();
			getLayoutSettings(settings, child);
		}
		else if (panelFrame = dynamic_cast<PanelFrame*>(child)) {
            settings.beginGroup(panelFrame->objectName());
			settings.setValue("type", "Panel");
			settings.setValue("ptype", panelFrame->getPanelType());
            Panel *panel = panelFrame->getPanel();
            if (panel) {
                const QMap<QString, QString> &parameterMap = panel->getPanelParameters();
                QMap<QString, QString>::const_iterator iter;
                for (iter = parameterMap.begin(); iter != parameterMap.end(); iter++) {
                    settings.setValue("/parameters/" + iter.key(), iter.value());
                }
            }
            settings.endGroup();
		}
	}
}


//!
//! Sets the window layout states
//!
//! \param The window layout settings state reference.
//! \param rootChild The root widget.
//!
QWidget * Window::setLayoutSettings (QSettings &settings, QString index)
{
	const QStringList &groups = settings.childGroups();
	
	if (!groups.contains(index))
		return 0;
	
    settings.beginGroup(index);
	QVariant typeVariant = settings.value("type");
	QVariant statesVariant = settings.value("states");
	QVariant leftVariant = settings.value("left");
	QVariant rightVariant = settings.value("right");
	QVariant ptypeVariant = settings.value("ptype");
    settings.beginGroup("parameters");
    const QStringList &parameterKeys  = settings.childKeys();
    QMap<QString, QString> parameterMap;
    for (int i = 0; i < parameterKeys.size(); ++i) {
        parameterMap[parameterKeys[i]] = settings.value(parameterKeys[i]).toString();
        std::cout << parameterMap[parameterKeys[i]].toStdString() << std::endl;
    }
    settings.endGroup();
	settings.endGroup();

	if (typeVariant.toString() == "Splitter") {
		QSplitter *splitter = new QSplitter(this);
		splitter->addWidget(setLayoutSettings(settings, leftVariant.toString()));
		splitter->addWidget(setLayoutSettings(settings, rightVariant.toString()));
		splitter->restoreState(statesVariant.toByteArray());
		return splitter;
	}
	else if (typeVariant.toString() == "Panel") {
		PanelFrame *panelFrame = new PanelFrame(static_cast<Panel::Type>(ptypeVariant.toString().toUInt()), this);
        if (panelFrame) {
            Panel *panel = panelFrame->getPanel();
            if (panel)
                panel->setPanelParameters(parameterMap);
        }
		return panelFrame;
	}
	return 0;
}

//!
//! Apply the viewport layout on the window
//!
//! \param The window reference - It is taken from the QObject.
//! \param The settings to be applied to the Viewport.
//!
void Window::applyViewportLayout(QObject *rootChild, QSettings &settings, QString index)
{
	const QObjectList &children = rootChild->children();
	PanelFrame *panelFrame;
	static unsigned int counter = 0;

	foreach (QObject *child, children) {

		if (panelFrame = dynamic_cast<PanelFrame*>(child)) {
			panelFrame->setObjectName(QString::number(counter++));
			break;
		}
	}	
	setViewportLayoutSettings(settings, panelFrame, index);
}

//!
//! Sets the Viewport layout states
//!
//! \param The settings of the Viewport.
//! \param The panelFrame where the settings will be set.
//!
void Window::setViewportLayoutSettings(QSettings &settings, PanelFrame *panelFrame, QString index)
{
	const QStringList &groups = settings.childGroups();

	if (!groups.contains(index))
		return;

	settings.beginGroup(index);
	QVariant typeVariant = settings.value("type");
	QVariant leftVariant = settings.value("left");
	QVariant rightVariant = settings.value("right");
	settings.beginGroup("parameters");
	const QStringList &parameterKeys  = settings.childKeys();
	QMap<QString, QString> parameterMap;
	for (int i = 0; i < parameterKeys.size(); ++i) {
		parameterMap[parameterKeys[i]] = settings.value(parameterKeys[i]).toString();
	}
	settings.endGroup();
	settings.endGroup();

	if (typeVariant.toString() == "Splitter") {
		setViewportLayoutSettings(settings, panelFrame, leftVariant.toString());
		setViewportLayoutSettings(settings, panelFrame, rightVariant.toString());
		return;
	}
	else if (typeVariant.toString() == "Panel") {
		if (panelFrame) {
			Panel *panel = panelFrame->getPanel();
			if (panel)
				panel->setPanelParameters(parameterMap);
		}
		return;
	}
	return;
	
}

//!
//! Insert unique IDs to the splitter and panel widget names
//!
void Window::setLayoutIds (QObject *rootChild)
{
	QSplitter *splitter;
	PanelFrame *panelFrame;
	static unsigned int counter = 0;
	const QObjectList &children = rootChild->children();

	foreach (QObject *child, children) {
		if (splitter = dynamic_cast<QSplitter*>(child)) {
			splitter->setObjectName(QString::number(counter++));
			setLayoutIds(child);
		}
		else if (panelFrame = dynamic_cast<PanelFrame*>(child)) {
			panelFrame->setObjectName(QString::number(counter++));
		}
	}
}


//!
//! Returns whether the window is the application's main window.
//!
//! \return True if the window is the application's main window, otherwise False.
//!
bool Window::isMainWindow ()
{
    return m_isMainWindow;
}


//!
//! Returns whether the window contains a documentation panel.
//!
//! \return True if the window contains a documentation panel, otherwise False.
//!
bool Window::hasDocumentation ()
{
    QList<PanelFrame *> panelFrames = findChildren<PanelFrame *>();
    for (int i = 0; i < panelFrames.size(); ++i)
        if (panelFrames.at(i)->getPanelType() == Panel::T_Documentation)
            return true;

    return false;
}


//!
//! Opens the documentation main page in the first documentation panel found in
//! the window's layout.
//!
void Window::showDocumentationMainPage ()
{
    // TODO: open the documentation main page in the first documentation panel found in the window's layout
    if (hasDocumentation()) {
        QList<PanelFrame *> panelFrames = findChildren<PanelFrame *>();
        for (int i = 0; i < panelFrames.size(); ++i)
            if (panelFrames.at(i)->getPanelType() == Panel::T_Documentation)
                panelFrames.at(i)->setFocus();
    }
}


//!
//! Returns the window's title.
//!
//! \return The window's title.
//!
QString Window::title ()
{
    return m_title;
}


//!
//! Sets the window's title.
//!
//! \param title The title to set for the window.
//!
void Window::setTitle ( const QString &title )
{
    m_title = title;
    setWindowTitle(QString("%1 - %2").arg(m_title).arg(m_applicationName));

    // notify connected objects that the window has been renamed
    emit windowRenamed(this);
}


//!
//! Set fullscreen.
//!
void Window::setFullscreen ( bool fullscreen )
{

    if (fullscreen) {
        Qt::WindowFlags flags = Qt::Window;
        flags |= Qt::FramelessWindowHint;
        setWindowFlags(flags);
        QDesktopWidget *desktopWidget = QApplication::desktop();
        const int screenId = desktopWidget->screenNumber(this);
        const QRect widgetRect = desktopWidget->availableGeometry(screenId);
        widgetRect.x();
        widgetRect.y();
        int posX = widgetRect.x()-1;
        int posY = widgetRect.y()-1;
        int width = widgetRect.width()+2;
        int height = widgetRect.height()+2;
        move(posX, posY);
        resize(QSize(width, height));
        QWidget::show();
        menubar->hide();
        statusbar->hide();
    } else {
        Qt::WindowFlags flags = Qt::Window;
        setWindowFlags(flags);
        QDesktopWidget *desktopWidget = QApplication::desktop();
        const int screenId = desktopWidget->screenNumber(this);
        const QRect widgetRect = desktopWidget->availableGeometry(screenId);
        widgetRect.x();
        widgetRect.y();
        int posX = widgetRect.x();
        int posY = widgetRect.y();
        move(20, 20);
        resize(QSize(800, 600));
        QWidget::show();
        menubar->show();
        statusbar->show();
    }
}

//!
//! Deletes all layout widgets on contral widget
//!
void Window::clearLayout ()
{
	if (centralWidget())
        centralWidget()->deleteLater();
}


///
/// Public Slots
///


//!
//! Updates the window title according to the given panel type name if
//! there is only one panel in the window.
//!
//! \param panelTypeName The name of the new panel type.
//!
void Window::updateWindowTitle ( const QString &panelTypeName )
{
    QList<Panel *> panels = findChildren<Panel *>();
    if (panels.size() == 1)
        setTitle(panelTypeName);
}


//!
//! Duplicates the given panel by creating a new window.
//!
//! \param panelFrame The panel frame to duplicate.
//!
void Window::duplicatePanelFrame ( PanelFrame *panelFrame )
{
    QPoint position (panelFrame->x(), panelFrame->y());
    QWidget *parentWidget = panelFrame->parentWidget();
    while (parentWidget) {
        position += parentWidget->pos();
        parentWidget = parentWidget->parentWidget();
    }

	Panel::Type panelType = panelFrame->getPanelType();
	
	// check if dynamic plugin panel
	if( panelType >= Panel::T_NumTypes) {
		ViewPanel* viewPanel = dynamic_cast<ViewPanel*>(panelFrame->getPanel());
		if( viewPanel ){
			// string-encoded panel type number within type name
			emit createWindowRequested( "_pt_" + QString::number(panelType) + "_" + viewPanel->getPanelTypeName(), position, panelFrame->size());
		}
	} else {
		emit createWindowRequested(Panel::getTypeName(panelType), position, panelFrame->size());
	}
}


//!
//! Extracts the given panel frame to a new window.
//!
//! \param panelFrame The panel frame to extract to a new window.
//!
void Window::extractPanelFrame ( PanelFrame *panelFrame )
{
    // check if the parent widget is a main window
    if (panelFrame->parentWidget()->isWindow()) {
        QMessageBox::information(this, tr("Tear Off Panel"), tr("This is the only panel in the window, so it cannot be torn off."));
        return;
    }

    duplicatePanelFrame(panelFrame);
    closePanelFrame(panelFrame);
}


//!
//! Closes and deletes the given panel frame.
//!
//! \param panelFrame The panel frame to close.
//!
void Window::closePanelFrame ( PanelFrame *panelFrame )
{
    // check if the parent widget is a main window
    if (panelFrame->parentWidget()->isWindow()) {
        QMessageBox::information(this, tr("Close Panel"), tr("This is the only panel in the window, so it cannot be closed."));
        return;
    }

    // check if the parent widget is a splitter (which it should be at this point)
    QSplitter *parentSplitter = qobject_cast<QSplitter *>(panelFrame->parentWidget());
    if (parentSplitter) {
        // get a pointer to the other widget in the splitter
        int otherWidgetIndex = parentSplitter->indexOf(panelFrame) == 0;
        QWidget *otherWidget = parentSplitter->widget(otherWidgetIndex);
        // replace parent splitter in its parent splitter with the other widget
        QSplitter *parentParentSplitter = qobject_cast<QSplitter *>(parentSplitter->parentWidget());
        if (parentParentSplitter) {
            // save parent parent splitter's sizes
            QList<int> sizes = parentParentSplitter->sizes();
            // obtain index of parent splitter in its parent splitter
            int index = parentParentSplitter->indexOf(parentSplitter);
            // insert other widget instead of the parent splitter
            parentParentSplitter->insertWidget(index, otherWidget);
            parentParentSplitter->insertWidget(parentParentSplitter->count(), parentSplitter);
            // restore parent parent splitter's sizes
            parentParentSplitter->setSizes(sizes);
        } else {
            // check if the parent widget is a main window
            QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentSplitter->parentWidget());
            if (mainWindow) {
                // reparent the other widget to the main window
                // (to prevent it from being deleted when the main window's central widget is reset)
                otherWidget->setParent(mainWindow);
                // set the other widget as the main window's central widget
                mainWindow->setCentralWidget(otherWidget);

                parentSplitter = 0;
            }
        }
        if (parentSplitter)
            delete parentSplitter;
    } else
        delete panelFrame;
}


///
/// Protected Events
///


//!
//! Event handler for context menu events.
//!
//! \param event The object that contains details about the event.
//!
void Window::contextMenuEvent ( QContextMenuEvent *event )
{
    // check if the context menu is requested for a network panel
    QObject *object = qobject_cast<QObject *>(childAt(event->pos()));
    if (object) {
        QObject *parent = object->parent();
        if (parent && parent->objectName() == "NetworkGraphicsView")
            // notify connected objects (the application object) that the node context menu should be displayed at the event's position
            emit nodeContextMenuRequested(event->globalPos());
    }
}


//!
//! Event handler for widget state change events.
//!
//! \param event The object that contains details about the event.
//!
void Window::changeEvent ( QEvent *event )
{
    if (event->type() == QEvent::ActivationChange) {
        if (isActiveWindow() && this == QApplication::activeWindow())
            emit windowActivated(this);
    }

    QWidget::changeEvent(event);
}


//!
//! Event handler for widget close events.
//!
//! \param event The object that contains details about the event.
//!
void Window::closeEvent ( QCloseEvent *event )
{
    // notify connected objects that the window is to be closed
    emit windowClosed(this, event);
}


///
/// Private Functions
///


//!
//! Applies the layout with the given name to the window.
//! The given panel type is used only in specific layouts.
//!
//! \param layoutName The name of the layout to apply to the window.
//! \param panelType A panel type that is used only in specific layout.
//!
void Window::applyLayout ( const QString &layoutName, Panel::Type panelType /* = ViewportPanelType */ )
{
    // delete existing panel layout
    clearLayout();

    if (layoutName == Window::DefaultLayout) {
		QSettings settings("config/default.lt", QSettings::IniFormat);
		QWidget *newLayout = setLayoutSettings(settings);
		if (newLayout != 0) 
			setCentralWidget(newLayout);
		else {
			// create default panels
			PanelFrame *viewportPanelFrame = new PanelFrame(Panel::T_Viewport, this);
			PanelFrame *networkPanelFrame = new PanelFrame(Panel::T_NetworkEditor, this);
			PanelFrame *hierarchyPanelFrame = new PanelFrame(Panel::T_HierarchyEditor, this);
			PanelFrame *parameterPanelFrame = new PanelFrame(Panel::T_ParameterEditor, this);
			PanelFrame *timelinePanelFrame = new PanelFrame(Panel::T_Timeline, this);
			PanelFrame *logPanelFrame = new PanelFrame(Panel::T_Log, this);
			PanelFrame *historyPanelFrame = new PanelFrame(Panel::T_History, this);
	        
			QSplitter *hierarchyViewportSplitter = new QSplitter(Qt::Horizontal, this);
			hierarchyViewportSplitter->addWidget(hierarchyPanelFrame);
			hierarchyViewportSplitter->addWidget(viewportPanelFrame);
			hierarchyViewportSplitter->setStretchFactor(0, 1);
			hierarchyViewportSplitter->setStretchFactor(1, 3);

			QSplitter *networkParameterSplitter = new QSplitter(Qt::Horizontal, this);
			networkParameterSplitter->addWidget(networkPanelFrame);
			networkParameterSplitter->addWidget(parameterPanelFrame);
			networkParameterSplitter->setStretchFactor(0, 3);
			networkParameterSplitter->setStretchFactor(1, 2);

			QSplitter *logHistorySplitter = new QSplitter(Qt::Horizontal, this);
			logHistorySplitter->addWidget(logPanelFrame);
			logHistorySplitter->addWidget(historyPanelFrame);
			logHistorySplitter->setStretchFactor(0, 2);
			logHistorySplitter->setStretchFactor(1, 1);

			QSplitter *networkParameterLogHistorySplitter = new QSplitter(Qt::Vertical, this);
			networkParameterLogHistorySplitter->addWidget(networkParameterSplitter);
			networkParameterLogHistorySplitter->addWidget(logHistorySplitter);
			networkParameterLogHistorySplitter->setStretchFactor(0, 2);
			networkParameterLogHistorySplitter->setStretchFactor(1, 1);

			QSplitter *networkParameterLogHistoryTimelineSplitter = new QSplitter(Qt::Vertical, this);
			networkParameterLogHistoryTimelineSplitter->addWidget(networkParameterLogHistorySplitter);
			networkParameterLogHistoryTimelineSplitter->addWidget(timelinePanelFrame);
			networkParameterLogHistoryTimelineSplitter->setStretchFactor(0, 10);
			networkParameterLogHistoryTimelineSplitter->setStretchFactor(1, 1);

			QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
			mainSplitter->addWidget(hierarchyViewportSplitter);
			mainSplitter->addWidget(networkParameterLogHistoryTimelineSplitter);
			mainSplitter->setStretchFactor(0, 2);
			mainSplitter->setStretchFactor(1, 1);

			setCentralWidget(mainSplitter);
		}
	} else if (layoutName == Window::SingleViewLayout) {
        // create single panel frame of the given type
        PanelFrame *singlePanelFrame = new PanelFrame(panelType, this);
        //setWindowFlags(Qt::FramelessWindowHint);
        setCentralWidget(singlePanelFrame);

		if( panelType >= Panel::T_NumTypes) {
			ViewPanel* viewPanel = dynamic_cast<ViewPanel*>(singlePanelFrame->getPanel());
			if( viewPanel ){
				setTitle( viewPanel->getPanelTypeName() );
			}
		} else {
			setTitle(Panel::getTypeName(panelType));
		}
    }
}


///
/// Private Slots
///


//!
//! Apply the default layout on the window.
//!
void Window::applyDefaultLayout ()
{
	applyLayout(Window::DefaultLayout);
}


//!
//! Apply the viewport layout on the window, which means that only a viewport
//! panel is displayed.
//!
void Window::applyViewportOnlyLayout ()
{
    applyLayout(Window::SingleViewLayout, Panel::T_Viewport);
}

} // end namespace Frapper