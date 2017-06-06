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
//! \file "KeyGraphicsItem.h"
//! \brief Header file for SegmentGraphicsItem class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.03.2013 (last updated)
//!

#ifndef SEGMENTGRAPHICSITEM_H
#define SEGMENTGRAPHICSITEM_H

#include "FrapperPrerequisites.h"
#include "KeyGraphicsItem.h"
#include <QGraphicsItem>
#include <QPainter>


namespace Frapper {

//!
//! Forward declaration for the KeyGraphicsItem class.
//!
class KeyGraphicsItem;


//!
//! Class for graphics items representing curve segments in the Curve Editor
//! panel.
//!
class FRAPPER_GUI_EXPORT SegmentGraphicsItem : public QGraphicsItem
{

public: // constructors and destructors

    //!
    //! Constructor of the SegmentGraphicsItem class.
    //!
    //! \param startKeyItem The key item where the curve segment starts.
    //! \param endKeyItem The key item where the curve segment ends.
    //! \param color The color to use for the curve segment.
    //!
	inline SegmentGraphicsItem ( 
		KeyGraphicsItem *startKeyItem, 
		KeyGraphicsItem *endKeyItem, 
		Key::KeyType keyType = Key::KT_Linear, 
		const QColor &color = QColor(Qt::darkGray) ) :
	m_startKeyItem(startKeyItem),
    m_endKeyItem(endKeyItem),
	m_keyType(keyType),
    m_color(color),
    m_startPoint(0.0f, 0.0f),
    m_endPoint(0.0f, 0.0f),
	m_width(0.9f)
	{
		setAcceptedMouseButtons(0);
		setHandlesChildEvents(false);
		setFlag(ItemClipsToShape);
		setFlag(ItemIsMovable, false);
		setFlag(ItemIsSelectable, false);

		if (m_startKeyItem)
			m_startKeyItem->addSegment(this);
		if (m_endKeyItem)
			m_endKeyItem->addSegment(this);

		adjust();
	}

    //!
    //! Destructor of the SegmentGraphicsItem class.
    //!
	inline ~SegmentGraphicsItem() {};

public: // functions

	//!
	//! Sets the with by the given value.
	//!
	//! \param width The value to set the item width.
	//!
	inline void setWidth( const float width )
	{
		m_width = width;
	}

    //!
    //! Calculates the start and end points for painting the curve segment.
    //!
	inline void adjust ()
	{
		if (m_startKeyItem && m_endKeyItem) {
			const QLineF line(mapFromItem(m_startKeyItem, 0, 0), mapFromItem(m_endKeyItem, 0, 0));
			const float length = line.length();
			QPointF edgeOffset(0.0, 0.0);
			if (!length == 0.0)
				QPointF edgeOffset ((line.dx()) / length, (line.dy()) / length);

			prepareGeometryChange();
			m_startPoint = line.p1() + edgeOffset;
			m_endPoint = line.p2() - edgeOffset;
		}
	}

protected: // functions

    //!
    //! Returns the bounding rectangle of the graphics item.
    //!
    //! \return The bounding rectangle of the graphics item.
    //!
	inline QRectF boundingRect () const
	{
		if (m_startKeyItem && m_endKeyItem) {
			switch (m_keyType) {
			case Key::KT_Bezier:
			{
				const TangentGraphicsItem *startTangent = m_startKeyItem->getTangentItem();
				const TangentGraphicsItem *endTangent = m_endKeyItem->getTangentItem();
				QPainterPath path;

				path.moveTo(m_startPoint);
				path.lineTo(startTangent->pos());
				if (endTangent)
					path.lineTo(m_endPoint*2.0f-endTangent->pos());
				else
					path.lineTo(m_endPoint);
				path.lineTo(m_endPoint);

				return path.boundingRect();
			}
			default:
				const float extra = m_width / 4.0f;
				return QRectF(m_startPoint, QSizeF(m_endPoint.x() - m_startPoint.x(), m_endPoint.y() - m_startPoint.y())).normalized().adjusted(-extra, -extra, extra, extra);
			}
		}
		else
			return QRectF();
	}
	 
    //!
    //! Paints the graphics item into a graphics view.
    //!
    //! \param painter The object to use for painting.
    //! \param option Style options for painting the graphics item.
    //! \param widget The widget into which to paint the graphics item.
    //!
	inline void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
	{
		if (m_startKeyItem && m_endKeyItem) {
			painter->setPen(QPen(m_color, m_width, Qt::SolidLine));
			switch (m_keyType) {
			case Key::KT_Bezier:
			{
				const TangentGraphicsItem *startTangent = m_startKeyItem->getTangentItem();
				const TangentGraphicsItem *endTangent = m_endKeyItem->getTangentItem();
				QPainterPath path;
				
				path.moveTo(m_startPoint);
				if (endTangent)
					path.cubicTo(startTangent->pos(), m_endPoint*2.0f-endTangent->pos(), m_endPoint);
				else 
					path.cubicTo(startTangent->pos(), m_endPoint, m_endPoint);
				
				painter->drawPath(path);
				break;
			}
			case Key::KT_Step:
				painter->drawLine(QLineF(m_startPoint.x(), m_startPoint.y(), m_endPoint.x(), m_startPoint.y()));
				break;
			default: 
				painter->drawLine(QLineF(m_startPoint, m_endPoint));
			}
		}
	}

private: // data
	//!
	//! The key type of the segment.
	//!
	Key::KeyType m_keyType;

	//!
	//! The width of the timeline.
	//!
	float m_width;

	//!
    //! The key item where the curve segment starts.
    //!
    KeyGraphicsItem *m_startKeyItem;

    //!
    //! The key item where the curve segment ends.
    //!
    KeyGraphicsItem *m_endKeyItem;

    //!
    //! The color to use for painting the curve segment.
    //!
    QColor m_color;

    //!
    //! The start point to use for painting the curve segment.
    //!
    QPointF m_startPoint;

    //!
    //! The end point to use for painting the curve segment.
    //!
    QPointF m_endPoint;
};

} // end namespace Frapper

#endif
