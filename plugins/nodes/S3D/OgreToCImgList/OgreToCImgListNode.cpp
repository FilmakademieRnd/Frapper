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
//! \file "OgreToCImgListNode.cpp"
//! \brief Implementation file for OgreToCImgListNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!


#include "OgreToCImgListNode.h"
#include <OgreBitwise.h>
#include "GenericParameter.h"
#include "Parameter.h"
#include "OgreTools.h"

#include <../CImgListTools/CImgListTools.hpp>

//charon
#include <ParameteredObject.hxx>
#include <Slots.hxx>

namespace OgreToCImgListNode {
	using namespace Frapper;


Q_DECLARE_METATYPE(Slot *);

using namespace cimg_library;

///
/// Constructors and Destructors
///

template <typename T>
OgreToCImgListPO<T>::OgreToCImgListPO(const std::string& name) :
	TemplatedParameteredObject<T>("OgreToCImgList2", name, "Converts an Ogre Image to a CImgList<T>")
{
	this->_addOutputSlot( image_out, "Output Map", "A copy of the Ogre image stored as CImgList<T>", "CImgList<T>");
}


//!
//! Constructor of the OgreToCImgListNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
OgreToCImgListNode::OgreToCImgListNode ( const QString &name, Frapper::ParameterGroup *parameterRoot ) :
	Frapper::Node(name, parameterRoot),
	m_parameteredObject(0),
	m_selectedType(0)
{	
	Frapper::EnumerationParameter *templateTypeParameter = getEnumerationParameter("Template Type");
	assert( templateTypeParameter);
	templateTypeParameter->setChangeFunction(SLOT(changeTemplateType()));

	Frapper::GenericParameter* outputImageParameter = getGenericParameter("Output Map");
	assert(outputImageParameter );

	outputImageParameter->setTypeInfo("CImgList<" + templateTypeParameter->getCurrentLiteral() + ">");
	outputImageParameter->setOnConnectFunction( SLOT(connectSlots()));
	outputImageParameter->setOnDisconnectFunction( SLOT(disconnectSlots(int)));
	outputImageParameter->setProcessingFunction( SLOT(processOutputSlot()));

	addAffection("Input Map", "Output Map");
	changeTemplateType();
	
	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the OgreToCImgListNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
OgreToCImgListNode::~OgreToCImgListNode ()
{
	DEC_INSTANCE_COUNTER;
}


//!
//! Initiates the Charon internal processing chain.
//!
void OgreToCImgListNode::processOutputSlot ()
{
	Frapper::Parameter *inputParameter = Frapper::Node::getParameter("Input Map");

	if (!inputParameter) {
		Frapper::Log::error(QString("No Input Parameter defined!"), "OgreToCImgListNode::processOutputImage");
		return;
	}

	Ogre::TexturePtr inputTexture = inputParameter->getValue().value<Ogre::TexturePtr>();

	if (inputTexture.isNull()) {
		Frapper::Log::error(QString("No Texture connected!"), "OgreToCImgListNode::processOutputImage");
		return;
	}

	int frame = m_timeParameter->getValue().toInt();
	if( frame < 1)
		return;

	try {

		Slot* slot = m_parameteredObject->getSlot("Output Map");
		Ogre::HardwarePixelBufferSharedPtr texBuffer = inputTexture->getBuffer();
		int startFrame = m_timeParameter->getMinValue().toInt();

		switch (m_selectedType) 
		{
		default:	
		case 0: 
			CImgListTools::resizeImageList<double>( slot, frame, false);
			CImgListTools::imageToSlot<double>( inputTexture, slot, frame-startFrame);
			break;
		case 1: 
			CImgListTools::resizeImageList<float>( slot, frame, false);
			CImgListTools::imageToSlot<float>( inputTexture, slot, frame-startFrame);
			break;
		case 2: 
			CImgListTools::resizeImageList<int>( slot, frame, false);
			CImgListTools::imageToSlot<int>( inputTexture, slot, frame-startFrame);
			break;
		}	

	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), "OgreToCImgListNode::processOutputImage");
		return;
	}
}


// private slots


//!
//! Canges the Charon plugins template type.
//!
void OgreToCImgListNode::changeTemplateType ()
{
	if (m_parameteredObject) {
		delete m_parameteredObject;
		m_parameteredObject = 0;
	}

	Frapper::EnumerationParameter *enumParameter = getEnumerationParameter("Template Type");
	assert( enumParameter);

	const QString typeString = enumParameter->getCurrentLiteral();
	m_selectedType = enumParameter->getCurrentIndex();
	const std::string objectName = "OgreToCImgList";

	switch ( m_selectedType) 
	{
		default:
		case 0: 
			m_parameteredObject = new OgreToCImgListPO<double>(objectName);
			prepareOutputSlot<double>();
			break;
		case 1: 
			m_parameteredObject = new OgreToCImgListPO<float>(objectName);
			prepareOutputSlot<float>();
			break;
		case 2: 
			m_parameteredObject = new OgreToCImgListPO<int>(objectName);
			prepareOutputSlot<int>();
			break;
	}

	Slot *outSlot = m_parameteredObject->getSlot("Output Map");

	Frapper::GenericParameter *outputImageParameter = getGenericParameter("Output Map");
	assert( outputImageParameter);
	outputImageParameter->setValue(QVariant::fromValue<Slot *>(outSlot), false);
	outputImageParameter->setTypeInfo("CImgList<" + typeString + ">");

	emit nodeChanged();
	forcePanelUpdate();
}

//!
//! Connects Frapper in/out parameter with Charon in/out slots.
//!
void OgreToCImgListNode::connectSlots ()
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
					changeTemplateType();
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
//! Disonnects Frapper in/out parameter with Charon in/out slots.
//!
void OgreToCImgListNode::disconnectSlots (int connectionID)
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

template<typename T>
void OgreToCImgListNode::prepareOutputSlot()
{
	try{
		Slot *slot = m_parameteredObject->getSlot("Output Map");
		OutputSlot<cimg_library::CImgList<T>>* slotOut = dynamic_cast<OutputSlot<cimg_library::CImgList<T>>*>(slot);

		if( slotOut){
			slotOut->prepare();
			cimg_library::CImgList<T> &imageListOut = (*slotOut)();
			// clear cimglist and create a single empty image
			imageListOut.clear();
			imageListOut.assign(1);
		}
	} catch (std::exception& e)	{
		Frapper::Log::error( QString(e.what()), "OgreToCImgListNode::prepareOutputSlot");
	}
}

} // end namespace
