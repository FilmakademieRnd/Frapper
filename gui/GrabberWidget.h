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
//! \file "GrabberWidget.h"
//! \brief Header file for GrabberWidget class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       23.03.2009 (last updated)
//!

#ifndef GRABBERWIDGET_H
#define GRABBERWIDGET_H

#include "FrapperPrerequisites.h"
#include <QWidget>


namespace Frapper {

//!
//! Class for a widget representing a frame grabber in a timeline.
//!
class FRAPPER_GUI_EXPORT GrabberWidget : public QWidget
{

public: // nested enumerations

    //!
    //! Nested enumeration of grabber position types specifying where to place
    //! grabber widgets relative to the rectangles of the frames that they
    //! represent.
    //!
    enum Position {
        P_BeforeFrame = -1, //!< A value indicating that the grabber should be placed before the frame that it represents.
        P_OnFrame, //!< A value indicating that the grabber should be placed on the frame that it represents.
        P_AfterFrame //!< A value indicating that the grabber should be placed after the frame that it represents.
    };

public: // static data

    //!
    //! The width of frame grabbers (should be an odd number).
    //!
    static const int Width = 5;

public: // constructors and destructors

    //!
    //! Constructor of the GrabberWidget class.
    //!
    //! \param frameRectangleSize The width of frame rectangles (used for positioning the widget).
    //! \param position The grabber position type.
    //! \param baseColor The base color to use for painting the widget.
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    GrabberWidget ( const QSize &frameRectangleSize, Position position, const QColor &baseColor, QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the GrabberWidget class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~GrabberWidget ();

public: // functions

    //!
    //! Returns the index of the frame that the grabber represents.
    //!
    //! \return The index of the frame that the grabber represents.
    //!
    int getFrameIndex () const;

    //!
    //! Sets the index of the frame that the grabber represents.
    //!
    //! \param index The index of the frame that the grabber represents.
    //!
    void setFrameIndex ( int index );

    //!
    //! Sets the width used for frame rectangles in the timeline widget to the
    //! given width.
    //!
    //! \param frameWidth The width of frame rectangles in the timeline widget.
    //!
    void setFrameWidth ( int frameWidth );

    //!
    //! Sets the index of the start frame in the animation range.
    //!
    //! \param index The index of the start frame in the animation range.
    //!
    void setStartFrameIndex ( int index );

protected: // event handlers

    //!
    //! Event handler that reacts to paint events.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void paintEvent ( QPaintEvent *event );

private: // functions

    //!
    //! Updates the position of the widget according to the index of the frame
    //! that it represents and the frame width.
    //!
    void updatePosition ();

private: // data

    //!
    //! The index of the frame the grabber represents.
    //!
    int m_frameIndex;

    //!
    //! The size of frame rectangles in the timeline widget.
    //!
    //! Used for positioning the grabber widget.
    //!
    QSize m_frameRectangleSize;

    //!
    //! The grabber's position type specifying where to place the widget
    //! relative to the rectangle of the frame that it represents.
    //!
    Position m_position;

    //!
    //! The base color that is used for painting the widget.
    //!
    QColor m_baseColor;

    //!
    //! The index of the start frame in the animation range.
    //!
    //! Used for positioning the grabber widget.
    //!
    int m_startFrameIndex;

};

} // end namespace Frapper

#endif
