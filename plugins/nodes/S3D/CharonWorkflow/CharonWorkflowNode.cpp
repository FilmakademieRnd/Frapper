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
//! \file "CharonWorkflowNode.cpp"
//! \brief Implementation file for CharonWorkflowNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       12.08.2013 (created)
//! \date       12.08.2013 (last updated)
//!


#include "CharonWorkflowNode.h"
#include "GenericParameter.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QProcess>
#include <QtCore/QTextStream>
#include <QtGui/QProgressDialog>
#include <QtGui/QMessageBox>
#include <QDir>
#include "LogProgressDialog.h"
#include "SceneModel.h"
#include "Connection.h"
#include <QMessageBox>
#include <FilenameParameter.h>

Q_DECLARE_METATYPE(Slot *);


namespace CharonWorkflowNode {

///
/// Constructors and Destructors
///

 
//!
//! Constructor of the CharonWorkflowNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
CharonWorkflowNode::CharonWorkflowNode ( const QString &name, Frapper::ParameterGroup *parameterRoot, Frapper::NumberParameter* timeParameter ) :
	Frapper::Node(name, parameterRoot),
	pluginInstanceCounter(0),
	m_workflow(0),
	m_workflowModified(false),
	m_executionThread(0),
	m_charonPath(""), m_workflowFile(""), m_templateType(-1),
	m_sceneLoading(false),
	m_inputFilesExists(true)
{
	m_timeParameter = timeParameter;

	InitializeParameters();

	// create & initialize charon workflow object
	m_workflow = new CharonWorkflow();
	m_workflow->setDefaultTemplateType(ParameteredObject::TYPE_FLOAT);

	// initialize charon plugin manager with given charon path
	m_workflow->setCharonPath(m_charonPath);

	m_executionThread = new QThread;
	m_workflow->moveToThread(m_executionThread);

	// let workflow execution be handled by the execution thread
	connect( m_executionThread, SIGNAL( started()), m_workflow, SLOT( ExecuteWorkflow() ));
	connect( m_workflow, SIGNAL( ExecutionFinished() ), m_executionThread, SLOT( quit() ));
	connect( m_workflow, SIGNAL( ExecutionError(QString) ), this, SLOT( handleExecutionError(QString) ));

#ifdef _DEBUG
	parameterRoot->addParameter( new Frapper::Parameter("savedoc", Frapper::Parameter::T_Command, QVariant()));
	setCommandFunction("savedoc", SLOT(savedoc()));
#endif

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the CharonWorkflowNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CharonWorkflowNode::~CharonWorkflowNode ()
{
}


void CharonWorkflowNode::loadCharonWorkflow()
{
	QString workflowFile = getStringValue("Setup > Workflow File");
	
	// Check if given workflow file is valid
	if( workflowFile.isEmpty() || !QFileInfo(workflowFile).exists() || !workflowFile.endsWith(".wrp", Qt::CaseInsensitive )) {
		Frapper::Log::warning("The given workflow file name is empty or does not specify a charon workflow file.", "CharonWorkflowNode::loadCharonWorkflow");
		return;
	}

	// check if workflow file is different from to current one
	if( m_workflowFile != workflowFile ) {

		// thats our new workflow file!
		m_workflowFile = workflowFile;

		// remove "altlasten"
		saveExternalConnections();
		removeCharonParameters();
		removeCharonSlots();

		// see if we can work with the new one..
		if( m_workflow->loadWorkflowFile( workflowFile )) {

			//Node::setName( QFileInfo(charonWorkflow).baseName() ); // creates lots of trouble, needs to be done manually via GUI :(
			createCharonSlots();
			createCharonParameters();
			restoreExternalConnections();
		}
		
		emit nodeChanged();
		forcePanelUpdate ();
	}
}

void CharonWorkflowNode::reloadCharonWorkflow()
{
	if( m_workflowModified )
	{
		// display save? message
		QMessageBox saveMsgBox;
		saveMsgBox.setWindowTitle("Reload Workflow");
		saveMsgBox.setText("Save changes of workflow to "+m_workflowFile+"?");
		saveMsgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
		saveMsgBox.setDefaultButton(QMessageBox::No);

		if( saveMsgBox.exec() == QMessageBox::Yes ) {
			saveStarted();
		}
	}

	m_workflowFile = "";
	loadCharonWorkflow();
}

void CharonWorkflowNode::createCharonParameters()
{
	bool showAdvanced = getBoolValue("Setup > Show Advanced Parameters");

	// reset file existing flag
	m_inputFilesExists = true;

	m_startFrameParameters.clear();
	m_endFrameParameters.clear();
	m_inFrameParameters.clear();
	m_outFrameParameters.clear();

	// iterate over plugins in loaded workflow
	const std::map<std::string, std::map<std::string, AbstractParameter *>>& workflowParameters = m_workflow->getParameters();
	for( std::map<std::string, std::map<std::string, AbstractParameter*>>::const_iterator Iter = workflowParameters.begin(); Iter != workflowParameters.end(); Iter++ ) {
		
		// gather parameters of current charon plugin
		QString pluginName = QString::fromStdString( Iter->first);
		m_charonPlugins.insert(pluginName);

		Frapper::ParameterGroup* pluginGroup = getPluginGroup(pluginName, getParameterRoot() );

		// create node description parameter
		Frapper::Parameter* nodeDescriptionParameter = 0;
		if( !nodeDescriptionParameter) {
			QString pluginDescription = m_workflow->getPluginDescription(pluginName);
			QString pluginComment = m_workflow->getEditorComment( pluginName );

			if( !pluginComment.isEmpty() ){
				pluginDescription += "\n\nComment: " + pluginComment;
			}

			if( !pluginDescription.isEmpty() ) {
				nodeDescriptionParameter = new Frapper::Parameter( pluginName+".description", Frapper::Parameter::T_TextInfo, pluginDescription );
				pluginGroup->addParameter( nodeDescriptionParameter );
			}
		}

		// Group for advanced parameters, created as needed
		Frapper::ParameterGroup* advancedGroup  = 0;

		// check if there are any parameters created in this group after processing plugin
		bool emptyPluginGroup = true;

		std::map<std::string, AbstractParameter*> pluginParameters = Iter->second;
		std::map<std::string, AbstractParameter*>::const_iterator pluginParametersIter;
		
		// iterate over parameters of current plugin
		for( pluginParametersIter = pluginParameters.begin(); pluginParametersIter != pluginParameters.end(); pluginParametersIter++ ) 
		{
			const QString &parameterName       = pluginParametersIter->first.c_str();
			const QString &frapperName         = pluginName + '.' + parameterName;
			const QString &type                = pluginParametersIter->second->guessType().c_str();
			const AbstractParameter* parameter = pluginParametersIter->second;

			// check if parameter was already created, e.g. by loading a scene
			Frapper::Parameter *frapperParameter = pluginGroup->getParameter( frapperName );
		
			// delete existing parameters, e.g. from scene loading 
			// values are obtained from charon workflow
			if( frapperParameter ) {
				pluginGroup->removeParameter(frapperParameter);
				frapperParameter = NULL;
			}
	 
			// bool values
			if (type == "bool")
			{
				frapperParameter = new Frapper::Parameter( frapperName, Frapper::Parameter::T_Bool, getValue<bool>(parameter) );
			}
			// integer values
			else if (type == "int") 
			{
				frapperParameter = new Frapper::NumberParameter( frapperName, Frapper::Parameter::T_Int, getValue<int>(parameter) );
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setMaxValue( INT_MAX);
			}
			else if (type == "uint") {
				frapperParameter = new Frapper::NumberParameter( frapperName, Frapper::Parameter::T_UnsignedInt, getValue<unsigned int>(parameter));
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setMaxValue( INT_MAX );
			}
			// floating point values
			else if (type == "float") {
				frapperParameter = new Frapper::NumberParameter( frapperName, Frapper::Parameter::T_Float, getValue<float>(parameter));
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setInputMethod(Frapper::NumberParameter::IM_SpinBox);
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setMaxValue(FLT_MAX);
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setStepSize(0.001);
			}
			// double precision values
			else if (type == "double") {
				frapperParameter = new Frapper::NumberParameter( frapperName, Frapper::Parameter::T_Float, getValue<double>(parameter));
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setInputMethod(Frapper::NumberParameter::IM_SpinBox);
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setMaxValue(FLT_MAX);
				static_cast<Frapper::NumberParameter *>(frapperParameter)->setStepSize(0.001);
			}
			// strings
			else if (type == "string") {

				const QString typeString = m_workflow->getParameterType(pluginName, parameterName);

				QString parameterValue = QString::fromStdString( getValue<std::string>(parameter));
				parameterValue = Frapper::FilenameParameter::relativeFilePath(parameterValue);
				
				// check if parameter name may indicate of file parameter
				//if( parameterName.contains("path", Qt::CaseInsensitive)) {
				if( typeString.contains("path", Qt::CaseInsensitive)) {

					frapperParameter = new Frapper::FilenameParameter(frapperName, parameterValue );
					static_cast<Frapper::FilenameParameter*>(frapperParameter)->setType(Frapper::FilenameParameter::FT_Dir);

				//} else if( parameterName.contains("file", Qt::CaseInsensitive) ) {
				} else if( typeString.contains("file", Qt::CaseInsensitive) ) {

					frapperParameter = new Frapper::FilenameParameter(frapperName, parameterValue );
					
					if( typeString.contains("open", Qt::CaseInsensitive) || typeString.contains("name", Qt::CaseInsensitive)) {
						static_cast<Frapper::FilenameParameter*>(frapperParameter)->setType(Frapper::FilenameParameter::FT_Open);
					} else {
						static_cast<Frapper::FilenameParameter*>(frapperParameter)->setType(Frapper::FilenameParameter::FT_Save);
					}
				}
				else
					frapperParameter = new Frapper::Parameter( frapperName, Frapper::Parameter::T_String, parameterValue);
			}

			// string lists
			else if ( type == "string list" || type == "T_List") {
				QStringList values;
				foreach( std::string val, getValueList<std::string>(parameter))
					values.append( QString::fromStdString(val));
				frapperParameter = new Frapper::Parameter( frapperName, Frapper::Parameter::T_String, values.join(";"));
			}

			// lists
			else if( type.contains("list")) {
				QStringList values;
				if( type.contains("float") || type.contains("double")) {
					foreach( float val, getValueList<float>(parameter))
						values.append( QString::number(val));
				}
				else if( type.contains("int")) {
					foreach( int val, getValueList<int>(parameter))
						values.append( QString::number(val));
				}
				else if( type.contains("uint")) {
					foreach( uint val, getValueList<uint>(parameter))
						values.append( QString::number(val));
				}
				frapperParameter = new Frapper::Parameter( frapperName, Frapper::Parameter::T_String, values.join(";"));
			}
			else {
				Frapper::Log::error( QString("Could not create Frapper parameter for \"%1\": Type \"%2\" unknown!").arg(frapperName, type), "CharonWorkflowNode::createCharonParameters" );
			}


			// add the parameter to the plugin parameter group
			if (frapperParameter)
			{
				// create parameter description
				QString parameterDescription = m_workflow->getParameterDescription(pluginName, parameterName);
				QString parameterComment = m_workflow->getEditorComment( pluginName+'.'+parameterName );
				QString parameterType    = m_workflow->getParameterType( pluginName, parameterName);
				QString parameterDefault = m_workflow->getParameterDefaultValue(pluginName, parameterName);

				if( !parameterType.isEmpty()){
					parameterDescription += "\n\ntype: " + parameterType;
				}
				if( !parameterDefault.isEmpty()){
					parameterDescription += "\nDefault Value: " + parameterDefault;
				}
				if( !parameterComment.isEmpty() ){
					parameterDescription += "\nComment: " + parameterComment;
				}
				if( !parameterDescription.isEmpty() ) {
					frapperParameter->setDescription(parameterDescription);
				}

				// get parameter priority from workflow
				CharonWorkflow::EditorPriority priority = m_workflow->getEditorPriority( frapperName);

				// watch out for special parameters..
				if( parameterName == "startFrame" ) {
					m_startFrameParameters.push_back( dynamic_cast<Frapper::NumberParameter*>(frapperParameter));
					frapperParameter->setVisible( CharonUseDebug ); // show only in debug mode
					priority = CharonWorkflow::EP_Low;
				} else if( parameterName == "endFrame" ) {
					m_endFrameParameters.push_back( dynamic_cast<Frapper::NumberParameter*>(frapperParameter));
					frapperParameter->setVisible( CharonUseDebug );
					priority = CharonWorkflow::EP_Low;
				} else if( parameterName == "inFrame" ) {
					m_inFrameParameters.push_back( dynamic_cast<Frapper::NumberParameter*>(frapperParameter));
					frapperParameter->setVisible( CharonUseDebug );
					priority = CharonWorkflow::EP_Low;
				} else if( parameterName == "outFrame" ) {
					m_outFrameParameters.push_back( dynamic_cast<Frapper::NumberParameter*>(frapperParameter));
					frapperParameter->setVisible( CharonUseDebug );
					priority = CharonWorkflow::EP_Low;
				}

				// insert parameter by its priority ..
				
				// high comes first, but after description parameter
				if( priority == CharonWorkflow::EP_High ) {
					if( nodeDescriptionParameter ) {
						pluginGroup->addParameterAfter( frapperParameter, nodeDescriptionParameter);
					} else {
						pluginGroup->addParameter( frapperParameter, true); // high parameter always on top!
					}
				
				// lower parameters after high, but before advanced group
				} else if( priority > CharonWorkflow::EP_Unset ) {
					pluginGroup->addParameterBefore(frapperParameter, advancedGroup );

				// parameters with no priority (default) are added to advanced group (hidden by default)
				} else if( showAdvanced ) {
					// create advanced group as needed
					if( !advancedGroup ) {
						advancedGroup  = new Frapper::ParameterGroup( "Advanced" );
						pluginGroup->addParameter(advancedGroup);
					}
					advancedGroup->addParameter(frapperParameter);

				// don't show parameter
				} else {
					delete frapperParameter;
					frapperParameter = 0;
				}


				// add change function and affection for frapper parameter
				if( frapperParameter ) {

					emptyPluginGroup = false;

					frapperParameter->setChangeFunction(SLOT(changeCharonParameter()));
					//frapperParameter->setPinType(Frapper::Parameter::PT_Input);
					//frapperParameter->setProcessingFunction(SLOT(changeCharonParameter()));

					// create affection between charon/frapper parameter and output slots
					foreach( QString outSlotName, m_charonSlotsOut ) {
						Frapper::Parameter* outSlot = getParameter(outSlotName);
						if( outSlot ){ 
							frapperParameter->addAffectedParameter( outSlot );
							//frapperParameter->addAffectedParameter( getParameter("Process"));
						}
					}

					// check for file parameters if file exists
					checkFileExists(frapperParameter, pluginName);

				} // end if frapper parameter
			} // end if frapper parameter
		} // end foreach plugin parameter

		if( emptyPluginGroup ) {
			pluginGroup->removeParameter( nodeDescriptionParameter, true, true );
			getParameterRoot()->removeParameterGroup( pluginGroup->getName() );
		}
	}

	// setup time dependencies after all parameters were loaded
	setUpTimeDependencies(m_timeParameter, m_rangeParameter );
}

void CharonWorkflowNode::removeCharonParameters()
{

	foreach( QString pluginGroupName, m_charonPlugins )
	{
		Frapper::ParameterGroup* pluginGroup = getParameterGroup(pluginGroupName);
		if( NULL != pluginGroup )
		{
			// remove affections
			foreach( Frapper::AbstractParameter* absParameter, pluginGroup->getParameterList()){
				Frapper::Parameter* parameter = dynamic_cast<Frapper::Parameter*>(absParameter);
				if( parameter ) {
					foreach( QString outSlotName, m_charonSlotsOut ) {
						Frapper::Parameter* outSlot = getParameter(outSlotName);
						if( outSlot ) {
							parameter->removeAffectedParameter( outSlot );
							//parameter->removeAffectedParameter( getParameter("Process"));
						}
					}
				}
			}
			pluginGroup->destroyAllParameters();
			getParameterRoot()->removeParameterGroup(pluginGroupName);
		}
	}

	m_charonPlugins.clear();
}

// write changes in frapper parameters back to charon paramters
void CharonWorkflowNode::changeCharonParameter()
{
	Frapper::Parameter *intParameter = dynamic_cast<Frapper::Parameter *>(sender());

	QString parameterName;
	QString pluginName;

	if (intParameter) try {

		parameterName = intParameter->getName();
		pluginName    = getPluginName(parameterName);
		AbstractParameter* chParameter = m_workflow->getParameter( parameterName );
		
		if( chParameter) {

			const QString  charonType           = QString::fromStdString( chParameter->guessType());
			const QVariant value                = intParameter->getValue();
			const Frapper::Parameter::Type type = intParameter->getType();

			switch (type) 
			{
			case Frapper::Parameter::T_Bool:
				setValue<bool>( chParameter, value.toBool());
				break;
			case Frapper::Parameter::T_Int:
				setValue<int>(chParameter, value.toInt());
				break;
			case Frapper::Parameter::T_UnsignedInt:
				setValue<uint>(chParameter, value.toUInt());
				break;
			case Frapper::Parameter::T_Float:
				if (charonType == "double")
					setValue<double>(chParameter, value.toDouble() );
				else if (charonType == "float")
					setValue<float>(chParameter, value.toFloat() );
				break;
			case Frapper::Parameter::T_Filename:
				setValue<std::string>(chParameter, Frapper::FilenameParameter::relativeFilePath(value.toString()).toStdString());
				checkFileExists(intParameter, pluginName );
				break;
			case Frapper::Parameter::T_String:
				if( charonType.contains("list", Qt::CaseInsensitive)) 
				{
					if (charonType == "float list") {
						setValueList<float>( chParameter, value.toString());
					}
					else if (charonType == "double list") {
						setValueList<double>( chParameter, value.toString());
					}
					else if (charonType == "int list") {
						setValueList<int>( chParameter, value.toString());
					}
					else if (charonType == "uint list") {
						setValueList<uint>( chParameter, value.toString());
					}
					else if (charonType == "string list") {
						setValueList<std::string>( chParameter, value.toString());
					}
				} 
				else if (charonType == "string") {
					setValue<std::string>( chParameter, value.toString().toStdString());
				}
				break;
			case Frapper::Parameter::T_Color: {
				QColor &color = intParameter->getValue().value<QColor>();
				std::vector<uint> values;
				values.push_back(color.red()); values.push_back(color.green()); values.push_back(color.blue());
				setValueList<uint>(chParameter, values);
				break;
				}
			}

			// don't change modified flag during scene load
			if( !m_sceneLoading )
				m_workflowModified = true;

			// reset execution of plugin
			m_workflow->ResetPlugin(pluginName);
		}
	} catch (std::exception& e) {
		Frapper::Log::error( "An Exception was thrown while changing charon parameter "+parameterName+" of plugin "+pluginName, "CharonWorkflowNode::changeCharonParameter");
		Frapper::Log::error( QString(e.what()), "CharonWorkflowNode::changeCharonParameter");
	}	
}

void CharonWorkflowNode::createCharonSlots()
{
	// parameter group to add slot parameters to
	Frapper::ParameterGroup *rootGroup  = getParameterRoot();
	
	// get all input slots grouped by plugin name
	const std::map<std::string, std::map<std::string, Slot*>>& pluginInputSlots = m_workflow->getUnconnectedInputSlots();
	for( std::map<std::string, std::map<std::string, Slot*>>::const_iterator Iter = pluginInputSlots.begin(); Iter != pluginInputSlots.end(); Iter++ )
	{
		// current charon plugin name
		QString pluginName = QString::fromStdString( Iter->first);

		// input slots
		std::map<std::string, Slot*> inSlots = Iter->second;

		// define group to add parameter to
		Frapper::ParameterGroup* pluginGroup = rootGroup;

		for (std::map<std::string, Slot*>::const_iterator inSlotsIter = inSlots.begin(); inSlotsIter != inSlots.end(); ++inSlotsIter) 
		{
			// read values of charon slots
			const QString &name = pluginName + '.' + inSlotsIter->first.c_str();
			const QString &type = inSlotsIter->second->getType().c_str();
			assert( type != "NO TYPE");
			Slot*		  slot =  inSlotsIter->second;
			assert(slot);
			createSlotParameter( pluginGroup, name, type, Frapper::Parameter::PT_Input, slot );

			m_charonSlotsIn.insert( name );
		}
	}

	// get all output slots grouped by plugin name
	const std::map<std::string, std::map<std::string, Slot*>>& pluginOutputSlots = m_workflow->getUnconnectedOutputSlots();
	for( std::map<std::string, std::map<std::string, Slot*>>::const_iterator Iter = pluginOutputSlots.begin(); Iter != pluginOutputSlots.end(); Iter++ )
	{
		// current charon plugin name
		QString pluginName = QString::fromStdString( Iter->first);

		// output slots
		std::map<std::string, Slot*> outSlots = Iter->second;

		// define group to add parameter to
		Frapper::ParameterGroup* pluginGroup = rootGroup;

		for (std::map<std::string, Slot*>::iterator outSlotsIter = outSlots.begin(); outSlotsIter != outSlots.end(); ++outSlotsIter) 
		{
			// read values of charon slots
			const QString &name = pluginName + '.' + outSlotsIter->first.c_str();
			const QString &type = outSlotsIter->second->getType().c_str();
			assert( type != "NO TYPE");
			Slot*		  slot  = outSlotsIter->second;
			assert(slot);
			createSlotParameter( pluginGroup, name, type, Frapper::Parameter::PT_Output, slot );

			m_charonSlotsOut.insert( name );
		}
	}

	// get all output slots grouped by plugin name
	const std::map<std::string, std::map<std::string, Slot*>>& pluginInternalOutputSlots = m_workflow->getConnectedOutputSlots();
	for( std::map<std::string, std::map<std::string, Slot*>>::const_iterator Iter = pluginInternalOutputSlots.begin(); Iter != pluginInternalOutputSlots.end(); Iter++ )
	{
		QString pluginName = QString::fromStdString( Iter->first);

		// internal output slots
		std::map<std::string, Slot*> outSlots = Iter->second;

		// define group to add parameter to
		Frapper::ParameterGroup* pluginGroup = getPluginGroup( "Internal Output Slots", rootGroup);
		m_slotOutputGroups.insert( "Internal Output Slots" );

		for (std::map<std::string, Slot*>::iterator outSlotsIter = outSlots.begin(); outSlotsIter != outSlots.end(); ++outSlotsIter) 
		{
			// read values of charon slots
			const QString &name = pluginName + '.' + outSlotsIter->first.c_str();
			const QString &type = outSlotsIter->second->getType().c_str();
			assert( type != "NO TYPE");
			Slot*		  slot  = outSlotsIter->second;
			assert(slot);
			createSlotParameter( pluginGroup, name, type, Frapper::Parameter::PT_Output, slot, false);

			m_charonSlotsOut.insert( name );
		}
	}

	// affections
	Q_FOREACH( QString outParamName, m_charonSlotsOut )
	{
		Frapper::Parameter *outParameter = getParameter( outParamName );
		
		if( outParameter  && !outParameter->isGroup()) {

			Q_FOREACH( QString inParamName, m_charonSlotsIn ) {
				Frapper::Parameter *inParameter = getParameter( inParamName );

				if( inParameter ) {
					// add affection between input and output slot
					inParameter->addAffectedParameter(outParameter);
					//inParameter->addAffectedParameter( getParameter("Process"));
				}
			}
		}
	}
}

void CharonWorkflowNode::removeCharonSlots()
{
	// remove charon slots
	foreach ( QString slotName, m_charonSlotsIn) {
		Frapper::Node::removeParameter( slotName );
		assert( Frapper::Node::getParameter(slotName) == 0);
	}
	m_charonSlotsIn.clear();

	foreach ( QString slotName, m_charonSlotsOut) {
		Frapper::Node::removeParameter( slotName );
		assert( Frapper::Node::getParameter(slotName) == 0);
	}
	m_charonSlotsOut.clear();

	// remove slot parameter groups
	Frapper::ParameterGroup* rootGroup = getParameterRoot();
	
	foreach ( QString slotGroupName, m_slotOutputGroups) {
		rootGroup->removeParameterGroup(slotGroupName);
		assert( Frapper::Node::getParameterGroup(slotGroupName) == 0);
	}
	m_slotOutputGroups.clear();
}

void CharonWorkflowNode::Process ()
{
	Frapper::Log::debug("Processing "+m_workflowFile, "CharonWorkflowNode::Process");

	// prevent automatic processing triggered by the in/out parameters
	Frapper::Parameter* senderParam = dynamic_cast<Frapper::Parameter*>(sender());
	
	// request update of frapper slot in parameters
	foreach( QString parameterName, m_charonSlotsIn ){
		Frapper::Parameter* parameter = getParameter(parameterName);
		if( parameter )
			parameter->getValue(true);
	}

	ProcessRequested();
}

void CharonWorkflowNode::Reset ()
{
	m_workflow->ResetWorkflow();

	foreach( QString slotName, m_charonSlotsOut) {
		Frapper::Parameter* slot = getParameter(slotName);
		if( slot ){
			slot->propagateDirty(true);
		}
		//getParameter("Process")->propagateDirty(true);
	}
}

void CharonWorkflowNode::charonPathChanged()
{
	Frapper::FilenameParameter* charonPathParameter = dynamic_cast<Frapper::FilenameParameter*>(sender());
	if( charonPathParameter ) {

		QString charonPath = charonPathParameter->getValue().toString();

		// check if given path is valid
		if( !charonPath.isEmpty() && QDir().exists( charonPath+"/bin" ) ) {

			// check if different path chosen
			if( m_charonPath != charonPath ){
				m_charonPath = charonPath;

				// set net charon path and reload charon workflow
				m_workflow->setCharonPath(charonPath);
				reloadCharonWorkflow();
			}
		}
	}
}

void CharonWorkflowNode::changeTemplateType ()
{
	int templateTypeId = getEnumerationParameter("Setup > Template Type")->getCurrentIndex();

	if( m_templateType != templateTypeId ) {
		m_templateType = templateTypeId;
		m_workflow->setDefaultTemplateType((ParameteredObject::template_type) templateTypeId);
		
		reloadCharonWorkflow();
	}
}

void CharonWorkflowNode::connectSlots ()
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	if( inParameter && inParameter->getPinType() == Frapper::Parameter::PT_Input )
	{
		try	{

			const QString &sourceName = inParameter->getName();
			const int nbrConnections  = inParameter->getConnectionMap().size();
			Slot *destSlot            = inParameter->getValue().value<Slot*>();
			assert(destSlot);

			for( int i=0; i<nbrConnections; ++i) 
			{
				Frapper::GenericParameter* connectedParameter = dynamic_cast<Frapper::GenericParameter *>(inParameter->getConnectedParameter( i ));
			
				if( !connectedParameter)
					continue;

				Slot *sourceSlot = qvariant_cast<Slot*>( connectedParameter->getValue());
				if( inParameter->isConnected() && sourceSlot && destSlot) 
				{
					const QString& srcType = inParameter->getTypeInfo().toLower();
					const QString& dstType = connectedParameter->getTypeInfo().toLower();
					if( srcType != dstType ) 
					{
						Frapper::Log::error(QString("In and Out -Slots have different types."), "CharonWorkflowNode::connectSlots");
						Frapper::Log::error( "Source type: "+srcType+" Destination type: "+dstType, "CharonWorkflowNode::connectSlots");
						removeFrapperConnection(inParameter, connectedParameter);
					}
					else 
					{
						try {
							destSlot->connect(*sourceSlot);
							sourceSlot->connect(*destSlot);
						} catch (std::exception& e) {
							Frapper::Log::error( QString(e.what()), "CharonWorkflowNode::connectSlots");
							removeFrapperConnection(inParameter, connectedParameter);
						}				
					}
				}
				else
				{
					Frapper::Log::error(QString("Parameter not connected!"), "CharonWorkflowNode::connectSlots");
					removeFrapperConnection(inParameter, connectedParameter);
				}
			}
		} catch (std::exception& e) {
			Frapper::Log::error( QString(e.what()), "CharonWorkflowNode::connectSlots");
			return;
		}
	}
}

void CharonWorkflowNode::disconnectSlots (int connectionID)
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	if( inParameter) 
	{		
		QString destName = inParameter->getName();
		QString sourceName = "";
	
		try{

			Frapper::Parameter *connectedParameter = inParameter->getConnectedParameterById(connectionID);

			if( connectedParameter) {

				sourceName = connectedParameter->getName();
				Slot *destSlot = qvariant_cast<Slot*>( inParameter->getValue());

				if( destSlot ) {

					std::set<Slot*> &aktSlots = destSlot->getTargets();
					for( std::set<Slot*>::iterator iter=aktSlots.begin(); iter!=aktSlots.end(); ++iter) 
					{
						Slot *slot = *iter;
						QString slotName = QString::fromStdString( slot->getParent().getName() + '.' + slot->getName());
						if (slotName == sourceName ) {
							Frapper::Log::debug(QString("Disconnect %1|%2...").arg(destSlot->getName().c_str()).arg(slot->getName().c_str()), "CharonWorkflowNode::disconnectSlots");
							if (!destSlot->disconnect(*slot))
								Frapper::Log::error(QString("disconnecting: %1|%2").arg(destSlot->getName().c_str()).arg(slot->getName().c_str()), "CharonWorkflowNode::disconnectSlots");
							else
								Frapper::Log::debug(QString("...done!"), "CharonInterface::disconnectSlots");
						}
					}

				} else {
					Frapper::Log::error("Error disconnecting " + destName + " from " + sourceName + "!", "CharonWorkflowNode::disconnectSlots");
					Frapper::Log::error("Destination Slot is NULL!", "CharonWorkflowNode::disconnectSlots");				
				}

			} else {
				Frapper::Log::error("Error disconnecting " + destName + "!", "CharonWorkflowNode::disconnectSlots");
				Frapper::Log::error("Connected Parameter not found!", "CharonWorkflowNode::disconnectSlots");						
			}

		} catch (std::exception& e) {
			Frapper::Log::error("Error disconnecting " + destName + " from " + sourceName + "!", "CharonWorkflowNode::disconnectSlots");
			Frapper::Log::error( QString(e.what()), "CharonWorkflowNode::disconnectSlots");
		}
	}
}

QString CharonWorkflowNode::estimateCharonPath( QString envVariableName /*= "CHARON_HOME"*/ )
{
	std::string ccEnvVarName = envVariableName.toStdString();
	//QString expandedPath = QString::fromStdString( std::getenv(ccEnvVarName)).replace('\\','/');
	QString expandedPath = QString( qgetenv( ccEnvVarName.c_str() ).constData() ).replace('\\','/');
	

	QDir charonPath(expandedPath);
	if (!charonPath.exists()) {
		Frapper::Log::warning(QString("Charon path could not be estimated! Please specify as environment variable CHARON_HOME"), "CharonWorkflowNode::estimateCharonPath");
		return QString();
	} else {
		return charonPath.absolutePath();
	}
}

void CharonWorkflowNode::InitializeParameters()
{

	// Create parameters for processing and resetting the charon plugin
	Frapper::Parameter *calculateParameter = getParameter("Calculate");
	if( !calculateParameter ) {
		calculateParameter = new Frapper::Parameter("Calculate", Frapper::Parameter::T_Command, 0);
		getParameterRoot()->addParameter(calculateParameter);
	}

	Frapper::Parameter *resetParameter = getParameter("Reset");
	if( !resetParameter ) {
		resetParameter = new Frapper::Parameter("Reset", Frapper::Parameter::T_Command, 0);
		getParameterRoot()->addParameter(resetParameter);
	}

	// "Setup" parameter group
	Frapper::ParameterGroup* setupGroup = getParameterGroup("Setup");
	if( !setupGroup ) {
		setupGroup = new Frapper::ParameterGroup( "Setup" );
		getParameterRoot()->addParameter( setupGroup);
	}

	// Create "Workflow File" parameter
	Frapper::FilenameParameter *workflowFileParameter = dynamic_cast<Frapper::FilenameParameter *>( setupGroup->getParameter("Workflow File"));
	if( !workflowFileParameter ) {
		workflowFileParameter = new Frapper::FilenameParameter("Workflow File", QVariant(""));
		workflowFileParameter->setFilters("Charon Workflows (*.wrp)");
		setupGroup->addParameter(workflowFileParameter);
	}

	// Create ""Edit in Tuchulcha"" parameter
	Frapper::Parameter *editParameter = dynamic_cast<Frapper::Parameter *>( setupGroup->getParameter("Edit in Tuchulcha"));
	if( !editParameter ) {
		editParameter = new Frapper::Parameter("Workflow File", Frapper::Parameter::T_Command, Frapper::Parameter::getDefaultValue(Frapper::Parameter::T_Command));
		setupGroup->addParameter(editParameter);
	}

	// Create "Show Advanced Parameters" parameter
	Frapper::Parameter *showAdvParameter = setupGroup->getParameter("Show Advanced Parameters");
	if( !showAdvParameter ) {
		showAdvParameter = new Frapper::Parameter("Show Advanced Parameters", Frapper::Parameter::T_Bool, false);
		setupGroup->addParameter(showAdvParameter);
	}

	// Create the template type parameter
	Frapper::EnumerationParameter *templateTypeParameter = dynamic_cast<Frapper::EnumerationParameter *>( setupGroup->getParameter("Template Type") );
	if( !templateTypeParameter ) {
		templateTypeParameter = new Frapper::EnumerationParameter("Template Type", "float");
		templateTypeParameter->setLiterals( QStringList() << "double" << "float" << "int");
		templateTypeParameter->setValues( QStringList() << "0" << "1" << "2");
		templateTypeParameter->setEnabled(false);
		setupGroup->addParameter(templateTypeParameter);
	}

	// Create "Charon Path" parameter
	QString charonPath = estimateCharonPath("CHARON_HOME");
	
	if( !charonPath.isEmpty() && QDir().exists(charonPath+"/bin") ) {

		// Initially use Charon Path as set in CHARON_HOME environment variable
		Frapper::Log::debug("Using Charon libraries from CHARON_HOME: "+charonPath, "CharonWorkflowNode::Initialize");
		m_charonPath = charonPath;

	} else {

		// create parameter for charon path in setup
		Frapper::FilenameParameter* charonPathParameter = new Frapper::FilenameParameter( "Charon Path", QVariant(charonPath));
		setupGroup->addParameter(charonPathParameter, true);
		charonPathParameter->setType(Frapper::FilenameParameter::FT_Dir);
		charonPathParameter->setDescription("The path to the Charon root folder which includes the bin folder.");
		charonPathParameter->setChangeFunction( SLOT(charonPathChanged()));
	}

	// set button command functions	
	calculateParameter    ->setCommandFunction( SLOT(ProcessRequested()) );
	resetParameter        ->setCommandFunction( SLOT(Reset()) );
	templateTypeParameter ->setChangeFunction(  SLOT(changeTemplateType()) );
	showAdvParameter      ->setChangeFunction(  SLOT(updateCharonParameters()));

	workflowFileParameter->setChangeFunction(  SLOT( loadCharonWorkflow()) );
	workflowFileParameter->setCommandFunction( SLOT( reloadCharonWorkflow()) );

	editParameter->setCommandFunction( SLOT( openInTuchulcha()) );

	m_templateType = templateTypeParameter->getValue().toInt();
	m_workflowFile = workflowFileParameter->getValue().toString();
}

Frapper::ParameterGroup* CharonWorkflowNode::getPluginGroup( QString pluginName, Frapper::ParameterGroup * rootGroup )
{
	// get frapper parameter group for plugin
	Frapper::ParameterGroup *charonPluginGroup = getParameterGroup(pluginName);
	if( !charonPluginGroup ) {
		charonPluginGroup = new Frapper::ParameterGroup(pluginName);
		rootGroup->addParameter(charonPluginGroup, true);
	}
	return charonPluginGroup;
}

void CharonWorkflowNode::createSlotParameter( Frapper::ParameterGroup* parameterGroup, const QString &name, const QString &type, const Frapper::Parameter::PinType &pinType, Slot* slot, bool visible/*=false */ )
{
	if( parameterGroup->getParameterMap().contains(name)) {
		Frapper::Parameter* parameter = parameterGroup->removeParameter(name);
		delete parameter;
	}

	Frapper::GenericParameter* parameter = new Frapper::GenericParameter( name, QVariant::fromValue<Slot *>(slot));
	parameterGroup->addParameter(parameter);

	// add template type to type description
	QString parameterType = type;
	if( !type.contains(QRegExp("<float>|<double>|<int>")))
		parameterType += (m_templateType == 0) ? "<double>" : (m_templateType == 1) ? "<float>" : "<int>";

	parameter->setTypeInfo( parameterType );
	parameter->setPinType( pinType);
	parameter->setDirty( true );
	parameter->setVisible(visible);

	if( pinType == Frapper::Parameter::PT_Input) {
		//parameter->setSelfEvaluating(false);
		parameter->setMultiplicity( Frapper::Parameter::M_OneOrMore );
		parameter->setOnConnectFunction(SLOT(connectSlots()));
		parameter->setOnDisconnectFunction(SLOT(disconnectSlots(int)));

	} else if( pinType == Frapper::Parameter::PT_Output) {
		//parameter->setProcessingFunction(SLOT(Process()));
		Slot *test = parameter->getValue().value<Slot*>();
	}
}

QString CharonWorkflowNode::getPluginName( const QString& parameterName )
{
	// split at the first occurrence of a '.'
	int pos = parameterName.indexOf(".");
	if( pos > 0) {
		return parameterName.left(pos);
	}
	else
		return QString();
}

void CharonWorkflowNode::updateCharonParameters()
{
	removeCharonParameters();
	createCharonParameters();

	bool showAdv = getBoolValue("Setup > Show Advanced Parameters");
	showInternalOutputSlots(showAdv);

	emit nodeChanged();
	forcePanelUpdate ();
}

void CharonWorkflowNode::showInternalOutputSlots( bool show /*= true */ )
{
	// parameter group to add slot parameters to
	Frapper::ParameterGroup* rootGroup  = getParameterRoot();
	Frapper::ParameterGroup* internalOutputSlotGroup = rootGroup->getParameterGroup("Internal Output Slots");
	if( internalOutputSlotGroup ) {
		foreach( Frapper::AbstractParameter* absParam, internalOutputSlotGroup->getParameterMap() ) {
			Frapper::Parameter* param = dynamic_cast<Frapper::Parameter*>(absParam);
			if (NULL != param)
			{
				param->setVisible(show);
			}
		}
	}
}

void CharonWorkflowNode::ProcessRequested()
{
	// create process dialog
	Frapper::LogProgressDialog* lpd = new Frapper::LogProgressDialog();

	lpd->setWindowTitle("Processing " + getName() + " ...");

	lpd->setWindowModality(Qt::ApplicationModal);
	//lpd->getProgressBar()->setRange( 0, 100);
	lpd->getTableView()->hideColumn(2); // hide function column

	// @TODO would be nice to have more detailed information about workflow execution progress here..
	//lpd->getProgressBar()->setValue(10);

	// take time
	QTime time;
	time.start();

	m_executionThread->start(QThread::LowPriority);
	connect( lpd, SIGNAL(rejected()), m_executionThread, SLOT(terminate()));

	while( m_executionThread->isRunning() ) {
		QCoreApplication::processEvents(); // keep GUI alive!
		if( time.elapsed() > 1 ) {// show Dialog after xxx mseconds
			lpd->show();
		}
	}

	//lpd->getProgressBar()->setValue(90);

	Frapper::AbstractParameter::List connectedParameters = getParameters(Frapper::Parameter::PT_Output, true);

	// ??? -> some frapper parameter evaluation magic happens here..
	foreach( QString outSlotName, m_charonSlotsIn)
		this->getParameter(outSlotName)->propagateDirty(false);

	//lpd->getProgressBar()->setValue(100);

	delete lpd;
}

void CharonWorkflowNode::handleExecutionError( QString errorMessage )
{
	QMessageBox::critical( 0, "Workflow Processing Error", 
		tr("An Exception was thrown while processing the workflow!\n"
		"\nError Message:\n") 
		+ errorMessage +
		tr("\n\nThe log window may provide further information." ));
}

void CharonWorkflowNode::setUpTimeDependencies( Frapper::NumberParameter *timeParameter, Frapper::NumberParameter *rangeParameter )
{
	if( m_timeParameter != timeParameter ){
		m_timeParameter = timeParameter;
		connect( m_timeParameter, SIGNAL(rangeChanged()), this, SLOT(timeParameterChanged()), Qt::UniqueConnection);
		timeParameterChanged();
	}
	if( m_rangeParameter != rangeParameter ){
		m_rangeParameter = rangeParameter;
		connect( m_rangeParameter, SIGNAL(rangeChanged()), this, SLOT(rangeParameterChanged()), Qt::UniqueConnection);
		rangeParameterChanged();
	}
}

void CharonWorkflowNode::timeParameterChanged()
{
	if( !m_timeParameter) return;

	int startFrame = m_timeParameter->getMinValue().toInt();
	foreach( Frapper::NumberParameter* param, m_startFrameParameters ){
		param->setValue( startFrame );
		param->executeChange();
	}

	int endFrame = m_timeParameter->getMaxValue().toInt();
	foreach( Frapper::NumberParameter* param, m_endFrameParameters ){
		param->setValue( endFrame );
		param->executeChange();
	}
}

void CharonWorkflowNode::rangeParameterChanged()
{
	if( !m_rangeParameter) return;

	int inFrame = m_rangeParameter->getMinValue().toInt();
	int startFrame = m_timeParameter->getMinValue().toInt();

	foreach( Frapper::NumberParameter* param, m_inFrameParameters ) {
		param->setValue( inFrame-startFrame ); // map from inFrame in [startFrame, endFrame] to CImgList in [0, end-start+1]
		param->executeChange();
	}

	int outFrame = m_rangeParameter->getMaxValue().toInt();
	foreach( Frapper::NumberParameter* param, m_outFrameParameters ) {
		param->setValue( outFrame-startFrame ); // map from outFrame in [startFrame, endFrame] to CImgList in [0, end-start+1]
		param->executeChange();
	}
}

void CharonWorkflowNode::openInTuchulcha()
{
	if( m_workflowFile == "")
		return;

	if( m_charonPath == "")
		return;

	// handle write back of workflow file before opening in tuchulcha
	if( m_workflowModified ) {

		// display save? message
		QMessageBox saveMsgBox;
		saveMsgBox.setWindowTitle("Edit in Tuchulcha");
		saveMsgBox.setText("The Workflow needs to be saved before it can be edited in Tuchulcha. Continue?");
		saveMsgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
		saveMsgBox.setDefaultButton(QMessageBox::Yes);

		if( saveMsgBox.exec() == QMessageBox::Yes ) {

			// write current state of workflow to file
			saveStarted();
		}
	}

	// continue only workflow was saved
	if( !m_workflowModified ){

		// check for modifications in workflow file
		QDateTime lastModifiedTime = QFileInfo(m_workflowFile).lastModified();

		QString tuchulchaExe = m_charonPath + "/bin/";

		if( CharonUseDebug ){
			tuchulchaExe += "tuchulcha_d.exe";
		} else {
			tuchulchaExe += "tuchulcha.exe";
		}

		Frapper::Log::debug("Loading tuchulcha from \"" + tuchulchaExe +"\".", "CharonWorkflowNode::openInTuchulcha" );
		Frapper::Log::debug("Workflow file: \"" + m_workflowFile +"\".", "CharonWorkflowNode::openInTuchulcha" );
		
		if( QFileInfo(tuchulchaExe).exists() ) {

			// Change current path to working directory, so that tuchulcha can handle relative paths
			QString currentDir = QDir::currentPath();
			QString workingDir = Frapper::SceneModel::getWorkingDirectory();
			QDir::setCurrent( workingDir );

			Frapper::Log::debug("Setting current directory to \"" + workingDir +"\".", "CharonWorkflowNode::openInTuchulcha" );
			
			int retCode = QProcess::execute(tuchulchaExe, QStringList() << m_workflowFile);

			// restore to execution path for frapper
			QDir::setCurrent(currentDir);

			// reload workflow upon changes
			if( lastModifiedTime != QFileInfo(m_workflowFile).lastModified())
			{
				// reload workflow from file
				m_workflowFile = "";
				loadCharonWorkflow();
			}
		} else {
			Frapper::Log::error("Tuchulcha executable was not found!", "CharonWorkflowNode::openInTuchulcha");
		}
	}
}

void CharonWorkflowNode::saveExternalConnections( bool disconnect /*= false*/ )
{
	m_slotConnections.clear();

	// store incoming slot connections
	foreach ( QString slotName, m_charonSlotsIn) {

		SlotConnection sc;
		Frapper::Parameter* inSlotParameter = Frapper::Node::getParameter( slotName );
		if( inSlotParameter ){

			sc.targetParameter = inSlotParameter->getName();
			sc.targetNode      = Frapper::Node::getName();

			QList<Frapper::Connection*> connections = inSlotParameter->getConnectionMap().values();
			
			foreach( Frapper::Connection* connection, connections ){
				Frapper::Parameter* sourceParameter = connection->getSourceParameter();
				if( sourceParameter ) {
					sc.sourceParameter = sourceParameter->getName();
					sc.sourceNode      = sourceParameter->getNode()->getName();
					m_slotConnections.push_back( sc );
				}
				if( disconnect ){
					deleteConnection(connection);
				}
			}
		}
	}

	// store outgoing slot connections
	foreach ( QString slotName, m_charonSlotsOut) {

		SlotConnection sc;
		Frapper::Parameter* outSlotParameter = Frapper::Node::getParameter( slotName );
		if( outSlotParameter ){

			sc.sourceParameter = outSlotParameter->getName();
			sc.sourceNode      = Frapper::Node::getName();

			QList<Frapper::Connection*> connections = outSlotParameter->getConnectionMap().values();

			foreach( Frapper::Connection* connection, connections ){
				Frapper::Parameter* targetParameter = connection->getTargetParameter();

				if( targetParameter ) {
					sc.targetParameter = targetParameter->getName();
					sc.targetNode      = targetParameter->getNode()->getName();
					m_slotConnections.push_back( sc );
				}
				if( disconnect ){
					deleteConnection(connection);
				}
			}
		}
	}
}

void CharonWorkflowNode::restoreExternalConnections()
{
	foreach( SlotConnection sc, m_slotConnections) {
		emit requestCreateConnection( sc.sourceNode, sc.sourceParameter, sc.targetNode, sc.targetParameter );
	}
}

void CharonWorkflowNode::loadStarted()
{
	m_sceneLoading = true;
}

void CharonWorkflowNode::loadReady()
{
	m_sceneLoading = false;

	// reset modified flag after scene was loaded
	m_workflowModified = false;
}

void CharonWorkflowNode::saveStarted()
{
	// prevent saving of charon parameters
	removeCharonParameters();

	if( m_workflowModified ) {
		m_workflow->saveWorkflow();
	}

	m_workflowModified = false;

	forcePanelUpdate();
}

void CharonWorkflowNode::saveFinished()
{
	// restore charon parameters after saving
	createCharonParameters();

	bool showAdv = getBoolValue("Setup > Show Advanced Parameters");
	showInternalOutputSlots(showAdv);

	emit nodeChanged();
	forcePanelUpdate ();
}

template<typename T>
T CharonWorkflowNode::getValue( const AbstractParameter* parameter)
{
	return (*((Parameter<T>*)parameter))();
}

template<typename T>
std::vector<T> CharonWorkflowNode::getValueList( const AbstractParameter* parameter)
{
	return (*((ParameterList<T>*)parameter))();
}

template<typename T>
void CharonWorkflowNode::setValue( const AbstractParameter* parameter, T value )
{
	(*((Parameter<T>*) parameter)) = value;
}

template<typename T>
void CharonWorkflowNode::setValueList( const AbstractParameter* parameter, std::vector<T> values )
{
	(*((ParameterList<T>*) parameter)) = values;
}

template<> void CharonWorkflowNode::setValueList<std::string>( const AbstractParameter* parameter, QString valueString )
{
	std::vector<std::string> values;
	const QStringList &valueList = valueString.split(";", QString::SkipEmptyParts);
	foreach ( QString sValue, valueList)
		values.push_back( sValue.toStdString());

	setValueList<std::string>( parameter, values);
};

template<>
void CharonWorkflowNode::setValueList<float>( const AbstractParameter* parameter, QString valueString )
{
	std::vector<float> values;
	//const QString paramString = intParameter->getValue().toString().remove(QRegExp("[^0-9;]"));
	const QStringList &valueList = valueString.split(";", QString::SkipEmptyParts);
	foreach ( QString sValue, valueList)
		values.push_back( sValue.trimmed().toFloat());

	setValueList<float>( parameter, values);
};

template<>
void CharonWorkflowNode::setValueList<double>( const AbstractParameter* parameter, QString valueString )
{
	std::vector<double> values;
	//const QString paramString = intParameter->getValue().toString().remove(QRegExp("[^0-9;]"));
	const QStringList &valueList = valueString.split(";", QString::SkipEmptyParts);
	foreach ( QString sValue, valueList)
		values.push_back( sValue.trimmed().toDouble());
	
	setValueList<double>( parameter, values);
};

template<>
void CharonWorkflowNode::setValueList<int>( const AbstractParameter* parameter, QString valueString )
{
	std::vector<int> values;
	//const QString paramString = intParameter->getValue().toString().remove(QRegExp("[^0-9;]"));
	const QStringList &valueList = valueString.split(";", QString::SkipEmptyParts);
	foreach ( QString sValue, valueList)
		values.push_back( sValue.trimmed().toInt());

	setValueList<int>( parameter, values);
};

template<>
void CharonWorkflowNode::setValueList<uint>( const AbstractParameter* parameter, QString valueString )
{
	std::vector<uint> values;
	//const QString paramString = uintParameter->getValue().toString().remove(QRegExp("[^0-9;]"));
	const QStringList &valueList = valueString.split(";", QString::SkipEmptyParts);
	foreach ( QString sValue, valueList)
		values.push_back( sValue.trimmed().toUInt());

	setValueList<uint>( parameter, values);
};

void CharonWorkflowNode::checkFileExists( Frapper::Parameter* fileParameter, QString charonPlugin )
{
	if( !fileParameter )
		return;

	Frapper::Parameter::Type type = fileParameter->getType();
	if( type == Frapper::Parameter::T_Directory ||
		type == Frapper::Parameter::T_Filename )
	{
		Frapper::FilenameParameter* filenameParameter = dynamic_cast<Frapper::FilenameParameter*>(fileParameter);
		if( filenameParameter )
		{
			Frapper::FilenameParameter::Type fileparameterType = filenameParameter->getType();
			if( fileparameterType == Frapper::FilenameParameter::FT_Dir ||
				fileparameterType == Frapper::FilenameParameter::FT_Open )
			{
				// get filename to update error flag
				QString filename = filenameParameter->getValue().toString();

				// prevent execution of plugin if input file does not exist
				bool fileFound = !filename.isEmpty() && !filenameParameter->FileNotFoundError();
				m_workflow->setActive( charonPlugin, fileFound);
			}
		}
	}
}

void CharonWorkflowNode::removeFrapperConnection( Frapper::Parameter * inParameter, Frapper::Parameter* connectedParameter )
{
	// remove frapper connection
	foreach( Frapper::Connection* conn, inParameter->getConnectionMap() ){
		if( conn && (conn->getSourceParameter() == connectedParameter || conn->getTargetParameter() == connectedParameter) )
			deleteConnection(conn);
	}
}

void CharonWorkflowNode::savedoc()
{
	QString workingDir = Frapper::SceneModel::getWorkingDirectory();
	QString filename = workingDir + '/' + m_name + "_doc.txt";
	QFile docfile( filename );

	docfile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&docfile);

	out << "^ Name\t" << "^ Description\t" << "^ Comment\t ^" << "\n";

	foreach( QString pluginGroupName, m_charonPlugins )
	{
		Frapper::ParameterGroup* pluginGroup = getParameterGroup(pluginGroupName);
		if( NULL != pluginGroup )
		{
			// remove affections
			foreach( Frapper::AbstractParameter* absParameter, pluginGroup->getParameterList()){
				Frapper::Parameter* parameter = dynamic_cast<Frapper::Parameter*>(absParameter);
				if( parameter ) {

					try{
						QStringList sp = parameter->getName().split('.');
						if( sp.size() > 1) {

							if( sp[1].contains("description"))
								throw std::exception("Skip plugin description!");

							QString description = m_workflow->getParameterDescription(sp[0], sp[1]);
							QString comment = m_workflow->getEditorComment( parameter->getName());

							out << '|' << parameter->getName() << '\t'
								<< '|' << description << '\t'
								<< '|' << comment << '\t'
								<< '|' << '\n';
						}
					} catch(std::exception& e) {}
				}
			}
		}
	}

	Frapper::Log::debug("doc written to "+filename);

	docfile.close();
}

} // end namespace

