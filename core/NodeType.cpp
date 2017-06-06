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
//! \file "NodeType.cpp"
//! \brief Implementation file for NodeType class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       09.06.2009 (last updated)
//!

#include "NodeType.h"
#include "Log.h"
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlInputSource>
#include <QtCore/QPluginLoader>
#include <QtCore/QVarLengthArray>

namespace Frapper {

INIT_INSTANCE_COUNTER(NodeType)

///
/// Public Static Data
///


//!
//! A special kind of node type category used for separating categories in
//! menus.
//!
NodeType::Category NodeType::CategorySeparator = NodeType::Category(QString(), QString());


///
/// Private Static Data
///


//!
//! The index of nodes created of this node type.
//! \see createNode
//!
QMap<QString, unsigned int> NodeType::s_nodeIndex;


///
/// Constructors and Destructors
///


//!
//! Constructor of the NodeType class.
//!
//! \param filename The name of an XML file describing the node type.
//!
NodeType::NodeType ( const QString &filename ) :
m_nodeTypeInterface(0),
m_parameterRoot(0),
m_available(false),
m_internal(false),
m_erroneous(false)
{
    m_available = parseDescriptionFile(filename);

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the NodeType class.
//!
NodeType::~NodeType ()
{
    if (m_parameterRoot)
        delete m_parameterRoot;

    DEC_INSTANCE_COUNTER
}


///
/// Public Functions
///


//!
//! Returns the name of the node type.
//!
//! \return The name of the node type.
//!
QString NodeType::getName () const
{
    return m_name;
}


//!
//! Returns the name of the category under which nodes of this type should
//! be filed.
//!
//! \return The name of the category under which nodes of this type should be filed.
//!
QString NodeType::getCategoryName () const
{
    return m_categoryName;
}


//!
//! Returns the color associated with this node type.
//!
//! \return The color associated with this node type.
//!
QColor NodeType::getColor () const
{
    return m_color;
}


//!
//! Returns whether nodes of this type can be created.
//!
//! \return True if nodes of this type can be created, otherwise False.
//!
bool NodeType::isAvailable () const
{
    return m_available;
}


//!
//! Returns whether nodes of this type are reserved for internal use.
//!
//! It should not be possible to create nodes of internal node types from
//! the framework's user interface.
//!
//! \return True if nodes of this type are reserved for internal use, otherwise False.
//!
bool NodeType::isInternal () const
{
    return m_internal;
}


//!
//! Returns whether errors or warnings occured while parsing the
//! description file.
//!
//! \return True if errors or warnings occured while parsing the description file, otherwise False.
//!
bool NodeType::isErroneous () const
{
    return m_erroneous;
}


//!
//! Creates a node of this type.
//!
//! \param name The name for the new node.
//! \return A pointer to a new node of this type.
//!
Node * NodeType::createNode ( const QString &name )
{
    // make sure the node type interface is available
    if (!m_available || !m_nodeTypeInterface)
        return 0;

    // set an automatic name if no name is given
    QString nodeName = name;
    if (nodeName == "")
        nodeName = QString("%1%2").arg(m_name.toLower()).arg(++s_nodeIndex[m_name]);

    // create a node with the given type with a deep copy of the parameter tree
    Node *node = m_nodeTypeInterface->createNode(nodeName, dynamic_cast<ParameterGroup *>(m_parameterRoot->clone()));

    // set the affections
    for (QMultiMap<QString, QString>::iterator iter = m_affectionMap.begin(); iter != m_affectionMap.end(); ++iter) {
        Parameter *affectingParameter = node->getParameter(iter.key());
        Parameter *affectedParameter = node->getParameter(iter.value());
        if (affectingParameter && affectedParameter)
            affectingParameter->addAffectedParameter(affectedParameter);
    }

    return node;
}


///
/// Private Functions
///


//!
//! Parses the XML description file with the given name and creates the
//! list of parameters and connectors that nodes of this type hold.
//!
//! \param filename The name of the XML description file to parse.
//! \return True if parsing the file succeeded, otherwise False.
//!
bool NodeType::parseDescriptionFile ( const QString &filename )
{
    QFile file (filename);
    if (!file.exists()) {
        Log::error(QString("XML description file not found: \"%1\"").arg(filename), "NodeType::parseDescriptionFile");
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Log::error(QString("Error loading XML description file \"%1\".").arg(filename), "NodeType::parseDescriptionFile");
        return false;
    }

    // read the content of the file using a simple XML reader
    QDomDocument description;
    QXmlInputSource source (&file);
    QXmlSimpleReader xmlReader;
    QString errorMessage;
    int errorLine = 0;
    int errorColumn = 0;
    if (!description.setContent(&source, &xmlReader, &errorMessage, &errorLine, &errorColumn)) {
        Log::error(QString("Error parsing \"%1\": %2, line %3, char %4").arg(filename).arg(errorMessage).arg(errorLine).arg(errorColumn), "NodeType::parseDescriptionFile");
        return false;
    }

    // decode the full filename into its path and base file name
    QString filePath = file.fileName().mid(0, file.fileName().lastIndexOf('/') + 1);
    QString baseFilename = file.fileName().mid(file.fileName().lastIndexOf('/') + 1);
    Log::debug(QString("Parsing \"%1\"...").arg(baseFilename), "NodeType::parseDescriptionFile");

    // obtain node type information from attributes
    QDomElement rootElement = description.documentElement();
    m_name = rootElement.attribute("name");
    m_categoryName = rootElement.attribute("category");
    QString colorString = rootElement.attribute("color");
    QString pluginFilename = rootElement.attribute("plugin");
    if (m_name.isEmpty() || m_categoryName.isEmpty() || colorString.isEmpty() || pluginFilename.isEmpty()) {
        Log::error(QString("\"%1\": A required attribute in the root node is missing.").arg(filename), "NodeType::parseDescriptionFile");
        return false;
    }

    // set the internal flag if the category name is "Internal"
    m_internal = m_categoryName == "Internal";

    // decode the color
    m_color = Parameter::decodeIntColor(colorString);
    if (!m_color.isValid()) {
        m_color = QColor(0, 0, 0);
        Log::error(QString("\"%1\": The color value for the node type is invalid: \"%2\". Using color Black instead.").arg(filename).arg(colorString), "NodeType::parseDescriptionFile");
        m_erroneous = true;
    }

    // reset the parameter tree
    if (m_parameterRoot)
        delete m_parameterRoot;
    m_parameterRoot = 0;

    // iterate over all child elements of the description XML document
    QDomElement childElement = rootElement.firstChildElement();
    while (!childElement.isNull()) {
        if (childElement.nodeName() == "parameters") {
            if (!m_parameterRoot)
                m_parameterRoot = parseParameters(childElement);
            else {
                Log::error(QString("%1: There should only be one root <parameters> element.").arg(filename), "NodeType::parseDescriptionFile");
                m_erroneous = true;
            }
        } else if (childElement.nodeName() == "affections")
            parseAffections(childElement);

        // go to next element
        childElement = childElement.nextSiblingElement();
    }

    Log::debug(QString("\"%1\" parsed.").arg(baseFilename), "NodeType::parseDescriptionFile");

    // load plugin if a plugin filename is given
    if (pluginFilename != "") {
#ifdef __MINGW32__
        pluginFilename = "lib" + pluginFilename;
#endif
        pluginFilename = filePath + pluginFilename;
#ifdef _DEBUG
        // adjust the plugin filename to load the debug version of the DLL
        pluginFilename = pluginFilename.replace(".dll", "_d.dll");
#endif
        if (QFile::exists(pluginFilename)) {
            Log::debug(QString("Loading plugin \"%1\"...").arg(pluginFilename), "NodeType::parseDescriptionFile");
            QPluginLoader loader (pluginFilename);
            m_nodeTypeInterface = qobject_cast<NodeTypeInterface *>(loader.instance());
            if (!m_nodeTypeInterface) {
                QString debugString = loader.errorString();
                Log::error(QString("Plugin \"%1\" could not be loaded: %2").arg(pluginFilename).arg(loader.errorString()), "NodeType::parseDescriptionFile");
                return false;
            }
        } else {
            Log::error(QString("Plugin file \"%1\" could not be found.").arg(pluginFilename), "NodeType::parseDescriptionFile");
            return false;
        }
    }

    return true;
}


//!
//! Parses the given DOM element and creates the tree of parameters that nodes
//! of this type hold.
//!
//! \param rootElement The DOM element containing a list of <parameter> and >parameters> elements.
//! \return A parameter group containing the created parameters.
//!
ParameterGroup * NodeType::parseParameters ( QDomElement rootElement )
{
    if (rootElement.nodeName() != "parameters") {
        Log::error(QString("Node type \"%1\": Root element \"%2\" is not a <parameters> element.").arg(m_name).arg(rootElement.nodeName()), "NodeType::parseParameters");
        m_erroneous = true;
        return 0;
    }

    // create the parameter group
    ParameterGroup *result = new ParameterGroup(rootElement.attribute("name"));
    result->setEnabled(rootElement.attribute("enabled", "true") != "false");

    // iterate over all child elements
    QDomElement childElement = rootElement.firstChildElement();
    while (!childElement.isNull()) {
        if (childElement.isElement() && childElement.nodeName() == "parameter") {
            // check if a parameter by the element's name already exists
            QString name = childElement.attribute("name");
            if (!result->getParameter(name)) {
                // create a new parameter based on the child element's data
                Parameter *parameter = Parameter::create(childElement);
                if (parameter)
                    result->addParameter(parameter);
                else {
                    Log::error(QString("Node type \"%1\": Parameter \"%2\" could not be created.").arg(m_name).arg(name), "NodeType::parseParameters");
                    m_erroneous = true;
                }
            } else {
                Log::error(QString("Node type \"%1\": Parameter with name \"%2\" already exists.").arg(m_name).arg(name), "NodeType::parseParameters");
                m_erroneous = true;
            }
        } else if (childElement.isElement() && childElement.nodeName() == "parameters") {
            // recursively parse parameters to create nested parameter groups
            result->addParameter(parseParameters(childElement));
        } else {
            Log::error(QString("Node type \"%1\": Parameter with name \"%2\" is neither a <parameter> nor a <parameters> element.").arg(m_name).arg(childElement.localName()), "NodeType::parseParameters");
            m_erroneous = true;
        }

        // go to next element
        childElement = childElement.nextSiblingElement();
    }

    return result;
}


//!
//! Parses the given DOM element and creates the list of forward and backward
//! affections that nodes of this type hold.
//!
//! \param rootElement The DOM element containing a list of <connector> elements.
//!
void NodeType::parseAffections ( QDomElement rootElement )
{
    if (rootElement.nodeName() != "affections") {
        Log::error("Error parsing affections: Root element is not an <affections> element.", "NodeType::parseAffections");
        m_erroneous = true;
        return;
    }

    // iterate over all child elements
    QDomElement childElement = rootElement.firstChildElement();
    while(!childElement.isNull()) {
        QString inputName = childElement.attribute("input");
        QString outputName = childElement.attribute("output");

        m_affectionMap.insert(inputName, outputName);
        //m_backwardAffectionMap.insert(outputName, inputName);

        // go to next element
        childElement = childElement.nextSiblingElement();
    }
}

} // end namespace Frapper
