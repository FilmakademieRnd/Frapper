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
//! \file "GrabberWidget.cpp"
//! \brief Implementation file for GrabberWidget class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "GrabberWidget.h"
#include <QPainter>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the GrabberWidget class.
//!
//! \param frameRectangleSize The width of frame rectangles (used for positioning the widget).
//! \param position The grabber position type.
//! \param baseColor The base color to use for painting the widget.
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
GrabberWidget::GrabberWidget ( const QSize &frameRectangleSize, GrabberWidget::Position position, const QColor &baseColor, QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    QWidget(parent, flags),
    m_frameIndex(1),
    m_frameRectangleSize(frameRectangleSize),
    m_position(position),
    m_baseColor(baseColor),
    m_startFrameIndex(1)
{
    if (m_position == P_OnFrame)
        setFixedSize(m_frameRectangleSize);
    else {
        setFixedSize(QSize(Width, m_frameRectangleSize.height()));
        setCursor(Qt::SizeHorCursor);
    }
}


//!
//! Destructor of the GrabberWidget class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
GrabberWidget::~GrabberWidget ()
{
}


///
/// Public Functions
///


//!
//! Returns the index of the frame that the grabber represents.
//!
//! \return The index of the frame that the grabber represents.
//!
int GrabberWidget::getFrameIndex () const
{
    return m_frameIndex;
}


//!
//! Sets the index of the frame that the grabber represents.
//!
//! \param index The index of the frame that the grabber represents.
//!
void GrabberWidget::setFrameIndex ( int index )
{
    m_frameIndex = index;

    updatePosition();
}


//!
//! Sets the width used for frame rectangles in the timeline widget to the
//! given width.
//!
//! \param frameWidth The width of frame rectangles in the timeline widget.
//!
void GrabberWidget::setFrameWidth ( int frameWidth )
{
    m_frameRectangleSize.setWidth(frameWidth);

    if (m_position == P_OnFrame)
        setFixedSize(m_frameRectangleSize);

    updatePosition();
}


//!
//! Sets the index of the start frame in the animation range.
//!
//! \param index The index of the start frame in the animation range.
//!
void GrabberWidget::setStartFrameIndex ( int index )
{
    m_startFrameIndex = index;

    updatePosition();
}


///
/// Protected Event Handlers
///


//!
//! Event handler that reacts to paint events.
//!
//! \param event The object containing details about the event.
//!
void GrabberWidget::paintEvent ( QPaintEvent *event )
{
    QPainter painter (this);

    QLinearGradient gradient (QPointF(0, 0), QPointF(width(), height()));
    gradient.setColorAt(0, m_baseColor.lighter(110));
    gradient.setColorAt(1, m_baseColor.darker(110));

    painter.setPen(m_baseColor);
    painter.setBrush(QBrush(gradient));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}


///
/// Private Functions
///


//!
//! Updates the position of the widget according to the index of the frame
//! that it represents and the frame width.
//!
void GrabberWidget::updatePosition ()
{
    int frameIndexOffset = 0;
    int xOffset = 0;

    if (m_position == P_OnFrame) {
        xOffset = Width - 1;
    } else if (m_position == P_AfterFrame) {
        frameIndexOffset = 1;
        xOffset = Width - 1;
    }

    move((m_frameRectangleSize.width() - 1) * (m_frameIndex - m_startFrameIndex + frameIndexOffset) + xOffset, y());
}

} // end namespace Frapper