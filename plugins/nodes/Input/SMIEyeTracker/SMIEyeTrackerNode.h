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
//! \file "SMIEyeTrackerNode.h"
//! \brief Header file SMIEyeTrackerNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    1.0
//! \date       04.07.2014 (last updated)
//!

#ifndef SMIEYETRACKERNODE_H
#define SMIEYETRACKERNODE_H


#include "Node.h"
#include <stddef.h>  /* Needed for the size_t definition. */
#include <string.h>  /* Used for: strcat, strcpy*/
#include <QString.h>
#include <math.h>

#include "iViewXAPI.h"
#include <Windows.h>

//#include "EyeTrackerDevice.h"

#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QByteArray>
#include <QtCore/QQueue>
#include <QtCore/QThread>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QMessageBox>
#include <QDateTime>

#include <QtNetwork/QUdpSocket>

#define DEBUG_TIMING


namespace SMIEyeTrackerNode {


int __stdcall SampleCallbackFunction(SampleStruct sampleData);
int __stdcall AOICallbackFunction(int outValue);

using namespace Frapper;

//! 
//! Struct that stores info needed to create a <Keyframe> in the XML.
//!
struct keyframe{
	AOIStruct aoi;
	QString visible;
	QString timestamp;
};


//!
//! Class representing nodes that will perform eye tracking.
//!
class SMIEyeTrackerNode : public Node
{
    Q_OBJECT
    ADD_INSTANCE_COUNTER

public:
    SMIEyeTrackerNode ( QString name, ParameterGroup *parameterRoot );
    ~SMIEyeTrackerNode ();

public slots:
	
	void	initializeDevice(QString ipStimulus, QString ipEyetracker, int calibrationPoints, int calibrationSpeed, int displayDevice, bool withCalibration);
	void	startDevice();
	void	stopDevice(QString filename);
	void	pauseTrack();
	void	recordData();
	void	setGazeValues();
	void	setGazeThreshold();
	void	setGazeControl();
	void	sendMessage();
	double	getEyesVerticalDistance();
	double  getEyesHorizontalDistance();
	double	convertPixelToMilim(double pixel,double screenSize, double screenRes);

	void	createAOIs();
	void	destroyAOI(QString regionName);	

	void	writeAOIsToXML();
	void	saveAOIValues();

private:
	//!
	//! Sets the calibration and validation visualization parameter. 
	//!
	void calibration(int calibrationPoints, int calibrationSpeed, int displayDevice);

	//!
	//! Initiates the tracking in the eye tracker. 
	//!
	void tracking();

	//!
	//! Pauses the tracking in the eye tracker. 
	//!
	void pauseTracking();

	//!
	//! Resumes the tracking in the eye tracker. 
	//!
	void resumeTracking();

	//! 
	//! Starts/Resumes gaze data recording (from the iView X SDK manual). 
	//! It also sends a message to the eye tracker with data from the animation 
	//!
	void startRecording();

	//!
	//! Pauses gaze data recording (from the iView X SDK manual). 
	//!
	void pauseRecording();

	//!
	//! Initialization of the parameters. 
	//!
	void initialization();

	//!
	//! Register all the possible colors for the AOIs to be displayed in BeGaze.
	//!
	void registerAOIColors();

	//!
	//! Verifies the path where the gaze data files (AOI, log, etc) will be stored. 
	//!
	bool pathVerification();

	//!
	//! Creates a keyframe for the AOI to be stored in the XML file
	//!
	void createXmlKeyframe(SampleStruct rawDataSample, AOIStruct* aoiStruct, QString visible);

	void createXmlKeyframe(SampleStruct rawDataSample, QString visible);

	void createXmlKeyframe(SampleStruct rawDataSample);

	//!
	//! Shows all the AOI values registered from the beginning of the recording.
	//!
	void showAllAOIValues();

	//! 
	//! Generates a DOM element for the given DOM document and append it to the children of the given node.
	//!
	QDomElement addElement(QDomDocument &doc, QDomNode &node, const QString &tag, const QString &value = QString::null);

	//!
	//! Verifies the existence of the SMI - Eye tracker libraries and displays a message for installation.
	//!
	bool eyeTrackerLibsVerification();

private:
	
	AOIStruct		*m_aoiEye;

	ParameterGroup	*parameterRoot;				// pointer to this Parameter Root
	ParameterGroup  *m_aoiRegionsGroup;	

	QString			m_ipAddressST_PC;			// IP address of the pc where the stimulus is run
	QString			m_ipAddressET_NB;			// IP address of the notebook/pc where the eye tracker is run
	QString			m_filename;					// name of file based on initial timestamp
	QString			m_path;						// path where to save the files (AOI xml, etc.)
	QTimer			*m_timer;					// timer to trigger an action
	QTimer			*m_timerAOI;				// timer to update the AOIs
	QUdpSocket		*udpSocket;
	QList<QString>	m_aoiColors;
	QList<keyframe*>	m_keyframes;	// for purposes of the XML with the AOIs
	QList<AOIStruct*>			m_aoiRegions;
	QHash<QString, AOIStruct*>	m_aoiStructs;
	
	double			m_gaze_Offset;				// gaze offset to normalize the gaze
	double			m_eyeVCScreenCoordX;	//TEST - position in screen of the pupil of one VC's eye
	double			m_eyeVCScreenCoordY;	//TEST - position in screen of the pupil of one VC's eye
	double			m_screenResX;
	double			m_screenResY;
	double			m_screenWidth;
	double			m_screenHeight;		
	unsigned int	m_port;
	long long		m_initialTimeStamp;
	bool			m_recordingStarted;	
	bool			m_propagateDirty;
	bool			m_eyetrackerconnected;	

	SampleStruct rawDataSample;
};

} // end namespace

#endif
