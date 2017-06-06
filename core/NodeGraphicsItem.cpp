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
//! \file "NodeGraphicsItem.cpp"
//! \brief Implementation file for NodeGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.11.2013 (last updated)
//!

#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "ViewNode.h"
#include "FlagGraphicsItem.h"
#include "Log.h"
#include <QGraphicsScene>
#include <QtSvg/QSvgGenerator>

namespace Frapper {

INIT_INSTANCE_COUNTER(NodeGraphicsItem)

///
/// Private Static Data
///


//!
//! The number of node graphics items created for the current scene.
//!
int NodeGraphicsItem::s_numberOfNodeGraphicsItems = 0;


///
/// Constructors and Destructors
///


//!
//! Constructor of the NodeGraphicsItem class.
//!
//! \param node The node that the graphics item represents.
//! \param position The initial position for the graphics item.
//! \param color The color in which to draw the graphics item.
//!
NodeGraphicsItem::NodeGraphicsItem ( Node *node, QPointF position, const QColor &color ) :
BaseRectItem(color),
m_evalFlagItem(0),
m_viewFlagItem(0),
m_pinDisplayMode(PDM_All),
m_deleteConnection(true)
{
	INC_INSTANCE_COUNTER

    setPos(position);
    setRect(0, 0, ItemWidth, ItemBaseHeight);
    setZValue(s_numberOfNodeGraphicsItems++);

#if QT_VERSION >= 0x040600
    // Since Qt 4.6 geometry changes are disabled by default. Introduction of new flag.
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    if (node)
		m_nodes.append(node);
	else
		return;

    setObjectName(m_nodes[0]->getName());
    setToolTip(m_nodes[0]->getName());

    // connect signals from node to slots of this graphics item
    connect(m_nodes[0], SIGNAL(selectedChanged(bool)), SLOT(setSelected(bool)));
    connect(m_nodes[0], SIGNAL(nodeChanged()), SLOT(refresh()));

    // create evaluate flag
    m_evalFlagItem = new FlagGraphicsItem(this, "Eval", m_nodes[0]->isEvaluated(), QColor(0, 255, 128), QPointF(4, 22));
    connect(m_evalFlagItem, SIGNAL(toggled(bool)), m_nodes[0], SLOT(setEvaluate(bool)));

    // create view flag
    ViewNode *viewNode = dynamic_cast<ViewNode *>(m_nodes[0]);
    if (viewNode) {
        m_viewFlagItem = new ViewFlagGraphicsItem(this, viewNode->isViewed(), QPointF(rect().width() - 4 - 23, 22), viewNode->getStageIndex());
        connect(m_viewFlagItem, SIGNAL(toggled(bool)), viewNode, SLOT(setView(bool)));
        connect(m_viewFlagItem, SIGNAL(stageIndexSet(unsigned int)), viewNode, SLOT(setStageIndex(unsigned int)));
        connect(viewNode, SIGNAL(viewSet(unsigned int, ViewNode *)), m_viewFlagItem, SLOT(setStageIndex(unsigned int, ViewNode *)));
    }

    // build list of pin sub items to display in the item
    updatePins();
}

NodeGraphicsItem::NodeGraphicsItem ( Node *node, QPointF position, const int width, const int height, const QColor &color ) :
BaseRectItem(color),
	m_evalFlagItem(0),
	m_viewFlagItem(0),
	m_pinDisplayMode(PDM_All),
	m_deleteConnection(true)
{
	INC_INSTANCE_COUNTER

		setPos(position);
	setRect(0, 0, width, height);
	setZValue(s_numberOfNodeGraphicsItems++);

#if QT_VERSION >= 0x040600
	// Since Qt 4.6 geometry changes are disabled by default. Introduction of new flag.
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);

	if (node)
		m_nodes.append(node);
	else
		return;

	setObjectName(m_nodes[0]->getName());
	setToolTip(m_nodes[0]->getName());

	// connect signals from node to slots of this graphics item
	connect(m_nodes[0], SIGNAL(selectedChanged(bool)), SLOT(setSelected(bool)));
	connect(m_nodes[0], SIGNAL(nodeChanged()), SLOT(refresh()));

	// create evaluate flag
	m_evalFlagItem = new FlagGraphicsItem(this, "Eval", m_nodes[0]->isEvaluated(), QColor(0, 255, 128), QPointF(4, 22));
	connect(m_evalFlagItem, SIGNAL(toggled(bool)), m_nodes[0], SLOT(setEvaluate(bool)));

	// create view flag
	ViewNode *viewNode = dynamic_cast<ViewNode *>(m_nodes[0]);
	if (viewNode) {
		m_viewFlagItem = new ViewFlagGraphicsItem(this, viewNode->isViewed(), QPointF(rect().width() - 4 - 23, 22), viewNode->getStageIndex());
		connect(m_viewFlagItem, SIGNAL(toggled(bool)), viewNode, SLOT(setView(bool)));
		connect(m_viewFlagItem, SIGNAL(stageIndexSet(unsigned int)), viewNode, SLOT(setStageIndex(unsigned int)));
		connect(viewNode, SIGNAL(viewSet(unsigned int, ViewNode *)), m_viewFlagItem, SLOT(setStageIndex(unsigned int, ViewNode *)));
	}

	// build list of pin sub items to display in the item
	updatePins();
}


//!
//! Destructor of the NodeGraphicsItem class.
//!
NodeGraphicsItem::~NodeGraphicsItem ()
{
	if (m_deleteConnection)
		removeConnectionItems();
	m_nodes.clear();
    --s_numberOfNodeGraphicsItems;
    DEC_INSTANCE_COUNTER
}


///
/// Public Functions
///


//!
//! Enables or disables the graphics item.
//!
//! \param enabled The new enabled state for the item.
//!
void NodeGraphicsItem::setEnabled ( bool enabled )
{
    // enable or disable all child items
    foreach (QGraphicsItem *item, childItems())
        item->setEnabled(enabled);

    QGraphicsItem::setEnabled(enabled);
}


//!
//! Paints the graphics item into a graphics view.
//!
//! \param painter The object to use for painting.
//! \param option Style options for painting the graphics item.
//! \param widget The widget into which to paint the graphics item.
//!
void NodeGraphicsItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
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
    if (m_nodes[0] && m_nodes[0]->isEvaluated())
        linearGradient.setColorAt(1, m_color);
    else
        linearGradient.setColorAt(1, backgroundColor.darker(125));

    // draw the node's shape
    QBrush gradientBrush (linearGradient);
    if (isEnabled() && m_nodes[0] && m_nodes[0]->isEvaluated())
        painter->setPen(QPen(Qt::black, 1));
    else
        painter->setPen(QPen(Qt::gray, 1));
    painter->setBrush(gradientBrush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(rect(), CornerRadius, CornerRadius);

    // draw the node's icon
    //QString iconName = m_nodes[0]->getNodeType().toLower();
    //if (m_nodes[0]->getNodeType() == "Animation")
    //    iconName = "file";
    //QString iconFilename = QString(":/icons/%1.png").arg(iconName);
    //painter->drawImage(QPointF(4, 4), QImage(iconFilename));

    // draw the node's name
    if (isEnabled())
        painter->setPen(QPen(Qt::white, 1));
    else
        painter->setPen(QPen(Qt::gray, 1));
    painter->setClipping(true);
    painter->setClipRect(rect().adjusted(2, 2, -2, -2));
    painter->setRenderHint(QPainter::TextAntialiasing);
    if (m_nodes[0])
        painter->drawText(QPointF(rect().left() + 4, rect().top() + 16), m_nodes[0]->getName());
    //painter->drawText(rect(), Qt::AlignCenter, QString("%1").arg(zValue()));
    painter->setClipping(false);
}


//!
//! Returns the node that the graphics item represents.
//!
//! \return The node that the graphics item represents.
//!
Node * NodeGraphicsItem::getNode () const
{
	if (m_nodes[0])
		return m_nodes[0];
	else
		return NULL;
}


//!
//! Sets the z-index of the graphics item so that it will be drawn over all
//! other graphics items in the scene.
//!
void NodeGraphicsItem::bringToFront ()
{
    // iterate over all graphics items in the scene
    QList<QGraphicsItem *> graphicsItems = scene()->items();
    for (int i = 0; i < graphicsItems.size(); ++i) {
        NodeGraphicsItem *nodeGraphicsItem = dynamic_cast<NodeGraphicsItem *>(graphicsItems[i]);
        // check if the current item is another node graphics item with a depth value higher than this one's
        if (nodeGraphicsItem && nodeGraphicsItem != this && nodeGraphicsItem->zValue() > zValue())
            // reduce the item's depth value by one layer
            nodeGraphicsItem->setZValue(nodeGraphicsItem->zValue() - 1);
    }

    // set this item's depth value so that it appears on top of all other items
    setZValue(s_numberOfNodeGraphicsItems);
}


//!
//! Associates a ConnectionGraphicsItem with the node graphics item.
//!
//! \param item The item to associate with the node graphics item.
//!
void NodeGraphicsItem::addConnectionItem ( ConnectionGraphicsItem *item )
{
    unsigned int id = item->getConnection()->getId();
    m_connectionItems.insert(id, item);
}


//!
//! Removes the association to the given ConnectionGraphicsItem from the
//! node graphics item.
//!
//! \param item The item to no longer associate with the node graphics item.
//!
void NodeGraphicsItem::removeConnectionItem ( ConnectionGraphicsItem *item )
{
    unsigned int id = item->getConnection()->getId();

    if (id != -1)
        m_connectionItems.remove(id);
}


//!
//! Removes all ConnectionGraphicsItem associations from the node graphics
//! item.
//!
void NodeGraphicsItem::removeConnectionItems ()
{
	foreach (ConnectionGraphicsItem *item, m_connectionItems)
		delete item;
}


//!
//! Switch if a Nodes connection will be deteted together with the graphics 
//! item (global default = true).
//!
void NodeGraphicsItem::deleteConnectionsWithItem (const bool value)
{
    m_deleteConnection = value;
}


//!
//! Sets the pin display mode for the item to the given mode.
//!
//! \param pinDisplayMode The pin display mode to set for the item.
//!
void NodeGraphicsItem::setPinDisplayMode ( NodeGraphicsItem::PinDisplayMode pinDisplayMode )
{
    m_pinDisplayMode = pinDisplayMode;
    updatePins();
}


//!
//! Returns the name of the item.
//!
//! \return The Name of the item.
//!
const QString NodeGraphicsItem::getName () const
{
	if (!m_nodes.empty() && m_nodes[0])
		return m_nodes[0]->getName();
	else
		return "";
}


//!
//! Returns a list containing all ConnectionGraphicsItems connected to these node item.
//!
//! \return The list containing the ConnectionGraphicsItems.
//!
const QList<ConnectionGraphicsItem *> NodeGraphicsItem::getConnectionItemList() const
{
	return m_connectionItems.values();
}


///
/// Public Slots
///


//!
//! Updates the selected state of the graphics item according to the given
//! value.
//!
//! This function is needed because QGraphicsItem::setSelected() is not a slot.
//!
//! \param selected The new selected state for the graphics item.
//!
void NodeGraphicsItem::setSelected ( bool selected )
{
    QGraphicsItem::setSelected(selected);
}


//!
//! Begins or ends creating a connection between nodes.
//! Is called when a parameter represented by a pin has been clicked.
//!
//! \param pinItem The pin graphics item representing the parameter.
//! \param parameter The parameter that was clicked.
//!
void NodeGraphicsItem::createConnection ( PinGraphicsItem *pinItem, Parameter *parameter )
{
    // check if there already is a connection being created
    if (!ConnectionGraphicsItem::isConnectionCreated())
        beginCreatingConnection(pinItem, parameter);
    else
        endCreatingConnection(pinItem, parameter);
}


//!
//! Refresh the graphics item to reflect changes in the node that it
//! represents.
//!
void NodeGraphicsItem::refresh ()
{
    if (m_nodes[0]) {
        // update flag graphics items
        m_evalFlagItem->setChecked(m_nodes[0]->isEvaluated());
        if (m_viewFlagItem)
            m_viewFlagItem->setChecked(dynamic_cast<ViewNode *>(m_nodes[0])->isViewed());
    }

    updatePins();
	update();
}


///
/// Protected Functions
///


//!
//! Event handler that reacts to mouse press events.
//!
//! \param event The object containing details about the event.
//!
void NodeGraphicsItem::mousePressEvent ( QGraphicsSceneMouseEvent *event )
{
    if (event->button() == Qt::LeftButton)
        bringToFront();

    BaseRectItem::mousePressEvent(event);
}


//!
//! Event handler that reacts to mouse release events.
//!
//! \param event The object containing details about the event.
//!
void NodeGraphicsItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
    //event->ignore();

    BaseRectItem::mouseReleaseEvent(event);
}


//!
//! Event handler that reacts to context menu events.
//!
//! \param event The object containing details about the event.
//!
void NodeGraphicsItem::contextMenuEvent ( QGraphicsSceneContextMenuEvent *event )
{
    m_hovered = true;
    update();

    // create evaluate flag action
    QAction *evalFlagAction = new QAction(tr("&Evaluate"), this);
    evalFlagAction->setShortcut(Qt::Key_E);
    evalFlagAction->setCheckable(true);
    evalFlagAction->setChecked(m_nodes[0]->isEvaluated());
    connect(evalFlagAction, SIGNAL(toggled(bool)), m_nodes[0], SLOT(setEvaluate(bool)));

    // prepare view flag action
    QAction *viewFlagAction = 0;
    if (m_viewFlagItem) {
        ViewNode *viewNode = dynamic_cast<ViewNode *>(m_nodes[0]);
        // create view flag action
        viewFlagAction = new QAction(tr("&View"), this);
        viewFlagAction->setShortcut(Qt::Key_V);
        viewFlagAction->setCheckable(true);
        viewFlagAction->setChecked(viewNode->isViewed());
        connect(viewFlagAction, SIGNAL(toggled(bool)), viewNode, SLOT(setView(bool)));
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
        m_evalFlagItem->setChecked(m_nodes[0]->isEvaluated());
    else if (m_viewFlagItem && viewFlagAction && selectedAction == viewFlagAction)
        m_viewFlagItem->setChecked(dynamic_cast<ViewNode *>(m_nodes[0])->isViewed());

    m_hovered = false;
    update();
}


//!
//!
//!
QVariant NodeGraphicsItem::itemChange ( GraphicsItemChange change, const QVariant &value )
{
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        emit nodeGraphicsItemMoved(newPos);

        QList<ConnectionGraphicsItem *> connectionItems = m_connectionItems.values();
        foreach (ConnectionGraphicsItem *item, connectionItems) {
            item->updatePath();
        }
    }

    return BaseRectItem::itemChange(change, value);
}


///
/// Private Functions
///


//!
//! Updates the item according to the pin display mode set to display all
//! pins, only connected pins, or all pins in one collapsed pin.
//!
void NodeGraphicsItem::updatePins ()
{
    // destroy all existing pin graphics items
    for (int i = 0; i < m_inputPins.size(); ++i) {
        delete m_inputPins[i];
    }
    for (int i = 0; i < m_outputPins.size(); ++i) {
        delete m_outputPins[i];
    }
    m_inputPins.clear();
    m_outputPins.clear();

    // make sure the node still exists
    if (!m_nodes[0]) {
        Log::warning("The node that the node graphics item represents has been destroyed.", "NodeGraphicsItem::updatePins");
        return;
    }

    ParameterGroup *parameterRoot = m_nodes[0]->getParameterRoot();

    switch (m_pinDisplayMode) {
        case PDM_All:
            {
                // retrieve lists of all input and output parameters from the node
                AbstractParameter::List inputParameters;
                AbstractParameter::List outputParameters;
                m_nodes[0]->fillParameterLists(&inputParameters, &outputParameters);

                // iterate over the list of input parameters
                for (int i = 0; i < inputParameters.size(); ++i)
                    if (inputParameters[i]->isGroup())
                        m_inputPins.append(new PinGraphicsItem(this, i, static_cast<ParameterGroup *>(inputParameters[i]), Parameter::PT_Input));
                    else
                        m_inputPins.append(new PinGraphicsItem(this, i, static_cast<Parameter *>(inputParameters[i])));

                // iterate over the list of output parameters
                for (int i = 0; i < outputParameters.size(); ++i)
                    if (outputParameters[i]->isGroup()) {
                        PinGraphicsItem* newItem = new PinGraphicsItem(this, i, static_cast<ParameterGroup *>(outputParameters[i]), Parameter::PT_Output);
                        //newItem->setObjectName(outputParameters[i]->getName()); /// seim was here !!!
                        m_outputPins.append(newItem);
                    }
                    else {
                        PinGraphicsItem* newItem = new PinGraphicsItem(this, i, static_cast<Parameter *>(outputParameters[i]));
                        //newItem->setObjectName(outputParameters[i]->getName());
                        m_outputPins.append(newItem);
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
                m_nodes[0]->fillParameterLists(&connectedInputParameters, &connectedOutputParameters, &unconnectedInputParametersAvailable, &unconnectedOutputParametersAvailable);

                // iterate over the list of connected input parameters
                for (int i = 0; i < connectedInputParameters.size(); ++i)
                    if (connectedInputParameters[i]->isGroup())
                        m_inputPins.append(new PinGraphicsItem(this, i, static_cast<ParameterGroup *>(connectedInputParameters[i]), Parameter::PT_Input, true));
                    else
                        m_inputPins.append(new PinGraphicsItem(this, i, static_cast<Parameter *>(connectedInputParameters[i])));

                // iterate over the list of connected output parameters
                for (int i = 0; i < connectedOutputParameters.size(); ++i)
                    if (connectedOutputParameters[i]->isGroup())
                        m_outputPins.append(new PinGraphicsItem(this, i, static_cast<ParameterGroup *>(connectedOutputParameters[i]), Parameter::PT_Output, true));
                    else
                        m_outputPins.append(new PinGraphicsItem(this, i, static_cast<Parameter *>(connectedOutputParameters[i])));

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
                m_inputPins.append(new PinGraphicsItem(this, 0, parameterRoot, Parameter::PT_Input));
            if (parameterRoot->contains(Parameter::PT_Output))
                m_outputPins.append(new PinGraphicsItem(this, 0, parameterRoot, Parameter::PT_Output));
            break;
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
void NodeGraphicsItem::disableOtherPins ( PinGraphicsItem *pinItem, Parameter *parameter )
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
void NodeGraphicsItem::beginCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter )
{
    if (pinItem && parameter) {
        Parameter::PinType pinType = parameter->getPinType();

        // deactivate all pins that do not correspond to the data type of the clicked pin
        foreach (QGraphicsItem *item, scene()->items()) {
            NodeGraphicsItem *nodeItem = dynamic_cast<NodeGraphicsItem *>(item);
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
        //ConnectionGraphicsItem *connectionItem = new ConnectionGraphicsItem(QString::number(444), QColor(0.3, 0.3, 0.3), QPointF(0.0, 0.0), QPointF(0.0, 0.0));
        //parameter->addConnection(connection);
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

        if (m_nodes[0]) {
            Parameter *parameter = new Parameter("connectAllDummy", Parameter::T_Bool, QVariant(false));
            parameter->setNode(m_nodes[0]);
            if (parameter) {
                Connection *connection;
                connection = new Connection(parameter, 0);
                connectionItem->setConnection(connection);
                connectionItem->setObjectName("connectAllDummy");

                // set end or start node item depending on the type of the clicked parameter
                connectionItem->setStartNodeItem(this);
                //connectionItem->setStartPoint(pos() + rect().center());
                connectionItem->setStartPoint(rect().center());
                //connectionItem->setEndPoint(pos() + rect().center());
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
void NodeGraphicsItem::endCreatingConnection ( PinGraphicsItem *pinItem, Parameter *parameter )
{
    if (pinItem && parameter) {
        // obtain the connection from the temporary connection graphics item
        ConnectionGraphicsItem *connectionItem = ConnectionGraphicsItem::getTempItem();
        Connection *connection = connectionItem->getConnection();
        ConnectionGraphicsItem::setTempItem(0);

        if (!connection || !connectionItem) {
            Log::error("No connection item or connection available.", "NodeGraphicsItem::endCreatingConnection");
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
                Log::error("Wrong combination of source and target parameters.", "NodeGraphicsItem::endCreatingConnection");

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
            Log::error("No connection item or connection available.", "NodeGraphicsItem::endCreatingConnection");
            return;
        }
        //delete dummyParameter;
        NodeGraphicsItem *otherNodeGraphicsItem = tempConnectionItem->getStartNodeItem();
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
        NodeGraphicsItem *nodeItem = dynamic_cast<NodeGraphicsItem *>(item);
        if (nodeItem)
            nodeItem->setEnabled(true);
        else {
            ConnectionGraphicsItem *connectionItem = dynamic_cast<ConnectionGraphicsItem *>(item);
            if (connectionItem)
                connectionItem->setEnabled(true);
        }
    }
}


///
/// Private Slots
///


//!
//! Updates the node's pin display mode according to the triggered action.
//!
//! \param action The action that was triggered.
//!
void NodeGraphicsItem::updatePinDisplayMode ( QAction *action )
{
    bool ok = false;
    int mode = action->data().toInt(&ok);
    if (!ok || mode != PDM_All && mode != PDM_Connected && mode != PDM_Collapsed) {
        Log::error("The given action seems not to be a pin display mode action.", "NodeGraphicsItem::pinDisplayModeActionTriggered");
        return;
    }

    m_pinDisplayMode = (PinDisplayMode) mode;
    updatePins();
}


//!
//! Connect by name has been clicked in context menu.
//!
void NodeGraphicsItem::connectByNameClicked ()
{
    createConnection(0, 0);
}

} // end namespace Frapper