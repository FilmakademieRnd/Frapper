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

/*
-----------------------------------------------------------------------------
(c) Copyright 1997-2014, SensoMotoric Instruments GmbH

NOTE: to run this node, the 'iView RED' needs to be on (the eye tracker 
activated), and the "Profile" selected. 

-----------------------------------------------------------------------------
*/


//!
//! \file "SMIEyeTrackerNode.cpp"
//! \brief Implementation file for SMIEyeTrackerNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    1.0
//! \date       04.07.2014 (created) 27.10.2014 (last update)
//!


#include "SMIEyeTrackerNode.h"



double	m_eyeUserScreenCoorX;	// position in screen of the pupil of one VC's eye
double	m_eyeUserScreenCoorY;	// position in screen of the pupil of one VC's eye
double	m_userDistaceToScreen;	// distance of the user to the camara


namespace SMIEyeTrackerNode {

using namespace Frapper;



int __stdcall SampleCallbackFunction(SampleStruct sampleData)
{
	m_userDistaceToScreen = sampleData.leftEye.eyePositionZ;

	// check where are the user's eyes (?)
	m_eyeUserScreenCoorX = sampleData.leftEye.gazeX;
	m_eyeUserScreenCoorY = sampleData.leftEye.gazeY;

	////std::cout << "From Sample Callback X: " << m_eyeUserScreenCoorX << " Y: " << m_eyeUserScreenCoorY <<  " Distance: " << m_userDistaceToScreen << std::endl;
	//std::cout << "Callback Y: " << m_eyeUserScreenCoorY << std::endl;
	
	//Frapper::Log::info( QString::number(sampleData.leftEye.gazeX), "leftEye.gazeX");
	//Frapper::Log::info( QString::number(sampleData.leftEye.eyePositionX), "leftEye.positionX");

	return 1;
}

int __stdcall AOICallbackFunction(int outValue)
{
	//std::cout << "AOI hit: " << outValue << std::endl;
	return outValue;
}

INIT_INSTANCE_COUNTER(SMIEyeTrackerNode)

//!
//! Constructor of the SMIEyeTrackerNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
SMIEyeTrackerNode::SMIEyeTrackerNode ( QString name, ParameterGroup *parameterRoot ) :
	Node(name, parameterRoot),
	parameterRoot(parameterRoot),
	m_propagateDirty(true),
	m_eyetrackerconnected(false)
{   

	// This node requires the libraries from the SMI iView, which is the SDK of the RED-m/RED-250/RED500 eye tracker.
	// This message box provides information about where to read the info for downloading and installing the libraries. 
	/*QMessageBox msgBox;
	msgBox.setText("SMIEyetracker node requires the SMI iView SDK. Go to the 'README-SMI.txt' file to get more info about it.");
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setStandardButtons(QMessageBox::Ok);
	int res = msgBox.exec();
*/
	if (eyeTrackerLibsVerification())
		initialization();
}

SMIEyeTrackerNode::~SMIEyeTrackerNode ()
{
   
}

//!
//! Initialization of the parameters. 
//!
void SMIEyeTrackerNode::initialization()
{
	m_timer = new QTimer();
	m_recordingStarted = false;

	// processing functions for all parameters corresponding to the AOI 
	setChangeFunction("Gaze Threshold", SLOT(setGazeThreshold()));
	setProcessingFunction("Deactivate Gaze Control", SLOT(setGazeControl()));
	setProcessingFunction("Start Device (with Calibration)", SLOT(startDevice()));
	setProcessingFunction("Start Device (without Calibration)", SLOT(startDevice()));
	setProcessingFunction("Record Data", SLOT(recordData()));
	setProcessingFunction("Sending Message", SLOT(sendMessage()));
	
	m_aoiRegionsGroup = getParameterGroup("AOI coords");

	connect(m_timer, SIGNAL(timeout()), this, SLOT(setGazeValues()));

	//fill the m_aoiColors with color names (from BeGaze)
	registerAOIColors();
}



//!
//! Slot that is called to start the eye tracker
//!
void SMIEyeTrackerNode::startDevice()
{
	QString ipStimulus, ipEyetracker, filename;
	int calibrationPoints, calibrationSpeed, displayDevice, ret_con;
	bool withCalib, withoutCalib;  
	
	ret_con		 = iV_IsConnected();
	withCalib	 = getValue("Start Device (with Calibration)").toBool();
	withoutCalib = getValue("Start Device (without Calibration)").toBool();

	m_filename = QVariant(QDateTime::currentDateTimeUtc().toLocalTime().toMSecsSinceEpoch()).toString();

	if (ret_con == RET_SUCCESS)		//for debugging
		Frapper::Log::info( "Device IsConnected", "SMIEyeTrackerNode::startDevice()");

	if (withCalib && withoutCalib) {
		 Frapper::Log::info( "Device already started", "SMIEyeTrackerNode::startDevice()");
		 return;
	}
	else if (!withCalib && !withoutCalib && ret_con == RET_SUCCESS) {
		//filename = getValue("Recorded Data File (path)").toString() + "\\" + m_filename + ".idf";
		filename = m_path + "\\" + m_filename + ".idf";
		stopDevice(filename);
	}
	else if ( (withCalib || withoutCalib) && ret_con != RET_SUCCESS ) {
		m_path = getValue("Recorded Data File (path)").toString();

		bool pathVerified = pathVerification();
		if (!pathVerified) {	
			if (withoutCalib)
				setValue("Start Device (without Calibration)", false);
			else
				setValue("Start Device (with Calibration)", false);
			return;
		}		

		ipStimulus			= getValue("IP Address Stimulus PC").toString();
		ipEyetracker		= getValue("IP Address Eye tracker").toString();
		displayDevice		= getValue("Display Device").toInt();
		calibrationPoints	= getValue("Calibration Method").toInt();
		calibrationSpeed	= getValue("Calibration Speed").toInt();
		m_eyeVCScreenCoordY = getValue("Eye Screen Coordinate (TEST) Y").toDouble();
		m_eyeVCScreenCoordX = getValue("Eye Screen Coordinate (TEST) X").toDouble();
		m_gaze_Offset		= getValue("Gaze Threshold").toDouble();
		m_screenResX		= getValue("Display Resolution Width (px)").toDouble();
		m_screenResY		= getValue("Display Resolution Height (px)").toDouble();
		
		initializeDevice(ipStimulus, ipEyetracker, calibrationPoints, calibrationSpeed, displayDevice, withCalib);
	}
}

//!
//! Establishes the connection with the device, starts the calibration process and the tracking of the eye gaze.
//!
//! \param ipStimulus			IP Address of the computer with the stimulus (i.e. the computer where Frapper runs)
//! \param ipEyetracker			IP Address of the computer with the eye tracker (i.e. where the device is installed)
//! \param calibrationPoints	Number of requested calibration point  (2, 3, 5, 9)
//! \param calibrationSpeed		set calibration/validation speed [0: slow (default), 1: fast] 
//! \param displayDevice		set display device [0: primary device (default), 1: secondary device] 
//!
void SMIEyeTrackerNode::initializeDevice(QString ipStimulus, QString ipEyetracker, int calibrationPoints, int calibrationSpeed, int displayDevice, bool withCalibration)
{
	SystemInfoStruct systemInfoData; 
	REDGeometryStruct deviceInfoData;
	int ret_connect = 0, ret_redgeometry = 0;
	QByteArray cipEyetracker = ipEyetracker.toLatin1();
	QByteArray cipStimulus = ipStimulus.toLatin1();
	char *c_str2 = cipEyetracker.data();
	char *c_str1 = cipStimulus.data();

	// connect to iViewX 
	ret_connect = iV_Connect( c_str2, 4444, c_str1, 5555);

	switch(ret_connect)
	{
	case RET_SUCCESS:
		
		Frapper::Log::info( "Connection was established successfully", "SMIEyeTrackerNode::initializeDevice()");
		m_timer->start(1000);

		// timer to save AOI Values into XML format
		m_timerAOI = new QTimer();
		connect(m_timerAOI, SIGNAL(timeout()), this, SLOT(createAOIs()));

		iV_GetCurrentREDGeometry(&deviceInfoData);
		m_screenWidth = deviceInfoData.stimX;
		m_screenHeight = deviceInfoData.stimY;
		
		m_eyetrackerconnected = true;

		// read out meta data from iViewX 
		std::cout << "GetSystemInfo: " << iV_GetSystemInfo(&systemInfoData) << std::endl;
		std::cout << "SystemInfo ETSystem: " << systemInfoData.iV_ETDevice << std::endl;
		std::cout << "SystemInfo iV_Version: " << systemInfoData.iV_MajorVersion << "." << systemInfoData.iV_MinorVersion << "." << systemInfoData.iV_Buildnumber << std::endl;
		std::cout << "SystemInfo API_Version: " << systemInfoData.API_MajorVersion << "." << systemInfoData.API_MinorVersion << "." << systemInfoData.API_Buildnumber << std::endl;
		std::cout << "SystemInfo samplerate: " << systemInfoData.samplerate << std::endl;

		std::cout << "m_screenHeight: " << m_screenHeight << std::endl;
		std::cout << "m_screenWidth: " << m_screenWidth << std::endl;

		break;
	case ERR_EYETRACKING_APPLICATION_NOT_RUNNING:		
		Frapper::Log::info( "iView Red is OFF or not running");
		break;
	case ERR_COULD_NOT_CONNECT:		
		Frapper::Log::info( "Connection could not be established" );
		break;
	case ERR_WRONG_PARAMETER:		
		Frapper::Log::info( "Wrong Parameter used" );
		break;
	default:		
		Frapper::Log::info( "Any other error appeared" );
		return;
	}

	if(ret_connect == RET_SUCCESS)
	{
		if (withCalibration) {

			int err = iV_GetSample(&rawDataSample);
			m_filename = QVariant(rawDataSample.timestamp).toString();

			// define logger 
			QString filename = m_path;
			filename.replace(QString("\\"), QString("\\\\"));
			filename = filename + "\\logfile" + m_filename + ".lgf";	// change for a para
			QByteArray ba = filename.toLatin1();
			char* logfilename = ba.data();
			int res_log = iV_SetLogger(16, logfilename);

			switch(res_log) {
				case RET_SUCCESS:
					Frapper::Log::debug("iV_SetLogger SUCESS", "SMIEyeTrackerNode::initializeDevice()");
					break;
				case ERR_WRONG_PARAMETER:
					Frapper::Log::debug("iV_SetLogger WRONG PARAMETER", "SMIEyeTrackerNode::initializeDevice()");
					break;
				case ERR_ACCESS_TO_FILE:
					Frapper::Log::debug("iV_SetLogger ERROR ACCESS TO FILE", "SMIEyeTrackerNode::initializeDevice()");
					break;
			}

			// start calibration
			calibration(calibrationPoints, calibrationSpeed, displayDevice);	//calibration is done
		}

		// set callback for receiving samples 
		int ret_err = iV_SetAOIHitCallback(AOICallbackFunction);
		switch (ret_err) 
		{
		case RET_SUCCESS:
			Frapper::Log::debug("iV_SetAOIHitCallback SUCESS", "SMIEyeTrackerNode::initializeDevice()");
			break;
		case ERR_WRONG_PARAMETER:
			Frapper::Log::debug("iV_SetAOIHitCallback WRONG", "SMIEyeTrackerNode::initializeDevice()");
			break;
		}

		// define a callback function for receiving samples 
		ret_err = iV_SetSampleCallback(SampleCallbackFunction);
		switch (ret_err) 
		{
		case RET_SUCCESS:
			Frapper::Log::debug("iV_SetAOIHitCallback SUCESS", "SMIEyeTrackerNode::initializeDevice()");
			break;
		case ERR_WRONG_PARAMETER:
			Frapper::Log::debug("iV_SetSampleCallback WRONG", "SMIEyeTrackerNode::initializeDevice()");
			break;
		}

		// initiate tracking
		tracking();		

		// always Record Data will begin after Start Device has been checked.
		setValue("Record Data", true);
		getParameter("Record Data")->propagateDirty(true);

		// create AOI
		createAOIs();	
	}
}



//!
//! Stops the recording of the eye gaze data, saves the data into a .idf file and stops the device
//!
//! \param filename			name of the file that will store the eye gaze data. The file is stored in the eye tracker computer
//!
void SMIEyeTrackerNode::stopDevice(QString filename)
{
	filename = filename.replace("/", "\\");
	QByteArray cFilename = filename.toLatin1();
	char *c_file = cFilename.data();

	// stop recording 
	if (m_recordingStarted)
	{
		iV_StopRecording();
		setValue("Record Data", false);
		m_recordingStarted = false;
		m_timerAOI->stop();

		// Saves the recording data into a .idf file
		int err = iV_SaveData( c_file, "shortDescription", "username", 1);

		switch(err)
		{
		case RET_SUCCESS:			
			Frapper::Log::info( "", "data saved");
			break;
		case ERR_NOT_CONNECTED:
			std::cout << "No connection established" << std::endl;
			break;
		case ERR_WRONG_DEVICE:
			std::cout << "Required eye tracker is not connected" << std::endl;
			break;
		case ERR_EMPTY_DATA_BUFFER:
			std::cout << "Recording buffer is empty" << std::endl;
			break;
		case ERR_FULL_DATA_BUFFER:
			std::cout << "Data buffer is full" << std::endl;
			break;
		default:
			std::cout << "OTHER ERROR FOUND" << std::endl;
			break;
		}
	}

	// disable callbacks
	iV_SetSampleCallback(NULL);
	iV_SetAOIHitCallback(NULL);
	
	// disconnect
	int ret_disconnect = iV_Disconnect();
	switch(ret_disconnect)
	{
	case RET_SUCCESS:
		std::cout <<  "Disconnection was successful" << std::endl;
		m_eyetrackerconnected = false;
		break;
	case ERR_DELETE_SOCKET:
		std::cout <<  "Problem deleting socket" << std::endl;
		break;
	default:
		std::cout << "OTHER ERROR FOUND" << std::endl;
		break;
	}

	// write the XML file
	writeAOIsToXML();

	//destroy AOIs
	if (m_aoiRegionsGroup)
	{
		QList<AbstractParameter*> aoiRegions = m_aoiRegionsGroup->getParameterList();
		foreach(AbstractParameter *aRegion, aoiRegions)
		{
			ParameterGroup *aoiRegionGroup = dynamic_cast<ParameterGroup *>(aRegion);
			destroyAOI(aoiRegionGroup->getName());
		}
	}

	//destroy AOI keyframes (for XML purposes)
	qDeleteAll(m_keyframes);
	m_keyframes.clear();

	m_timer->stop();

	// Write the file
	std::cout << c_file << std::endl;
}


//!
//! Slot that is called to start/stop the recording of the eye gaze data
//!
void SMIEyeTrackerNode::recordData()
{
	bool off = getValue("Record Data").toBool();

	if (off)
		startRecording();	
	else
		pauseRecording();	
}


//!
//! Sets the calibration and validation visualization parameter. 
//!
//! \param calibrationPoints	Number of requested calibration point  (2, 3, 5, 9)
//! \param calibrationSpeed		set calibration/validation speed [0: slow (default), 1: fast] 
//! \param displayDevice		set display device [0: primary device (default), 1: secondary device] 
//!
void SMIEyeTrackerNode::calibration(int calibrationPoints, int calibrationSpeed, int displayDevice)
{
	AccuracyStruct accuracyData; 
	CalibrationStruct calibrationData;
	CalibrationStatusEnum calibrationStatus;	
	int ret_calibrate = 0, ret_validate = 0;

	// set up calibration
	calibrationData.method					= calibrationPoints;			
	calibrationData.speed					= calibrationSpeed;
	calibrationData.displayDevice			= displayDevice;
	calibrationData.targetShape				= 2;
	calibrationData.foregroundBrightness	= 250;
	calibrationData.backgroundBrightness	= 230;
	calibrationData.autoAccept				= 1;
	calibrationData.targetSize				= 20;
	calibrationData.visualization			= 1;

	strcpy(calibrationData.targetFilename, "");

	iV_SetupCalibration(&calibrationData);

	//iV_ShowTrackingMonitor();		//shows a window with the tracking of the eyes

	std::cout << iV_GetCalibrationStatus(&calibrationStatus) << std::endl;

	// start calibration
	ret_calibrate = iV_Calibrate();

	switch(ret_calibrate)
	{
	case RET_SUCCESS:
		std::cout <<  "Calibration done successfully" << std::endl;
		iV_Log("Calibration done successfully");
		// start validation
		ret_validate = iV_Validate();

		// show accuracy only if validation was successful.
		if(ret_validate == RET_SUCCESS)
		{
			std::cout << "iV_GetAccuracy: " << iV_GetAccuracy(&accuracyData, 0) << std::endl;
			std::cout << "AccuracyData DevX: " << accuracyData.deviationLX << " DevY: " << accuracyData.deviationLY << std::endl;
			Frapper::Log::info( QString::number(accuracyData.deviationLX), "AccuracyData DevX");
			Frapper::Log::info( QString::number(accuracyData.deviationLY), "AccuracyData DevY");				
		}
		break;
	case ERR_NOT_CONNECTED:
		std::cout <<  "iViewX is not reachable" << std::endl;
		break;
	case ERR_WRONG_PARAMETER:
		std::cout <<  "Wrong Parameter used" << std::endl;
		break;
	case ERR_WRONG_DEVICE:
		std::cout <<  "Not possible to calibrate connected Eye Tracking System" << std::endl;
		break;
	case RET_CALIBRATION_ABORTED:
		std::cout <<  "Calibration aborted" << std::endl;
		break;
	default:
		std::cout <<  "An unknown error appeared" << std::endl;
		break;
	}
}


//!
//! Initiates the tracking in the eye tracker. 
//!
void SMIEyeTrackerNode::tracking()
{
	int err = 0; 

	//send message to the eye tracker 
	err = iV_SendImageMessage( "Recording Note:Eye gaze recording started  " ); 

	switch(err)
	{
	case RET_SUCCESS:
		Frapper::Log::debug( "tracking():", "Recording Note:Eye gaze recording started");
		break;
	case ERR_NOT_CONNECTED:
		Frapper::Log::debug( "tracking():", "no message sent");
		break;
	default:
		break;
	}

	// show some eye tracker windows
	/*err = iV_ShowEyeImageMonitor();
	std::cout << "show eye " << err << std::endl;
	iV_ShowTrackingMonitor();
	iV_ShowSceneVideoMonitor();*/
}

//!
//! Suspends the eye tracking application and disables calculation of gaze data (from the iView X SDK manual). 
//!
void SMIEyeTrackerNode::pauseTracking()
{
	iV_PauseEyetracking();	
}

//!
//! Wakes up and enables the eye tracking application from suspend mode to continue processing gaze data (from the iView X SDK manual). 
//!
void SMIEyeTrackerNode::resumeTracking()
{
	iV_ContinueEyetracking();	
}

//!
//! Pauses gaze data recording (from the iView X SDK manual). 
//!
void SMIEyeTrackerNode::pauseRecording()
{
	iV_PauseRecording();
}

//!
//! Starts/Resumes gaze data recording (from the iView X SDK manual). 
//! It also sends a message to the eye tracker with data from the animation. 
//! (for the DECT Test: the parameters passed from PsychoPy to play a certain animation)
//!
void SMIEyeTrackerNode::startRecording()
{
	int ret_record, err = 0;
	bool withCalib, withoutCalib;  
	withCalib	 = getBoolValue("Start Device (with Calibration)");
	withoutCalib = getBoolValue("Start Device (without Calibration)");

	if(m_recordingStarted)
	{
		ret_record = iV_ContinueRecording("");
		switch(ret_record)
		{
		case RET_SUCCESS:
			m_recordingStarted = true;
			Frapper::Log::info( "Started with recording", "SMIEyeTrackerNode::startRecording()");
			break;
		case ERR_NOT_CONNECTED:			
			Frapper::Log::info( "No connection established", "SMIEyeTrackerNode::startRecording()");
			break;
		case ERR_WRONG_DEVICE:			
			Frapper::Log::info( "Required eye tracker is not connected", "SMIEyeTrackerNode::startRecording()");
			break;
		case ERR_EMPTY_DATA_BUFFER:			
			Frapper::Log::info( "Empty data buffer", "SMIEyeTrackerNode::startRecording()");
			break;
		default:			
			Frapper::Log::info( "OTHER ERROR FOUND", "SMIEyeTrackerNode::startRecording()");
			break;
		}
	}
	else
	{
		if (withoutCalib) {
			QFileInfo info(m_path + "\\\\");
			QFileInfoList files = info.absoluteDir().entryInfoList();

			std::cout << info.absoluteDir().dirName().toStdString() << std::endl;

			foreach (QFileInfo file, files){
				if (file.isFile()) {

					if (file.suffix().compare("avi") == 0) {	//the screen recording is the first file to be created when recording starts.						
						QDateTime creationUTC = file.created().toUTC().toLocalTime();
						qint64 creationMS = creationUTC.toMSecsSinceEpoch();
						m_initialTimeStamp = creationMS;

						std::cout << "name: " << file.baseName().toStdString() << std::endl;						
						std::cout << "UTC: " <<creationUTC.toString().toStdString() << std::endl;												
						std::cout << "m_initialTimeStamp: " << QVariant(m_initialTimeStamp).toString().toStdString() << std::endl;
						Frapper::Log::info( QVariant(m_initialTimeStamp).toString(), "Without calibration: m_initialTimeStamp");

						m_filename = QVariant(m_initialTimeStamp).toString();	
						break;
					}
				}
			}
		}			
		else {
			// get timestamp of first image message
			err = iV_GetSample(&rawDataSample);
			QDateTime current;

			switch(err)
			{
			case RET_SUCCESS:
				//m_initialTimeStamp = rawDataSample.timestamp;			

				// to work with system time
				current = QDateTime::currentDateTime();
				m_initialTimeStamp = current.toMSecsSinceEpoch();

				Frapper::Log::info( "StartRecording() - Sample obtained:", QVariant(rawDataSample.timestamp).toString());
				Frapper::Log::info( QVariant(m_initialTimeStamp).toString(), "With calibration: m_initialTimeStamp");
				break;
			case RET_NO_VALID_DATA:
				m_initialTimeStamp = 0;
				Frapper::Log::info( " No Valid Data ", "With calibration: m_initialTimeStamp");
				break;
			default:
				break;
			}
		}

		ret_record = iV_StartRecording();
		switch(ret_record)
		{
		case RET_SUCCESS:
			m_recordingStarted = true;
			Frapper::Log::info( "Started with recording", "SMIEyeTrackerNode::startRecording()");
			break;
		case ERR_NOT_CONNECTED:			
			Frapper::Log::info( "No connection established", "SMIEyeTrackerNode::startRecording()");
			break;
		case ERR_WRONG_DEVICE:			
			Frapper::Log::info( "Required eye tracker is not connected", "SMIEyeTrackerNode::startRecording()");
			break;
		case ERR_RECORDING_DATA_BUFFER:			
			m_recordingStarted = true;
			Frapper::Log::info( "Recording is activated", "SMIEyeTrackerNode::startRecording()");
			break;
		default:
			Frapper::Log::info( "OTHER ERROR FOUND", "SMIEyeTrackerNode::startRecording()");
			break;
		}
	}

	//send message to the eye tracker device -> message is stored in the .idf file
	sendMessage();
}

//!
//! Slot that sends a message to the eye tracker, which will be stored in the .idf file
//!
//! \param char* eMessage		string message
void SMIEyeTrackerNode::sendMessage()
{
	if (!m_eyetrackerconnected)
		return;

	QString msg;
	QByteArray ba;
	char *msgChar, *prefix;
	int err;

	if (getValue("Sending Message").toBool())
	{
		prefix = "Recording Note: Started - ";
	}
	else
	{
		prefix = "Recording Note: Ended - ";
	}

	msg = getValue("Message").toString();
	ba = msg.toLocal8Bit();
	ba.prepend(prefix);	// to make the message recognized by the eye tracker
	msgChar = ba.data();

	//send message to the eye tracker 
	err = iV_SendImageMessage( msgChar ); 
	
	switch(err)
	{
		case RET_SUCCESS:			
			Frapper::Log::info( "Message sent", "SMIEyeTrackerNode::sendMessage()");
			break;
		case ERR_NOT_CONNECTED:			
			Frapper::Log::info( "video or device not connected", "SMIEyeTrackerNode::sendMessage()");
			break;
		default:
			break;
	}
}

//!
//! Slot that is called to pause/resume the eye tracking 
//!
//! \param ipStimulus			IP Address of the computer with the stimulus (i.e. the computer where Frapper runs)
//! \param ipEyetracker			IP Address of the computer with the eye tracker (i.e. where the device is installed)
//! \param calibrationPoints	Number of requested calibration point  (2, 3, 5, 9)
//! \param calibrationSpeed		set calibration/validation speed [0: slow (default), 1: fast] 
//! \param displayDevice		set display device [0: primary device (default), 1: secondary device] 
//!
void SMIEyeTrackerNode::pauseTrack()
{
	bool off = getValue("Start Tracking").toBool();

	if (off)
		resumeTracking();	
	else
		pauseTracking();	
}

//!
//! Sets the value (true or false) in the parameter to propagate the values of the user's gaze.
//!
void SMIEyeTrackerNode::setGazeControl()
{
	if (getValue("Deactivate Gaze Control").toBool())
		m_propagateDirty = false;
	else
		m_propagateDirty = true;
}
//!
//! Sets the values of the eye gaze (both eyes) in the node's gaze parameters 
//!
void SMIEyeTrackerNode::setGazeValues()
{
	// if verticalDist > 0 -> user looks up = upper gaze
 	double verticalDist = convertPixelToMilim(getEyesVerticalDistance(), m_screenHeight, m_screenResY);
	// if horizontalDist > 0 -> user looks to the  left = left gaze
	double horizontalDist = convertPixelToMilim(getEyesHorizontalDistance(), m_screenWidth, m_screenResX);

	if (m_userDistaceToScreen > 0.0)
	{
		// the values of the gaze are between 0 and 100
		double gazeAngleV = atan(abs(verticalDist)/m_userDistaceToScreen) * 30.0;		
		double gazeAngleH = atan(abs(horizontalDist)/m_userDistaceToScreen) * 30.0;		

		// Horizontal gaze
		if (horizontalDist < 0) {		
			setValue("GazeHRight", 0.0, m_propagateDirty);
			setValue("GazeHLeft", gazeAngleH, m_propagateDirty);	
		}
		else {		
			setValue("GazeHRight", gazeAngleH, m_propagateDirty);
			setValue("GazeHLeft", 0.0, m_propagateDirty);
		}

		// Vertical gaze down
		if (verticalDist < 0) {
			setValue("GazeVDown", gazeAngleV, m_propagateDirty);
			setValue("GazeVUp", 0.0, m_propagateDirty);
		}
		// Vertical gaze up
		else {
			setValue("GazeVDown", 0.0, m_propagateDirty);
			setValue("GazeVUp", gazeAngleV, m_propagateDirty);
		}
	}
	//re-start the timer
	m_timer->start(250);
}

//!!!!
//!!!! AOI related functions 
//!!!!

//!
//! Destroy the AOI Structs created.
//!
void SMIEyeTrackerNode::destroyAOI(QString regionName)
{
	m_timerAOI->stop();

	// get the AOI struct from the list and remove it. Remove also from m_aoiStructs
	if(m_aoiStructs.size() > 0)
	{
		QHash<QString, AOIStruct*>::iterator iter = m_aoiStructs.find(regionName);

		while(iter != m_aoiStructs.end() && iter.key() == regionName)
		{
			AOIStruct *aois = iter.value();

			if( QString(aois->aoiName).compare(regionName) == 0)
			{
				//Disable AOIs
				int ret_suc = iV_DisableAOI(aois->aoiName);
				switch (ret_suc)
				{
				case RET_SUCCESS:
					//Frapper::Log::debug(QString("AOI Disabled: (\"%1\")").arg(aois->aoiName), "SMIEyeTrackerNode::destroyAOI");		
					break;
				case RET_NO_VALID_DATA:					
					//Frapper::Log::debug(QString("AOI No data available: (\"%1\")").arg(aois->aoiName), "SMIEyeTrackerNode::destroyAOI");
					break;
				case ERR_AOI_ACCESS:
					Frapper::Log::debug(QString("Failed to access AOI: (\"%1\")").arg(aois->aoiName), "SMIEyeTrackerNode::destroyAOI");
					break;
				}

				//Delete AOIs
				int ret_rem = iV_RemoveAOI(aois->aoiName);
				switch (ret_rem)
				{
				case RET_SUCCESS:
					//Frapper::Log::debug(QString("AOI removed: (\"%1\")").arg(aois->aoiName), "SMIEyeTrackerNode::destroyAOI");
					break;
				case RET_NO_VALID_DATA:
					//Frapper::Log::debug(QString("AOI No data available: (\"%1\")").arg(aois->aoiName), "SMIEyeTrackerNode::destroyAOI");
					break;
				case ERR_AOI_ACCESS:
					Frapper::Log::debug(QString("Failed to access AOI: (\"%1\")").arg(aois->aoiName), "SMIEyeTrackerNode::destroyAOI");
					break;
				}

				//delete from m_aoiStructs
				m_aoiStructs.erase(iter);			
				break;
			}
		}
	}
}

//! 
//! creates the AOI Struct (iView struct) to be passed to the eye tracker.
//!
void SMIEyeTrackerNode::createAOIs()
{
	// Proceed to create the AOIs
	QList<AbstractParameter*> aoiRegions = m_aoiRegionsGroup->getParameterList();

	foreach(AbstractParameter *aRegion, aoiRegions)
	{
		ParameterGroup *aoiRegionGroup = dynamic_cast<ParameterGroup *>(aRegion);

		// destroy the AOIStruct if existent
		destroyAOI(aoiRegionGroup->getName());

		// create the AOIStruct
		AOIRectangleStruct aoiRectangle;
		QList<AbstractParameter*> aoiRegion = aoiRegionGroup->getParameterList();

		foreach(AbstractParameter *aoiC, aoiRegion)
		{
			Parameter *aoiCoord = dynamic_cast<Parameter*>(aoiC);
			QString aoiCoordName = aoiCoord->getName();	
			int aoiCoordValue = 0;

			// verification to get the correct coord value
			QVariantList list = aoiCoord->getValueList();
			for(int j=0; j<list.size(); j++) {
				QVariant listVal = list.at(j);
				const char* typeName = listVal.typeName();				
				if (strcmp(typeName, "int") == 0)				
					aoiCoordValue  = list.at(j).toInt();
			}	

			// create the AOIRectangle structure for the AOI
			if (aoiCoordName.contains("left"))
				aoiRectangle.x1 = aoiCoordValue;
			else if (aoiCoordName.contains("right"))
				aoiRectangle.x2 = aoiCoordValue;
			else if (aoiCoordName.contains("upper"))
				aoiRectangle.y1 = aoiCoordValue;
			else if (aoiCoordName.contains("lower"))
				aoiRectangle.y2 = aoiCoordValue;
		}


		QString aoiRegionName = aoiRegionGroup->getName();
		const QByteArray inBytes = aoiRegionName.toAscii(); 
		const char * regionName = inBytes.constData();
		char msgChar[256] = "Hit AOI: ";
		strcat(msgChar, regionName);

		
		AOIStruct *aoiStruct = new AOIStruct();
		std::strcpy(aoiStruct->aoiName, regionName);
		std::strcpy(aoiStruct->outputMessage, msgChar);
		aoiStruct->position = aoiRectangle;
		aoiStruct->fixationHit = 0;
		aoiStruct->eye = 'l';							//this is the eye that is considered during the eye tracking

		// Define the AOI in the eye tracker
		int ret_aoi = iV_DefineAOI(aoiStruct);
		switch(ret_aoi)
		{
		case RET_SUCCESS:				
			//Frapper::Log::debug(QString("AOI Created: (\"%1\")").arg(regionName), "SMIEyeTrackerNode::createAOI");
			break;
		case ERR_WRONG_PARAMETER:
			Frapper::Log::debug(QString("AOI has wrong parameter: (\"%1\")").arg(aoiStruct->aoiName), "SMIEyeTrackerNode::createAOI");
			return;
		}

		// Enable the AOI in the eye tracker
		int ret_enable = iV_EnableAOI(aoiStruct->aoiName);
		switch (ret_enable)
		{
		case RET_SUCCESS:			
			//Frapper::Log::debug(QString("AOI Enabled: (\"%1\")").arg(aoiStruct->aoiName), "SMIEyeTrackerNode::createAOI");				
			break;
		case RET_NO_VALID_DATA:
			Frapper::Log::debug(QString("AOI No data available: (\"%1\")").arg(aoiStruct->aoiName), "SMIEyeTrackerNode::createAOI");
			break;
		case ERR_AOI_ACCESS:
			Frapper::Log::debug(QString("Failed to access AOI: (\"%1\")").arg(aoiStruct->aoiName), "SMIEyeTrackerNode::createAOI");
			break;
		}

		// List with the defined AOIs.
		m_aoiStructs.insert(aoiRegionName, aoiStruct);

	}

	// Create the keyframe with AOI info to be written afterwards in the XML file.
	if (m_recordingStarted) 
		saveAOIValues(); 
		
	m_timerAOI->start(100);	// every 100 ms it will make timeout and call the function createAOIs() .
}

void SMIEyeTrackerNode::saveAOIValues()
{
	// get the SampleStruct to obtain the timestamp -> for XML purposes
	int ret_con = iV_GetSample(&rawDataSample);
	switch(ret_con)
	{
	case RET_SUCCESS:				
		//Frapper::Log::debug(QString("rawDataSample - obtained: (\"%1\")").arg(rawDataSample.timestamp), "SMIEyeTrackerNode::saveAOIValues");
		createXmlKeyframe(rawDataSample);
		break;
	case RET_NO_VALID_DATA:
		Frapper::Log::debug(QString("rawDataSample - not valid data: (\"%1\")").arg(rawDataSample.timestamp), "SMIEyeTrackerNode::saveAOIValues");
		break;
	case ERR_NOT_CONNECTED:
		Frapper::Log::debug(QString("rawDataSample - no connection: (\"%1\")").arg(rawDataSample.timestamp), "SMIEyeTrackerNode::saveAOIValues");
		break;
	}
}

//!
//! Creates a keyframe for the AOI to be stored in the XML file
//!
void SMIEyeTrackerNode::createXmlKeyframe(SampleStruct rawDataSample, AOIStruct* aoiStruct, QString visible)
{
	// store the <KeyFrame> data for the XML
	keyframe *aoiKeyFrame = new keyframe;

	aoiKeyFrame->timestamp = QVariant(rawDataSample.timestamp - m_initialTimeStamp).toString();
	aoiKeyFrame->aoi = *aoiStruct;
	//aoiKeyFrame->visible = "true";
	aoiKeyFrame->visible = visible;

 	m_keyframes.append(aoiKeyFrame);
}

//!
//! Creates keyframe for all the AOIs to be stored in the XML file
//!
void SMIEyeTrackerNode::createXmlKeyframe(SampleStruct rawDataSample, QString visible)
{
	QHash<QString, AOIStruct*>::iterator iter;
	
	for (iter=m_aoiStructs.begin(); iter!=m_aoiStructs.end(); iter++) {

		// store the <KeyFrame> data for the XML
		keyframe *aoiKeyFrame = new keyframe;

		aoiKeyFrame->timestamp = QVariant(rawDataSample.timestamp - m_initialTimeStamp).toString();
		aoiKeyFrame->aoi = *(iter.value());
		aoiKeyFrame->visible = visible;

		m_keyframes.append(aoiKeyFrame);
	}

	
}

//!
//! Creates keyframe for all the AOIs to be stored in the XML file
//!
void SMIEyeTrackerNode::createXmlKeyframe(SampleStruct rawDataSample)
{
	QHash<QString, AOIStruct*>::iterator iter;
	int enabledAOIs = 0;

	for (iter=m_aoiStructs.begin(); iter!=m_aoiStructs.end(); iter++) {

		QString aoiStrct_name = iter.key();
		QByteArray ba = aoiStrct_name.toAscii(); 
		char *aoi_name = ba.data();

		//showAllAOIValues();

		// to work with system time
		long long currentTimestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();





		if (iter.value()->position.x1 != 0 && iter.value()->position.x2 != 0 && iter.value()->position.y1 != 0 && iter.value()->position.y2 != 0 )
		{			
			// store the <KeyFrame> data for the XML
			keyframe *aoiKeyFrame = new keyframe;

			//aoiKeyFrame->timestamp = QVariant(rawDataSample.timestamp - m_initialTimeStamp).toString();	
			long long tStamp = (currentTimestamp - m_initialTimeStamp) * 1000;	// to convert to microseconds
			//aoiKeyFrame->timestamp = QVariant(currentTimestamp - m_initialTimeStamp).toString();			
			aoiKeyFrame->timestamp = QVariant(tStamp).toString();			
			aoiKeyFrame->aoi = *(iter.value());
			aoiKeyFrame->visible = "true";

			m_keyframes.append(aoiKeyFrame);
		}
		else if (iter.value()->position.x1 == 0 && iter.value()->position.x2 == 0 && iter.value()->position.y1 == 0 && iter.value()->position.y2 == 0) 
		{						
			// store the <KeyFrame> data for the XML
			keyframe *aoiKeyFrame = new keyframe;

			//aoiKeyFrame->timestamp = QVariant(rawDataSample.timestamp - m_initialTimeStamp).toString();			
			long long tStamp = (currentTimestamp - m_initialTimeStamp) * 1000;	// to convert to microseconds
			//aoiKeyFrame->timestamp = QVariant(currentTimestamp - m_initialTimeStamp).toString();			
			aoiKeyFrame->timestamp = QVariant(tStamp).toString();	
			aoiKeyFrame->aoi = *(iter.value());
			aoiKeyFrame->visible = "false";

			m_keyframes.append(aoiKeyFrame);
		}
		else if (iter.value()->position.x1 == 0 || iter.value()->position.x2 == 0 || iter.value()->position.y1 == 0 || iter.value()->position.y2 == 0)
		{
			//std::cout << " \n algun valor es 0 \n " << std::endl;
			return;
		}				
	}
}



//!!!!! 
//!!!!! Auxiliary Functions 
//!!!!! 

//!
//! Shows all the AOI values registered from the beginning of the recording.
//!
void SMIEyeTrackerNode::showAllAOIValues()
{
	QHash<QString, AOIStruct*>::iterator iter;

	for (iter=m_aoiStructs.begin(); iter!=m_aoiStructs.end(); iter++) {
		std::cout << iter.key().toStdString() << " " << iter.value()->position.x1 << " " << iter.value()->position.y1 << " " << iter.value()->position.x2 << " " << iter.value()->position.y2 << std::endl;
	}
}

//!
//! Returns the vertical difference (in pixels) between 2 points in the screen:
//! the center of the eyes of the character in the screen and the point in the screen where the user is looking at.
//!
double SMIEyeTrackerNode::getEyesVerticalDistance()
{
	//The upper point is 0. The lower is the same as the Y resolution of the screen
	double dif = m_eyeVCScreenCoordY - m_eyeUserScreenCoorY;

	if (abs(dif) < m_gaze_Offset)
		dif = 0.0;

	return dif;
}

//!
//! Returns the horizontal difference (in pixels) between 2 points in the screen:
//! the center of the eyes of the character in the screen and the point in the screen where the user is looking at.
//!
double SMIEyeTrackerNode::getEyesHorizontalDistance()
{
	//The leftmost point is 0. The rightmost is the same as the X resolution of the screen
	double dif = m_eyeVCScreenCoordX - m_eyeUserScreenCoorX;

	//if ( (dif > (m_gaze_Offset * -1.0)) && (dif < m_gaze_Offset) )
	if (abs(dif) < m_gaze_Offset)
		dif = 0.0;
	
	return dif;
}

//! 
//! Converts the pixel coordinates to milimeters, which is used to obtain the measure in the screen.
//!
double SMIEyeTrackerNode::convertPixelToMilim(double pixel, double screenSize, double screenRes)
{
	//std::cout << "pixel: " << pixel << std::endl;
	//std::cout << "screenSize: " << screenSize << std::endl;
	//std::cout << "screenRes: " << screenRes << std::endl;

	return ((pixel * screenSize) / screenRes);	
}

//!
//! Sets the threshold around the mid-point between the eyes, to make smoother movement of the head. 
//!
void SMIEyeTrackerNode::setGazeThreshold()
{
	m_gaze_Offset = getValue("Gaze Threshold").toDouble();
}

//! 
//! Writes the AOI parameters to an XML file,  which can then be loaded in BeGaze (software for eye tracking analysis from SMI)
//!
void SMIEyeTrackerNode::writeAOIsToXML()
{
	if (m_keyframes.size() == 0) {
		std::cout << " \n There are no keyframes stored. No XML is written. \n" << std::endl;
		return;
	}

	QString filename = m_path;
	filename.replace(QString("\\"), QString("\\\\"));
	filename = filename + "\\aoidata" + QVariant(m_filename).toString() + ".xml";	// change for a parameter

	// header of the xml file
	QDomDocument doc;
	QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0'");
	doc.appendChild(instr);

	// XML element: ArrayOfDynamicAOI
	QDomElement arrayAOIElement = addElement(doc, doc, "ArrayOfDynamicAOI");
	const QString ns("http://www.w3.org/2001/XMLSchema-instance");
	arrayAOIElement.setAttribute("xmlns:xsi", ns);
	const QString ns1("http://www.w3.org/2001/XMLSchema");
	arrayAOIElement.setAttribute("xmlns:xsd", ns1);

	// The loop begins iterating all the m_aoiRegions
	QHash<QString, AOIStruct*>::iterator iter;
	int i=0;

	for (iter=m_aoiStructs.begin(); iter!=m_aoiStructs.end(); iter++)
	{
		AOIStruct *aoi = iter.value();

		QDomElement dynamicAOIElement = addElement(doc, arrayAOIElement, "DynamicAOI");
		addElement(doc, dynamicAOIElement, "ID", QVariant(i).toString());		//changes 
		addElement(doc, dynamicAOIElement, "ParentID", "1");			//remains
		addElement(doc, dynamicAOIElement, "TrackingUsage", "None");	//remains
		addElement(doc, dynamicAOIElement, "Enabled", "true");			//remains
		addElement(doc, dynamicAOIElement, "Scope", "Global");			//remains
		addElement(doc, dynamicAOIElement, "Transparency", "50");		//remains

		// Points of the AOI 
		QDomElement pointsElement = addElement(doc, dynamicAOIElement, "Points");
		QDomElement pointElement = addElement(doc, pointsElement, "Point");
		addElement(doc, pointElement, "X", QVariant(aoi->position.x1).toString());			//changes all the time
		addElement(doc, pointElement, "Y", QVariant(aoi->position.y1).toString());			//changes all the time
		pointElement = addElement(doc, pointsElement, "Point");
		addElement(doc, pointElement, "X", QVariant(aoi->position.x2).toString());			//changes all the time
		addElement(doc, pointElement, "Y", QVariant(aoi->position.y2).toString());			//changes all the time

		addElement(doc, dynamicAOIElement, "BorderWidth", "2");			//remains
		addElement(doc, dynamicAOIElement, "Type", "Rectangle");		//remains
		addElement(doc, dynamicAOIElement, "Style", "HalfTransparent");	//remains
		addElement(doc, dynamicAOIElement, "Color", m_aoiColors.at(i));	//changes -> needs a list with colors names
		addElement(doc, dynamicAOIElement, "Name", aoi->aoiName);		//changes 
		addElement(doc, dynamicAOIElement, "CurrentTimestamp", "0");		//remains (?) 

		QDomElement fontElement = addElement(doc, dynamicAOIElement, "Font");	//remains
		addElement(doc, fontElement, "FontName", "Tahoma");
		addElement(doc, fontElement, "FontSize", "10");
		addElement(doc, fontElement, "FontStyle", "Regular");
		addElement(doc, fontElement, "FontUnit", "Point");

		addElement(doc, dynamicAOIElement, "ReferenceStimulusID", "0");
		addElement(doc, dynamicAOIElement, "MovieFrameRate", "25");
		addElement(doc, dynamicAOIElement, "PlaneGridRows", "0");
		addElement(doc, dynamicAOIElement, "PlaneGridColumns", "0");
		addElement(doc, dynamicAOIElement, "Visible", "true");

		//KeyFrames
		QDomElement keyFrameElement = addElement(doc, dynamicAOIElement, "KeyFrames");

		keyframe *keyf = m_keyframes.at(0);

		QDomElement keyframeEl = addElement(doc, keyFrameElement, "KeyFrame");
		QDomElement pointsEl = addElement(doc, keyframeEl, "Points");
		QDomElement pointEl = addElement(doc, pointsEl, "Point");
		addElement(doc, pointEl, "X", QVariant(keyf->aoi.position.x1).toString());
		addElement(doc, pointEl, "Y", QVariant(keyf->aoi.position.y1).toString());
		pointEl = addElement(doc, pointsEl, "Point");
		addElement(doc, pointEl, "X", QVariant(keyf->aoi.position.x2).toString());
		addElement(doc, pointEl, "Y", QVariant(keyf->aoi.position.y2).toString());

		addElement(doc, keyframeEl, "Visible", keyf->visible);
		addElement(doc, keyframeEl, "Timestamp", "0");
		addElement(doc, keyframeEl, "Angle", "0");
		addElement(doc, keyframeEl, "Area", "1");
		addElement(doc, keyframeEl, "ManuallyCreated", "false");

		for (int j=0; j<m_keyframes.size(); j++)
		{
			keyframe *keyf = m_keyframes.at(j);

			if ( strcmp(aoi->aoiName, keyf->aoi.aoiName) == 0 )
			{
				QDomElement keyframeEl = addElement(doc, keyFrameElement, "KeyFrame");
				QDomElement pointsEl = addElement(doc, keyframeEl, "Points");
				QDomElement pointEl = addElement(doc, pointsEl, "Point");
				addElement(doc, pointEl, "X", QVariant(keyf->aoi.position.x1).toString());
				addElement(doc, pointEl, "Y", QVariant(keyf->aoi.position.y1).toString());
				pointEl = addElement(doc, pointsEl, "Point");
				addElement(doc, pointEl, "X", QVariant(keyf->aoi.position.x2).toString());
				addElement(doc, pointEl, "Y", QVariant(keyf->aoi.position.y2).toString());

				addElement(doc, keyframeEl, "Visible", keyf->visible);
				addElement(doc, keyframeEl, "Timestamp", keyf->timestamp);
				addElement(doc, keyframeEl, "Angle", "0");
				
				int area = (keyf->aoi.position.x2 - keyf->aoi.position.x1) * (keyf->aoi.position.y2 - keyf->aoi.position.y1);
				addElement(doc, keyframeEl, "Area", QVariant(area).toString());
				addElement(doc, keyframeEl, "ManuallyCreated", "false");
			}
		}
		i++;
	}

	// xml stored in a QString
	QString xml = doc.toString();

	std::cout << "WRITE XML" << std::endl;

	// writing the xml file
	QFile file(filename);
	if (file.open(QIODevice::ReadWrite))
	{
		QTextStream stream(&file);
		stream << xml << endl;
	}
	file.close();
}

//! 
//! Generates a DOM element for the given DOM document and append it to the children of the given node.
//!
QDomElement SMIEyeTrackerNode::addElement(QDomDocument &doc, QDomNode &node, const QString &tag, const QString &value)
{
	QDomElement el = doc.createElement(tag);
	node.appendChild(el);
	if (!value.isNull())
	{
		QDomText txt = doc.createTextNode(value);
		el.appendChild(txt);
	}
	return el;
}

//!
//! Register all the possible colors for the AOIs to be displayed in BeGaze.
//!
void SMIEyeTrackerNode::registerAOIColors()
{
	m_aoiColors.append("NamedColor:CornflowerBlue");
	m_aoiColors.append("NamedColor:Coral");
	m_aoiColors.append("NamedColor:DarkSalmon");
	m_aoiColors.append("NamedColor:DarkRed");
	m_aoiColors.append("NamedColor:DarkOrchid");
	m_aoiColors.append("NamedColor:DarkOrange");
	m_aoiColors.append("NamedColor:DarkOliveGreen");
	m_aoiColors.append("NamedColor:DarkMagenta");
	m_aoiColors.append("NamedColor:DarkKhaki");
	m_aoiColors.append("NamedColor:DarkGreen");
	m_aoiColors.append("NamedColor:DarkGray");
	m_aoiColors.append("NamedColor:DarkGoldenrod");
	m_aoiColors.append("NamedColor:DarkCyan");
	m_aoiColors.append("NamedColor:DarkBlue");
	m_aoiColors.append("NamedColor:Cyan");
	m_aoiColors.append("NamedColor:Crimson");

}

//!
//! Verifies the path where the gaze data files (AOI, log, etc) will be stored. 
//!
bool SMIEyeTrackerNode::pathVerification() 
{
	bool withoutCalib	= getBoolValue("Start Device (without Calibration)");

	if (!m_path.isEmpty()) {
		
		/*if (withoutCalib) {
			QMessageBox msgBox;
			msgBox.setText("Verify that the path in \"Recorded Data File (path)\" is similar to where EXPERIMENT CENTER stores the result files for this experiment.");
			msgBox.setIcon(QMessageBox::Question);
			msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			int res = msgBox.exec();

			if (res == 0x00400000) 
				return false; 
		}*/

		// verify if path ends with slash or backslash
		if (m_path.at(m_path.length()-1) == '/' || m_path.at(m_path.length()-1) == '\\' )
			m_path.chop(1);
		// again, in case there is double slash or backslash
		if (m_path.at(m_path.length()-1) == '/' || m_path.at(m_path.length()-1) == '\\' )
			m_path.chop(1);
	}
	else {
		//verification of path
		QMessageBox msgBox;
		msgBox.setText("The path in \"Recorded Data File (path)\" cannot be empty.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return false;
	}
	return true;
}

//!
//! Searches for the SMI iView libraries that are needed to run this node.
//!
bool SMIEyeTrackerNode::eyeTrackerLibsVerification()
{
	QString curPath = QDir::currentPath();
	QFileInfoList files = QDir::current().entryInfoList();

	Frapper::Log::info(QString("Path SMI libs: \"%1\" ").arg(curPath), "SMIEyeTrackerNode::eyeTrackerLibsVerification");

	// These are the libraries needed from SMI SDK to run the eye tracker in 64-bits version.
	if ( !files.contains(QFileInfo("inpoutx64.dll")) || !files.contains(QFileInfo("iViewXAPI_NBS.dll")) || !files.contains(QFileInfo("iViewXAPI64.dll"))
		|| !files.contains(QFileInfo("JPEGcompression64.dll")) || !files.contains(QFileInfo("USBControllerCheck64.dll")) ) {

		//verification of path
		QMessageBox msgBox;
		msgBox.setText("The iView SDK for the SMI RED-Eye tracker needs to be installed. Read 'Eye_tracker_Manual.pdf' for more information.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return false;
	}

	return true;
}

} // namespace SMIEyeTrackerNode 