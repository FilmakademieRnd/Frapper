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
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "ParameterPlugin.cpp"
//! \brief Implementation file for vectorfieldclass.
//!
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       22.12.2009 (last updated)
//!

#include "ParameterPlugin.h"

namespace Frapper {

///
/// Constructors and Destructors
///

//!
//! Constructor of the ParameterPlugin class.
//!
//! \param name The name of the parameter.
//! \param type The type of the parameter's widget.
//! \param falgs The flags of the parameter.
//! \param value The parameter's value.
//!
ParameterPlugin::ParameterPlugin ( const QString &name, QString type, QMap<QString, QVariant> flags, const QVariant &value )  : 
Parameter(name, Parameter::T_PlugIn, value) 
{
    m_call = type;
	m_flags = flags;
	
}
//!
//! Constructor of the ParameterPlugin class
//! Overload without flags
//!
//! \param name The name of the parameter.
//! \param type The type of the parameter's widget.
//! \param value The parameter's value.
//!
ParameterPlugin::ParameterPlugin ( const QString &name, QString type, const QVariant &value )  : 
Parameter(name, Parameter::T_PlugIn, value) 
{
    m_call = type;	
}

//!
//! Constructor of the ParameterPlugin class
//! Overload without flags or call
//!
//! \param name The name of the parameter.
//! \param value The parameter's value.
//!
//!
ParameterPlugin::ParameterPlugin ( const QString &name, const QVariant &value ): 
Parameter(name, Parameter::T_PlugIn, value) 
{
	m_call = "";
}

//!
//! Copy constructor
//!
//! \param parameter The parameter to copy
//!
ParameterPlugin::ParameterPlugin ( ParameterPlugin * parameter ): 
Parameter(parameter->getName(), Parameter::T_PlugIn, parameter->getValue()) 
{
	m_call = parameter->getCall();
	m_flags = parameter->getFlags();
}

//!
//! Destructor of the ParameterPlugin class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ParameterPlugin::~ParameterPlugin()
{

}

///
/// Public Functions
///

//!
//! Sets the flags, replaces existing flags
//!
//! \param flags QMap of flags
//!
void ParameterPlugin::setFlags(QMap<QString, QVariant> flags)
{
	m_flags = flags;
}

//!
//! Adds a flag, overwrites the flag if existing
//!
//! \param key The name of the flag
//! \param value The value of the flag
//!
void ParameterPlugin::addFlag(QString key, QVariant value)
{
	m_flags[key] = value;
}

//!
//! returns all flags
//!
//! \return A QMap of all flags
//!
QMap<QString, QVariant> ParameterPlugin::getFlags()
{
	return m_flags;
}

//!
//! returns the value of a certain flag
//!
//! \param flag The name of the flag
//! \return the Value of the flag
//!
QVariant ParameterPlugin::getFlag(QString flag)
{
	if(m_flags.keys().contains(flag)){
		return m_flags[flag];
	}else{
		return QVariant();
	}
}

//!
//! returns the identification of the widget
//!
//! \return The name of the associated plugin widget
//!
QString ParameterPlugin::getCall()
{
	return m_call;
}	

//!
//! sets the identification of the widget
//!
//! \param call The name of the associated widget
//!
void ParameterPlugin::setCall(QString call)
{
	m_call = "";
	m_call = call;
}

//!
//! forces the associated widget to redraw
//!
void ParameterPlugin::triggerWidgetRedraw()
{
	emit forceRedraw();
}

//																				
//
//	the following functions are still in developement and not intended for use
//
//


//!
//! copies the value from another parameter to this parameter
//!
//! \param parameter Pointer of the source parameter
//!
void ParameterPlugin::copyParameterValue(Parameter * parameter)
{
	setValue(parameter->getValue());
}

//!
//! copies the value from a nuumber parameter to this parameter
//!
//! \param numberParameter Pointer of the source NumberParameter
//!
void ParameterPlugin::copyNumberParameterValue(NumberParameter * numberParameter)
{
	Parameter * parameter =  dynamic_cast<Parameter *>(numberParameter);
	copyParameterValue(parameter);
}


//!
//! Sets the value of a integer to the integer value of a flag
//!
//! \param flag Name of the flag
//! \param number Pointer to the number variable to be set
//! \return True if number was changed
//!
bool ParameterPlugin::getIntegerFlag(QString flag, int& number)
{
	bool change = false;

	if(m_flags.keys().contains(flag))
		if(!m_flags[flag].isNull())
			if(m_flags[flag].type() == QVariant::Int){
				number = m_flags[flag].toInt();
				change = true;
			}
	return change;
}

//!
//! Sets the value of a float to the float value of a flag
//!
//! \param flag Name of the flag
//! \param number Pointer to the number variable to be set
//! \return True if number was changed
//!
bool ParameterPlugin::getFloatFlag(QString flag, float& number)
{
	bool change = false;

	if(m_flags.keys().contains(flag))
		if(!m_flags[flag].isNull())
			if(m_flags[flag].type() == QVariant::Double){
				number = m_flags[flag].toFloat();
				change = true;
			}
	return change;
}

//!
//! Sets the value of a QString to the QString value of a flag
//!
//! \param flag Name of the flag
//! \param number Pointer to the string variable to be set
//! \return True if string was changed
//!
bool ParameterPlugin::getStringFlag(QString flag, QString& string)
{
	bool change = false;

	if(m_flags.keys().contains(flag))
		if(!m_flags[flag].isNull())
			if(m_flags[flag].type() == QVariant::String){
				string = m_flags[flag].toString();
				change = true;
			}
	return change;
}

//!
//! Sets the value of a bool to the bool value of a flag
//!
//! \param flag Name of the flag
//! \param boolean Pointer to the boolean variable to be set
//! \return True if boolean variabe was changed
//!
bool ParameterPlugin::getBoolFlag(QString flag, bool& boolean)
{
	bool change = false;

	if(m_flags.keys().contains(flag))
		if(!m_flags[flag].isNull())
			if(m_flags[flag].type() == QVariant::Bool){
				boolean = m_flags[flag].toBool();
				change = true;
			}
	return change;
}

//!
//! Returns the type of a flag
//!
//! \param flag Nam of the flag
//! \return Type of the flag
//!
QVariant::Type ParameterPlugin::getFlagType(QString flag)
{
	QVariant::Type variant_type = QVariant::Invalid;

	if(m_flags.keys().contains(flag))
		variant_type = m_flags[flag].type();

	return variant_type;
}

//!
//! Returns the type of the parameters value
//!
//! \return Type of the parameters value
//!
QVariant::Type ParameterPlugin::getParameterType()
{
	QVariant::Type variant_type = QVariant::Invalid;
	variant_type = getValue(true).type();
	return variant_type;
}

//!
//! Transforms a List of floats into a list of QVariants
//!
//! \param list QList of float values
//! \return QList of QVariant values
//!
QList<QVariant> * ParameterPlugin::TransformFloatToVariantList(QList<float> * list)
{
	QList<QVariant> *variant_list = new QList<QVariant>;

	for(int i = 0; i < list->length(); ++i)
		variant_list->append(QVariant(list->at(i)));

	return variant_list;
}

//!
//! Transforms a List of QVariants into a list of floats
//!
//! \param list QList of QVariant values
//! \return QList of float values
//!
QList<float> * ParameterPlugin::TransformVariantToFloatList(QList<QVariant> * list)
{
	QList<float> *float_list = new QList<float>;

	for(int i = 0; i < list->length(); ++i)
		float_list->append(list->at(i).toDouble());

	return float_list;
}

} // end namespace Frapper