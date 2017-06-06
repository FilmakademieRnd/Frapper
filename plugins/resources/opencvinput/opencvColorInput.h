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

//!
//! \file "opencvColorInput.h"
//! \brief Header file for opencvColor class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \author     Felix Bucella <filmakademie@bucella.de>
//! \version    1.1
//! \date       02.10.2013 (last updated)
//!
#ifndef OPENCVCOLORINPUT_H
#define OPENCVCOLORINPUT_H
#include "ImageNode.h"
#include "ParameterGroup.h"
#include "GenericParameter.h"
#include "OgreManager.h"
#include <QtCore/QThread>
#include <QMutex>
#include <QWaitCondition>
#ifdef _WIN32
#include "cv.h"         // from OpenCV
#include "cxcore.h"     // from OpenCV
#include "highgui.h"    // from OpenCV, for CvCapture, cvReleaseCapture, cvCaptureFromCAM, cvGrabFrame, cvRetrieveFrame, ...
#endif
#ifdef _OSX
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#endif
#ifdef _LINUX
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <opencv/cv.h>
#endif


using namespace Frapper;


class opencvColorInput : public ImageNode
{
	Q_OBJECT
public: //constructors and destructors both are protected that only derivaded classes can use the constructor and destructor
    //!
    //! Constructor of the opencvColorInput class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    opencvColorInput (QString name, ParameterGroup *parameterRoot);

    //!
    //! Destructor of the opencvColorInput class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual inline ~opencvColorInput ();
	

	private slots: //slots

	//!
    //! connectMatrixSlot checks if the connected Parameter is valid
    //!
	void connectMatrixSlot();

	//!
	//! Is used if the generic Parameter is disconnected
	//!
	void disconnectMatrixSlot();



	protected slots: 

	//!
	//! SLOT that is used on the matrixParameter
	//!
	virtual void processInputMatrix();



	protected: //methods

	//!
	//! Updates the videoTexture
	//!
	virtual void updateVideoTexture();

	//!
	//! This function runs as thread until the connection is established
	//!
	void run();



	protected: //data
	//!
	//! Pointer to the cv::Mat 
	//!
	cv::Mat* m_imagePtr;

	//!
	//! Clone of Picture in Pointer
	//!
	cv::Mat m_picture;

	//!
	//! The width of the videoinput
	//!
	int m_width;

	//!
	//! The height of the videoinput
	//!
	int m_height;

	//!
	//! Is used to save the size of the videoinput -> width*height*(numberOfchannels)
	//!
	int m_videoSize;

	//!
	//! The Input Parameter
	//!
	Frapper::GenericParameter *m_matrixParameter;
	//!
    //!  Image that is used if no Camera is available
    //!
	Ogre::Image m_defaultImage;

	//!
	//! The used SceneManager
	//!
	Ogre::SceneManager *m_sceneMgr;

	//!
    //! The texture for the camera
    //!
	Ogre::TexturePtr m_texture;
	
	//!
	//! used to save unique name for the cam texture
	//!
	Ogre::String m_uniqueRenderTextureName;

	//!
	//! This Mutex is used by the wait condition to pause the main thread the run method is runnimg
	//!
	QMutex m_ConnectMutex;

	//!
	//! This wait condition Is used to pause the main thread while connection thread is running
	//!
	QWaitCondition m_waitForConnect;
};
Q_DECLARE_METATYPE(cv::Mat*);
#endif
