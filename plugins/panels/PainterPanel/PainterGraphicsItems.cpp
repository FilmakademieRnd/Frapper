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
//! \file "PainterGraphicsItems.h"
//! \brief Header file for PainterGraphicsItems class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.09.2013 (last updated)
//!

#include "PainterGraphicsItems.h"
#include "Log.h"

#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QDebug>
#include <qmath.h>


namespace PainterPanel {


// Init static values
bool BaseShapeItem::s_useLUT = false;

/************************************************************************/
/* PointsHandle Methods                                                 */
/************************************************************************/

	//!
	//! Constructor of the PointHandle class.
	//!
	BaseShapeItem::PointHandle::PointHandle( QPointF position, BaseShapeItem* parent, QColor penColor) : 
		QGraphicsRectItem(-4, -4, 8, 8, parent), 
		m_parent(parent)
	{
		setPos(position);
		setFlag( ItemIsMovable );
		setFlag( ItemIgnoresTransformations);
		setBrush( Qt::NoBrush );
		setPen( QPen(penColor));
		setVisible(false);
		setCursor(Qt::SizeAllCursor);
	}
	
	//!
	//! Override event handler to listen to mouse move events
	//! \param event the graphics scene mouse event to listen to
	//!
	void BaseShapeItem::PointHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{ 
		// set new position of handle and call updateShape on parent
		setPos( mapToParent(event->pos()) );
		m_parent->updateShape();
	}

	void BaseShapeItem::PointHandle::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
	{
		m_parent->updateShape();
		m_parent->signalItemPointsChange();
	}

	//!
	//! Constructor of the CurvePointHandle class.
	//!
	BaseShapeItem::CurvePointHandle::CurvePointHandle( QPointF position, BaseShapeItem* parent, QColor curveColor, QColor handleColor) : 
	PointHandle(position, parent, curveColor), 
		m_parent(parent),
		m_handleOnePosition( new ControlPointHandle( position, this, handleColor ) ),
		m_handleTwoPosition( new ControlPointHandle( position, this, handleColor ) )
	{
		m_handleOnePosition->setPos(QPointF(0,0));
		m_handleTwoPosition->setPos(QPointF(0,0));
		setVisible(false);
		setFlag( ItemIgnoresTransformations, false );
	}

	//!
	//! Destructor of the CurvePointHandle class.
	//!
	BaseShapeItem::CurvePointHandle::~CurvePointHandle() 
	{
		delete m_handleOnePosition;
		delete m_handleTwoPosition;
	}

	//!
	//! Override event handler to listen to mouse move events
	//! \param event the graphics scene mouse event to listen to
	//!
	void BaseShapeItem::CurvePointHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{ 
		if( event->modifiers() == Qt::ControlModifier )
		{
			setHandlesMovable(true);
			setHandlesVisible(true);
			setHandlesSelected(true);
			
			QPointF motherPos = this->scenePos();
			QPointF newPos = event->scenePos();
			QPointF diff = newPos - motherPos;

			setHandleOnePosition( (diff * -1) );
			setHandleTwoPosition( diff );
			m_parent->updateShape();
		}
		else
		{
			PointHandle::mouseMoveEvent(event);
		}
	}

	void BaseShapeItem::CurvePointHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		// Alt: Add Curve Point
		if( event->modifiers() == Qt::AltModifier ) {

			QPointF pos = this->pos();
			BezierCurveItem* bezierItem = dynamic_cast<BezierCurveItem*>(m_parent);

			if(bezierItem) {
				CurvePointHandle* newHandle = new CurvePointHandle( pos, m_parent, Qt::blue, Qt::red );
				newHandle->setSelected(true);
				bezierItem->insertCurvePointAtPosition(pos, newHandle);
				bezierItem->updateShape();
				this->setSelected(false);
			}
		}
		// Shift: Delete Curve Point
		else if( event->modifiers() == Qt::ShiftModifier ) {
			BezierCurveItem* bezierItem = dynamic_cast<BezierCurveItem*>(m_parent);
			QPointF pos = this->pos();

			if(bezierItem) {
				QList<QPointF> points = bezierItem->getPoints();
				for(int i = 0; i < points.size(); i++) {
					if(points.at(i) == pos) 
						bezierItem->removeCurvePoint(i);
				}
			}
		}
	}

	void BaseShapeItem::CurvePointHandle::setHandlesMovable( bool moveable /*= true */ )
	{
		m_handleOnePosition->setFlag( ItemIsMovable, moveable );
		m_handleTwoPosition->setFlag( ItemIsMovable, moveable );
	}

	void BaseShapeItem::CurvePointHandle::setHandlesVisible( bool visible /*= true */ )
	{
		m_handleOnePosition->setVisible(visible);
		m_handleTwoPosition->setVisible(visible);
	}

	void BaseShapeItem::CurvePointHandle::setHandlesSelected( bool selected /*= true */ )
	{
		m_handleOnePosition->setSelected(selected);
		m_handleTwoPosition->setSelected(selected);
	}

	QPointF BaseShapeItem::CurvePointHandle::getHandleOnePosition()
	{
		return m_handleOnePosition->pos();
	}

	QPointF BaseShapeItem::CurvePointHandle::getHandleOneScenePosition()
	{

		return m_handleOnePosition->scenePos();
	}

	QPointF BaseShapeItem::CurvePointHandle::getHandleTwoPosition()
	{
		return m_handleTwoPosition->pos();
	}

	QPointF BaseShapeItem::CurvePointHandle::getHandleTwoScenePosition()
	{
		return m_handleTwoPosition->scenePos();
	}

	//! Sets the Position of the first handle point
	//!
	//! \param position the new position in scene coordinates
	//! \param invert if the position should be point mirrored to the curve
	void BaseShapeItem::CurvePointHandle::setHandleOnePosition( QPointF position, bool invert)
	{
		if(invert)
			m_handleOnePosition->setPos( (position * -1) );
		else
			m_handleOnePosition->setPos( position );
	}

	//! Sets the Position of the second handle point
	//!
	//! \param position the new position in scene coordinates
	//! \param invert if the position should be point mirrored to the curve
	void BaseShapeItem::CurvePointHandle::setHandleTwoPosition( QPointF position, bool invert )
	{
		if(invert)
			m_handleTwoPosition->setPos( (position * -1) );
		else
			m_handleTwoPosition->setPos( position );
	}

	void BaseShapeItem::CurvePointHandle::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0 */ )
	{
		// apply translation only, reset scaling & rotation
		qreal scaleItem = scale(); // <- thats the current scale factor of this item

		painter->rotate( -m_parent->getRotation()); // unwind items inherited rotation
		qreal scaleView = painter->transform().m11(); // <- thats the scale factor inherited from the view

		painter->scale( scaleItem / scaleView, scaleItem / scaleView ); // apply the inverse of the views scale the reveal the original items scale

		PointHandle::paint(painter,option,widget);
	}

	//!
	//! Constructor of the PointHandle class.
	//!
	BaseShapeItem::CurvePointHandle::ControlPointHandle::ControlPointHandle( QPointF position, CurvePointHandle* parent, QColor penColor) : 
		QGraphicsRectItem(-4, -4, 8, 8, parent), 
		m_parent(parent)
	{
		setPos(position);
		setBrush( Qt::NoBrush );
		setPen( QPen(penColor));
		setCursor(Qt::SizeAllCursor);
		setVisible(false);
		setFlag( ItemIgnoresTransformations, false);
	}

	//!
	//! Override event handler to listen to mouse press events
	//! \param event the graphics scene mouse event to listen to
	//!
	void BaseShapeItem::CurvePointHandle::ControlPointHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		if( event->modifiers() == Qt::ShiftModifier ) {
			m_parent->setHandleOnePosition(QPointF(0.0, 0.0));
			m_parent->setHandleTwoPosition(QPointF(0.0, 0.0));
			m_parent->setHandlesVisible(false);
			m_parent->setHandlesMovable(false);
			m_parent->m_parent->updateShape();
			m_parent->m_parent->signalItemPointsChange();
		}
		else {
			QGraphicsRectItem::mousePressEvent(event);
		}
	}

	//!
	//! Override event handler to listen to mouse move events
	//! \param event the graphics scene mouse event to listen to
	//!
	void BaseShapeItem::CurvePointHandle::ControlPointHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{
		QGraphicsRectItem::mouseMoveEvent(event);
		m_parent->m_parent->updateShape();
	}

	void BaseShapeItem::CurvePointHandle::ControlPointHandle::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{

		if( pos().manhattanLength() > 5) {

			// apply translation only, reset scaling & rotation
			qreal scaleItem = scale(); // <- thats the current scale factor of this item
				
			painter->rotate( -m_parent->m_parent->getRotation()); // unwind items inherited rotation
			qreal scaleView = painter->transform().m11(); // <- thats the scale factor inherited from the view

			painter->scale( scaleItem / scaleView, scaleItem / scaleView ); // apply the inverse of the views scale the reveal the original items scale
			QGraphicsRectItem::paint(painter, option, widget);
		}
	}

	//!
	//! Constructor of the TranslationHandle class.
	//!
	BaseShapeItem::TranslationHandle::TranslationHandle( BaseShapeItem* parent ) : 
		QGraphicsRectItem(-6, -6, 12, 12, parent), 
		m_parent(parent)
	{
		setBrush( Qt::gray );
		setFlag( ItemIsMovable);
		setFlag( ItemIgnoresTransformations);
		setVisible(false);
		setCursor(Qt::SizeAllCursor);
		setPos(100, 100);
	}

	void BaseShapeItem::TranslationHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{ 
		QPointF off = event->scenePos() - event->lastScenePos();
		QList<QGraphicsItem*> items = m_parent->scene()->selectedItems();

		foreach(QGraphicsItem* item, items) {
			BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);

			if(bs_item) {
				bs_item->setSavedPosition(bs_item->pos());
			}
		}
	}

	void BaseShapeItem::TranslationHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{ 
		QPointF off = event->scenePos() - event->lastScenePos();
		QList<QGraphicsItem*> items = m_parent->scene()->selectedItems();

		foreach(QGraphicsItem* item, items) {
			BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);

			if(bs_item) {
				bs_item->setPosition(bs_item->pos() + off);
				bs_item->setSavedDistance(bs_item->pos() - bs_item->getSavedPosition());
			}
		}
	}

	void BaseShapeItem::TranslationHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
	{ 
		m_parent->signalItemPositionChange();
	}

	BaseShapeItem::RotationHandle::RotationHandle( BaseShapeItem* parent) : 
		QGraphicsEllipseItem(-6, -6, 12, 12, parent), 
		m_parent(parent),
		m_angle(0.0f)
	{
		setBrush(QBrush(Qt::green));
		setFlag( ItemIsMovable );
		setFlag( ItemIgnoresTransformations);
		setVisible(false);
		setCursor(Qt::SizeHorCursor);
		setPos(150, 150);
	}

	void BaseShapeItem::RotationHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{ 
		QList<QGraphicsItem*> items = m_parent->scene()->selectedItems();

		foreach(QGraphicsItem* item, items) {
			BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);

			if(bs_item) {
				bs_item->setSavedRotation(bs_item->getRotation());
			}
		}
	}

	void BaseShapeItem::RotationHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{ 
		// update angle by cursor movement in scene coords
		m_angle += (event->scenePos() - event->lastScenePos()).x();

		QList<QGraphicsItem*> items = m_parent->scene()->selectedItems();

		foreach(QGraphicsItem* item, items) {
			BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);
			if(bs_item) {
				bs_item->setRotation(m_angle);
			}
		}
	}

	void BaseShapeItem::RotationHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
	{ 
		m_parent->signalItemRotationChange();
	}

	qreal BaseShapeItem::RotationHandle::getRotationValue()
	{
		return m_angle;
	}

	void BaseShapeItem::RotationHandle::setRotationValue( qreal angle )
	{
		m_angle = angle;
		this->setRotation(angle);
	}

	//!
	//! Constructor of the BaseShapeItem class.
	//!
	//! \param penColor The parent widget the created instance will be a child of.
	//! \param penWidth Extra widget options.
	//!
	BaseShapeItem::BaseShapeItem () : 
		QAbstractGraphicsShapeItem(),
		m_inCubicTransformMode(false),
		m_convexHullDirty(true),
		m_savedPosition(QPointF(0.0,0.0))
	{
		setFlags( ItemIsSelectable );
		
		m_rotateHandle = new RotationHandle(this);
		m_translateHandle = new TranslationHandle(this);

		// during creation, set item selected, but hide handles
		setSelected(true);
		showHandles(false);
	}

	//!
	//! Destructor of the PainterPanel class.
	//!
	//! Defined virtual to guarantee that the destructor of a derived class
	//! will be called if the instance of the derived class is saved in a
	//! variable of its parent class type.
	//!
	BaseShapeItem::~BaseShapeItem()
	{
		// delete extra handles of this item
		foreach( PointHandle* handle, m_points)
			delete handle;
		delete m_translateHandle;
		delete m_rotateHandle;
	}

	//! Enable the use of qgraphicsitem_cast with this item.
	int BaseShapeItem::type() 
	{ return BaseShapeItem::Type; }

	//! The item type is used to distinguish between the different types of items
	BaseShapeItem::ItemType BaseShapeItem::GetItemType() 
	{ return BASE; }

	//! Forget the last position provided by add-/updatePosition()
	//! \param emptyItem Set to true, if this operation creates an empty item, otherwise false
	void BaseShapeItem::skipLastPosition( bool& emptyItem ) 
	{
		int fuzi = type();
		if(type() == BaseShapeItem::BEZIER_CURVE || type() == BaseShapeItem::LINE || type() == BaseShapeItem::POLYGON || type() == BaseShapeItem::POLYLINE ) {	
			if (m_points.size() >= 1) {
				emptyItem = true;
				return;
			}
		}
		emptyItem = false;
		return;
	}

	//!
	//! Returns the bounding rectangle of the graphics item.
	//!
	//! \return The bounding rectangle of the graphics item.
	//!
	QRectF BaseShapeItem::boundingRect () const
	{
		static const qreal extra = 5;
		return m_shape.controlPointRect().adjusted(-extra, -extra, extra, extra);
	}

	//!
	//! Returns the shape of the item as QPainterPath.
	//!
	//! \param The shape of the item as QPainterPath.
	//!
	QPainterPath BaseShapeItem::shape () const
	{
		QPainterPath result;
		result.addPath(m_shape);
		return result;
	}

	void BaseShapeItem::calculateConvexHull( ) {

		//will be the result
		QPainterPath checkPath = this->m_shape;
		QPainterPath newPath;
		qreal tolerance = 10;

		if(checkPath.elementCount() >= 2) {

		//during the loop, p1 is the "previous" point, initially the first one
		QPointF p1 = checkPath.elementAt(0);

		//begin with a circle around the start point
			newPath.addEllipse(p1, tolerance, tolerance);

		//input now starts with the 2nd point (there was a takeFirst)
			for(int i = 1; i < checkPath.elementCount(); i++ ) {

			QPointF p2 = checkPath.elementAt(i);

			//offset = the distance vector from p1 to p2
			QPointF offset = QPointF(0.0, 0.0);

			// avoid division through zero
			if(p2 != p1) {
				offset = p2 - p1;
				//normalize offset to length of tolerance
				offset *= tolerance / sqrt(offset.x() * offset.x() + offset.y() * offset.y());
			}

			//"rotate" the offset vector 90 degrees to the left and right
			QPointF leftOffset = QPointF(-offset.y(),  offset.x());
			QPointF rightOffset = QPointF(offset.y(), -offset.x());
					
			//if (p1, p2) goes downwards, then left lies to the left and
			//right to the right of the source path segment
			QPointF left1 = p1 + leftOffset; 
			QPointF left2 = p2 + leftOffset;
			QPointF right1 = p1 + rightOffset;
			QPointF right2 = p2 + rightOffset;

			//rectangular connection from p1 to p2
			{
				QPainterPath p;
				p.moveTo(left1);
				p.lineTo(left2);
				p.lineTo(right2);
				p.lineTo(right1);
				p.lineTo(left1);
					newPath += p; //add this to the result path
			}

			//circle around p2
			{
				QPainterPath p;
				p.addEllipse(p2, tolerance, tolerance);
					newPath += p; //add this to the result path
			}
			p1 = p2;
		}

			m_convexHullShape = newPath;
	}
	}

	bool BaseShapeItem::collidesWithPath ( const QPainterPath &path, Qt::ItemSelectionMode mode ) const
	{
		return m_convexHullShape.intersects(path);
	}

	//! Call this function to update the shape of the item.
	//! This function updates the position of the translation and rotation handle.
	void BaseShapeItem::updateShape()
	{
		QPointF center = m_shape.boundingRect().center();
		setTransformOriginPoint( center );
		m_rotateHandle->setPos( center );
		m_translateHandle->setPos(center.x(), center.y()-20);
	}

	//! Paint the item with the specified pen and brush
	//! Use a yellow pen when the item is selected
	void BaseShapeItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		QPen painterPen = pen();
		QBrush painterBrush = brush();

		if( s_useLUT ) {

			// set item color according to LUT
			painterPen.setColor( lut( painterPen.color().red() ));
			painterBrush.setColor( lut( painterPen.color().red() ));

			// draw bounding box if item is selected
			if( isSelected() ) {
				QPen rectPen;
				rectPen.setColor( QColor( 255, 255, 0, 100));
				rectPen.color().setAlpha(100);
				painter->setPen(rectPen);
				painter->drawRect( boundingRect() );
			}
		}

		// make item appear in transparent yellow if LUT isn't used
		else if (isSelected()) {
			painterPen.setColor( QColor( 255, 255, 0, 100));
			painterPen.color().setAlpha(100);
		}

		// draw item with selected pen and brush
		painter->setPen( painterPen);
		painter->setBrush( painterBrush );
		painter->drawPath( m_shape );


//#ifdef _DEBUG
//		painterPen.setColor( QColor( 255, 0, 0, 100));
//		painter->setPen(painterPen);
//		painter->drawRect( boundingRect() );
//		
//		painterPen.setColor( QColor( 0, 0, 255, 100));
//		painter->setPen(painterPen);
//		painter->drawPath( m_convexHullShape );
//#endif
	}

	void BaseShapeItem::setCubicTransformMode( bool value )
	{
		m_inCubicTransformMode = value;
	}

	bool BaseShapeItem::getCubicTransformMode()
	{
		return m_inCubicTransformMode;
	}


	QString BaseShapeItem::getDescriptionText()
	{
		BaseShapeItem::ItemType itemType = GetItemType() ;
		switch( itemType )
		{
		case BASE:			return "Basic";
		case PAINT:			return "Paint";
		case LINE:			return "Line";
		case RECT:			return "Rectangle";
		case CIRCLE:		return "Circle";
		case ELLIPSE:		return "Ellipse";
		case POLYGON:		return "Polygon";
		case BEZIER_CURVE:	return "Bezier Curve";
		case ERASE:			return "Eraser";
		}
		return "Unknown";
	}

	//! React on changes of the item
	QVariant BaseShapeItem::itemChange(GraphicsItemChange change, const QVariant &value)
	{
		if( change == ItemSelectedHasChanged)
			this->showHandles( isSelected());
		return QGraphicsItem::itemChange( change, value);
	}

	//! Show/Hide handles of the item
	void BaseShapeItem::showHandles( bool show /** = true */ )
	{
		foreach( PointHandle *handle, m_points )
			handle->setVisible(show);
		m_rotateHandle->setVisible(show);
		m_translateHandle->setVisible(show);
	}

	QString BaseShapeItem::getName()
	{
		return m_name;
	}

	void BaseShapeItem::setName( const QString& name )
	{
		m_name = name;
	}

	qreal BaseShapeItem::getRotation()
	{
		return QGraphicsItem::rotation();
	}

	void BaseShapeItem::setRotation( qreal angle )
	{
		m_rotateHandle->setRotationValue(angle);
		QGraphicsItem::setRotation(angle);
	}

	QPointF BaseShapeItem::getPosition()
	{
		return pos();
	}

	void BaseShapeItem::setPosition( QPointF position )
	{
		setPos(position);
	}

	QList<QPointF> BaseShapeItem::getPoints()
	{
		QList<QPointF> result;
		foreach( PointHandle* p, m_points) {
			result.push_back( p->pos() );
		}
		return result;
	}

	void BaseShapeItem::setPoints( const QList<QPointF>& points )
	{
		prepareGeometryChange();

		if( m_points.size() <= points.size() )
		{
			for( int i=0; i<m_points.size(); i++ ) {
				m_points.at(i)->setPos( points.at(i) );
			}
		}
		return;
	}

	void BaseShapeItem::setControlPointsPosition( QPointF scenePos )
	{
		if( !m_points.isEmpty() ) {

			CurvePointHandle* cph = dynamic_cast<CurvePointHandle*>(m_points.back());
			if( cph ) {

				QPointF offset = scenePos - cph->scenePos();

				cph->setHandleOnePosition( offset * -1.0f );
				cph->setHandleTwoPosition( offset );

				if( offset.manhattanLength() > 5) {
					cph->setHandlesMovable(true);
					cph->setHandlesVisible(true);
					cph->setHandlesSelected(true);
				}
				updateShape();
			}
		}
	}

	const QPainterPath& BaseShapeItem::getPainterPath()
	{
		return m_shape;
	}

	void BaseShapeItem::setPainterPath( const QPainterPath& path )
	{
		m_shape = path;
	}

	void BaseShapeItem::signalItemPositionChange()
	{
		emit itemPositionChanged();
		calculateConvexHull();
	}

	void BaseShapeItem::signalItemRotationChange()
	{
		emit itemRotationChanged();
		calculateConvexHull();
	}

	void BaseShapeItem::signalItemPointsChange()
	{
		emit itemPointsChanged();
		calculateConvexHull();
	}

	void BaseShapeItem::setUseLUT( bool val )
	{
		s_useLUT = val;
	}

	const QColor BaseShapeItem::lut( int i ) const
	{
		if (i == 0 )
			return	qRgb( 0, 0, 0);
		else if( i < 128)
			return	qRgb( 2*i, 2*i, 255 );
		else
			return	qRgb( 255, 511-2*i, 511-2*i);
	}

	//! Constructor of the PaintItem class.
	PaintItem::PaintItem( QPointF startPos ) : 
		BaseShapeItem()
	{
		m_shape.moveTo( startPos );
	}

	//! Handle position updates
	//! \param pos The position to append to the shape
	void PaintItem::updatePosition( QPointF pos ) 
	{
		prepareGeometryChange();

		//m_shape.lineTo( pos );

		if((m_shape.currentPosition()-pos).manhattanLength() > 0.5f ){ // need at least half a pixel difference in x and y direction!
			m_shape.lineTo( pos );
		}

//#ifdef _DEBUG
//		// approx. of convex hull
//		QPointF prevPos = m_shape.currentPosition();
//		qreal tol = 5.0f;
//		qreal tolDbl = 10.0f;
//		m_shape.addRect( QRectF( pos.x()-tol, pos.y()-tol, tolDbl, tolDbl) );
//		m_shape.addRect( QRectF( pos, prevPos) );
//		m_shape.moveTo(pos);
//#endif

		updateShape();
	}

	//! Append position to the shape
	//! \param pos The position to append to the shape
	void PaintItem::addPosition( QPointF pos )
	{
		// this starts a new creation session, hide handles again
		showHandles(false);

		prepareGeometryChange();
		m_shape.moveTo( pos );
		updateShape();
	}

	//! Call this function to update the shape of the item.
	void PaintItem::updateShape()
	{
		BaseShapeItem::updateShape();
	}

	bool PaintItem::collidesWithPath ( const QPainterPath &path, Qt::ItemSelectionMode mode ) const
	{
		if( mode == Qt::ItemSelectionMode::IntersectsItemShape ) {
			qreal tol = 5.0f;
			qreal tolDbl = 2.0f * tol;

			int nElements = m_shape.elementCount();
			if( nElements > 0 ) {

				QPointF prevPos = m_shape.elementAt(0);
				for( int i=1; i<m_shape.elementCount(); i++ ) {
					QPainterPath::Element el = m_shape.elementAt(i);
					if( el.isLineTo() ) {
						if( path.intersects( QRectF( el.x-tol, el.y-tol, tolDbl, tolDbl)) || 
							path.intersects( QRectF( el, prevPos).normalized() )) {
							return true;
						}
					}
					prevPos = el;
				}
			}
			return false;
		} else {
			QGraphicsItem::collidesWithPath(path,mode);
		}
	}


	//! Get the type of this item
	BaseShapeItem::ItemType PaintItem::GetItemType() { return PAINT; }

	void PaintItem::erase( QPainterPath eraserShape )
	{
		qreal tol = 2.0f;
		qreal tolDbl = 2.0f * tol;
		QPainterPath m_newShape;

		int nElements = m_shape.elementCount();
		if( nElements > 0 ) {

			QPointF prevPos = m_shape.elementAt(0);
			m_newShape.moveTo(prevPos);

			for( int i=1; i<m_shape.elementCount(); i++ ) {

				QPainterPath::Element el = m_shape.elementAt(i);

				// only add line-to elements to new shape that lie outside of eraserShape
				// considers that paint elements consists only of move-to and line-to elements
				if( el.isMoveTo() ){
					m_newShape.moveTo(el);
				} else if( el.isLineTo() ) {

					if( eraserShape.intersects( QRectF( el.x-tol, el.y-tol, tolDbl, tolDbl)) || 
						eraserShape.intersects( QRectF( el, prevPos).normalized() )) 
					{
						m_newShape.moveTo( el );
					} else {
						m_newShape.lineTo(el);
					}
				}
				prevPos = el;
			}
		}

		m_shape = m_newShape; // copy result
	}








	//! Constructor of the EraseItem class.
	EraseItem::EraseItem( QPointF startPos ) :
		BaseShapeItem(),
		m_size( 5.0 )
	{
		updateShape(startPos);
	}

	//! Handle position updates
	//! \param pos The position to append to the shape
	void EraseItem::updatePosition( QPointF pos ) 
	{
		prepareGeometryChange();
		updateShape(pos);

		QList<QGraphicsItem*> items = scene()->selectedItems();
		if( items.size() == 0)
			items = scene()->items( m_shape, Qt::IntersectsItemBoundingRect );

		foreach(QGraphicsItem* item, items) {
			BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);

			if(bs_item && bs_item->GetItemType() == PAINT) {
				static_cast<PaintItem*>(bs_item)->erase( m_shape);
			}
		}
	}

	//! Append position to the shape
	//! \param pos The position to append to the shape
	void EraseItem::addPosition( QPointF pos )
	{
	}

	//! Get the type of this item
	BaseShapeItem::ItemType EraseItem::GetItemType() { return ERASE; }

	void EraseItem::updateShape( QPointF pos )
	{
		m_shape = QPainterPath(); //clear
		m_shape.addEllipse( pos, m_size, m_size);
	}

	void EraseItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
	{
		QPen painterPen = pen();
		QBrush painterBrush = brush();

		painterPen.setColor( QColor( 255, 0, 0));
		painterBrush.setStyle(Qt::NoBrush);

		// draw item with selected pen and brush
		painter->setPen( painterPen);
		painter->setBrush( painterBrush );
		painter->drawPath( m_shape );

	}

	//! Constructor of the LineItem class.
	LineItem::LineItem( QPointF startPos ) : 
		BaseShapeItem()
	{
		m_points.push_back( new PointHandle( startPos, this));
		m_points.push_back( new PointHandle( startPos, this));
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the line
	void LineItem::updatePosition( QPointF pos )
	{
		m_points.at(1)->setPos(pos);
		updateShape();
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the line
	void LineItem::addPosition( QPointF pos )
	{
		updatePosition(pos);
	}

	//! Call this function to update the shape of the item.
	void LineItem::updateShape()
	{
		prepareGeometryChange();
		m_shape = QPainterPath(); // clear
		m_shape.moveTo( ( m_points.at(0)->pos()));
		m_shape.lineTo( ( m_points.at(1)->pos()));
		BaseShapeItem::updateShape();
	}

	//! Get the type of this item
	BaseShapeItem::ItemType LineItem::GetItemType() { return LINE; }


	//! Constructor of the RectItem class.
	RectItem::RectItem( QPointF startPos ) :
		BaseShapeItem()
	{
		m_points.push_back( new PointHandle( startPos, this));
		m_points.push_back( new PointHandle( startPos, this));
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the rectangle
	void RectItem::updatePosition( QPointF pos )
	{
		m_points.at(1)->setPos(pos);
		updateShape();
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the rectangle
	void RectItem::addPosition( QPointF pos )
	{
		updatePosition(pos);
	}

	//! Call this function to update the shape of the item.
	void RectItem::updateShape()
	{
		prepareGeometryChange();
		m_shape = QPainterPath(); // clear
		m_shape.addRect( QRectF(( m_points.at(0)->pos()), ( m_points.at(1)->pos())).normalized()); // use normalized to get a valid (non-negative) rect
		BaseShapeItem::updateShape();
	}

	//! Get the type of this item
	BaseShapeItem::ItemType RectItem::GetItemType() { return RECT; }



	//! Constructor of the CircleItem class.
	CircleItem::CircleItem( QPointF startPos ) : 
		BaseShapeItem()
	{
		m_points.push_back( new PointHandle( startPos, this));
		m_points.push_back( new PointHandle( startPos, this));
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the circle
	void CircleItem::updatePosition( QPointF pos )
	{
		m_points.at(1)->setPos(pos);
		updateShape();
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the circle
	void CircleItem::addPosition( QPointF pos )
	{
		updatePosition(pos);
	}

	void CircleItem::calculateConvexHull( ) {

		//will be the result
		QPainterPath newPath;
		
		QPointF tolerance = QPointF(5.0, 5.0);
		QPointF x1 = m_points.at(0)->pos();
		QPointF x2 = m_points.at(1)->pos();
		QPointF x_kl1 = x1 - tolerance;
		QPointF x_kl2 = x2 + tolerance;
		QPointF x_bg1 = x1 + tolerance;
		QPointF x_bg2 = x2 - tolerance;

		QPointF c_kl = .5*x_kl1 +.5*x_kl2;
		QPointF c_bg = .5*x_bg1 +.5*x_bg2;

		float r_kl = .5 * sqrt( (x_kl1.x()-x_kl2.x())*(x_kl1.x()-x_kl2.x()) + (x_kl1.y()-x_kl2.y())*(x_kl1.y()-x_kl2.y()));
		float r_bg = .5 * sqrt( (x_bg1.x()-x_bg2.x())*(x_bg1.x()-x_bg2.x()) + (x_bg1.y()-x_bg2.y())*(x_bg1.y()-x_bg2.y()));
		QRectF rect_kl = QRectF( c_kl.x()-r_kl, c_kl.y()-r_kl, 2*r_kl, 2*r_kl );
		QRectF rect_bg = QRectF( c_bg.x()-r_bg, c_bg.y()-r_bg, 2*r_bg, 2*r_bg );

		// calculate rect of circle
		newPath.addEllipse( rect_kl );
		newPath.addEllipse( rect_bg );
		m_convexHullShape = newPath;	
	}

	//! Call this function to update the shape of the item.
	void CircleItem::updateShape()
	{
		prepareGeometryChange();

		m_shape = QPainterPath(); // clear

		QPointF x1 = m_points.at(0)->pos();
		QPointF x2 = m_points.at(1)->pos();
		QPointF c = .5*x1 +.5*x2;

		float r = .5 * sqrt( (x1.x()-x2.x())*(x1.x()-x2.x()) + (x1.y()-x2.y())*(x1.y()-x2.y()));
		QRectF rect = QRectF( c.x()-r, c.y()-r, 2*r, 2*r );

		// calculate rect of circle
		m_shape.addEllipse( rect );
		BaseShapeItem::updateShape();
	}

	//! Get the type of this item
	BaseShapeItem::ItemType CircleItem::GetItemType() { return CIRCLE; }


	//! Constructor of the EllipseItem class.
	EllipseItem::EllipseItem( QPointF startPos ) : 
		BaseShapeItem()
	{
		m_points.push_back( new PointHandle( startPos, this));
		m_points.push_back( new PointHandle( startPos, this));
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the ellipse
	void EllipseItem::updatePosition( QPointF pos )
	{
		m_points.at(1)->setPos(pos);
		updateShape();
		calculateConvexHull();
	}

	//! Update the second position of the item
	//! \param pos The new second position of the ellipse
	void EllipseItem::addPosition( QPointF pos )
	{
		updatePosition(pos);
	}

	void EllipseItem::calculateConvexHull( ) {

		//will be the result
		QPainterPath newPath;

		QPointF tolerance = QPointF(5.0, 5.0);
		QPointF x1 = m_points.at(0)->pos();
		QPointF x2 = m_points.at(1)->pos();
		QPointF x_kl1 = x1 - tolerance;
		QPointF x_kl2 = x2 + tolerance;
		QPointF x_bg1 = x1 + tolerance;
		QPointF x_bg2 = x2 - tolerance;

		QRectF rect_kl = QRectF(( x_kl1), ( x_kl2)).normalized();
		QRectF rect_bg = QRectF(( x_bg1), ( x_bg2)).normalized();

		// calculate rect of circle
		newPath.addEllipse( rect_kl );
		newPath.addEllipse( rect_bg );
		m_convexHullShape = newPath;	
	}

	//! Call this function to update the shape of the item.
	void EllipseItem::updateShape()
	{
		prepareGeometryChange();
		QRectF rect = QRectF(( m_points.at(0)->pos()), ( m_points.at(1)->pos())).normalized();
		m_shape = QPainterPath(); // clear
		m_shape.addEllipse( rect );
		BaseShapeItem::updateShape();
	}

	//! Get the type of this item
	BaseShapeItem::ItemType EllipseItem::GetItemType() { return ELLIPSE; }


	//! Constructor of the PolyLineItem class.
	PolyLineItem::PolyLineItem( QPointF startPos ) : 
	BaseShapeItem()
	{
		m_points.push_back( new PointHandle( startPos, this));
		calculateConvexHull();
	}

	//! Adds a new position to the polyline
	//! \param pos The new position to append to the polyline
	void PolyLineItem::addPosition( QPointF pos )
	{
		m_points.push_back( new PointHandle( pos, this));
		updateShape();
		calculateConvexHull();
	}

	//! Updates the last added position of the polyline
	//! \param pos The new position of the last position of the polyline
	void PolyLineItem::updatePosition( QPointF pos )
	{
		m_points.back()->setPos(pos);
		updateShape();
		calculateConvexHull();
	}

	//! Skip the last position of the polyline
	//! \param emptyItem Set to true, if this operation creates an empty item, otherwise false
	void PolyLineItem::skipLastPosition( bool& emptyItem )
	{
			delete m_points.back();
			m_points.pop_back();
			emptyItem = m_points.size() < 2;
			updateShape();
			if(!emptyItem)
				calculateConvexHull();
	}

	//! Call this function to update the shape of the item.
	void PolyLineItem::updateShape()
	{
		// calculate new geometry
		prepareGeometryChange();
		QPointF start = ( m_points.at(0)->pos());
		m_shape = QPainterPath(); // clear
		m_shape.moveTo( start);
		for( int i=1; i<m_points.size(); i++)
		{
			QPointF end = ( m_points.at(i)->pos());
			m_shape.lineTo(end);
		}
		BaseShapeItem::updateShape();
	}

	//! Get the type of this item
	BaseShapeItem::ItemType PolyLineItem::GetItemType() { return POLYLINE; }


	//! Constructor of the PolygonItem class.
	PolygonItem::PolygonItem( QPointF startPos ) : 
	BezierCurveItem( startPos )
	{}

	//! Call this function to update the shape of the item.
	void PolygonItem::updateShape()
	{
		BezierCurveItem::updateShape();
		CurvePointHandle* curvePointHandleNow = dynamic_cast<CurvePointHandle*>(m_points.back());
		CurvePointHandle* curvePointHandleNext = dynamic_cast<CurvePointHandle*>(m_points.front());
		QPointF handle2_now = mapFromScene(curvePointHandleNow->getHandleTwoScenePosition());
		QPointF handle1_next = mapFromScene(curvePointHandleNext->getHandleOneScenePosition());
		m_shape.cubicTo( handle2_now, handle1_next, m_points.front()->pos());
		calculateConvexHull();
	}

	//! Get the type of this item
	BaseShapeItem::ItemType PolygonItem::GetItemType() { return POLYGON; }

	// BEZIER CURVE ITEM:

	//! Constructor of the BezierCurveItem class.
	BezierCurveItem::BezierCurveItem( QPointF startPos ) : 
	BaseShapeItem()
	{
		m_points.push_back( new CurvePointHandle( startPos, this, Qt::blue, Qt::red ) );
		m_inCubicTransformMode = true;
		calculateConvexHull();
	};

	//! Destructor of the BezierCurveItem class.
	BezierCurveItem::~BezierCurveItem()
	{}

	//! Adds a new position to the curve 
	//! \param pos The new position to append to the curve
	void BezierCurveItem::addPosition( QPointF pos )
	{
		m_points.push_back( new CurvePointHandle( pos, this, Qt::blue, Qt::red ) );
		updateShape();
		calculateConvexHull();
	}

	//! Updates the last added position of the curve
	//! \param pos The new position of the last position of the curve
	void BezierCurveItem::updatePosition( QPointF pos )
	{
		m_points.back()->setPos(pos);
		updateShape();
		calculateConvexHull();
	}

	//! Skip the last position of the curve
	//! \param emptyItem Set to true, if this operation creates an empty item, otherwise false
	void BezierCurveItem::skipLastPosition( bool& emptyItem )
	{
			delete m_points.back();
			m_points.pop_back();
			emptyItem = m_points.size() < 2;
			updateShape();
			if(!emptyItem)
				calculateConvexHull();
	}

	void BezierCurveItem::insertCurvePointAtPosition ( QPointF position, CurvePointHandle* newHandle )
	{	
		for(int i = 0; i < m_points.length(); i++) {

			QPointF searchPos = m_points.at(i)->scenePos();

			if (searchPos == position) {

				PointHandle* old = m_points.at(i);
				CurvePointHandle* oldHandle = dynamic_cast<CurvePointHandle*>(old);

				QPointF pos1 = oldHandle->getHandleOnePosition();
				QPointF pos2 = oldHandle->getHandleTwoPosition();
				newHandle->setHandleOnePosition(pos1);
				newHandle->setHandleTwoPosition(pos2);

				QPointF diff1 = newHandle->getHandleOneScenePosition() - newHandle->scenePos();
				QPointF diff2 = newHandle->getHandleTwoScenePosition() - newHandle->scenePos();
				qreal diff = (diff1.manhattanLength()+diff2.manhattanLength())/2;
				bool choice = false;

				if(diff > 5)
					choice = true;
				newHandle->setHandlesMovable(choice);
				newHandle->setHandlesVisible(choice);
		
				QPointF nothing = QPointF(0.0, 0.0);
				oldHandle->setHandleOnePosition(nothing);
				oldHandle->setHandleTwoPosition(nothing);
				oldHandle->setHandlesVisible(false);
				oldHandle->setHandlesMovable(false);

				m_points.insert(i, newHandle);
				return;
			}
		}
		calculateConvexHull();
	}

	//! Call this function to update the shape of the item.
	void BezierCurveItem::updateShape()
	{
		prepareGeometryChange();
		if(!m_points.isEmpty()) {

			QPointF startPos = m_points.at(0)->pos();

			m_shape = QPainterPath(); // clear
			m_shape.moveTo( startPos );
			m_controlShape = QPainterPath();
			m_controlShape.moveTo( startPos );

			if(m_points.length() > 0 )
			{
				for ( int i=0; i< m_points.size(); i++)
				{
					QPointF handle1_now;
					QPointF handle2_now;
					QPointF handle1_next;
					QPointF handle2_next;
					QPointF curve_now;
					QPointF curve_next;
					
					CurvePointHandle* curvePointHandleNow = dynamic_cast<CurvePointHandle*>(m_points.at(i));
					CurvePointHandle* curvePointHandleNext;

					if(i < (m_points.size()-1) && m_points.length() > 1)
					{
						curvePointHandleNext = dynamic_cast<CurvePointHandle*>(m_points.at(i+1));
						curve_now = m_points.at(i)->pos();
						handle1_now = mapFromScene(curvePointHandleNow->getHandleOneScenePosition());
						handle2_now = mapFromScene(curvePointHandleNow->getHandleTwoScenePosition());
						curve_next = m_points.at(i+1)->pos();
						handle1_next = mapFromScene(curvePointHandleNext->getHandleOneScenePosition());
						handle2_next = mapFromScene(curvePointHandleNext->getHandleTwoScenePosition());

						m_shape.cubicTo( handle2_now, handle1_next, curve_next);
						m_controlShape.moveTo( curve_now );
						m_controlShape.lineTo( handle1_now );
						m_controlShape.moveTo( curve_now );
						m_controlShape.lineTo( handle2_now );
						m_controlShape.moveTo( curve_next );
						m_controlShape.lineTo( handle1_next );
						m_controlShape.moveTo( curve_next );
						m_controlShape.lineTo( handle2_next );
					}

					else if ( curvePointHandleNow )
					{
						curve_now = m_points.at(i)->pos();
						handle1_now = mapFromScene(curvePointHandleNow->getHandleOneScenePosition());
						handle2_now = mapFromScene(curvePointHandleNow->getHandleTwoScenePosition());

						m_controlShape.moveTo( curve_now );
						m_controlShape.lineTo( handle1_now );
						m_controlShape.moveTo( curve_now );
						m_controlShape.lineTo( handle2_now );
					}
				}
			}
		}

		//foreach( PointHandle *handle, m_points )
		//	handle->setVisible(true);

		BaseShapeItem::updateShape();
	}

	//! Sets the input list as new positions for the curve handles
	void BezierCurveItem::setHandlePoints( const QList<QPointF>& list )
	{
		for(int i = 0; i < m_points.length(); i++)
		{
			PointHandle* ph = m_points.at(i);
			CurvePointHandle* cph = dynamic_cast<CurvePointHandle*>(ph);
			cph->setHandleOnePosition( list.at(i*2) );
			cph->setHandleTwoPosition( list.at((i*2)+1) );
		}
		return;
	}

	//! Returns a list of positions for the curve handles
	QList<QPointF> BezierCurveItem::getHandlePoints( )
	{
		QList<QPointF> result;
		for(int i = 0; i < m_points.length(); i++)
		{
			PointHandle* ph = m_points.at(i);
			CurvePointHandle* cph = dynamic_cast<CurvePointHandle*>(ph);
			result.push_back(cph->getHandleOnePosition());
			result.push_back(cph->getHandleTwoPosition());
		}
		return result;
	}

	//! Returns a list of positions for the curve positions
	QList<QPointF> BezierCurveItem::getPoints( )
	{
		QList<QPointF> result;
		for(int i = 0; i < m_points.length(); i++)
		{

			result.push_back(m_points.at(i)->pos());
		}
		return result;
	}

	//! Get the type of this item
	BaseShapeItem::ItemType BezierCurveItem::GetItemType() { return BEZIER_CURVE; }

	//! Paint the curveItem with the given pen and brush
	//! If selected, also the control polygon will be drawn
	void BezierCurveItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		if (isSelected() )
		{
			painter->setPen(QPen( QColor(192,192,192,100), 2) );
			painter->drawPath( m_controlShape );
		}
		BaseShapeItem::paint(painter, option, widget);
	}

	void BezierCurveItem::removeCurvePoint( int i )
	{
		PointHandle* handle = m_points.at(i);
		CurvePointHandle* cpHandle = dynamic_cast<CurvePointHandle*>(handle);
		m_points.removeAt(i);
		delete cpHandle;
		updateShape();
		calculateConvexHull();
		return;
	}

} // end namespace Frapper