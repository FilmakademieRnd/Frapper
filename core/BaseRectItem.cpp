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
//! \file "BaseRectItem.cpp"
//! \brief Implementation file for BaseRectItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    0.2
//! \date       27.11.2008 (last updated)
//!

#include "BaseRectItem.h"
#include <QGraphicsSceneMouseEvent>

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the BaseRectItem class.
//!
//! \param color The base color for the graphics item.
//! \param actAsButton Flag that states whether the item should act as a button.
//! \param parent The graphics item this item will be a child of.
//!
BaseRectItem::BaseRectItem ( const QColor &color, bool actAsButton /* = false */, QGraphicsItem *parent /* = 0 */ ) :
QGraphicsRectItem(parent)
{
    m_color = color;
    m_actAsButton = actAsButton;
    m_hovered = false;
    m_clicked = false;
    setAcceptHoverEvents(true);
}

//!
//! Constructor of the BaseRectItem class.
//!
//! \param color The base color for the graphics item.
//! \param actAsButton Flag that states whether the item should act as a button.
//! \param parent The graphics item this item will be a child of.
//!
BaseRectItem::BaseRectItem ( const int width, const int height, const QColor &color, bool actAsButton /* = false */, QGraphicsItem *parent /* = 0 */ ) :
QGraphicsRectItem(parent)
{
	setRect(0, 0, width, height);
	m_color = color;
	m_actAsButton = actAsButton;
	m_hovered = false;
	m_clicked = false;
	setAcceptHoverEvents(true);
}


//!
//! Destructor of the BaseRectItem class.
//!
BaseRectItem::~BaseRectItem ()
{
}


///
/// Public Functions
///


//!
//! Returns the bounding rectangle of the graphics item.
//!
//! \return The bounding rectangle of the graphics item.
//!
QRectF BaseRectItem::boundingRect () const
{
    qreal borderWidth = 4;
    return rect().adjusted(-borderWidth, -borderWidth, borderWidth, borderWidth);
}


//!
//! Returns the item's base color.
//!
//! \return The item's base color.
//!
QColor BaseRectItem::getColor () const
{
    return m_color;
}


///
/// Protected Functions
///


//!
//! Event handler that reacts to hover enter events.
//!
//! \param event The object containing details about the event.
//!
void BaseRectItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
    m_hovered = true;

    // call the base implementation for updating the item
    QGraphicsRectItem::hoverEnterEvent(event);
}


//!
//! Event handler that reacts to hover leave events.
//!
//! \param event The object containing details about the event.
//!
void BaseRectItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
    m_hovered = false;

    // call the base implementation for updating the item
    QGraphicsRectItem::hoverLeaveEvent(event);
}


//!
//! Event handler that reacts to mouse press events.
//!
//! \param event The object containing details about the event.
//!
void BaseRectItem::mousePressEvent ( QGraphicsSceneMouseEvent *event )
{
    m_clicked = true;
    update();

    if (!m_actAsButton)
        // call the base implementation for selection and move handling
        QGraphicsRectItem::mousePressEvent(event);
}


//!
//! Event handler that reacts to mouse release events.
//!
//! \param event The object containing details about the event.
//!
void BaseRectItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
    m_clicked = false;
    update();

    if (!m_actAsButton)
        // call the base implementation for selection and move handling
        QGraphicsRectItem::mouseReleaseEvent(event);
}

} // end namespace Frapper