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
//! \file "PainterPanelItemData.h"
//! \brief Header file for PainterPanelItemData class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       08.10.2013 (created)
//!


#ifndef PAINTERPANELITEMPARAMETER_H
#define PAINTERPANELITEMPARAMETER_H

// includes, local classes
#include "Parameter.h"
#include "NumberParameter.h"
#include "ParameterGroup.h"
#include "PainterGraphicsItems.h"


namespace PainterPanel {
using namespace Frapper;

//!
//! Class representing a view for displaying and editing parameters of nodes.
//!
class PainterPanelItemData
{

public: // constructors and destructors

    //!
    //! Constructor of the PainterPanelItemData class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    PainterPanelItemData ();
	PainterPanelItemData ( ParameterGroup* itemParameter, int time = 0 );
	PainterPanelItemData ( BaseShapeItem* shapeItem, int time = 0 );

    //!
    //! Destructor of the PainterPanelItemData class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PainterPanelItemData ();

public: // functions

	void fromShapeItem ( BaseShapeItem* shapeItem );
	void fromParameter( ParameterGroup* itemParameter );
	void toParameter( ParameterGroup* itemGroup );

	void setParameterKey( ParameterGroup* itemGroup, int time );

	const QString& ItemName() const { return m_itemName; }
	const BaseShapeItem::ItemType& ItemType() const { return m_itemType; }
	const QPointF& ItemPosition() const { return m_itemPosition; }
	const float& ItemRotation() const { return m_itemRotation; }
	const QList<QPointF>& ItemPoints() const { return m_itemPoints; }
	const QList<QPointF>& ItemControlPoints() const { return m_itemControlPoints; }

	int VisibleFrame() const { return m_visibleFrame; }
	void VisibleFrame(int val) { m_visibleFrame = val; }

	QColor PenColor() const { return m_penColor; }
	int    PenSize() const { return m_penSize; }
	QColor BrushColor() const { return m_brushColor; }

	bool   Fill() const { return m_fill; }
	bool   Valid() const { return m_isValid; }


	QPainterPath getPaintShape() const;

protected slots: //

signals: 

private: 

	//!
	//! Create a parameter holding a certain position as X and Y values
	//! \param name The name of the position parameter
	//! \param parentGroup The name of the parameterGroup to add the position parameter to
	//! \param value The initial value of the new position parameter
	//! \return The newly created parameter
	//!
	ParameterGroup* createPositionParameter( const QString &name, const QPointF &value );

	//!
	//! 
	//!
	void setPositionParameter( ParameterGroup* positionParameter, QPointF position );

	//!
	//! 
	//!
	QPointF getPositionFromParameter( ParameterGroup* positionParameter );
	
	void positionParameterSetKey( ParameterGroup* positionParameter );

	template <typename T>
	T getParameterValue( NumberParameter* parameter );
		
	QPointF stringToPosition( QString positionString ) const;
	QString positionToString( QPointF position );

private: // data

	// The data of the item
	QString m_itemName;
	BaseShapeItem::ItemType m_itemType;
	QPointF m_itemPosition;
	float m_itemRotation;
	QList<QPointF> m_itemPoints;
	QList<QPointF> m_itemControlPoints;
	int m_visibleFrame;


	// the keys of the item

	//QList<Key> m_positionKeys;
	//QList<Key> m_rotationKeys;

	QStringList m_paintShape;

	int    m_penSize;
	int    m_opacity;
	QColor m_penColor;
	QColor m_brushColor;
	bool   m_fill;
	bool   m_isValid;

	// time value uses for keying
	int m_time;
};

} // end namespace PainterPanel

#endif
