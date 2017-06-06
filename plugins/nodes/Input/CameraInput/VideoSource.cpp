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
//! \file "VideoSource.cpp"
//! \brief Implementation file for VideoSource class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       30.01.2012 (last updated)
//!
//!

#include "VideoSource.h"
#include <iostream>

namespace CameraInputNode {
using namespace cv;

///
/// Public Constructors
///
//!
//! Constructor of the VideoSource class.
//! \param camid The camid is used to setup VideoCapture. The camid says which cam should be used
//!
VideoSource::VideoSource(const int camid, const unsigned int fps /*= 0*/)
{
	m_cap = new VideoCapture(camid);

	//std::cout << getBuildInformation();

	if(m_cap->isOpened()){
		m_camAvailable = true;
		//m_cap->set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
		m_cap->set(CAP_PROP_FOURCC, CV_FOURCC('Y','U','Y','2'));
		//m_cap->set(CAP_PROP_FPS, 30.0);
		//m_cap->set(CAP_PROP_FPS, 25.0);
		m_cap->set(CAP_PROP_FRAME_WIDTH, 1280);
		m_cap->set(CAP_PROP_FRAME_HEIGHT, 720);

		//m_cap->set(CAP_PROP_AUTO_EXPOSURE, 0.0);
		//if (fps > 0)
			//m_cap->set(CAP_PROP_FPS, (double) fps);
	} else {
		m_camAvailable = false;
	}
}

//!
//! Destructor of the VideoSource class.
//!
VideoSource::~VideoSource()
{
	if(m_cap->isOpened()){
		m_cap->release();
	}
}

//!
//!
//!
VideoCapture* VideoSource::getCaptureDevice()
{
	return m_cap;
}

//!
//! This static function is used to check if camera with given ID is available
//! \param The camid to check if available
//! \return true if Camera with ID i is available
//!
bool VideoSource::checkCamera(const int camid)
{
	VideoCapture test(camid);
	bool ret;
	if(!test.isOpened()){
		ret = false;
	} else {
		test.release();
		ret = true;
	}
	return ret;
}

//!
//! Is used to grab a ColorImage from the camera
//! \return Mat newColorFrame from the camera. The returned matrix has 4 Channels (=BGRA)
//!
const Mat &VideoSource::getNewColorImage()
{
	*m_cap >> m_newColorFrame;
	cvtColor(m_newColorFrame, m_newColorFrame, CV_8U, 3);
	return m_newColorFrame;
}

//!
//! Is used to grab a GrayImage from the camera
//! \return Mat newGrayFrame from the camera. The returned matrix has 1 Channel
//!
const Mat &VideoSource::getNewGrayImage()
{
	*m_cap >> m_newGrayFrame;
	cvtColor(m_newGrayFrame, m_newGrayFrame, CV_RGB2GRAY, 1);
	return m_newGrayFrame;
}

//!
//! Is used to grab a RawImage from the camera
//! \return Mat newRawFrame from the camera. 
//!
const Mat &VideoSource::getNewRawImage()
{
	*m_cap >> m_newRawFrame;
	return m_newRawFrame;
}

//!
//! Is used to grab a colorImage and a RawImage
//! \param The colorImage
//! \param The rawImage
//!
void VideoSource::getRawAndColorImage(Mat &colorimage, Mat &raw)
{
	*m_cap >> raw;
	cvtColor(raw, colorimage, CV_8U, 3);
}

//!
//! \return True if Camera is available
//!
const bool VideoSource::isCamAvailable() const
{
	return m_camAvailable;
}

//!
//!
//!
const double VideoSource::getFPS() const
{
	return m_cap->get(CV_CAP_PROP_FPS)/1000;
}

//!
//! Sets the fps for the capturing
//!
void VideoSource::setFPS(const unsigned int fps)
{
	//m_cap->set(CV_CAP_PROP_FPS, (double)fps);
}

//!
//! Sets the exposure value for the capturing
//!
void VideoSource::setExposure(const float exposure)
{
	m_cap->set(CV_CAP_PROP_EXPOSURE, (double)exposure);
}

//!
//! Sets the exposure value for the capturing
//!
void VideoSource::setAutoExposure(const bool autoexposure)
{
	m_cap->set(CV_CAP_PROP_AUTO_EXPOSURE, (double)autoexposure);
}

//!
//! Shows the camera specific configuration dialogue
//!
void VideoSource::openCamSettings() const
{
	m_cap->set(CV_CAP_PROP_SETTINGS, 1.0);
}

} // namespace CameraInputNode
