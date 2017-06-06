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
//! \file "VideoSource.h"
//! \brief Header file for VideoSource class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       30.01.2012 (last updated)
//!
//!
#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#ifdef _WIN32
//#include "cv.h"         // from OpenCV
//#include "cxcore.h"     // from OpenCV
//#include "highgui.h"    // from OpenCV, for CvCapture, cvReleaseCapture, cvCaptureFromCAM, cvGrabFrame, cvRetrieveFrame, ...
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

namespace CameraInputNode {
using namespace cv;

//!
//! Class representing VideoSource
//!
class VideoSource
{
public: //constructors and destructors
	//!
	//! Constructor of the VideoSource class.
	//! \param camid The camid is used to setup VideoCapture. The camid says which cam should be used
	//!
	VideoSource(const int camid, const unsigned int fps = 0);

	//!
	//! Destructor of the VideoSource class.
	//!
	~VideoSource();

public: // Public Methods

	//!
	//! This static function is used to check if camera with given ID is available
	//! \param The camid to check if available
	//! \return true if Camera with ID i is available
	//!
	static bool checkCamera(const int camid);

	//!
	//!
	//!
	VideoCapture* getCaptureDevice();

	//!
	//! Is used to get a actual Mat from the camera
	//! \return Mat New Mat from the camera. The returned matrix has 4 Channels (=BGRA)
	//!
	const Mat &getNewColorImage();

	//!
	//! Is used to grab a newGrayFrame from the camera
	//! \return Mat newGrayMat from the camera. The returned matrix has 1 Channel
	//!
	const Mat &getNewGrayImage();

	//!
	//! Is used to grab a RawImage from the camera
	//! \return Mat newRawFrame from the camera. 
	//!
	const Mat &getNewRawImage();

	//!
	//! Is used to grab a colorImage and a RawImage
	//! \param The colorImage
	//! \param The rawImage
	//!
	void getRawAndColorImage(Mat &colorimage, Mat &raw);

	//!
	//! \return True if Camera is Opened
	//!
	const bool isCamAvailable() const;

	//!
	//! \return The actual fps the system captures with.
	//!
	const double getFPS() const;

	//!
	//! Sets the fps for the capturing
	//!
	void setFPS(const unsigned int fps);

	//!
	//! Sets the auto exposure value for the capturing
	//!
	void setAutoExposure(const bool autoexposure);

	//!
	//! Sets the exposure value for the capturing
	//!
	void setExposure(const float exposure);

	//!
	//! Shows the camera specific configuration dialogue
	//!
	void openCamSettings() const; 

private: //data
	//!
	//! Reference to the VideoCapture
	//!
	VideoCapture* m_cap;
	//!
	//! Is used to set if Camera can be accessed  
	//!
	bool m_camAvailable;
	//!
	//! Is used to store the grabbed ColorImage from cam
	//!
	Mat m_newColorFrame;
	//!
	//! Is used to store the grabbed GrayImage from cam
	//!
	Mat m_newGrayFrame;
	//!
	//! Is used to store the grabbed newRawFrame from cam
	//!
	Mat m_newRawFrame;
};

} // namespace CameraInputNode

#endif
