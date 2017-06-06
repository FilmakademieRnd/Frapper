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
//! \file "TimelineGraphicsItem.h"
//! \brief Header file for TimelineGraphicsItem class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       03.07.2009 (last updated)
//!

#ifndef TIMELINEGRAPHICSITEM_H
#define TIMELINEGRAPHICSITEM_H

#include "FrapperPrerequisites.h"
#include <QGraphicsItem>


namespace Frapper {

//!
//! Class for graphics items representing keys in the Curve Editor panel.
//!
class FRAPPER_GUI_EXPORT TimelineGraphicsItem : public QGraphicsItem
{

public: // constructors and destructors

    //!
    //! Constructor of the TimelineGraphicsItem class.
    //!
    //! \param height The height of th item.
    //!
    inline TimelineGraphicsItem ( const float height ) :
	m_height(height),
	m_width(1.0f)
	{
		setAcceptedMouseButtons(0);
		setHandlesChildEvents(false);
		setFlag(ItemIsMovable, false);
		setFlag(ItemIsSelectable, false);
		setCacheMode(DeviceCoordinateCache);
		setZValue(1);
	}


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
	//! Returns a rectangle describing the bounding region of the key item.
	//!
	//! \return A rectangle describing the bounding region of the key item.
	//!
	inline QRectF boundingRect () const
	{
		return QRectF(-1.0f, 0.0f, 2.0f, m_height);
	}

    //!
    //! The paint function of the node.
    //!
    virtual inline void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
	{
		painter->setPen(QPen(QColor(0, 152, 206), m_width, Qt::SolidLine));
		painter->drawLine(QLineF(QPointF(0.0f, 0.0f), QPointF(0.0f, m_height)));
	}

private: // data

	//!
	//! the height of the timeline
	//!
	float m_height;

	//!
	//! the width of the timeline
	//!
	float m_width;
};

} // end namespace Frapper

#endif
