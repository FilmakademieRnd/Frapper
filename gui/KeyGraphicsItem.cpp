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
//! \brief Implementation file for KeyGraphicsItem class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.03.2013 (last updated)
//!

#include "KeyGraphicsItem.h"
#include "SegmentGraphicsItem.h"


namespace Frapper {


// the size if the Item.
const float KeyGraphicsItem::s_size = 2.0f;


///
/// Public Functions
///


//!
//! Adds the given segment item to the list of segment items for the key
//! item.
//!
//! \param segmentItem The segment item to add to the list of segment items.
//!
void KeyGraphicsItem::addSegment ( SegmentGraphicsItem *segmentItem )
{
    m_segmentItems << segmentItem;
    segmentItem->adjust();
}

//!
//! Adds the given tangent item.
//!
//! \param tangentItem The tangent item to add.
//!
void KeyGraphicsItem::addTangentItem ( TangentGraphicsItem *tangentItem )
{
	m_tangentItem = tangentItem;
}


///
/// Protected Functions
///


//!
//! The item change handler for updating the node.
//!
QVariant KeyGraphicsItem::itemChange ( GraphicsItemChange change, const QVariant &value )
{
	switch (change) {
	case ItemPositionHasChanged :
		foreach (SegmentGraphicsItem *segmentItem, m_segmentItems)
			segmentItem->adjust();
		if (m_tangentItem)
			m_tangentItem->adjust();
		break;
	case ItemSelectedHasChanged :
		if (m_tangentItem)
			m_tangentItem->setVisibility(isSelected() || m_tangentItem->isSelected());
		break;
	}
	return QGraphicsItem::itemChange(change, value);
}

//!
//! The item change handler for updating the node.
//!
QVariant TangentGraphicsItem::itemChange ( GraphicsItemChange change, const QVariant &value )
{
	switch (change) {
	case ItemPositionHasChanged :
		foreach (SegmentGraphicsItem *segmentItem, m_segmentItems)
			segmentItem->adjust();
		break;
	case ItemSelectedHasChanged :
		setVisibility(isSelected());
		break;
	}
	return QGraphicsItem::itemChange(change, value);
}

//!
//! Sets the visibility for the item.
//!
//! \param The visibility state for the item.
//!
void TangentGraphicsItem::setVisibility( const bool visibility )
{
	foreach (SegmentGraphicsItem *segmentItem, m_segmentItems)
		segmentItem->setVisible(visibility);
}


} // end namespace Frapper