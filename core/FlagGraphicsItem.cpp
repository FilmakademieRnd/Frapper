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
//! \file "FlagGraphicsItem.cpp"
//! \brief Implementation file for FlagGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    0.3
//! \date       16.02.2009 (last updated)
//!

#include "FlagGraphicsItem.h"
#include <QGraphicsSceneMouseEvent>

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the FlagGraphicsItem class.
//!
//! \param parent The graphics item this item will be a child of.
//! \param name The flag's name.
//! \param state The initial state of the flag.
//! \param color The flag's base color.
//! \param position The flag's relative position in its parent.
//!
FlagGraphicsItem::FlagGraphicsItem ( QGraphicsItem *parent, const QString &name, bool state, const QColor &color, const QPointF &position ) :
BaseRectItem(color, true, parent),
m_state(state)
{
    setRect(QRectF(position, QSizeF(23, 10)));
    setToolTip(name);
}


//!
//! Destructor of the FlagGraphicsItem class.
//!
FlagGraphicsItem::~FlagGraphicsItem ()
{
}


///
/// Public Functions
///


//!
//! Paints the graphics item into a graphics view.
//!
//! \param painter The object to use for painting.
//! \param option Style options for painting the graphics item.
//! \param widget The widget into which to paint the graphics item.
//!
void FlagGraphicsItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
    static QColor penColor (109, 109, 109);
    qreal penWidth = 0.8;
    QBrush brush;

    if (isEnabled()) {
        if (m_state && m_hovered)
            penWidth = 1.2;

        if (m_hovered && m_clicked)
            brush = QBrush(m_color.darker(115));
        else if (m_hovered || m_clicked)
            brush = QBrush(m_color.lighter(115));
        else if (m_state)
            brush = QBrush(m_color);
        else
            brush = Qt::NoBrush;
    } else
        if (m_state)
            brush = QBrush(m_color);
        else
            brush = Qt::NoBrush;

    painter->setPen(QPen(QBrush(penColor), penWidth));
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundRect(rect());
}


///
/// Public Slots
///


//!
//! Sets the state of the flag to the given value.
//!
//! \param checked The new state for the flag.
//!
void FlagGraphicsItem::setChecked ( bool checked )
{
    m_state = checked;
    update();
}


///
/// Protected Functions
///


//!
//! Event handler that reacts to mouse press events.
//!
//! \param event The object containing details about the event.
//!
void FlagGraphicsItem::mousePressEvent ( QGraphicsSceneMouseEvent *event )
{
    if (event->button() != Qt::LeftButton)
        return;

    BaseRectItem::mousePressEvent(event);
}


//!
//! Event handler that reacts to mouse release events.
//!
//! \param event The object containing details about the event.
//!
void FlagGraphicsItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
    if (event->button() != Qt::LeftButton)
        return;

    if (isEnabled()) {
        m_state = !m_state;

        // notify connected objects of the new state of the flag
        emit toggled(m_state);
    }

    BaseRectItem::mouseReleaseEvent(event);
}

} // end namespace Frapper