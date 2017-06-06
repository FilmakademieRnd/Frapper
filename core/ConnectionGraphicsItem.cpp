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
//! \file "ConnectionGraphicsItem.cpp"
//! \brief Implementation file for ConnectionGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.11.2013 (last updated)
//!

#include "ConnectionGraphicsItem.h"
#include <QGraphicsScene>
#include <math.h>
#include "Log.h"

namespace Frapper {

///
/// Global Variables with File Scope
///


//!
//! The number Pi.
//!
static const float Pi = 3.141593f;



///
/// Private Static Data
///


//!
//! Temporary Connection object that is created when a connection is set up.
//!
ConnectionGraphicsItem *ConnectionGraphicsItem::s_tempItem = 0;


///
/// Public Static Functions
///


//!
//! Returns the temporary Connection object that is used when creating a
//! connection between nodes.
//!
//! \return The temporary Connection object that is used when creating a connection between nodes.
//!
ConnectionGraphicsItem * ConnectionGraphicsItem::getTempItem ()
{
    return s_tempItem;
}


//!
//! Sets the temporary Connection object that is used when creating a
//! connection between nodes to the given object.
//!
//! \param item The temporary Connection object that is used when creating a connection between nodes.
//!
void ConnectionGraphicsItem::setTempItem ( ConnectionGraphicsItem *item )
{
    if (s_tempItem)
        s_tempItem->setTemp(false);

    s_tempItem = item;

    if (s_tempItem)
        s_tempItem->setTemp(true);
}


//!
//! Returns whether a connection between nodes is currently being created.
//!
//! \return True if a connection between nodes is currently being created, otherwise False.
//!
bool ConnectionGraphicsItem::isConnectionCreated ()
{
    return s_tempItem != 0;
}


///
/// Constructors and Destructors
///


//!
//! Constructor of the ConnectionGraphicsItem class.
//!
//! \param name The name of the item.
//! \param color The color to use for painting the item.
//! \param startPoint The point where the connection line begins.
//! \param endPoint The point where the connection line ends.
//!
ConnectionGraphicsItem::ConnectionGraphicsItem ( const QString &name, const QColor &color, const QPointF &startPoint, const QPointF &endPoint ) :
QGraphicsLineItem(),
m_name(name),
m_color(color),
m_startNodeItem(0),
m_endNodeItem(0),
m_startPoint(startPoint),
m_endPoint(endPoint),
m_hovered(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    setAcceptHoverEvents(true);

    setZValue(-1);

    // set up signal/slot connection for context menu
    //connect(&m_contextMenu, SIGNAL(triggered(QAction *)), this, SLOT(contextClicked(QAction *)));
    //setCacheMode(DeviceCoordinateCache);
}


//!
//! Destructor of the ConnectionGraphicsItem class.
//!
ConnectionGraphicsItem::~ConnectionGraphicsItem ()
{
    if (m_startNodeItem)
        m_startNodeItem->removeConnectionItem(this);
    if (m_endNodeItem)
        m_endNodeItem->removeConnectionItem(this);

    emit destroyed(m_connection);
}


///
/// Public Slots
///


////!
////! Process the action when it is clicked in context menu.
////!
//void ConnectionGraphicsItem::contextClicked ( QAction *action )
//{
//    QString actionName (action->text());
//
//    QMap<QString, Connection*>::iterator cIter;
//    cIter = m_connections.find(actionName);
//    if (cIter != m_connections.end()) {
//        Connection *connection = cIter.value();
//        removeConnection(actionName);
//        if (m_connections.size() == 0) {
//            if (m_startNodeItem)
//                m_startNodeItem->removeConnectionItem(this);
//            if (m_endNodeItem)
//                m_endNodeItem->removeConnectionItem(this);
//            deleteLater();
//        }
//    }
//    //deleteLater();
//    //removeConnection();
//}


///
/// Public Functions
///


//!
//! Returns the bounding rectangle of the graphics item.
//!
//! \return The bounding rectangle of the graphics item.
//!
QRectF ConnectionGraphicsItem::boundingRect () const
{
    static const float extra = 5;
    return m_mainPath.controlPointRect().adjusted(-extra, -extra, extra, extra);
}


//!
//! Returns the shape of the item as QPainterPath.
//!
//! \param The shape of the item as QPainterPath.
//!
QPainterPath ConnectionGraphicsItem::shape () const
{
    QPainterPath result;
    result.addPath(m_mainPath);
    result.addPolygon(m_arrowHeadPolygon);
    return result;
}


//!
//! Enables or disables the graphics item.
//!
//! \param enabled The new enabled state for the item.
//!
void ConnectionGraphicsItem::setEnabled ( bool enabled )
{
    QGraphicsLineItem::setEnabled(enabled);

    setAcceptHoverEvents(enabled);
    if (enabled)
        setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton | Qt::XButton1 | Qt::XButton2);
    else
        setAcceptedMouseButtons(0);
}


//!
//! Paints the graphics item into a graphics view.
//!
//! \param painter The object to use for painting.
//! \param option Style options for painting the graphics item.
//! \param widget The widget into which to paint the graphics item.
//!
void ConnectionGraphicsItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
    const QColor shadowColor = Qt::darkGray;

    // draw the paths
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(Qt::NoBrush));
    painter->setPen(QPen(shadowColor, 2));
    painter->drawPath(m_shadowPath);
    if (m_hovered)
        painter->setPen(QPen(Qt::blue, 2));
    else
        painter->setPen(QPen(m_color, 2));
    painter->drawPath(m_mainPath);

    // check if the item is currently selected
    if (isSelected()) {
        // draw an outline around the curve path
        QPainterPathStroker pathStroker;
        pathStroker.setWidth(5);
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(pathStroker.createStroke(m_mainPath));
    }

    // paint the arrow head on top of the path
    painter->setBrush(QBrush(shadowColor));
    painter->setPen(QPen(shadowColor, 2));
    m_arrowHeadPolygon.translate(1, 1);
    painter->drawPolygon(m_arrowHeadPolygon);
    m_arrowHeadPolygon.translate(-1, -1);
    painter->setBrush(QBrush(QColor(m_color)));
    painter->setPen(QPen(m_color, 2));
    painter->drawPolygon(m_arrowHeadPolygon);
}


//!
//! Updates the path end points according to the positions of start and end
//! nodes.
//!
void ConnectionGraphicsItem::updatePath ()
{
    prepareGeometryChange();

    // calculate positions of the end points
    QPointF startPoint = m_startPoint;
    QPointF endPoint = m_endPoint;
    if (m_startNodeItem)
        startPoint += m_startNodeItem->pos();
    if (m_endNodeItem)
        endPoint += m_endNodeItem->pos();


    // calculate the rectangles to help calculating the positions of the node's anchor points
    const float offset = 10.0f;
    QRectF baseAnchorRect = QRectF(-offset, -offset, 2.0f * offset, 2.0f * offset);
    QRectF startAnchorRect = baseAnchorRect.translated(startPoint);
    QRectF endAnchorRect = baseAnchorRect.translated(endPoint);
    if (m_startNodeItem)
        startAnchorRect = m_startNodeItem->rect().adjusted(-offset, -offset, offset, offset).translated(m_startNodeItem->pos());
    if (m_endNodeItem)
        endAnchorRect = m_endNodeItem->rect().adjusted(-offset, -offset, offset, offset).translated(m_endNodeItem->pos());

    //
    // Diagram of anchor points for start and end nodes:
    //
    //    x        x      sU2, sU1     eU1, eU2      x        x
    //      ,----,                                     ,----,
    //      |    |                                     |    |
    //      |    |                                     |    |
    //      |   x| x      sP, sO         eO, eP      x |x   |
    //      '----'                                     '----'
    //    x        x      sL2, sL1     eL1, eL2      x        x
    //

    const QPointF &sP = startPoint;
    const QPointF &sO = QPointF(startAnchorRect.right(), startPoint.y());
    const QPointF &sU1 = startAnchorRect.topRight();
    const QPointF &sU2 = startAnchorRect.topLeft();
    const QPointF &sL1 = startAnchorRect.bottomRight();
    const QPointF &sL2 = startAnchorRect.bottomLeft();

    const QPointF &eP = endPoint;
    const QPointF &eO = QPointF(endAnchorRect.left(), endPoint.y());
    const QPointF &eU1 = endAnchorRect.topLeft();
    const QPointF &eU2 = endAnchorRect.topRight();
    const QPointF &eL1 = endAnchorRect.bottomLeft();
    const QPointF &eL2 = endAnchorRect.bottomRight();

    // declare path segments
    QList<QPointF> startPoints;
    QPainterPath cubicPath;
    QList<QPointF> endPoints;

    // construct the path segments
    if (eO.x() < sO.x() && eU2.x() > sL2.x() && eU2.y() < sL2.y() && eL2.y() > sU2.y()) {
        //> case 1V: elements very close to each other
        startPoints << sP << sO;

        const QPointF offsetVector = QPointF(0, 0.75f * (eO.y() - sO.y()));
        cubicPath.moveTo(sO);
        cubicPath.cubicTo(sO + offsetVector, eO - offsetVector, eO);

        endPoints << eO << eP;
    } else if (eO.x() >= sO.x()) {
        //> case 1H: end node is right of start node
        startPoints << sP << sO;

        const QPointF offsetVector = QPointF(0.75f * (eO.x() - sO.x()), 0);
        cubicPath.moveTo(sO);
        cubicPath.cubicTo(sO + offsetVector, eO - offsetVector, eO);

        endPoints << eO << eP;
    } else if (eU1.y() >= sL1.y()) {
        //> case 2LV
        startPoints << sP << sO << sL1;

        const QPointF offsetVector = QPointF(0, 0.75f * (eU1.y() - sL1.y()));
        cubicPath.moveTo(sL1);
        cubicPath.cubicTo(sL1 + offsetVector, eU1 - offsetVector, eU1);

        endPoints << eU1 << eO << eP;
    } else if (eL1.y() <= sU1.y()) {
        //> case 2UV
        startPoints << sP << sO << sU1;

        const QPointF offsetVector = QPointF(0, 0.75f * (eL1.y() - sU1.y()));
        cubicPath.moveTo(sU1);
        cubicPath.cubicTo(sU1 + offsetVector, eL1 - offsetVector, eL1);

        endPoints << eL1 << eO << eP;
    } else if (eP.y() >= sP.y()) {
        //> case 3L
        startPoints << sP << sO << sL1 << sL2;

        const QPointF offsetVector = QPointF(0.75f * (eU2.x() - sL2.x()), 0);
        cubicPath.moveTo(sL2);
        cubicPath.cubicTo(sL2 + offsetVector, eU2 - offsetVector, eU2);

        endPoints << eU2 << eU1 << eO << eP;
    } else {
        //> case 3U
        startPoints << sP << sO << sU1 << sU2;

        const QPointF offsetVector = QPointF(0.75f * (eL2.x() - sU2.x()), 0);
        cubicPath.moveTo(sU2);
        cubicPath.cubicTo(sU2 + offsetVector, eL2 - offsetVector, eL2);

        endPoints << eL2 << eL1 << eO << eP;
    }

    // build the main path from the path segments
    m_mainPath = QPainterPath();
    for (int i = 0; i < startPoints.size(); ++i)
        (i == 0) ? m_mainPath.moveTo(startPoints[0]) : m_mainPath.lineTo(startPoints[i]);
    
	m_mainPath.addPath(cubicPath);
    for (int i = 0; i < endPoints.size(); ++i)
        (i == 0) ? m_mainPath.moveTo(endPoints[0]) : m_mainPath.lineTo(endPoints[i]);

    // create the shadow path as a copy of the main path
    m_shadowPath = QPainterPath(m_mainPath);
    // move the path elements of the shadow path one pixel down and to the right
    for (int i = 1; i < m_shadowPath.elementCount(); ++i) {
        const QPainterPath::Element &element = m_shadowPath.elementAt(i);
        m_shadowPath.setElementPositionAt(i, element.x + 1, element.y + 1);
    }

    // get the center point for the arrow and the angle at that point
    static const float t = 0.5f;
    QPointF arrowPoint = cubicPath.pointAtPercent(t);
    float angle = cubicPath.angleAtPercent(t) * Pi / 180.0f;

    // calculate the polygon for the arrow head
    float pathLengthFraction = m_mainPath.length() / 10.0f;
    static const float maxArrowSize = 10.0f;
    float arrowSize = pathLengthFraction < maxArrowSize ? pathLengthFraction : maxArrowSize;
    QPointF arrowPoint1 = arrowPoint - QPointF(arrowSize * sin(angle - Pi / 2.0f), arrowSize * cos(angle - Pi / 2.0f));
    QPointF arrowPoint2 = arrowPoint - QPointF(arrowSize * sin(angle + Pi / 3.0f), arrowSize * cos(angle + Pi / 3.0f));
    QPointF arrowPoint3 = arrowPoint - QPointF(arrowSize * sin(angle + Pi - Pi / 3.0f), arrowSize * cos(angle + Pi - Pi / 3.0f));
    m_arrowHeadPolygon.clear();
    m_arrowHeadPolygon << arrowPoint1 << arrowPoint2 << arrowPoint3;

    // repaint the graphics item
    update();
}


//!
//! Returns the Connection object that the connection graphics item represents.
//!
//! \return The Connection object that the connection graphics item represents.
//!
Connection * ConnectionGraphicsItem::getConnection () const
{
    return m_connection;
}


//!
//! Sets the Connection object that the connection graphics item represents.
//!
//! \param connection The Connection object that the connection graphics item represents.
//!
void ConnectionGraphicsItem::setConnection ( Connection *connection )
{
    m_connection = connection;
}


////!
////! Gets the connection list.
////!
//QMap<QString, Connection*> ConnectionGraphicsItem::getConnections ()
//{
//    return m_connections;
//}
//
//
////!
////! Adds a new connection.
////!
//void ConnectionGraphicsItem::addConnection ( Connection *connection )
//{
//    //bool found == true;
// //   for ( int i = 0; i < m_connections.end(); ++i)
// //   {
// //       //! \todo Overload = operator for connection.
// //       Connector* sourceConnector1 = m_connections[i]->getSource();
// //       Connector* targetConnector1 = m_connections[i]->getTarget();
// //       Node* sourceNode1 = sourceConnector1->getNode();
// //       Node* targetNode1 = targetConnector1->getNode();
// //
// //       Connector* sourceConnector2 = connection->getSource();
// //       Connector* targetConnector2 = connection->getTarget();
// //       Node* sourceNode2 = sourceConnector2->getNode();
// //       Node* targetNode2 = targetConnector2->getNode();
//
// //       if ( sourceNode1 == sourceNode2 && targetNode1 == targetNode2 &&
// //            sourceConnector1 == sourceConnector2 && targetConnector1 == targetConnector2 )
// //       {
// //           found = true;
// //       }
//
// //   }
//
//    QString sourceNodeName = connection->getSource()->getNode()->getName();
//    QString targetNodeName = connection->getTarget()->getNode()->getName();
//    QString sourceConnectorName = connection->getSource()->getName();
//    QString targetConnectorName = connection->getTarget()->getName();
//    QString connectionName = sourceNodeName + "." + sourceConnectorName + "--->" + targetNodeName + "." + targetConnectorName;
//    m_connections.insert(connectionName, connection);
//}
//
//
////!
////! Removes the connection with the given name.
////!
//void ConnectionGraphicsItem::removeConnection ( const QString &connectionName )
//{
//    m_connections.remove(connectionName);
//    if (m_connections.size() == 0)
//        this->deleteLater();
//}


//!
//! Gets the startnode of the connection.
//!
NodeGraphicsItem * ConnectionGraphicsItem::getStartNodeItem ()
{
    return m_startNodeItem;
}


//!
//! Sets the startnode of the connection.
//!
void ConnectionGraphicsItem::setStartNodeItem ( NodeGraphicsItem *item )
{
    m_startNodeItem = item;
    if (m_startNodeItem) {
        m_startNodeItem->addConnectionItem(this);
        updatePath();
    }
}


//!
//! Gets the endnode of the connection.
//!
NodeGraphicsItem * ConnectionGraphicsItem::getEndNodeItem () const
{
    return m_endNodeItem;
}


//!
//! Sets the endnode of the connection.
//!
void ConnectionGraphicsItem::setEndNodeItem ( NodeGraphicsItem *item )
{
    m_endNodeItem = item;
    if (m_endNodeItem) {
        m_endNodeItem->addConnectionItem(this);
        updatePath();
    }
}


//!
//! Gets the startpoint of the connection.
//!
QPointF ConnectionGraphicsItem::getStartPoint () const
{
    return m_startPoint;
}


//!
//! Sets the start point of the connection.
//!
void ConnectionGraphicsItem::setStartPoint ( const QPointF &startPoint )
{
    m_startPoint = startPoint;
    updatePath();
}


//!
//! Gets the endpoint of the connection.
//!
QPointF ConnectionGraphicsItem::getEndPoint () const
{
    return m_endPoint;
}


//!
//! Sets the end point of the connection.
//!
void ConnectionGraphicsItem::setEndPoint ( const QPointF &endPoint )
{
    m_endPoint = endPoint;
    updatePath();
}


//!
//! Updates the position of an end point for a connection that is currently
//! being created.
//!
//! \param scenePosition The position of the mouse pointer over the graphics view in scene coordinates.
//!
void ConnectionGraphicsItem::updatePosition ( const QPointF &scenePosition )
{
    if (!m_connection || !m_temp)
        return;

    if (!m_connection->hasSource())
        setStartPoint(scenePosition);
    else //if (!m_connection->hasTarget())
        setEndPoint(scenePosition);

    update();
}


//!
//! Returns the name of the item.
//!
//! \return The Name of the item.
//!
const QString ConnectionGraphicsItem::getName () const
{
	return m_name;
}


///
/// Protected Functions
///


//!
//! Process hover enter events.
//!
void ConnectionGraphicsItem::hoverEnterEvent ( QGraphicsSceneHoverEvent *event )
{
    m_hovered = true;
    if (m_connection) {
        Parameter *sourceParameter = m_connection->getSourceParameter();
        Parameter *targetParameter = m_connection->getTargetParameter();
        if (sourceParameter && targetParameter) {
            const Node *sourceNode = sourceParameter->getNode();
            const Node *targetNode = targetParameter->getNode();
            if (sourceNode && targetNode) {
                QString sourceNodeName = sourceNode->getName();
                QString targetNodeName = targetNode->getName();
                QString sourceParameterName = sourceParameter->getName();
                QString targetParameterName = targetParameter->getName();
                setToolTip(sourceNodeName + "." + sourceParameterName + " --> " + targetNodeName + "." + targetParameterName);
            }
        }
    }
    update();
}


//!
//! Process hover leave events.
//!
void ConnectionGraphicsItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent *event )
{
    m_hovered = false;
    update();
}


//!
//!
//!
//QVariant ConnectionGraphicsItem::itemChange ( GraphicsItemChange change, const QVariant &value )
//{
//    if (change == ItemSelectedChange) {
//        emit ConnectionGraphicsItemSelected(m_name);
//    }
//    return QGraphicsRectItem::itemChange(change, value);
//}


///
/// Private Functions
///


//!
//! Sets the temp flag.
//!
void ConnectionGraphicsItem::setTemp ( bool temp )
{
    m_temp = temp;
    setEnabled(!m_temp);
}


////!
////! Is triggered by right button click.
////!
//void ConnectionGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
//{
//    // Delete contents of context menu.
//    m_contextMenu.clear();
//    //if ( m_contextMenu )
//    //    m_contextMenu->deleteLater();
//
//    m_contextMenu.setTitle("Connection");
//    m_contextMenu.addAction("Delete ...");
//    m_contextMenu.addSeparator();
//
//    QList<Connection*> connections = m_connections.values();
//    QMap<QString, Connection*>::iterator cIter;
//    for (cIter = m_connections.begin(); cIter != m_connections.end(); ++cIter)
//    {
//        /*sourceNodeName = connections[i]->getSource()->getNode()->getName();
//        targetNodeName = connections[i]->getTarget()->getNode()->getName();
//        sourceConnectorName = connections[i]->getSource()->getName();
//        targetConnectorName = connections[i]->getTarget()->getName();
//        m_contextMenu.addAction(sourceNodeName + "." + sourceConnectorName + "--->"
//                               targetNodeName + "." + targetConnectorName );*/
//        m_contextMenu.addAction(cIter.key());
//    }
//
//    // Show context menu.
//    QAction *selectedAction = m_contextMenu.exec(event->screenPos());
//}

} // end namespace Frapper
