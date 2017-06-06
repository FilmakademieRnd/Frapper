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
//! \brief Implementation file for CharonWorkflow class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       12.08.2013 (created)
//! \date       22.08.2013 (last updated)
//!

#include "CharonWorkflow.h"

// template implementation of ParameterFile functions
#include <charon-core/ParameterFile.hxx>

#include <QFile>
#include <QDir>
#include <Log.h>
#include <SceneModel.h>

Q_DECLARE_METATYPE(Slot *);

namespace CharonWorkflowNode {

///
/// Constructors and Destructors
///

 
//!
//! Constructor of the CharonWorkflow class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
CharonWorkflow::CharonWorkflow ( ) :
	m_pluginManager(0),
	m_charonPath(""), m_oldCharonPath(""),
	m_templateType(ParameteredObject::TYPE_FLOAT), m_oldTemplateType(ParameteredObject::TYPE_FLOAT),
	m_workflowLoaded(false),
	m_workflowFile("")
{
	// pipe charon messages to frapper log
	Frapper::Log::s_streamListener.setName("Charon");
	sout.assign( Frapper::Log::s_streamListener );
}


//!
//! Destructor of the CharonWorkflow class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CharonWorkflow::~CharonWorkflow ()
{
	if( m_pluginManager)
	{
		delete m_pluginManager;
	}
}


bool CharonWorkflow::initialize()
{
	if( m_pluginManager ) {
		try {
			disconnectAllSlots();
			delete m_pluginManager; // may throw exception
			m_pluginManager = 0;
		} catch (std::exception& ex) {
			Frapper::Log::error( "An error occured while deleting the plugin manager: " + QString(ex.what()), "CharonWorkflow::initialize");
			return false;
		}
	}

	try {

#ifdef _WIN32
		if( QFile().exists(m_charonPath+"/bin/charon-core.dll")) { // This is windows specific!
#else
		if( m_charonPath != "" ) {
#endif // _WIN32
			std::string charonPathStd = m_charonPath.toStdString() + "/bin";
			m_pluginManager = new PluginManager( charonPathStd, "", CharonUseDebug, false);
			m_pluginManager->setDefaultTemplateType( m_templateType );
			m_oldCharonPath = m_charonPath;
			m_oldTemplateType = m_templateType;
		} else {
			Frapper::Log::error("The given charon path is empty or charon-core could not be found in the given path.", "CharonWorkflowNode::Initialize");
			return false;
		}
	} catch( std::exception e) {
		Frapper::Log::error("The charon plugin manager could not be initialized.", "CharonWorkflowNode::Initialize");
		Frapper::Log::error("Charon path used: "+m_charonPath, "CharonWorkflowNode::Initialize");
	}

	return true;
}

//!
//! Loads the specified Charon
//!
bool CharonWorkflow::loadWorkflowFile ( QString workflowFile )
{
	// check if plugin manager requires initialization
	if( !m_pluginManager && !initialize() ) {
		Frapper::Log::error("The plugin manager could not be initialized!", "CharonWorkflowNode::loadWorkflowFile");
		return false;
	} 

	// Check if given workflow file is valid
	if( workflowFile == "" || !workflowFile.endsWith(".wrp", Qt::CaseInsensitive)) {
		Frapper::Log::error("The given workflow file name is empty or does not specify a charon workflow file.", "CharonWorkflowNode::loadWorkflowFile");
		return false;
	}

	// some vital parameters of the PM have changed -> PM requires re-initialization
	if ( m_templateType != m_oldTemplateType || m_charonPath != m_oldCharonPath ) 
	{
		if( !initialize() ) {
			Frapper::Log::error("The plugin manager could not be initialized!", "CharonWorkflowNode::loadWorkflowFile");
			return false;
		}
	}

	// disconnect all slots of all plugins before resetting
	disconnectAllSlots();

	// reset plugin manager
	m_pluginManager->reset();

	// reset sets default template to current template type
	m_pluginManager->setDefaultTemplateType(m_templateType);

	// Charons plugin manager changes the current execution directory, 
	// so it needs to be restored after execution
	std::string currentDir = FileTool::getCurrentDir();

	m_workflowLoaded = false;
	try
	{
		m_parameterFile.load( workflowFile.toStdString() ); // throws

		// set desired template type in parameter file
		m_parameterFile.set<std::string> ("global.templatetype", m_pluginManager->templateTypeToString( m_templateType ));

		m_pluginManager->loadParameterFile( m_parameterFile );

		m_workflowLoaded = true;
		m_workflowFile = workflowFile;

		// determine target points for execution
		determineTargetPoints();

		calculateSlotLists();
	}
	catch (std::exception& e)
	{
		Frapper::Log::error( QString(e.what()), "CharonWorkflowNode::loadCharonPlugin");
		Frapper::Log::error( "The given Workflow could not be loaded:", "CharonWorkflowNode::loadCharonPlugin");
	}

	FileTool::changeDir(currentDir);

	return m_workflowLoaded;
}

void CharonWorkflow::saveWorkflow() 
{
	writeWorkflowFile();
}

void CharonWorkflow::writeWorkflowFile()
{
	if( m_workflowLoaded ) {

		std::map<Slot*, std::set<Slot*>> connections;

		// disconnect all externally connected input slots
		for( std::map<std::string, std::map<std::string, Slot*>>::const_iterator iter = m_unconnectedInputSlots.begin(); iter != m_unconnectedInputSlots.end(); iter++) {
			// iterate over input slots of current plugin and check if they are unconnected
			for( std::map<std::string, Slot *>::const_iterator slotIter = iter->second.begin(); slotIter != iter->second.end(); slotIter++) {
				if( slotIter->second->connected() ) {
					connections[slotIter->second] = slotIter->second->getTargets(); // store connections for reconnecting					
					slotIter->second->disconnect();
				}
			}
		}

		// disconnect all externally connected output slots
		for( std::map<std::string, std::map<std::string, Slot*>>::const_iterator iter = m_unconnectedOutputSlots.begin(); iter != m_unconnectedOutputSlots.end(); iter++) {
			// iterate over input slots of current plugin and check if they are unconnected
			for( std::map<std::string, Slot *>::const_iterator slotIter = iter->second.begin(); slotIter != iter->second.end(); slotIter++) {
				if( slotIter->second->connected() ) {
					connections[slotIter->second] = slotIter->second->getTargets(); // store connections for reconnecting					
					slotIter->second->disconnect();
				}
			}
		}

		// disconnect all internal output slots that are connected to external plugins
		const std::map<std::string, ParameteredObject*> internalPlugins = m_pluginManager->getObjectList();
		for( std::map<std::string, std::map<std::string, Slot*>>::const_iterator iter = m_connectedOutputSlots.begin(); iter != m_connectedOutputSlots.end(); iter++) {

			// iterate over output slots of current plugin and check if they are connected to an external plugin
			for( std::map<std::string, Slot *>::const_iterator slotIter = iter->second.begin(); slotIter != iter->second.end(); slotIter++) {

				if( slotIter->second->connected() ) {
					foreach( Slot* target, slotIter->second->getTargets() ){
						if( internalPlugins.find( target->getParent().getName() ) == internalPlugins.end()) {
							connections[slotIter->second].insert(target); // store connection to external plugin for reconnecting
							slotIter->second->disconnect(*target);
						}				
					}
				}
			}
		}

		m_pluginManager->saveParameterFile(m_parameterFile);
		m_parameterFile.save( m_workflowFile.toStdString());
		Frapper::Log::debug( "Workflow saved to "+ m_workflowFile +"!");

		// reconnect collected slots
		for( std::map<Slot*, std::set<Slot*>>::const_iterator connIter = connections.begin(); connIter != connections.end(); ++connIter ) {
			for( std::set<Slot*>::iterator targetIter = connIter->second.begin(); targetIter != connIter->second.end(); ++targetIter) {
				connIter->first->connect( **targetIter );
			}
		}

	}
}

//!
//! Go though all input and output slots and sort by connected / unconnected / input / output
//!
void CharonWorkflow::calculateSlotLists()
{
	m_unconnectedInputSlots.clear();
	m_unconnectedOutputSlots.clear();
	m_connectedInputSlots.clear();
	m_connectedOutputSlots.clear();

	// iterate over all ParameteredObjects in workflow
	const std::map<std::string, ParameteredObject *>& objectsList = m_pluginManager->getObjectList();
	for( std::map<std::string, ParameteredObject *>::const_iterator it = objectsList.begin(); it != objectsList.end(); it++) 
	{

		// checks whether the plugin is active
		if( it->second->_active) 
		{
			// iterate over input slots of current plugin and sort by connected / unconnected
			const std::map<std::string, Slot *>& inputSlots = it->second->getInputSlots();
			for( std::map<std::string, Slot *>::const_iterator slotIter = inputSlots.begin(); slotIter != inputSlots.end(); slotIter++) 
			{
				if( slotIter->second->connected() )
					m_connectedInputSlots[ it->first ][slotIter->first] = slotIter->second;
				else
					m_unconnectedInputSlots[ it->first ][slotIter->first] = slotIter->second;
			}
			// iterate over output slots of current plugin and sort by connected / unconnected
			const std::map<std::string, Slot *>& outputSlots = it->second->getOutputSlots();
			for( std::map<std::string, Slot *>::const_iterator slotIter = outputSlots.begin(); slotIter != outputSlots.end(); slotIter++) 
			{
				if( slotIter->second->connected() )
					m_connectedOutputSlots[ it->first ][slotIter->first] = slotIter->second;
				else
					m_unconnectedOutputSlots[ it->first ][slotIter->first] = slotIter->second;
			}
		}
	}
}

std::map<std::string, std::map<std::string, AbstractParameter *>> CharonWorkflow::getParameters()
{
	std::map<std::string, std::map<std::string, AbstractParameter *>> parameterList;

	if( !m_pluginManager )
		return parameterList;

	// iterate over all ParameteredObjects in workflow
	const std::map<std::string, ParameteredObject *>& objectsList = m_pluginManager->getObjectList();
	for( std::map<std::string, ParameteredObject *>::const_iterator it = objectsList.begin(); it != objectsList.end(); it++) 
	{
		// checks whether the plugin is active
		if( it->second->_active) 
		{
			// Store parameter map by plugin name
			parameterList[ it->first ] = it->second->getParameters();
		}
	}

	return parameterList;
}

void CharonWorkflow::ExecuteWorkflow()
{
	if( !m_workflowLoaded ) {
		Frapper::Log::warning(QString("No workflow loaded."), "CharonWorkflow::ExecuteWorkflow");
		return;
	}

	// Change current path to working directory, so that charon can handle relative paths
	QString currentDir = QDir::currentPath();
	QDir::setCurrent( Frapper::SceneModel::getWorkingDirectory() );

	try {
		runWorkflow();
	}
	catch (const std::exception &msg) {

		QString errorMessage = QString( msg.what());
		Frapper::Log::error( "An Exception was thrown while executing workflow:", "CharonWorkflow::ExecuteWorkflow" );
		Frapper::Log::error( "Message: " + errorMessage, "CharonWorkflow::ExecuteWorkflow" );
		emit ExecutionError( errorMessage );
	}

	// restore to execution path for frapper
	QDir::setCurrent(currentDir);

	emit ExecutionFinished();
}

void CharonWorkflow::ResetWorkflow()
{
	if( !m_workflowLoaded ) {
		Frapper::Log::warning(QString("No workflow loaded."), "CharonWorkflow::ExecuteWorkflow");
		return;
	}

	// propagate reset
	const std::map<std::string, ParameteredObject *>& objectsList = m_pluginManager->getObjectList();
	for( std::map<std::string, ParameteredObject *>::const_iterator it = objectsList.begin(); it != objectsList.end(); it++) 
	{
		// checks whether the plugin is active
		if( it->second->_active) 
		{
			// see if this works, may be too much..
			it->second->resetExecuted(true);
		}
	}
}

void CharonWorkflow::ResetPlugin( QString pluginName )
{
	if( !m_workflowLoaded ) {
		Frapper::Log::warning(QString("No workflow loaded."), "CharonWorkflow::ExecuteWorkflow");
		return;
	}

	ParameteredObject* pPlugin = getPlugin(pluginName);
	if( NULL != pPlugin && pPlugin->_active ){
		pPlugin->resetExecuted(true);
	}
}

AbstractParameter* CharonWorkflow::getParameter( QString name, QString pluginName /*= QString() */ )
{
	if( !m_workflowLoaded ) {
		Frapper::Log::warning(QString("No workflow loaded."), "CharonWorkflow::ExecuteWorkflow");
		return NULL;
	}

	QString parameterName;
	QString estPluginName;
	splitParameterName( name, parameterName, estPluginName );

	if( pluginName.isEmpty() )
		pluginName = estPluginName;
	
	if( !pluginName.isEmpty() ) 
	{
		try
		{
			ParameteredObject* pPlugin = m_pluginManager->getInstance( pluginName.toStdString() );
			return &pPlugin->getParameter(parameterName.toStdString());
		}
		catch (const std::exception &msg) {
			Frapper::Log::error( QString( msg.what()), "CharonWorkflow::getParameter" );
			Frapper::Log::error( QString( "The Parameter \"%1\" on Plugin \"%2\" could not be found:").arg(name, pluginName), "CharonWorkflow::getParameter" );
		}
	}
	return NULL;
}

void CharonWorkflow::splitParameterName( const QString& pluginAndParameterName, QString& parameterName, QString& pluginName )
{
	// split at the first occurrence of a '.'
	int pos = pluginAndParameterName.indexOf(".");
	if( pos > 0) {
		pluginName    = pluginAndParameterName.left(pos);
		parameterName = pluginAndParameterName.mid(pos+1);
	} else {
		pluginName = pluginAndParameterName;
	}
}

ParameteredObject* CharonWorkflow::getPlugin( QString pluginName )
{
	if( !m_workflowLoaded ) {
		Frapper::Log::warning(QString("No workflow loaded."), "CharonWorkflow::getPlugin");
		return NULL;
	}

	if( !pluginName.isEmpty() ) 
	{
		try
		{
			ParameteredObject* pPlugin = m_pluginManager->getInstance( pluginName.toStdString() );
			return pPlugin;
		}
		catch (const std::exception &msg) {
			Frapper::Log::error( QString( msg.what()), "CharonWorkflow::getPlugin" );
			Frapper::Log::error( QString( "The Plugin \"%1\" could not be found:").arg(pluginName), "CharonWorkflow::getPlugin" );
		}
	}
	return NULL;
}

std::map<std::string, Slot*> CharonWorkflow::getPluginOutputSlots( QString pluginName )
{
	ParameteredObject* pPlugin = getPlugin(pluginName);
	if( pPlugin )
	{
		return pPlugin->getOutputSlots();
	}
	return std::map<std::string, Slot*>();
}

std::map<std::string, Slot*> CharonWorkflow::getPluginInputSlots( QString pluginName )
{
	ParameteredObject* pPlugin = getPlugin(pluginName);
	if( pPlugin )
	{
		return pPlugin->getInputSlots();
	}
	return std::map<std::string, Slot*>();
}

void CharonWorkflow::runWorkflow()
{
	if (m_targetPoints.empty()) {
		Frapper::Log::error( "Could not execute workflow:","CharonWorkflow::runWorkflow");
		Frapper::Log::error( "No valid target point found. Please check if ","CharonWorkflow::runWorkflow");
		Frapper::Log::error( "all required plugins could be loaded, then check if","CharonWorkflow::runWorkflow");
		Frapper::Log::error( "this is a valid parameter file.","CharonWorkflow::runWorkflow");
		return;
	}

	for( std::set<ParameteredObject*>::const_iterator iter = m_targetPoints.begin(); iter != m_targetPoints.end(); iter++) {
		(*iter)->run();
	}
}

void CharonWorkflow::determineTargetPoints()
{
	m_targetPoints.clear();
	std::map<std::string, ParameteredObject *> objects = m_pluginManager->getObjectList();
	std::map<std::string, ParameteredObject *>::const_iterator it;
	for (it = objects.begin(); it != objects.end(); it++) {
		if (it->second->_active){ // checks whether the plugin is active
			bool connected = false;
			bool activeTest = false;
			std::map<std::string, Slot *> outputslots =
				it->second->getOutputSlots();
			std::map<std::string, Slot *>::const_iterator slotIter;
			for (slotIter = outputslots.begin(); !connected && slotIter
				!= outputslots.end(); slotIter++) {
					connected = slotIter->second->connected();
					const std::set<Slot*>& listOfTargets = slotIter->second->getTargets();
					std::set<Slot*>::const_iterator tmp = listOfTargets.begin();
					for (; tmp!=listOfTargets.end();tmp++) {
						if ((*tmp)->getParent()._active){
							activeTest = true;
						}
					}
			}
			if (!connected || !(activeTest)) { // verifies whether the next plugin is active
				sout << "(DD) Found target point \"" << it->second->getName()
					<< "\"" << std::endl;
				m_targetPoints.insert(it->second);
			}
		}
	}
}

QString CharonWorkflow::getEditorComment( QString parameterName )
{
	std::string request = parameterName.toStdString()+".editorcomment";
	return QString::fromStdString( m_parameterFile.get<std::string>( request, ""));
}

CharonWorkflow::EditorPriority CharonWorkflow::getEditorPriority( QString parameterName )
{
	std::string request = parameterName.toStdString()+".editorpriority";
	return (CharonWorkflow::EditorPriority) m_parameterFile.get<int>( request, EP_Unset);
}

QString CharonWorkflow::getPluginDescription( QString pluginName )
{
	return getPluginMetadata(pluginName, ".doc");
}

QString CharonWorkflow::getPluginMetadata( QString pluginName, QString metadataName )
{
	ParameteredObject* po = getPlugin(pluginName);
	if( po ){
		const ParameterFile& metadata = po->getMetadata();
		try{
			const std::string& result = metadata.get<std::string>( po->getClassName() + metadataName.toStdString() );
			return QString::fromStdString( result );
		}
		catch ( std::exception& e) {
			Frapper::Log::debug("No description for plugin " +pluginName+ " found.", "CharonWorkflow::getParameterType");
		}
	}
	return QString();
}

QString CharonWorkflow::getParameterDescription( QString pluginName, QString parameterName )
{
	return getParameterMetadata(pluginName, parameterName, ".doc");
}

QString CharonWorkflow::getParameterType( QString pluginName, QString parameterName )
{
	return getParameterMetadata(pluginName, parameterName, ".type");
}

QString CharonWorkflow::getParameterDefaultValue( QString pluginName, QString parameterName )
{
	return getParameterMetadata(pluginName, parameterName, "");
}

QString CharonWorkflow::getParameterMetadata( QString pluginName, QString parameterName, QString metadataName )
{
	ParameteredObject* po = getPlugin(pluginName);
	if( po ){
		const ParameterFile& metadata = po->getMetadata();
		try{
			const std::string& result = metadata.get<std::string>( po->getClassName() + "." + parameterName.toStdString() + metadataName.toStdString() );
			return QString::fromStdString( result );
		}
		catch ( std::exception& e) {
			if( metadataName.isEmpty() ) 
				Frapper::Log::debug("No default value for parameter "+parameterName+" of plugin "+pluginName+" found.", "CharonWorkflow::getParameterMetadata");
			else
				Frapper::Log::debug("Metadata \""+metadataName+"\" for parameter "+parameterName+" of plugin "+pluginName+" found.", "CharonWorkflow::getParameterMetadata");

		}
	}
	return QString();
}


void CharonWorkflow::disconnectAllSlots()
{
	assert( m_pluginManager );

	// iterate over all ParameteredObjects in workflow
	const std::map<std::string, ParameteredObject *>& objectsList = m_pluginManager->getObjectList();
	for( std::map<std::string, ParameteredObject *>::const_iterator it = objectsList.begin(); it != objectsList.end(); it++) 
	{
		// checks whether the plugin is active
		if( it->second && it->second->_active)
		{
			// iterate over input slots of current plugin and check if they are unconnected
			const std::map<std::string, Slot *>& inputSlots = it->second->getInputSlots();
			for( std::map<std::string, Slot *>::const_iterator slotIter = inputSlots.begin(); slotIter != inputSlots.end(); slotIter++) 
			{
				if( slotIter->second && slotIter->second->connected() )
					slotIter->second->disconnect();
			}
			// iterate over input slots of current plugin and check if they are unconnected
			const std::map<std::string, Slot *>& outputSlots = it->second->getOutputSlots();
			for( std::map<std::string, Slot *>::const_iterator slotIter = outputSlots.begin(); slotIter != outputSlots.end(); slotIter++) 
			{
				if( slotIter->second && slotIter->second->connected() )
					slotIter->second->disconnect();
			}
		}
	}
}

void CharonWorkflow::setDefaultTemplateType( ParameteredObject::template_type templateType )
{
	m_templateType = templateType;
}

void CharonWorkflow::setCharonPath( QString charonPath )
{
	m_charonPath = charonPath;
}

const std::map<std::string, std::map<std::string, Slot*>>& CharonWorkflow::getConnectedInputSlots()
{
	return m_connectedInputSlots;
}

const std::map<std::string, std::map<std::string, Slot*>>& CharonWorkflow::getUnconnectedInputSlots()
{
	return m_unconnectedInputSlots;
}

const std::map<std::string, std::map<std::string, Slot*>>& CharonWorkflow::getConnectedOutputSlots()
{
	return m_connectedOutputSlots;
}

const std::map<std::string, std::map<std::string, Slot*>>& CharonWorkflow::getUnconnectedOutputSlots()
{
	return m_unconnectedOutputSlots;
}

void CharonWorkflow::setActive( QString pluginName, bool active )
{
	// Disabled, causes major problems in the charon execution chain!

	//ParameteredObject* plugin = getPlugin(pluginName);
	//if( plugin ){
	//	plugin->setExecuted(!active);
	//}
}

} // end namespace
