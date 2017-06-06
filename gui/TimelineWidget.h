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
//! \file "TimelineWidget.h"
//! \brief Header file for TimelineWidget class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       16.06.2009 (last updated)
//!

#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include "GrabberWidget.h"
#include <QWidget>
#include <QMouseEvent>
#include <QScrollArea>
#include <QLineEdit>
#include <QIntValidator>


namespace Frapper {

//!
//! Class for a timeline widget for accessing the scene model's time.
//!
class FRAPPER_GUI_EXPORT TimelineWidget : public QWidget
{

    Q_OBJECT

public: // static data

    //!
    //! The default size of frame rectangles (should be odd numbers).
    //!
    static const QSize DefaultFrameSize;

public: // constructors and destructors

    //!
    //! Constructor of the TimelineWidget class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    TimelineWidget ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the TimelineWidget class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TimelineWidget ();

public: // functions

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

    //!
    //! Calculates the optimum frame width for fitting all frames from start to
    //! end frame (or in to out frame) into the timeline widgets dimensions.
    //!
    //! \param inToOut Flag to control whether to calculate the optimum frame width for the range from in to out frame instead of the start to end frame.
    //! \return The frame width at which all frames in the frame range fit into the timeline widgets dimensions.
    //!
    int calculateFrameWidth ( bool inToOut = false );

public slots: //

    //!
    //! Enables or disables automatic scrolling of the widget's parent scroll
    //! area to ensure the current frame is visible.
    //!
    //! \param autoScroll The new value for the automatic scrolling flag.
    //!
    void setAutoScroll ( bool autoScroll );

    //!
    //! Sets the width of frame rectangles to the given width.
    //!
    //! \param frameWidth The width to use for frame rectangles.
    //!
    void setFrameWidth ( int frameWidth );

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

protected: // event handlers

    //!
    //! Event handler that reacts to paint events.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void paintEvent ( QPaintEvent *event );

    //!
    //! Event handler that reacts to key press events.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void keyPressEvent ( QKeyEvent *event );

    //!
    //! Event handler that reacts to mouse double clicks.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void mouseDoubleClickEvent ( QMouseEvent *event );

    //!
    //! Event handler that reacts to mouse movement.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void mouseMoveEvent ( QMouseEvent *event );

    //!
    //! Event handler that reacts to mouse clicks.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void mousePressEvent ( QMouseEvent *event );

    //!
    //! Event handler that reacts to the release of mouse buttons.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void mouseReleaseEvent ( QMouseEvent *event );

private: // functions

    //!
    //! Updates the width of the timeline widget depending on the current start
    //! and end frames and the current frame width.
    //!
    void updateWidth ();

    //!
    //! Paints a part of the image that is drawn when painting the widget.
    //!
    //! \param rectangle The area of the image to paint.
    //!
    void paintImage ( const QRect &rectangle );

    //!
    //! Returns the grabber at the given mouse pointer position.
    //!
    //! \param position The position of the mouse pointer over the widget.
    //! \param rangeGrabbersOnly Flag to control whether to only check if a frame range grabber is located at the given position.
    //! \return The grabber at the given mouse pointer position.
    //!
    GrabberWidget * grabberAt ( const QPoint &position, bool rangeGrabbersOnly = true );

    //!
    //! Enters or leaves the mode for editing the index of the current frame.
    //!
    //! \param startEditing Flag to control whether to start or stop editing the current frame index.
    //!
    void editCurrentFrame ( bool startEditing = true );

private slots: //

    //!
    //! Slot that is called when the return key has been pressed while the
    //! current frame line edit has focus.
    //!
    //! Sets the entered index as the new index for the current frame.
    //!
    void on_m_currentFrameEdit_returnPressed ();

private: // data

    //!
    //! The image that will be used for painting the widget.
    //!
    QImage m_image;

    //!
    //! The scroll area in which the timeline widget resides.
    //!
    QScrollArea *m_parentScrollArea;

    //!
    //! The start frame grabber widget.
    //!
    GrabberWidget *m_startGrabber;

    //!
    //! The end frame grabber widget.
    //!
    GrabberWidget *m_endGrabber;

    //!
    //! The in frame grabber widget.
    //!
    GrabberWidget *m_inGrabber;

    //!
    //! The out frame grabber widget.
    //!
    GrabberWidget *m_outGrabber;

    //!
    //! The current frame grabber widget.
    //!
    GrabberWidget *m_currentGrabber;

    //!
    //! The grabber that is currently moved in the timeline.
    //!
    GrabberWidget *m_movedGrabber;

    //!
    //! The widget for editing the index of the current frame.
    //!
    QLineEdit *m_currentFrameEdit;

    //!
    //! The validator to use when editing the index of the current frame.
    //!
    QIntValidator *m_currentFrameValidator;

    //!
    //! Flag that states whether automatic scrolling of the parent scroll area
    //! is enabled.
    //!
    bool m_autoScroll;

    //!
    //! The width of frame rectangles (should be an odd number).
    //!
    int m_frameWidth;

    //!
    //! The heigth of frame rectangles (should be an odd number).
    //!
    int m_frameHeight;

    //!
    //! The step size for indices for which ticks will be drawn in the widget.
    //!
    int m_tickStep;

};

} // end namespace Frapper

#endif
