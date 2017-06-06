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
//! \date       06.09.2012 (last updated)
//!

#ifndef PAINTERGRAPHICSITEMS_H
#define PAINTERGRAPHICSITEMS_H

#include "FrapperPrerequisites.h"

#include <QCursor>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>
#include <QAbstractGraphicsShapeItem>
#include <QtCore/QEvent>

namespace PainterPanel {

/************************************************************************/
/* BaseShapeItem                                                        */
/************************************************************************/
    //!
    //! BaseShapeItem class
    //!
    //! This class provides objects handles for bounding points, translation and rotation
    //! Implement functions addPosition( QPointF pos ) and updatePosition( QPointF pos ) for custom items
    //!
    //! If you override updateShape() for complex shapes, don't forget to call prepareGeometryChange() at the beginning
    //! and BaseShapeItem::updateShape() at the end. This file also contains some examples.
    //!
    class BaseShapeItem: public QObject, public QAbstractGraphicsShapeItem
    {
		Q_OBJECT

	protected: // inner classes

		/************************************************************************/
		/* PointHandle                                                          */
		/************************************************************************/
		//!
		//! The PointHandle class provides handles to update its parents bounding points
		//!
		class PointHandle : public QGraphicsRectItem
		{
			public:

				//! Constructor of the PointHandle class.
				PointHandle( QPointF position, BaseShapeItem* parent, QColor penColor = Qt::gray);

			protected:

				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

				virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

			private:

				//! The parent of this handle
				BaseShapeItem* m_parent;

		}; // PointHandle Class

		/************************************************************************/
		/* CurvePointHandle                                                      */
		/************************************************************************/
		//!
		//! The CurvePointHandle Class represents curves positions
		//!	and their bending through two handle positions
		//!
		class CurvePointHandle : public PointHandle
		{

			/************************************************************************/
			/* ControlPointHandle                                                   */
			/************************************************************************/
			//!
			//! The PointHandle class provides handles to update its parents bounding points
			//!
			class ControlPointHandle : public QGraphicsRectItem
			{
			public:

				//! Constructor of the PointHandle class.
				ControlPointHandle( QPointF position, CurvePointHandle* parent, QColor penColor = Qt::gray);

			protected:

				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

				//! Override event handler to listen to mouse press events
				//! \param event the graphics scene mouse event to listen to
				virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

				//! Paint the item with the given pen and brush
				virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

			private:

				//! The parent of this handle
				CurvePointHandle* m_parent;

			}; // ControlPointHandle Class

		public:

			//! Constructor of the CurvePointHandle class.
			CurvePointHandle( QPointF position, BaseShapeItem* parent, QColor curveColor = Qt::blue, QColor handleColor = Qt::red );

			//! Destructor of the CurvePointHandle class.
			~CurvePointHandle();

			//! Get the Position of handle point one
			QPointF getHandleOnePosition();

			//! Get the Position of handle point one in scene coordinates
			QPointF getHandleOneScenePosition();

			//! Get the Position of handle point two
			QPointF getHandleTwoPosition();

			//! Get the Position of handle point two in scene coordinates
			QPointF getHandleTwoScenePosition();

			//! Sets the control curve handles movable
			void setHandlesMovable( bool moveable = true );

			//! Set Visibility of the Handle Items
			void setHandlesVisible( bool visible = true );

			//! Set Selection of the Handle Items
			void setHandlesSelected( bool selected = true );

			//! Sets the Position of the first handle point
			//!
			//! \param position the new position in scene coordinates
			//! \param invert if the position should be point mirrored to the curve
			void setHandleOnePosition( QPointF position, bool invert = false);



			//! Sets the Position of the second handle point
			//!
			//! \param position the new position in scene coordinates
			//! \param invert if the position should be point mirrored to the curve
			void setHandleTwoPosition( QPointF position, bool invert = false);

		protected:

			//! Override event handler to listen to mouse move events
			//! \param event the graphics scene mouse event to listen to
			virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

			//! Override event handler to listen to mouse move events
			//! \param event the graphics scene mouse event to listen to
			virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

			virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

		private:

			//! The parent of this handle
			BaseShapeItem* m_parent;

			//! The first handle of the curve point
			ControlPointHandle* m_handleOnePosition;

			//! The second handle of the curve point
			ControlPointHandle* m_handleTwoPosition;


		}; // CurvePointHandle Class

	protected: // inner classes

		/************************************************************************/
		/* TranslationHandle                                                    */
		/************************************************************************/
		//!
		//! The TranslationHandle class provides handles to update its parents position
		//!
		class TranslationHandle : public QGraphicsRectItem
		{
			public:
				//! Constructor of the TranslationHandle class.
				TranslationHandle( BaseShapeItem* parent );

			protected:

				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );

				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );

				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

			private:
				//! The parent of this handle
				BaseShapeItem* m_parent;

		}; // TranslationHandle Class

	protected: // inner classes

		/************************************************************************/
		/* RotationHandle                                                       */
		/************************************************************************/
		//!
		//! The RotationHandle class provides handles to update its parents orientation
		//!
		class RotationHandle : public QGraphicsEllipseItem
		{
			public:
				//! Constructor of the RotationHandle class.
				RotationHandle( BaseShapeItem* parent);

				//! Get rotation value of handle
				qreal getRotationValue();

				//! Set rotation value of handle
				void setRotationValue( qreal angle );

			protected:
				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );

				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );

				//! Override event handler to listen to mouse move events
				//! \param event the graphics scene mouse event to listen to
				virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

			private:
				//! The parent of this handle
				BaseShapeItem* m_parent;

				//! The current orientation of the parent item
				qreal m_angle;

		}; //RotationHandle Class


	/************************************************************************/
	/* BaseShapeItem                                                        */
	/************************************************************************/

    public: // constructors and destructor

        //!
        //! Constructor of the BaseShapeItem class.
        //!
        //! \param penColor The parent widget the created instance will be a child of.
        //! \param penWidth Extra widget options.
        //!
        BaseShapeItem ();

        //!
        //! Destructor of the PainterPanel class.
        //!
        //! Defined virtual to guarantee that the destructor of a derived class
        //! will be called if the instance of the derived class is saved in a
        //! variable of its parent class type.
        //!
        virtual ~BaseShapeItem();

    public: // enumerations
        enum ItemType { 
            BASE=0,
            PAINT, LINE, RECT, CIRCLE, ELLIPSE, POLYGON, POLYLINE, BEZIER_CURVE, ERASE,
            NUM_ITEMTYPES
        };

        enum { Type = UserType + 1 };

	signals:
		void itemPositionChanged();
		void itemRotationChanged();
		void itemPointsChanged();

    public: // virtual functions
        
        //! Enable the use of qgraphicsitem_cast with this item.
        virtual int type();

        //! The item type is used to distinguish between the different types of items
        virtual ItemType GetItemType();

		//! Call this function to update the shape of the item.
		//! This function updates the position of the translation and rotation handle.
		virtual void updateShape();

		//!
		//! Returns the bounding rectangle of the graphics item.
		//!
		//! \return The bounding rectangle of the graphics item.
		//!
		virtual QRectF boundingRect () const;

		//!
		//! Returns the shape path of an item.
		//!
		virtual QPainterPath shape () const;

		//!
		//! Returns a boolean if the Item is colliding with the given path 
		//!
		virtual bool collidesWithPath(const QPainterPath &path, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;

        //! Add a new position to this item
        virtual void addPosition( QPointF pos ) = 0;

        //! Update the current position of this item
        virtual void updatePosition( QPointF pos ) = 0;

        //! Forget the last position provided by add-/updatePosition()
        //! \param emptyItem Set to true, if this operation creates an empty item, otherwise false
        virtual void skipLastPosition( bool& emptyItem );

        //! Paint the item with the specified pen and brush
        //! Use a yellow pen when the item is selected
        virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		//! return a descriptive text for this item depending on its type, if derived
        QString getDescriptionText();

		//! return the name of this item
		QString getName();

		//! set the items name
		void setName( const QString& name );

		//! return the rotation angle of this item
		qreal getRotation();

		//! set the rotation angle of this item
		void setRotation( qreal angle );

		//! return the position of this item
		QPointF getPosition();

		//! set the position of this item
		void setPosition( QPointF position );

		//! return the control points of this item
		virtual QList<QPointF> getPoints();

		//! set the control points of this item
		void setPoints( const QList<QPointF>& points );

		const QPainterPath& getPainterPath();

		void setPainterPath( const QPainterPath& path );

		//! toggles if the item is in cubic transform mode (bezier curve)
		void setCubicTransformMode( bool value );

		//! returns if the item is in cubic transform mode (bezier curve)
		bool getCubicTransformMode();

		//! Sets the control points of a bezier curve in cubic transform mode
		void setControlPointsPosition( QPointF scenePos );


		//! Set flag to use scenes LUT for drawing
		static void setUseLUT( bool val);
		const QColor lut( int i) const;
		//! Not the accurate position while moving!
		//! This is the position saved for changing parameters and redo/undo
		QPointF getSavedPosition() { return m_savedPosition; }
		void setSavedPosition( QPointF pos ) { m_savedPosition = pos; } 

		//! Not the accurate position while moving!
		//! This is the distance since last move action saved for changing parameters and redo/undo
		QPointF getSavedDistance() { return m_savedDistance; }
		void setSavedDistance( QPointF pos ) { m_savedDistance = pos; } 

		//! Not the accurate rotation while moving!
		//! This is the rotation saved for changing parameters and redo/undo
		float getSavedRotation() { return m_savedRotation; }
		void setSavedRotation( float rot ) { m_savedRotation = rot; } 
    protected: // protected functions

        //! React on changes of the item
        virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

		virtual void signalItemPositionChange();
		virtual void signalItemRotationChange();
		virtual void signalItemPointsChange();

		//! calculate the convex hull
		virtual void calculateConvexHull();

        //! Show/Hide handles of the item
        virtual void showHandles( bool show = true );

    protected: // data

		//! the name of this item
		QString m_name;

        //! The shape of the item to draw
        QPainterPath m_shape;

        //! The list of point handles
        QList<PointHandle*> m_points;

        //! The handle to rotate this shape item
        RotationHandle* m_rotateHandle;

        //! The handle to translate this shape item
        TranslationHandle* m_translateHandle;

		//! Saves if the item (bezier curve) is currently
		//! in the mode where the cubic control handles are moved
		bool m_inCubicTransformMode;

		//! The Convex Hull of the Item for hit testing
		QPainterPath m_convexHullShape;

		//! Boolean if the convex hull needs to be recalculated
		bool m_convexHullDirty;

		//! Flag for drawing items using the scenes LUT
		static bool s_useLUT;
		//! The saved position for moving commands, it is the position
		//! where the item was last time the movement has been started
		QPointF m_savedPosition;

		//! The saved distance for moving commands, it is the distance
		//! the item has traveled within this actual active movement
		QPointF m_savedDistance;

		//! The saved rotation for rotation commands, before actual rotation
		float m_savedRotation;    }; //BaseShapeItem Class


/************************************************************************/
/* PaintItem                                                            */
/************************************************************************/
    //!
    //! PaintItem class
    //!
    //! This class implements an item which stores all incoming positions in its shape
    //! and therefore follows the mouse events like a painter

    class PaintItem : public BaseShapeItem
    {

    public:
        //! Constructor of the PaintItem class.
        PaintItem( QPointF startPos );

        //! Handle position updates
        //! \param pos The position to append to the shape
        void updatePosition( QPointF pos );

        //! Append position to the shape
        //! \param pos The position to append to the shape
        void addPosition( QPointF pos );

		//! Call this function to update the shape of the item.
		void updateShape();

		void erase( QPainterPath eraserShape );

		//!
		//! Returns a boolean if the Item is colliding with the given path 
		//!
		bool collidesWithPath ( const QPainterPath &path, Qt::ItemSelectionMode mode ) const;

        //! Get the type of this item
        virtual ItemType GetItemType();

    }; //PaintItem Class


/************************************************************************/
/* EraseItem                                                            */
/************************************************************************/
	//!
	//! EraseItem class
	//!
	//! This class implements an item which stores all incoming positions in its shape
	//! and therefore follows the mouse events like a painter

	class EraseItem : public BaseShapeItem
	{

	public:
		//! Constructor of the EraseItem class.
		EraseItem( QPointF startPos );

		//! Handle position updates
		//! \param pos The position to append to the shape
		void updatePosition( QPointF pos );

		//! Append position to the shape
		//! \param pos The position to append to the shape
		void addPosition( QPointF pos );

		//! Get the type of this item
		virtual ItemType GetItemType();

		int Size() const { return m_size; }
		void Size(int val) { m_size = val; }

	protected:
		void updateShape( QPointF pos );

		virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

		int m_size;
	}; //EraseItem Class

/************************************************************************/
/* LineItem                                                             */
/************************************************************************/
    //!
    //! LineItem class
    //!
    //! This class implements an item which represents a line by two positions
    class LineItem : public BaseShapeItem
    {

    public:
        //! Constructor of the LineItem class.
        LineItem( QPointF startPos );

        //! Update the second position of the item
        //! \param pos The new second position of the line
        void updatePosition( QPointF pos );
        
        //! Update the second position of the item
        //! \param pos The new second position of the line
        void addPosition( QPointF pos );

        //! Call this function to update the shape of the item.
        void updateShape();

        //! Get the type of this item
        virtual ItemType GetItemType();

    }; //LineItem Class


/************************************************************************/
/* RectItem                                                             */
/************************************************************************/
    //!
    //! RectItem class
    //!
    //! This class implements an item which represents a rectangle by two positions
    class RectItem : public BaseShapeItem
    {

    public:
        //! Constructor of the RectItem class.
        RectItem( QPointF startPos );

        //! Update the second position of the item
        //! \param pos The new second position of the rectangle
        void updatePosition( QPointF pos );

        //! Update the second position of the item
        //! \param pos The new second position of the rectangle
        void addPosition( QPointF pos );

        //! Call this function to update the shape of the item.
        void updateShape();

        //! Get the type of this item
        virtual ItemType GetItemType();

    }; //RectItem Class


/************************************************************************/
/* CircleItem                                                           */
/************************************************************************/
    //!
    //! CircleItem class
    //!
    //! This class implements an item which represents a circle by two positions
    class CircleItem : public BaseShapeItem
    {

    public:
        //! Constructor of the CircleItem class.
        CircleItem( QPointF startPos );

        //! Update the second position of the item
        //! \param pos The new second position of the circle
        void updatePosition( QPointF pos );

        //! Update the second position of the item
        //! \param pos The new second position of the circle
        void addPosition( QPointF pos );

		//! Calculating the convex hull of the item
		void calculateConvexHull( );

        //! Call this function to update the shape of the item.
        void updateShape();

        //! Get the type of this item
        virtual ItemType GetItemType();

    }; //CircleItem Class


/************************************************************************/
/* EllipseItem                                                          */
/************************************************************************/
    //!
    //! EllipseItem class
    //!
    //! This class implements an item which represents an ellipse by two positions
    class EllipseItem : public BaseShapeItem
    {

    public:
        //! Constructor of the EllipseItem class.
        EllipseItem( QPointF startPos );

        //! Update the second position of the item
        //! \param pos The new second position of the ellipse
        void updatePosition( QPointF pos );

        //! Update the second position of the item
        //! \param pos The new second position of the ellipse
        void addPosition( QPointF pos );

		//! Calculating the convex hull of the item
		void calculateConvexHull( );

        //! Call this function to update the shape of the item.
        void updateShape();

        //! Get the type of this item
        virtual ItemType GetItemType();

    }; //EllipseItem Class


/************************************************************************/
/* PolyLineItem                                                         */
/************************************************************************/
    //!
    //! PolyLineItem class
    //!
    //! This class implements an item which represents a polyline by several positions
    class PolyLineItem : public BaseShapeItem
    {

    public:
        //! Constructor of the PolyLineItem class.
        PolyLineItem( QPointF startPos );

        //! Adds a new position to the polyline
        //! \param pos The new position to append to the polyline
        void addPosition( QPointF pos );

        //! Updates the last added position of the polyline
        //! \param pos The new position of the last position of the polyline
        void updatePosition( QPointF pos );

        //! Skip the last position of the polyline
        //! \param emptyItem Set to true, if this operation creates an empty item, otherwise false
        void skipLastPosition( bool& emptyItem );

        //! Call this function to update the shape of the item.
        virtual void updateShape();

        //! Get the type of this item
        virtual ItemType GetItemType();

    }; //PolyLineItem Class


	/************************************************************************/
	/* Bezier Curve Item                                                    */
	/************************************************************************/
	//!
	//! BezierCurveItem class
	//!
	//! This class implements an item which represents a curve of degree 3 by several positions 
	//! and 2 additional control points per position
	class BezierCurveItem : public BaseShapeItem
	{
	public:
		//! Constructor of the CurveItem class.
		BezierCurveItem( QPointF startPos );

		//! Destructor of the CurveItem class.
		virtual ~BezierCurveItem();

		//! Updates the last added position of the curve
		//! \param pos The new position of the last position of the curve
		virtual void updatePosition( QPointF pos );

		//! Skip the last position of the curve
		//! \param emptyItem Set to true, if this operation creates an empty item, otherwise false
		virtual void skipLastPosition( bool& emptyItem );

		//! Adds a new position to the curve
		//! \param pos The new position to append to the curve
		virtual void addPosition( QPointF pos );

		//! Sets the input list as new positions for the curve handles
		void setHandlePoints( const QList<QPointF>& list );

		//! Returns a list of positions for the curve handles
		QList<QPointF> getHandlePoints( );

		//! Returns a list of positions for the curve positions
		virtual QList<QPointF> getPoints( );

		//! Call this function to update the shape of the item.
		virtual void updateShape();

		//! Get the type of this item
		virtual ItemType GetItemType();

		//! Returns the Position of a Curve Point in the Point List from his position in scene
		void BezierCurveItem::insertCurvePointAtPosition ( QPointF position, CurvePointHandle* newHandle );

		//! removes curve points at a specific position
		void removeCurvePoint( int i );

	private: // functions

		//! Paint the curveItem with the given pen and brush
		//! If selected, also the control polygon will be drawn
		virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		
	protected:
		//! The polygon defined by the control points of the curve
		QPainterPath m_controlShape;

	}; //BezierCurveItem Class

	/************************************************************************/
	/* PolygonItem                                                          */
	/************************************************************************/
	//!
	//! PolygonItem class
	//!
	//! This class implements an item which represents a PolygonItem by several positions
	class PolygonItem : public BezierCurveItem
	{

	public:
		//! Constructor of the PolygonItem class.
		PolygonItem( QPointF startPos );

		//! Call this function to update the shape of the item.
		void updateShape();

		//! Get the type of this item
		virtual ItemType GetItemType();

	}; // PolygonItem Class

} // end namespace PainterPanel

#endif // PAINTERGRAPHICSITEMS_H