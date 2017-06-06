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
//! \file "NodeType.h"
//! \brief Header file for NodeType class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       09.06.2009 (last updated)
//!

#ifndef NODETYPE_H
#define NODETYPE_H

#include "FrapperPrerequisites.h"
#include <QtCore/QString>
#include <QColor>
#include "Node.h"
#include "ParameterGroup.h"
#include <QtXml/QDomElement>
#include "NodeTypeInterface.h"
#include "InstanceCounterMacros.h"

namespace Frapper {

    //!
    //! Class representing a specific type of nodes.
    //!
    class FRAPPER_CORE_EXPORT NodeType
    {

        ADD_INSTANCE_COUNTER

    public: // type definitions

        //!
        //! Type definition for a map with node type names as keys and pointers to
        //! node type objects as values.
        //!
        typedef QMap<QString, NodeType *> Map;

        //!
        //! Type definition of a node type category data structure containing a
        //! node type category name and node type category icon name.
        //!
        typedef QPair<QString, QString> Category;

        //!
        //! Type definition for a list of node type categories.
        //!
        typedef QList<Category> CategoryList;

    public: // static data

        //!
        //! A special kind of node type category used for separating categories in
        //! menus.
        //!
        static Category CategorySeparator;

    private: // static data

        //!
        //! The indices of nodes created of the different node types.
        //! \see createNode
        //!
        static QMap<QString, unsigned int> s_nodeIndex;

    public: // constructors and destructors

        //!
        //! Constructor of the NodeType class.
        //!
        //! \param filename The name of an XML file describing the node type.
        //!
        NodeType ( const QString &filename );

        //!
        //! Destructor of the NodeType class.
        //!
        ~NodeType ();

    public: // functions

        //!
        //! Returns the name of the node type.
        //!
        //! \return The name of the node type.
        //!
        QString getName () const;

        //!
        //! Returns the name of the category under which nodes of this type should
        //! be filed.
        //!
        //! \return The name of the category under which nodes of this type should be filed.
        //!
        QString getCategoryName () const;

        //!
        //! Returns the color associated with this node type.
        //!
        //! \return The color associated with this node type.
        //!
        QColor getColor () const;

        //!
        //! Returns whether nodes of this type can be created.
        //!
        //! \return True if nodes of this type can be created, otherwise False.
        //!
        bool isAvailable () const;

        //!
        //! Returns whether nodes of this type are reserved for internal use.
        //!
        //! It should not be possible to create nodes of internal node types from
        //! the framework's user interface.
        //!
        //! \return True if nodes of this type are reserved for internal use, otherwise False.
        //!
        bool isInternal () const;

        //!
        //! Returns whether errors or warnings occured while parsing the
        //! description file.
        //!
        //! \return True if errors or warnings occured while parsing the description file, otherwise False.
        //!
        bool isErroneous () const;

        //!
        //! Creates a node of this type.
        //!
        //! \param name The name for the new node.
        //! \return A pointer to a new node of this type.
        //!
        Node * createNode ( const QString &name );

    private: // functions

        //!
        //! Parses the XML description file with the given name and creates the
        //! list of parameters and connectors that nodes of this type hold.
        //!
        //! \param filename The name of the XML description file to parse.
        //!
        bool parseDescriptionFile ( const QString &filename );

        //!
        //! Parses the given DOM element and creates the tree of parameters that
        //! nodes of this type hold.
        //!
        //! \param rootElement The DOM element containing a list of <code>\<parameter\></code> and <code>\<parameters\></code> elements.
        //! \return A parameter group containing the created parameters.
        //!
        ParameterGroup * parseParameters ( QDomElement rootElement );

        //!
        //! Parses the given DOM element and creates the list of forward and backward
        //! affections that nodes of this type hold.
        //!
        //! \param rootElement The DOM element containing a list of <code>\<connector\></code> elements.
        //!
        void parseAffections ( QDomElement rootElement );

    private: // data

        //!
        //! Pointer to a object which implements the NodeTypeInterface.
        //! For Node Plugins.
        //!
        NodeTypeInterface *m_nodeTypeInterface;

        //!
        //! The tree containing the parameters with default values for the node type.
        //!
        ParameterGroup *m_parameterRoot;

        //!
        //! The name of the node type.
        //!
        QString m_name;

        //!
        //! The name of the category under which nodes of this type should be filed.
        //!
        QString m_categoryName;

        //!
        //! The color associated with the node type.
        //!
        QColor m_color;

        //!
        //! Flag that states whether this node type is available.
        //! Is set to false when parsing the XML description file failed.
        //!
        //! \see NodeType::NodeType
        //! \see parseDescriptionFile
        //!
        bool m_available;

        //!
        //! Flag that states whether nodes of this type are reserved for internal
        //! use.
        //!
        bool m_internal;

        //!
        //! Flag that states whether errors or warnings occured while parsing the
        //! description file.
        //!
        bool m_erroneous;

        //!
        //! Which input pin is affecting which output pins.
        //!
        QMultiMap<QString, QString> m_affectionMap;


    };

} // end namespace Frapper

#endif
