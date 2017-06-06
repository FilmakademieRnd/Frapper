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
//! \file "NodeGraphicsItem.h"
//! \brief Header file for NodeGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       26.05.2009 (last updated)
//!

#ifndef NODEGRAPHICSITEM_H
#define NODEGRAPHICSITEM_H

#include "FrapperPrerequisites.h"
#include "BaseRectItem.h"
#include "Node.h"
#include "FlagGraphicsItem.h"
#include "ViewFlagGraphicsItem.h"
#include "PinGraphicsItem.h"
#include "instancecountermacros.h"
#include <QGraphicsSceneContextMenuEvent>
#include <QWidget>
#include <QMenu>

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
    //!   digraph NodeGraphicsItem {
    //!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
    //!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
    //!
    //!     NodeGraphicsItem [label="NodeGraphicsItem",fillcolor="grey75"];
    //!     QObject -> NodeGraphicsItem;
    //!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
    //!     BaseRectItem -> NodeGraphicsItem;
    //!     BaseRectItem [label="BaseRectItem",URL="class_base_rect_item.html"];
    //!   }
    //! \enddot
    //! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
    //!
    class FRAPPER_CORE_EXPORT NodeGraphicsItem : public BaseRectItem
    {

        Q_OBJECT
            ADD_INSTANCE_COUNTER

    public: // nested enumerations

        //!
        //! Nested enumeration of pin display modes that define how many pins will
        //! be displayed in the node graphics item.
        //!
        enum PinDisplayMode {
            PDM_All,
            PDM_Connected,
            PDM_Collapsed
        };

    protected: // static constants

        //!
        //! The standard width for the items.
        //!
        static const int ItemWidth = 60;

        //!
        //! The base height for the items.
        //!
        static const int ItemBaseHeight = 44;

        //!
        //! The height of a row of pins.
        //!
        static const int ItemPinRowHeight = 12;

    private: // static data

        //!
        //! The number of node graphics items created for the current scene.
        //!
        static int s_numberOfNodeGraphicsItems;

    public: // constructors and destructors

        //!
        //! Constructor of the NodeGraphicsItem class.
        //!
        //! \param node The node that the graphics item represents.
        //! \param position The initial position for the graphics item.
        //! \param color The color in which to draw the graphics item.
        //!
        NodeGraphicsItem ( Node *node, QPointF position, const QColor &color );

		//!
		//! Constructor of the NodeGraphicsItem class.
		//!
		//! \param node The node that the graphics item represents.
		//! \param position The initial position for the graphics item.
		//! \param width The width of the graphics item
		//! \param height The height of the graphics item
		//! \param color The color in which to draw the graphics item.
		//!
		NodeGraphicsItem ( Node *node, QPointF position, const int width, const int height, const QColor &color );

        //!
        //! Destructor of the NodeGraphicsItem class.
        //!
        virtual ~NodeGraphicsItem ();

    public: // functions

        //!
        //! Enables or disables the graphics item.
        //!
        //! \param enabled The new enabled state for the item.
        //!
        virtual void setEnabled ( bool enabled );

        //!
        //! Paints the graphics item into a graphics view.
        //!
        //! \param painter The object to use for painting.
        //! \param option Style options for painting the graphics item.
        //! \param widget The widget into which to paint the graphics item.
        //!
        virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

        //!
        //! Returns the node that the graphics item represents.
        //!
        //! \return The node that the graphics item represents.
        //!
        virtual Node * getNode () const;

        //!
        //! Sets the z-index of the graphics item so that it will be drawn over all
        //! other graphics items in the scene.
        //!
        virtual void bringToFront ();

        //!
        //! Associates a ConnectionGraphicsItem with the node graphics item.
        //!
        //! \param item The item to associate with the node graphics item.
        //!
        virtual void addConnectionItem ( ConnectionGraphicsItem *item );

        //!
        //! Removes the association to the given ConnectionGraphicsItem from the
        //! node graphics item.
        //!
        //! \param item The item to no longer associate with the node graphics item.
        //!
        virtual void removeConnectionItem ( ConnectionGraphicsItem *item );

		//!
		//! Switch if a Nodes connection will be deteted together with the graphics 
		//! item (global default = true).
		//!
		virtual void deleteConnectionsWithItem (const bool value);

        //!
        //! Removes all ConnectionGraphicsItem associations from the node graphics
        //! item.
        //!
        virtual void removeConnectionItems ();

        //!
        //! Sets the pin display mode for the item to the given mode.
        //!
        //! \param pinDisplayMode The pin display mode to set for the item.
        //!
        virtual void setPinDisplayMode ( PinDisplayMode pinDisplayMode );

		//!
		//! Returns the name of the item.
		//!
		//! \return The Name of the item.
		//!
		const QString getName () const;

		//!
		//! Returns a list containing all ConnectionGraphicsItems connected to these node item.
		//!
		//! \return The list containing the ConnectionGraphicsItems.
		//!
		virtual const QList<ConnectionGraphicsItem *> getConnectionItemList() const;

        public slots: //

            //!
            //! Updates the selected state of the graphics item according to the given
            //! value.
            //!
            //! This function is needed because QGraphicsItem::setSelected() is not a
            //! slot.
            //!
            //! \param selected The new selected state for the graphics item.
            //!
            void setSelected ( bool selected );

            //!
            //! Begins or ends creating a connection between nodes.
            //! Is called when a parameter represented by a pin has been clicked.
            //!
            //! \param pinItem The pin graphics item representing the parameter.
            //! \param parameter The parameter that was clicked.
            //!
            void createConnection ( PinGraphicsItem *pinItem, Parameter *parameter );

            //!
            //! Refresh the graphics item to reflect changes in the node that it
            //! represents.
            //!
            virtual void refresh ();

signals: //

            //!
            //! Signal that is emitted when a connection between the given parameters
            //! should be created.
            //!
            //! The parameters to be connected must have the same parameter type.
            //!
            //! \param sourceParameter The output parameter of the source node to connect.
            //! \param targetParameter The input parameter of the target node to connect.
            //!
            void connectionRequested ( Parameter *sourceParameter, Parameter *targetParameter );

            //!
            //! Signal that is emitted when a connection between the given nodes
            //! should be created. (connect by name)
            //!
            //! The parameters to be connected must have the same parameter type.
            //!
            //! \param sourceNode The source node.
            //! \param targetNode The target node.
            //!
            void connectionRequested ( Node *sourceNode, Node *targetNode );

            //!
            //! Signal that is emitted when the item has been moved to a new position.
            //!
            //! \param pos The position that the item has moved to.
            //!
            void nodeGraphicsItemMoved ( const QPointF &pos );

    protected: // functions

        //!
        //! Event handler that reacts to mouse press events.
        //!
        //! \param event The object containing details about the event.
        //!
        void mousePressEvent ( QGraphicsSceneMouseEvent *event );

        //!
        //! Event handler that reacts to mouse release events.
        //!
        //! \param event The object containing details about the event.
        //!
        void mouseReleaseEvent ( QGraphicsSceneMouseEvent *event );

        //!
        //! Event handler that reacts to context menu events.
        //!
        //! \param event The object containing details about the event.
        //!
        virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent *event );

        //!
        //! Processing on changed item.
        //!
        QVariant itemChange ( GraphicsItemChange change, const QVariant& value );

    private: // functions

        //!
        //! Updates the item according to the pin display mode set to display all
        //! pins, only connected pins, or all pins in one collapsed pin.
        //!
        virtual void updatePins ();

        //!
        //! Disables all pins in the node item that are of a different parameter type
        //! or the same connector types as the parameter that was clicked.
        //! Also disables all of the node item's flags.
        //! The whole node will be disabled if there are no enabled pins left.
        //!
        //! \param pinItem The pin graphics item representing the parameter that was clicked.
        //! \param parameter The parameter that was clicked.
        //!
        virtual void disableOtherPins ( PinGraphicsItem *pinItem, Parameter *parameter );

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
        virtual void beginCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter );

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
        virtual void endCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter );

        private slots: //

            //!
            //! Connect by name has been clicked in context menu.
            //!
            void connectByNameClicked ();

            //!
            //! Updates the node's pin display mode according to the triggered action.
            //!
            //! \param action The action that was triggered.
            //!
            void updatePinDisplayMode ( QAction *action );

    protected: // data

		//!
        //! The flag that controls if the Nodes connection will be deteted together 
		//! with the graphics item
        //!
		bool m_deleteConnection;

        //!
        //! The node that the graphics item represents.
        //!
        QList<Node *> m_nodes;

        //!
        //! The node's eval flag item.
        //!
        FlagGraphicsItem *m_evalFlagItem;

        //!
        //! The node's view flag item.
        //!
        ViewFlagGraphicsItem *m_viewFlagItem;

        //!
        //! The pin display mode that defines how many pins will be displayed in
        //! the node graphics item
        //!
        PinDisplayMode m_pinDisplayMode;

        //!
        //! The list of input pins to display in the item.
        //!
        QList<PinGraphicsItem *> m_inputPins;

        //!
        //! The list of output pins to display in the item.
        //!
        QList<PinGraphicsItem *> m_outputPins;

        //!
        //! All ConnectionGraphicsItems connected to this item.
        //!
        QMap<unsigned int, ConnectionGraphicsItem *> m_connectionItems;

        //!
        //! The node context menu.
        //!
        QMenu m_nodeContextMenu;

        //!
        //! The pin context menu.
        //!
        QMenu m_pinContextMenu;
    };

} // end namespace Frapper

#endif
