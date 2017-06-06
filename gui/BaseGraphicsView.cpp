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
//! \file "BaseGraphicsView.h"
//! \brief Implementation file for BaseGraphicsView class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       22.06.2009 (last updated)
//!

#include "BaseGraphicsView.h"
#include "ConnectionGraphicsItem.h"
#include "Log.h"
#include <QGraphicsItem>
#include <QApplication>
#include <QScrollBar>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the BaseGraphicsView class.
//!
//! \param parent The parent widget the created instance will be a child of.
//!
BaseGraphicsView::BaseGraphicsView ( QWidget *parent /* = 0 */ ) :
    QGraphicsView(parent),
    m_viewMode(false)
{
    setObjectName("BaseGraphicsView");
    setMouseTracking(true);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::RubberBandDrag);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setBackgroundBrush(palette().dark());
	//setCacheMode(QGraphicsView::CacheBackground);
	
	//QGLWidget *glWidget = new QGLWidget(QGLFormat(QGL::SampleBuffers));
	//glWidget->format().setSwapInterval(0);
	//glWidget->setAutoFillBackground(false);
	//setViewport(glWidget);
}


//!
//! Destructor of the BaseGraphicsView class.
//!
BaseGraphicsView::~BaseGraphicsView(void)
{
}


///
/// Public Slots
///


//!
//! Resets the network graphics view's matrix.
//!
void BaseGraphicsView::homeView ()
{
    setScale(1);

    QRectF boundingRect = scene()->itemsBoundingRect();
    centerOn(boundingRect.left() + boundingRect.width() / 2, boundingRect.top() + boundingRect.height() / 2);
}


//!
//! Changes the viewing transformation so that all items are visible at maximum
//! zoom level.
//!
void BaseGraphicsView::frameAll ()
{
    frame(scene()->itemsBoundingRect());
}


//!
//! Changes the viewing transformation so that the selected items are visible
//! at maximum zoom level.
//!
void BaseGraphicsView::frameSelected ()
{
    // make sure there is a selection
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    if (selectedItems.size() == 0)
        return;

    // obtain the bounding rectangle that encompasses all selected items
    QRectF boundingRect;
    foreach (QGraphicsItem *item, selectedItems)
        boundingRect = boundingRect.united(item->sceneBoundingRect());

    frame(boundingRect);
}


//!
//! Toggles the visibility of scrollbars for the network graphics view.
//!
//! \param visible Flag to control whether to show or hide the scrollbars.
//!
void BaseGraphicsView::toggleScrollbars ( bool visible )
{
    // store the scroll values of both scrollbars
    int horizontalValue = horizontalScrollBar()->value();
    int verticalValue = verticalScrollBar()->value();

    Qt::ScrollBarPolicy scrollbarPolicy = Qt::ScrollBarAsNeeded;
    if (visible)
        scrollbarPolicy = Qt::ScrollBarAlwaysOn;
    else
        scrollbarPolicy = Qt::ScrollBarAlwaysOff;

    setHorizontalScrollBarPolicy(scrollbarPolicy);
    setVerticalScrollBarPolicy(scrollbarPolicy);

    // restore the scroll values of both scrollbars
    horizontalScrollBar()->setValue(horizontalValue);
    verticalScrollBar()->setValue(verticalValue);
}


///
/// Protected Events
///


//!
//! Event handler for key press events.
//!
//! \param event The description of the key event.
//!
void BaseGraphicsView::keyPressEvent ( QKeyEvent *event )
{
    if (event->isAutoRepeat() && event->key() != Qt::Key_Plus && event->key() != Qt::Key_Minus) {
        event->ignore();
        return;
    }

    switch (event->key()) {
        case Qt::Key_Plus:
            zoomIn();
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_Alt:
            m_viewMode = true;
            setDragMode(QGraphicsView::ScrollHandDrag);
            //QGraphicsView::keyPressEvent(event);
            break;
        default:
            QGraphicsView::keyPressEvent(event);
    }
}


//!
//! Event handler for key release events.
//!
//! \param event The description of the key event.
//!
void BaseGraphicsView::keyReleaseEvent ( QKeyEvent *event )
{
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    switch (event->key()) {
        case Qt::Key_Alt:
            m_viewMode = false;
            setCursor(Qt::ArrowCursor);
            setDragMode(QGraphicsView::NoDrag);
            setDragMode(QGraphicsView::RubberBandDrag);
            //QGraphicsView::keyReleaseEvent(event);
            break;
        default:
            QGraphicsView::keyReleaseEvent(event);
    }
}


//!
//! Event handler for mouse press events.
//!
//! \param event The description of the mouse event.
//!
void BaseGraphicsView::mousePressEvent ( QMouseEvent *event )
{
    m_lastPosition = event->pos();

    if (m_viewMode) {
        if (event->button() == Qt::LeftButton)
            QGraphicsView::mousePressEvent(event);
        else if (event->button() == Qt::MidButton) {
            // create a new mouse event that simulates a click of the left button instead of the middle button
            QMouseEvent mouseEvent (event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
            QGraphicsView::mousePressEvent(&mouseEvent);
        } else if (event->button() == Qt::RightButton)
            setCursor(Qt::SizeFDiagCursor);
    } else {
        // check if the middle mouse button has been pressed
        if (event->button() == Qt::MidButton) {
            setDragMode(QGraphicsView::ScrollHandDrag);

            // create a new mouse event that simulates a click of the left button instead of the middle button
            QMouseEvent mouseEvent (event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
            QGraphicsView::mousePressEvent(&mouseEvent);
		}
		else
			QGraphicsView::mousePressEvent(event);
	}
}


//!
//! Event handler for mouse move events.
//!
//! \param event The description of the mouse event.
//!
void BaseGraphicsView::mouseMoveEvent ( QMouseEvent *event )
{
    if (qApp->focusWidget() != this) {
        m_viewMode = false;
        setCursor(Qt::ArrowCursor);
        setFocus(Qt::MouseFocusReason);
    }

    if (m_viewMode) {
        if (event->buttons() & Qt::RightButton) {
            // calculate the difference in X coordinates
            double dx = double(event->x() - m_lastPosition.x());
            if (dx != 0)
                if (dx > 0)
                    zoomIn();
                else
                    zoomOut();
        }
    } else {
        m_lastPosition = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}


//!
//! Event handler for mouse release events.
//!
//! \param event The description of the mouse event.
//!
void BaseGraphicsView::mouseReleaseEvent ( QMouseEvent *event )
{
    if (m_viewMode) {
        setCursor(Qt::OpenHandCursor);
        QGraphicsView::mouseReleaseEvent(event);
    } else
        if (event->button() == Qt::MidButton) {
            setCursor(Qt::ArrowCursor);
            setDragMode(QGraphicsView::NoDrag);
            setDragMode(QGraphicsView::RubberBandDrag);
            QGraphicsView::mouseReleaseEvent(event);
        } else
            QGraphicsView::mouseReleaseEvent(event);
}


//!
//! Event handler for mouse wheel events.
//!
//! \param event The description of the mouse wheel event.
//!
void BaseGraphicsView::wheelEvent ( QWheelEvent *event )
{
    if (event->delta() != 0)
        if (event->delta() > 0)
            zoomIn();
        else
            zoomOut();
}


///
/// Private Functions
///


//!
//! Changes the scale of the graphics scene so that it appears bigger in
//! the graphics view.
//!
void BaseGraphicsView::zoomIn ()
{
    // get the current scale from the viewing transformation matrix
    const qreal scaleFactor = matrix().m11();

    if (scaleFactor < MAX_SCALE)
        scale(1.1f, 1.1f);
}


//!
//! Changes the scale of the graphics scene so that it appears smaller in
//! the graphics view.
//!
void BaseGraphicsView::zoomOut ()
{
    // get the current scale from the viewing transformation matrix
    const qreal scaleFactor = matrix().m11();

    if (scaleFactor > MIN_SCALE)
        scale(0.9f, 0.9f);
}


//!
//! Sets the scale of the viewing transformation matrix that is used for
//! displaying the graphics scene.
//!
//! \param s The new scale for the viewing transformation matrix.
//!
void BaseGraphicsView::setScale ( qreal s )
{
    setMatrix(QMatrix(s, matrix().m12(), matrix().m21(), s, matrix().dx(), matrix().dy()));
}


//!
//! Sets the viewing transformation so that the given bounding rectangle is
//! fully visible in the graphics view, and centers the bounding rectangle
//! in the middle of the view.
//!
//! \param rect The rectangular region to frame in the graphics view.
//!
void BaseGraphicsView::frame ( const QRectF &rect )
{
    fitInView(rect, Qt::KeepAspectRatio);

    // make sure the scale factor does not exceed the limits
    double scaleFactor = matrix().m11();
    if (scaleFactor < MIN_SCALE)
        setScale(MIN_SCALE);
    else if (scaleFactor > MAX_SCALE)
        setScale(MAX_SCALE);

    centerOn(rect.left() + rect.width() / 2, rect.top() + rect.height() / 2);
}

} // end namespace Frapper