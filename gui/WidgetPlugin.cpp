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
//! \file "WidgetPlugin.cpp"
//! \brief Implementation file for WidgetPlugin.
//!
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       22.12.2009 (last updated)
//!

#include "WidgetPlugin.h"
#include "Log.h"


namespace Frapper {

///
/// Constructors and Destructors
///

//!
//! Constructor of the WidgetPlugin class.
//!
//! \param parent The parent widget.
//! \param parameter The associated parameter.
//!
WidgetPlugin::WidgetPlugin( QWidget *parent ,  ParameterPlugin * parameter) : QWidget (parent)
{
	// saves the parameter
	m_parameter = parameter;

	QObject::connect(this, SIGNAL(updateWidget()), this, SLOT(update()));
	QObject::connect(m_parameter, SIGNAL(forceRedraw()), this, SIGNAL(updateWidget()));

	// Controlls if a "ParameterTriggersRedraw" flag is set
	if(checkFlag("ParameterTriggersRedraw"))
		if(getFlag("ParameterTriggersRedraw").toBool())
			this->setParameterTriggersRedraw(true);
}

//!
//! Destructor of the WidgetPlugin class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
WidgetPlugin::~WidgetPlugin(void)
{
}

///
/// Public Functions
///

//!
//! Returns the parameters value
//!
//! \return the QVariant value of the parameter
//!
QVariant WidgetPlugin::getParameterValue()
{
	return m_parameter->getValue(true);
}

//!
//! sets the parameter큦 value
//!
//! \param value The QVariant to be set as the parameter큦 value
//!
void WidgetPlugin::setParameterValue(QVariant value)
{
	m_parameter->setValue(value);
}

//!
//! returns the name of the associated parameter
//!
//! \return The name of the parameter as QString
//!
QString WidgetPlugin::getParameterName()
{
	return m_parameter->getName();
}

//!
//! updates the parameter and forces a update of the widget
//!
//! \param value The QVariant to be set as the parameter큦 value
//!
void WidgetPlugin::updateParameterValue(QVariant value)
{
	m_selfTriggered = true;

	m_parameter->setValue(value);

	emit updateWidget();
}

//!
//! is called to emit the signal "updateWidget"
//!
void WidgetPlugin::widgetUpdate()
{
	emit updateWidget();
}

//!
//! sets the flags of the parameter
//!
//! \param flags The QMap of flags
//!
void WidgetPlugin::setFlags(QMap<QString, QVariant> flags)
{
	m_parameter->setFlags(flags);
}

//!
//! set a Flag
//!
//! \param key Name of the flag
//! \param value Value of the flag
//!
void WidgetPlugin::setFlag(QString key, QVariant value)
{
	m_parameter->addFlag(key, value);
}

//!
//! returns the flags of the parameter
//!
//! \return The QMap of flags
//!
QMap<QString, QVariant> WidgetPlugin::getFlags()
{
	return m_parameter->getFlags();
}


//!
//! Sets or deletes a connection between the parameter큦 update and the widget큦 update
//!
//! \param trigger Controll if the connection should be created (true) or destroyed (false)
//!
void WidgetPlugin::setParameterTriggersRedraw(bool trigger)
{
	if(trigger) {
		QObject::connect(m_parameter, SIGNAL(valueChanged()), this, SLOT(redrawByParameterChange()));
		QObject::connect(m_parameter, SIGNAL(enabledChanged()), this, SLOT(redrawByParameterChange()));
	}
	else {
		QObject::disconnect(m_parameter, SIGNAL(valueChanged()), this, SLOT(redrawByParameterChange()));
		QObject::disconnect(m_parameter, SIGNAL(enabledChanged()), this, SLOT(redrawByParameterChange()));
	}
}


//!
//! is called if the parameter is changed
//!
void WidgetPlugin::redrawByParameterChange()
{
	if(!m_selfTriggered)
		emit updateWidget();

	m_selfTriggered = false;
}

//!
//! returns a list of all flag keys
//!
//! \return QStringList of all flag names
//!
QStringList WidgetPlugin::getFlagList()
{
	QStringList flagList = m_parameter->getFlags().keys();
	return flagList;
}

//!
//! cecks if a flag exists and has a value
//!
//! \param flag Name of the flag
//! \return Boolean value, "true" if flag exists and has a value
//!
bool WidgetPlugin::checkFlag(QString flag)
{
	bool flagValid = false;

	if(m_parameter->getFlags().keys().contains(flag))
		if(!m_parameter->getFlag(flag).isNull())
			flagValid = true;

	return flagValid;
}

//!
//! returns the value of a flag
//!
//! \param flag Name of the flag
//! \return QVariant value of the flag
//!
QVariant WidgetPlugin::getFlag(QString flag)
{
	return m_parameter->getFlag(flag);
}

} // end namespace Frapper