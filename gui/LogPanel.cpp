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
//! \file "LogPanel.h"
//! \brief Implementation file for LogPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "LogPanel.h"
#include "Log.h"
#include <QtCore/QTime>
#include <QMenu>
#include <QClipboard>
#include <QKeyEvent>
#include <QLabel>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the LogPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
LogPanel::LogPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    Panel(parent, flags),
    m_numActiveFilters(0),
	m_searchEdit(new QLineEdit(this)),
    m_searchTimer(0)
{
    setupUi(this);

    // create filter menu
    QMenu *filterMenu = new QMenu("Filter Messages", this);
    QAction *separatorAction = filterMenu->addSeparator();
    filterMenu->removeAction(separatorAction);
    filterMenu->addActions(QList<QAction *>()
        << ui_infoAction
        << ui_warningAction
        << ui_errorAction
        << ui_debugAction
        << separatorAction
        << ui_qtAction
        << ui_ogreAction
    );
    filterMenu->addSeparator();
    filterMenu->addAction(ui_resetFilterAction);

    ui_filterAction->setMenu(filterMenu);
    ui_filterAction->setIcon(QIcon(":/resetFilterIcon"));

    ui_resetFilterAction->setEnabled(m_numActiveFilters > 0);

	// initialize the search edit widget
    m_searchEdit->setToolTip(tr("Log Search"));
    m_searchEdit->setStatusTip(tr("Enter a name or part of a name of log message to display"));
	ui_resetSearchAction->setEnabled(true);
	
	connect(m_searchEdit, SIGNAL(textChanged(const QString &)), SLOT(m_searchEdit_textChanged(const QString &)));

	Log::registerView(ui_tableView);
}


//!
//! Destructor of the LogPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
LogPanel::~LogPanel ()
{
    Log::unregisterView(ui_tableView);
}


///
/// Public Functions
///


//!
//! Fills the given tool bars with actions for the log panel.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void LogPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
    mainToolBar->addAction(ui_filterAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(ui_copyAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(ui_clearAction);

	// create the search icon label
    QLabel *searchIconLabel = new QLabel(this);
    searchIconLabel->setFixedWidth(20);
    searchIconLabel->setStyleSheet(
        "background-image: url(:/searchIcon);"
        "background-repeat: no-repeat;"
        "background-origin: content;"
        "background-position: left center;"
        "margin-right: 2px;"
    );
    searchIconLabel->setToolTip(tr("Log Search"));

    // add the parameter search controls to the panel tool bar
    panelToolBar->addWidget(searchIconLabel);
    panelToolBar->addWidget(m_searchEdit);
    panelToolBar->addAction(ui_resetSearchAction);
	panelToolBar->addSeparator();
}


//!
//! Copies the currently selected (or all) log messages to the clipboard.
//!
void LogPanel::copy ()
{
    ui_copyAction->trigger();
}

///
/// Protected Events
///


//!
//! Event handler that reacts to timer events.
//!
//! \param event The description of the timer event.
//!
void LogPanel::timerEvent ( QTimerEvent *event )
{
    if (event->timerId() == m_searchTimer) {
        killTimer(m_searchTimer);
        m_searchTimer = 0;

		Log::filter(ui_tableView, m_searchEdit->text());
    }
}

///
/// Private Slots
///

//!
//! Starts the timer that launches the log filtering search after a
//! specific delay.
//!
//! Is called when the text in the search edit widget has changed.
//!
//! \param text The text that has been entered in the search edit widget.
//!
void LogPanel::m_searchEdit_textChanged ( const QString &text )
{
    // stop a previously started search timer
    if (m_searchTimer != 0)
        killTimer(m_searchTimer);

    m_searchTimer = startTimer(1000);

    ui_resetSearchAction->setEnabled(!text.isEmpty());
}

//!
//! Toggles filtering of log messages in the log panel.
//!
//! \param enabled Flag that controls whether to enable or disable filtering.
//!
void LogPanel::on_ui_filterAction_toggled ( bool enabled )
{
    if (enabled && m_numActiveFilters == 0)
        ui_infoAction->setChecked(true);
    else
        Log::setFilterEnabled(ui_tableView, enabled);
}


//!
//! Toggles the display of informational messages in the log panel.
//!
//! \param checked Flag that controls whether to show or hide the messages.
//!
void LogPanel::on_ui_infoAction_toggled ( bool checked )
{
    Log::filter(ui_tableView, Log::MT_Info, checked);
    updateActions(checked);
}


//!
//! Cancels a running log search and resets the parameter search
//! settings for the currently edited node.
//!
//! Is called when the reset search action has been triggered.
//!
//! \param checked The state of the action (unused).
//!
void LogPanel::on_ui_resetSearchAction_triggered ( bool /* checked = false */ )
{
    // stop the search timer if it is currently running
    if (m_searchTimer != 0) {
        killTimer(m_searchTimer);
        m_searchTimer = 0;
    }

    // reset the log  search widgets
    m_searchEdit->setText("");
}

//!
//! Toggles the display of warning messages in the log panel.
//!
//! \param checked Flag that controls whether to show or hide the messages.
//!
void LogPanel::on_ui_warningAction_toggled ( bool checked )
{
    Log::filter(ui_tableView, Log::MT_Warning, checked);
    updateActions(checked);
}


//!
//! Toggles the display of error messages in the log panel.
//!
//! \param checked Flag that controls whether to show or hide the messages.
//!
void LogPanel::on_ui_errorAction_toggled ( bool checked )
{
    Log::filter(ui_tableView, Log::MT_Error, checked);
    updateActions(checked);
}


//!
//! Toggles the display of debug messages in the log panel.
//!
//! \param checked Flag that controls whether to show or hide the messages.
//!
void LogPanel::on_ui_debugAction_toggled ( bool checked )
{
    Log::filter(ui_tableView, Log::MT_Debug, checked);
    updateActions(checked);
}


//!
//! Toggles the display of Qt messages in the log panel.
//!
//! \param checked Flag that controls whether to show or hide the messages.
//!
void LogPanel::on_ui_qtAction_toggled ( bool checked )
{
    Log::filter(ui_tableView, Log::MT_Qt, checked);
    updateActions(checked);
}


//!
//! Toggles the display of OGRE messages in the log panel.
//!
//! \param checked Flag that controls whether to show or hide the messages.
//!
void LogPanel::on_ui_ogreAction_toggled ( bool checked )
{
    Log::filter(ui_tableView, Log::MT_Ogre, checked);
    updateActions(checked);
}


//!
//! Resets the log panel's filter settings.
//!
void LogPanel::on_ui_resetFilterAction_triggered ()
{
    ui_infoAction->setChecked(false);
    ui_warningAction->setChecked(false);
    ui_errorAction->setChecked(false);
    ui_debugAction->setChecked(false);
    ui_qtAction->setChecked(false);
    ui_ogreAction->setChecked(false);
}


//!
//! Copies the text of the currently selected log messages to the clipboard.
//!
void LogPanel::on_ui_copyAction_triggered ()
{
    // get the text of the currently selected log messages by iterating over all rows in the item model
    QString messages;
    QStandardItemModel *itemModel = 0;
    QSortFilterProxyModel *filterModel = qobject_cast<QSortFilterProxyModel *>(ui_tableView->model());   
    if (filterModel)
        itemModel = qobject_cast<QStandardItemModel *>(filterModel->sourceModel());
    if (!itemModel)
        return;

    QItemSelectionModel *selectionModel = ui_tableView->selectionModel();
    if (selectionModel->selectedIndexes().size() > 0) {
        // copy only the selected messages to the clipboard
        for (int row = 0; row < itemModel->rowCount(); ++row)
            if (selectionModel->isSelected(filterModel->mapFromSource(itemModel->index(row, 0))))
                messages += Log::getMessage(itemModel, row);
    } 

    // copy the messages to the clipboard
    QApplication::clipboard()->setText(messages);
}


//!
//! Clears the log message history.
//!
void LogPanel::on_ui_clearAction_triggered ()
{
    Log::clear();
}


///
/// Private Functions
///

//!
//! Keeps track of the number of active message filters and enables the Filter
//! and Reset Filter actions accordingly.
//!
//! \param activated Flag that states whether a message filter has been activated or deactivated.
//!
void LogPanel::updateActions ( bool activated )
{
    // keep track of the number of active message filters
    if (activated)
        ++m_numActiveFilters;
    else
        --m_numActiveFilters;

    // enable or disable message filtering
    ui_filterAction->blockSignals(true);
    if (m_numActiveFilters == 0 && ui_filterAction->isChecked())
        ui_filterAction->setChecked(false);
    else if (m_numActiveFilters > 0 && !ui_filterAction->isChecked())
        ui_filterAction->setChecked(true);
    ui_filterAction->blockSignals(false);

    // set icon of filter action
    if (m_numActiveFilters == 0)
        ui_filterAction->setIcon(QIcon(":/resetFilterIcon"));
    else if (m_numActiveFilters == 1) {
        if (ui_infoAction->isChecked())
            ui_filterAction->setIcon(ui_infoAction->icon());
        else if (ui_warningAction->isChecked())
            ui_filterAction->setIcon(ui_warningAction->icon());
        else if (ui_errorAction->isChecked())
            ui_filterAction->setIcon(ui_errorAction->icon());
        else if (ui_debugAction->isChecked())
            ui_filterAction->setIcon(ui_debugAction->icon());
        else if (ui_qtAction->isChecked())
            ui_filterAction->setIcon(ui_qtAction->icon());
        else if (ui_ogreAction->isChecked())
            ui_filterAction->setIcon(ui_ogreAction->icon());
    } else
        ui_filterAction->setIcon(QIcon(":/filterIcon"));

    // enable the reset action only when there are active filters
    ui_resetFilterAction->setEnabled(m_numActiveFilters > 0);
}

} // end namespace Frapper