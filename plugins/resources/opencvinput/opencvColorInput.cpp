/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2012 Filmakademie Baden-Wuerttemberg, Institute of Animation 
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

//! \file "opencvColorInput.cpp"
//! \brief Implementation file for opencvColorInput class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \author     Felix Bucella <filmakademie@bucella.de>
//! \version    1.1
//! \date       02.10.2013 (last updated)
//!

///
/// Constructors and Destructors
///

#include "opencvColorInput.h"


//!
//! Constructor of the opencvColorInput class.
//!
//! \param name The name to give the new mesh node.
//!
opencvColorInput::opencvColorInput (QString name, ParameterGroup *parameterRoot) : ImageNode(name, parameterRoot)
{
	m_sceneMgr = 0;
	m_imagePtr = 0;
	m_matrixParameter = new Frapper::GenericParameter("OpencvMatrix", 0);
	m_matrixParameter->setTypeInfo("cv::Mat");
	m_matrixParameter->setPinType(Frapper::Parameter::PT_Input);
	m_matrixParameter->setDirty(true);
	m_matrixParameter->setSelfEvaluating(true);
	parameterRoot->addParameter(m_matrixParameter);
	m_matrixParameter->setOnConnectFunction(SLOT(connectMatrixSlot()));
	m_matrixParameter->setOnDisconnectFunction(SLOT(disconnectMatrixSlot()));
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	m_defaultImage.load("DefaultRenderImage.png", "General");
	if (textureManager.resourceExists(m_uniqueRenderTextureName)) {
		textureManager.remove(m_uniqueRenderTextureName);
	}
	// create unique name for the cam texture
	m_uniqueRenderTextureName = createUniqueName("MissingCameraTexture");;
	m_texture = textureManager.loadImage(m_uniqueRenderTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, m_defaultImage);
	setValue(m_outputImageName, m_texture, true);	
}

//!
//! Destructor of the opencvColorInput class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
opencvColorInput::~opencvColorInput ()
{

}



///
/// Private Slots
///
//!
//! Is used if the generic Parameter is connected
//!
void opencvColorInput::connectMatrixSlot() {

	m_matrixParameter = dynamic_cast<Frapper::GenericParameter *>(sender());

	if(m_matrixParameter->getConnectedParameter()->getDescription() == "ColorMat") {

		if (m_matrixParameter) {

			m_matrixParameter->setSelfEvaluating(true);

			// Use a thread to wait for valid image pointer
			start(QThread::LowPriority);
			m_waitForConnect.wait(&m_ConnectMutex);

			// Quit the thread and wait for tis termination
			quit();
			while(!isFinished()) {}

			if (!m_imagePtr){
				Log::error("Something within the connection thread went wrong", "opencvColor2Inputs::connectMatrixSlot");
				m_matrixParameter->setSelfEvaluating(false);
				deleteConnection(m_matrixParameter->getConnectionMap().begin().value());
				return;
			}

			m_picture = m_imagePtr->clone();
			m_width = m_imagePtr->cols;
			m_height = m_imagePtr->rows;
			m_videoSize = m_width*m_height*m_imagePtr->channels();
			Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
			if (textureManager.resourceExists(m_uniqueRenderTextureName)) {
				textureManager.remove(m_uniqueRenderTextureName);
			}
			// create unique name for the cam texture
			m_uniqueRenderTextureName = createUniqueName("CameraTexture");
			m_texture = Ogre::TextureManager::getSingleton().createManual(
				m_uniqueRenderTextureName, 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				Ogre::TEX_TYPE_2D,
				m_width, m_height, 0, 
				Ogre::PF_X8R8G8B8,
				Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

			setValue(m_outputImageName, m_texture, true);
			setProcessingFunction(m_matrixParameter->getName(), SLOT(processInputMatrix()));
			Log::info( m_matrixParameter->getName() + " connected", "opencvColorInput::connectMatrixSlot");
		}
	} else {
		m_matrixParameter->setSelfEvaluating(false);
		deleteConnection(m_matrixParameter->getConnectionMap().begin().value());
		Log::error("This node needs OpencvMatrix as Input. Please check the connected Parameter");
	}
}

//!
//! Is used if the generic Parameter is disconnected
//!
void opencvColorInput::disconnectMatrixSlot()
{
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	m_defaultImage.load("DefaultRenderImage.png", "General");
	if (textureManager.resourceExists(m_uniqueRenderTextureName)) {
		textureManager.remove(m_uniqueRenderTextureName);
	}
	// create unique name for the cam texture
	m_uniqueRenderTextureName = createUniqueName("MissingCameraTexture");
	m_texture = textureManager.loadImage(m_uniqueRenderTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, m_defaultImage);
	setValue(m_outputImageName, m_texture, true);	
	m_matrixParameter->setSelfEvaluating(false); 
	Log::info("Matrix disconnected", "opencvColorInput::disconnectMatrixSlot");
}


///
/// Protected Slots
///

//!
//! SLOT that is used on the matrixParameter
//!
void opencvColorInput::processInputMatrix()
{
	if(m_matrixParameter->isConnected() && m_imagePtr) {
		m_picture = m_imagePtr->clone();
		updateVideoTexture();
	}
	#ifdef _DEBUG
	else
		Log::debug("No connection established at the moment", "opencvColorInput::processInputMatrix");
	#endif
}


///
/// Protected Methods
///

//!
//! Updates the videoTexture
//!
void opencvColorInput::updateVideoTexture()
{	
	if(m_matrixParameter->isConnected() && isViewed())
	{
		Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_texture->getBuffer();
		// lock the pixel buffer 
		pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);// for best performance use HBL_DISCARD!
		const Ogre::PixelBox pixelBox = pixelBuffer->getCurrentLock();
		unsigned char* pDest = static_cast<unsigned char*>(pixelBox.data);
		//copy imageData
		if(m_picture.data != 0)
			memcpy(pDest, m_picture.data, m_videoSize);
		else
			memcpy(pDest, m_imagePtr->data, m_videoSize);
		//this is just an example how you can set every pixel
		////for(int row=0;row< m_imagePtr->rows ;row++)
		////{
		////	uchar* ptr = (uchar*)(m_imagePtr->data + m_imagePtr->row*m_imagePtr->step);
		////	for(int col=0;col<m_imagePtr->cols;col++)
		////	{
		////		*pDest++ = *ptr++;
		////		*pDest++ = *ptr++;
		////		*pDest++ = *ptr++; 
		////		*pDest++ = *ptr++;
		////	}
		////}
		// unlock pixel buffer
		pixelBuffer->unlock();
		this->triggerRedraw();	
	}
}


//!
//! This function runs as thread until the connection is established
//!
void opencvColorInput::run()
{
	QMutexLocker locker(&m_ConnectMutex); //Lock mutex while method is runnimg
	int counter = 0;

	forever {

		m_imagePtr = 0;
		const QString sourceName = m_matrixParameter->getName();
		QVariant var = m_matrixParameter->getValue(true);
		if(!var.isValid())
			continue;
		m_imagePtr = var.value<cv::Mat*>();
		if(m_imagePtr || (counter > 100)) {
			#ifdef _DEBUG
				Log::debug(QString("Connection thread was running \"%1\" times").arg(counter + 1), "opencvColor2Inputs::run");
			#endif
			m_waitForConnect.wakeAll();
			return;
		}
		counter++;
	}
}