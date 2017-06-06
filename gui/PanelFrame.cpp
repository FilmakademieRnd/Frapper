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
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       05.01.2012 (last updated)
//!

#include "PanelFrame.h"
#include "ViewportPanel.h"
#include "HierarchyPanel.h"
#include "NetworkPanel.h"
#include "ParameterPanel.h"
#include "CurveEditorPanel.h"
#include "TimelinePanel.h"
#include "HistoryPanel.h"
#include "LogPanel.h"
#include "DocumentationPanel.h"
#include "Log.h"
#include "PanelFactory.h"
#include <QSplitter>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QMainWindow>
#include <QLabel>
#include <QTreeWidget>
#include <QBoxLayout>
#include <QMenu>
#include <QToolButton>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the PanelFrame class.
//!
//! \param panelType The type of the panel to create.
//! \param parent A widget the created instance will be child of.
//! \param flags Extra widget options.
//! \param showToolBars Flag to control whether to display tool bars in the panel frame.
//!
PanelFrame::PanelFrame ( Panel::Type panelType, QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */, bool showToolBars /* = 0 */ ) :
    QFrame(parent, flags),
    m_mainToolBar(0),
    m_panelToolBar(0),
    m_panelType(Panel::T_Uninitialized),
    m_panel(0),
    m_label(0)
{
    // create gui elements defined in the Ui_Panel class
    setupUi(this);
    setFrameShape(QFrame::StyledPanel);

    // create the panel's main tool bar
    m_mainToolBar = new QToolBar(this);
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_mainToolBar->setIconSize(QSize(16, 16));
    m_mainToolBar->setMinimumHeight(22);
    m_mainToolBar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    // create the panel's main tool bar
    m_panelToolBar = new QToolBar(this);
    m_panelToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_panelToolBar->setIconSize(QSize(16, 16));
    m_panelToolBar->setMinimumHeight(22);
    m_panelToolBar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    if (!showToolBars) {
        m_mainToolBar->hide();
        m_panelToolBar->hide();
        ui_panelTypeComboBox->hide();
        //ui_vboxLayout->removeItem(ui_hboxLayout);
    }

    // insert the panel's main tool bar after the spacer behind the panel type combo box
    ui_hboxLayout->insertWidget(2, m_mainToolBar);
    ui_hboxLayout->insertWidget(3, m_panelToolBar);

    // fill panel type combo box with panel type names
    ui_panelTypeComboBox->blockSignals(true);
    for (int i = 0; i < Panel::T_NumTypes; ++i) {
        Panel::Type panelType = (Panel::Type) i;
        QString panelTypeName = Panel::getTypeName(panelType);
        QString panelTypeIconName = Panel::getTypeIconName(panelType);
        if (panelTypeIconName.isEmpty())
            ui_panelTypeComboBox->addItem(panelTypeName);
        else
            ui_panelTypeComboBox->addItem(QIcon(panelTypeIconName), panelTypeName);
    }

	// fill the panel type combo box with dynamic type names
	for(int i = 0; i < PanelFactory::m_panelTypeMap.keys().length(); ++i){
		QString PlugInName = PanelFactory::m_panelPluginNames.at(i);
		QIcon PlugInIcon = QIcon(PanelFactory::m_panelPluginIcons.at(i));
		ui_panelTypeComboBox->addItem(PlugInIcon, PlugInName);
	}

    ui_panelTypeComboBox->blockSignals(false);

    // relay the viewPanelCreated signal to the window that this panel frame is contained in
    connect(this, SIGNAL(viewPanelCreated(ViewPanel *)), window(), SIGNAL(viewPanelCreated(ViewPanel *)));

    // initialize the panel frame's panel type
    setPanelType(panelType);

    // create the panel drop-down menu
    QMenu *panelMenu = new QMenu("Panel Menu", this);
    panelMenu->addAction(ui_splitHorizontallyAction);
    panelMenu->addAction(ui_splitVerticallyAction);
    panelMenu->addSeparator();
    panelMenu->addAction(ui_maximizeAction);
    panelMenu->addAction(ui_restoreAction);
    panelMenu->addAction(ui_maximizeHorizontallyAction);
    panelMenu->addAction(ui_restoreHorizontallyAction);
    panelMenu->addAction(ui_maximizeVerticallyAction);
    panelMenu->addAction(ui_restoreVerticallyAction);
    panelMenu->addAction(ui_minimizeAction);
    panelMenu->addSeparator();
    panelMenu->addAction(ui_maximizeRowAction);
    panelMenu->addAction(ui_minimizeRowAction);
    panelMenu->addAction(ui_maximizeColumnAction);
    panelMenu->addAction(ui_minimizeColumnAction);
    panelMenu->addSeparator();
    panelMenu->addAction(ui_duplicateAction);
    panelMenu->addAction(ui_extractAction);
    panelMenu->addSeparator();
    panelMenu->addAction(ui_closeRowAction);
    panelMenu->addAction(ui_closeColumnAction);
    panelMenu->addAction(ui_closeAction);
    ui_panelMenuAction->setMenu(panelMenu);

    ui_maximizeAction->setVisible(false);
    ui_restoreAction->setVisible(false);
    ui_maximizeHorizontallyAction->setVisible(false);
    ui_restoreHorizontallyAction->setVisible(false);
    ui_maximizeVerticallyAction->setVisible(false);
    ui_restoreVerticallyAction->setVisible(false);
    ui_maximizeColumnAction->setVisible(false);
    ui_minimizeColumnAction->setVisible(false);
    ui_maximizeRowAction->setVisible(false);
    ui_minimizeRowAction->setVisible(false);
    ui_closeRowAction->setVisible(false);
    ui_closeColumnAction->setVisible(false);

    // set up external signal/slot connections
    connect(this, SIGNAL(panelTypeChanged(const QString &)), window(), SLOT(updateWindowTitle(const QString &)));
    connect(this, SIGNAL(duplicateRequested(PanelFrame *)), window(), SLOT(duplicatePanelFrame(PanelFrame *)));
    connect(this, SIGNAL(extractRequested(PanelFrame *)), window(), SLOT(extractPanelFrame(PanelFrame *)), Qt::QueuedConnection);
    connect(this, SIGNAL(closeRequested(PanelFrame *)), window(), SLOT(closePanelFrame(PanelFrame *)), Qt::QueuedConnection);
}


//!
//! Destructor of the PanelFrame class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PanelFrame::~PanelFrame ()
{
}


///
/// Public Functions
///

//!
//! Returns the the panel.
//!
//! \return The panel.
//!
::Panel * PanelFrame::getPanel ()
{
    return m_panel;
}

//!
//! Returns the type of the panel frame.
//!
//! \return The type of the panel frame.
//!
Panel::Type PanelFrame::getPanelType ()
{
    return m_panelType;
}


//!
//! Toggles the display of tool bars in the panel frame.
//!
//! \param visible Flag to control whether to display tool bars in the panel frame.
//!
void PanelFrame::toggleToolBars ( bool visible )
{
    if (visible) {
        //ui_vboxLayout->removeItem(ui_hboxLayout);
        ui_vboxLayout->insertLayout(0, ui_hboxLayout);
        ui_vboxLayout->setSpacing(1);
        ui_vboxLayout->setMargin(1);
        setFrameStyle(22);
        m_mainToolBar->show();
        m_panelToolBar->show();
        ui_panelTypeComboBox->show();
    } else {
        ui_vboxLayout->removeItem(ui_hboxLayout);
        ui_hboxLayout->setParent(0);
        m_mainToolBar->hide();
        m_panelToolBar->hide();
        ui_panelTypeComboBox->hide();
        ui_vboxLayout->setSpacing(0);
        ui_vboxLayout->setMargin(0);
        int frameSyle = frameStyle();
        setFrameStyle(0);

    }
}


///
/// Protected Event Handlers
///


//!
//! Event handler that is called when the widget receives keyboard focus.
//!
//! \param event The object containing details about the event.
//!
void PanelFrame::focusInEvent ( QFocusEvent *event )
{
    if (m_panel)
        m_panel->setFocus();

    QFrame::focusInEvent(event);
}


//!
//! Event handler that is called when the widget is resized.
//!
//! \param event The object containing details about the event.
//!
void PanelFrame::resizeEvent ( QResizeEvent *event )
{
    if (m_panel)
        m_panel->update();

    QFrame::resizeEvent(event);
}


///
/// Private Functions
///


//!
//! Sets the type of the panel frame to the given value.
//!
//! \param panelType The new type for the panel.
//!
void PanelFrame::setPanelType ( Panel::Type panelType )
{
    // check if the panel type will actually change
    if (panelType == m_panelType)
        return;

    if (m_panel) {
        // delete the existing panel contained in the panel frame's main layout
        ui_vboxLayout->removeWidget(m_panel);
        delete m_panel;
        m_panel = 0;
    }
    if (m_label) {
        // delete the existing label contained in the panel frame's main layout
        ui_vboxLayout->removeWidget(m_label);
        delete m_label;
        m_label = 0;
    }

    m_panelType = panelType;
    if (ui_panelTypeComboBox->currentIndex() != (int) m_panelType)
        ui_panelTypeComboBox->setCurrentIndex((int) m_panelType);

    switch (m_panelType) {		
		// view panels
        case Panel::T_Viewport:
            m_panel = new ViewportPanel(this);
            break;
        case Panel::T_NetworkEditor:
            m_panel = new NetworkPanel(this);
            break;
        case Panel::T_HierarchyEditor:
            m_panel = new HierarchyPanel(this);
            break;
        case Panel::T_ParameterEditor:
            m_panel = new ParameterPanel(this);
            break;
        case Panel::T_CurveEditor:
            m_panel = new CurveEditorPanel(this);
            break;
        case Panel::T_Timeline:
            m_panel = new TimelinePanel(this);
            break;

        // other panel types
        case Panel::T_History:
            m_panel = new HistoryPanel(this);
            break;
        case Panel::T_Log:
            m_panel = new LogPanel(this);
            break;
        case Panel::T_Documentation:
            m_panel = new DocumentationPanel(this);
            break;

        // unsupported panel type or dynamic panel
        default:
			if(panelType >= Panel::T_NumTypes){ 
				int pluginCount = panelType - Panel::T_NumTypes;
				if( pluginCount < PanelFactory::m_panelPluginFiles.size())
					m_panel = PanelFactory::createPanel(PanelFactory::m_panelPluginFiles.at(pluginCount), this);	
			}
			if(m_panel == 0){
				m_label = new QLabel(QString("The %1 panel is not available yet.\n\n").arg(Panel::getTypeName(m_panelType)), this);
				m_label->setAlignment(Qt::AlignCenter);
			}   
    }

    //if (m_panelType == Panel::PT_Timeline)
    //    setMaximumHeight(50);
    //else
    //    setMaximumHeight(QWIDGETSIZE_MAX);

    // empty the tool bars
    m_mainToolBar->clear();
    m_panelToolBar->clear();

    if (m_panel) {
        // check if the panel is derived from the ViewPanel class
        if (qobject_cast<ViewPanel *>(m_panel))
            // notify connected objects that a view panel has been created
            emit viewPanelCreated(qobject_cast<ViewPanel *>(m_panel));
        
        // fill the panel frame's tool bars with widget-related actions
        m_panel->fillToolBars(m_mainToolBar, m_panelToolBar);

        // add the panel to the panel frame's main layout
        ui_vboxLayout->addWidget(m_panel);
    } else
        // add the label to the panel frame's main layout
        ui_vboxLayout->addWidget(m_label);

    // create a null-sized border widget to give the panel tool bar enough space to the right
    QWidget *borderWidget = new QWidget(m_panelToolBar);
    borderWidget->setMinimumSize(QSize(0, 0));
    borderWidget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    // add a spacer widget to the panel tool bar to right-align the buttons
    QWidget *spacerWidget = new QWidget(m_panelToolBar);
    spacerWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    if (m_panelToolBar->actions().size() == 0) {
        m_panelToolBar->addWidget(spacerWidget);
        //m_panelToolBar->addSeparator();
    } else {
        QAction *firstAction = m_panelToolBar->actions().at(0);
        m_panelToolBar->insertWidget(firstAction, spacerWidget);
        //m_panelToolBar->insertSeparator(firstAction);
    }

    // add the panel menu action and the border widget
    m_panelToolBar->addAction(ui_panelMenuAction);
    m_panelToolBar->addWidget(borderWidget);

    // set the panel menu's tool button to instantly display the menu when clicked
    QToolButton *panelToolButton = qobject_cast<QToolButton *>(m_panelToolBar->widgetForAction(ui_panelMenuAction));
    if (panelToolButton)
        panelToolButton->setPopupMode(QToolButton::InstantPopup);

    // notify connected objects that the panel type has changed
    emit panelTypeChanged(Panel::getTypeName(m_panelType));
}


//!
//! Splits the panel frame according to the given orientation.
//!
//! \param orientation The orientation in which to split the panel frame.
//!
void PanelFrame::split ( Qt::Orientation orientation )
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    QSplitter *parentSplitter = qobject_cast<QSplitter *>(parentWidget());
    if (!parentSplitter && !mainWindow) {
        QMessageBox::information(this, "Split Panel", "Splitting this panel is not possible.");
        return;
    }

    // flip the orientation
    if (orientation == Qt::Horizontal)
        orientation = Qt::Vertical;
    else
        orientation = Qt::Horizontal;

    // save the size of this panel
    QSize panelSize = size();
    int widthHeight = orientation == Qt::Horizontal ? panelSize.width() : panelSize.height();
    // clone this panel frame
    PanelFrame *clonedPanelFrame = new PanelFrame(m_panelType, parentWidget(), windowFlags());

    QList<int> sizes;
    int panelIndex;
    if (parentSplitter) {
        // save the parent splitter's sizes
        sizes = parentSplitter->sizes();
        // obtain the index of this panel in its parent splitter
        panelIndex = parentSplitter->indexOf(this);
    }

    // create new splitter for this panel and its clone
    QSplitter *newSplitter = new QSplitter(orientation);
    newSplitter->addWidget(this);
    newSplitter->addWidget(clonedPanelFrame);
    // give both panels in the new splitter the same space
    QList<int> newSizes;
    newSizes << widthHeight / 2 << widthHeight / 2;
    newSplitter->setSizes(newSizes);

    if (parentSplitter) {
        // insert the new splitter into the panel's former parent splitter
        parentSplitter->insertWidget(panelIndex, newSplitter);
        // restore the parent splitter's sizes
        parentSplitter->setSizes(sizes);
    } else {
        mainWindow->setCentralWidget(newSplitter);
    }
}


//!
//! Maximizes the given widget inside its parent widget.
//!
//! The widget can be a panel frame or a splitter widget.
//!
//! \param widget The widget to maximize.
//!
void PanelFrame::maximizeWidget ( QWidget *widget )
{
    QWidget *parentWidget = widget->parentWidget();
    QSplitter *parentSplitter = qobject_cast<QSplitter *>(parentWidget);
    if (!parentSplitter) {
        QMessageBox::information(this, "Maximize Panel", "The panel is already maximized.");
        return;
    }

    int index = parentSplitter->indexOf(widget);
    QList<int> sizes = parentSplitter->sizes();

    // count number of visible panel frames
    int visiblePanelFrames = 0;
    for (int i = 0; i < sizes.size(); ++i)
        if (sizes[i] > 0)
            ++visiblePanelFrames;

    if (visiblePanelFrames == 1)
        maximizeWidget(parentSplitter);
    else {
        // hide all other panels in the same splitter
        for (int i = 0; i < sizes.size(); ++i)
            if (i != index)
                sizes[i] = 0;
        parentSplitter->setSizes(sizes);
    }
}


//!
//! Minimizes the given widget inside its parent widget.
//!
//! The widget can be a panel frame or a splitter widget.
//!
//! \param widget The widget to minimize.
//!
void PanelFrame::minimizeWidget ( QWidget *widget )
{
    QWidget *parentWidget = widget->parentWidget();
    QSplitter *parentSplitter = qobject_cast<QSplitter *>(parentWidget);
    if (!parentSplitter) {
        QMessageBox::information(this, "Minimize Panel", "The last visible panel cannot be minimized.");
        return;
    }

    int index = parentSplitter->indexOf(widget);
    QList<int> sizes = parentSplitter->sizes();

    // count number of visible panel frames
    int visiblePanelFrames = 0;
    for (int i = 0; i < sizes.size(); ++i)
        if (sizes[i] > 0)
            ++visiblePanelFrames;

    if (visiblePanelFrames == 1)
        minimizeWidget(parentSplitter);
    else {
        sizes[index] = 0;
        parentSplitter->setSizes(sizes);
    }
}


///
/// Private Slots
///


//!
//! Fills the panel frame with a panel according to the given panel type
//! index.
//!
//! \param index The new panel type index.
//!
void PanelFrame::on_ui_panelTypeComboBox_currentIndexChanged ( int index )
{
   // if (index > Panel::T_Uninitialized && index < Panel::T_NumTypes)
	if (index > Panel::T_Uninitialized)
        setPanelType((Panel::Type) index);
}


//!
//! Splits the panel horizontally.
//!
//! Convenience function that calls splitPanel(Qt::Horizontal).
//!
void PanelFrame::on_ui_splitHorizontallyAction_triggered ()
{
    split(Qt::Horizontal);
}


//!
//! Splits the panel vertically.
//!
//! Convenience function that calls splitPanel(Qt::Vertical).
//!
void PanelFrame::on_ui_splitVerticallyAction_triggered ()
{
    split(Qt::Vertical);
}


//!
//! Maximizes the panel.
//!
void PanelFrame::on_ui_maximizeAction_triggered ()
{
    maximizeWidget(this);
    ui_maximizeAction->setVisible(false);
    ui_restoreAction->setVisible(true);
}


//!
//! Maximizes the panel in its row.
//!
void PanelFrame::on_ui_maximizeHorizontallyAction_triggered ()
{
    maximizeWidget(this);
    ui_maximizeHorizontallyAction->setVisible(false);
    ui_restoreHorizontallyAction->setVisible(true);
}


//!
//! Maximizes the panel in its column.
//!
void PanelFrame::on_ui_maximizeVerticallyAction_triggered ()
{
    maximizeWidget(this);
    ui_maximizeVerticallyAction->setVisible(false);
    ui_restoreVerticallyAction->setVisible(true);
}


//!
//! Minimizes the panel frame.
//!
void PanelFrame::on_ui_minimizeAction_triggered ()
{
    minimizeWidget(this);
}


//!
//! Duplicates the panel frame by creating a new window.
//!
//! Does not duplicate the panel itself, but notifies connected objects
//! that the panel should be duplicated.
//!
void PanelFrame::on_ui_duplicateAction_triggered ()
{
    emit duplicateRequested(this);
}


//!
//! Extracts the panel frame to a new window.
//!
//! Does not extract the panel frame itself, but notifies connected objects
//! that the panel frame should be extracted.
//!
void PanelFrame::on_ui_extractAction_triggered ()
{
    emit extractRequested(this);
}


//!
//! Closes the panel.
//!
//! Does not close the panel itself, but notifies connected objects that
//! the panel should be closed.
//!
void PanelFrame::on_ui_closeAction_triggered ()
{
    emit closeRequested(this);
}

} // end namespace Frapper