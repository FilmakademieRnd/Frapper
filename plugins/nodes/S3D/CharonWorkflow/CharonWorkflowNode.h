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
//! \brief Header file for CharonWorkflowNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       12.08.2013 (created)
//! \date       12.08.2013 (last updated)

//!

#ifndef CHARONWORKFLOWNODE_H
#define CHARONWORKFLOWNODE_H

#include "Node.h"

#include <PluginManager.h>
#include <ParameteredObject.h>
#include "CharonWorkflow.h"

namespace CharonWorkflowNode {

//!
//! Base class for all render nodes.
//!
class CharonWorkflowNode : public Frapper::Node
{

	Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the CharonWorkflowNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	CharonWorkflowNode ( const QString &name, Frapper::ParameterGroup *parameterRoot, Frapper::NumberParameter* timeParameter );

	//!
    //! Destructor of the CharonWorkflowNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CharonWorkflowNode ();

protected: // functions

	//!
	//!
	//!
	virtual void setUpTimeDependencies( Frapper::NumberParameter *timeParameter, Frapper::NumberParameter *rangeParameter );

private: // functions

	void InitializeParameters();

	//!
	//! Creates Frapper parameters from Charon plugin parameters
	//!
	void createCharonParameters();

	void removeCharonParameters();

	//!
	//! Creates Frapper parameters from Charon in/out slots
	//!
	void createCharonSlots();
	void removeCharonSlots();

	void saveExternalConnections( bool disconnect = false);
	void restoreExternalConnections();

	void showInternalOutputSlots( bool show = true );

	void createSlotParameter( Frapper::ParameterGroup* parameterGroup, const QString &name, const QString &type, const Frapper::Parameter::PinType &pinType, Slot* slot, bool visible=false );

	Frapper::ParameterGroup* getPluginGroup( QString pluginName, Frapper::ParameterGroup * rootGroup );

	QString estimateCharonPath( QString envVariableName = "CHARON_HOME");

	QString getPluginName( const QString& parameterName );

	void checkFileExists( Frapper::Parameter* fileParameter, QString charonPlugin );

	void removeFrapperConnection( Frapper::Parameter * inParameter, Frapper::Parameter* connectedParameter );

	template<typename T> T getValue( const AbstractParameter* parameter );
	template<typename T> std::vector<T> getValueList( const AbstractParameter* parameter );
	template<typename T> void setValue( const AbstractParameter* parameter, T value );
	template<typename T> void setValueList( const AbstractParameter* parameter, std::vector<T> values );
	template<typename T> void setValueList( const AbstractParameter* parameter, QString valueString );

	template<> void setValueList<float>( const AbstractParameter* parameter, QString valueString );
	template<> void setValueList<double>( const AbstractParameter* parameter, QString valueString );
	template<> void setValueList<int>( const AbstractParameter* parameter, QString valueString );
	template<> void setValueList<uint>( const AbstractParameter* parameter, QString valueString );
	template<> void setValueList<std::string>( const AbstractParameter* parameter, QString valueString );

private slots: 

	//!
    //! Changes the Charon parameters.
    //!
	void changeCharonParameter ();

    //!
    //! Connects Frapper in/out parameter with Charon in/out slots.
    //!
    void connectSlots ();

	//!
	//! Disonnects Frapper in/out parameter with Charon in/out slots.
	//!
	void disconnectSlots (int connenctionID);

	//!
	//! Initiates the Charon internal processing chain.
	//!
	void Process ();

	//!
	//! Execute workflow via processing parameter
	//!
	void ProcessRequested();

	//!
    //! Resets the Charon internal processing chain.
    //!
    void Reset ();

	//!
	//! Canges the Charon plugins template type.
	//!
	void changeTemplateType ();	

	//!
	//!
	//!
	void charonPathChanged();

	//!
	//!
	//!
	void loadCharonWorkflow();

	//!
	//!
	//!
	void reloadCharonWorkflow();

	////!
	////! Updates Frapper parameters from Charon plugin parameters
	////!
	void updateCharonParameters();

	//!
	//! This Slot handles updates of the timeline range
	//!
	void timeParameterChanged();

	//!
	//! This Slot handles updates of the In/Out frame range
	//!
	void rangeParameterChanged();

	//!
	//! Open the current workflow file in tuchulcha
	//!
	void openInTuchulcha();

	//!
	//! Open the current workflow file in tuchulcha
	//!
	void saveStarted();

	//!
	//!
	//!
	virtual void loadReady();

	//!
	//! Slot to handle error signal from execution thread
	//!
	void handleExecutionError( QString errorMessage );

	virtual void saveFinished();

	virtual void loadStarted();

	void savedoc();

private: // data

	CharonWorkflow* m_workflow;
	QThread*		m_executionThread;

	QSet<QString> m_charonSlotsIn;
	QSet<QString> m_charonSlotsOut;
	QSet<QString> m_charonPlugins;
	QSet<QString> m_slotOutputGroups;

	struct SlotConnection{
		QString sourceNode,
				sourceParameter,
				targetNode,
				targetParameter;
	};
	QList<SlotConnection> m_slotConnections;

	QList<Frapper::NumberParameter*> m_startFrameParameters;
	QList<Frapper::NumberParameter*> m_endFrameParameters;
	QList<Frapper::NumberParameter*> m_inFrameParameters;
	QList<Frapper::NumberParameter*> m_outFrameParameters;

	QString m_charonPath;
	int m_templateType;
	QString m_workflowFile;

	int pluginInstanceCounter;
	bool m_workflowModified;
	bool m_sceneLoading;
	
	//! check if all input files have been found before execution
	bool m_inputFilesExists;
};

} // end namespace

#endif
