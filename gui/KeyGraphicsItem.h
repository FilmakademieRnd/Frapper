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
//! \brief Header file for KeyGraphicsItem class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.03.2013 (last updated)
//!

#ifndef KEYGRAPHICSITEM_H
#define KEYGRAPHICSITEM_H

#include "FrapperPrerequisites.h"
#include "NumberParameter.h"
#include <QGraphicsItem>
#include <QtCore/QList>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>


//!
//! Forward declaration for the mouse event handler.
//!
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE


namespace Frapper {

Q_DECLARE_METATYPE(Key *)


//!
//! Forward declaration for the edge class.
//!
class SegmentGraphicsItem;

//!
//! Forward declaration for the tangent class.
//!
class TangentGraphicsItem;

//!
//! Class for graphics items representing keys in the Curve Editor panel.
//!
class FRAPPER_GUI_EXPORT KeyGraphicsItem : public QGraphicsItem
{

public: // constructors and destructors

    //!
    //! Constructor of the KeyGraphicsItem class.
    //!
    //! \param curveEditorGraphicsView The graphics view in which the key graphics item is contained.
    //!
    inline KeyGraphicsItem ( const QSizeF &scale = QSizeF(1.0f, 1.0f), const float minX = 0.0f ) :
	m_minX(minX),
	m_sceneScale(scale),
	m_tangentItem(0)
	{
		setFlag(ItemIsMovable);
		setFlag(ItemIsSelectable);
		setFlag(ItemClipsToShape);
		setHandlesChildEvents(false);
		//setFlag(ItemIgnoresTransformations);
		setFlag(ItemSendsGeometryChanges, true);
		setCacheMode(DeviceCoordinateCache);
		//setCacheMode(NoCache);
		setZValue(1);

		m_segmentItems.reserve(2);
	}

public: // functions

    //!
    //! Adds the given segment item to the list of segment items for the key
    //! item.
    //!
    //! \param segmentItem The segment item to add to the list of segment items.
    //!
    void addSegment ( SegmentGraphicsItem *segmentItem );

	//!
    //! Adds the given tangent item.
    //!
    //! \param tangentItem The tangent item to add.
    //!
    void addTangentItem ( TangentGraphicsItem *tangentItem );

	//!
    //! Returns a pointer to the tangent item stored in this key item.
    //!
    //! \return A pointer to the tangent item stored in this key item.
    //!
	inline TangentGraphicsItem *getTangentItem() const
	{
		return m_tangentItem;
	}

	//!
	//! Sets the max x value for the item.
	//!
	//! \param max The max x value for the item.
	//!
	inline float getMinX( ) const
	{
		return m_minX;
	}

protected: // functions

    //!
    //! The item change handler for updating the node.
    //!
    QVariant itemChange ( GraphicsItemChange change, const QVariant &value );

	//!
    //! Returns a rectangle describing the bounding region of the key item.
    //!
    //! \return A rectangle describing the bounding region of the key item.
    //!
    inline QRectF boundingRect () const
	{
		return QRectF(-s_size, -s_size, s_size*2.f, s_size*2.f);
	}

    //!
    //! The paint function of the node.
    //!
    inline void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
	{
		painter->setPen(Qt::NoPen);
		if (isSelected())
			painter->setBrush(Qt::yellow);
		else 
			painter->setBrush(Qt::black);
		painter->drawRect(-s_size, -s_size, s_size*2.f, s_size*2.f);
	}

	//!
	//! Overwrite the default mouseMoveEvent handler to lock x axis
	//!
	inline void mouseMoveEvent ( QGraphicsSceneMouseEvent *event )
	{
		Key *key;
		const KeyGraphicsItem *keyItem;

		event->setScenePos(QPointF(m_minX, event->scenePos().y()));

		const QList<QGraphicsItem *> &selectedItems = scene()->selectedItems();
		foreach (QGraphicsItem *item, selectedItems) {
			key = item->data(0).value<Key *>();
			keyItem = dynamic_cast<KeyGraphicsItem *>(item);
			if (keyItem) {
				key->keyValue.setValue<float>((scene()->height() / 2.0f - keyItem->pos().y()) / m_sceneScale.height());
			}
			else
				continue;
		}

		QGraphicsItem::mouseMoveEvent(event);
	}
	
protected: // data

	// The size if the Item.
	static const float s_size;

	//!
    //! The minimum x value of the key
    //!
	float m_minX;

	//!
    //! The curve/scene scale
    //!
	QSizeF m_sceneScale;

	//!
    //! The list containing all curve segment items connected to the key item.
    //!
    QList<SegmentGraphicsItem *> m_segmentItems;

private: // data
	
	//!
	//! The Item representing the tangent controller
	//!
	TangentGraphicsItem *m_tangentItem;
};


class FRAPPER_GUI_EXPORT TangentGraphicsItem : public KeyGraphicsItem
{
public: // constructors and destructors
	
	//!
    //! Constructor of the KeyGraphicsItem class.
    //!
    //! \param curveEditorGraphicsView The graphics view in which the key graphics item is contained.
    //!
    TangentGraphicsItem ( KeyGraphicsItem *parent = 0, const QSizeF &scale = QSizeF(1.0f, 1.0f), const float posX = 0.0f, const float maxX = HUGE) : 
	KeyGraphicsItem(scale, posX),
	m_maxX(maxX),
	m_parentKeyItem(parent),
	m_oldParentPos(parent->pos())
	{
		if (m_parentKeyItem)
			m_parentKeyItem->addTangentItem(this);
	}


public:

	//!
	//! The item change handler for updating the node.
	//!
	QVariant itemChange ( GraphicsItemChange change, const QVariant &value );

	//!
	//! Sets the visibility for the item.
	//!
	//! \param The visibility state for the item.
	//!
	void setVisibility( const bool visibility );
	
	//!
	//! Sets the max x value for the item.
	//!
	//! \param max The max x value for the item.
	//!
	inline void setMaxX( const float max )
	{
		m_maxX = max;
	}

	//!
	//! Returns the max x value for the item.
	//!
	//! \return The max x value for the item.
	//!
	inline float getMaxX() const
	{
		return m_maxX;
	}


	//!
    //! Calculates the start and end points for painting the curve segment.
    //!
	inline void adjust ()
	{
		const QPointF newPos = m_parentKeyItem->pos();
		setPos(pos() + (newPos - m_oldParentPos));

		Key *key = data(0).value<Key *>();
		key->tangentIndex = pos().x() / m_sceneScale.width();
		key->tangentValue = (scene()->height() / 2.0f - pos().y()) / m_sceneScale.height();
		m_oldParentPos = newPos;
	}

protected: // functions


    //!
    //! The paint function of the node.
    //!
    inline void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
	{
		painter->setPen(Qt::NoPen);
		if (isSelected())
			painter->setBrush(Qt::green);
		else 
			painter->setBrush(Qt::darkGreen);
		painter->drawRect(-s_size, -s_size, s_size*2.f, s_size*2.f);
	}

	//!
	//! Overwrite the default mouseMoveEvent handler to lock x axis
	//!
	inline void mouseMoveEvent ( QGraphicsSceneMouseEvent *event )
	{
		Key *key;
		const TangentGraphicsItem *tangentItem;

		if (event->scenePos().x() < m_minX)
			event->setScenePos(QPointF(m_minX, event->scenePos().y()));

		else if (event->scenePos().x() > m_maxX)
			event->setScenePos(QPointF(m_maxX, event->scenePos().y()));

		const QList<QGraphicsItem *> &selectedItems = scene()->selectedItems();
		foreach (QGraphicsItem *item, selectedItems) {
			key = item->data(0).value<Key *>();
			tangentItem = static_cast<TangentGraphicsItem *>(item);
			key->tangentIndex = tangentItem->pos().x() / m_sceneScale.width();
			key->tangentValue = (scene()->height() / 2.0f - tangentItem->pos().y()) / m_sceneScale.height();
		}

		QGraphicsItem::mouseMoveEvent(event);
	}

private:

	//!
    //! The maximum x value of the key
    //!
	float m_maxX;

	//!
    //! The key item where the curve segment starts.
    //!
    KeyGraphicsItem *m_parentKeyItem;

	//!
    //! The former position of the parent item.
    //!
	QPointF m_oldParentPos;
};

} // end namespace Frapper

#endif
