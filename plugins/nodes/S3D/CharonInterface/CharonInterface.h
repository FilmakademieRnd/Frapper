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
//! \file "CharonInterface.h"
//! \brief Header file for CharonInterface.
//!
//! This Interface uses Qt for threading. Don't forget to MOC it!
//!
//! \author    Michael Bussler <michael.bussler@filmakademie.de>
//! \version   1.1
//! \date      21.01.2014 (created)
//! \date      21.01.2014 (last updated)

#ifndef CHARONINTERFACE_H
#define CHARONINTERFACE_H

#include <QObject>

class QThread;
class ParameteredObject;
class Slot;

namespace Frapper {
	class GenericParameter;
	class Connection;
	class Parameter;
}

class Threaded_PO : public QObject {

	Q_OBJECT

public:
	Threaded_PO();

	void setPO(ParameteredObject* po);
	const ParameteredObject* getPO();

public slots:
	void executePO();

signals:
	void ExecutionFinished();
	void ExecutionError(QString errorMessage);

private:
	ParameteredObject* m_po;
};

class CharonInterface : public QObject {

	Q_OBJECT

public:
	CharonInterface( QString name );
	~CharonInterface();

public slots:

	//!
	//! Execute Charon nodes connected to slots of given Parametered Object.
	//! The workflow is processed in its own thread using low priority.
	//!
	void processWorkflow();

	//!
	//! Get access to a Slot of the internal parametered object.
	//! May throw an exception.
	//!
	//! \parameter slotName The name of the slot to get.
	//!
	Slot* getSlot(const std::string& slotName) const;

	//!
	//! Check if workflow was already executed
	//!
	//! \parameter slotName The name of the slot to get.
	//!
	bool executed();

	void setPO(ParameteredObject* po);
	const ParameteredObject* getPO();

	void connectSlots( Frapper::GenericParameter *inParameter );
	void disconnectSlots( Frapper::GenericParameter *inParameter, int connectionID );

private slots:

	//!
	//! Slot to handle error signal from execution thread
	//!
	void handleExecutionError( QString errorMessage );
	void removeFrapperConnection( Frapper::Parameter * inParameter, Frapper::Parameter* connectedParameter );


signals:
	void requestDeleteConnection( Frapper::Connection* conn);

private:

	// The charon parametered object, handles slots and processing
	Threaded_PO* m_threadedPO;
	QThread* m_executionThread;

	QString m_name;
};


#endif
//!