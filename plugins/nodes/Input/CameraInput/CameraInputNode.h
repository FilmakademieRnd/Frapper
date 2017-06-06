/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "CameraInputNode.h"
//! \brief Header file for CameraInputNode class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       30.01.2011 (last updated)
//!
//!
#ifndef CAMERAINPUTNODE_H
#define CAMERAINPUTNODE_H

#include "RenderNode.h"
#include "InstanceCounterMacros.h"
#include <QtCore/QThread>
#include "OgreManager.h"
#include "VideoSource.h"
#include <QWaitCondition>
#include <QMutex>
#include <QTimer>
#include <QFile>
#include "GenericParameter.h"

#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
	#include <windows.h>
#endif

namespace CameraInputNode {
using namespace Frapper;

//!
//! Class representing CameraInput
//!
class CameraInputNode : public RenderNode
{
    Q_OBJECT
	ADD_INSTANCE_COUNTER

public: /// constructors and destructors

    //!
    //! Constructor of the CameraInputNode class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    CameraInputNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the CameraInputNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual inline ~CameraInputNode ();

private:
	
	//!
    //! Static list containing all avaiable cameras.
    //!
	static QStringList s_camIDs;
	static QStringList s_camNames;

private: //methods

	//!
	//! Static function to find avaiable cameras
	//!
	static void findCameras();

	//!
	//! Thread to get newFrame from Camera
	//!
	virtual void run();

	//!
	//! Is used for initialising or reinitialing
	//! \param CameraId this value is used to setup the camera 
	//!
	void initialise(int CameraId);

	//!
	//! Starts the timer for the threaded capture trigger.
	//!
	void startCap();

	//!
	//! Stops the timer for the threaded capture trigger.
	//!
	void stopCap();
   
signals:

	//!
	//! This signal is used to trigger the method updateVideoTexture() from the runThread
	//!
	void newFrame();

private slots:

	//!
	//! Updates the videoTexture
	//!
	void updateVideoTexture();

	//!
	//! Sets Workerthread in sleeping mode or wakes the thread
	//!
	void sleepOrWakeupThread();

	//!
	//! Changes the Camera Source
	//!
	void changeCameraSource();

	//!
	//! Handles timer events for the Node.
	//!
	void wakeupThreadOne ();

	//!
	//! Function that is called when the trigger parameter is connected.
	//!
	void triggerConnected ();

	//!
	//! Function that is called when the trigger parameter is disconnected.
	//!
	void triggerDisconnected ();

	//!
	//! Sets the update interval.
	//!
	void setFPS();

	//!
	//! Sets the auto exposure value.
	//!
	void setAutoExposure();

	//!
	//! Sets the exposure value.
	//!
	void setExposure();

	//!
	//!	Open the camera specific configuration dialogue.
	//!
	void setupCamera ();

	//!
	//!	Starts and stops the recording.
	//!
	void record ();

	//!
	//! The slot which triggers the external camera initialisation.
	//!
	void triggerStart ();

	//!
	//! The slot which triggers the external camera recording.
	//!
	void triggerRecording ();


private: //data

	//!
    //! Is termination of run-thread requested?
    //!
	bool m_endThreadRequested;

	//!
	//! Stores the Choosen CameraID
	//!
	int m_CameraID;

	//!
	//! Is used to save the size of the videoinput -> pSrc.rows*pSrc.cols*4
	//!
	int m_videoSize;

	//!
	//! Pointer to the timer object
	//!
	QTimer *m_timer;

	//!
	//! VideoSource
	//!
	VideoSource *m_video;

	//!
	//! VideoRecorder
	//!
	VideoWriter *m_recorder;

	//!
	//! The file storing the time per frame;
	//!
	QFile *m_timeFile;

	//!
	//! The Generic Parameter for the colorMatrix
	//!
	GenericParameter *m_matrixParameter;

	//!
	//! The Generic Parameter for the rawMatrix
	//!
	GenericParameter *m_matrixRawParameter;

	//!
	//! Pointer to the input time parameter
	//!
	NumberParameter *m_triggerParameter;

	//!
    //! The camTexture
    //!
	Ogre::TexturePtr m_texture;

	//!
	//! WaitCondition is used to syncronize Mainthread with workerthread 
	//!
	QWaitCondition m_moreFrames;

	//!
	//! Is used to set the workerthread to sleep
	//!
	QWaitCondition m_sleepThread;

	//!
	//! Is used by moreFrames to syncronize Mainthread with workerthread
	//!
	QMutex m_frameVideoMutex;

	//!
	//! used to save unique name for the cam texture
	//!
	Ogre::String m_uniqueRenderTextureName;

	//!
	//! Mat from VideoSource color
	//!
	Mat m_colorImage;

	//!
	//! Mat from VideoSource raw
	//!
	Mat m_rawImage;

    //!
    //!  Image that is used if no Camera is available
    //!
	Ogre::Image m_defaultImage;

};
Q_DECLARE_METATYPE(cv::Mat*);

} // namespace CameraInputNode

#endif
