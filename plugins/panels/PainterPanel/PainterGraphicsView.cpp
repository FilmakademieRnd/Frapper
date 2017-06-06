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
//! \file "PainterGraphicsView.cpp"
//! \brief Implementation file for PainterGraphicsView class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       16.01.2012 (last updated)
//!

#include "PainterGraphicsView.h" 
namespace PainterPanel {

///
/// Constructors and Destructors
///

//!
//! Constructor of the PainterGraphicsView class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
PainterGraphicsView::PainterGraphicsView ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    BaseGraphicsView(parent),
	m_scene(NULL),
    m_currentItem( NULL ),
    m_drawMode( NONE ),
    m_pen( QPen(Qt::white, 3)),
    m_brush( QBrush(Qt::white, Qt::NoBrush))
{
    //setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setTransformationAnchor(AnchorViewCenter);
    setResizeAnchor(AnchorViewCenter);
	
	setMouseTracking(false); // disable mouse tracking for performance reasons
}


//!
//! Destructor of the PainterGraphicsView class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PainterGraphicsView::~PainterGraphicsView ()
{
}

void PainterGraphicsView::setDrawMode( DrawMode drawmode )
{
    m_drawMode = drawmode;
	this->setCursor();
}

PainterGraphicsView::DrawMode PainterGraphicsView::getDrawMode( )
{
	return m_drawMode;
}

void PainterGraphicsView::setCursor()
{
	//switch( m_drawMode )
	//{
	//case NONE:
	//	QGraphicsView::setCursor( Qt::ArrowCursor);
	//	return;
	//case PAINT:
	//	QGraphicsView::setCursor( Qt::PointingHandCursor);
	//	return;
	//case RECT:
	//case CIRCLE:
	//case ELLIPSE:
	//	QGraphicsView::setCursor( Qt::SizeAllCursor);
	//	return;
	//case POLYGON:
	//case BEZIER_CURVE:
	//case LINE:
	//	QGraphicsView::setCursor( Qt::CrossCursor);
	//	return;
	//default:
	//	QGraphicsView::setCursor( Qt::ArrowCursor);
	//	return;
	//}
}

void PainterGraphicsView::setCurrentScene( PainterGraphicsScene *scene )
{
	if( m_scene ) {
		// disconnect from scene events, but copy old scene rect
		disconnect( m_scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
		QRectF sr = sceneRect();
		setScene( scene );
		setSceneRect(sr);
	} else {
		setScene( scene );
	}

	if( scene ) {
		centerOn(0, scene->sceneRect().height()/2);
		connect( scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	}

	// here is our new scene
	m_scene = scene;
}

///
/// Public Slots
///

void PainterGraphicsView::onSelectionChanged()
{
    if( m_scene ) {
		QList<QGraphicsItem*> items = m_scene->selectedItems();
		emit selectionChanged(items);

		if( items.size() == 1 )
		{
			BaseShapeItem *item = dynamic_cast<BaseShapeItem*>(items.at(0));
			if( item )
			{
				m_pen = item->pen();
				m_brush = item->brush();

				emit penChanged( m_pen );
				emit brushChanged( m_brush );
			}
		}
	}
}

void PainterGraphicsView::changeBrush( QBrush brush )
{
    m_brush = brush;

	if( m_scene )
	{
		foreach (QGraphicsItem *graphics_item, m_scene->selectedItems())
		{
			BaseShapeItem *item = dynamic_cast<BaseShapeItem*>(graphics_item);
			if( item ) {
				item->setBrush( brush);
				m_scene->signalItemChanged(item);
			}
		}
	}
}

void PainterGraphicsView::changePen( QPen pen )
{
    m_pen = pen;

	if( m_scene )
	{
	    foreach (QGraphicsItem *graphics_item, m_scene->selectedItems())
		{
			BaseShapeItem *item = dynamic_cast<BaseShapeItem*>(graphics_item);
			if( item )
				item->setPen(m_pen);
				m_scene->signalItemChanged(item);
		}
	}
}

void PainterGraphicsView::mouseMoveEvent ( QMouseEvent * event )
{
	// LMB + Alt -> move region
	if( dragMode() == QGraphicsView::ScrollHandDrag ) {
		QGraphicsView::mouseMoveEvent(event);
	} else {

		// Item is in creation at the moment at is not a bezier item
		if( m_currentItem ) {


			// Item is in creation at the moment and IS a bezier item
			if((m_currentItem->GetItemType() == BaseShapeItem::BEZIER_CURVE ||
				m_currentItem->GetItemType() == BaseShapeItem::POLYGON ) &&
				m_currentItem->getCubicTransformMode() )
			{
				m_currentItem->setControlPointsPosition( mapToScene( event->pos()) );
			}
			else
			{
				m_currentItem->updatePosition( mapToScene( event->pos()));
			}
		} else {
			BaseGraphicsView::mouseMoveEvent(event);
		}
		this->setCursor();
	}
}

void PainterGraphicsView::wheelEvent( QWheelEvent *event )
{
	if( m_currentItem && m_currentItem->GetItemType() == BaseShapeItem::ERASE ) {
		EraseItem* eraseItem = dynamic_cast<EraseItem*>(m_currentItem);
		if( eraseItem ) {
			int newSize = ( eraseItem->Size() + event->delta() / 120) ;
			if( newSize > 1 ) {
				eraseItem->Size(newSize);
				eraseItem->updatePosition( eraseItem->scenePos());
			}
		}
	} else {
		BaseGraphicsView::wheelEvent(event);
	}
}

void PainterGraphicsView::setCurrentItem( BaseShapeItem* item) {
	m_currentItem = item;
}

BaseShapeItem* PainterGraphicsView::getCurrentItem( ) {
	return m_currentItem;
}

void PainterGraphicsView::mousePressEvent(QMouseEvent *event)
{
	QPointF pos = mapToScene( event->pos());

	// left mouse button clicked
    if ( event->button() == Qt::LeftButton )
	{	
		// use Alt key to move region
		if( event->modifiers().testFlag(Qt::AltModifier)) {
			BaseGraphicsView::mousePressEvent(event);
		}

		// Current Draw Mode active?
		else if( m_drawMode > NONE )
		{
			// No current item active?
			if( !m_currentItem )
			{
				if( !continuePaintItem( pos, m_drawMode))
				{
					// create a new item with the current setup
					itemToCreateOnStack( pos );
				}
			}
			// Item exists?
			else if ( m_currentItem->GetItemType() != BaseShapeItem::BEZIER_CURVE && m_currentItem->GetItemType() != BaseShapeItem::POLYGON)
			{
				// add position to the current item
				m_currentItem->addPosition(pos);

				// If the Item is no poly, we are done with creating the item
				if( m_currentItem->GetItemType() != BaseShapeItem::PAINT  )
				{   
					// signal new item to panel
					m_currentItem->setSelected(false);
					if(m_scene) 
						m_scene->signalItemChanged(m_currentItem);
					m_currentItem = NULL;
				}
			}
			// If we have a bezier curve, we want to transform the cubic handles, till mouse is released
			else {
				m_currentItem->setCubicTransformMode(true);
			}
		}  
		else
		{
			BaseGraphicsView::mousePressEvent(event);
		}
	}
	// right mouse button and active item present
    else if (event->button() == Qt::RightButton && m_currentItem)
    {
        // cancel creation of current item
        if( m_currentItem->GetItemType() == BaseShapeItem::BEZIER_CURVE ||
            m_currentItem->GetItemType() == BaseShapeItem::POLYGON ) 
        {  
            bool emptyItem = false;

			// check if deletion of last position would lead to an empty item, then delete it
			m_currentItem->skipLastPosition(emptyItem);

            if( emptyItem ) {
				QString name = m_currentItem->getName();
				m_currentItem = NULL;
				emit itemCanceled(name);
				if( m_scene ) 
					m_scene->deleteGraphicsItem( name );
			} 
			else {
				m_currentItem->setSelected(false);
				if( m_scene) 
					m_scene->signalItemChanged(m_currentItem);
				m_currentItem = NULL;
			}
        } 
		// item is a item that should be finished after second click, so this is a cancel click
		else
		{ 
			QString name = m_currentItem->getName();
			if( m_scene)
				m_scene->deleteGraphicsItem(name);
			m_currentItem = NULL;
			emit itemCanceled(name);
        }
    }
	// right mouse button and select mode active, for HoverSelectMenu
	else if(event->button() == Qt::RightButton && !m_currentItem && m_drawMode == NONE && m_scene)
	{
		QPointF eventPos = this->mapToScene(event->pos());
		QList<QGraphicsItem*> list = m_scene->items(eventPos);
		QMenu menu;
		m_scene->clearSelection();
		Q_FOREACH(QGraphicsItem* item, list) {
			BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);
			if(bs_item) {
				bs_item->setSelected(false);
				menu.addAction(bs_item->getName());
			}
		}
		QAction* action = menu.exec(QCursor::pos());
		if(action) {
			BaseShapeItem* item = m_scene->getGraphicsItemByName(action->iconText());
			item->setSelected(true);
			qreal zVal = 0;
			Q_FOREACH(QGraphicsItem* item, list) {
				if(item->zValue() > zVal)
					zVal = item->zValue();
			}
			zVal = zVal + 1;
			item->setZValue(zVal);
		}
	}
	// right mouse button and a draw mode active, but no active drawing
    else if (event->button() == Qt::RightButton && !m_currentItem && m_drawMode > NONE)
    {
		if( m_drawMode == PAINT && m_scene && m_scene->selectedItems().size() > 0 ) {
			// if in paint item mode, RMB de-selects current item so that a new paint item can be created with LMB
			m_scene->clearSelection();
		} else {
			// switch to selection mode
			emit drawModeChanged(NONE);
		}

    }
	this->setCursor();
}

void PainterGraphicsView::mouseDoubleClickEvent( QMouseEvent *event )
{

	if (event->button() == Qt::RightButton && !m_currentItem && m_drawMode > NONE)
	{
		// right dbl-click clears set mode to select and clears current selection
		m_scene->clearSelection();
		emit drawModeChanged(NONE);
	}
}

void PainterGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if ( event->button() == Qt::LeftButton )
	{
		// check if in item creation mode
		if( m_currentItem ) 
		{
			if ( m_currentItem->GetItemType() == BaseShapeItem::PAINT )
			{
				// if current item is paint item, finish creation on mouse release
				if(m_scene) 
					m_scene->signalItemChanged(m_currentItem);
				m_currentItem = NULL;
			}
			else if ((m_currentItem->GetItemType() == BaseShapeItem::BEZIER_CURVE || m_currentItem->GetItemType() == BaseShapeItem::POLYGON ) && 
				      m_currentItem->getCubicTransformMode() ) 
			{
				m_currentItem->setCubicTransformMode(false);
				m_currentItem->addPosition(mapToScene( event->pos()));
			}
			else if( m_currentItem->GetItemType() == BaseShapeItem::ERASE )
			{
				// get selected items
				QList<QGraphicsItem*> items = scene()->selectedItems();
				if( items.size() == 0)
					items = scene()->items( m_currentItem->getPainterPath(), Qt::IntersectsItemBoundingRect );

				// signal possible change of selected paint items
				foreach(QGraphicsItem* item, items) {
					BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);

					if(bs_item && bs_item->GetItemType() == PAINT) {
						m_scene->signalItemChanged(bs_item);
					}
				}

				delete m_currentItem;
				m_currentItem = NULL;
			}

			// signal scene has changed
			emit triggerRedraw();
		}
		// No current item in creation
		else
		{
			if( m_scene && !m_scene->selectedItems().isEmpty())
				emit itemsMovedOrRotated();
			BaseGraphicsView::mouseReleaseEvent(event);
		}
	}
	else
	{
		BaseGraphicsView::mouseReleaseEvent(event);
	}
}

void PainterGraphicsView::enterEvent(QEvent *event)
{
	BaseGraphicsView::enterEvent(event);
	this->setCursor();
}

void PainterGraphicsView::keyPressEvent( QKeyEvent *event )
{
    switch (event->key()) 
    {
        // P toggles the different draw modes
    case Qt::Key_P:
        toggleDrawmode();
        break;

        // C clears the scene
    case Qt::Key_C:
        clearScene();
        //emit sceneChanged( this->scene());
        break;

	case Qt::Key_K:
		if( m_scene )
			m_scene->keySelectedItems();

		break;

	case Qt::Key_Space:
		// ignored!

		break;

    default:
        BaseGraphicsView::keyPressEvent(event);
        break;
    }
}

void PainterGraphicsView::toggleDrawmode()
{
    // cycle through different draw modes
    int drawMode = m_drawMode;
    ++drawMode %= NUM_MODES;
	this->setDrawMode(static_cast<PainterGraphicsView::DrawMode>(drawMode));
    emit drawModeChanged(m_drawMode);
}

//private functions

void PainterGraphicsView::clearScene()
{
	if( m_scene ) {
		m_scene->clearItems();
	}

    m_currentItem = NULL;
}

void PainterGraphicsView::adjustToBackground()
{
	if( m_scene && m_scene->getBackground())
	{
		fitInView( m_scene->getBackground()->sceneBoundingRect(), Qt::KeepAspectRatio );
	}
}

BaseShapeItem* PainterGraphicsView::createItem( QPointF pos, BaseShapeItem::ItemType type, QString name )
{
	if( m_scene )
	{
		return m_scene->createGraphicsItem( pos, type, name, m_pen, m_brush );
	}
	return NULL;
}

BaseShapeItem* PainterGraphicsView::recreateItem( PainterPanelItemData data )
{
	if( m_scene )
	{
		return m_scene->createGraphicsItem(data);
	}
	return NULL;
}

bool PainterGraphicsView::continuePaintItem( QPointF pos, DrawMode drawMode )
{
	// make previous paint item active again
	if( drawMode == PAINT && m_scene && m_scene->selectedItems().size() == 1 )
	{
		PaintItem* pi = dynamic_cast<PaintItem*>(m_scene->selectedItems().at(0));
		if( pi ) {
			m_currentItem = pi;
			pi->addPosition(pos);
			return true;
		}
	}
	return false;
}

} // end namespace PainterPanel