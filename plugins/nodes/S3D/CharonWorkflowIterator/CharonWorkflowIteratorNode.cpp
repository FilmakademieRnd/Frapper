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
//! \file "CharonWorkflowIteratorNode.cpp"
//! \brief Implementation file for CharonWorkflowIteratorNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       12.08.2013 (created)
//! \date       12.08.2013 (last updated)
//!


#include <QtCore/QString>
#include <QtCore/QObject>

#include "CharonWorkflowIteratorNode.h"
#include "Parameter.h"
#include "GenericParameter.h"

// charon
#include <ParameteredObject.hxx>
#include <Slots.hxx>

namespace CharonWorkflowIteratorNode {

Q_DECLARE_METATYPE(Slot *);

///
/// Constructors and Destructors
///

template <typename T>
NodeParameteredObject<T>::NodeParameteredObject(const std::string& name) :
TemplatedParameteredObject<T>( "CharonWorkflowIterator", name, "Iteratively process a charon workflow.")
{
	this->_addOutputSlot( filename, "Filenames", "List of auto-generated file names", "vector<string>");
} 

//!
//! Constructor of the CharonWorkflowIteratorNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
CharonWorkflowIteratorNode::CharonWorkflowIteratorNode ( const QString &name, Frapper::ParameterGroup *parameterRoot ) :
	Frapper::Node(name, parameterRoot)
{
	m_parameteredObject = new NodeParameteredObject<double>("CharonWorkflowIterator");
	m_parameteredObject->initialize();

	Slot *outSlot = m_parameteredObject->getSlot("Filenames");
	assert( outSlot );
	outSlot->prepare();

	Frapper::GenericParameter* outputFileParameter = getGenericParameter("filename");
	assert( outputFileParameter );
	outputFileParameter->setValue( QVariant::fromValue<Slot*>(outSlot), false);
	outputFileParameter->setTypeInfo("vector<string>");
	//outputFileParameter->setOnConnectFunction( SLOT(connectSlots()));
	//outputFileParameter->setOnDisconnectFunction( SLOT(disconnectSlots(int)));

	setCommandFunction("Start Iteration", SLOT(startIteration()));
}


//!
//! Destructor of the CharonWorkflowIteratorNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CharonWorkflowIteratorNode::~CharonWorkflowIteratorNode ()
{
}

void CharonWorkflowIteratorNode::startIteration()
{
	int inFrame  = getIntValue("In Frame");
	int outFrame = getIntValue("Out Frame");
	QString FilenameTemplate = getStringValue("Filename Template");

	Frapper::Parameter* startFrame = getParameter("startFrame");
	Frapper::Parameter* endFrame = getParameter("endFrame");
	assert(startFrame && endFrame );

	try {
		Slot* slot = m_parameteredObject->getSlot("Filenames");
		OutputSlot<std::vector<std::string>>* outSlot = dynamic_cast<OutputSlot<std::vector<std::string>>*>( slot );

		if( outSlot ){
			std::vector<std::string>& filenameList = (*outSlot)();

			for( int i=inFrame; i<outFrame; i++) {

				// write filename to slot
				QString filenameStr = FilenameTemplate.arg( i, 6, 10, QChar('0'));
				filenameList.clear();
				filenameList.push_back(filenameStr.toStdString());

				startFrame->setValue(QVariant(i), true);
				endFrame->setValue(QVariant(i+1), true);

				// start execution
				Frapper::Parameter* processParameter = getParameter("Process");
				processParameter->setDirty(true);
				processParameter->getValue(true);
			}
		}
	} catch (std::exception* e) {
		Frapper::Log::error("failed!");
	}
}

//!
//! Connects Frapper in/out parameter with Charon in/out slots.
//!

void CharonWorkflowIteratorNode::connectSlots()
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	if (inParameter) {
		const QString &sourceName = inParameter->getName();

		const int nbrConnections = inParameter->getConnectionMap().size();
		Slot *destSlot = m_parameteredObject->getSlot(sourceName.toStdString());

		for (int i=0; i<nbrConnections; ++i) 
		{
			Frapper::Parameter* connectedParameter = inParameter->getConnectedParameter( i );
			if( !connectedParameter)
				continue;

			Slot *sourceSlot = connectedParameter->getValue().value<Slot *>();
			if (inParameter->isConnected() && sourceSlot && destSlot) {
				if (sourceSlot->getType() != destSlot->getType()) {
					Frapper::Log::error(QString("In and Out -Slots have different types."), "CharonNode::connectSlots");
				}
				else {
					try {
						destSlot->connect(*sourceSlot);
						sourceSlot->connect(*destSlot);
					} catch (std::exception& e) {
						Frapper::Log::error( QString(e.what()), "CharonNode::connectSlots");
					}				
				}
			}
			else 
				Frapper::Log::error(QString("Parameter not connected!"), "CharonNode::connectSlots");
		}
	}
}

//!
//! Disconnects Frapper in/out parameter with Charon in/out slots.
//!
void CharonWorkflowIteratorNode::disconnectSlots( int connectionID )
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	if (inParameter) {
		Frapper::Parameter *connectedParameter = inParameter->getConnectedParameterById(connectionID);
		if (connectedParameter) {
			const QString &destName = inParameter->getName();
			const QString &sourceName = connectedParameter->getName();
			Slot *destSlot = m_parameteredObject->getSlot(destName.toStdString());
			std::set<Slot*> &aktSlots = destSlot->getTargets();

			for (std::set<Slot*>::iterator iter=aktSlots.begin(); iter!=aktSlots.end(); ++iter) {
				Slot *slot = *iter;
				if (slot->getName() == sourceName.toStdString()) {
					Frapper::Log::debug(QString("Disconnect %1|%2...").arg(destSlot->getName().c_str()).arg(slot->getName().c_str()));
					if (!destSlot->disconnect(*slot))
						Frapper::Log::error(QString("disconnecting: %1|%2").arg(destSlot->getName().c_str()).arg(slot->getName().c_str()), getName() + "Node::disconnectSlots");
					else
						Frapper::Log::debug(QString("...done!"));
				}
			}
		}
	}
}

} // end namespace

