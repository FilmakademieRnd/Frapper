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
//! \file "WidgetPlugin.h"
//! \brief Implementation file for WidgetPlugin.
//!
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       22.12.2009 (last updated)
//!

#ifndef WIDGETPLUGIN_H
#define WIDGETPLUGIN_H

#include <QWidget>
#include <QtCore/QVariant>

#include "FrapperPrerequisites.h"
#include "ParameterPlugin.h"

namespace Frapper {

class FRAPPER_GUI_EXPORT WidgetPlugin: public QWidget
{
    Q_OBJECT
	

public:// constructors and destructors : public Parameter

	//!
	//! Constructor of the WidgetPlugin class.
	//!
	//! \param parent The parent widget.
	//! \param parameter The associated parameter.
	//!
	WidgetPlugin( QWidget *parent , ParameterPlugin * parameter);

	//!
	//! Destructor of the WidgetPlugin class.
	//!
	//! Defined virtual to guarantee that the destructor of a derived class
	//! will be called if the instance of the derived class is saved in a
	//! variable of its parent class type.
	//!
	virtual ~WidgetPlugin(void);

public: // functions

	//!
	//! Returns the parameter큦 value
	//!
	//! \return the QVariant value of the parameter
	//!
	QVariant getParameterValue();

	//!
	//! sets the parameter큦 value
	//!
	//! \param value The QVariant to be set as the parameter큦 value
	//!
	void setParameterValue(QVariant value);

	//!
	//! returns the name of the associated parameter
	//!
	//! \return The name of the parameter as QString
	//!
	QString getParameterName();

	//!
	//! is called to emit the signal "updateWidget"
	//!
	void widgetUpdate();

	//!
	//! updates the parameter and forces a update of the widget
	//!
	//! \param value The QVariant to be set as the parameter큦 value
	//!
	void updateParameterValue(QVariant value);

	//!
	//! sets the flags of the parameter
	//!
	//! \param flags The QMap of flags
	//!
	void setFlags(QMap<QString, QVariant> flags);

	//!
	//! set a Flag
	//!
	//! \param key Name of the flag
	//! \param value Value of the flag
	//!
	void setFlag(QString key, QVariant value);

	//!
	//! returns the flags of the parameter
	//!
	//! \return The QMap of flags
	//!
	QMap<QString, QVariant> getFlags();

	//!
	//! Sets or deletes a connection between the parameter큦 update and the widget큦 update
	//!
	//! \param trigger Controll if the connection should be created (true) or destroyed (false)
	//!
	void setParameterTriggersRedraw(bool trigger);

	//!
	//! returns a list of all flag keys
	//!
	//! \return QStringList of all flag names
	//!
	QStringList getFlagList();

	//!
	//! cecks if a flag exists and has a value
	//!
	//! \param flag Name of the flag
	//! \return Boolean value, "true" if flag exists and has a value
	//!
	bool checkFlag(QString flag);

	//!
	//! returns the value of a flag
	//!
	//! \param flag Name of the flag
	//! \return QVariant value of the flag
	//!
	QVariant getFlag(QString flag);

	
public: // data

private slots:

	//!
	//! is called if parameter is changed
	//!
	void redrawByParameterChange();

protected:  // Data

	//!
	//! the associated parameter
	//! 
	ParameterPlugin * m_parameter;

private: // Data

	//!
	//! Flag if the parameter change was done by the widget
	//!
	bool m_selfTriggered;

signals:

	//!
	//! is emitted if the widget must be redrawn
	//!
	void updateWidget();
};

} // end namespace Frapper

#endif
