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
//! \file "CurveEditorGraphicsView.cpp"
//! \brief Implementation file for CurveEditorGraphicsView class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       28.07.2009 (last updated)
//!

#include "CurveEditorGraphicsView.h"
#include "SegmentGraphicsItem.h"
#include "KeyGraphicsItem.h"
#include "NumberParameter.h"
#include <QApplication>
#include <QPainter>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QWheelEvent>

#define scenescale 5
#define collumns 10 


namespace Frapper {

Q_DECLARE_METATYPE(AbstractParameter *)


///
/// Constructors and Destructors
///


//!
//! Constructor of the CurveEditorGraphicsView class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
CurveEditorGraphicsView::CurveEditorGraphicsView ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    BaseGraphicsView(parent),
    m_timeline(new TimelineGraphicsItem(height()*scenescale)),
    m_maxLength(1),
	m_scaleFactor(1.0f, 1.0f),
	m_renderWindowSize(1.0f, 1.0f),
    m_timelinePos(0.0f),
	m_inFrame(0),
	m_outFrame(1),
	m_showEnabledCurves(false)
{
    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    graphicsScene->setSceneRect(0, 0, width()*scenescale, height()*scenescale);
    setScene(graphicsScene);

	centerOn(0,scene()->sceneRect().height()/2);

    //setCacheMode(CacheBackground);
	setViewportUpdateMode(FullViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorViewCenter);
    setResizeAnchor(AnchorViewCenter);
}


//!
//! Destructor of the CurveEditorGraphicsView class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CurveEditorGraphicsView::~CurveEditorGraphicsView ()
{
}


///
/// Public Functions
///


//!
//! Displays the curves of the given parameter group in the curve editor
//! widget.
//!
//! \param parameterGroup The parameter group whose animated parameters to display in the curve editor widget.
//!
void CurveEditorGraphicsView::showCurves ( QList<QTreeWidgetItem *> *selectedItems )
{
    m_numberParametersToDraw.clear();
    
    m_maxLength = 1;

    foreach (QTreeWidgetItem *item, *selectedItems) {
        drawCurves(item->data(1,0).value<AbstractParameter *>());
    }
}

//!
//! Displays the curves of the given parameter group in the curve editor
//! widget.
//!
//! \param parameterGroup The parameter group whose animated parameters to display in the curve editor widget.
//!
inline void CurveEditorGraphicsView::drawCurves ( AbstractParameter *parameter )
{
    // iterate over the list of parameters contained in the given parameter group
    NumberParameter *numberParameter;
    float length;

    if (parameter->isGroup()) {
        const AbstractParameter::List& parameters = static_cast<ParameterGroup *>(parameter)->getParameterList();
		foreach (AbstractParameter *parameter, parameters) {
            // recursively parse nested parameter groups and parameters
			if ((m_showEnabledCurves && !parameter->isEnabled()))
				continue;
            drawCurves(parameter);
		}
	}
    else {
		if (m_showEnabledCurves && !parameter->isEnabled())
			return;
        numberParameter = dynamic_cast<NumberParameter *>(parameter);
		if (numberParameter && numberParameter->isAnimated() && numberParameter->getSize() == 1) {
			length = numberParameter->getLastKeyPos()/numberParameter->getTimeStepSize();
            if (length > m_maxLength) 
				m_maxLength = length;
			m_numberParametersToDraw.append(numberParameter);
        }
    }
}


//!
//! The overwritten the event handler for the drag move event.
//!
//! \param event The description of the drag move event.
//!
void CurveEditorGraphicsView::mouseMoveEvent ( QMouseEvent * event )
{
	if (qApp->focusWidget() != this) {
		m_viewMode = false;
		setCursor(Qt::ArrowCursor);
		setFocus(Qt::MouseFocusReason);
	}

	if (m_viewMode) {
		if (event->buttons() & Qt::RightButton) {
			// calculate the difference
			const int dx = event->x() - m_lastPosition.x();
			const int dy = event->y() - m_lastPosition.y();
			const float scaleFactorX = matrix().m11();
			const float scaleFactorY = matrix().m22();
			if ( (scaleFactorX > MIN_SCALE && scaleFactorX < MAX_SCALE) || 
				 (scaleFactorY > MIN_SCALE && scaleFactorY < MAX_SCALE)) {
				if (abs(dx) > abs(dy))
					(dx > 0) ? scale(1.01, 1.0) : scale(0.99, 1.0);
				else
					(dy < 0) ? scale(1.0, 1.01) : scale(1.0, 0.99);
				m_lastPosition = event->pos();
			}
		}
	}

	QGraphicsView::mouseMoveEvent(event);

	if (event->buttons() & Qt::LeftButton) {
		emit drag();
		emitSelectedKeyTypeChanged();
	}
}


//!
//! Event handler for mouse wheel events.
//!
//! \param event The description of the mouse wheel event.
//!
void CurveEditorGraphicsView::wheelEvent ( QWheelEvent *event )
{
	BaseGraphicsView::wheelEvent(event);
	if (event->delta() != 0)
		scaleSceneItems(matrix().m11());
}


//!
//! Event handler for mouse press events.
//!
//! \param event The description of the mouse event.
//!
void CurveEditorGraphicsView::mousePressEvent ( QMouseEvent *event )
{
	BaseGraphicsView::mousePressEvent(event);
	emitSelectedKeyTypeChanged();
}


//!
//! The overwritten the event handler for resize events.
//! Adds scene resizing and redrawing.
//!
//! \param event The description of the key event.
//!
void CurveEditorGraphicsView::resizeEvent ( QResizeEvent *event )
{
	//const QPointF oldPos = this->sceneRect().center();
	scene()->setSceneRect(0, 0, width()*scenescale, height()*scenescale);
    buildScene();
	centerOn(0,scene()->sceneRect().height()/2);
    QGraphicsView::resizeEvent(event);
}

//!
//! The overwritten the event handler for background drawing events.
//! Adds coordinate system to the background.
//!
//! \param painter The QT painter object.
//! \param rect The drawing region painter object.
//!
void CurveEditorGraphicsView::drawBackground (QPainter *painter, const QRectF &rect)
{
	const float scaleX = 1.0f/matrix().m11();
	const float scaleY = 1.0f/matrix().m22();
	const float sceneScale = sceneRect().height()/2.0;
	const float rectScale = rect.height() / collumns;
	const QTransform transform = painter->transform();
    float iRectScale, scaleText;
	QPoint up, down;
    QVector<QLineF> lines;

	m_renderWindowSize = rect.size();
	m_scaleFactor.setHeight(rect.size().height()/collumns/scaleX);
	
	painter->setBrush(Qt::gray);
	painter->drawRect(sceneRect());
	painter->scale(scaleX, scaleY);

	painter->setPen(QPen(Qt::black, scaleY, Qt::SolidLine));
    for (unsigned int i = 0; i < collumns; ++i) {
		scaleText = scaleY * i;
		iRectScale = rectScale * i;
		lines.append(QLineF( QPointF(rect.left(), sceneScale + iRectScale), QPointF(rect.right(), sceneScale + iRectScale) ));
		lines.append(QLineF( QPointF(rect.left(), sceneScale - iRectScale), QPointF(rect.right(), sceneScale - iRectScale) ));

		painter->drawText(QPointF(rect.left()/scaleX+10, (sceneScale + iRectScale)/scaleY-3), QString::number(-scaleText, 'g', 3));
		painter->drawText(QPointF(rect.left()/scaleX+10, (sceneScale - iRectScale)/scaleY-3), QString::number( scaleText, 'g', 3));
		painter->drawText(QPointF(rect.right()/scaleX-40, (sceneScale + iRectScale)/scaleY-3), QString::number(-scaleText, 'g', 3));
		painter->drawText(QPointF(rect.right()/scaleX-40, (sceneScale - iRectScale)/scaleY-3), QString::number( scaleText, 'g', 3));
    }

	painter->setTransform(transform);
	painter->setPen(QPen(Qt::lightGray, scaleY, Qt::SolidLine));
    painter->drawLines(lines);
    lines.clear();

    lines.push_back(QLineF( QPointF(0.f, sceneScale), QPointF(sceneRect().width(), sceneScale)));
    lines.push_back(QLineF( QPointF(0.f, 0.f), QPointF(0.f, sceneRect().height())));
    painter->setPen(QPen(Qt::black, scaleY, Qt::SolidLine));
    painter->drawLines(lines);
}


///
/// Public Slots
///


//!
//! Resets the network graphics view's matrix.
//!
void CurveEditorGraphicsView::homeView ()
{
    setScale(1);
	QRectF boundingRect = scene()->itemsBoundingRect();
    centerOn(boundingRect.left() + boundingRect.width() / 2.0f, boundingRect.top() + boundingRect.height() / 2.0f);

	scaleSceneItems(matrix().m11());
}


//!
//! Changes the viewing transformation so that all items are visible at maximum
//! zoom level.
//!
void CurveEditorGraphicsView::frameAll ()
{
    frame(scene()->itemsBoundingRect());
	scaleSceneItems(matrix().m11());
}


//!
//! Changes the viewing transformation so that the selected items are visible
//! at maximum zoom level.
//!
void CurveEditorGraphicsView::frameSelected ()
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

	scaleSceneItems(matrix().m11());
}


//!
//! Toggle to show only enabled parameters
//!
//! \param visible Flag to control whether to show or hide enabled parameters.
//!
void CurveEditorGraphicsView::toggleShowEnabledOnly( bool enabled )
{
	m_showEnabledCurves = enabled;
}


//!
//! Builds the scene containing the segment ande key widgets
//!
void CurveEditorGraphicsView::buildScene ()
{
    // local variable definitions
    float keyPosX, keyPosY;
	QVariant data;
	QColor curveColor;
	Key::KeyType preKeyType = Key::KT_Linear;
    unsigned int index = 0;
    KeyGraphicsItem *keyItem = 0;
    KeyGraphicsItem *preKey = 0;
	TangentGraphicsItem *tangentItem = 0;
	TangentGraphicsItem *preTan = 0;
	SegmentGraphicsItem *segmentItem = 0;
	//const QList<Key> *keys;

    // offset and scale for curve drawing
    const float yOffset = scene()->height() / 2.0f;
	const float rescale = 1.0f / matrix().m11();
    
	// remove all items from scene	
	scene()->clear();

    // iterate over axes
    foreach (NumberParameter *numberParameter, m_numberParametersToDraw) {
        switch (index) {
            case 0:
                curveColor = QColor(Qt::red);
                break;
            case 1:
                curveColor = QColor(Qt::green);
                break;
            case 2:
                curveColor = QColor(Qt::blue);
                break;
            default:
                curveColor = QColor(Qt::darkGray);
        }

        const QList<Key> &keys = numberParameter->getKeys();
        preKey = preTan = 0;

        // iterate through keys
        foreach (const Key &key, keys) {
			// get data and calulate positions in scene
			data = QVariant::fromValue<Key*>(const_cast<Key *>(&key));
			keyPosX = key.index * m_scaleFactor.width();
			keyPosY = yOffset - key.keyValue.toFloat() * m_scaleFactor.height();

			// add key item
			keyItem  = new KeyGraphicsItem(m_scaleFactor, keyPosX);
			scene()->addItem(keyItem);
			keyItem->setPos(keyPosX, keyPosY);
			keyItem->setScale(rescale);
			keyItem->setData(0, data);
						
			// add segment items
			segmentItem = new SegmentGraphicsItem(preKey, keyItem, preKeyType, curveColor);
			segmentItem->setWidth(rescale);
			scene()->addItem(segmentItem);

			if (preTan && (preTan->getMaxX() > keyPosX)) {
				preTan->setMaxX(keyPosX);
				preTan = 0;
			}

			// add tangent items with handles
			if (key.type == Key::KT_Bezier)
			{
				tangentItem = new TangentGraphicsItem(keyItem, m_scaleFactor, keyPosX);
				scene()->addItem(tangentItem);
				tangentItem->setPos(
					key.tangentIndex * m_scaleFactor.width(), 
					yOffset - key.tangentValue * m_scaleFactor.height());
				tangentItem->setScale(rescale);
				tangentItem->setData(0, data);
				
				if (preKey)
					tangentItem->setMaxX((2.0f*keyPosX - preKey->pos().x()));
			
				segmentItem = new SegmentGraphicsItem(keyItem, tangentItem);
				segmentItem->setWidth(rescale);
				scene()->addItem(segmentItem);
				preTan = tangentItem;
			}
            
			// store previous items
			preKeyType = key.type;
			preKey = keyItem;
		}
        ++index;
	}

    // adding the timeline widget
	m_scaleFactor.setWidth((float) width() / (float) (m_outFrame-m_inFrame));
    m_timeline = new TimelineGraphicsItem(height()*scenescale);
	m_timeline->setPos(m_timelinePos, 0);
	m_timeline->setWidth(rescale);
    scene()->addItem(m_timeline);
}

//!
//! Sets the index of the current frame in the scene's time.
//!
//! \param index The new index of the current frame in the scene's time.
//!
void CurveEditorGraphicsView::setCurrentFrame ( const int pos )
{
	m_timelinePos = m_scaleFactor.width() * pos;
    m_timeline->setPos(m_timelinePos, 0);
    update();
}


//!
//! Sets the index of the in frame in the scene's time.
//!
//! \param index The new index of the in frame in the scene's time.
//!
void CurveEditorGraphicsView::setInFrame ( const int index )
{
	m_inFrame = index;
}

//!
//! Sets the index of the out frame in the scene's time.
//!
//! \param index The new index of the out frame in the scene's time.
//!
void CurveEditorGraphicsView::setOutFrame ( const int index )
{
	m_outFrame = index;
}

//private functions

//!
//! Scales the graphics items in the scene by given value.
//!
//! \param value The value for scaling the scene items.
//!
void CurveEditorGraphicsView::scaleSceneItems ( const float value )
{
	SegmentGraphicsItem *segmentItem;
	const float scale = 1.0f/value;
	const QList<QGraphicsItem *> &itemList = scene()->items();
	foreach(QGraphicsItem *item, itemList) {
		if (item->isVisible())
			//if (dynamic_cast<KeyGraphicsItem *>(item))
				//item->setScale(scale);
			if (segmentItem = dynamic_cast<SegmentGraphicsItem *>(item))
				segmentItem->setWidth(scale);
	}
	m_timeline->setWidth(scale);
}

//!
//! Emits the signal that the selected key type hase changed.
//!
//!
void CurveEditorGraphicsView::emitSelectedKeyTypeChanged ( )
{
	QList<QGraphicsItem *> &itemList = scene()->selectedItems();
	if (!itemList.isEmpty()) {
		foreach(QGraphicsItem *item, itemList) {
			KeyGraphicsItem *keyItem = dynamic_cast<KeyGraphicsItem *>(item);
			if (keyItem && !dynamic_cast<TangentGraphicsItem *>(item)) {
				emit selectedKeyTypeChanged(static_cast<int>(item->data(0).value<Key*>()->type));
				break;
			}
		}
	}
}

} // end namespace Frapper