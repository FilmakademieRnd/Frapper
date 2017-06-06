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
//! \file "NetworkGraphicsView.h"
//! \brief Implementation file for NetworkGraphicsView class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       03.07.2009 (last updated)
//!

#include "NetworkGraphicsView.h"
#include "ConnectionGraphicsItem.h"
#include "Log.h"
#include <QGraphicsItem>
#include <QApplication>
#include <QScrollBar>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the NetworkGraphicsView class.
//!
//! \param parent The parent widget the created instance will be a child of.
//!
NetworkGraphicsView::NetworkGraphicsView ( QWidget *parent /* = 0 */ ) :
    BaseGraphicsView(parent)
{
    setObjectName("NetworkGraphicsView");
}


//!
//! Destructor of the NetworkGraphicsView class.
//!
NetworkGraphicsView::~NetworkGraphicsView(void)
{
}


///
/// Public Functions
///


//!
//! Sets the scene to display in the graphics view.
//!
//! \param scene The scene to display in the graphics view.
//!
void NetworkGraphicsView::setScene ( QGraphicsScene *scene )
{
    BaseGraphicsView::setScene(scene);
    homeView();
}


///
/// Public Slots
///



//!
//! Sets the pin display mode for selected or all node items to show all
//! pins.
//!
void NetworkGraphicsView::displayAllPins ()
{
    setPinDisplayMode(NodeGraphicsItem::PDM_All);
}


//!
//! Sets the pin display mode for selected or all node items to show only
//! connected pins.
//!
void NetworkGraphicsView::displayConnectedPins ()
{
    setPinDisplayMode(NodeGraphicsItem::PDM_Connected);
}


//!
//! Sets the pin display mode for selected or all node items to show all
//! pins as one collapsed pin.
//!
void NetworkGraphicsView::displayPinsCollapsed ()
{
    setPinDisplayMode(NodeGraphicsItem::PDM_Collapsed);
}


///
/// Protected Events
///


//!
//! Event handler for key press events.
//!
//! \param event The description of the key event.
//!
void NetworkGraphicsView::keyPressEvent ( QKeyEvent *event )
{
    if (event->isAutoRepeat() && event->key() != Qt::Key_Plus && event->key() != Qt::Key_Minus) {
        event->ignore();
        return;
    }

    switch (event->key()) {
        case Qt::Key_Escape:
            cancelCreatingConnection();
            break;
		case Qt::Key_Delete:
		emit deleteSelectedNode();
			break;
        default:
            BaseGraphicsView::keyPressEvent(event);
    }
}


//!
//! Event handler for mouse press events.
//!
//! \param event The description of the mouse event.
//!
void NetworkGraphicsView::mousePressEvent ( QMouseEvent *event )
{
    m_lastPosition = event->pos();
    if (m_viewMode) {
        if (event->button() == Qt::LeftButton)
            QGraphicsView::mousePressEvent(event);
        else if (event->button() == Qt::MidButton) {
            // create a new mouse event that simulates a click of the left button instead of the middle button
            QMouseEvent mouseEvent (event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
            QGraphicsView::mousePressEvent(&mouseEvent);
        } else if (event->button() == Qt::RightButton)
            setCursor(Qt::SizeFDiagCursor);
    } else {
        // check if the middle mouse button has been pressed
        if (event->button() == Qt::MidButton) {
            setDragMode(QGraphicsView::ScrollHandDrag);

            // create a new mouse event that simulates a click of the left button instead of the middle button
            QMouseEvent mouseEvent (event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
            QGraphicsView::mousePressEvent(&mouseEvent);
        } else
            // check if a connection is currently being created
            if (ConnectionGraphicsItem::isConnectionCreated()) {
                // check if the user clicked into empty space
                QList<QGraphicsItem *> clickedItems = items(event->pos());
                if (clickedItems.size() >= 1 && clickedItems[0] == ConnectionGraphicsItem::getTempItem())
                    cancelCreatingConnection();
                else
                    QGraphicsView::mousePressEvent(event);
            } else {
                if (event->button() == Qt::RightButton)
                    // notify connected objects of the scene coordinates at which the context menu has been requested
                    emit contextMenuRequested(mapToScene(event->pos()));

                QGraphicsView::mousePressEvent(event);
            }
    }
}

//!
//! Event handler for mouse move events.
//!
//! \param event The description of the mouse event.
//!
void NetworkGraphicsView::mouseMoveEvent ( QMouseEvent *event )
{
    if (event->buttons() & Qt::LeftButton) {
        int x = 0;
    }
    if (qApp->focusWidget() != this) {
        m_viewMode = false;
        setCursor(Qt::ArrowCursor);
        setFocus(Qt::MouseFocusReason);
    }

    if (m_viewMode) {
        if (event->buttons() & Qt::RightButton) {
            // calculate the difference in X coordinates
            double dx = double(event->x() - m_lastPosition.x());
            if (dx != 0)
                if (dx > 0)
                    zoomIn();
                else
                    zoomOut();
        }
    } else {
        m_lastPosition = event->pos();
    }

    // iterate over the list of all graphics items contained in the scene
    QList<QGraphicsItem *> graphicsItems = items();
    foreach (QGraphicsItem *item, graphicsItems) {
        // check if the current item is a (temporary) connection graphics item
        ConnectionGraphicsItem *connectionGraphicsItem = dynamic_cast<ConnectionGraphicsItem *>(item);
        if (connectionGraphicsItem && connectionGraphicsItem == ConnectionGraphicsItem::getTempItem())
            connectionGraphicsItem->updatePosition(mapToScene(event->pos()));
    }

    QGraphicsView::mouseMoveEvent(event);
}


//!
//! Event handler for context menu events.
//!
//! \param event The object that contains details about the event.
//!
void NetworkGraphicsView::contextMenuEvent ( QContextMenuEvent *event )
{
    if (m_viewMode)
        // accept the context menu event so that it is not passed on to parent widgets
        event->accept();
    else
        BaseGraphicsView::contextMenuEvent(event);
}


///
/// Private Functions
///


//!
//! Changes the scale of the graphics scene so that it appears bigger in
//! the graphics view.
//!


//!
//! Sets the pin display mode for selected or all node items to the given
//! mode.
//!
//! \param pinDisplayMode The pin display mode to set for the node items.
//!
void NetworkGraphicsView::setPinDisplayMode ( NodeGraphicsItem::PinDisplayMode pinDisplayMode )
{
    QList<QGraphicsItem *> itemsToProcess;

    // check if there is a selection of node graphics items
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    if (selectedItems.size() > 0)
        itemsToProcess = selectedItems;
    else
        itemsToProcess = scene()->items();

    // set up a list for caching the node graphics items
    // (needed because pin graphics items that are contained in the list could
    // be deleted when setting the pin display mode for node graphics items)
    QList<NodeGraphicsItem *> nodeGraphicsItemsToProcess;
    // iterate over all graphics items to process
    foreach (QGraphicsItem *item, itemsToProcess) {
        // check if the current item is a node graphics item
        NodeGraphicsItem *nodeGraphicsItem = dynamic_cast<NodeGraphicsItem *>(item);
        if (nodeGraphicsItem)
            nodeGraphicsItemsToProcess.append(nodeGraphicsItem);
    }

    // iterate over the list of node graphics items to process
    foreach (NodeGraphicsItem *nodeGraphicsItem, nodeGraphicsItemsToProcess)
        nodeGraphicsItem->setPinDisplayMode(pinDisplayMode);
}


//!
//! Cancels the creation of a new connection between pins of nodes.
//!
void NetworkGraphicsView::cancelCreatingConnection ()
{
    ConnectionGraphicsItem *tempItem = ConnectionGraphicsItem::getTempItem();
    if (tempItem) {
        ConnectionGraphicsItem::setTempItem(0);
        Connection *connection = tempItem->getConnection();
        
        // delete the temporary connection
        delete tempItem;
        tempItem = 0;
        if (connection) {
            Parameter *sourceParameter = connection->getSourceParameter();
            Parameter *targetParameter = connection->getTargetParameter();
            delete connection;
            if (sourceParameter && sourceParameter->getName() == "connectAllDummy")
                delete sourceParameter;
            if (targetParameter && targetParameter->getName() == "connectAllDummy")
                delete targetParameter;
        }

        // reactivate all flags and pins
        foreach (QGraphicsItem *item, scene()->items()) {
            NodeGraphicsItem *nodeItem = dynamic_cast<NodeGraphicsItem *>(item);
            if (nodeItem)
                nodeItem->setEnabled(true);
            else {
                ConnectionGraphicsItem *connectionItem = dynamic_cast<ConnectionGraphicsItem *>(item);
                if (connectionItem) {
                    connectionItem->setEnabled(true);
                }
            }
        }
    }
}

} // end namespace Frapper