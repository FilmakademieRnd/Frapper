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
//! \file "ParameterGroup.cpp"
//! \brief Implementation file for ParameterGroup class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       07.04.2010 (last updated)
//!

#include "ParameterGroup.h"
#include "Node.h"
#include "Log.h"

namespace Frapper {

INIT_INSTANCE_COUNTER(ParameterGroup)

///
/// Constructors and Destructors
///


//!
//! Constructor of the ParameterGroup class.
//!
//! \param name The name of the parameter group.
//!
ParameterGroup::ParameterGroup ( const QString &name /* = "" */ ) :
    AbstractParameter(name)   
{
    INC_INSTANCE_COUNTER
}

//!
//! Copy Constructor of the ParameterGroup class.
//!
//! \param parameterGroup The parameter group to copy.
//!
ParameterGroup::ParameterGroup ( const ParameterGroup &parameterGroup, Node* node /*= 0*/ ) :
	AbstractParameter(parameterGroup, node)
{
	INC_INSTANCE_COUNTER

	QList<AbstractParameter*> list = parameterGroup.m_parameterList;
	foreach(AbstractParameter* param, list) {
		this->setNode(node);
		this->addParameter(param->clone());
	}
}

//!
//! Destructor of the ParameterGroup class.
//!
ParameterGroup::~ParameterGroup ()
{
	destroyAllParameters();

    DEC_INSTANCE_COUNTER
}


///
/// Public Functions
///


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
void ParameterGroup::setValue ( const QString &name, const QVariant &value, const bool triggerDirtying /* = false */ )
{
    QString parameterName = name;

    // check if the parameter name ends with an index into value list
    static const QRegExp indexExpression ("\\[(\\d+)\\]$");
    int index = -1;
    if (indexExpression.indexIn(name) > -1) {
        index = indexExpression.cap(1).toInt();
        parameterName = name.left(name.indexOf('['));
    }

    // obtain the parameter with the given name
    Parameter *parameter = getParameter(parameterName);
    if (parameter) {
        // set the value for the parameter
        if (index == -1)
            parameter->setValue(value, triggerDirtying);
        else
            parameter->setValue(index, value, triggerDirtying);

	} else {
		QString nodeName = "NO_NODE";
		if (m_node)
			nodeName = m_node->getName();
        Log::error(QString("Parameter \"%1.%2\" not found.").arg(nodeName, name), "ParameterGroup::setValue");
	}
}


//!
//! Convenience function for setting the value of a boolean parameter
//! while optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void ParameterGroup::setValue ( const QString &name, const bool value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, const int value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, const unsigned int value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, const double value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, const char *value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, const QString &value, const bool triggerDirtying /* = false */)
{
    // obtain the parameter with the given name
    Parameter *parameter = getParameter(name);

	// check if the parameter exists
	if (!parameter) {
		// create the parameter if the intended type for the node is unsupported
		if (m_node->getTypeUnknown()) {
			Parameter *parameter = new Parameter(name, Parameter::T_String, value);
			parameter->setDefaultValue("");     // to make sure the parameter is considered when saving the scene
			parameter->setReadOnly(true);
			addParameter(parameter);
			QString nodeName = "NO_NODE";
			if (m_node)
				nodeName = m_node->getName();
			Log::debug(QString("The \"%1\" parameter has been automatically added to the node \"%2\".").arg(name, nodeName), "ParameterGroup::setValue");
		} else {
			QString nodeName = m_node ? m_node->getName() : "NO_NODE";
			Log::error(QString("Parameter \"%1.%2\" not found.").arg(nodeName, name), "ParameterGroup::setValue");
		}
		return;
	}

    // find out if the value contains a separator
    QString separator;
    if (value.contains(", "))
        separator = ", ";
    else if (value.contains(" "))
        separator = " ";

    // check the type of the parameter and try converting the given value according to the parameter's type if it's not a string-based type
    switch (parameter->getType()) {

        // character string-based parameters
        case Parameter::T_String:
        case Parameter::T_Filename:
        case Parameter::T_TextInfo:
            setValue(name, QVariant(value), triggerDirtying);
            break;

		// generic parameter: set value only if not in or out parameter
		case Parameter::T_Generic:
			if( parameter->getPinType() == Parameter::PT_None )
				setValue(name, QVariant(value), triggerDirtying);
			break;

        // boolean parameters
        case Parameter::T_Bool:
            setValue(name, value == "true", triggerDirtying);
            break;

        // integer number parameters
        case Parameter::T_Int:
            if (separator.isEmpty()) {
                // convert a single value
                bool ok = true;
                int intValue = value.toInt(&ok);
                if (ok)
                    setValue(name, intValue, triggerDirtying);
                else
                    Log::error(QString("The value \"%1\" for parameter \"%2\" could not be converted to an integer number.").arg(value, name), "ParameterGroup::setValue");
            } else {
                // convert a list of values
                QStringList valueParts = value.split(separator);
                bool ok = true;
                for (int i = 0; i < valueParts.size(); ++i) {
                    int intValue = valueParts[i].toInt(&ok);
                    if (ok)
                        setValue(QString("%1[%2]").arg(name).arg(i), intValue, triggerDirtying);
                    else
                        Log::error(QString("The value part \"%1\" for parameter \"%2\" could not be converted to an integer number.").arg(valueParts[i], name), "ParameterGroup::setValue");
                }
            }
            break;

        // unsigned integer number parameters
        case Parameter::T_UnsignedInt:
            if (separator.isEmpty()) {
                // convert a single value
                bool ok = true;
                unsigned int unsignedIntValue = value.toUInt(&ok);
                if (ok)
                    setValue(name, unsignedIntValue, triggerDirtying);
                else
                    Log::error(QString("The value \"%1\" for parameter \"%2\" could not be converted to an unsigned integer number.").arg(value, name), "ParameterGroup::setValue");
            } else {
                // convert a list of values
                QStringList valueParts = value.split(separator);
                bool ok = true;
                for (int i = 0; i < valueParts.size(); ++i) {
                    unsigned int unsignedIntValue = valueParts[i].toUInt(&ok);
                    if (ok)
                        setValue(QString("%1[%2]").arg(name).arg(i), unsignedIntValue, triggerDirtying);
                    else
                        Log::error(QString("The value part \"%1\" for parameter \"%2\" could not be converted to an unsigned integer number.").arg(valueParts[i], name), "ParameterGroup::setValue");
                }
            }
            break;

        // floating-point number parameters
		case Parameter::T_Float:
            if (separator.isEmpty()) {
                // convert a single value
                bool ok = true;
                double doubleValue = value.toDouble(&ok);
                if (ok)
                    setValue(name, doubleValue, triggerDirtying);
                else
                    Log::error(QString("The value \"%1\" for parameter \"%2\" could not be converted to a double-precision floating-point number.").arg(value, name), "ParameterGroup::setValue");
            } else {
                // convert a list of values
                QStringList valueParts = value.split(separator);
                bool ok = true;
                for (int i = 0; i < valueParts.size(); ++i) {
                    double doubleValue = valueParts[i].toDouble(&ok);
                    if (ok)
                        setValue(QString("%1[%2]").arg(name).arg(i), doubleValue, triggerDirtying);
                    else
                        Log::error(QString("The value part \"%1\" for parameter \"%2\" could not be converted to a double-precision floating-point number.").arg(valueParts[i], name), "ParameterGroup::setValue");
                }
            }
            break;

        // color parameters
        case Parameter::T_Color:
			if (!separator.isEmpty()) {
				const QStringList colorSrings = value.split(separator);
				if (colorSrings.size() == 4) {
					bool ok = true;
					const int r = colorSrings[0].toInt(&ok);
					const int g = colorSrings[1].toInt(&ok);
					const int b = colorSrings[2].toInt(&ok);
					const int a = colorSrings[3].toInt(&ok);
					if (ok)
						setValue(name, QColor(r, g, b, a), triggerDirtying);
				}
			}
            break;
        // enumeration parameters
        case Parameter::T_Enumeration:
            setValue(name, value.toInt(), triggerDirtying);
            break;

        // unsupported parameters
        default:
            Log::warning(QString("Parameter \"%1\" is not a string parameter. The value \"%2\" should be converted to a <%3>.").arg(name, value).arg(Parameter::getTypeName(parameter->getType())), "ParameterGroup::setValue");
    }
}


//!
//! Convenience function for setting the value of a color parameter while
//! optionally triggering the dirtying chain.
//!
//! \param name The name of the parameter to set the value for.
//! \param value The value to set for the parameter with the given name.
//! \param triggerDirtying Flag to control whether to trigger the dirtying chain.
//!
void ParameterGroup::setValue ( const QString &name, const QColor &value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, const Ogre::Vector3 &value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, Ogre::SceneNode *value, const bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, Ogre::TexturePtr value, bool triggerDirtying /* = false */ )
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
void ParameterGroup::setValue ( const QString &name, ParameterGroup *value, bool triggerDirtying /* = false */ )
{
    setValue(name, QVariant::fromValue<ParameterGroup *>(value), triggerDirtying);
}


//!
//! Convenience function for setting the enabled flag of a string parameter
//!
//! \param name The name of the parameter to set the value for.
//! \param enabled The enabled flag to set for the parameter with the given name.
//!
void ParameterGroup::setParameterEnabled ( const QString &name, const bool enabled )
{
    // obtain the parameter with the given name
    Parameter *parameter = getParameter(name);

	if (parameter)
		parameter->setEnabled(enabled);
	else {
		QString nodeName = "NO_NODE";
		if (m_node)
			nodeName = m_node->getName();
		Log::error(QString("Parameter \"%1.%2\" not found.").arg(nodeName, name), "Node::setParameterEnabled");
	}
}

//! Sets whether the controls for editing the parameter group in the UI
//! should be enabled.
//!
//! Recursively sets the enabled state for all parameters and parameter
//! groups contained in this group.
//!
//! \param enabled The value for the parameters' enabled flags.
//! \propagate Flag to control whether to emit the enabledChanched signal.
//!
void ParameterGroup::setEnabled ( const bool enabled, const bool propagate /*= true*/ )
{
	setEnabled(enabled, propagate, true);
}


//!
//! Sets whether the controls for editing the parameter group in the UI
//! should be enabled.
//!
//! Recursively sets the enabled state for all parameters and parameter
//! groups contained in this group.
//!
//! \param enabled The value for the parameters' enabled flags.
//! \diveInGroups Flag to control whether to also process childs.
//! \propagate Flag to control whether to emit the enabledChanched signal.
//!
void ParameterGroup::setEnabled ( const bool enabled, const bool propagate, const bool diveInGroups )
{
	if (diveInGroups)
		foreach (AbstractParameter *abstParameter, m_parameterList)
			abstParameter->setEnabled(enabled, propagate);

	AbstractParameter::setEnabled(enabled, propagate);
}


//!
//! Sets the node to which this parameter group belongs.
//!
//! Recursively sets the node for all parameters and parameter groups
//! contained in this group.
//!
//! \param node The node to which the parameter group belongs.
//!
void ParameterGroup::setNode ( Node *node )
{
    AbstractParameter::setNode(node);

    foreach (AbstractParameter *abstParameter, m_parameterList)
			abstParameter->setNode(node);
}


//!
//! Creates an exact copy of the parameter.
//!
//! \return An exact copy of the parameter.
//!
AbstractParameter* ParameterGroup::clone ()
{
	return new ParameterGroup(*this);
}


//!
//! Returns whether the parameter object derived from this class is a
//! parameter group.
//!
//! \return True if the parameter object is a parameter group, otherwise False.
//!
inline bool ParameterGroup::isGroup () const
{
    return true;
}


//!
//! Resets the parameters contained in the group to their default values.
//!
void ParameterGroup::reset ()
{
    foreach (AbstractParameter *abstParameter, m_parameterList)
			abstParameter->reset();
}


//!
//! Disconnects all signals from parameters contained in the group from
//! slots of the given receiver object.
//!
//! \param receiver The object to disconnect the contained parameters from.
//!
void ParameterGroup::disconnectParameters ( QObject *receiver )
{
    // iterate over all parameter groups and parameters contained in this group
    foreach (AbstractParameter *abstParameter, m_parameterList)
        if (abstParameter->isGroup()) {
            // recursively disconnect parameters contained in the nested parameter group
            ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(abstParameter);
            if (parameterGroup)
                parameterGroup->disconnectParameters(receiver);
        } else {
            Parameter *parameter = dynamic_cast<Parameter *>(abstParameter);
            if (parameter)
                disconnect(parameter, 0, receiver, 0);
        }
}


//!
//! Clears the parameter group by emptying the parameter data structures.
//!
void ParameterGroup::clear ()
{
    m_parameterList.clear();
    m_parameterMap.clear();
}

//!
//! Destroys all parameters in the group.
//!
void ParameterGroup::destroyAllParameters ()
{
	foreach (AbstractParameter *parameter, m_parameterList) {
        delete parameter;
		parameter = 0;
	}
    clear();
}


//!
//! Returns a list of parameters whose names contain the given search
//! text.
//!
//! \param searchText The text to match the parameter names against.
//! \param diveInChildGroups Flag to control whether to also process child groups.
//! \return A list of parameters whose names contain the given search text.
//!
Parameter::List ParameterGroup::filterParameters ( const QString &searchText, const bool diveInChildGroups, const bool getAll /* = false */ ) const
{
    Parameter::List parameterList;
    if (getAll || searchText != "")
        foreach (AbstractParameter *abstParameter, m_parameterList) {
            if (abstParameter->isGroup() && diveInChildGroups) {
                // recursively search for the parameter in the current group
                ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(abstParameter);
                if (parameterGroup)
                    parameterList += parameterGroup->filterParameters(searchText, true, getAll);
            } else {
				if (getAll || abstParameter->getName().toLower().contains(searchText))
					if (!parameterList.contains(abstParameter))
						parameterList.append(abstParameter);
            }
        }
    return parameterList;
}


//!
//! Sorts the list of parameters by type.
//!
void ParameterGroup::sortParametersInList ()
{
	AbstractParameter::List groupList, parameterList;
	
	foreach(AbstractParameter *abstParameter, m_parameterList)
		if (abstParameter->isGroup())
			groupList.append(abstParameter);
		else
			parameterList.append(abstParameter);
	
	m_parameterList.clear();
	m_parameterList.append(parameterList);
	m_parameterList.append(groupList);
}


//!
//! Returns whether the group contains a parameter with the given name.
//!
//! \param name The parameter name to find in the group.
//! \return True if the group contains a parameter with the given name, otherwise False.
//!
bool ParameterGroup::contains ( const QString &name ) const
{
    bool result = false;
    int i = 0;
    while (!result && i < m_parameterList.size()) {
        if (m_parameterList[i]->isGroup())
            // recursively search for the parameter in the current group
            result = dynamic_cast<ParameterGroup *>(m_parameterList[i])->contains(name);
        else
            result = dynamic_cast<Parameter *>(m_parameterList[i])->getName() == name;
        ++i;
    }
    return result;
}

//!
//! Returns whether the group contains a group with the given name.
//!
//! \param name The group name to find in the group.
//! \return True if the group contains a group with the given name, otherwise False.
//!
bool ParameterGroup::containsGroup ( const QString &name ) const
{
    bool result = false;
    int  i= 0;
    AbstractParameter::Map::const_iterator iter = m_parameterMap.find(name);

    if (iter != m_parameterMap.end()) {
        if (iter.value()->isGroup()) {
            if (name == dynamic_cast<ParameterGroup *>(iter.value())->getName())
                result = true;
            else
                // recursively search for the parameter in the current group
                result = dynamic_cast<ParameterGroup *>(iter.value())->containsGroup(name);
        }
        else
            result = dynamic_cast<Parameter *>(iter.value())->getName() == name;
    }
    return result;
}


//!
//! Returns whether the group contains the given parameter.
//!
//! \param parameter The parameter to find in the group.
//! \return True if the group contains the given parameter, otherwise False.
//!
bool ParameterGroup::contains ( const Parameter *parameter ) const
{
    bool result = false;
    int i = 0;
    while (!result && i < m_parameterList.size()) {
        if (m_parameterList[i]->isGroup())
            // recursively search for the parameter in the current group
            result = dynamic_cast<ParameterGroup *>(m_parameterList[i])->contains(parameter);
        else
            result = dynamic_cast<Parameter *>(m_parameterList[i]) == parameter;
        ++i;
    }
    return result;
}


//!
//! Returns whether the group contains parameters of the given type.
//!
//! \param parameterType The parameter type to find in the group.
//! \return True if the group contains parameters of the given type, otherwise False.
//!
bool ParameterGroup::contains ( const Parameter::Type parameterType ) const
{
    bool result = false;
    int i = 0;
    while (!result && i < m_parameterList.size()) {
        if (m_parameterList[i]->isGroup())
            // recursively search for the parameter in the current group
            result = dynamic_cast<ParameterGroup *>(m_parameterList[i])->contains(parameterType);
        else
            result = dynamic_cast<Parameter *>(m_parameterList[i])->getType() == parameterType;
        ++i;
    }
    return result;
}


//!
//! Returns whether the group contains parameters of the given pin type.
//!
//! \param pinType The pin type of parameters to look for in the group.
//! \return True if the group contains parameters of the given pin type, otherwise False.
//!
bool ParameterGroup::contains ( const Parameter::PinType pinType ) const
{
    bool result = false;
    int i = 0;
    while (!result && i < m_parameterList.size()) {
        if (m_parameterList[i]->isGroup())
            // recursively search for parameters in the current group
            result = dynamic_cast<ParameterGroup *>(m_parameterList[i])->contains(pinType);
        else
            result = dynamic_cast<Parameter *>(m_parameterList[i])->getPinType() == pinType;
        ++i;
    }
    return result;
}


//!
//! Returns whether the group contains parameters of the given pin type and
//! of the given connection status.
//!
//! \param pinType The pin type of parameters to look for in the group.
//! \param connected The connection status of parameters to look for in the group.
//! \return True if the group contains parameters of the given pin type and of the given connection status, otherwise False.
//!
bool ParameterGroup::contains ( const Parameter::PinType pinType, const bool connected ) const
{
    bool result = false;
    int i = 0;
    while (!result && i < m_parameterList.size()) {
        if (m_parameterList[i]->isGroup())
            // recursively search for parameters in the current group
            result = dynamic_cast<ParameterGroup *>(m_parameterList[i])->contains(pinType, connected);
        else {
            Parameter *parameter = dynamic_cast<Parameter *>(m_parameterList[i]);
            result = parameter->getPinType() == pinType && parameter->isConnected() == connected;
        }
        ++i;
    }
    return result;
}


//!
//! Returns whether the group contains parameters or not
//!
//!
bool ParameterGroup::isEmpty ( ) const
{
	return m_parameterList.isEmpty() && m_parameterMap.isEmpty();
}


bool ParameterGroup::isVisible() const
{
	bool visible = false;
	foreach( AbstractParameter* absParameter, m_parameterList ){
		visible |= absParameter->isVisible();
	}
	return visible;
}

//!
//! Adds the given parameter to the list of parameters.
//!
//! \param parameter The parameter to add.
//! \param prepend Flag to control whether to insert the parameter at the beginning of the parameter list.
//!
inline void ParameterGroup::addParameter ( AbstractParameter *parameter, const bool prepend /* = false */ )
{
    if (!m_parameterMap.contains(parameter->getName())) {

		if( parameter->isGroup() ) {
			if( prepend ) {
				// look for first group and insert before
				AbstractParameter::List::iterator iter;
				for( iter = m_parameterList.begin(); iter!=m_parameterList.end(); ++iter) {
					if ((*iter)->isGroup())
						break;
				}
				//if (iter != m_parameterList.end())
					m_parameterList.insert(iter, parameter);
				//else
					//m_parameterList.append(parameter);

			} else {
				m_parameterList.append(parameter);
			}
		} else {
			if( prepend ) {
				m_parameterList.prepend( parameter);
			} else {
				AbstractParameter::List::iterator iter;
				// search for the first group and insert parameter before
				for (iter=m_parameterList.begin(); iter!=m_parameterList.end(); ++iter) {
					if ((*iter)->isGroup())
						break;
				}
				//if (iter != m_parameterList.end())
					m_parameterList.insert(iter, parameter);
				//else
				//	m_parameterList.append(parameter);
			}

		}	
		m_parameterMap.insert(parameter->getName() , parameter);
		
		if (m_node)
			parameter->setNode(m_node);

		if (!m_enabled)
			parameter->setEnabled(false);
	
	} else {
		Log::error(QString("A parameter \"%1.%2\" already exists.").arg(m_node->getName(), parameter->getName()), "ParameterGroup::addParameter");
	}
}

//!
//! Adds the given parameter to the group of parameters after the given parameter.
//!
//! \param parameter The parameter to add.
//! \param parameterAfter The parameter after which the parameter is inserted
//!
void ParameterGroup::addParameterAfter( AbstractParameter *parameter, AbstractParameter* parameterAfter )
{
	if( m_parameterMap.contains( parameter->getName() )) {
		Log::error(QString("A parameter \"%1.%2\" already exists.").arg(m_node->getName(), parameter->getName()), "ParameterGroup::addParameterAfter");
	
	} else {
		if( parameter->isGroup() ) {
			m_parameterList.append( parameter );

		} else {

			// search 2nd parameter in list and add 1st parameter after
			AbstractParameter::List::iterator iter = m_parameterList.begin();
			for(;; iter++ ) {
				if ( *iter == parameterAfter || iter == m_parameterList.end() )
					break;
			}
			if( iter == m_parameterList.end() ) {
				m_parameterList.push_back( parameter );
			} else {
				m_parameterList.insert( ++iter, parameter);
			}
		}

		m_parameterMap.insert( parameter->getName() , parameter);
		parameter->setNode(m_node);

		if (!m_enabled)
			parameter->setEnabled(false);
	}
}

//!
//! Adds the given parameter to the group of parameters after the given parameter.
//!
//! \param parameter The parameter to add.
//! \param parameterBefore The parameter before which the parameter is inserted
//!
void ParameterGroup::addParameterBefore( AbstractParameter *parameter, AbstractParameter* parameterBefore )
{
	if( m_parameterMap.contains( parameter->getName() )) {
		Log::error(QString("A parameter \"%1.%2\" already exists.").arg(m_node->getName(), parameter->getName()), "ParameterGroup::addParameterBefore");

	} else {
		if( parameter->isGroup() ) {
			m_parameterList.append( parameter );

		} else {

			// search 2nd parameter in list and add 1st parameter after
			AbstractParameter::List::iterator iter = m_parameterList.begin();
			for(;; iter++ ) {
				if ( *iter == parameterBefore || iter == m_parameterList.end() )
					break;
			}
			if( iter == m_parameterList.begin()) {
				m_parameterList.push_front(parameter);
			} else if (iter == m_parameterList.end() ) {
				m_parameterList.push_back( parameter );
			} else {
				m_parameterList.insert( iter, parameter);
			}
		}

		m_parameterMap.insert(parameter->getName() , parameter);
		parameter->setNode(m_node);

		if (!m_enabled)
			parameter->setEnabled(false);
	}
}

//!
//! Returns the parameter with the given name.
//!
//! \param name The name of the parameter to return.
//! \return The parameter with the given name.
//!
inline Parameter * ParameterGroup::getParameter ( const QString &name ) const
{
    Parameter *result = 0;

    QString parameterName = name;
    QString parentParameterGroupName = Parameter::extractFirstGroupName(&parameterName);

    if (parentParameterGroupName.isEmpty())
        if (m_parameterMap.contains(parameterName)) {
            AbstractParameter *abstractParameter = m_parameterMap[parameterName];
            if (!abstractParameter->isGroup())
                result = dynamic_cast<Parameter *>(abstractParameter);
        } else {
            // iterate over the parameters and recursively search for the parameter name in nested parameter groups
            AbstractParameter::Map::const_iterator iter = m_parameterMap.begin();
            while (iter != m_parameterMap.end() && !result) {
                if (iter.value()->isGroup())
                    result = dynamic_cast<ParameterGroup *>(iter.value())->getParameter(parameterName);
                ++iter;
            }
        }
    else
        if (m_parameterMap.contains(parentParameterGroupName)) {
            AbstractParameter *parentAbstractParameter = m_parameterMap[parentParameterGroupName];
            if (parentAbstractParameter->isGroup()) {
                ParameterGroup *parentParameterGroup = dynamic_cast<ParameterGroup *>(parentAbstractParameter);
                result = parentParameterGroup->getParameter(parameterName);
            }
        }

    return result;
}


//!
//! Removes and deletes the parameter with the given name from the
//! parameter group.
//!
//! \param name The name of the parameter to remove and delete.
//! \return 0.
//!
Parameter * ParameterGroup::removeParameter ( const QString &name, bool diveInGroups /*= true*/, bool deleteParameter/* = true */ )
{
    Parameter *parameter = getParameter(name);
    if (!parameter) {
        Log::warning(QString("Parameter \"%1.%2\" could not be found.").arg(m_node->getName(), name), "ParameterGroup::removeParameter");
        return 0;
    }

    return removeParameter(parameter, diveInGroups, deleteParameter );
}


//!
//! Removes and deletes the given parameter from the parameter group.
//!
//! \param parameter The parameter to remove and delete.
//! \return 0.
//!
Parameter * ParameterGroup::removeParameter ( Parameter *parameter, bool diveInGroups /*= true*/, bool deleteParameter/* = true*/ )
{
    int i = 0;
    while (parameter && i < m_parameterList.size()) {
        if (m_parameterList[i]->isGroup() && diveInGroups)
            // recursively remove the parameter in child groups
            parameter = dynamic_cast<ParameterGroup *>(m_parameterList[i])->removeParameter(parameter, diveInGroups, deleteParameter );
        ++i;
    }

    // check if the parameter has not been removed in a child group and is contained in this group
    if (parameter && m_parameterMap.contains(parameter->getName())) {
        m_parameterList.removeAll(parameter);
		m_parameterMap.remove(parameter->getName());

		if( deleteParameter ) {
			delete parameter;
			parameter = 0;
		}
    }

    return parameter;
}


//!
//! Removes and deletes the given parameter group from the parameter group.
//!
//! \param parameter The parameter group to remove and delete.
//!
void ParameterGroup::removeParameterGroup ( const QString &name )
{
    ParameterGroup *parameterGroup = getParameterGroup(name, false);

    m_parameterList.removeAll(parameterGroup);
    m_parameterMap.remove(name);

    delete parameterGroup;
}


//!
//! Returns the parameter group with the given name.
//!
//! \param name The name of the parameter group to return.
//! \return The parameter group with the given name.
//!
ParameterGroup * ParameterGroup::getParameterGroup ( const QString &name, bool recursive /*= true*/ ) const
{
	ParameterGroup *result = 0;
	AbstractParameter::Map::const_iterator iter = m_parameterMap.find(name);

	if (iter != m_parameterMap.end()){
		if (iter.value()->isGroup()) {
			ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(iter.value());
			return parameterGroup;
		}
	}
	else if (recursive) {
		for(iter = m_parameterMap.begin(); iter != m_parameterMap.end(); ++iter)
		if (iter.value()->isGroup()) {
			ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(iter.value());
			result = parameterGroup->getParameterGroup(name);
			if (result)
				return result;
		}
	}
	return result;
}


//!
//! Returns a pointer to the list of parameters and parameter groups that
//! are contained in this group.
//!
//! \return A pointer to the list of parameters of this group.
//!
const AbstractParameter::List& ParameterGroup::getParameterList () const
{
    return m_parameterList;
}


//!
//! Returns a pointer to the map of parameters and parameter groups that
//! are contained in this group.
//!
//! \return A pointer to the map of parameters of this group.
//!
const AbstractParameter::Map& ParameterGroup::getParameterMap () const
{
    return m_parameterMap;
}

//!
//! Fills a list with all parameters contained in the group.
//!
//! \return The list containing all Parameters from the parameter group.
//!
AbstractParameter::List ParameterGroup::getAllParameters ()
{
	return getAllParameters (m_parameterList);
}


//!
//! Internal function that recursively fills a list with all parameters contained
//! in the group.
//!
//! \param parameter the input parameter to be analyzed.
//! \return The list containing all Parameters from the parameter group.
//!
AbstractParameter::List ParameterGroup::getAllParameters (const List& parameterList)
{
	List returnList;
	foreach(AbstractParameter *parameter, parameterList) {
		if (parameter->isGroup()) {
			ParameterGroup *parameterGroup = static_cast<ParameterGroup *>(parameter);
			returnList.append(getAllParameters( parameterGroup->getParameterList()));
		}
		else
			returnList.append(parameter);
	}
	return returnList;
}

} // end namespace Frapper