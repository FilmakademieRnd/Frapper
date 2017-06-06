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
//! \file "NodeFactory.cpp"
//! \brief Implementation file for NodeFactory class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#include "NodeFactory.h"
#include "Log.h"

namespace Frapper {

///
/// Private Static Data
///


//!
//! A list of node type categories available in the node factory.
//!
QList<NodeType::Category> NodeFactory::s_nodeTypeCategories;

//!
//! The map of node types with node type names as keys and pointers to node
//! type objects as values.
//!
QMap<QString, NodeType *> NodeFactory::s_nodeTypes;

///
/// Public Static Functions
///


//!
//! Initializes the node factory for producing nodes.
//!
void NodeFactory::initialize ()
{
    // initialize the list of node type categories with predefined items
    s_nodeTypeCategories << NodeType::Category("Animation", ":/playLoopingIcon");
    s_nodeTypeCategories << NodeType::Category("Materials", ":/colorSwatchIcon");
    s_nodeTypeCategories << NodeType::Category("Geometry", ":/geometryIcon");
    s_nodeTypeCategories << NodeType::Category("Cameras", ":/cameraIcon");
    s_nodeTypeCategories << NodeType::Category("Lights", ":/lightIcon");
	s_nodeTypeCategories << NodeType::Category("Images", ":/imagesIcon");
	s_nodeTypeCategories << NodeType::CategorySeparator;
	s_nodeTypeCategories << NodeType::Category("Shading", ":/shadingIcon");
    s_nodeTypeCategories << NodeType::Category("Rendering", ":/renderingIcon");
	s_nodeTypeCategories << NodeType::Category("Compositing", ":/compositingIcon");
    s_nodeTypeCategories << NodeType::CategorySeparator;
	s_nodeTypeCategories << NodeType::Category("Export", ":/exportIcon");
	s_nodeTypeCategories << NodeType::Category("Timer", ":/clockIcon");
    s_nodeTypeCategories << NodeType::Category("Utility", ":/wrenchIcon");
    s_nodeTypeCategories << NodeType::Category("Tracking", ":/gridIcon");
    s_nodeTypeCategories << NodeType::Category("Controllers", ":/controllerIcon");
	s_nodeTypeCategories << NodeType::Category("Input", ":/webcamIcon");
    s_nodeTypeCategories << NodeType::Category("Solver", ":/tagIcon");
    s_nodeTypeCategories << NodeType::Category("S3D", ":/3dGlassesIcon");
    s_nodeTypeCategories << NodeType::CategorySeparator;

	// TODO: NILS: This seems to be an internal method and should not be used. 
	//s_nodeTypes.setInsertInOrder(true);
}


//!
//! Loads the XML description of a node type from the file with the given
//! name.
//!
//! \param filename The name of an XML file describing a node type.
//!
void NodeFactory::registerType ( const QString &filename )
{
    NodeType *nodeType = new NodeType(filename);

    // if the node type isn't available delete the NodeType instance right away
    if (!nodeType->isAvailable()) {
        delete nodeType;
        return;
    }

    // check if a node type of the same name already exists
    QString nodeTypeName = nodeType->getName();
    if (s_nodeTypes.contains(nodeTypeName)) {
        Log::warning(QString("A node type of name \"%1\" has already been registered. Please check the node type description files for duplicate node type names.").arg(nodeTypeName), "NodeFactory::registerType");
        delete nodeType;
        return;
    }

    // check if a category for the node type already exists and if not, create one
    QString nodeTypeCategoryName = nodeType->getCategoryName();
    if (nodeTypeCategoryName != "Internal") {
        bool found = false;
        foreach (NodeType::Category category, s_nodeTypeCategories)
            if (category.first == nodeTypeCategoryName)
                found = true;
        if (!found)
            s_nodeTypeCategories << NodeType::Category(nodeTypeCategoryName, QString());
    }

    s_nodeTypes[nodeTypeName] = nodeType;
}


//!
//! Returns the number of node type categories available in the node
//! factory.
//!
//! \return The number of node type categories available in the node factory.
//!
unsigned int NodeFactory::getNumCategories ()
{
    return (unsigned int) s_nodeTypeCategories.size();
}


//!
//! Returns the name of the node type category with the given index.
//!
//! \param index The index of the node type category whose name to return.
//! \return The name of the node type category with the given index.
//!
QString NodeFactory::getCategoryName ( unsigned int index )
{
    return s_nodeTypeCategories[index].first;
}


//!
//! Returns the name of the category under which nodes of the given type
//! name should be filed.
//!
//! \param typeName The name of a node type for which to return the category name.
//! \return The category name corresponding to the given node type name.
//!
QString NodeFactory::getCategoryName ( const QString &typeName )
{
    if (s_nodeTypes.contains(typeName))
        return s_nodeTypes[typeName]->getCategoryName();
	else if (typeName == "BackDrop")
		return QObject::tr("BackDrop");
    else
        return QObject::tr("Unknown");
}


//!
//! Returns the name of the icon of the node type category with the given
//! index.
//!
//! \param index The index of the node type category whose icon name to return.
//! \return The name of the icon of the node type category with the given index.
//!
QString NodeFactory::getCategoryIconName ( unsigned int index )
{
    return s_nodeTypeCategories[index].second;
}


//!
//! Returns the name of the icon of the category under which nodes of the
//! given type name should be filed.
//!
//! \param typeName The name of a node type for which to return the category icon.
//! \return The name of the category icon corresponding to the given node type name.
//!
QString NodeFactory::getCategoryIconName ( const QString &typeName )
{
    if (s_nodeTypes.contains(typeName)) {
        QString categoryName = s_nodeTypes[typeName]->getCategoryName();
        foreach (NodeType::Category category, s_nodeTypeCategories)
            if (category.first == categoryName)
                return category.second;
    }
    return QString();
}


//!
//! Returns the list of types that are registered with the NodeFactory.
//!
//! \param internalTypes Flag to control whether to include internal node types in the result.
//! \return The list of types that are registered with the NodeFactory.
//!
QStringList NodeFactory::getTypeNames ( bool internalTypes /* = false */ )
{
    if (internalTypes)
        return s_nodeTypes.keys();

    QStringList result;
    NodeType::Map::iterator nodeTypeIterator = s_nodeTypes.begin();
    while (nodeTypeIterator != s_nodeTypes.end()) {
        QString typeName = nodeTypeIterator.key();
        NodeType *nodeType = nodeTypeIterator.value();
        if (!nodeType->isInternal())
            result << typeName;
        ++nodeTypeIterator;
    }
    return result;
}


//!
//! Returns the color associated with the node type of the given name.
//!
//! \param typeName The name of the node type for which to return the associated color.
//! \return A color representing nodes of the given type name.
//!
QColor NodeFactory::getColor ( const QString &typeName )
{
    if (s_nodeTypes.contains(typeName))
        return s_nodeTypes[typeName]->getColor();
    else
        return QColor(Qt::black);
}


//!
//! Returns whether errors or warnings occured while parsing the
//! description file for the node type with the given name.
//!
//! \param typeName The name of the node type for which to return whether errors or warnings occured.
//! \return True if errors or warnings occured while parsing the description file for the node type with the given name, otherwise False.
//!
bool NodeFactory::isErroneous ( const QString &typeName )
{
    if (s_nodeTypes.contains(typeName))
        return s_nodeTypes[typeName]->isErroneous();
    else
        return false;
}


//!
//! Creates a node of the given type name with the given name.
//!
//! \param typeName The name of the type to use for the new node.
//! \param name The name to give to the new node.
//! \return A pointer to the new node.
//!
Node * NodeFactory::createNode ( const QString &typeName, const QString &name )
{
    Node *node;
    // check if a node type of the given name is registered
    if (s_nodeTypes.contains(typeName))
        // create a new node of the given type
        node = s_nodeTypes[typeName]->createNode(name);
    else {
        // create a new node with an empty parameter tree and mark it as having an unknown type
        node = new Node(name, new ParameterGroup());
        node->setTypeUnknown();
    }
    node->setTypeName(typeName);
    return node;
}


//!
//! Frees all resources that were used by private static data of the node
//! factory class.
//!
void NodeFactory::freeResources ()
{
    NodeType::Map::iterator nodeTypeIterator = s_nodeTypes.begin();
    while (nodeTypeIterator != s_nodeTypes.end()) {
        QString typeName = nodeTypeIterator.key();
        NodeType *nodeType = nodeTypeIterator.value();
        delete nodeType;
        s_nodeTypes[typeName] = 0;  // not needed actually, but still good style
        ++nodeTypeIterator;
    }
}

} // end namespace Frapper