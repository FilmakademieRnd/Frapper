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
//! \file "TimelinePanel.cpp"
//! \brief Implementation file for TimelinePanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#include "TimelinePanel.h"
#include <QLabel>
#include <QScrollBar>
#include <QToolButton>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the TimelinePanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
TimelinePanel::TimelinePanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_Timeline, parent, flags),
    m_scaleScrollBar(0),
    m_timelineWidget(0),
    m_controlToolBar(0)
{
    setupUi(this);

    // create the timeline widget as a child of the scroll area
    m_timelineWidget = new TimelineWidget(ui_scrollArea);
    ui_scrollArea->setWidget(m_timelineWidget);

    // set up signal relay connections from the timeline widget
    connect(m_timelineWidget, SIGNAL(currentFrameSet(int)), SIGNAL(currentFrameSet(int)));
    connect(m_timelineWidget, SIGNAL(startFrameSet(int)), SIGNAL(startFrameSet(int)));
    connect(m_timelineWidget, SIGNAL(endFrameSet(int)), SIGNAL(endFrameSet(int)));
    connect(m_timelineWidget, SIGNAL(inFrameSet(int)), SIGNAL(inFrameSet(int)));
    connect(m_timelineWidget, SIGNAL(outFrameSet(int)), SIGNAL(outFrameSet(int)));

    // set up action connections
    connect(ui_autoScrollAction, SIGNAL(toggled(bool)), m_timelineWidget, SLOT(setAutoScroll(bool)));

    // create the tool bar for timeline controls
    m_controlToolBar = new QToolBar("Timeline Controls", this);
    m_controlToolBar->setStyleSheet(
        "QToolButton {"
        "    max-height: 24px;"
        "}"
    );
    ui_horizontalLayout->insertWidget(2, m_controlToolBar);

    // set the special value text for the frame step spin box to an empty string that is displayed instead of "1st"
    ui_frameStepSpinBox->setSpecialValueText(" ");
}


//!
//! Destructor of the TimelinePanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TimelinePanel::~TimelinePanel ()
{
}


///
/// Public Functions
///


//!
//! Fills the given tool bars in a panel frame with actions for the panel.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void TimelinePanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
#ifdef _DEBUG
    if (!mainToolBar) {
        Log::debug("The given main tool bar is invalid.", "TimelinePanel::fillToolBars");
        return;
    }
#endif

    // create a label for the frame rectangle scale
    QLabel *scaleLabel = new QLabel(tr("Scale:"), mainToolBar);
    scaleLabel->setStyleSheet("margin-right: 2px;");

    // create a scroll bar for the frame rectangle scale
    m_scaleScrollBar = new QScrollBar(mainToolBar);
    m_scaleScrollBar->setOrientation(Qt::Horizontal);
    m_scaleScrollBar->setMinimum(3);
    m_scaleScrollBar->setMaximum(47);
    m_scaleScrollBar->setSingleStep(2);
    m_scaleScrollBar->setValue(TimelineWidget::DefaultFrameSize.width());
    m_scaleScrollBar->setMinimumWidth(200);
    connect(m_scaleScrollBar, SIGNAL(valueChanged(int)), m_timelineWidget, SLOT(setFrameWidth(int)));

    // create a label for separating the scroll bar and the following actions
    QLabel *separatorLabel = new QLabel(mainToolBar);

    // fill the main tool bar
    mainToolBar->addActions(QList<QAction *>() << ui_autoScrollAction);
    mainToolBar->addSeparator();
    mainToolBar->addWidget(scaleLabel);
    mainToolBar->addWidget(m_scaleScrollBar);
    mainToolBar->addWidget(separatorLabel);
    mainToolBar->addActions(QList<QAction *>() << ui_scaleToFitAction);
}


//!
//! Fills the panel's control and options tool bars with the given actions.
//!
//! \param controlActions The actions to add to the control tool bar.
//! \param optionsActions The actions to add to the options tool bar.
//!
void TimelinePanel::fillControlToolBar ( QList<QAction *> controlActions )
{
    m_controlToolBar->addActions(controlActions);
    // set the popup mode for all tool buttons in the control tool bar to immediately display an associated menu (if present)
    for (int i = 0; i < controlActions.size(); ++i) {
        QToolButton *toolButton = qobject_cast<QToolButton *>(m_controlToolBar->widgetForAction(controlActions[i]));
        if (toolButton) {
            toolButton->setPopupMode(QToolButton::InstantPopup);
            toolButton->setFixedWidth(44);
        }
    }
}


//!
//! Sets the frame range available for the scene time to the given frame
//! indices.
//!
//! \param firstFrame The index of the first frame in the scene's time range.
//! \param lastFrame The index of the last frame in the scene's time range.
//!
void TimelinePanel::setFrameRange ( int firstFrame, int lastFrame )
{
    ui_startFrameSpinBox->setRange(firstFrame, lastFrame);
    ui_endFrameSpinBox->setRange(firstFrame, lastFrame);
}


///
/// Public Slots
///

//!
//! Enables and disables certain widgets when realtime mode is toggled.
//!
//! \param realtime Indicates whether realtime mode is on or off.
//!
void TimelinePanel::toggleRealtimeMode ( bool realtime )
{
    bool notRealtime = !realtime;
    ui_startFrameSpinBox->setEnabled(notRealtime);
    ui_endFrameSpinBox->setEnabled(notRealtime);
    ui_inFrameSpinBox->setEnabled(notRealtime);
    ui_outFrameSpinBox->setEnabled(notRealtime);
}

//!
//! Sets the frame step displayed in the frame step spin box to the given
//! value.
//!
//! \param frameStep The frame step currently used in the scene.
//!
void TimelinePanel::setFrameStep ( int frameStep )
{
    ui_frameStepSpinBox->setValue(frameStep);
}


//!
//! Sets the frame rate displayed in the FPS spin box to the given value.
//!
//! \param frameRate The frame rate currently used in the scene.
//!
void TimelinePanel::setFrameRate ( int frameRate )
{
    ui_fpsSpinBox->setValue(frameRate);
}


//!
//! Sets the index of the current frame in the scene's time.
//!
//! \param index The new index of the current frame in the scene's time.
//!
void TimelinePanel::setCurrentFrame ( int index )
{
    if (m_timelineWidget)
        m_timelineWidget->setCurrentFrame(index);
}


//!
//! Sets the index of the start frame in the scene's time.
//!
//! \param index The new index of the start frame in the scene's time.
//!
void TimelinePanel::setStartFrame ( int index )
{
    ui_startFrameSpinBox->setValue(index);

    ui_inFrameSpinBox->setMinimum(index);
    ui_outFrameSpinBox->setMinimum(index);
    ui_endFrameSpinBox->setMinimum(index);

    if (m_timelineWidget)
        m_timelineWidget->setStartFrame(index);
}


//!
//! Sets the index of the end frame in the scene's time.
//!
//! \param index The new index of the end frame in the scene's time.
//!
void TimelinePanel::setEndFrame ( int index )
{
    ui_endFrameSpinBox->setValue(index);

    ui_startFrameSpinBox->setMaximum(index);
    ui_inFrameSpinBox->setMaximum(index);
    ui_outFrameSpinBox->setMaximum(index);

    if (m_timelineWidget)
        m_timelineWidget->setEndFrame(index);
}


//!
//! Sets the index of the in frame in the scene's time.
//!
//! \param index The new index of the in frame in the scene's time.
//!
void TimelinePanel::setInFrame ( int index )
{
    ui_inFrameSpinBox->setValue(index);

    ui_outFrameSpinBox->setMinimum(index);

    if (m_timelineWidget)
        m_timelineWidget->setInFrame(index);
}


//!
//! Sets the index of the out frame in the scene's time.
//!
//! \param index The new index of the out frame in the scene's time.
//!
void TimelinePanel::setOutFrame ( int index )
{
    ui_outFrameSpinBox->setValue(index);

    ui_inFrameSpinBox->setMaximum(index);

    if (m_timelineWidget)
        m_timelineWidget->setOutFrame(index);
}


///
/// Protected Events
///


//!
//! Handles key press events for the widget.
//!
//! \param event The description of the key event.
//!
void TimelinePanel::keyPressEvent ( QKeyEvent *event )
{
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    switch (event->key()) {
        case Qt::Key_A:
            ui_autoScrollAction->toggle();
            break;
        case Qt::Key_F:
            ui_scaleToFitAction->trigger();
            break;
        default:
            ViewPanel::keyPressEvent(event);
    }
}


///
/// Private Slots
///

//!
//! Sets the real-time mode on or off.
//!
//! \param realtime True -> realtime, false -> no-real-time.
//!
void TimelinePanel::on_ui_realtimeAction_triggered ( bool realtime )
{
    bool notRealtime = !realtime;
    ui_startFrameSpinBox->setEnabled(notRealtime);
    ui_inFrameSpinBox->setEnabled(notRealtime);
    ui_outFrameSpinBox->setEnabled(notRealtime);
    ui_endFrameSpinBox->setEnabled(notRealtime);
    m_controlToolBar->setEnabled(notRealtime);
}

//!
//! Event handler that is called when the Scale to Fit action has been
//! triggered.
//!
//! \param checked The state of the action.
//!
void TimelinePanel::on_ui_scaleToFitAction_triggered ( bool checked /* = false */ )
{
    int frameWidth = m_timelineWidget->calculateFrameWidth();
    m_scaleScrollBar->setValue(frameWidth);
}


//!
//! Event handler that is called when the value of the start frame spin
//! box has been changed.
//!
//! \param value The value of the spin box.
//!
void TimelinePanel::on_ui_startFrameSpinBox_valueChanged ( int value )
{
    emit startFrameSet(value);
}


//!
//! Event handler that is called when the value of the end frame spin
//! box has been changed.
//!
//! \param value The value of the spin box.
//!
void TimelinePanel::on_ui_endFrameSpinBox_valueChanged ( int value )
{
    emit endFrameSet(value);
}


//!
//! Event handler that is called when the value of the in frame spin
//! box has been changed.
//!
//! \param value The value of the spin box.
//!
void TimelinePanel::on_ui_inFrameSpinBox_valueChanged ( int value )
{
    emit inFrameSet(value);
}


//!
//! Event handler that is called when the value of the out frame spin
//! box has been changed.
//!
//! \param value The value of the spin box.
//!
void TimelinePanel::on_ui_outFrameSpinBox_valueChanged ( int value )
{
    emit outFrameSet(value);
}


//!
//! Event handler that is called when the value of the frame step spin
//! box has been changed.
//!
//! \param value The value of the spin box.
//!
void TimelinePanel::on_ui_frameStepSpinBox_valueChanged ( int value )
{
    if (value % 10 == 1 && value != 11)
        ui_frameStepSpinBox->setSuffix(tr("st"));
    else if (value % 10 == 2 && value != 12)
        ui_frameStepSpinBox->setSuffix(tr("nd"));
    else if (value % 10 == 3 && value != 13)
        ui_frameStepSpinBox->setSuffix(tr("rd"));
    else
        ui_frameStepSpinBox->setSuffix(tr("th"));

    emit frameStepSet(value);
}


//!
//! Event handler that is called when the value of the frame rate spin
//! box has been changed.
//!
//! \param value The value of the spin box.
//!
void TimelinePanel::on_ui_fpsSpinBox_valueChanged ( int value )
{
    emit frameRateSet(value);
}

} // end namespace Frapper