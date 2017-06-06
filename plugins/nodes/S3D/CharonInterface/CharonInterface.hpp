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
//! \file "CharonInterface.hpp"
//! \brief Sources for CharonInterface to be included in your source file.
//!
//! \author    Michael Bussler <michael.bussler@filmakademie.de>
//! \version   1.1
//! \date      21.01.2014 (created)
//! \date      21.01.2014 (last updated)

#include "CharonInterface.h"
#include <charon-core/ParameteredObject.hxx>
#include <charon-core/Slots.hxx>
#include <QtGui/QProgressDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QTime>
#include <QDir>

#include "LogProgressDialog.h"
#include "GenericParameter.h"
#include "SceneModel.h"
#include "Log.h"

Threaded_PO::Threaded_PO() : 
	m_po(NULL) 
{
};

void Threaded_PO::setPO(ParameteredObject* po)
{
	if( m_po != po)
	{
		delete m_po;
		m_po = po;
		if(po) po->initialize();
	}
}

const ParameteredObject* Threaded_PO::getPO() 
{
	return m_po;
}

void Threaded_PO::executePO() 
{
	// Change current path to working directory, so that charon can handle relative paths
	QString currentDir = QDir::currentPath();
	QDir::setCurrent( Frapper::SceneModel::getWorkingDirectory() );

	if(m_po) try {
	
		m_po->run();
	
	} catch (const std::exception &msg) {

		QString errorMessage = QString( msg.what());
		Frapper::Log::error( "An Exception was thrown while executing workflow:" );
		Frapper::Log::error( "Message: " + errorMessage );
		emit ExecutionError( errorMessage );

	}

	// restore to execution path for frapper
	QDir::setCurrent(currentDir);

	emit ExecutionFinished();
}


CharonInterface::CharonInterface(QString name) :
	m_threadedPO(new Threaded_PO()),
	m_executionThread(new QThread()),
	m_name(name)
{
	// pipe charon messages to frapper log
	Frapper::Log::s_streamListener.setName("Charon");
	sout.assign( Frapper::Log::s_streamListener );
	
	m_threadedPO->moveToThread(m_executionThread);
	connect( m_executionThread, SIGNAL( started()), m_threadedPO, SLOT( executePO() ));
	connect( m_threadedPO, SIGNAL( ExecutionFinished() ), m_executionThread, SLOT( quit() ));
	connect( m_threadedPO, SIGNAL( ExecutionError(QString) ), this, SLOT( handleExecutionError(QString)));
}

CharonInterface::~CharonInterface()
{ 
	delete m_threadedPO; 
	delete m_executionThread; 
}
	
void CharonInterface::processWorkflow()
{
	// create process dialog
	Frapper::LogProgressDialog* lpd = new Frapper::LogProgressDialog();

	lpd->setWindowTitle("Processing " + m_name + " ...");

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
		if( time.elapsed() > 1 ) { // show Dialog after xxx mseconds
			lpd->show(); 
		}
	}

	//lpd->getProgressBar()->setValue(100);

	delete lpd;
}

void CharonInterface::handleExecutionError( QString errorMessage )
{
	QMessageBox::critical( 0, "Workflow Processing Error", 
		tr("An Exception was thrown while processing the workflow!\n"
		"\nError Message:\n") 
		+ errorMessage +
		tr("\n\nThe log window may provide further information." ));
}

void CharonInterface::connectSlots(Frapper::GenericParameter *inParameter)

{

	if (inParameter && inParameter->getPinType() == Frapper::Parameter::PT_Input ) 
	{
		try{

			const QString &sourceName = inParameter->getName();
			const int nbrConnections = inParameter->getConnectionMap().size();
			Slot *destSlot = m_threadedPO->getPO()->getSlot(sourceName.toStdString());



			for (int i=0; i<nbrConnections; ++i) 
			{
				Frapper::GenericParameter* connectedParameter = dynamic_cast<Frapper::GenericParameter *>(inParameter->getConnectedParameter( i ));

				if( !connectedParameter)
					continue;

				Slot *sourceSlot = connectedParameter->getValue().value<Slot *>();
				if (inParameter->isConnected() && sourceSlot && destSlot) 
				{
					const QString& srcType = inParameter->getTypeInfo().toLower();
					const QString& dstType = connectedParameter->getTypeInfo().toLower();
					if ( srcType != dstType ) 
					{
						Frapper::Log::error( "In and Out -Slots have different types:", "CharonInterface::connectSlots");
						Frapper::Log::error( "Source type: "+srcType+" Destination type: "+dstType, "CharonInterface::connectSlots");
						removeFrapperConnection(inParameter, connectedParameter);
					}
					else 
					{
						try {
							destSlot->connect(*sourceSlot);
							sourceSlot->connect(*destSlot);
						} catch (std::exception& e) {
							Frapper::Log::error( QString(e.what()), "CharonInterface::connectSlots");
							removeFrapperConnection(inParameter, connectedParameter);
						}				
					}
				}
				else 
				{
					Frapper::Log::error(QString("Parameter not connected!"), "CharonNode::connectSlots");
					removeFrapperConnection(inParameter, connectedParameter);
				}
			}
		} catch (std::exception& e) {
			Frapper::Log::error( QString(e.what()), "CharonInterface::connectSlots");
			return;
		}
	}
}

//!
//! Disonnects Frapper in/out parameter with Charon in/out slots.
//!
void CharonInterface::disconnectSlots( Frapper::GenericParameter *inParameter, int connectionID )
{
	if (inParameter) 
	{
		QString destName = inParameter->getName();
		QString sourceName = "";

		try{
			Frapper::Parameter *connectedParameter = inParameter->getConnectedParameterById(connectionID);
			
			if (connectedParameter) {
			
				const QString &sourceName = connectedParameter->getName();
				Slot* destSlot = m_threadedPO->getPO()->getSlot(destName.toStdString());

				if(destSlot) {

					std::set<Slot*> &aktSlots = destSlot->getTargets();
					for (std::set<Slot*>::iterator iter=aktSlots.begin(); iter!=aktSlots.end(); ++iter) 
					{
						Slot *slot = *iter;
						QString slotName = QString::fromStdString( slot->getParent().getName() + '.' + slot->getName());
						if (slotName == sourceName ) {
							Frapper::Log::debug(QString("Disconnect %1|%2...").arg(destSlot->getName().c_str()).arg(slot->getName().c_str()), "CharonInterface::disconnectSlots");
							if (!destSlot->disconnect(*slot))
								Frapper::Log::error(QString("disconnecting: %1|%2").arg(destSlot->getName().c_str()).arg(slot->getName().c_str()), "CharonInterface::disconnectSlots");
							else
								Frapper::Log::debug(QString("...done!"), "CharonInterface::disconnectSlots");
						}
					}
				
				} else {
					Frapper::Log::error("Error disconnecting " + destName + " from " + sourceName + "!", "CharonInterface::disconnectSlots");
					Frapper::Log::error("Destination Slot is NULL!", "CharonInterface::disconnectSlots");				
				}
			} else {
				Frapper::Log::error("Error disconnecting " + destName + "!", "CharonInterface::disconnectSlots");
				Frapper::Log::error("Connected Parameter not found!", "CharonInterface::disconnectSlots");						
			}

		} catch (std::exception& e) {
			Frapper::Log::error("Error disconnecting " + destName + " from " + sourceName + "!", "CharonInterface::disconnectSlots");
			Frapper::Log::error( QString(e.what()), "CharonInterface::disconnectSlots");
		}
	}
}

void CharonInterface::setPO(ParameteredObject* po)
{ 
	m_threadedPO->setPO(po); 
}

const ParameteredObject* CharonInterface::getPO() 
{ 
	return m_threadedPO->getPO();
}

Slot* CharonInterface::getSlot( const std::string& slotName ) const
{
	if( m_threadedPO->getPO() )
		return m_threadedPO->getPO()->getSlot(slotName);
	return NULL;
}

bool CharonInterface::executed()
{
	return m_threadedPO->getPO() && m_threadedPO->getPO()->executed();
}

void CharonInterface::removeFrapperConnection( Frapper::Parameter * inParameter, Frapper::Parameter* connectedParameter )
{
	// remove frapper connection
	foreach( Frapper::Connection* conn, inParameter->getConnectionMap() ){
		if( conn && (conn->getSourceParameter() == connectedParameter || conn->getTargetParameter() == connectedParameter) )
			emit requestDeleteConnection(conn);
	}
}
//!