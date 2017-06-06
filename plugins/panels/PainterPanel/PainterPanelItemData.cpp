/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "PainterPanelItemData.cpp"
//! \brief Implementation file for PainterPanelItemData class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       08.10.2013 (created)
//!

#include "PainterPanelItemData.h"
#include "Log.h"
#include <QColor>

namespace PainterPanel {
using namespace Frapper;

Q_DECLARE_METATYPE(Parameter *)

  
///
/// Constructors and Destructors
///

//!
//! Constructor of the PainterPanelItemData class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
PainterPanelItemData::PainterPanelItemData()
{
}

PainterPanelItemData::PainterPanelItemData( ParameterGroup* itemParameter, int time /*= 0 */ )
{
	m_time = time;
	fromParameter(itemParameter);
}

PainterPanelItemData::PainterPanelItemData( BaseShapeItem* shapeItem, int time /*= 0 */)
{
	m_time = time;
	fromShapeItem(shapeItem);
}

//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PainterPanelItemData::~PainterPanelItemData ()
{
}

///
/// Template Functions
///

template <typename T>
T PainterPanelItemData::getParameterValue( NumberParameter* parameter )
{
	if( parameter->isAnimated() ){
		return parameter->getKeyValueTime(m_time).value<T>();
	}
	else {
		return parameter->getValue().value<T>();
	}
}


///
/// Private Slots
///

void PainterPanelItemData::fromShapeItem( BaseShapeItem* shapeItem )
{
	m_itemName = shapeItem->getName();
	m_itemType = shapeItem->GetItemType();
	m_itemPosition = shapeItem->getPosition();
	m_itemRotation = shapeItem->getRotation();

	m_penSize = shapeItem->pen().width();
	m_opacity = shapeItem->pen().color().alpha();
	m_penColor = shapeItem->pen().color();
	m_brushColor = shapeItem->brush().color();

	m_fill = shapeItem->brush().style() == Qt::SolidPattern;
	m_isValid = true;

	m_itemPoints.clear();
	m_itemControlPoints.clear();

	// item is obviously visible in the current frame
	m_visibleFrame = m_time;

	// for paint element add first move element as first point in list
	if( m_itemType == BaseShapeItem::PAINT ) 
	{
		// store item painter path as item points
		const QPainterPath& painterPath = shapeItem->getPainterPath();

		m_paintShape.clear();
		bool lastWasMove = false;

		for( int i=0; i<painterPath.elementCount(); i++ ) {

			const QPainterPath::Element& el = painterPath.elementAt(i);

			if( el.isMoveTo() ) {
				if( lastWasMove ) {
					m_paintShape.pop_back();
				}
				m_paintShape.push_back( 'm' + positionToString(el) );
				lastWasMove = true;

			} else if( el.isLineTo() ) {
				m_paintShape.push_back( positionToString(el));
				lastWasMove = false;
			}
		}
	} 
	else if( m_itemType == BaseShapeItem::BEZIER_CURVE )
	{
		BezierCurveItem* bezierItem = dynamic_cast<BezierCurveItem*>( shapeItem );
		m_itemPoints = bezierItem->getPoints();
		m_itemControlPoints = bezierItem->getHandlePoints();
	}
	else
	{
		m_itemPoints = shapeItem->getPoints();
	}
}


void PainterPanelItemData::fromParameter( ParameterGroup* itemParameter )
{
	m_isValid = false;

	const Parameter::Map& itemParameterMap = itemParameter->getParameterMap();

	// item name
	Parameter* itemNameParameter = dynamic_cast<Parameter*>( itemParameterMap.value("Item Name", NULL));
	m_itemName = (itemNameParameter == NULL) ? itemParameter->getName() : itemNameParameter->getValue().toString();

	// item type
	NumberParameter* itemTypeParameter = dynamic_cast<NumberParameter*>  ( itemParameterMap.value("Item Type", NULL));
	m_itemType = (itemTypeParameter == NULL) ? BaseShapeItem::BASE : (BaseShapeItem::ItemType) itemTypeParameter->getValue().toInt();

	// position
	ParameterGroup*	  posParameter = dynamic_cast<ParameterGroup*>  ( itemParameterMap.value("Position", NULL));
	m_itemPosition = (posParameter == NULL) ? QPointF(0,0) : getPositionFromParameter(posParameter);

	// rotation
	NumberParameter*  rotValue      = dynamic_cast<NumberParameter*> ( itemParameterMap.value("Rotation", NULL));
	m_itemRotation = (rotValue == NULL ) ? 0.0f : getParameterValue<float>(rotValue);

	// hide item if not visible in current frame and not keyed
	NumberParameter* visibleFramesParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Visible Frame", NULL));
	m_visibleFrame = (visibleFramesParameter == NULL) ? -1 : visibleFramesParameter->getValue().toInt();

	// skip item creation if item not visible in current frame and has no keys set
	if( (m_visibleFrame != m_time) && !(rotValue && rotValue->isAnimated())) {
		m_isValid = false;
		return;
	}

	// pen size
	NumberParameter* penSize = dynamic_cast<NumberParameter*> ( itemParameterMap.value("Pen Size", NULL));
	m_penSize = (penSize == NULL ) ? 1 : floor( getParameterValue<float>(penSize)+0.5);

	// opacity
	NumberParameter*  opacity      = dynamic_cast<NumberParameter*> ( itemParameterMap.value("Opacity", NULL));
	m_opacity = (opacity == NULL ) ? 255 : floor( getParameterValue<float>(opacity)+0.5);

	// pen color
	NumberParameter*  penColor      = dynamic_cast<NumberParameter*> ( itemParameterMap.value("Pen Color", NULL));
	m_penColor = (penColor == NULL ) ? Qt::black : getParameterValue<QColor>(penColor);
	m_penColor.setAlpha(m_opacity);

	// brush color
	NumberParameter*  brushColor      = dynamic_cast<NumberParameter*> ( itemParameterMap.value("Brush Color", NULL));
	m_brushColor = (brushColor == NULL ) ? Qt::black : getParameterValue<QColor>(brushColor);
	m_brushColor.setAlpha(m_opacity);

	// brush fill
	Parameter*  brushFill      = dynamic_cast<Parameter*> ( itemParameterMap.value("Fill", NULL));
	m_fill = (brushFill == NULL ) ? false : brushFill->getValue().toBool();

	// points
	m_itemPoints.clear();
	m_itemControlPoints.clear();

	if( m_itemType == BaseShapeItem::PAINT ) {

		Parameter* itemShapeParameter = dynamic_cast<Parameter*>( itemParameterMap.value("Paint Shape", NULL));
		QString value = (itemShapeParameter == NULL) ? "" : itemShapeParameter->getValue().toString();
		m_paintShape = value.split(";");
		m_isValid = (m_paintShape.size() > 0);

	} else {
		ParameterGroup* pointsParameter = dynamic_cast<ParameterGroup*>  ( itemParameterMap.value("Points", NULL));
		if( pointsParameter ) {

			const Parameter::Map& pointsMap = pointsParameter->getParameterMap();

			for( int i=0; i< pointsMap.size(); i++) {

				QString name = "Point " + QString::number(i);
				ParameterGroup* pointParameter = dynamic_cast<ParameterGroup*>( pointsMap.value(name, NULL));

				if( pointParameter ) {
				
					QPointF point = getPositionFromParameter( pointParameter);
					m_itemPoints.push_back(point);

					// control points
					if ( m_itemType == BaseShapeItem::BEZIER_CURVE ) {

						ParameterGroup* handle1Pos = dynamic_cast<ParameterGroup*>( pointParameter->getParameterMap().value("Handle Point One", NULL));
						ParameterGroup* handle2Pos = dynamic_cast<ParameterGroup*>( pointParameter->getParameterMap().value("Handle Point Two", NULL));

						if( handle1Pos && handle2Pos ) {
							m_itemControlPoints.push_back( getPositionFromParameter( handle1Pos ));
							m_itemControlPoints.push_back( getPositionFromParameter( handle2Pos ));
						} else {
							m_itemControlPoints.push_back( QPointF(0,0));
							m_itemControlPoints.push_back( QPointF(0,0));
						}
					}
				} else {
					Log::warning(QString("The parameter group %1 has no parameter %2").arg( pointsParameter->getName(), name));
				}
			}
		}
		m_isValid = (m_itemPoints.size() > 0);
	}
}

void PainterPanelItemData::toParameter( ParameterGroup* itemGroup )
{
	if( !itemGroup )
		return;

	// item parameter
	ParameterGroup* itemParameter = dynamic_cast<ParameterGroup*>( itemGroup->getParameterMap().value(m_itemName, NULL));
	if( NULL == itemParameter ) {
		itemParameter = new ParameterGroup( m_itemName );
		itemGroup->addParameter(itemParameter);
	}
	
	const Parameter::Map& itemParameterMap = itemParameter->getParameterMap();

	// item name parameter
	Parameter* itemNameParameter = dynamic_cast<Parameter*>( itemParameterMap.value("Item Name", NULL));
	if( NULL == itemNameParameter ) {
		itemNameParameter = new Parameter( "Item Name", Parameter::T_Label, QVariant(""));
		itemParameter->addParameter(itemNameParameter);
	}
	itemNameParameter->setValue(m_itemName);

	// item type parameter
	NumberParameter* itemTypeParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Item Type", NULL));
	if( NULL == itemTypeParameter ) {
		itemTypeParameter = new NumberParameter( "Item Type", Parameter::T_Int, QVariant(-1));
		itemTypeParameter->setReadOnly(true);
		itemParameter->addParameter(itemTypeParameter);
	}
	itemTypeParameter->setValue(QVariant( (int)m_itemType));
	
	// Visible Frame parameter
	NumberParameter* visibleFramesParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Visible Frame", NULL));
	if( NULL == visibleFramesParameter ) {
		visibleFramesParameter = new NumberParameter( "Visible Frame", Parameter::T_Int, QVariant(-1));
		visibleFramesParameter->setReadOnly(true);
		itemParameter->addParameter( visibleFramesParameter);
	}
	visibleFramesParameter->setValue( m_visibleFrame );

	// position parameter
	ParameterGroup* positionParameter = dynamic_cast<ParameterGroup*>( itemParameterMap.value("Position", NULL));
	if( NULL == positionParameter ) {
		positionParameter = createPositionParameter( "Position", m_itemPosition);
		itemParameter->addParameter(positionParameter);
	} else {
		setPositionParameter( positionParameter, m_itemPosition);
	}

	// rotation
	NumberParameter* rotParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Rotation", NULL));
	if( NULL == rotParameter ) {
		rotParameter = new NumberParameter( "Rotation", Parameter::Type::T_Float, QVariant(0.0f));
		rotParameter->setMinValue(-360); rotParameter->setMaxValue(360);
		itemParameter->addParameter( rotParameter );
	}
	rotParameter->setValue( m_itemRotation );

	// pen size
	NumberParameter* penSizeParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Pen Size", NULL));
	if( NULL == penSizeParameter ) {
		penSizeParameter = new NumberParameter( "Pen Size", Parameter::Type::T_Float, QVariant( 1.0 ));
		penSizeParameter->setMinValue(1); penSizeParameter->setMaxValue(100);
		itemParameter->addParameter( penSizeParameter );
	}
	penSizeParameter->setValue( (float)m_penSize );

	// pen color
	NumberParameter* penColorParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Pen Color", NULL));
	if( NULL == penColorParameter ) {
		penColorParameter = new NumberParameter( "Pen Color", NumberParameter::Type::T_Color, QVariant((int) Qt::black));
		penColorParameter->setVisible(false);
		itemParameter->addParameter( penColorParameter );
	}
	penColorParameter->setValue( m_penColor );

	//brush color
	NumberParameter* brushColorParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Brush Color", NULL));
	if( NULL == brushColorParameter ) {
		brushColorParameter = new NumberParameter( "Brush Color", NumberParameter::Type::T_Color, QVariant((int) Qt::black));
		brushColorParameter->setVisible(false);
		itemParameter->addParameter( brushColorParameter );
	}
	brushColorParameter->setValue( m_brushColor );

	// opacity
	NumberParameter* opacityParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Opacity", NULL));
	if( NULL == opacityParameter ) {
		opacityParameter = new NumberParameter( "Opacity", Parameter::Type::T_Float, QVariant( 255.0 ));
		opacityParameter->setMinValue(0); opacityParameter->setMaxValue(255);
		itemParameter->addParameter( opacityParameter );
	}
	opacityParameter->setValue( (float)m_opacity );

	//brush fill
	Parameter* fillParameter = dynamic_cast<Parameter*>( itemParameterMap.value("Fill", NULL));
	if( NULL == fillParameter ) {
		fillParameter = new Parameter( "Fill", Parameter::Type::T_Bool, QVariant( false ));
		itemParameter->addParameter( fillParameter );
	}
	fillParameter->setValue( m_fill );

	if( m_itemType == BaseShapeItem::PAINT ) {
		
		// paint item shape
		Parameter* itemShapeParameter = dynamic_cast<Parameter*>( itemParameterMap.value("Paint Shape", NULL));
		if( NULL == itemShapeParameter ) {
			itemShapeParameter = new Parameter( "Paint Shape", Parameter::T_String, QVariant(""));
			itemParameter->addParameter(itemShapeParameter);
		}
		itemShapeParameter->setValue( QVariant( m_paintShape.join(";")));

	} else {

		// group for points
		ParameterGroup*   pointsParameter = dynamic_cast<ParameterGroup*>  ( itemParameterMap.value("Points"));
		if( NULL == pointsParameter ) {
			pointsParameter = new ParameterGroup( "Points" );
			itemParameter->addParameter(pointsParameter);
		}
		const Parameter::Map& pointsParameterMap = pointsParameter->getParameterMap();

		// points
		for( size_t i=0; i<m_itemPoints.size(); i++ ) {

			const QPointF& pointValue = m_itemPoints.at(i);
			const QString& pointName  = "Point " + QString::number(i);

			ParameterGroup* pointParameter = dynamic_cast<ParameterGroup*>( pointsParameterMap.value(pointName, NULL));
			if( NULL == pointParameter ) {
				pointParameter = createPositionParameter( pointName, pointValue );
				pointsParameter->addParameter( pointParameter );
			} else {
				setPositionParameter( pointParameter, pointValue );
			}

			// control points
			if( m_itemType == BaseShapeItem::BEZIER_CURVE )
			{
				assert( m_itemControlPoints.length() > 2*i+1 );

				QPointF handle1Pos = m_itemControlPoints.at( 2*i );
				QPointF handle2Pos = m_itemControlPoints.at( 2*i+1 );

				const Parameter::Map& pointParameterMap = pointParameter->getParameterMap();
				ParameterGroup* handle1Parameter = dynamic_cast<ParameterGroup*>  ( pointParameterMap.value("Handle Point One"));
				if( NULL == handle1Parameter ) {
					handle1Parameter = createPositionParameter("Handle Point One", handle1Pos);
					pointParameter->addParameter( handle1Parameter );
				} else {
					setPositionParameter( handle1Parameter, handle1Pos);
				}

				ParameterGroup* handle2Parameter = dynamic_cast<ParameterGroup*>  ( pointParameterMap.value("Handle Point Two"));
				if( NULL == handle2Parameter ) {
					handle2Parameter = createPositionParameter("Handle Point Two", handle2Pos);
					pointParameter->addParameter( handle2Parameter );
				} else {
					setPositionParameter( handle2Parameter, handle2Pos);
				}
			}
		}
	}
}

void PainterPanelItemData::setParameterKey( ParameterGroup* itemGroup, int time )
{
	if( !itemGroup ) return;

	m_time = time;

	// item parameter
	ParameterGroup* itemParameter = dynamic_cast<ParameterGroup*>( itemGroup->getParameterMap().value(m_itemName, NULL));
	if( NULL != itemParameter ) 
	{
		const Parameter::Map& itemParameterMap = itemParameter->getParameterMap();

		// position parameter
		ParameterGroup* positionParameter = dynamic_cast<ParameterGroup*>( itemParameterMap.value("Position", NULL));
		if( NULL != positionParameter ) {
			positionParameterSetKey( positionParameter );
		}

		// rotation
		NumberParameter* rotParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Rotation", NULL));
		if( NULL != rotParameter ) {
			rotParameter->addKey( m_time );
		}

		// pen size
		NumberParameter* penSizeParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Pen Size", NULL));
		if( NULL != penSizeParameter ) {
			penSizeParameter->addKey( m_time );
		}

		// pen color
		NumberParameter* penColorParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Pen Color", NULL));
		if( NULL != penColorParameter ) {
			penColorParameter->addKey( m_time );
		}

		// brush color
		NumberParameter* brushColorParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Brush Color", NULL));
		if( NULL != brushColorParameter ) {
			brushColorParameter->addKey( m_time );
		}

		// opacity
		NumberParameter* opacityParameter = dynamic_cast<NumberParameter*>( itemParameterMap.value("Opacity", NULL));
		if( NULL != opacityParameter  ) {
			opacityParameter->addKey( m_time, Key::KT_Step );
		}

		// points group
		ParameterGroup* pointsParameter	= dynamic_cast<ParameterGroup*>  ( itemParameterMap.value("Points"));
		if( NULL != pointsParameter ) 
		{
			foreach( AbstractParameter* pointParameterAbs, pointsParameter->getParameterMap().values()) 
			{
				ParameterGroup* pointParameter = dynamic_cast<ParameterGroup*>(pointParameterAbs);
				if (NULL != pointParameter)
				{
					positionParameterSetKey( pointParameter );

					ParameterGroup* handle1Parameter = dynamic_cast<ParameterGroup*>  ( pointParameter->getParameterMap().value("Handle Point One"));
					if( NULL != handle1Parameter )
						positionParameterSetKey(handle1Parameter );

					ParameterGroup* handle2Parameter = dynamic_cast<ParameterGroup*>  ( pointParameter->getParameterMap().value("Handle Point Two"));
					if( NULL != handle2Parameter )
						positionParameterSetKey(handle2Parameter );
				}
			}
		}
	}
}

ParameterGroup* PainterPanelItemData::createPositionParameter( const QString &name, const QPointF &value )
{
	ParameterGroup* posParameter = new ParameterGroup( name );

	NumberParameter* posX = new NumberParameter( "X", Parameter::T_Float, QVariant( 0.0));
	NumberParameter* posY = new NumberParameter( "Y", Parameter::T_Float, QVariant( 0.0));
	posX->setMinValue(-FLT_MAX); posX->setMaxValue(FLT_MAX);
	posY->setMinValue(-FLT_MAX); posY->setMaxValue(FLT_MAX);
	posX->setValue( value.x());
	posY->setValue( value.y());

	posParameter->addParameter( posX );
	posParameter->addParameter( posY );

	return posParameter;
}


void PainterPanelItemData::setPositionParameter( ParameterGroup* positionParameter, QPointF position )
{
	NumberParameter* posX = dynamic_cast<NumberParameter*> ( positionParameter->getParameterMap().value("X"));
	if( posX ) posX->setValue(position.x());
	NumberParameter* posY = dynamic_cast<NumberParameter*> ( positionParameter->getParameterMap().value("Y"));
	if( posY ) posY->setValue(position.y());
}

QPointF PainterPanelItemData::getPositionFromParameter( ParameterGroup* positionParameter )
{
	QPointF result = QPointF( 0.0f, 0.0f);
	NumberParameter* posX = dynamic_cast<NumberParameter*> ( positionParameter->getParameterMap().value("X"));
	if( posX ) result.setX( getParameterValue<float>(posX) );
	NumberParameter* posY = dynamic_cast<NumberParameter*> ( positionParameter->getParameterMap().value("Y"));
	if( posY ) result.setY( getParameterValue<float>(posY) );
	return result;
}

void PainterPanelItemData::positionParameterSetKey( ParameterGroup* positionParameter )
{
	NumberParameter* posX = dynamic_cast<NumberParameter*> ( positionParameter->getParameterMap().value("X"));
	if( posX ) posX->addKey( m_time );
	NumberParameter* posY = dynamic_cast<NumberParameter*> ( positionParameter->getParameterMap().value("Y"));
	if( posY ) posY->addKey( m_time );
}

QPainterPath PainterPanelItemData::getPaintShape() const
{
	QPainterPath path;
	foreach( QString element, m_paintShape ) {
		if( element.startsWith('m') ) {
			path.moveTo( stringToPosition( element.right( element.size()-1 )));
		} else {
			path.lineTo( stringToPosition( element ));
		}
	}
	return path;
}

QPointF PainterPanelItemData::stringToPosition( QString positionString ) const
{
	QStringList xy = positionString.split(' ');
	if( xy.size() == 2 ) {
		return QPointF( xy[0].toFloat(), xy[1].toFloat());
	}
	return QPointF();
}

QString PainterPanelItemData::positionToString( QPointF position ) {
	return QString::number( position.x(), 'f', 2) + ' ' + QString::number( position.y(), 'f', 2);
}

} // namespace PainterPanel