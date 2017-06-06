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
//! \file "NodeModel.cpp"
//! \brief Implementation file for NodeModel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.11.2013 (last updated)
//!

#include "NodeModel.h"
#include "NodeFactory.h"
#include <QGraphicsScene>
#include <QProgressDialog>
#include "Log.h"
#include "BackDropNode.h"

namespace Frapper {

#define DEBUG_LOG_LOCKS
#undef DEBUG_LOG_LOCKS
#ifdef DEBUG_LOG_LOCKS
#define DEBUG_LOG_LOCK(functionName) Log::debug("Node model has been locked.", functionName)
#define DEBUG_LOG_UNLOCK(functionName) Log::debug("Node model has been unlocked.", functionName)
#define DEBUG_LOG_UNLOCK_IMMINENT(functionName) Log::debug("Node model will be unlocked immediately.", functionName)
#else
#define DEBUG_LOG_LOCK(functionName)
#define DEBUG_LOG_UNLOCK(functionName)
#define DEBUG_LOG_UNLOCK_IMMINENT(functionName)
#endif

///
/// Constructors and Destructors
///


//!
//! Constructor of the NodeModel class.
//!
NodeModel::NodeModel () :
m_updating(false),
m_selectingAll(false)
{
    // set the horizontal labels for the standard item model
    setHorizontalHeaderLabels(QStringList() << "Scene");
}


//!
//! Destructor of the NodeModel class.
//!
NodeModel::~NodeModel ()
{
    QProgressDialog progressDialog (tr("Freeing resources of node model..."), QString(), 0, m_standardItemNodeMap.size());
    progressDialog.setWindowTitle(tr("Closing"));
    progressDialog.setWindowModality(Qt::ApplicationModal);
    progressDialog.setMaximum(m_standardItemNodeMap.size());
    int progress = 0;

    beginUpdate();
    // iterate over all standard items
    QHashIterator<QString, QStandardItem *> it (m_standardItemNodeMap);
    it.toBack();
    while (it.hasPrevious()) {
        it.previous();
        // delete the node with the given name from the model
        QStandardItem *item = it.value();
        deleteNode(item->text());

        ++progress;
        progressDialog.setValue(progress);
    }
    endUpdate();
}


///
/// Public Functions
///

//!
//! Sets an updating flag for the node model to prevent emitting redundant
//! updated() signals.
//!
//! Should be called to initiate a substantial update of the model.
//!
//! \see endUpdate()
//!
void NodeModel::beginUpdate ()
{
    m_updating = true;
}


//!
//! Resets the updating flag for the node model and emits the updated()
//! signal.
//!
//! Should be called when a substantial update of the model that was initiated
//! by calling beginUpdate() has finished.
//!
//! \see beginUpdate()
//!
void NodeModel::endUpdate ()
{
    m_updating = false;

    // notify connected objects that the node model has been modified
    emit modified();
}


//!
//! Sets the m_selectingAll flag for the node model to prevent emitting
//! redundant nodeSelected() signals.
//!
//! Should be called before selecting all nodes in the node model.
//!
//! \see endSelectingAll()
//!
void NodeModel::beginSelectingAll ()
{
    m_selectingAll = true;
}


//!
//! Resets the m_selectingAll flag for the node model and optionally emits
//! the nodeSelected() signal for the last selected node.
//!
//! Should be called after selecting all nodes in the node model.
//!
//! \param emitSignal Flag to control whether to emit the nodeSelected() signal.
//! \see beginSelectingAll()
//!
void NodeModel::endSelectingAll ( bool emitSignal /* = true */ )
{
    m_selectingAll = false;

    if (emitSignal)
        // notify connected objects that a node in the node model has been selected
        foreach (Node *node, m_nodeMap.values())
        if (node->isSelected()) {
            emit nodeSelected(node);
            return;
        }
}

//!
//! Creates a backdrop special node type
//!
//! \param typeName The name of the type of node to add to the model.
//! \param name The name to give to the node to add to the model.
//! \param visible Flag that controls whether the object represented by the node should appear in views.
//! \return The new node or 0 if the node could not be created.
//!
Node * NodeModel::createBackDrop (const QString name /* = "" */, const QPointF position, const qreal width /* = 200 */, const qreal height /* = 200 */, const QColor backgroundColor /* = QColor(48,48,48) */, const QColor textColor /* = QColor::white */, const QString text /* = "" */, const int textSize /* = 11 */, const QString head /* = "" */, const int headSize /* = 22 */, const bool lockChange /* = false */ )
{
	QMutexLocker mutexLocker (&m_mutex);
	DEBUG_LOG_LOCK("NodeModel::createBackDrop");

	Node *node;
	node = new BackDropNode(name, new ParameterGroup(), position, width, height, backgroundColor, textColor, text, textSize, head, headSize, lockChange);
	node->setTypeName("Backdrop");

	if (!node) {
		Log::error(QString("Node of type \"%1\" could not be created.").arg("BackDrop"), "NodeModel::createBackDrop");
		DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::createBackDrop");
		return 0;
	}
	QString nodeName = node->getName();
	QString nodeCategoryName = NodeFactory::getCategoryName("BackDrop");

	// set up connections for the new node
	connect(node, SIGNAL(nodeChanged()), SIGNAL(modified()));

	// check if there already is a standard item for the category
	bool nodeCategoryNameExists = false;
	int i = 0;
	while (!nodeCategoryNameExists && i < m_categoryItems.size())
		if (m_categoryItems.at(i)->text() == nodeCategoryName)
			nodeCategoryNameExists = true;
		else
			++i;
	if (!nodeCategoryNameExists) {
		// add a new node category standard item
		QStandardItem *nodeCategoryItem = new QStandardItem(nodeCategoryName);
		nodeCategoryItem->setToolTip(nodeCategoryName);
		nodeCategoryItem->setSelectable(false);
		m_categoryItems.append(nodeCategoryItem);
		invisibleRootItem()->appendRow(nodeCategoryItem);
	}

	m_nodeMap.insert(nodeName, node);

	// create a new standard item in the node's category
	QStandardItem *categoryItem = getCategoryItem(nodeCategoryName);
	if (categoryItem) {
		QStandardItem *nodeItem = new QStandardItem(nodeName);
		nodeItem->setData(nodeName);
		m_standardItemNodeMap.insert(nodeName, nodeItem);
		categoryItem->appendRow(nodeItem);
	}


	emit modified(); 

	DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::createBackDrop");

	return node;
}

//!
//! Creates a node of the given type with the given name in the node model.
//!
//! \param typeName The name of the type of node to add to the model.
//! \param name The name to give to the node to add to the model.
//! \param visible Flag that controls whether the object represented by the node should appear in views.
//! \return The new node or 0 if the node could not be created.
//!
Node * NodeModel::createNode ( const QString &typeName, const QString &name /* = "" */, bool visible /* = true */ )
{
    QMutexLocker mutexLocker (&m_mutex);
    DEBUG_LOG_LOCK("NodeModel::createNode");

    Node *node = NodeFactory::createNode(typeName, name);
    if (!node) {
        Log::error(QString("Node of type \"%1\" could not be created.").arg(typeName), "NodeModel::createNode");
        DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::createNode");
        return 0;
    }
    QString nodeName = node->getName();
    QString nodeCategoryName = NodeFactory::getCategoryName(typeName);

    // set up connections for the new node
    connect(node, SIGNAL(nodeChanged()), SIGNAL(modified()));

    // check if there already is a standard item for the category
    bool nodeCategoryNameExists = false;
    int i = 0;
    while (!nodeCategoryNameExists && i < m_categoryItems.size())
        if (m_categoryItems.at(i)->text() == nodeCategoryName)
            nodeCategoryNameExists = true;
        else
            ++i;
    if (!nodeCategoryNameExists) {
        // add a new node category standard item
        QStandardItem *nodeCategoryItem = new QStandardItem(nodeCategoryName);
        nodeCategoryItem->setToolTip(nodeCategoryName);
        nodeCategoryItem->setSelectable(false);
        m_categoryItems.append(nodeCategoryItem);
        invisibleRootItem()->appendRow(nodeCategoryItem);
    }

    m_nodeMap.insert(nodeName, node);

    // create a new standard item in the node's category
    QStandardItem *categoryItem = getCategoryItem(nodeCategoryName);
    if (categoryItem) {
        QStandardItem *nodeItem = new QStandardItem(nodeName);
		nodeItem->setData(nodeName);
        m_standardItemNodeMap.insert(nodeName, nodeItem);
        if (visible)
            categoryItem->appendRow(nodeItem);
    }


	emit modified(); 

    DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::createNode");

    return node;
}

//!
//! Creates a connection.
//!
//! \param typeName The name of the type of node to add to the model.
//! \return The new connection or 0 if the connection could not be created.
//!
Connection * NodeModel::createConnection ( Parameter *sourceParameter, Parameter *targetParameter )
{
    QMutexLocker mutexLocker (&m_mutex);
    DEBUG_LOG_LOCK("NodeModel::createNode");

    Connection *connection = new Connection(sourceParameter, targetParameter);
    if (!connection) {
        Log::error(QString("Connection could not be created."), "NodeModel::createConnection");
        DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::createConnection");
        return 0;
    }

    Node *sourceParameterNode = sourceParameter->getNode();
    Node *targetParameterNode = targetParameter->getNode();

    if(!sourceParameterNode || !targetParameterNode)
        return 0;

    QString connectionName = sourceParameterNode->getName() + "." + sourceParameter->getName() + " -> " + targetParameterNode->getName() + "." + targetParameter->getName();
    QString connectionId = QString::number(connection->getId());
    QString categoryName = "Connections";

    // check if there already is a standard item for the category
    bool categoryNameExists = false;
    int i = 0;
    while (!categoryNameExists && i < m_categoryItems.size())
        if (m_categoryItems.at(i)->text() == categoryName)
            categoryNameExists = true;
        else
            ++i;
    if (!categoryNameExists) {
        // add a new node category standard item
        QStandardItem *categoryItem = new QStandardItem(categoryName);
        categoryItem->setSelectable(false);
        m_categoryItems.append(categoryItem);
        invisibleRootItem()->appendRow(categoryItem);
    }

    m_connectionMap.insert(connectionId, connection);

    // create a new standard item in the category
    QStandardItem *categoryItem = getCategoryItem(categoryName);
    if (categoryItem) {
        QStandardItem *connectionItem = new QStandardItem(connectionName);
		connectionItem->setData( connectionId);
        m_standardItemConnectionMap.insert(connectionId, connectionItem);
        categoryItem->appendRow(connectionItem);
        categoryItem->sortChildren(0);
    }

	emit modified();

    DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::createConnection");

    return connection;
}


//!
//! Groups the nodes from the given list into a node group with the given name.
//!
//! \param groupName The name of the new node group.
//! \param nodeList A list containing the nodes to be grouped.
//!
void NodeModel::groupNodes ( const QString &groupName, const QList<Node *> &nodeList )
{
	if (m_nodeGroupMap.keys().contains(groupName)) {
        Log::error(QString("Node Group already exists."), "NodeModel::groupNodes");
	}
	else {
		foreach (Node *node, nodeList)
			m_nodeGroupMap.insert(groupName, node);
	}
}


//!
//! Ungroups the nodes contained in the group with the given name.
//!
//! \param groupName The name of the node group.
//! \return A list containing the ungrouped nodes.
//!
QList<Node *> NodeModel::unGroupNodes ( const QString &groupName )
{
	QList<Node *> returnvalue = m_nodeGroupMap.values(groupName);
	if (!returnvalue.empty()) 
		m_nodeGroupMap.remove(groupName);

	return returnvalue;
}


//!
//! Ungroups the node with the given name contained in the group with the given name.
//!
//! \param nodeName The name of the node.
//! \param groupName The name of the node group.
//!
void NodeModel::unGroupNode ( const QString &nodeName, const QString &groupName )
{
	m_nodeGroupMap.remove(groupName, getNode(nodeName));
}


//!
//! Returns the node with the given name or 0 if no node with the given
//! name can be found.
//!
//! \param name The name of the node to return.
//! \return The node with the given name or 0.
//!
Node * NodeModel::getNode ( const QString &name ) const
{
	const QHash<QString, Node*>::ConstIterator iter = m_nodeMap.constFind(name);
	if (iter != m_nodeMap.end())
		return iter.value();
    else
        return 0;
}


//!
//! Returns a list of nodes of the given type name contained in the node
//! model.
//!
//! \param typeName The name of the type of nodes to return, or an empty string to return all nodes.
//! \return A list of nodes of the given type name.
//!
QList<Node *> NodeModel::getNodes ( const QString &typeName /* = "" */ ) const
{
    QList<Node *> result;

    QHashIterator<QString, Node *> it (m_nodeMap);
    while (it.hasNext()) {
        it.next();
        Node *node = it.value();
        if ((typeName == "") || (node && node->getTypeName() == typeName))
            result << node;
    }
    return result;
}


//!
//! Returns a list of nodes contained in the given node group.
//!
//! \param groupName The name of the group of nodes to return, or an empty string to return all nodes.
//! \return A list of nodes contained in the group with the given name.
//!
QList<Node *> NodeModel::getGroupedNodes ( const QString &groupName/* = ""*/ ) const
{
	if (groupName.isEmpty())
		return m_nodeGroupMap.values();
	else
		return m_nodeGroupMap.values(groupName);
}


//!
//! Returns a list of all ungrouped nodes.
//!
//! \return A list of all ungrouped nodes.
//!
QList<Node *> NodeModel::getUngroupedNodes ( ) const
{
	QList<Node *> ungroupedNodes;
	const QList<Node *> &groupedNodes = getGroupedNodes();

	foreach (Node* node, m_nodeMap)
		if (!groupedNodes.contains(node))
			ungroupedNodes.append(node);

	return ungroupedNodes;
}


//!
//! Returns a list of connections of the given type name contained in the node
//! model.
//!
//! \return A list of connections.
//!
QList<Connection *> NodeModel::getConnections () const
{
    return m_connectionMap.values();
}


//!
//! Returns a connection of the given name contained in the node
//! model.
//!
//! \parameter name The name of the connection.
//! \return A list of connections.
//!
Connection * NodeModel::getConnection (const QString &name) const
{
	return m_connectionMap.value(name);
}


//!
//! Returns the list of names of nodes of the given type name contained in
//! the node model.
//!
//! \param typeName The name of the type of node names to return, or an empty string to return all node names.
//! \return A list of names of nodes of the given type name.
//!
QStringList NodeModel::getNodeNames ( const QString &typeName /* = "" */ ) const
{
    QStringList result;
    const QList<Node *> &nodes = getNodes(typeName);
    foreach (const Node *node, nodes)
        result << node->getName();
    return result;
}


//!
//! Returns the list of names of nodes of the given group.
//!
//! \param groupName The name of the group of nodes to return, or an empty string to return all nodes.
//! \return A list of names of nodes of the given group name.
//!
QStringList NodeModel::getGroupedNodesNames ( const QString &groupName ) const
{
	QStringList result;
    const QList<Node *> &nodes = getGroupedNodes(groupName);
    foreach (const Node *node, nodes)
        result << node->getName();
    return result;
}


//!
//! Returns the list of names of all node groups.
//!
//! \return A list of names of all node groups.
//!
QStringList NodeModel::getGroupNames ( ) const
{
	return m_nodeGroupMap.uniqueKeys();
}


//!
//! Returns the standard item (node) with the given name or 0 if no item with the
//! given name can be found.
//!
//! \param name The name of the standard item to return.
//! \return The item with the given name or 0.
//!
QStandardItem * NodeModel::getStandardItemNode ( const QString &name ) const
{
    if (m_standardItemNodeMap.contains(name))
        return m_standardItemNodeMap.value(name);
    else
        return 0;
}

//!
//! Returns the standard item (connection) with the given name or 0 if no item with the
//! given name can be found.
//!
//! \param name The name of the standard item to return.
//! \return The item with the given name or 0.
//!
QStandardItem * NodeModel::getStandardItemConnection ( const QString &name ) const
{
    if (m_standardItemConnectionMap.contains(name))
        return m_standardItemConnectionMap.value(name);
    else
        return 0;
}

//!
//! Returns the name of the node group containing the node with the given name.
//!
//! \return The name of the node group containing the node with the given name.
//!
QString NodeModel::findGroupByNodeName( const QString &nodeName ) const
{
	const QList<QString> &groupNames = m_nodeGroupMap.keys();
	foreach (const QString &groupName, groupNames) {
		const QList<Node *> groupNodes = m_nodeGroupMap.values(groupName);
		foreach(const Node *groupNode, groupNodes) {
			if (groupNode->getName() == nodeName)
				return QString(groupName);
		}
	}
	return QString();
}

//!
//! Sets the selection state of the items in the given selection to the given
//! value.
//!
//! \param selection The items to set the selection state for.
//! \param selected The selection state to set for the items.
//!
void NodeModel::setSelected ( const QItemSelection &selection, bool selected )
{
	QStringList selectionList;
    // iterate over the list of QItemSelectionRange objects
    for (int i = 0; i < selection.size(); ++i) {
        QItemSelectionRange range = selection.at(i);
        // get the model index of the parent item
        QModelIndex parentModelIndex = range.parent();
        if (parentModelIndex.isValid()) {
            // get the parent item from its index
            QStandardItem *parentItem = itemFromIndex(parentModelIndex);
            // iterate over the rows of selected items
            for (int row = range.top(); row < range.top() + range.height(); ++row) {
                QStandardItem *item = parentItem->child(row);
                if (item)
					selectionList.append(item->text());
            }
        }
    }
	if (!selectionList.empty())
		setSelected(selectionList, selected);
}


//!
//! Deletes the node with the given name from the node model.
//!
//! \param name The name of the node to delete from the node model.
//!
void NodeModel::deleteNode ( const QString &name )
{
    QMutexLocker mutexLocker (&m_mutex);
    DEBUG_LOG_LOCK("NodeModel::deleteNode");

    Node *node = m_nodeMap.value(name);
    if (!node) {
        Log::error(QString("Node not found: \"%1\"").arg(name), "NodeModel::deleteNode");
        DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::deleteNode");
        return;
    }
    QStandardItem *standardItem = m_standardItemNodeMap.value(name);
    if (!standardItem) {
        Log::error(QString("Standard item not found: \"%1\"").arg(name), "NodeModel::deleteNode");
        DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::deleteNode");
        return;
    }

    // check if the item is contained in the model
    if (standardItem->row() > -1)
        // remove the corresponding item from the model
        standardItem->parent()->removeRow(standardItem->row());

    // delete the node
    delete node;
    node = 0;

    // remove the object from the maps
    m_nodeMap.remove(name);
    m_standardItemNodeMap.remove(name);

    mutexLocker.unlock();
    DEBUG_LOG_UNLOCK("NodeModel::deleteNode");

    // notify connected objects that the node has been deleted
    emit nodeDeleted(name);

    if (!m_updating)
        // notify connected objects that the node model has been modified
        emit modified();
}

//!
//! Deletes the connection with the given name from the node model.
//!
//! \param name The name of the connection to delete from the node model.
//!
void NodeModel::deleteConnection ( const QString &name )
{
    // QMutexLocker mutexLocker (&m_mutex);
    // DEBUG_LOG_LOCK("NodeModel::deleteConnection");

    Connection *connection = m_connectionMap.value(name);
    if (!connection) {
        Log::error(QString("Connection not found: \"%1\"").arg(name), "NodeModel::deleteConnection");
        DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::deleteConnection");
        return;
    }

    QStandardItem *standardItem = m_standardItemConnectionMap.value(name);
    if (!standardItem) {
        Log::error(QString("Standard item not found: \"%1\"").arg(name), "NodeModel::deleteConnection");
        DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::deleteConnection");
        return;
    }

    // check if the item is contained in the model
    if (standardItem->row() > -1)
        // remove the corresponding item from the model
        standardItem->parent()->removeRow(standardItem->row());

    // reset the target parameter
    Parameter *targetParameter = connection->getTargetParameter();
    // delete the connection
    delete connection;
    connection = 0;

    //mutexLocker.unlock();
    //DEBUG_LOG_UNLOCK("NodeModel::deleteConnnection");

    if (targetParameter)
        targetParameter->reset();

    // remove the object from the maps
    m_connectionMap.remove(name);
	m_standardItemConnectionMap.remove(name);

    // notify connected objects that the node has been deleted
    //emit nodeDeleted(name);

    if (!m_updating)
        // notify connected objects that the node model has been modified
        emit modified();
}

///
/// Public Slots
///


//!
//! Selects or deselects the object with the given name.
//!
//! \param name The name of the object to select or deselect.
//! \param selected The new selected state for the object.
//!
void NodeModel::setSelected ( const QStringList &names, bool selected )
{
    QList<Node *> nodes;

	foreach(const QString &name, names) {
		if (selected)
			m_selectedNodes.append(name);
		else
			m_selectedNodes.removeAll(name);
		Node *node = getNode(name);
		if (node)
			nodes.append(node);
	}

	if (!nodes.empty()) {
        nodes[0]->setSelected(selected);
        if (selected && !m_selectingAll) {
            // notify connected objects that the node has been selected
            emit nodeSelected(nodes[0]);  // <-- old signal for backward compatibility 
			emit nodesSelected(nodes);
		}

        // notify connected objects that the node model has been modified
        //emit modified();
    }
}


//!
//! Gives the node with the given old name the given new name.
//!
//! \param oldName The old name of the scene object.
//! \param newName The new name of the scene object.
//!
void NodeModel::renameNode ( const QString &oldName, const QString &newName )
{
    Node *node = getNode(oldName);
    if (!node) {
        Log::error("Node \"%1\" could not be found.", "NodeModel::renameNode");
        return;
    }

    QString name (newName);

    // resolve German characters
    name = name.replace(QRegExp("[ß]"), "ss")
        .replace(QRegExp("[ä]"), "ae").replace(QRegExp("[ö]"), "oe").replace(QRegExp("[ü]"), "ue")
        .replace(QRegExp("[Ä]"), "Ae").replace(QRegExp("[Ö]"), "Oe").replace(QRegExp("[Ü]"), "Ue");

    // replace non-word characters in the name with underscores
    name = name.replace(QRegExp("[\\W]"), "_");

    // check if the node's name has actually changed
    if (name == oldName) {
        Log::debug("Node name has not changed.", "NodeModel::renameNode");
        return;
    }

    // make sure no other object of the same name exists
    if (m_nodeMap.contains(name)) {
        Log::warning(QString("The node \"%1\" cannot be renamed. A node called \"%2\" already exists.").arg(oldName).arg(newName), "NodeModel::renameNode");
        return;
    }

    QMutexLocker mutexLocker (&m_mutex);
    DEBUG_LOG_LOCK("NodeModel::renameNode");

    QStandardItem *standardItem = m_standardItemNodeMap[oldName];
    node->setName(name);
    if (standardItem) {
        standardItem->setText(name);
		standardItem->setData(name);
	}

    // update the name in the maps
    m_nodeMap.remove(oldName);
    m_nodeMap.insert(name, node);
    m_standardItemNodeMap.remove(oldName);
    m_standardItemNodeMap.insert(name, standardItem);

	emit modified();

    DEBUG_LOG_UNLOCK_IMMINENT("NodeModel::renameNode");
}


//!
//! Changes the value of the parameter with the given name of the object
//! with the given name to the given value.
//!
//! \param name The name of the object for which to change the parameter.
//! \param parameterName The name of the parameter to change.
//! \param value The new value for the parameter.
//!
void NodeModel::changeParameter ( const QString &name, const QString &parameterName, const QVariant &value )
{
    Node *node = getNode(name);
    
	if (node) {
		// check if the parameter's value has actually changed
		QVariant oldValue = node->getValue(parameterName);
		QVariant newValue = value;
		if (newValue != oldValue) {
			// apply the new value for the parameter
			//QMutexLocker mutexLocker (&m_mutex);
			node->setValue(parameterName, value, true);
		
			const int index = parameterName.indexOf('[');
			QString shortName;
			if (index > -1)
				shortName = parameterName.left(index);
			else
				shortName = parameterName;

			Parameter* parameter = node->getParameter(shortName);
			if( parameter )
				parameter->executeChange();
			//mutexLocker.unlock();

			// notify connected objects that the node model has been modified
			emit modified();
		}
	}
}


//!
//! Changes the self evaluation property of the parameter with the given name of the object
//! with the given name.
//!
//! \param name The name of the object for which to change the parameter.
//! \param parameterName The name of the parameter to change.
//! \param selfEvaluating Enable or disable self evaluating.
//!
void NodeModel::changeParameterPinType ( const QString &name, const QString &parameterName, const Parameter::PinType pinType)
{
	Node *node = getNode(name);

	if (node) {
		Parameter *parameter = node->getParameter(parameterName);
		if (parameter)
			if (pinType != parameter->getPinType()) {
				Connection::Map connectionMap = parameter->getConnectionMap();
				foreach (Connection *connection, connectionMap)
					node->deleteConnection(connection);
				parameter->setPinType(pinType);
				node->notifyChange();
				emit modified();
			}
	}
}


//!
//! Changes the value of the parameter with the given name of the object
//! with the given name to the given value.
//!
//! \param name The name of the object for which to change the parameter.
//! \param parameterName The name of the parameter to change.
//! \param value The new value for the parameter.
//!
void NodeModel::changeParameterSelfEvaluation ( const QString &name, const QString &parameterName, const bool selfEvaluating )
{
	Node *node = getNode(name);

	if (node) {
		Parameter *parameter = node->getParameter(parameterName);
		if (parameter)
			if (selfEvaluating != parameter->isSelfEvaluating()) {
				parameter->setSelfEvaluating(selfEvaluating);
				emit modified();
			}
	}
}


///
/// Private Functions
///


//!
//! Returns the category item that stands for nodes of the given node type.
//!
//! \param nodeCategory The category name of nodes that the category item stands for.
//! \return The category item that stands for nodes of the given node type.
//!
QStandardItem * NodeModel::getCategoryItem ( const QString &nodeCategory )
{
    int i = 0;
    while (i < m_categoryItems.size())
        if (m_categoryItems.at(i)->text() == nodeCategory) {
            return m_categoryItems.at(i);
        } else
            ++i;
        return 0;
}

} // end namespace Frapper