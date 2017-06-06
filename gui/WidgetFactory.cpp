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
//! \file "WidgetFactory.cpp"
//! \brief Implementation file for WidgetFactory class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       10.12.2009 (last updated)
//!

#include "WidgetFactory.h"
#include "Log.h"
#include "Parameter.h"
#include <QtCore/QFile>
#include <QtCore/QPluginLoader>
#include <QTextStream>
#include <QtXml/QDomDocument>
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlInputSource>
#include <QtCore/QPluginLoader>
#include <QtCore/QVarLengthArray>


namespace Frapper {

///
/// Public Static Data
///
QStringList WidgetFactory::m_WidgetNames;
QStringList WidgetFactory::m_WidgetCall;
QMap<QString, WidgetTypeInterface*> WidgetFactory::m_widgetTypeMap;

//!
//! Loads the XML description of a widget type from the file with the given
//! name.
//!
//! \param filename The name of an XML file describing a widget type.
//!
bool WidgetFactory::registerType ( const QString &filename )
{
	QFile file (filename);
    if (!file.exists()) {
        Log::error(QString("widget plugin file not found: \"%1\"").arg(filename), "WidgetFactory::registerType");
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Log::error(QString("Error loading widget plugin file \"%1\".").arg(filename), "WidgetFactory::registerType");
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
        Log::error(QString("Error parsing \"%1\": %2, line %3, char %4").arg(filename).arg(errorMessage).arg(errorLine).arg(errorColumn), "WidgetFactory::registerType");
        return false;
    }

    // obtain node type information from attributes
    QDomElement rootElement = description.documentElement();
    QString PluginName = rootElement.attribute("name");
    QString PluginDLL = rootElement.attribute("plugin");
	QString PluginCall = rootElement.attribute("call");


	//// decode the full filename into its path and base file name
    QString filePath = file.fileName().mid(0, file.fileName().lastIndexOf('/') + 1);
    QString baseFilename = file.fileName().mid(file.fileName().lastIndexOf('/') + 1);
    Log::debug(QString("Parsing \"%1\"...").arg(baseFilename), "WidgetFactory::registerType");

    // load plugin if a plugin filename is given
    if (filename != "") {
        QString pluginFilename =  filePath  + PluginDLL;

#ifdef _DEBUG
        // adjust the plugin filename to load the debug version of the DLL
        pluginFilename = pluginFilename.replace(".dll", "_d.dll");
#endif
        if (QFile::exists(pluginFilename)) {
            Log::debug(QString("Loading plugin \"%1\"...").arg(pluginFilename), "WidgetFactory::registerType");
            QPluginLoader loader (pluginFilename);
            WidgetTypeInterface *widgetTypeInterface = qobject_cast<WidgetTypeInterface *>(loader.instance());
            if (!widgetTypeInterface) {
                Log::error(QString("Plugin \"%1\" could not be loaded: %2").arg(pluginFilename).arg(loader.errorString()), "WidgetFactory::registerType");
                return false;
            }
			if(!m_widgetTypeMap.contains(PluginCall)){
				m_widgetTypeMap[PluginCall] = widgetTypeInterface;
			}
        } else {
            Log::error(QString("Plugin file \"%1\" could not be found.").arg(pluginFilename), "WidgetFactory::registerType");
            return false;
        }
    }

    return true;
}

//!
//! Returns a widget
//!
//! \param call Identification of the widget
//! \param parameter Associated parameter
//! \param parent Parent widget
//! \return The created widget
//!
WidgetPlugin * WidgetFactory::createWidget(QString call , ParameterPlugin * parameter , QWidget *parent)
{
	WidgetPlugin *widget;

	if (m_widgetTypeMap.keys().contains(call)) {
        // create a new widget of the given type
        WidgetTypeInterface *widgetTypeInterface = m_widgetTypeMap[call];
		if(widgetTypeInterface) {
			widget = widgetTypeInterface->createWidget(parent, parameter);
		}
		else{
			Log::error(QString("Type name returns no valid pointer"), "WidgetFactory::createWidget");
			widget = 0;
		}
	}
    else {
		Log::error(QString("Widget type is not a dynamic widget."),"WidgetFactory::createWidget");
		widget = 0;
    } 

	return widget;
}

} // end namespace Frapper