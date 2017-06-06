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
//! \file "Node.cpp"
//! \brief Implementation file for Node class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       24.03.2010 (last updated)
//!

#include "Node.h"
#include "Log.h"

namespace Frapper {

INIT_INSTANCE_COUNTER(Node)

///
/// Constructors and Destructors
///


//!
//! Constructor of the Node class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
Node::Node ( const QString &name, ParameterGroup *parameterRoot ) :
m_name(name),
m_instanceNumber(s_totalNumberOfInstances),
m_timeParameter(0),
m_rangeParameter(0),
m_typeName(""),
m_typeUnknown(false),
m_evaluate(true),
m_selected(false),
m_selfEvaluating(false),
m_saveable(true),
m_parametersChanged(false),
m_parameterRoot(parameterRoot),
m_searchText("")
{
    if (m_parameterRoot)
        // tell all parameters in the tree that this node is their parent
        m_parameterRoot->setNode(this);

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the Node class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
Node::~Node ()
{
    finalize();
    DEC_INSTANCE_COUNTER
}

void Node::finalize()
{
    if (m_parameterRoot) {
        //QList<Parameter *> parameterList = m_parameterRoot->filterParameters("", true, true);
        //for (int i = 0; i < parameterList.size(); ++i) {
        //    const Parameter *parameter = parameterList[i];
        //    if (parameter->getPinType() == Parameter::PT_Output && parameter->isConnected()) {
        //        QList<Connection *> connectionList = parameter->getConnectionMap().values();
        //        for (int j = 0; j < connectionList.size(); ++j) {
        //            Connection *connection = connectionList[j];
        //            if (!connection)
        //                continue;
        //            Parameter *targetParameter = connection->getTargetParameter();
        //            if (targetParameter)
        //                targetParameter->reset();
        //        }
        //    }
        //}
        delete m_parameterRoot;
    }
}


//!
//! Execute function for threading.
//!
void Node::run ( const QString &parameterName )
{
    // noop
}

///
/// Public Functions
///

//########### obsolete - just for performance testing
//!
//! Processes the node's input data to generate the data for the parameter
//! with the given name.
//!
//! \param parameterName The name of the parameter whose data should be generated.
//! \return True if generating the data succeeded, otherwise False.
//!
bool Node::process ( const QString &parameterName )
{
    return true;
}


bool Node::getTypeUnknown () const
{
    return m_typeUnknown;
}


//!
//! Returns the actual timeline parameter.
//!
//! \return The actual timeline intex.
//!
NumberParameter * Node::getTimeParameter() const
{
    return m_timeParameter;
}

//!
//! Returns the name of the node.
//! The name must be unique in the node network so that each node object can be uniquely identified by its name.
//!
//! \return The name of the node.
//!
const QString &Node::getName () const
{
    return m_name;
}


//!
//! Sets the name of the node.
//! The name must be unique in the node network so that each node object can be uniquely identified by its name.
//!
//! \param name The new name for the node.
//!
void Node::setName ( const QString &name )
{
    if (m_name != name) {
        QString oldName = m_name;
        m_name = name;

        // notify connected objects that the node has changed
        emit nodeChanged();
        emit nodeNameChanged(oldName);
    }
}


//!
//! Returns the name of the node's type.
//!
//! \return The name of the node's type.
//!
const QString &Node::getTypeName () const
{
    return m_typeName;
}


//!
//! Set the name of the node's type.
//!
//! This function should normally only be called once directly after the
//! node has been created in the NodeFactory::createNode function.
//!
//! \param typeName The name of the node's type.
//! \see NodeFactory::createNode
//!
void Node::setTypeName ( const QString &typeName )
{
    m_typeName = typeName;
}


//!
//! Returns whether a plugin for the node's type is available.
//!
//! \return True if a plugin for the node's type is available, otherwise False.
//!
bool Node::isTypeUnknown () const
{
    return m_typeUnknown;
}


//!
//! Sets the flag that indicates whether a plugin for the node's type is
//! available.
//!
//! \param typeUnknown Flag to indicate whether a plugin for the node's type is available.
//!
void Node::setTypeUnknown ( bool typeUnknown /* = true */ )
{
    m_typeUnknown = typeUnknown;
}


//!
//! Returns whether this node should be evaluated in the network.
//!
//! \return True if this node should be evaluated in the network, otherwise False.
//!
bool Node::isEvaluated () const
{
    return m_evaluate;
}


//!
//! Returns whether this node is currently selected.
//!
//! \return True if this node is currently selected, otherwise False.
//!
bool Node::isSelected () const
{
    return m_selected;
}


//!
//! Sets whether this node is currently selected.
//!
//! \param The new selected value for the node.
//!
void Node::setSelected ( bool selected )
{
    if (m_selected != selected) {
        m_selected = selected;

        // notify connected objects that the selected state has changed
        emit selectedChanged(m_selected);
    }
}


//!
//! Returns whether the node is self-evaluating.
//!
//! \return True if the node is self-evaluating, otherwise false.
//!
bool Node::isSelfEvaluating () const
{
    return m_selfEvaluating;
}


//!
//! Returns whether the node is saveable.
//!
//! \return True if the node is saveable, otherwise false.
//!
bool Node::isSaveable () const
{
    return m_saveable;
}


//!
//! Sets whether the node is self-evaluating.
//!
//! \param selfEvaluating The new value for the self-evaluating flag.
//!
void Node::setSelfEvaluating ( bool selfEvaluating )
{
    if (m_selfEvaluating != selfEvaluating) {
        m_selfEvaluating = selfEvaluating;

        // notify connected objects that the node has changed
        emit nodeChanged();
    }
}


//!
//! Sets whether the node is saveable.
//!
//! \param saveable The new value for the saveable flag.
//!
void Node::setSaveable ( bool saveable )
{
    m_saveable = saveable;
}


//!
//! Returns the search text currently set for the node.
//!
//! \return The search text currently set for the node.
//!
const QString &Node::getSearchText () const
{
    return m_searchText;
}


//!
//! Sets the text to use when filtering parameters of the node before
//! creating editing widgets in a parameter editor panel.
//!
//! \param searchText The text to use for filtering parameters.
//!
void Node::setSearchText ( const QString &searchText )
{
    m_searchText = searchText;
}


///
/// Public Parameter Group Functions
///


//!
//! Returns the root of the tree of parameters and parameter groups for the
//! node.
//!
//! \return The root of the tree of parameters for the node.
//!
ParameterGroup * Node::getParameterRoot () const
{
    return m_parameterRoot;
}


//!
//! Returns the parameter group with the given name.
//!
//! \param name The name of the parameter group to return.
//! \return The parameter group with the given name.
//!
ParameterGroup * Node::getParameterGroup ( const QString &name ) const
{
    if (m_parameterRoot)
        return m_parameterRoot->getParameterGroup(name);
    else
        return 0;
}


//!
//! Iterates over the children of the parameter root and fills the given
//! lists with input and output parameters and parameter groups containing
//! input and output parameters.
//!
//! \param inputParameters The list to fill with input parameters and parameter groups containing input parameters.
//! \param outputParameters The list to fill with output parameters and parameter groups containing output parameters.
//!
void Node::fillParameterLists ( AbstractParameter::List *inputParameters, AbstractParameter::List *outputParameters )
{
    const AbstractParameter::List& parameterList = m_parameterRoot->getParameterList();
    for (int i = 0; i < parameterList.size(); ++i)
        if (parameterList.at(i)->isGroup()) {
            ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(parameterList.at(i));
            if (parameterGroup->contains(Parameter::PT_Input))
                inputParameters->append(parameterGroup);
            else if (parameterGroup->contains(Parameter::PT_Output))
                outputParameters->append(parameterGroup);
        } else {
            Parameter *parameter = dynamic_cast<Parameter *>(parameterList.at(i));
            if (parameter->getPinType() == Parameter::PT_Input)
                inputParameters->append(parameter);
            else if (parameter->getPinType() == Parameter::PT_Output)
                outputParameters->append(parameter);
        }
}


//!
//! Iterates over the children of the parameter root and fills the given
//! lists with connected input and output parameters and parameter groups
//! containing connected input and output parameters.
//!
//! \param connectedInputParameters The list to fill with connected input parameters and parameter groups containing connected input parameters.
//! \param connectedOutputParameters The list to fill with connected output parameters and parameter groups containing connected output parameters.
//! \param unconnectedInputParametersAvailable Flag that returns whether there are unconnected input parameters contained in the node.
//! \param unconnectedOutputParametersAvailable Flag that returns whether there are unconnected output parameters contained in the node.
//!
void Node::fillParameterLists ( AbstractParameter::List *connectedInputParameters, AbstractParameter::List *connectedOutputParameters, bool *unconnectedInputParametersAvailable, bool *unconnectedOutputParametersAvailable )
{
    *unconnectedInputParametersAvailable = false;
    *unconnectedOutputParametersAvailable = false;
    const AbstractParameter::List& parameterList = m_parameterRoot->getParameterList();
    for (int i = 0; i < parameterList.size(); ++i)
        if (parameterList.at(i)->isGroup()) {
            ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(parameterList.at(i));
            if (parameterGroup->contains(Parameter::PT_Input, true))
                connectedInputParameters->append(parameterGroup);
            else if (parameterGroup->contains(Parameter::PT_Output, true))
                connectedOutputParameters->append(parameterGroup);
            if (!*unconnectedInputParametersAvailable && parameterGroup->contains(Parameter::PT_Input, false))
                *unconnectedInputParametersAvailable = true;
            if (!*unconnectedOutputParametersAvailable && parameterGroup->contains(Parameter::PT_Output, false))
                *unconnectedOutputParametersAvailable = true;
        } else {
            Parameter *parameter = dynamic_cast<Parameter *>(parameterList.at(i));
            if (parameter->getPinType() == Parameter::PT_Input) {
                if (parameter->isConnected())
                    connectedInputParameters->append(parameter);
                else
                    if (!*unconnectedInputParametersAvailable)
                        *unconnectedInputParametersAvailable = true;
            } else if (parameter->getPinType() == Parameter::PT_Output)
                if (parameter->isConnected())
                    connectedOutputParameters->append(parameter);
                else
                    if (!*unconnectedOutputParametersAvailable)
                        *unconnectedOutputParametersAvailable = true;
        }
}


//!
//! Returns a list of parameters contained in the given parameter group
//! that are of the given pin type. If the respective flag is set only
//! connected parameters will be returned.
//!
//! \param pinType Only parameters of this pin type will be returned.
//! \param connectedOnly Flag that controls whether only connected parameters should be returned.
//! \param parameterGroup The root of the parameter tree to get parameters from.
//! \return A list of (connected) parameters of the given pin type contained in the given parameter group.
//!
Parameter::List Node::getParameters ( Parameter::PinType pinType, bool connectedOnly /* = false */, ParameterGroup *parameterGroup /* = 0 */ )
{
    if (parameterGroup == 0)
        parameterGroup = m_parameterRoot;

    Parameter::List result;

    const AbstractParameter::List& parameterList = parameterGroup->getParameterList();
    for (int i = 0; i < parameterList.size(); ++i) {
        AbstractParameter *abstractParameter = parameterList.at(i);
        if (abstractParameter->isGroup()) {
            // recursively get the parameters from the current parameter group
            Parameter::List parameterList = getParameters(pinType, connectedOnly, dynamic_cast<ParameterGroup *>(abstractParameter));
            // merge the obtained list with the result of this function
            for (int n = 0; n < parameterList.size(); ++n)
                result.append(parameterList[n]);
        } else {
            Parameter *parameter = dynamic_cast<Parameter *>(abstractParameter);
            if (parameter->getPinType() == pinType && (!connectedOnly || parameter->isConnected()))
                result.append(parameter);
        }
    }

    return result;
}


///
/// Public Parameter Functions
///


//!
//! Returns whether the node contains a parameter with the given name.
//!
//! \param name The name of the parameter to check for existence.
//! \return True if a parameter with the given name exists in the node, otherwise False.
//!
bool Node::hasParameter ( const QString &name ) const
{
    return m_parameterRoot->contains(name);
}


//!
//! Returns the parameter with the given name.
//!
//! \param name The name of the parameter to return.
//! \return The parameter with the given name.
//!
Parameter * Node::getParameter ( const QString &name ) const
{
    return m_parameterRoot->getParameter(name);
}


//!
//! Returns the generic parameter with the given name.
//!
//! Convenience function that uses getParameter and performs type casting.
//!
//! \param name The name of the generic parameter to return.
//! \return The generic parameter with the given name.
//!
GenericParameter * Node::getGenericParameter( const QString &name ) const
{
	Parameter *parameter = getParameter(name);
	if (parameter)
		return dynamic_cast<GenericParameter *>(parameter);
	else
		return 0;
}

//!
//! Returns the number parameter with the given name.
//!
//! Convenience function that uses getParameter and performs type casting.
//!
//! \param name The name of the number parameter to return.
//! \return The number parameter with the given name.
//!
NumberParameter * Node::getNumberParameter ( const QString &name ) const
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        return dynamic_cast<NumberParameter *>(parameter);
    else
        return 0;
}


//!
//! Returns the filename parameter with the given name.
//!
//! Convenience function that uses getParameter and performs type casting.
//!
//! \param name The name of the filename parameter to return.
//! \return The filename parameter with the given name.
//!
FilenameParameter * Node::getFilenameParameter ( const QString &name ) const
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        return dynamic_cast<FilenameParameter *>(parameter);
    else
        return 0;
}


//!
//! Returns the enumeration parameter with the given name.
//!
//! Convenience function that uses getParameter and performs type casting.
//!
//! \param name The name of the enumeration parameter to return.
//! \return The enumeration parameter with the given name.
//!
EnumerationParameter * Node::getEnumerationParameter ( const QString &name ) const
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        return dynamic_cast<EnumerationParameter *>(parameter);
    else
        return 0;
}


//!
//! Returns the geometry parameter with the given name.
//!
//! Convenience function that uses getParameter and performs type casting.
//!
//! \param name The name of the geometry parameter to return.
//! \return The geometry parameter with the given name.
//!
GeometryParameter * Node::getGeometryParameter ( const QString &name ) const
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        return dynamic_cast<GeometryParameter *>(parameter);
    else
        return 0;
}


//!
//! Returns the light parameter with the given name.
//!
//! Convenience function that uses getParameter and performs type casting.
//!
//! \param name The name of the light parameter to return.
//! \return The light parameter with the given name.
//!
LightParameter * Node::getLightParameter ( const QString &name ) const
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        return dynamic_cast<LightParameter *>(parameter);
    else
        return 0;
}


//!
//! Returns the camera parameter with the given name.
//!
//! Convenience function that uses getParameter and performs type casting.
//!
//! \param name The name of the camera parameter to return.
//! \return The camera parameter with the given name.
//!
CameraParameter * Node::getCameraParameter ( const QString &name ) const
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        return dynamic_cast<CameraParameter *>(parameter);
    else
        return 0;
}


//!
//! Sets the dirty state of the parameter with the given name to the given
//! state.
//!
//! \param name The name of the parameter to set the dirty state for.
//! \param dirty The new value for the parameter's dirty state.
//!
void Node::setDirty ( const QString &name, bool dirty )
{
    Parameter *parameter = getParameter(name);
    if (parameter) {
        parameter->setDirty(dirty);
        m_dirty = dirty;
    }
    else
        Log::error(QString("A parameter named \"%1\" could not be found.").arg(name), "Node::setDirty");
}

//!
//! Sets the dirty state of the node and all child parameters.
//!
//! \param dirty The new value for the node's dirty state.
//!
void Node::setDirty ( bool dirty )
{
    Parameter::List parameterList = getParameters(Parameter::PT_Output, true, getParameterRoot());
    for (int i = 0; i < parameterList.size(); ++i) {
        Parameter *parameter = (Parameter *) parameterList[i];
        parameter->setDirty(dirty);
    }
    setNodeDirty(dirty);
}

//!
//! Sets the dirty state of the node.
//!
//! \param dirty The new value for the node's dirty state.
//!
void Node::setNodeDirty ( bool dirty )
{
    m_dirty = dirty;
}

//!
//! Returns the dirty state of the node.
//!
//! \return The value for the node's dirty state.
//!
bool Node::isNodeDirty ()
{
    return m_dirty;
}


//!
//! Removes and deletes the parameter with the given name.
//!
//! \param name The name of the parameter to remove and delete.
//!
void Node::removeParameter ( const QString &name, bool diveInGroups /*= true*/ )
{
    m_parameterRoot->removeParameter(name, diveInGroups);
}


//!
//! Removes and deletes the given parameter.
//!
//! \param parameter The parameter to remove and delete.
//!
void Node::removeParameter ( Parameter *parameter, bool diveInGroups /*= true*/ )
{
    m_parameterRoot->removeParameter(parameter, diveInGroups);
}


//!
//! Adds a affection between two parameters.
//!
void Node::addAffection ( const QString &parameterName, const QString &affectedParameterName )
{
    Parameter *parameter = getParameter(parameterName);
    if(!parameter) {
        Log::warning(QString("Could not obtain parameter \"%1\" on node \"%2\".").arg(parameterName, m_name), "Node::addAffection");
        return;
    }
    Parameter *affectedParameter = getParameter(affectedParameterName);
    if(!affectedParameter) {
        Log::warning(QString("Could not obtain parameter \"%1\" on node \"%2\".").arg(affectedParameterName, m_name), "Node::addAffection");
        return;
    }
    // set up parameter affections
    affectedParameter->addAffectingParameter(parameter);
}


///
/// Public Value Getter Functions
///


//!
//! Returns the value of the parameter with the given name while optionally
//! triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
QVariant Node::getValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
    Parameter *parameter = getParameter(name);
    if (parameter)
		return parameter->getValue(triggerEvaluation);
	else
        return QVariant();
}


//!
//! Convenience function for getting the value of a boolean parameter while
//! optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
bool Node::getBoolValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
	return getValue(name, triggerEvaluation).toBool();
}


//!
//! Convenience function for getting the value of an integer parameter
//! while optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
int Node::getIntValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
	return getValue(name, triggerEvaluation).toInt();
}


//!
//! Convenience function for getting the value of an unsigned integer
//! parameter while optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
unsigned int Node::getUnsignedIntValue ( const QString &name, bool triggerEvaluation /* = false */ )
{
	return getValue(name, triggerEvaluation).toUInt();
}


//!
//! Convenience function for getting the value of a 32bit-precision
//! floating point parameter while optionally triggering the evaluation
//! chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
float Node::getFloatValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
	return getValue(name, triggerEvaluation).toFloat();
}


//!
//! Convenience function for getting the value of a double-precision
//! floating point parameter while optionally triggering the evaluation
//! chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
double Node::getDoubleValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
	return getValue(name, triggerEvaluation).toDouble();
}


//!
//! Convenience function for getting the value of a string parameter while
//! optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
QString Node::getStringValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
    QVariant value = getValue(name, triggerEvaluation);
	if (value.canConvert<QString>())
        return value.toString();
    else {
        Log::warning(QString("Parameter \"%1\" does not contain a string value, but a value of type <%2>.").arg(name, QVariant::typeToName(value.type())), "Node::getStringValue");
        return QString("");
    }
}


//!
//! Convenience function for getting the value of a color parameter while
//! optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
QColor Node::getColorValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
    QVariant value = getValue(name, triggerEvaluation);
    if (value.canConvert<QColor>())
        return value.value<QColor>();
    else {
        Log::warning(QString("Could not convert value of parameter \"%1\" to a color.").arg(name), "Node::getColorValue");
        return QColor(Qt::black);
    }
}


//!
//! Convenience function for getting the value of an OGRE vector parameter
//! while optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
Ogre::Vector3 Node::getVectorValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
    QVariant value = getValue(name, triggerEvaluation);
    if (value.canConvert<Ogre::Vector3>())
        return value.value<Ogre::Vector3>();
    else {
        Log::warning(QString("Could not convert value of parameter \"%1\" to an Ogre::Vector3.").arg(name), "Node::getVectorValue");
        return Ogre::Vector3();
    }
}


//!
//! Convenience function for getting the value of a scene node parameter
//! while optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
Ogre::SceneNode * Node::getSceneNodeValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
    QVariant value = getValue(name, triggerEvaluation);
    if (value.canConvert<Ogre::SceneNode *>())
        return value.value<Ogre::SceneNode *>();
    else {
        Log::warning(QString("Could not convert value of parameter \"%1\" to an Ogre::SceneNode pointer.").arg(name), "Node::getSceneNodeValue");
        return 0;
    }
}


//!
//! Convenience function for getting the value of an OGRE texture parameter
//! while optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
Ogre::TexturePtr Node::getTextureValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
    QVariant value = getValue(name, triggerEvaluation);
    if (value.canConvert<Ogre::TexturePtr>())
        return value.value<Ogre::TexturePtr>();
    else {
        Log::warning(QString("Could not convert value of parameter \"%1\" to an Ogre::TexturePtr.").arg(name), "Node::getTextureValue");
        return Ogre::TexturePtr();
    }
}


//!
//! Convenience function for getting the value of a parameter group
//! parameter while optionally triggering the evaluation chain.
//!
//! \param name The name of the parameter whose value to return.
//! \param triggerEvaluation Flag to control whether to trigger the evaluation chain.
//! \return The value of the parameter with the given name.
//!
ParameterGroup * Node::getGroupValue ( const QString &name, bool triggerEvaluation /* = false */ ) const
{
    QVariant value = getValue(name, triggerEvaluation);
    if (value.canConvert<ParameterGroup *>())
        return value.value<ParameterGroup *>();
    else {
        Log::warning(QString("Could not convert value of parameter \"%1\" to a ParameterGroup pointer.").arg(name), "Node::getGroupValue");
        return 0;
    }
}


///
/// Public Value Setter Functions
///


//!
//! Sets the value of the parameter with the given name to the given value
//! while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The new value for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, const QVariant &value, bool triggerDirtying /* = false */ )
{
    m_parameterRoot->setValue(name, value, triggerDirtying);
}


//!
//! Convenience function for setting the value of a boolean parameter
//! while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, bool value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of an integer parameter
//! while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, int value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of an unsigned integer
//! parameter while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, unsigned int value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of a double-precision
//! floating point parameter while optionally triggering the dirtying
//! chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, float value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of a double-precision
//! floating point parameter while optionally triggering the dirtying
//! chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, double value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of a string parameter while
//! optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, char *value, bool triggerDirtying /* = false */ )
{
    setValue(name, QString(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of a string parameter while
//! optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, const char *value, bool triggerDirtying /* = false */ )
{
    setValue(name, QString(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of a string parameter while
//! optionally triggering the dirtying chain.
//!
//! If the parameter with the given name is not a string parameter the
//! given value will be converted according to the parameter's type.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, const QString &value, bool triggerDirtying /* = false */)
{
    m_parameterRoot->setValue(name, value, triggerDirtying);
}


//!
//! Convenience function for setting the value of a color parameter while
//! optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, const QColor &value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of an OGRE vector parameter
//! while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, const Ogre::Vector3 &value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant::fromValue<Ogre::Vector3>(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of a scene node parameter
//! while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, Ogre::SceneNode *value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant::fromValue<Ogre::SceneNode *>(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of an OGRE texture parameter
//! while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, const Ogre::TexturePtr value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant::fromValue<Ogre::TexturePtr>(value), triggerDirtying);
}


//!
//! Convenience function for setting the value of a parameter group
//! parameter while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void Node::setValue ( const QString &name, ParameterGroup *value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant::fromValue<ParameterGroup *>(value), triggerDirtying);
}


//!
//! Convenience function for setting the enabled flag of a string parameter
//!
//! \param name The name of the parameter to set the value for.
//! \param enabled The enabled flag to set for the parameter with the given name.
//!
void Node::setParameterEnabled ( const QString &name, bool enabled )
{
    m_parameterRoot->setParameterEnabled(name, enabled);
}

///
/// Public Callback Setter Functions
///


//!
//! Sets the function to call when a parameter's value changes.
//!
//! Convenience function that calls setChangeFunction for the parameter
//! with the given name.
//!
//! \param name The name of the parameter to set the change function for.
//! \param changeFunction The name of the function to call when the parameter's value changes.
//!
void Node::setChangeFunction ( const QString &name, const char *changeFunction )
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        parameter->setChangeFunction(changeFunction);
    else
        Log::warning(QString("Could not set change function for parameter \"%1\": Parameter could not be found.").arg(name), "Node::setChangeFunction");
}


//!
//! Sets the function to use for computing a parameter's value.
//!
//! Convenience function that calls setProcessingFunction for the parameter
//! with the given name.
//!
//! \param name The name of the parameter to set the processing function for.
//! \param processingFunction The name of the function to use for computing the parameter's value.
//!
void Node::setProcessingFunction ( const QString &name, const char *processingFunction )
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        parameter->setProcessingFunction(processingFunction);
    else
        Log::warning(QString("Could not set processing function for parameter \"%1\": Parameter could not be found.").arg(name), "Node::setProcessingFunction");
}


//!
//! Sets the auxiliary function to use for computing a parameter's value.
//!
//! Convenience function that calls setProcessingFunction for the parameter
//! with the given name.
//!
//! \param name The name of the parameter to set the processing function for.
//! \param processingFunction The name of the function to use for computing the parameter's value.
//!
void Node::setAuxProcessingFunction ( const QString &name, const char *processingFunction )
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        parameter->setAuxProcessingFunction(processingFunction);
    else
        Log::warning(QString("Could not set aux processing function for parameter \"%1\": Parameter could not be found.").arg(name), "Node::setAuxProcessingFunction");
}


//!
//! Sets the function to use for executing the command represented by a
//! parameter.
//!
//! Convenience function that calls setCommandFunction for the parameter
//! with the given name.
//!
//! \param name The name of the parameter to set the command function for.
//! \param commandFunction The function to use for executing the command represented by the parameter.
//!
void Node::setCommandFunction ( const QString &name, const char *commandFunction )
{
    Parameter *parameter = getParameter(name);
    if (parameter)
        parameter->setCommandFunction(commandFunction);
    else
        Log::warning(QString("Could not set command function for parameter \"%1\": Parameter could not be found.").arg(name), "Node::setCommandFunction");
}

//!
//! Sets the function which should be called on parameter's connection.
//!
//!
//! \parem name The name of the parameter which should be called on parameter's connection.
//! \param onCreateConnectFunction The function called on parameter's connection.
//!
void Node::setOnCreateConnectionFunction ( const QString &name, const char *onCreateConnectFunction )
{
	Parameter *parameter = getParameter(name);
    if (parameter) {
		parameter->setOnCreateConnectionFunction(onCreateConnectFunction);
    }
	else
		Log::warning(QString("Could not set onCreateConnectFunction function for parameter \"%1\": Parameter could not be found.").arg(m_name), "Node::setOnCreateConnectionFunction");
}

//!
//! Sets the function which should be called on parameter's connection.
//!
//!
//! \parem name The name of the parameter which should be called on parameter's connection.
//! \param onConnectFunction The function called on parameter's connection.
//!
void Node::setOnConnectFunction ( const QString &name, const char *onConnectFunction )
{
    Parameter *parameter = getParameter(name);
    if (parameter) {
		parameter->setOnConnectFunction(onConnectFunction);
    }
	else
		Log::warning(QString("Could not set onConnectFunction function for parameter \"%1\": Parameter could not be found.").arg(m_name), "Node::setOnConnectFunction");
}


//!
//! Sets the function which should be called on parameter's disconnection.
//!
//!
//! \parem name The name of the parameter which should be called on parameter's connection.
//! \param onDisconnectFunction The function called on parameter's connection.
//!
void Node::setOnDisconnectFunction ( const QString &name, const char *onDisconnectFunction )
{
    Parameter *parameter = getParameter(name);
    if (parameter) {
		parameter->setOnDisconnectFunction(onDisconnectFunction);
    }
	else
		Log::warning(QString("Could not set OnDisconnect function for parameter \"%1\": Parameter could not be found.").arg(m_name), "Node::setOnDisconnectFunction");
}


///
/// Public Misc Functions
///


//!
//! Notify when something has changed in the node.
//! (Parameters added/deleted)
//!
void Node::notifyChange ()
{
    emit nodeChanged();
}


//!
//! Sets up time dependencies for node parameters.
//!
//! Can be implemented in derived classes to set up parameter affections
//! from the given time parameter to specific parameters of the node that
//! should be evaluated when the scene time represented by the given time
//! parameter changes.
//!
//! The default implementation does nothing.
//!
//! \param timeParameter The parameter representing the current scene time.
//! \param slot The slot which should be triggered when frame has changed
//!
void Node::setUpTimeDependencies( NumberParameter *timeParameter, NumberParameter *rangeParameter )
{
    m_timeParameter = timeParameter;
    m_rangeParameter = rangeParameter;
}


//!
//! Evaluates the given new connection for the node.
//!
//! \param connection The new connection to evaluate.
//!
void Node::evaluateConnection ( Connection *connection )
{
    //
}

//!
//! Signal that is emitted in order to delete a connection (through SceneModel)
//!
//! \param name The name of the object.
//!
void Node::deleteConnection ( Connection *connection)
{
    emit sendDeleteConnection(connection);
}

//!
//! Render a single frame.
//!
void Node::renderSingleFrame ()
{
    emit updateFrame();
}

//!
//! Forces all panels to reinit.
//!
void Node::forcePanelUpdate ()
{
	m_parametersChanged = true;
	emit selectDeselectObject(m_name);
}

//!
//! Returns true if the nodes internal structure has changed.
//!
bool Node::parametersChanged ()
{
	const bool returnvalue = m_parametersChanged;
	m_parametersChanged = false;
	return returnvalue;
}


///
/// Public Slots
///


//!
//! Sets whether this node should be evaluated in the network.
//!
//! \param evaluate The new value for eval flag of this node.
//!
void Node::setEvaluate ( bool evaluate )
{
    if (m_evaluate != evaluate) {
        m_evaluate = evaluate;

        // notify connected objects that the node has changed
        emit nodeChanged();
    }
}

//!
//! Executed when scene loading is ready.
//!
void Node::loadReady()
{
    //
}

//!
//! Executed when scene saving begins.
//!
void Node::saveStarted()
{
    //
}

//!
//! Executed when scene saving has finished.
//!
void Node::saveFinished()
{
	//
}

//!
//! Executed when scene loading is begins.
//!
void Node::loadStarted()
{
	//
}

//!
//! Executed when the scene elements loading is ready.
//!
void Node::loadSceneElementsReady()
{
	//
}

//!
//! Execute before redraw is triggered.
//!
void Node::onPreRedraw()
{
	//
}


///
/// Protected Methods
///

//!
//! Creates a unique name from the given object name by adding the Node
//! number.
//!
Ogre::String Node::createUniqueName ( const QString &objectName )
{
    return QString("%1_%2").arg(m_instanceNumber).arg(objectName).toStdString();
}

} // end namespace Frapper