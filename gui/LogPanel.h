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
//! \brief Header file for LogPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef LOGPANEL_H
#define LOGPANEL_H

#include "FrapperPrerequisites.h"
#include "Panel.h"
#include "ui_LogPanel.h"
#include "CopyHandler.h"
#include <QLineEdit>


namespace Frapper {

//!
//! Class representing a panel for displaying log messages.
//!
class FRAPPER_GUI_EXPORT LogPanel : public Panel, protected Ui::LogPanel, public CopyHandler
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the LogPanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    LogPanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the LogPanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~LogPanel ();

public: // functions

    //!
    //! Fills the given tool bars with actions for the log panel.
    //!
    //! \param mainToolBar The main tool bar to fill with actions.
    //! \param panelToolBar The panel tool bar to fill with actions.
    //!
    virtual void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

    //!
    //! Copies the currently selected (or all) log messages to the clipboard.
    //!
    virtual void copy ();

protected: // events

    //!
    //! Event handler that reacts to timer events.
    //!
    //! \param event The description of the timer event.
    //!
    virtual void timerEvent ( QTimerEvent *event );

private slots: //
	
	//!
    //! Starts the timer that launches the log filtering search after a
    //! specific delay.
    //!
    //! Is called when the text in the search edit widget has changed.
    //!
    //! \param text The text that has been entered in the search edit widget.
    //!
    void m_searchEdit_textChanged ( const QString &text );

    //!
    //! Toggles filtering of log messages in the log panel.
    //!
    //! \param enabled Flag that controls whether to enable or disable filtering.
    //!
    void on_ui_filterAction_toggled ( bool enabled );

    //!
    //! Toggles the display of informational messages in the log panel.
    //!
    //! \param checked Flag that controls whether to show or hide the messages.
    //!
    void on_ui_infoAction_toggled ( bool checked );

    //!
    //! Cancels a running log search and resets the log search
    //! settings for the currently edited node.
    //!
    //! Is called when the reset search action has been triggered.
    //!
    //! \param checked The state of the action (unused).
    //!
    void on_ui_resetSearchAction_triggered ( bool checked = false );

    //!
    //! Toggles the display of warning messages in the log panel.
    //!
    //! \param checked Flag that controls whether to show or hide the messages.
    //!
    void on_ui_warningAction_toggled ( bool checked );

    //!
    //! Toggles the display of error messages in the log panel.
    //!
    //! \param checked Flag that controls whether to show or hide the messages.
    //!
    void on_ui_errorAction_toggled ( bool checked );

    //!
    //! Toggles the display of debug messages in the log panel.
    //!
    //! \param checked Flag that controls whether to show or hide the messages.
    //!
    void on_ui_debugAction_toggled ( bool checked );

    //!
    //! Toggles the display of Qt messages in the log panel.
    //!
    //! \param checked Flag that controls whether to show or hide the messages.
    //!
    void on_ui_qtAction_toggled ( bool checked );

    //!
    //! Toggles the display of OGRE messages in the log panel.
    //!
    //! \param checked Flag that controls whether to show or hide the messages.
    //!
    void on_ui_ogreAction_toggled ( bool checked );

    //!
    //! Resets the log panel's filter settings.
    //!
    void on_ui_resetFilterAction_triggered ();

    //!
    //! Copies the text of the currently selected log messages to the clipboard.
    //!
    void on_ui_copyAction_triggered ();

    //!
    //! Clears the log message history.
    //!
    void on_ui_clearAction_triggered ();

private: // functions

    //!
    //! Keeps track of the number of active message filters and enables the Filter
    //! and Reset Filter actions accordingly.
    //!
    //! \param activated Flag that states whether a message filter has been activated or deactivated.
    //!
    void updateActions ( bool activated );

private: // data

    //!
    //! The number of currently active message filters.
    //!
    int m_numActiveFilters;

	//!
    //! The line edit for editing the string for a log search.
    //!
    QLineEdit *m_searchEdit;

    //!
    //! The ID of the timer that is (re-)started each time the search text in
    //! the search edit widget is changed.
    //!
    int m_searchTimer;

};

} // end namespace Frapper

#endif
