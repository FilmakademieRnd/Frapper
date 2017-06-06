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
//! \file "TimelinePanel.h"
//! \brief Header file for TimelinePanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#ifndef TIMELINEPANEL_H
#define TIMELINEPANEL_H

#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "ui_TimelinePanel.h"
#include "TimelineWidget.h"


namespace Frapper {

//!
//! Class for a panel that contains a timeline widget for accessing the scene
//! model's time.
//!
class FRAPPER_GUI_EXPORT TimelinePanel : public ViewPanel, protected Ui::TimelinePanel
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the TimelinePanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    TimelinePanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the TimelinePanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TimelinePanel ();

public: // functions

    //!
    //! Fills the given tool bars in a panel frame with actions for the panel.
    //!
    //! \param mainToolBar The main tool bar to fill with actions.
    //! \param panelToolBar The panel tool bar to fill with actions.
    //!
    void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

    //!
    //! Fills the panel's control tool bar with the given actions.
    //!
    //! \param controlActions The actions to add to the control tool bar.
    //!
    void fillControlToolBar ( QList<QAction *> controlActions );

    //!
    //! Sets the frame range available for the scene time to the given frame
    //! indices.
    //!
    //! \param firstFrame The index of the first frame in the scene's time range.
    //! \param lastFrame The index of the last frame in the scene's time range.
    //!
    void setFrameRange ( int firstFrame, int lastFrame );

public slots: //

    //!
    //! Enables and disables certain widgets when realtime mode is toggled.
    //!
    //! \param realtime Indicates whether realtime mode is on or off.
    //!
    void toggleRealtimeMode ( bool realtime );

    //!
    //! Sets the frame step displayed in the frame step spin box to the given
    //! value.
    //!
    //! \param frameStep The frame step currently used in the scene.
    //!
    void setFrameStep ( int frameStep );

    //!
    //! Sets the frame rate displayed in the FPS spin box to the given value.
    //!
    //! \param frameRate The frame rate currently used in the scene.
    //!
    void setFrameRate ( int frameRate );

    //!
    //! Sets the index of the current frame in the scene's time.
    //!
    //! \param index The new index of the current frame in the scene's time.
    //!
    void setCurrentFrame ( int index );

    //!
    //! Sets the index of the start frame in the scene's time.
    //!
    //! \param index The new index of the start frame in the scene's time.
    //!
    void setStartFrame ( int index );

    //!
    //! Sets the index of the end frame in the scene's time.
    //!
    //! \param index The new index of the end frame in the scene's time.
    //!
    void setEndFrame ( int index );

    //!
    //! Sets the index of the in frame in the scene's time.
    //!
    //! \param index The new index of the in frame in the scene's time.
    //!
    void setInFrame ( int index );

    //!
    //! Sets the index of the out frame in the scene's time.
    //!
    //! \param index The new index of the out frame in the scene's time.
    //!
    void setOutFrame ( int index );

signals: //

    //!
    //! Signal that is emitted when the index of the current frame in the
    //! scene's time has been changed.
    //!
    //! \param index The new index of the current frame in the scene's time.
    //!
    void currentFrameSet ( int index );

    //!
    //! Signal that is emitted when the index of the start frame in the scene's
    //! time has been changed.
    //!
    //! \param index The new index of the start frame in the scene's time.
    //!
    void startFrameSet ( int index );

    //!
    //! Signal that is emitted when the index of the end frame in the scene's
    //! time has been changed.
    //!
    //! \param index The new index of the end frame in the scene's time.
    //!
    void endFrameSet ( int index );

    //!
    //! Signal that is emitted when the index of the in frame in the scene's
    //! time has been changed.
    //!
    //! \param index The new index of the in frame in the scene's time.
    //!
    void inFrameSet ( int index );

    //!
    //! Signal that is emitted when the index of the out frame in the scene's
    //! time has been changed.
    //!
    //! \param index The new index of the out frame in the scene's time.
    //!
    void outFrameSet ( int index );

    //!
    //! Signal that is emitted when the frame step for the scene has been
    //! changed.
    //!
    //! \param frameStep The new frame step to use for the scene.
    //!
    void frameStepSet ( int frameStep );

    //!
    //! Signal that is emitted when the frame rate for the scene has been
    //! changed.
    //!
    //! \param frameRate The new frame rate to use for the scene.
    //!
    void frameRateSet ( int frameRate );

protected: // events

    //!
    //! Handles key press events for the widget.
    //!
    //! \param event The description of the key event.
    //!
    virtual void keyPressEvent ( QKeyEvent *event );

private slots: //

    //!
    //! Sets the real-time mode on or off.
    //!
    //! \param realtime True -> realtime, false -> no-real-time.
    //!
    void on_ui_realtimeAction_triggered ( bool realtime );

    //!
    //! Event handler that is called when the Scale to Fit action has been
    //! triggered.
    //!
    //! \param checked The state of the action.
    //!
    void on_ui_scaleToFitAction_triggered ( bool checked = false );

    //!
    //! Event handler that is called when the value of the start frame spin
    //! box has been changed.
    //!
    //! \param value The value of the spin box.
    //!
    void on_ui_startFrameSpinBox_valueChanged ( int value );

    //!
    //! Event handler that is called when the value of the end frame spin
    //! box has been changed.
    //!
    //! \param value The value of the spin box.
    //!
    void on_ui_endFrameSpinBox_valueChanged ( int value );

    //!
    //! Event handler that is called when the value of the in frame spin
    //! box has been changed.
    //!
    //! \param value The value of the spin box.
    //!
    void on_ui_inFrameSpinBox_valueChanged ( int value );

    //!
    //! Event handler that is called when the value of the frame step spin
    //! box has been changed.
    //!
    //! \param value The value of the spin box.
    //!
    void on_ui_frameStepSpinBox_valueChanged ( int value );

    //!
    //! Event handler that is called when the value of the out frame spin
    //! box has been changed.
    //!
    //! \param value The value of the spin box.
    //!
    void on_ui_outFrameSpinBox_valueChanged ( int value );

    //!
    //! Event handler that is called when the value of the frame rate spin
    //! box has been changed.
    //!
    //! \param value The value of the spin box.
    //!
    void on_ui_fpsSpinBox_valueChanged ( int value );

private: // data

    //!
    //! The scroll bar widget for adjusting the scale of the timeline widget's
    //! frames.
    //!
    QScrollBar *m_scaleScrollBar;

    //!
    //! The widget for accessing the scene model's time.
    //!
    TimelineWidget *m_timelineWidget;

    //!
    //! The tool bar that will receive actions for controlling the scene's
    //! time.
    //!
    //! \see getControlToolBar
    //!
    QToolBar *m_controlToolBar;

};

} // end namespace Frapper

#endif
