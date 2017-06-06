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
//! \file "NodeFactory.h"
//! \brief Header file for NodeFactory class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include "FrapperPrerequisites.h"
#include <QtCore/QString>
#include <QColor>
#include "Node.h"
#include "NodeType.h"

namespace Frapper {

    //!
    //! Static class managing node types and Node object creation.
    //!
    class FRAPPER_CORE_EXPORT NodeFactory
    {

    public: // static functions

        //!
        //! Initializes the node factory for producing nodes.
        //!
        static void initialize ();

        //!
        //! Loads the XML description of a node type from the file with the given
        //! name.
        //!
        //! \param filename The name of an XML file describing a node type.
        //!
        static void registerType ( const QString &filename );

        //!
        //! Returns the number of node type categories available in the node
        //! factory.
        //!
        //! \return The number of node type categories available in the node factory.
        //!
        static unsigned int getNumCategories ();

        //!
        //! Returns the name of the node type category with the given index.
        //!
        //! \param index The index of the node type category whose name to return.
        //! \return The name of the node type category with the given index.
        //!
        static QString getCategoryName ( unsigned int index );

        //!
        //! Returns the name of the category under which nodes of the given type
        //! name should be filed.
        //!
        //! \param typeName The name of a node type for which to return the category name.
        //! \return The category name corresponding to the given node type name.
        //!
        static QString getCategoryName ( const QString &typeName );

        //!
        //! Returns the name of the icon of the node type category with the given
        //! index.
        //!
        //! \param index The index of the node type category whose icon name to return.
        //! \return The name of the icon of the node type category with the given index.
        //!
        static QString getCategoryIconName ( unsigned int index );

        //!
        //! Returns the name of the icon of the category under which nodes of the
        //! given type name should be filed.
        //!
        //! \param typeName The name of a node type for which to return the category icon.
        //! \return The name of the category icon corresponding to the given node type name.
        //!
        static QString getCategoryIconName ( const QString &typeName );

        //!
        //! Returns the list of types that are registered with the NodeFactory.
        //!
        //! \param internalTypes Flag to control whether to include internal node types in the result.
        //! \return The list of types that are registered with the NodeFactory.
        //!
        static QStringList getTypeNames ( bool internalTypes = false );

        //!
        //! Returns the color associated with the node type of the given name.
        //!
        //! \param typeName The name of the node type for which to return the associated color.
        //! \return A color representing nodes of the given type name.
        //!
        static QColor getColor ( const QString &typeName );

        //!
        //! Returns whether errors or warnings occured while parsing the
        //! description file for the node type with the given name.
        //!
        //! \param typeName The name of the node type for which to return whether errors or warnings occured.
        //! \return True if errors or warnings occured while parsing the description file for the node type with the given name, otherwise False.
        //!
        static bool isErroneous ( const QString &typeName );

        //!
        //! Creates a node of the given type name with the given name.
        //!
        //! \param typeName The name of the type to use for the new node.
        //! \param name The name to give to the new node.
        //! \return A pointer to the new node.
        //!
        static Node * createNode ( const QString &typeName, const QString &name );

        //!
        //! Frees all resources that were used by private static data of the node
        //! factory class.
        //!
        static void freeResources ();

    private: // static data

        //!
        //! A list of node type categories available in the node factory.
        //!
        static NodeType::CategoryList s_nodeTypeCategories;

        //!
        //! The map of node types with node type names as keys and pointers to node
        //! type objects as values.
        //!
        static NodeType::Map s_nodeTypes;

    };

} // end namespace Frapper

#endif
