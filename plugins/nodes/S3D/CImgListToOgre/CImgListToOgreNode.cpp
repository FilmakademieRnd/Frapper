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
//! \file "CImgListToOgreNode.cpp"
//! \brief Implementation file for CImgListToOgreNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!


#include "CImgListToOgreNode.h"

#include <QtCore/QFile>
#include "GenericParameter.h"
#include "OgreTools.h"

#include <../CImgListTools/CImgListTools.hpp>

#include <../CharonInterface/CharonInterface.hpp>

namespace CImgListToOgreNode {
	
Q_DECLARE_METATYPE(Slot *);

using namespace cimg_library;
using namespace Frapper;

///
/// Constructors and Destructors
///

template <typename T>
CImgListToOgrePO<T>::CImgListToOgrePO(const std::string& name) :
	TemplatedParameteredObject<T>( "CImgListToOgreImage", name, "Reads in a CImgList and outputs an Ogre Image")
{
	this->_addInputSlot( image_in, "Input Map", "A copy of the image stored as cimg_library::CImgList<T>", "CImgList<T>");
}


//!
//! Constructor of the CImgListToOgreNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
CImgListToOgreNode::CImgListToOgreNode ( const QString &name, ParameterGroup *parameterRoot ) :
	ImageNode(name, parameterRoot),
	m_inputParameter(0),
	m_currentFrame(1),
	m_selectedType(0)
{	
	m_renderImageName = createUniqueName("RenderTexture_CImgListToOgreNode");
	
	// let workflow execution be handled by the execution thread
	m_charonInterface = new CharonInterface(name);
	connect( m_charonInterface, SIGNAL(requestDeleteConnection(Frapper::Connection*)), this, SLOT(deleteConnection(Frapper::Connection*)));

	EnumerationParameter *templateTypeParameter = getEnumerationParameter("Template Type");
	assert( templateTypeParameter);
	templateTypeParameter->setChangeFunction(SLOT(changeTemplateType()));

	m_inputParameter = getGenericParameter("Input Map");
	assert( m_inputParameter);
	m_inputParameter->setTypeInfo("CImgList<" + templateTypeParameter->getCurrentLiteral() + ">");
	m_inputParameter->setSelfEvaluating(true);
	m_inputParameter->setOnConnectFunction(SLOT(connectSlots()));
	m_inputParameter->setOnDisconnectFunction(SLOT(disconnectSlots(int)));

	Frapper::Parameter *outputImageParameter = getParameter(m_outputImageName);
	if (outputImageParameter) {
		outputImageParameter->setProcessingFunction(SLOT(processOutputImage()));
	}

	addAffection("Input Map", m_outputImageName);

	initOutputTexture();

	// Need this extra parameter as m_timeParameter is initialized after the c'tor
	// The change function is called on scene load
	setChangeFunction("Update on frame change", SLOT(setUpdateOption()));

	//connect(this, SIGNAL(frameChanged(int)), SLOT(updateFrame(int)));
	changeTemplateType();
}


//!
//! Destructor of the CImgListToOgreNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CImgListToOgreNode::~CImgListToOgreNode ()
{
}


// private slots

//!
//! Canges the Charon plugins template type.
//!
void CImgListToOgreNode::changeTemplateType ()
{
	m_charonInterface->setPO(NULL);

	EnumerationParameter *enumParameter = getEnumerationParameter("Template Type");
	assert( enumParameter);

	const QString typeString = enumParameter->getCurrentLiteral();
	m_selectedType = enumParameter->getCurrentIndex();

	const std::string objectName = "CImgListToOgre";
	ParameteredObject* po = 0;
	switch (m_selectedType) 
	{
		default : 
		case 0: 
			po = new CImgListToOgrePO<double>(objectName);
			break;
		case 1: 
			po = new CImgListToOgrePO<float>(objectName);
			break;
		case 2: 
			po = new CImgListToOgrePO<int>(objectName);
			break;
	}	

	m_charonInterface->setPO(po);

	Slot *inSlot = m_charonInterface->getSlot("Input Map");
	assert (inSlot);

	GenericParameter* inputImageParameter = getGenericParameter("Input Map");
	assert(inputImageParameter);
	inputImageParameter->setValue(QVariant::fromValue<Slot *>(inSlot), false);
	inputImageParameter->setTypeInfo("CImgList<" + typeString + ">");

	emit nodeChanged();
	forcePanelUpdate();
}

//!
//! Initiates the Charon internal processing chain.
//!
void CImgListToOgreNode::processOutputImage ()
{

	Slot* slot = 0;
	try {
		// trigger evaluation of input image list
		Frapper::Parameter* inputMap = getParameter("Input Map");
		if( inputMap->isConnected() && !m_charonInterface->executed() ){
			
			if( getBoolValue("Show workflow processing warning") ){
				QMessageBox msgBox(QMessageBox::Warning, "Process Workflow: "+getName(), "This operation may take a while! Continue?", QMessageBox::Ok | QMessageBox::Cancel);
				if( msgBox.exec() == QMessageBox::Cancel)
					return;
			}
			m_charonInterface->processWorkflow();
		}
		slot = m_charonInterface->getSlot("Input Map");
	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), "CImgListToOgreNode::processOutputImage");
		return;
	}

	Ogre::TexturePtr image = getTextureValue( m_outputImageName, false );
	m_currentFrame = m_timeParameter->getValue().toInt();
	int startFrame = m_timeParameter->getMinValue().toInt();
	switch (m_selectedType) {
		default :
		case 0: 
			CImgListTools::imageFromSlot<double>( image, slot, m_currentFrame-startFrame); //use startFrame as offset to access charon image list
			break;
		case 1: 
			CImgListTools::imageFromSlot<float>( image, slot, m_currentFrame-startFrame); //use startFrame as offset to access charon image list
			break; 
		case 2: 
			CImgListTools::imageFromSlot<int>( image, slot, m_currentFrame-startFrame); //use startFrame as offset to access charon image list
			break;
	}

	setOutputImage(image);
}

//!
//! Updates the local frame number and triggers blit
//!
void CImgListToOgreNode::updateFrame(int frame)
{
	m_currentFrame = frame;

	if (isViewed())
		processOutputImage();
}

//!
//! Connects Frapper in/out parameter with Charon in/out slots.
//!

void CImgListToOgreNode::connectSlots()
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	m_charonInterface->connectSlots(inParameter);
}

//!
//! Disonnects Frapper in/out parameter with Charon in/out slots.
//!
void CImgListToOgreNode::disconnectSlots( int connectionID )
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	m_charonInterface->disconnectSlots(inParameter, connectionID);
}

void CImgListToOgreNode::setUpdateOption()
{
	Frapper::Parameter *outputImageParameter = getParameter(m_outputImageName);
	if( getBoolValue("Update on frame change"))
	{
		// set affection of image to time parameter to handle update on frame change
		m_timeParameter->addAffectedParameter(outputImageParameter);
	}
	else
	{
		m_timeParameter->removeAffectedParameter(outputImageParameter);
	}
}

void CImgListToOgreNode::initOutputTexture()
{
	// re-/create ogre image buffer
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	if (textureManager.resourceExists(m_renderImageName)) {
		textureManager.remove(m_renderImageName);
	}

	//Ogre::TexturePtr outputTexture = textureManager.createManual(
	//	m_renderImageName,
	//	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
	//	1024, 768, 0, Ogre::PF_A8R8G8B8, Ogre::TU_DEFAULT );

	Ogre::Image defaultImage;
	defaultImage.load("DefaultRenderImage.png", "General");
	Ogre::TexturePtr outputTexture = textureManager.loadImage(m_renderImageName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, defaultImage,
		Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_A8R8G8B8);

	setValue(m_outputImageName, outputTexture );
}

void CImgListToOgreNode::setUpTimeDependencies( Frapper::NumberParameter *timeParameter, Frapper::NumberParameter *rangeParameter )
{
	Node::setUpTimeDependencies( timeParameter, rangeParameter );
	setUpdateOption();
}

void CImgListToOgreNode::deleteConnection( Frapper::Connection *connection )
{
	Frapper::Node::deleteConnection(connection);
}

} // end namespace
