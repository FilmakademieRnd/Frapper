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
//! \file "ConnectionGraphicsItem.h"
//! \brief Header file for ConnectionGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.11.2013 (last updated)
//!

#ifndef CONNECTIONGRAPHICSITEM_H
#define CONNECTIONGRAPHICSITEM_H

#include "FrapperPrerequisites.h"
#include "Connection.h"
#include "NodeGraphicsItem.h"
#include <QGraphicsLineItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QWidget>
#include <QPainter>
#include <QMenu>

namespace Frapper {

//!
//! Class for graphical representation of connections in the node framework.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph ConnectionGraphicsItem {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     ConnectionGraphicsItem [label="ConnectionGraphicsItem",fillcolor="grey75"];
//!     QObject -> ConnectionGraphicsItem;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!     QGraphicsLineItem -> ConnectionGraphicsItem;
//!     QGraphicsLineItem [label="QGraphicsLineItem",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qgraphicslineitem.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT ConnectionGraphicsItem : public QObject, public QGraphicsLineItem
{

    Q_OBJECT

private: // static data

    //!
    //! Temporary Connection object that is used when creating a connection
    //! between nodes.
    //!
    static ConnectionGraphicsItem *s_tempItem;

public: // static functions

    //!
    //! Returns the temporary Connection object that is used when creating a
    //! connection between nodes.
    //!
    //! \return The temporary Connection object that is used when creating a connection between nodes.
    //!
    static ConnectionGraphicsItem * getTempItem ();

    //!
    //! Sets the temporary Connection object that is used when creating a
    //! connection between nodes to the given object.
    //!
    //! \param connection The temporary Connection object that is used when creating a connection between nodes.
    //!
    static void setTempItem ( ConnectionGraphicsItem *connection );

    //!
    //! Returns whether a connection between nodes is currently being created.
    //!
    //! \return True if a connection between nodes is currently being created, otherwise False.
    //!
    static bool isConnectionCreated ();

public: // constructors and destructors

    //!
    //! Constructor of the ConnectionGraphicsItem class.
    //!
    //! \param name The name of the item.
    //! \param color The color to use for painting the item.
    //! \param startPoint The point where the connection line begins.
    //! \param endPoint The point where the connection line ends.
    //!
    ConnectionGraphicsItem ( const QString &name, const QColor &color, const QPointF &startPoint, const QPointF &endPoint );

    //!
    //! Destructor of the ConnectionGraphicsItem class.
    //!
    ~ConnectionGraphicsItem ();

public: // functions

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
    //! Enables or disables the graphics item.
    //!
    //! \param enabled The new enabled state for the item.
    //!
    void setEnabled ( bool enabled );

    //!
    //! Paints the graphics item into a graphics view.
    //!
    //! \param painter The object to use for painting.
    //! \param option Style options for painting the graphics item.
    //! \param widget The widget into which to paint the graphics item.
    //!
    virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

    //!
    //! Updates the path end points according to the positions of start and end
    //! nodes.
    //!
    void updatePath ();

    //!
    //! Returns the Connection object that the connection graphics item represents.
    //!
    //! \return The Connection object that the connection graphics item represents.
    //!
    Connection * getConnection () const;

    //!
    //! Sets the Connection object that the connection graphics item represents.
    //!
    //! \param connection The Connection object that the connection graphics item represents.
    //!
    void setConnection ( Connection *connection );

    ////!
    ////! Gets the connection list.
    ////!
    //QMap<QString, Connection *> getConnections ();

    ////!
    ////! Adds a new connection.
    ////!
    //void addConnection ( Connection *connection );

    ////!
    ////! Removes a connection.
    ////!
    //void removeConnection ( const QString &name );

    //!
    //! Gets the startnode of the connection.
    //!
    NodeGraphicsItem * getStartNodeItem ();

    //!
    //! Sets the startnode of the connection.
    //!
    void setStartNodeItem ( NodeGraphicsItem *item );

    //!
    //! Gets the endnode of the connection.
    //!
    NodeGraphicsItem * getEndNodeItem() const;

    //!
    //! Sets the endnode of the connection.
    //!
    void setEndNodeItem ( NodeGraphicsItem *item );

    //!
    //! Gets the start point of the connection.
    //!
    QPointF getStartPoint () const;

    //!
    //! Sets the start point of the connection.
    //!
    void setStartPoint ( const QPointF &startPos );

    //!
    //! Gets the end point of the connection.
    //!
    QPointF getEndPoint () const;

    //!
    //! Sets the end point of the connection.
    //!
    void setEndPoint ( const QPointF &endPos );

    //!
    //! Updates the position of an end point for a connection that is currently
    //! being created.
    //!
    //! \param scenePosition The position of the mouse pointer over the graphics view in scene coordinates.
    //!
    void updatePosition ( const QPointF &scenePosition );

	//!
	//! Returns the name of the item.
	//!
	//! \return The Name of the item.
	//!
	const QString getName () const;

    public slots: //

        //!
        //!
        //!
        //!
        //!
        //void contextClicked ( QAction *action );

signals: //

        //!
        //!
        //!
        //!
        //!
        void nodeGraphicsItemSelected ( const QString &name );

        //!
        //! Signal that is emitted when the connection item is being destroyed.
        //!
        void destroyed ( Connection *connection );

        //!
        //! Signal that is emitted when the connection item is being destroyed.
        //!
        void destroyed ( ConnectionGraphicsItem *connectionItem );

protected: // functions

    //!
    //! Process hover enter events.
    //!
    void hoverEnterEvent ( QGraphicsSceneHoverEvent *event );

    //!
    //! Process hover leave events.
    //!
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent *event );

    //!
    //!
    //!
    //QVariant itemChange ( GraphicsItemChange change, const QVariant &value );

private: // functions

    //!
    //! Sets the temp flag of the connection.
    //!
    void setTemp ( bool temp );

    ////!
    ////! Process contextMenuEvents.
    ////!
    //void contextMenuEvent ( QGraphicsSceneContextMenuEvent *event );

private: // data
	
    //!
    //! Flag for temporary connection item.
    //!
    bool m_temp;

    //!
    //! Flag for mouse hovers over item.
    //!
    bool m_hovered;

	//!
    //! Flag that states whether a current update has been initiated by this
    //! object itself.
    //!
    bool m_selfInitiated;

	//!
    //! Pointer to Connection. (Deprecated.)
    //!
    Connection *m_connection;

	//!
    //! Pointer to NodeGraphicsItem.
    //!
    NodeGraphicsItem *m_startNodeItem;

    //!
    //! Pointer to NodeGraphicsItem.
    //!
    NodeGraphicsItem *m_endNodeItem;

    //!
    //! The name of the connection.
    //!
    QString m_name;

    //!
    //! The color to use for painting the item.
    //!
    QColor m_color;

    //!
    //! The arrow head shape to use for painting the item.
    //!
    QPolygonF m_arrowHeadPolygon;

    //!
    //! The path to return when the shape of the item is requested.
    //!
    QPainterPath m_shapePath;

	//!
    //! The path to use for painting the curved path.
    //!
    QPainterPath m_mainPath;

    //!
    //! The path to use for painting the shadow of the curved path.
    //!
    QPainterPath m_shadowPath;

    //!
    //! The start point of the path.
    //!
    QPointF m_startPoint;

    //!
    //! The end point of the path.
    //!
    QPointF m_endPoint;

    //!
    //! List of Connections.
    //!
    QMap<QString, Connection *> m_connections;

    ////!
    ////! Pointer to context menu.
    ////!
    //QMenu m_contextMenu;

};

} // end namespace Frapper

#endif
