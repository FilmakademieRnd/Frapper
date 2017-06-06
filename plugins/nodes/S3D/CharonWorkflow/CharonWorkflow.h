/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "CharonWorkflow.cpp"
//! \brief Header file for CharonWorkflow class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       12.08.2013 (created)
//! \date       12.08.2013 (last updated)

//!

#ifndef CHARONWORKFLOW_H
#define CHARONWORKFLOW_H

#include <charon-core/PluginManager.h>
#include <charon-core/ParameteredObject.h>
#include <charon-core/Parameter.h>

#include <QObject>
#include <QString>
#include <QList>
#include <map>

#ifdef _DEBUG
	#define CharonUseDebug true
#else
	#define CharonUseDebug false
#endif

namespace CharonWorkflowNode {

//!
//! Base class for all render nodes.
//!
class CharonWorkflow : public QObject {

	Q_OBJECT

public: // enumerations

	enum EditorPriority {
		EP_Unset = 0,
		EP_Low,
		EP_Medium,
		EP_High,
		EP_NumPriorities
	};


public: // constructors and destructors

    //!
    //! Constructor of the CharonWorkflow class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	CharonWorkflow ();

	//!
    //! Destructor of the CharonWorkflow class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CharonWorkflow ();


public slots:
	//!
	//! Execute loaded workflow
	//!
	void ExecuteWorkflow ();

signals:

	//!
	//! Signal that is emitted when the workflow execution was finished
	//!
	void ExecutionFinished();

	//!
	//! Signal that is emitted when the workflow execution has thrown an error
	//!
	void ExecutionError(QString errorMessage);

public: // functions

	//!
	//! Sets the default template type
	//!
	void setDefaultTemplateType( ParameteredObject::template_type templateType );;

	//!
	//! Sets the charon path
	//!
	void setCharonPath( QString charonPath );

	//!
	//! Load charon workflow file
	//! 
	bool loadWorkflowFile ( QString workflowFile);
	
	//!
	//! save current workflow state to workflow file
	//!
	void saveWorkflow();

	//!
	//! Reset execution
	//!
	void ResetWorkflow ();

	//!
	//! Reset a specific plugin of the loaded workflow
	//!
	void ResetPlugin( QString pluginName );

	//!
	//! get connected input slots
	//!
	const std::map<std::string, std::map<std::string, Slot*>>& getConnectedInputSlots();

	//!
	//! get unconnected input slots
	//!
	const std::map<std::string, std::map<std::string, Slot*>>& getUnconnectedInputSlots();

	//!
	//! get connected output slots
	//!
	const std::map<std::string, std::map<std::string, Slot*>>& getConnectedOutputSlots();

	//!
	//! get unconnected output slots
	//!
	const std::map<std::string, std::map<std::string, Slot*>>& getUnconnectedOutputSlots();

	//!
	//! get parameters of all plugins
	//!
	std::map<std::string, std::map<std::string, AbstractParameter *>> getParameters();

	//!
	//! get parameter by name. Name should be formatted "Plugin.Parameter" or pluginName needs to be given
	//!
	AbstractParameter* getParameter( QString name, QString pluginName = QString() );
	
	//!
	//! get all input slots of a plugin
	//!
	std::map<std::string, Slot*> getPluginInputSlots( QString pluginName );

	//!
	//! get all output slots of a plugin
	//!
	std::map<std::string, Slot*> getPluginOutputSlots( QString pluginName );

	//!
	//! Get the editor comment for a specific workflow parameter
	//!
	QString getEditorComment( QString parameterName );

	//!
	//! Get the editor priority for a specific workflow parameter
	//!
	EditorPriority getEditorPriority( QString parameterName );

	//!
	//! Get meta data from plugin
	//!
	QString getPluginMetadata(QString pluginName, QString metadataName );

	//!
	//! Get the plugin description from meta data
	//!
	QString getPluginDescription( QString pluginName );

	//!
	//! Get meta data from parameter
	//!
	QString getParameterMetadata(QString pluginName, QString parameterName, QString metadataName );

	//!
	//! Get the parameter description from meta data
	//!
	QString getParameterDescription( QString pluginName, QString parameterName );

	//!
	//! Get the parameter type from meta data
	//!
	QString getParameterType( QString pluginName, QString parameterName );

	//!
	//! Get the parameter type from meta data
	//!
	QString getParameterDefaultValue( QString pluginName, QString parameterName );

	//!
	//! Sets execute flag on loaded plugins
	//!
	void setActive( QString pluginName, bool active );

private: // functions

	// create plugin manager using given charon path and template type
	bool initialize();

	void runWorkflow();

	void writeWorkflowFile();

	//! get plugin from workflow by name
	ParameteredObject* getPlugin( QString pluginName );

	//! Split given parameterName of format Plugin.Parameter into plugin and parameter name
	void splitParameterName( const QString& pluginAndParameterName, QString& parameterName, QString& pluginName );

	void determineTargetPoints();
	void disconnectAllSlots();

	void calculateSlotLists();

private: // data

	// The charon plugin manager used to load charon workflows
	PluginManager *m_pluginManager;

	ParameteredObject::template_type m_templateType, m_oldTemplateType;
	QString m_charonPath, m_oldCharonPath;

	// maps of plugin name to in/output slots of plugin
	std::map<std::string, std::map<std::string, Slot*>> m_unconnectedInputSlots;
	std::map<std::string, std::map<std::string, Slot*>> m_unconnectedOutputSlots;
	std::map<std::string, std::map<std::string, Slot*>> m_connectedInputSlots;
	std::map<std::string, std::map<std::string, Slot*>> m_connectedOutputSlots;

	ParameterFile m_parameterFile;
	bool m_workflowLoaded;
	QString m_workflowFile;

	std::set<ParameteredObject*> m_targetPoints;
};

} // end namespace

#endif
