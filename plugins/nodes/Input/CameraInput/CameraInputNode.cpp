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
//! \file "CameraInputNode.cpp"
//! \brief Camera Input
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       27.4.2012 (last updated)
//!

#include "CameraInputNode.h"
#include "Log.h"
#include <QTextStream>

#define MAXNUMBEROFCAMERAS 3 //maximum of connected cameras - set this higher if you have more connected capture 

namespace CameraInputNode {
using namespace Frapper;
using namespace std;


INIT_INSTANCE_COUNTER(CameraInputNode)

///
/// static data
///
QStringList CameraInputNode::s_camIDs;
QStringList CameraInputNode::s_camNames;


///
/// Public Constructors
///

//!
//! Constructor of the CameraInputNode class.
//!
//! \param name The name to give the new mesh node.
//!
CameraInputNode::CameraInputNode ( QString name, ParameterGroup *parameterRoot) :
RenderNode(name, parameterRoot),
m_video(0),
m_recorder(0),
m_timeFile(0)
{
	INC_INSTANCE_COUNTER

	// create new timer for capture triggering
	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), SLOT(wakeupThreadOne()));

	// scan for cameras
	if (s_camIDs.empty())
		findCameras();

	// create enum parameter for device list
	Frapper::EnumerationParameter *availableCameraEnumeration = new Frapper::EnumerationParameter("Choose Camera", 0);
	parameterRoot->addParameter(availableCameraEnumeration);

	availableCameraEnumeration->setLiterals(QStringList() << s_camNames);
	availableCameraEnumeration->setValues(QStringList() << s_camIDs);

	setCommandFunction("SetupCamera", SLOT(setupCamera()));
	setChangeFunction("Exposure", SLOT(setExposure()));
	setChangeFunction("AutoExposure", SLOT(setAutoExposure()));
	setProcessingFunction("triggerStart", SLOT(triggerStart()));
	setProcessingFunction("triggerRecord", SLOT(triggerRecording()));

	Parameter *commandParameter = new Parameter("Start", Parameter::T_Command, 0);
	parameterRoot->addParameter(commandParameter);
	commandParameter->setCommandFunction(SLOT(changeCameraSource()));

	Parameter *recordParameter = new Parameter("Record", Parameter::T_Command, 0);
	parameterRoot->addParameter(recordParameter);
	recordParameter->setCommandFunction(SLOT(record()));

	Frapper::Parameter *fpsParameter = getParameter("FPS");
	fpsParameter->setChangeFunction(SLOT(setFPS()));

	//use Signal newFrame to trigger produceNewFrame -> interprocess communication
	connect(this, SIGNAL(newFrame()), SLOT(updateVideoTexture()));

	m_matrixParameter = new Frapper::GenericParameter("OpencvMatrix", 0); //gernerate a new GenericParameter
	m_matrixParameter->setTypeInfo("cv::Mat");
	m_matrixParameter->setDescription("ColorMat");
	m_matrixParameter->setPinType(Frapper::Parameter::PT_Output);	
	parameterRoot->addParameter(m_matrixParameter);
	m_matrixParameter->setValue(QVariant::fromValue<cv::Mat*>(&m_colorImage), true);
	m_matrixParameter->setOnDisconnectFunction(SLOT(sleepOrWakeupThread()));
	m_matrixParameter->setOnConnectFunction(SLOT(sleepOrWakeupThread()));
	m_matrixParameter->setVisible(false);

	m_matrixRawParameter = new Frapper::GenericParameter("OpencvRawMatrix", 0); //gernerate a new GenericParameter
	m_matrixRawParameter->setTypeInfo("cv::Mat");
	m_matrixRawParameter->setDescription("RawMat");
	m_matrixRawParameter->setPinType(Frapper::Parameter::PT_Output);	
	parameterRoot->addParameter(m_matrixRawParameter);
	m_matrixRawParameter->setValue(QVariant::fromValue<cv::Mat*>(&m_rawImage), true);
	m_matrixRawParameter->setOnDisconnectFunction(SLOT(sleepOrWakeupThread()));
	m_matrixRawParameter->setOnConnectFunction(SLOT(sleepOrWakeupThread()));
	m_matrixRawParameter->setVisible(false);

	m_outputParameter->setOnDisconnectFunction(SLOT(sleepOrWakeupThread()));
	m_outputParameter->setOnConnectFunction(SLOT(sleepOrWakeupThread()));
	
	connect(this, SIGNAL( nodeChanged ()), SLOT(sleepOrWakeupThread())); //if node changes sleepOrWakeupThread is triggered
	
	m_triggerParameter = new NumberParameter("Trigger", Parameter::T_Int, 0);
	m_triggerParameter->setPinType(Parameter::PT_Input);
	m_triggerParameter->setMaxValue(std::numeric_limits<int>::max());
	m_triggerParameter->setMinValue(0);
	m_triggerParameter->setVisible(false);
	m_triggerParameter->setSelfEvaluating(true);
	parameterRoot->addParameter(m_triggerParameter);
	m_triggerParameter->setOnConnectFunction(SLOT(triggerConnected()));
	m_triggerParameter->setOnDisconnectFunction(SLOT(triggerDisconnected()));

	FilenameParameter *filenameParameter = new FilenameParameter("Filename","");	
	filenameParameter->setType(FilenameParameter::FT_Save);
	filenameParameter->setFilters("AVI (*.avi);; MJPEG (*.mjpg)");
	parameterRoot->addParameter(filenameParameter);
}

//!
//! Destructor of the CameraInputNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CameraInputNode::~CameraInputNode ()
{
	m_endThreadRequested = true;
	startCap();
	
	while(isRunning())
	{
		m_moreFrames.wakeOne();
		m_sleepThread.wakeOne();
	}

	stopCap();

	if (m_timeFile)
		delete m_timeFile;
	if (m_recorder)
		delete m_recorder;
	if (m_video)
		delete m_video;
	if (m_timer)
		delete m_timer;

	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	if (textureManager.resourceExists(m_uniqueRenderTextureName)) {
		textureManager.remove(m_uniqueRenderTextureName);
	}
	m_texture.setNull();

	DEC_INSTANCE_COUNTER
}

///
/// Public Slots
///



///
/// Private Slots
///

///
/// Static private functions
///

void CameraInputNode::findCameras()
{
	bool ret = true;
	for(int i=0; (MAXNUMBEROFCAMERAS) > i; i++){
		ret = VideoSource::checkCamera(i);
		if (!ret && i==0){ // no cam available
			s_camNames.append("No Cam Available");
			s_camIDs.append(QString::number(i));
		}
		if (i == MAXNUMBEROFCAMERAS && i > 0){ //debug some cameras dont work propper with opencv and put them self on every id
			s_camNames.append("Something is wrong with one of your connected cameras");
			s_camIDs.append(QString::number(i));
			Frapper::Log::debug("Something is wrong with on of your connected cameras");
			ret = false;
		}
		if (ret){
			s_camNames.append("CameraID");
			s_camIDs.append(QString::number(i));	
		} else 
			ret = false;
		msleep(100); // give the camera some time to answer
	}
}


//!
//! Sets Thread in Sleeping Mode or wakes the thread
//!
void CameraInputNode::sleepOrWakeupThread()
{
	if( m_recorder || 
		isViewed() || 
		m_outputParameter->isConnected() || 
		m_matrixParameter->isConnected() || 
		m_matrixRawParameter->isConnected() ){
		startCap();
	} 
	else {
		stopCap();
	}
}

//!
//! Changes the Camera Source
//!
void CameraInputNode::changeCameraSource()
{
   // obtain the render resolution preset parameter 
    EnumerationParameter *availableCamera = getEnumerationParameter("Choose Camera");
    if (!availableCamera) {
        Log::error("Available Camera parameter could not be obtained.", "CameraInputNode::changeCameraSource");
        return;
    }

    // get the value of the currently selected item in the enumeration
    QString value = availableCamera->getCurrentValue(); 
	if (value.isNull()){
        //> no value is available for the selected render resolution preset
        return;
	}
	int camid = value.toInt();
	m_endThreadRequested = true;
	startCap();
	
	while(isRunning()) {
		m_moreFrames.wakeOne();
		m_sleepThread.wakeOne();
	};
	
	if (m_video)
		delete m_video;
	
	initialise(camid);
}

//!
//! Updates the videoTexture
//!
void CameraInputNode::updateVideoTexture()
{	
	QMutexLocker locker(&m_frameVideoMutex); //lock Mutex
	if(m_colorImage.data != 0 && (isViewed() || m_outputParameter->isConnected()))
	{
		// lock the pixel buffer 
		OgreTools::HardwareBufferLocker hbl(m_texture->getBuffer());
		const Ogre::PixelBox &pixelBox = hbl.getCurrentLock();
		unsigned char* pDest = static_cast<unsigned char*>(pixelBox.data);
		//copy imageData
		memcpy(pDest, m_colorImage.data, m_videoSize); //copy image.data into pDest
		////this is just an example how you can set every pixel
		//for(int row=0;row< colorImage.rows ;row++)
		//{
		//	uchar* ptr = (uchar*)(colorImage.data + colorImage.row*colorImage.step);
		//	for(int col=0;col<colorImage.cols;col++)
		//	{
		//		*pDest++ = *ptr++;
		//		*pDest++ = *ptr++;
		//		*pDest++ = *ptr++; 
		//		*pDest++ = *ptr++;
		//	}
		//}
		//// unlock pixel buffer
	}

	m_moreFrames.wakeOne(); //trigger waiting condition once
}

//!
//! Handles timer events for the Node.
//!
void CameraInputNode::wakeupThreadOne ()
{
	m_sleepThread.wakeOne();
}

//!
//! Function that is called when the trigger parameter is connected.
//!
void CameraInputNode::triggerConnected ()
{
	NumberParameter *externalTrigger = getNumberParameter("Trigger");
	getParameter("FPS")->setEnabled(false);
	
	m_timer->stop();
	if (disconnect(m_timer, SIGNAL(timeout()), this, SLOT(wakeupThreadOne())))
		externalTrigger->setProcessingFunction(SLOT(wakeupThreadOne()));
}

//!
//! Function that is called when the trigger parameter is disconnected.
//!
void CameraInputNode::triggerDisconnected ()
{
	NumberParameter *externalTrigger = getNumberParameter("Trigger");
	externalTrigger->setProcessingFunction("");
	
	getParameter("FPS")->setEnabled(true);

	connect(m_timer, SIGNAL(timeout()), SLOT(wakeupThreadOne()));	
	const int interval = Helpers::MathH::DoubleToInt(1000.f / (getUnsignedIntValue("FPS")+1));
	m_timer->start(interval);
}

//!
//! Sets the update interval.
//!
void CameraInputNode::setFPS()
{
	const unsigned int fps = getUnsignedIntValue("FPS");
	const int interval = Helpers::MathH::DoubleToInt(1000.f / fps+1);
	if (m_video)
		m_video->setFPS(fps);
	if (m_timer)
		m_timer->setInterval(interval);
}

//!
//! Sets the auto exposure value.
//!
void CameraInputNode::setAutoExposure()
{
	const bool autoexposure = getBoolValue("AutoExposure");
	if (m_video)
		m_video->setAutoExposure(autoexposure);
}

//!
//! Sets the exposure value.
//!
void CameraInputNode::setExposure()
{
	const float exposure = getFloatValue("Exposure");
	if (m_video)
		m_video->setExposure(exposure);
}

//!
//!	Open the camera specific configuration dialogue.
//!
void CameraInputNode::setupCamera ()
{
	if (m_video)
		m_video->openCamSettings();
}

//!
//!	Starts and stops the recording.
//!
void CameraInputNode::record ()
{
	if (m_recorder) {
		m_endThreadRequested = true;

		while(isRunning())
		{
			m_moreFrames.wakeOne();
			m_sleepThread.wakeOne();
		}

		delete m_recorder;
		m_recorder = 0;

		if (m_timeFile) {
			m_timeFile->close();
			delete m_timeFile;
			m_timeFile = 0;
		}

		startCap();
		m_endThreadRequested = false;
		start(QThread::LowPriority);
	}
	else {
		// the base filename
		const QStringList &filename = getStringValue("Filename").split('.');
		if (!filename.empty() && !filename.at(0).isEmpty()) {
			const QString finalFilename = QString("%1.%2")
				.arg(filename.at(0))
				.arg(filename.at(1));

			if (!m_video)
				changeCameraSource();
			if (!m_video)
				return;
			VideoCapture *videoSource = m_video->getCaptureDevice();
			Size frameSize = Size(
				(int) videoSource->get(CV_CAP_PROP_FRAME_WIDTH),
				(int) videoSource->get(CV_CAP_PROP_FRAME_HEIGHT) );

			m_recorder = new VideoWriter(finalFilename.toStdString(), CV_FOURCC('M','J','P','G'), 25, frameSize);

			if (m_triggerParameter->isConnected()) {
				m_timeFile = new QFile(QString("%1.%2")
					.arg(filename.at(0))
					.arg("txt"));
				m_timeFile->open(QIODevice::WriteOnly | QIODevice::Text);
			}
		}
	}
}


//!
//! The slot which triggers the external camera initialisation.
//!
void CameraInputNode::triggerStart ()
{
	bool value = dynamic_cast<Parameter *>(sender())->getValue().toBool();
	if (value)
		changeCameraSource();
	else {
		m_endThreadRequested = true;
		startCap();

		while(isRunning()) {
			m_moreFrames.wakeOne();
			m_sleepThread.wakeOne();
		};

		stopCap();

		if (m_video) {
			delete m_video;
			m_video = 0;
		}
	}
}


//!
//! The slot which triggers the external camera recording.
//!
void CameraInputNode::triggerRecording ()
{
	bool value = dynamic_cast<Parameter *>(sender())->getValue().toBool();
	if (value && !m_recorder) {
		record();
	}
	else if (m_recorder) {
		m_endThreadRequested = true;

		while(isRunning())
		{
			m_moreFrames.wakeOne();
			m_sleepThread.wakeOne();
		}

		delete m_recorder;
		m_recorder = 0;

		if (m_timeFile) {
			m_timeFile->close();
			delete m_timeFile;
			m_timeFile = 0;
		}

		startCap();
		m_endThreadRequested = false;
		start(QThread::LowPriority);
	}
}


///
/// Public Methods
///


///
/// Private Methods
///

//!
//! Thread to get newFrame from Camera
//!
void CameraInputNode::run()
{
	forever {
		//if stopping of thread has been requested, stop the thread
		if ( m_endThreadRequested ) { 
			quit();
			return;		
		}
		QMutexLocker locker(&m_frameVideoMutex); //lock mutex
		m_sleepThread.wait(&m_frameVideoMutex);

		if (m_recorder) {
			m_rawImage = m_video->getNewRawImage();
			*m_recorder << m_rawImage;

			if (m_timeFile && m_timeFile->isOpen()) {
				const int inTime = m_triggerParameter->getValue().toInt();
				if (inTime > 0) {
					div_t qr = div(inTime, 1000);
					const int ms = qr.rem;
					qr = div(qr.quot, 60);
					const int s  = qr.rem;
					qr = div(qr.quot, 60);
					const int m  = qr.rem;
					const int h  = qr.quot;

					const QString timeString = QString("%1%2_%3_%4\n")
						.arg(h, 2, 10, QLatin1Char('0'))
						.arg(m, 2, 10, QLatin1Char('0'))
						.arg(s, 2, 10, QLatin1Char('0'))
						.arg(ms, 4, 10, QLatin1Char('0'));

					QTextStream outStream(m_timeFile);
					outStream << timeString;
				}
			}
		}
		else {	
			m_video->getRawAndColorImage(m_colorImage, m_rawImage);

			if(m_matrixParameter->isConnected() && !m_matrixParameter->isDirty()){
				m_matrixParameter->propagateDirty();
			}

			if(m_matrixRawParameter->isConnected() && !m_matrixRawParameter->isDirty()){
				m_matrixRawParameter->propagateDirty();
			}

			if (m_outputParameter->isConnected() || isViewed()) {
				//trigger mainthread updateVideoTexture()
				emit newFrame(); 
				m_moreFrames.wait(&m_frameVideoMutex);

				//if (isViewed())
					//this->triggerRedraw(); // redraw the ogreScene

				if (!m_outputParameter->isDirty())
					m_outputParameter->propagateDirty();
			}
		}
	}
}


//!
//! Is used for initialising or reinitialing
//! \param CameraId this value is used to setup the camera 
//!
void CameraInputNode::initialise(int CameraId)
{
	m_video = new VideoSource(CameraId);
	msleep(500);
	if(m_video->isCamAvailable()){
		//initialising successful
		m_CameraID = CameraId; //save CameraID
		m_colorImage = m_video->getNewColorImage(); //get a new ColorImage
		m_rawImage = m_video->getNewRawImage();	
		m_videoSize = m_colorImage.rows*m_colorImage.cols*m_colorImage.channels();

		m_video->setFPS(getUnsignedIntValue("FPS"));

		setValue("Resolution", "Native Resolution " + QString::number(m_colorImage.cols) + " x " + QString::number(m_colorImage.rows) + " width x height in pixels", true);
		
		// recreate unique name for the cam texture
		Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
		if (textureManager.resourceExists(m_uniqueRenderTextureName)) {
			textureManager.remove(m_uniqueRenderTextureName);
		}
		m_uniqueRenderTextureName = createUniqueName("CameraTexture");
		m_texture = Ogre::TextureManager::getSingleton().createManual(m_uniqueRenderTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, m_colorImage.cols, m_colorImage.rows, 0, Ogre::PF_X8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
		setValue(m_outputImageName, m_texture, true);
		m_matrixParameter->setValue(QVariant::fromValue<cv::Mat*>(&m_colorImage), true);
		m_matrixRawParameter->setValue(QVariant::fromValue<cv::Mat*>(&m_rawImage), true);
		
		//thread shall sleep at the beginning
		startCap();
		m_endThreadRequested = false;
		start(QThread::LowPriority);
	
	} else {
		//initialising not successful
		setValue(m_outputImageName, m_defaultTexture);	
	}
}


//!
//! Starts the timer for the threaded capture trigger.
//!
void CameraInputNode::startCap()
{
	NumberParameter *externalTrigger = getNumberParameter("Trigger");
	if (externalTrigger->isConnected())
		externalTrigger->setProcessingFunction(SLOT(wakeupThreadOne()));
	else {
		const int interval = Helpers::MathH::DoubleToInt(1000.f / (getUnsignedIntValue("FPS")+1));
		m_timer->start(interval);
	}
}


//!
//! Stops the timer for the threaded capture trigger.
//!
void CameraInputNode::stopCap()
{
	NumberParameter *externalTrigger = getNumberParameter("Trigger");
	if (externalTrigger->isConnected())
		externalTrigger->setProcessingFunction("");
	else
		m_timer->stop();
}


} // namespace CameraInputNode
