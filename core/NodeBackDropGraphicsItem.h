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
//! \file "NodeBackDropGraphicsItem.h"
//! \brief Header file for NodeBackDropGraphicsItem class.
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       06.03.2014 (last updated)
//!

#ifndef NODEBACKDROPGRAPHICSITEM_H
#define NODEBACKDROPGRAPHICSITEM_H

#include "NodeGraphicsItem.h"
#include "BackDropNode.h"
#include "Log.h"
#include <QGraphicsScene>
#include <QTextDocument>
#include <QtSvg/QSvgGenerator>

namespace Frapper {

    //!
    //! Class for graphical representation of scene objects.
    //!
    //! <div align="center"><b>Inheritance Diagram</b></div>
    //!
    //! \dot
    //!   digraph NodeBackDropGraphicsItem {
    //!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
    //!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
    //!
    //!     NodeBackDropGraphicsItem [label="NodeGroupGraphicsItem",fillcolor="grey75"];
    //!     QObject -> NodeBackDropGraphicsItem;
    //!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
    //!     BaseRectItem -> NodeBackDropGraphicsItem;
    //!     BaseRectItem [label="BaseRectItem",URL="class_base_rect_item.html"];
    //!   }
    //! \enddot
    //! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
    //!
    class FRAPPER_CORE_EXPORT NodeBackDropGraphicsItem : public BaseRectItem
    {
			Q_OBJECT

	// private inner class
	private:

		class BackDropCorner : public QGraphicsRectItem
		{

		public:

			BackDropCorner(QPointF position, NodeBackDropGraphicsItem *parent = 0);

			bool isInResizeMode();

			void prepareChange();

			void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ );

		private:

			virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
			virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
			virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

			NodeBackDropGraphicsItem* m_parent;

			bool m_resizeMode;
		};


    public: // constructors and destructors

		//!
		//! Constructor of the NodeGroupGraphicsItem class.
		//!
		//! \param node The node that the graphics item represents.
		//! \param position The initial position for the graphics item.
		//! \param color The color in which to draw the graphics item.
		//! \param width The width of the graphics item
		//! \param height The height of the graphics item

		//!
		NodeBackDropGraphicsItem ( const QString &name, Node *node, const QPointF position, const qreal width, const qreal height );

        //!
        //! Destructor of the NodeGroupGraphicsItem class.
        //!
       virtual ~NodeBackDropGraphicsItem ();

	public slots: // slots

		void setSelected(bool selection);

		void redraw();

    public: // functions

        //!
        //! Paints the graphics item into a graphics view.
        //!
        //! \param painter The object to use for painting.
        //! \param option Style options for painting the graphics item.
        //! \param widget The widget into which to paint the graphics item.
        //!
        virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

		void setWidth( qreal width );

		void setHeight ( qreal height );

		bool getLock();

		//!
		//! Returns the selected node.
		//!
		//! \param i The position of the node in list.
		//! 
		//! \return The node that the graphics item represents.
		//!
		Node * getNode () const;


		//!
		//! Returns whether the group item contains the node with
		//! the given name or not.
		//!
		//! \return True the group item contains the node with
		//! the given name, flase otherwise.
		//!
		const bool containsNode(const QString &nodeName) const;

		//!
		//! Returns the name of the item.
		//!
		//! \return The Name of the item.
		//!
		virtual const QString getName () const;

	protected:
		virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );

    private: // functions

		//!
		//! Internal function to determine if an parameter of the node group is internal.
		//!
		//! \param parameter The Parameter to check.
		//! \return The estimated result.
		//!
		bool isInternal(AbstractParameter *parameter) const;

		virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

	private: //data

		int m_headTextSize;

		int m_descriptionTextSize;
		
		//!
		//! The list storing pointers to all nodes in that backdrop
		//!
		Node* m_node;

		BackDropCorner*  m_corner; 

		QGraphicsTextItem* m_textItem;

		QString m_name;

		QString m_headText;

		QString m_descriptionText;

		QColor m_descriptionColor;

		QColor m_backgroundColor;
    };

} // end namespace Frapper

#endif
