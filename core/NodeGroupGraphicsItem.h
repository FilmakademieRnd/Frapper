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
//! \file "NodeGroupGraphicsItem.h"
//! \brief Header file for NodeGroupGraphicsItem class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       09.07.2013 (last updated)
//!

#ifndef NODEGROUPGRAPHICSITEM_H
#define NODEGROUPGRAPHICSITEM_H

#include "NodeGraphicsItem.h"


namespace Frapper {

    //!
    //! Forward declaration for connection graphics items.
    //!
    class ConnectionGraphicsItem;


    //!
    //! Class for graphical representation of scene objects.
    //!
    //! <div align="center"><b>Inheritance Diagram</b></div>
    //!
    //! \dot
    //!   digraph NodeGroupGraphicsItem {
    //!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
    //!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
    //!
    //!     NodeGroupGraphicsItem [label="NodeGroupGraphicsItem",fillcolor="grey75"];
    //!     QObject -> NodeGroupGraphicsItem;
    //!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
    //!     BaseRectItem -> NodeGroupGraphicsItem;
    //!     BaseRectItem [label="BaseRectItem",URL="class_base_rect_item.html"];
    //!   }
    //! \enddot
    //! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
    //!
    class FRAPPER_CORE_EXPORT NodeGroupGraphicsItem : public NodeGraphicsItem
    {
        Q_OBJECT

    public: // constructors and destructors

        //!
        //! Constructor of the NodeGroupGraphicsItem class.
        //!
        //! \param node The node that the graphics item represents.
        //! \param position The initial position for the graphics item.
        //! \param color The color in which to draw the graphics item.
        //!
        NodeGroupGraphicsItem ( const QString &name, const QList<Node *> &nodeList, const QList<QPointF> &posList, const QColor &color );

        //!
        //! Destructor of the NodeGroupGraphicsItem class.
        //!
        virtual ~NodeGroupGraphicsItem ();

    public: // functions

        //!
        //! Paints the graphics item into a graphics view.
        //!
        //! \param painter The object to use for painting.
        //! \param option Style options for painting the graphics item.
        //! \param widget The widget into which to paint the graphics item.
        //!
        virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

		//!
		//! Returns the selected node.
		//!
		//! \param i The position of the node in list.
		//! 
		//! \return The node that the graphics item represents.
		//!
		Node * getNode (const int i) const;

		//!
		//! Returns the name og the group.
		//!
		const QString getName () const;

		//!
		//! Returns the nodes that the graphics item represents.
		//!
		//! \return The nodes that the graphics item represents.
		//!
		const QList<Node *> & getNodes () const;

		//!
		//! Returns the node positions of the containing nodes.
		//!
		//! \return The node positions of the containing nodes.
		//!
		const QPointF getNodeItemPosition (const QString &nodeName) const;

		//!
		//! Returns the node positions of the containing nodes.
		//!
		//! \return The node positions of the containing nodes.
		//!
		const QHash<QString, QPointF> getNodeItemPositions () const;

		//!
		//! Returns whether the group item contains the node with
		//! the given name or not.
		//!
		//! \return True the group item contains the node with
		//! the given name, flase otherwise.
		//!
		const bool containsNode(const QString &nodeName) const;

        //!
        //! Resets the node to 0.
        //!
        //! Should be called when the node that the graphics item represents will
        //! be destroyed.
        //!
        //! Temporary. Should be replaced by improved code when destroying nodes
        //! in the scene model.
        //!
        void resetNode ();


        public slots: //

            //!
            //! Refresh the graphics item to reflect changes in the node that it
            //! represents.
            //!
            virtual void refresh ();


    protected: // functions

        //!
        //! Event handler that reacts to context menu events.
        //!
        //! \param event The object containing details about the event.
        //!
        virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent *event );


    private: // functions

        //!
        //! Updates the item according to the pin display mode set to display all
        //! pins, only connected pins, or all pins in one collapsed pin.
        //!
        void updatePins ();

        //!
        //! Disables all pins in the node item that are of a different parameter type
        //! or the same connector types as the parameter that was clicked.
        //! Also disables all of the node item's flags.
        //! The whole node will be disabled if there are no enabled pins left.
        //!
        //! \param pinItem The pin graphics item representing the parameter that was clicked.
        //! \param parameter The parameter that was clicked.
        //!
        void disableOtherPins ( PinGraphicsItem *pinItem, Parameter *parameter );

        //!
        //! Begins creating a connection between parameters of nodes by creating a
        //! new (temporary) connection graphics item that has the given pin as one
        //! of its end points.
        //! Is called when a parameter represented by a pin is clicked.
        //!
        //! \param pinItem The pin graphics item representing the parameter that was clicked.
        //! \param parameter The parameter that was clicked.
        //! \see parameterClicked
        //! \see endCreatingConnection
        //!
        void beginCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter );

        //!
        //! Ends creating a connection between parameters of nodes by keeping the
        //! temporarily created connection graphics item that has the given pin as
        //! one of its end points, or destroying it when creating the connection is
        //! aborted.
        //! Is called when a parameter represented by a pin is clicked.
        //!
        //! \param pinItem The pin graphics item representing the parameter that was clicked.
        //! \param parameter The parameter that was clicked.
        //! \see parameterClicked
        //! \see beginCreatingConnection
        //!
        void endCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter );

		//!
		//! Internal function to estimate the view state of the node group.
		//!
		//! \return The estimated view state of the node.
		//!
		bool isViewed () const;

		//!
		//! Internal function to estimate the evaluation state of the node group.
		//!
		//! \return The estimated evaluation state of the node.
		//!
		bool isEvaluated () const;

		//!
		//! Internal function to estimate all source nodes of an node group.
		//!
		//! \return The estimated source nodes of an node group.
		//!
		QList<Node *> getInNodes() const;

		//!
		//! Internal function to estimate all sink nodes of an node group.
		//!
		//! \return The estimated sink nodes of an node group.
		//!
		QList<Node *> getOutNodes() const;

		//!
		//! Internal function to determine if an parameter of the node group is internal.
		//!
		//! \param parameter The Parameter to check.
		//! \return The estimated result.
		//!
		bool isInternal(AbstractParameter *parameter) const;

	private: //data
		//!
		//! The Name of the Item
		//!
		QString m_name;

		//!
		//! The list storing the ols node positions
		//!
		QHash<QString, QPointF> m_nodePositions;
    };

} // end namespace Frapper

#endif
