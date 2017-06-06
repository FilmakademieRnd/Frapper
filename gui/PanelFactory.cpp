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
//! \file "PanelFactory.cpp"
//! \brief Implementation file for PanelFactory class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author		Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       02.12.2009 (last updated)
//!

#include "PanelFactory.h"
#include "Log.h"
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
/// Private Static Data
///
QMap<QString, PanelTypeInterface*> PanelFactory::m_panelTypeMap;


///
/// Public Static Data
///
QStringList PanelFactory::m_panelPluginFiles;
QStringList PanelFactory::m_panelPluginNames;
QStringList PanelFactory::m_panelPluginIcons;

//!
//! Loads the XML description of a panel type from the file with the given
//! name.
//!
//! \param filename The name of an XML file describing a panel type.
//!
bool PanelFactory::registerType ( const QString &filename )
{
	QFile file (filename);
    if (!file.exists()) {
        Log::error(QString("Panel plugin file not found: \"%1\"").arg(filename), "PanelFactory::registerType");
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Log::error(QString("Error loading panel plugin file \"%1\".").arg(filename), "PanelFactory::registerType");
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
        Log::error(QString("Error parsing \"%1\": %2, line %3, char %4").arg(filename).arg(errorMessage).arg(errorLine).arg(errorColumn), "PanelFactory::registerType");
        return false;
    }

    // obtain node type information from attributes
    QDomElement rootElement = description.documentElement();
    QString PluginName = rootElement.attribute("name");
    QString PluginDLL = rootElement.attribute("plugin");
	QString PluginIcon = rootElement.attribute("icon");

	// If no "icon" defined, use standard icon
	if(PluginIcon.isEmpty())
		PluginIcon = QString(":/pluginIcon");

	//// decode the full filename into its path and base file name
    QString filePath = file.fileName().mid(0, file.fileName().lastIndexOf('/') + 1);
    QString baseFilename = file.fileName().mid(file.fileName().lastIndexOf('/') + 1);
    Log::debug(QString("Parsing \"%1\"...").arg(baseFilename), "PanelFactory::registerType");

    // load plugin if a plugin filename is given
    if (filename != "") {
        QString pluginFilename =  filePath  + PluginDLL;

#ifdef _DEBUG
        // adjust the plugin filename to load the debug version of the DLL
        pluginFilename = pluginFilename.replace(".dll", "_d.dll");
#endif
        if (QFile::exists(pluginFilename)) {
            Log::debug(QString("Loading plugin \"%1\"...").arg(pluginFilename), "PanelFactory::registerType");
            QPluginLoader loader (pluginFilename);
            PanelTypeInterface *panelTypeInterface = qobject_cast<PanelTypeInterface *>(loader.instance());
            if (!panelTypeInterface) {
                Log::error(QString("Plugin \"%1\" could not be loaded: %2").arg(pluginFilename).arg(loader.errorString()), "PanelFactory::registerType");
                return false;
            }

 			if (!m_panelTypeMap.keys().contains(baseFilename)){
				m_panelTypeMap[baseFilename] = panelTypeInterface;
				//m_panelNameMap[baseFilename] = baseFilename;
				m_panelPluginFiles.append(baseFilename);
				m_panelPluginNames.append(PluginName);
				m_panelPluginIcons.append(PluginIcon);
			}
        } else {
            Log::error(QString("Plugin file \"%1\" could not be found.").arg(pluginFilename), "PanelFactory::registerType");
            return false;
        }
    }

    return true;
}

//!
//! Creates a panel of the given type name with the given name.
//!
//! \param typeName The name of the type to use for the new panel.
//! \param name The name to give to the new panel.
//! \return A pointer to the new panel.
//!
Panel * PanelFactory::createPanel ( const QString &typeName, QWidget *parent )
{
    Panel *panel;
    // check if a panel type of the given name is registered

	if (m_panelTypeMap.keys().contains(typeName)) {
        // create a new panel of the given type
        PanelTypeInterface *panelTypeInterface = m_panelTypeMap[typeName];
		if(panelTypeInterface) {
			panel = panelTypeInterface->createPanel(parent);
		}
		else{
			Log::error(QString("Type name returns no valid pointer"), "PanelFactory::createPanel");
			panel = 0;
		}
	}
    else {
		Log::error(QString("Panel type is not a dynamic panel."),"PanelFactory::createPanel");
		panel = 0;
    }

    return panel;
}


//!
//! Frees all resources that were used by private static data of the panel
//! factory class.
//!
void PanelFactory::freeResources ()
{
}

} // end namespace Frapper