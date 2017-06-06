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
//! \file "PainterGraphicsScene.cpp"
//! \brief Implementation file for PainterGraphicsScene class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       06.09.2013 (last updated)
//!


#include "PainterGraphicsScene.h"
#include "PainterGraphicsItems.h"


namespace PainterPanel {

// init static instance counter
	int PainterGraphicsScene::s_itemInstanceCount[BaseShapeItem::NUM_ITEMTYPES] = {1,1,1,1,1,1,1,1,1};

//!
//! Constructor of the PainterGraphicsScene class.
//!
//!
PainterGraphicsScene::PainterGraphicsScene () : 
	QGraphicsScene(),
	m_background(NULL),
	m_maskLUT(NULL),
	m_mask(NULL),
	m_useLUT(false),
	m_bgDarkenFactor(0.5f)
{}

//!
//! Destructor of the PainterGraphicsScene class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PainterGraphicsScene::~PainterGraphicsScene ()
{
	delete m_background;
	delete m_maskLUT;
	delete m_mask;
	foreach( QGraphicsItem* layer, m_layers ) {
		delete layer;
	}
}

//!
//! Set the background image as none selectable graphics pixmap item
//!
void PainterGraphicsScene::setBackground( QImage image )
{
	if( m_background ) {
		delete m_background;
		m_background = NULL;
	}
	if( image.isNull() )
		return;

	// set new background item
	// add background image as pixmap item
	QPixmap background( QPixmap::fromImage(image) );

	if( m_useLUT ){
		QPainter p(&background);
		p.setCompositionMode(QPainter::CompositionMode_SourceOver);
		p.setPen( QColor(0,0,0,120));
		p.setBrush( QBrush( QColor(0,0,0, floor( m_bgDarkenFactor * 255) ), Qt::SolidPattern));
		p.drawRect(background.rect());
	}
	
	m_background = new QGraphicsPixmapItem( background );
	m_background->setZValue(-10000.0f);
	m_background->setFlag( QGraphicsItem::ItemIsSelectable, false );

	this->addItem(m_background);
	this->setSceneRect(m_background->sceneBoundingRect());
}

//!
//! Set the mask image as none selectable graphics pixmap item
//!
void PainterGraphicsScene::setMask( QImage mask )
{
	if( m_mask ) {
		delete m_mask;
		m_mask = NULL;
	}
	if( m_maskLUT ){
		delete m_maskLUT;
		m_maskLUT = NULL;
	}

	if( mask.isNull() )
		return;
	
	// use compositor to create actual transparent pixmap
	QPixmap mask_tr( mask.size());
	mask_tr.fill(Qt::transparent);

	QPainter p(&mask_tr);
	p.setCompositionMode( QPainter::CompositionMode_SourceOver);
	p.drawImage(0,0, mask);
	p.end();

	m_mask = new QGraphicsPixmapItem( mask_tr );
	m_mask->setZValue(-9991.0f); // value needs to be larger than background and LUT
	m_mask->setFlag( QGraphicsItem::ItemIsSelectable, false );
	dynamic_cast<QGraphicsPixmapItem*>(m_mask)->setShapeMode( QGraphicsPixmapItem::BoundingRectShape );

	this->addItem(m_mask);

	// create maskLUT
	if( m_useLUT ) {

		QImage maskLUT = QImage(mask);

		if( mask.format() == QImage::Format_Indexed8) {

			maskLUT.setColor(0, qRgba(0,0,0,0)); // black & transparent
			for( int i=1; i<256; i++)
				maskLUT.setColor(i, lut(i));
		
		} else {

			for( size_t y = 0; y < mask.height(); ++y) 
			{
				QRgb* srcCol = (QRgb*) mask.scanLine(y);
				QRgb* dstCol = (QRgb*) maskLUT.scanLine(y);

				for( size_t x = 0; x < mask.width(); ++x) 
				{
					if( (srcCol[x] & RGB_MASK) == 0)
						dstCol[x] = 0; // black & transparent
					else
						dstCol[x] = lut( qRed( srcCol[x] ));
				}
			}
		}

		// use compositor to create actual transparent pixmap
		QPixmap mask_tr( maskLUT.size());
		mask_tr.fill(Qt::transparent);

		QPainter p(&mask_tr);
		p.setCompositionMode( QPainter::CompositionMode_SourceOver);
		p.drawImage(0,0, maskLUT);
		p.end();

		m_maskLUT = new QGraphicsPixmapItem( mask_tr );
		m_maskLUT->setZValue(-9990.0f); // put LUT-mask over normal mask
		m_maskLUT->setFlag( QGraphicsItem::ItemIsSelectable, false );
		dynamic_cast<QGraphicsPixmapItem*>(m_maskLUT)->setShapeMode( QGraphicsPixmapItem::BoundingRectShape );

		this->addItem(m_maskLUT);
	}
}

void PainterGraphicsScene::setLayer( int id, QImage image )
{
	QGraphicsPixmapItem* layer = m_layers.value(id, NULL);

	if( layer) {
		delete layer;
		m_layers.remove(id);
	}

	if( image.isNull()) {
		return;
	}

	// mask out black color
	layer = new QGraphicsPixmapItem( QPixmap::fromImage(image) );
	layer->setZValue(-1000.0f - id); // between mask and background
	layer->setFlag( QGraphicsItem::ItemIsSelectable, false );
	layer->setShapeMode( QGraphicsPixmapItem::BoundingRectShape );

	m_layers[id] = layer;

	this->addItem( layer );
}



//!
//! Get the background image as graphics pixmap item
//!
QGraphicsItem* PainterGraphicsScene::getBackground()
{
	return m_background; 
}

//!
//! Render the content of the scene to a QImage
//! The size is determined by the background image
//!
void PainterGraphicsScene::renderToImage( QImage &image, bool withBackground )
{
	// clear selection in scene
	QList<QGraphicsItem*> selection = selectedItems();
	clearSelection();

	// init canvas with black
	QPainter painter( &image );
	painter.fillRect(image.rect(), Qt::black);
	painter.setCompositionMode( QPainter::CompositionMode_SourceOver);

	// hide all additional layers
	foreach( QGraphicsItem* layer, m_layers ) {
		layer->setVisible(false);
	}

	// don't render with LUT enabled
	bool useLUT = m_useLUT;
	setUseLUT(false);

	// render current scene
	if( m_background ) {

		m_background->setVisible(withBackground);
		this->render( &painter, image.rect(), m_background->boundingRect());
		m_background->setVisible( true );
	
	} else {

		this->render( &painter, image.rect());
	}

	// restore usage of LUT
	setUseLUT(useLUT);

	// show additional layers
	foreach( QGraphicsItem* layer, m_layers ) {
		layer->setVisible(true);
	}

	// restore selection
	foreach( QGraphicsItem* item, selection )
		item->setSelected(true);
}

//!
//! Clear the whole scene and delete all items
//!
void PainterGraphicsScene::clearItems()
{
	Q_FOREACH( QGraphicsItem *item, this->items() ) 
	{
		if( dynamic_cast<BaseShapeItem*>(item))
		{
			QString itemName = dynamic_cast<BaseShapeItem*>(item)->getName();
			delete item;
			
			emit itemDeleted(itemName);
		}
		else if( item == m_background || item == m_mask )
		{
			// skip
		}
	}
	emit sceneChanged();
}

//!
//! Override QGraphicsScenes clear method to handle background an mask pointer
//!
void PainterGraphicsScene::clear()
{
	clearItems();
	QGraphicsScene::clear();
	m_background = NULL;
	m_mask = NULL;

	m_layers.clear();
}

BaseShapeItem* PainterGraphicsScene::createGraphicsItem( const PainterPanelItemData& itemData )
{
	BaseShapeItem* item = NULL;
	const QList<QPointF> points = itemData.ItemPoints();

	if( itemData.Valid() ) {
		if( itemData.ItemType() == BaseShapeItem::PAINT )
			item = createGraphicsItem( QPointF(), itemData.ItemType(), itemData.ItemName() );
		else
			item = createGraphicsItem( itemData.ItemPoints().at(0), itemData.ItemType(), itemData.ItemName() );
	}

	if( NULL != item ) {

		if( item->GetItemType() == BaseShapeItem::PAINT ) {
			QPainterPath painterPath = itemData.getPaintShape();
			item->setPainterPath( painterPath );

		} else {

			for( int i=1; i < points.length(); i++ ) {
				item->addPosition( points.at(i));
			}
		
			if( itemData.ItemType() == BaseShapeItem::BEZIER_CURVE )
			{
				BezierCurveItem* bezierItem = dynamic_cast<BezierCurveItem*>(item);
				bezierItem->setHandlePoints( itemData.ItemControlPoints() );
			}
		}

		item->setPosition( itemData.ItemPosition() );
		item->setRotation( itemData.ItemRotation() );

		QPen pen( itemData.PenColor());
		pen.setWidth( itemData.PenSize());
		item->setPen(pen);

		QBrush brush( itemData.BrushColor());
		brush.setStyle( itemData.Fill() ? Qt::SolidPattern : Qt::NoBrush );
		item->setBrush(brush);

		item->updateShape();

		// items that are created from itemData should not be selected
		item->setSelected(false);
	}

	return item;
}

BaseShapeItem* PainterGraphicsScene::createGraphicsItem( QPointF pos, BaseShapeItem::ItemType itemType, QString name /*= "" */, QPen pen /*= QPen()*/, QBrush brush /*= QBrush()*/  )
{
	BaseShapeItem* result = NULL;

	// erase can be restricted to selected paint items, leave them selected here
	if( itemType != BaseShapeItem::ItemType::ERASE )
		this->clearSelection();

	switch( itemType ) 
	{
	case BaseShapeItem::ItemType::PAINT:
		result = new PaintItem( pos);
		break;
	case BaseShapeItem::ItemType::ERASE:
		result = new EraseItem( pos);
		break;
	case BaseShapeItem::ItemType::LINE:
		result = new LineItem( pos);
		break;
	case BaseShapeItem::ItemType::RECT:
		result = new RectItem( pos);
		break;
	case BaseShapeItem::ItemType::CIRCLE:
		result = new CircleItem( pos);
		break;
	case BaseShapeItem::ItemType::ELLIPSE:
		result = new EllipseItem( pos);
		break;
	case BaseShapeItem::ItemType::POLYGON:
		result = new PolygonItem( pos);
		break;
	case BaseShapeItem::ItemType::BEZIER_CURVE:
		result = new BezierCurveItem( pos);
		break;
	default:
		Log::error("Tried to create ShapeItem without correct ItemType!", "PainterPanel");
		return NULL;
		break;
	}

	if( result ) {

		if( result->GetItemType() != BaseShapeItem::ERASE )
		{
			connect( result, SIGNAL( itemPositionChanged()), this, SLOT(sceneItemHasChanged()));
			connect( result, SIGNAL( itemRotationChanged()), this, SLOT(sceneItemHasChanged()));
			connect( result, SIGNAL( itemPointsChanged()),   this, SLOT(sceneItemHasChanged()));

			// set name as descriptive text plus instance number, post increment instance counter
			if( !name.isEmpty() ) {		
				QString nameCount = QString(name);
				nameCount = nameCount.remove(QRegExp("\\D"));
				int count = nameCount.toInt();

				if( count >= s_itemInstanceCount[ result->GetItemType()] ) {
					result->setName(name);
					s_itemInstanceCount[ result->GetItemType() ] = count+1;
				}
				else
				{
					result->setName(name);
				}
			}
			else
				result->setName( result->getDescriptionText() + " " + QString::number( s_itemInstanceCount[ result->GetItemType()]++ ));

			result->setPen(pen);
			result->setBrush(brush);
			result->setUseLUT(m_useLUT);

			addItem( result );

			emit itemCreated(result);
			emit sceneChanged();
		} 
		else 
		{
			addItem( result);
		}
	}

	return result;
}

void PainterGraphicsScene::sceneHasChanged() {
	emit sceneChanged();
}

void PainterGraphicsScene::sceneItemHasChanged()
{
	BaseShapeItem* shapeItem = dynamic_cast<BaseShapeItem*>(sender());
	if( shapeItem )
		emit itemChanged(shapeItem);
	emit sceneChanged();
}

BaseShapeItem* PainterGraphicsScene::getGraphicsItemByName( const QString& name )
{
	foreach( QGraphicsItem* gi, items() )
	{
		BaseShapeItem* bsi = dynamic_cast<BaseShapeItem*>(gi);
		if( bsi && bsi->getName() == name )
			return bsi;
	}
	return NULL;
}

void PainterGraphicsScene::updateGraphicsItem( const PainterPanelItemData& itemData )
{
	BaseShapeItem* item = getGraphicsItemByName( itemData.ItemName() );

	if( !item ) {
		createGraphicsItem( itemData );
	
	} else {
		if( !itemData.Valid() ){
			QString itemName = item->getName();
			delete item;
			emit itemDeleted( itemName );

		} else {
			item->setPoints( itemData.ItemPoints());
			item->setRotation( itemData.ItemRotation());

			if( item->GetItemType() == BaseShapeItem::BEZIER_CURVE )
				dynamic_cast<BezierCurveItem*>(item)->setHandlePoints( itemData.ItemControlPoints() );

			if( item->GetItemType() != BaseShapeItem::BEZIER_CURVE )
				item->setPosition( itemData.ItemPosition());

			QPen pen( itemData.PenColor());
			pen.setWidth( itemData.PenSize());
			item->setPen(pen);

			QBrush brush( itemData.BrushColor());
			brush.setStyle( itemData.Fill() ? Qt::SolidPattern : Qt::NoBrush );
			item->setBrush(brush);

			item->updateShape();
		}
	}
	
	emit sceneChanged();
}

void PainterGraphicsScene::updateGraphicsItemNames()
{
	QList<QString> itemList = QList<QString>();

	foreach( QGraphicsItem *item, items())
	{
		BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>( item );
		if(bs_item)
			itemList.append(bs_item->getName());
	}
	emit itemEntrysIntoHistory(itemList);
	
}

void PainterGraphicsScene::keySelectedItems()
{
	foreach( QGraphicsItem *item, selectedItems())
	{
		BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>( item );
		emit itemKeyed( bs_item );
	}
	//this->clearSelection();
	emit sceneChanged();
}

QList<QString> PainterGraphicsScene::getSelectedItems()
{
	QList<QString> itemList = QList<QString>();

	foreach( QGraphicsItem *item, selectedItems())
	{
		BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>( item );
		itemList.append(bs_item->getName());
	}
	return itemList;
}

void PainterGraphicsScene::deleteGraphicsItem( QString name )
{
	BaseShapeItem* itemToDelete = getGraphicsItemByName(name);
	if( itemToDelete ) {
		delete itemToDelete;
	}
}


void PainterGraphicsScene::signalItemChanged( BaseShapeItem* item )
{
	emit itemChanged(item);
}

const QRgb PainterGraphicsScene::lut( int i ) const
{
	if( i == 0 )
		return qRgb( 0, 0, 0);
	else if( i < 128)
		return	qRgb( 2*i, 2*i, 255 );
	else
		return	qRgb( 255, 511-2*i, 511-2*i);
}

void PainterGraphicsScene::setUseLUT( bool val )
{
	m_useLUT = val;

	if( m_maskLUT )
		m_maskLUT->setVisible(val);

	foreach( QGraphicsItem *item, this->items() ) 
	{
		BaseShapeItem* bshItem = dynamic_cast<BaseShapeItem*>(item);
		if( bshItem )
		{
			bshItem->setUseLUT(val);
			break;
		}
	}

	emit useLUT(val);
}

const bool PainterGraphicsScene::getUseLUT() const
{
	return m_useLUT;
}

} // end namespace PainterPanel