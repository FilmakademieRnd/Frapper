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
//! \file "NodeGroupGraphicsItem.cpp"
//! \brief Implementation file for NodeGroupGraphicsItem class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       09.07.2013 (last updated)
//!

#include "NodeGroupGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "ViewNode.h"
#include "FlagGraphicsItem.h"
#include "Log.h"
#include <QGraphicsScene>
#include <QtSvg/QSvgGenerator>

namespace Frapper {


///
/// Constructors and Destructors
///


//!
//! Constructor of the NodeGroupGraphicsItem class.
//!
//! \param node The node that the graphics item represents.
//! \param position The initial position for the graphics item.
//! \param color The color in which to draw the graphics item.
//!
NodeGroupGraphicsItem::NodeGroupGraphicsItem (const QString &name, const QList<Node *> &nodeList, const QList<QPointF> &posList, const QColor &color ) :
NodeGraphicsItem(0, QPointF(0.f, 0.f), color)
{
	if (nodeList.empty())
        return;
	
	m_name = name + QString::number(s_totalNumberOfInstances);

	QString toolTip = "Node Group: ";
	bool isEvaluated, isViewed;
	isEvaluated = isViewed = false;
	QPointF newPos = QPointF(0.f, 0.f);

	foreach (const QPointF &pos, posList)
		newPos += pos;
	newPos /= nodeList.size();
	setPos(newPos);

	for(int i=0; i<nodeList.size(); i++) {
		Node *node = nodeList.at(i);
		const QPointF &pos = posList.at(i);

		m_nodes.append(node);
		m_nodePositions[node->getName()] = pos - newPos;

		if (i == nodeList.size()-1)
			toolTip.append(node->getName());
		else
			toolTip.append(node->getName() + ",");
		
		// connect signals from node to slots of this graphics item
		connect(node, SIGNAL(selectedChanged(bool)), SLOT(setSelected(bool)));
		connect(node, SIGNAL(nodeChanged()), SLOT(refresh()));

		// create evaluate flag
		isEvaluated = isEvaluated && node->isEvaluated();
		if (!m_evalFlagItem)
			m_evalFlagItem = new FlagGraphicsItem(this, "Eval", false, QColor(0, 255, 128), QPointF(4, 22));
		connect(m_evalFlagItem, SIGNAL(toggled(bool)), node, SLOT(setEvaluate(bool)));

		// create view flag
		ViewNode *viewNode = dynamic_cast<ViewNode *>(node);
		if (viewNode) {
			isViewed = isViewed || viewNode->isViewed();
			if (!m_viewFlagItem)
				m_viewFlagItem = new ViewFlagGraphicsItem(this, false, QPointF(rect().width() - 4 - 23, 22), viewNode->getStageIndex());
			connect(m_viewFlagItem, SIGNAL(toggled(bool)), viewNode, SLOT(setView(bool)));
			connect(m_viewFlagItem, SIGNAL(stageIndexSet(unsigned int)), viewNode, SLOT(setStageIndex(unsigned int)));
			connect(viewNode, SIGNAL(viewSet(unsigned int, ViewNode *)), m_viewFlagItem, SLOT(setStageIndex(unsigned int, ViewNode *)));

			m_viewFlagItem->setChecked(isViewed);
		}
	}

	m_evalFlagItem->setChecked(isEvaluated);


	setObjectName("NodeGroup " + QString(s_numberOfInstances));
	setToolTip(toolTip);

	// set connection for context menu
	//connect(&m_contextMenu, SIGNAL(triggered(QAction *)), SLOT(contextClicked(QAction *)));

	// build list of pin sub items to display in the item
	updatePins();
}


//!
//! Destructor of the NodeGroupGraphicsItem class.
//!
NodeGroupGraphicsItem::~NodeGroupGraphicsItem ()
{
}


///
/// Public Functions
///



//!
//! Paints the graphics item into a graphics view.
//!
//! \param painter The object to use for painting.
//! \param option Style options for painting the graphics item.
//! \param widget The widget into which to paint the graphics item.
//!
void NodeGroupGraphicsItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
    static const int CornerRadius = 10;

    // define background color
    QColor backgroundColor;
    if (widget)
        backgroundColor = widget->palette().background().color().dark(110);
    else
        backgroundColor = Qt::gray;
    if (m_hovered) // isUnderMouse() didn't always work here
        backgroundColor = backgroundColor.lighter(107);

    // draw selection border
    if (isSelected()) {
        painter->setPen(QPen(m_color, 8));
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRoundedRect(rect(), CornerRadius, CornerRadius);
    }

    // set up the node's background gradient
    QLinearGradient linearGradient (rect().left(), 0, rect().right(), 0);
    linearGradient.setColorAt(0.7, backgroundColor);
	if (!m_nodes.empty() && isEvaluated())
        linearGradient.setColorAt(1, m_color);
    else
        linearGradient.setColorAt(1, backgroundColor.darker(125));

    // draw the node's shape
    QBrush gradientBrush (linearGradient);
    if (isEnabled() && !m_nodes.empty() && isEvaluated())
        painter->setPen(QPen(Qt::black, 1));
    else
        painter->setPen(QPen(Qt::gray, 1));
    painter->setBrush(gradientBrush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(rect(), CornerRadius, CornerRadius);

    // draw the node's name
    if (isEnabled())
        painter->setPen(QPen(Qt::black, 1));
    else
        painter->setPen(QPen(Qt::gray, 1));
    painter->setClipping(true);
    painter->setClipRect(rect().adjusted(2, 2, -2, -2));
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->drawText(QPointF(rect().left() + 4, rect().top() + 16), m_name);
    //painter->drawText(rect(), Qt::AlignCenter, QString("%1").arg(zValue()));
    painter->setClipping(false);
}


//!
//! Returns the selected node.
//!
//! \param i The position of the node in list.
//! 
//! \return The node that the graphics item represents.
//!
Node * NodeGroupGraphicsItem::getNode (const int i) const
{
    return m_nodes[i];
}


//!
//! Returns the name og the group.
//!
const QString NodeGroupGraphicsItem::getName () const
{
	return m_name;
}


//!
//! Returns the nodes that the graphics item represents.
//!
//! \return The nodes that the graphics item represents.
//!
const QList<Node *> & NodeGroupGraphicsItem::getNodes () const
{
    return m_nodes;
}


//!
//! Returns the node positions of the containing nodes.
//!
//! \return The node positions of the containing nodes.
//!
const QPointF NodeGroupGraphicsItem::getNodeItemPosition (const QString &nodeName) const
{
	QHash<QString, QPointF>::ConstIterator iter = m_nodePositions.constFind(nodeName);
	if (iter != m_nodePositions.end())
		return iter.value();
	else
		return QPointF(.0f, .0f);
}

//!
//! Returns the node positions of the containing nodes.
//!
//! \return The node positions of the containing nodes.
//!
const QHash<QString, QPointF> NodeGroupGraphicsItem::getNodeItemPositions () const
{
		return m_nodePositions;
}


//!
//! Returns whether the group item contains the node with
//! the given name or not.
//!
//! \return True the group item contains the node with
//! the given name, flase otherwise.
//!
const bool NodeGroupGraphicsItem::containsNode(const QString &nodeName) const
{
	foreach (const Node *node, m_nodes) {
		if (node->getName() == nodeName)
			return true;
	}
	return false;
}


//!
//! Resets the node to 0.
//!
//! Should be called when the node that the graphics item represents will
//! be destroyed.
//!
//! Temporary. Should be replaced by improved code when destroying nodes
//! in the scene model.
//!
void NodeGroupGraphicsItem::resetNode ()
{
	m_nodes.clear();
}


///
/// Public Slots
///


//!
//! Refresh the graphics item to reflect changes in the node that it
//! represents.
//!
void NodeGroupGraphicsItem::refresh ()
{
	bool isEvaluated, isViewed;
	isEvaluated = isViewed = false;

	// update flag graphics items
	m_viewFlagItem->setChecked(this->isViewed());
	m_evalFlagItem->setChecked(this->isEvaluated());

    updatePins();
}


///
/// Protected Functions
///


//!
//! Event handler that reacts to context menu events.
//!
//! \param event The object containing details about the event.
//!
void NodeGroupGraphicsItem::contextMenuEvent ( QGraphicsSceneContextMenuEvent *event )
{
    m_hovered = true;
    update();

    // create evaluate flag action
    QAction *evalFlagAction = new QAction(tr("&Evaluate"), this);
    evalFlagAction->setShortcut(Qt::Key_E);
    evalFlagAction->setCheckable(true);
    evalFlagAction->setChecked(isEvaluated());

	foreach (Node *node, m_nodes)
		connect(evalFlagAction, SIGNAL(toggled(bool)), node, SLOT(setEvaluate(bool)));

    // prepare view flag action
    QAction *viewFlagAction = 0;
    if (m_viewFlagItem) {
        // create view flag action
        viewFlagAction = new QAction(tr("&View"), this);
        viewFlagAction->setShortcut(Qt::Key_V);
        viewFlagAction->setCheckable(true);

		foreach (Node *node, m_nodes) {
			ViewNode *viewNode = dynamic_cast<ViewNode *>(node);
			connect(viewFlagAction, SIGNAL(toggled(bool)), viewNode, SLOT(setView(bool)));
		}

		viewFlagAction->setChecked(isViewed());
    }

    // create pin/connector viewing mode actions
    QActionGroup *actionGroup = new QActionGroup(this);
    QAction *displayAllPinsAction = new QAction(tr("&All"), actionGroup);
    displayAllPinsAction->setCheckable(true);
    displayAllPinsAction->setData(PDM_All);
    displayAllPinsAction->setStatusTip(tr("Display all pins"));
    QAction *displayConnectedPinsAction = new QAction(tr("&Connected"), actionGroup);
    displayConnectedPinsAction->setCheckable(true);
    displayConnectedPinsAction->setData(PDM_Connected);
    displayConnectedPinsAction->setStatusTip(tr("Display connected pins only"));
    QAction *displayPinsCollapsedAction = new QAction(tr("Colla&psed"), actionGroup);
    displayPinsCollapsedAction->setCheckable(true);
    displayPinsCollapsedAction->setData(PDM_Collapsed);
    displayPinsCollapsedAction->setStatusTip(tr("Display pins collapsed"));
    switch (m_pinDisplayMode) {
        case PDM_All:
            displayAllPinsAction->setChecked(true);
            break;
        case PDM_Connected:
            displayConnectedPinsAction->setChecked(true);
            break;
        case PDM_Collapsed:
            displayPinsCollapsedAction->setChecked(true);
            break;
    }
    connect(actionGroup, SIGNAL(triggered(QAction *)), SLOT(updatePinDisplayMode(QAction *)));

    // build list of flag actions
    QList<QAction *> flagActions;
    flagActions << evalFlagAction;
    if (viewFlagAction)
        flagActions << viewFlagAction;

    // connect actions
    QAction *connectByNameAction = new QAction(tr("Connect by Name"), this);
    connect(connectByNameAction, SIGNAL(triggered()), SLOT(connectByNameClicked()));
    QList<QAction *> connectionActions;
    connectionActions << connectByNameAction;

    // build context menu
    QMenu menu;
    menu.addActions(flagActions);
    menu.addSeparator()->setText(tr("Display Pins"));
    menu.addActions(QList<QAction *>()
        << displayAllPinsAction
        << displayConnectedPinsAction
        << displayPinsCollapsedAction
        );
    menu.addSeparator()->setText(tr("Create Connections"));
    menu.addActions(connectionActions);

    // show the menu
    QAction *selectedAction = menu.exec(event->screenPos());

    // process menu results
    if (selectedAction == evalFlagAction)
        m_evalFlagItem->setChecked(isEvaluated());
    else if (m_viewFlagItem && viewFlagAction && selectedAction == viewFlagAction)
        m_viewFlagItem->setChecked(isViewed());

    m_hovered = false;
    update();
}


///
/// Private Functions
///


//!
//! Updates the item according to the pin display mode set to display all
//! pins, only connected pins, or all pins in one collapsed pin.
//!
void NodeGroupGraphicsItem::updatePins ()
{
    // destroy all existing pin graphics items
	foreach (PinGraphicsItem *pinItem, m_inputPins)
        delete pinItem;

	foreach (PinGraphicsItem *pinItem, m_outputPins)
        delete pinItem;

    m_inputPins.clear();
    m_outputPins.clear();
	Node *node = 0;

	for (int n=0; n<m_nodes.size(); ++n) {
		node = m_nodes[n];
		// make sure the node still exists
		if (!node) {
			Log::warning("The node that the node graphics item represents has been destroyed.", "NodeGroupGraphicsItem::updatePins");
			continue;
		}

		ParameterGroup *parameterRoot = node->getParameterRoot();

		switch (m_pinDisplayMode) {
			case PDM_All:
				{
					// retrieve lists of all input and output parameters from the node
					AbstractParameter::List inputParameters;
					AbstractParameter::List outputParameters;
					node->fillParameterLists(&inputParameters, &outputParameters);

					// iterate over the list of input parameters
					for (int i = 0; i < inputParameters.size(); ++i)
						if (!isInternal(inputParameters[i])) {
							if (inputParameters[i]->isGroup())
								m_inputPins.append(new PinGraphicsItem(this, m_inputPins.size(), static_cast<ParameterGroup *>(inputParameters[i]), Parameter::PT_Input));
							else
								m_inputPins.append(new PinGraphicsItem(this, m_inputPins.size(), static_cast<Parameter *>(inputParameters[i])));
						}
					// iterate over the list of output parameters
					for (int i = 0; i < outputParameters.size(); ++i)
						if (!isInternal(outputParameters[i])) {
							if (outputParameters[i]->isGroup())
								m_outputPins.append(new PinGraphicsItem(this, m_outputPins.size(), static_cast<ParameterGroup *>(outputParameters[i]), Parameter::PT_Output));
							else
								m_outputPins.append(new PinGraphicsItem(this, m_outputPins.size(), static_cast<Parameter *>(outputParameters[i])));
						}
				}
				break;
			case PDM_Connected:
				{
					// retrieve lists of connected input and output parameters from the node
					// and check if there are unconnected input or output parameters available
					AbstractParameter::List connectedInputParameters;
					AbstractParameter::List connectedOutputParameters;
					bool unconnectedInputParametersAvailable = false;
					bool unconnectedOutputParametersAvailable = false;
					node->fillParameterLists(&connectedInputParameters, &connectedOutputParameters, &unconnectedInputParametersAvailable, &unconnectedOutputParametersAvailable);

					// iterate over the list of connected input parameters
					for (int i = 0; i < connectedInputParameters.size(); ++i)
						if (!isInternal(connectedInputParameters[i])) {
							if (connectedInputParameters[i]->isGroup())
								m_inputPins.append(new PinGraphicsItem(this, m_inputPins.size(), static_cast<ParameterGroup *>(connectedInputParameters[i]), Parameter::PT_Input, true));
							else
								m_inputPins.append(new PinGraphicsItem(this, m_inputPins.size(), static_cast<Parameter *>(connectedInputParameters[i])));
						}
					// iterate over the list of connected output parameters
					for (int i = 0; i < connectedOutputParameters.size(); ++i)
						if (!isInternal(connectedOutputParameters[i])) {
							if (connectedOutputParameters[i]->isGroup())
								m_outputPins.append(new PinGraphicsItem(this, m_outputPins.size(), static_cast<ParameterGroup *>(connectedOutputParameters[i]), Parameter::PT_Output, true));
							else
								m_outputPins.append(new PinGraphicsItem(this, m_outputPins.size(), static_cast<Parameter *>(connectedOutputParameters[i])));
						}
					// add another input pin if unconnected input parameters are available
					if (unconnectedInputParametersAvailable)
						m_inputPins.append(new PinGraphicsItem(this, connectedInputParameters.size(), parameterRoot, Parameter::PT_Input, false));

					// add another output pin if unconnected output parameters are available
					if (unconnectedOutputParametersAvailable)
						m_outputPins.append(new PinGraphicsItem(this, connectedOutputParameters.size(), parameterRoot, Parameter::PT_Output, false));
				}
				break;
			case PDM_Collapsed:
				if (parameterRoot->contains(Parameter::PT_Input))
					m_inputPins.append(new PinGraphicsItem(this, m_inputPins.size(), parameterRoot, Parameter::PT_Input));
				if (parameterRoot->contains(Parameter::PT_Output))
					m_outputPins.append(new PinGraphicsItem(this, m_outputPins.size(), parameterRoot, Parameter::PT_Output));
				break;
		}
	}

    // set up signal/slot connections for all input pins
    for (int i = 0; i < m_inputPins.size(); ++i)
        connect(m_inputPins[i], SIGNAL(parameterClicked(PinGraphicsItem *, Parameter *)), SLOT(createConnection(PinGraphicsItem *, Parameter *)));

    // set up signal/slot connections for all output pins
    for (int i = 0; i < m_outputPins.size(); ++i)
        connect(m_outputPins[i], SIGNAL(parameterClicked(PinGraphicsItem *, Parameter *)), SLOT(createConnection(PinGraphicsItem *, Parameter *)));

    // find the maximum number of visible pins
    int numberOfPins;
    if (m_inputPins.size() > m_outputPins.size())
        numberOfPins = m_inputPins.size();
    else
        numberOfPins = m_outputPins.size();

    // update connected connection graphics items
    foreach (ConnectionGraphicsItem *item, m_connectionItems) {
        Connection *connection = item->getConnection();
        bool representingPinFound = false;
        int i = 0;
        while (!representingPinFound && i < m_inputPins.size())
            if (m_inputPins[i]->represents(connection->getTargetParameter())) {
                item->setEndPoint(m_inputPins[i]->center());
                representingPinFound = true;
            } else
                ++i;
            if (!representingPinFound) {
                i = 0;
                while (!representingPinFound && i < m_outputPins.size())
                    if (m_outputPins[i]->represents(connection->getSourceParameter())) {
                        item->setStartPoint(m_outputPins[i]->center());
                        representingPinFound = true;
                    } else
                        ++i;
            }
    }

    // update the item's height according to the number of visible pins
    QRectF itemRect = rect();
    itemRect.setHeight(ItemBaseHeight + numberOfPins * ItemPinRowHeight);
    setRect(itemRect);
}


//!
//! Disables all pins in the node item that are of a different parameter type
//! or the same pin types as the parameter that was clicked.
//! Also disables all of the node item's flags.
//! The whole node will be disabled if there are no enabled pins left.
//!
//! \param pinItem The pin graphics item representing the parameter that was clicked.
//! \param parameter The parameter that was clicked.
//!
void NodeGroupGraphicsItem::disableOtherPins ( PinGraphicsItem *pinItem, Parameter *parameter )
{
    Parameter::Type parameterType = parameter->getType();
    Parameter::PinType pinType = parameter->getPinType();
    bool enabledPins = false;
    foreach (QGraphicsItem *item, childItems()) {
        if (item == pinItem)
            enabledPins = true;
        else {
            // deactivate those pins that are of a different parameter type or the same connector type
            PinGraphicsItem *pinItem = dynamic_cast<PinGraphicsItem *>(item);
            if (pinItem) {
                if (pinItem->setEnabled(parameterType, pinType == Parameter::PT_Input ? Parameter::PT_Output : Parameter::PT_Input))
                    enabledPins = true;
            } else {
                // deactivate all flags
                FlagGraphicsItem *flagItem = dynamic_cast<FlagGraphicsItem *>(item);
                if (flagItem)
                    flagItem->setEnabled(false);
            }
        }
    }
    if (!enabledPins)
        setEnabled(false);
}


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
void NodeGroupGraphicsItem::beginCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter )
{
    if (pinItem && parameter) {
        Parameter::PinType pinType = parameter->getPinType();

        // deactivate all pins that do not correspond to the data type of the clicked pin
        foreach (QGraphicsItem *item, scene()->items()) {
            NodeGroupGraphicsItem *nodeItem = dynamic_cast<NodeGroupGraphicsItem *>(item);
            if (nodeItem) {
                nodeItem->disableOtherPins(pinItem, parameter);
            } else {
                ConnectionGraphicsItem *connectionItem = dynamic_cast<ConnectionGraphicsItem *>(item);
                if (connectionItem)
                    connectionItem->setEnabled(false);
            } 
        }

        // create a new connection for the connection graphics item and register it with the clicked parameter
        Connection *connection;
        if (pinType == Parameter::PT_Output)
            connection = new Connection(parameter, 0);
        else
            connection = new Connection(0, parameter);

        // create the temporary connection graphics item and add it to the scene
        ConnectionGraphicsItem *connectionItem = new ConnectionGraphicsItem(QString::number(connection->getId()), Parameter::getTypeColor(parameter->getType()), pinItem->center(), pinItem->center());
        connectionItem->setConnection(connection);
        ConnectionGraphicsItem::setTempItem(connectionItem);
        scene()->addItem(connectionItem);
        // set end or start node item depending on the type of the clicked parameter
        if (pinType == Parameter::PT_Output) {
            connectionItem->setStartNodeItem(this);
            connectionItem->setStartPoint(pinItem->center());
            connectionItem->setEndPoint(pos() + pinItem->center());
        } else {
            connectionItem->setEndNodeItem(this);
            connectionItem->setEndPoint(pinItem->center());
            connectionItem->setStartPoint(pos() + pinItem->center());
        }
    } else {
        // make a connection without using pin (used for context menu make connection by name)

        // create the temporary connection graphics item and add it to the scene
        ConnectionGraphicsItem *connectionItem = new ConnectionGraphicsItem("tempConnection", QColor(0.3, 0.3, 0.3), rect().center(), rect().center());
        ConnectionGraphicsItem::setTempItem(connectionItem);
        scene()->addItem(connectionItem);

		// just take the first one
		if (!m_nodes.empty() && m_nodes[0]) {
            Parameter *parameter = new Parameter("connectAllDummy", Parameter::T_Bool, QVariant(false));
            parameter->setNode(m_nodes[0]);
            if (parameter) {
                Connection *connection;
                connection = new Connection(parameter, 0);
                connectionItem->setConnection(connection);
                connectionItem->setObjectName("connectAllDummy");

                // set end or start node item depending on the type of the clicked parameter
                connectionItem->setStartNodeItem(this);
                connectionItem->setStartPoint(rect().center());
            }
        }
    }
}


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
void NodeGroupGraphicsItem::endCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter )
{
    if (pinItem && parameter) {
        // obtain the connection from the temporary connection graphics item
        ConnectionGraphicsItem *connectionItem = ConnectionGraphicsItem::getTempItem();
        Connection *connection = connectionItem->getConnection();
        ConnectionGraphicsItem::setTempItem(0);

        if (!connection || !connectionItem) {
            Log::error("No connection item or connection available.", "NodeGroupGraphicsItem::endCreatingConnection");
            return;
        }

        // check if the same pin has been clicked
        if ((connection->hasSource() && connection->getSourceParameter() == parameter) || (connection->hasTarget() && connection->getTargetParameter() == parameter)) {
            // cancel creating the connection
            if (connection)
                delete connection;
            if (connectionItem) {
                delete connectionItem;
                connectionItem = 0;
            }
        } else {
            Parameter *sourceParameter = 0;
            Parameter *targetParameter = 0;
            if (parameter->getPinType() == Parameter::PT_Input && connection->hasSource()) {
                sourceParameter = connection->getSourceParameter();
                targetParameter = parameter;
            } else if (parameter->getPinType() == Parameter::PT_Output && connection->hasTarget()) {
                sourceParameter = parameter;
                targetParameter = connection->getTargetParameter();
            } else
                Log::error("Wrong combination of source and target parameters.", "NodeGroupGraphicsItem::endCreatingConnection");

            if (sourceParameter && targetParameter)
                // notify connected objects that a connection should be established between the source and target parameters
                emit connectionRequested(sourceParameter, targetParameter);

            NodeGraphicsItem *startNodeItem = connectionItem->getStartNodeItem();
            if (startNodeItem)
                startNodeItem->refresh();

            NodeGraphicsItem *endNodeItem = connectionItem->getEndNodeItem();
            if (endNodeItem)
                endNodeItem->refresh();

            refresh();

            // delete the temporary connection
            if (connectionItem) {
                delete connectionItem;
                //scene()->removeItem(connectionItem);
                connectionItem = 0;
            }
            if (connection)
                delete connection;
        }
	} else {
        ConnectionGraphicsItem *tempConnectionItem = ConnectionGraphicsItem::getTempItem();
        Connection *connection = tempConnectionItem->getConnection();
        Parameter *dummyParameter = connection->getSourceParameter();
        Node *firstNode = dummyParameter->getNode();
        NodeGraphicsItem *startNodeItem = tempConnectionItem->getStartNodeItem();
        if (!connection || !tempConnectionItem || !dummyParameter) {
            Log::error("No connection item or connection available.", "NodeGroupGraphicsItem::endCreatingConnection");
            return;
        }
        //delete dummyParameter;
        NodeGraphicsItem *otherNodeGroupGraphicsItem = tempConnectionItem->getStartNodeItem();
        if (firstNode) {
            emit connectionRequested(firstNode, m_nodes[0]);
        }

        ConnectionGraphicsItem::setTempItem(0);

        if (tempConnectionItem) {
            delete tempConnectionItem;
            tempConnectionItem = 0;
        }
        if (connection) {
            Parameter *sourceParameter = connection->getSourceParameter();
            Parameter *targetParameter = connection->getTargetParameter();
            delete connection;
            if (sourceParameter)
                delete sourceParameter;
            if (targetParameter)
                delete targetParameter;

        }
    }

    // reactivate all flags and pins
    foreach (QGraphicsItem *item, scene()->items()) {
        NodeGroupGraphicsItem *nodeItem = dynamic_cast<NodeGroupGraphicsItem *>(item);
        if (nodeItem)
            nodeItem->setEnabled(true);
        else {
            ConnectionGraphicsItem *connectionItem = dynamic_cast<ConnectionGraphicsItem *>(item);
            if (connectionItem)
                connectionItem->setEnabled(true);
        }
    }
}


//!
//! Internal function to estimate the view state of the node group.
//!
//! \return The estimated view state of the node.
//!
bool NodeGroupGraphicsItem::isViewed () const
{
	bool isViewed = false;
	foreach (Node *node, m_nodes) {
		ViewNode *viewNode = dynamic_cast<ViewNode *>(node);
		if (viewNode)
			isViewed = isViewed || viewNode->isViewed();
	}
	return isViewed;
}


//!
//! Internal function to estimate the evaluation state of the node group.
//!
//! \return The estimated evaluation state of the node.
//!
bool NodeGroupGraphicsItem::isEvaluated () const
{
	bool isEvaluated = false;
	foreach (Node *node, m_nodes)
		isEvaluated = isEvaluated || node->isEvaluated();
	
	return isEvaluated;
}


//!
//! Internal function to estimate all source nodes of an node group.
//!
//! \return The estimated source nodes of an node group.
//!
QList<Node *> NodeGroupGraphicsItem::getInNodes() const
{
	QList<Node *> inNodes;
	
	foreach(Node *node, m_nodes) {
		AbstractParameter::List &inParamList = node->getParameters(Parameter::PT_Input, true);
		foreach (AbstractParameter *inParam, inParamList) {
			Parameter *parameter = dynamic_cast<Parameter *>(inParam);
			const Connection::Map &connectionMap = parameter->getConnectionMap();
			foreach (const Connection *connection, connectionMap) {
				Node *connectedNode = connection->getSourceParameter()->getNode();
				if (connectedNode && (!m_nodes.contains(connectedNode)))
					inNodes.append(node);
			}
		}
	}
	return inNodes;
}


//!
//! Internal function to estimate all sink nodes of an node group.
//!
//! \return The estimated sink nodes of an node group.
//!
QList<Node *> NodeGroupGraphicsItem::getOutNodes() const
{
	QList<Node *> outNodes;
	
	foreach(Node *node, m_nodes) {
		AbstractParameter::List &outParamList = node->getParameters(Parameter::PT_Output, true);
		foreach (AbstractParameter *outParam, outParamList) {
			Parameter *parameter = dynamic_cast<Parameter *>(outParam);
			const Connection::Map &connectionMap = parameter->getConnectionMap();
			foreach (Connection *connection, connectionMap) {
				Node *connectedNode = connection->getTargetParameter()->getNode();
				if (connectedNode && (!m_nodes.contains(connectedNode)))
					outNodes.append(node);
			}
		}
	}
	return outNodes;
}


//!
//! Internal function to determine if an parameter of the node group is internal.
//!
//! \param parameter The Parameter to check.
//! \return The estimated result.
//!
bool NodeGroupGraphicsItem::isInternal(AbstractParameter *inParameter) const
{
	bool returnvalue = false;
	Parameter *connectedParameter = 0;
	if (inParameter->isGroup()) {
		ParameterGroup *parameterGroup = static_cast<ParameterGroup *>(inParameter);
		const AbstractParameter::List& parameterList = parameterGroup->getParameterList();
		foreach (AbstractParameter *curPara, parameterList)
			returnvalue = returnvalue || isInternal(curPara);
	}
	else {
		Parameter *parameter = static_cast<Parameter *>(inParameter);
		if (parameter->isConnected()) {
			const Connection::Map &connectionMap = parameter->getConnectionMap();
			foreach (const Connection *connection, connectionMap) {
				connectedParameter = connection->getTargetParameter();
				if (connectedParameter)
					returnvalue = returnvalue || m_nodes.contains(connectedParameter->getNode());
				connectedParameter = connection->getSourceParameter();
				if (connectedParameter)
					returnvalue = returnvalue && m_nodes.contains(connectedParameter->getNode());
			}
		}
	}
	return returnvalue;
}

} // end namespace Frapper