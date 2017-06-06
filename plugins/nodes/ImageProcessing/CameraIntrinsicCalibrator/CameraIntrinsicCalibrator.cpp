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
//! \file "CameraIntrinsicCalibrator.cpp"
//! \brief CameraIntrinsicCalibrator
//!
//! \author		Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!

#include "CameraIntrinsicCalibrator.h"
#include "qmessagebox.h"

namespace CameraIntrinsicCalibrator {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the CameraIntrinsicCalibrator class.
//!
//! \param name The name to give the new mesh node.
//!
CameraIntrinsicCalibrator::CameraIntrinsicCalibrator ( QString name, ParameterGroup *parameterRoot) :
	ImageNode(name, parameterRoot)
{
	imagePathes = new QStringList(0);
	setCommandFunction("Calibrate!", SLOT(run()));
}

//!
//! Destructor of the CameraIntrinsicCalibrator class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CameraIntrinsicCalibrator::~CameraIntrinsicCalibrator ()
{
}

///
/// Private Slots
///


void CameraIntrinsicCalibrator::run()
{
	//load file Pathes
	QString pathListString = getValue("Calibation Frames",true).toString();
	imagePathes = new QStringList(pathListString.split(";",QString::SkipEmptyParts));

	//Target image Size
	cv::Size targetSize(1920,1080);
	targetSize.width = getValue("target width",true).toInt();
	targetSize.height = getValue("target height",true).toInt();

	// rows-1 & colums-1 of captured grid
	int boardWidth = getValue("Chessboard Columns", true).toInt()-1;
	int boardHeight = getValue("Chessboard Rows", true).toInt()-1;
	cv::Size boardSize(boardWidth,boardHeight);

	//vector storing board coordiantes of each frame
	cv::vector<cv::vector<cv::Point2f>> imgpt;

	//temporary variables storing currently processed image
	cv::Mat view, viewGray;
	//Output Variables
    cv::Mat cameraMatrix, distCoeffs, R, P;

	//initialize cameraMatrix & distortion Coefficients for each camera;
	//	|1 0 0|
	//  |0 1 0|		&	[0, 0, 0, 0, 0, 0, 0, 0]
	//	|0 0 1|
    cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    distCoeffs = cv::Mat::zeros(cv::Size(8, 1), CV_64F);
	//initialize output
	QString output;
	output = QString::number(cameraMatrix.at<double>(cv::Point(0,0)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(1,0)))+";"+QString::number(targetSize.width/2)+";";
	output = output+QString::number(cameraMatrix.at<double>(cv::Point(0,1)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(1,1)))+";"+QString::number(targetSize.height/2)+";";
	output = output+QString::number(cameraMatrix.at<double>(cv::Point(0,2)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(1,2)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(2,2)))+",";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(0,0)))+";";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(1,0)))+";";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(2,0)))+";";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(3,0)))+";";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(4,0)))+";";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(5,0)))+";";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(6,0)))+";";
	output = output+QString::number(distCoeffs.at<double>(cv::Point(7,0)))+";";
	setValue("Camera Intrisics",output,false);

	//find Chessboard in each frame
    for(int i = 0; i < imagePathes->size(); i++ )
    {
   		viewGray = cv::imread(imagePathes->at(i).toLatin1().data(), CV_LOAD_IMAGE_GRAYSCALE);
		//resize image to target size while keeping aspect ratio
		cv::Mat temp = cv::Mat::zeros(targetSize,CV_8U);
		double factor = (double)targetSize.height/(double)viewGray.rows;
		cv::resize(viewGray,viewGray,cv::Size(),factor,factor,cv::INTER_CUBIC);
		cv::Rect roi( (targetSize.width-viewGray.cols)/2 ,0, viewGray.cols, viewGray.rows );
		cv::Mat destinationROI = temp( roi );
		viewGray.copyTo( destinationROI );
		viewGray = temp;

        if(viewGray.data)
        {
            cv::vector<cv::Point2f> ptvec;
            bool found = cv::findChessboardCorners( viewGray, boardSize, ptvec, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
            if( found )
            {
				//refine result
				cv::cornerSubPix( viewGray, ptvec, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.01 ));
                imgpt.push_back(ptvec);
            }
        }
    }
	
    //calibrate camera
    cv::vector<cv::vector<cv::Point3f> > objpt(1);
	objpt[0].resize(0);
	//setup chessboard corners (clean)
	for( int i = 0; i < boardSize.height; i++ )
	{
		for( int j = 0; j < boardSize.width; j++ )
		{
			objpt[0].push_back(cv::Point3f(float(i),float(j), 0.0f));
		}
	}
	//check if the grid was found on at least one image
    if( !(imgpt.size() < 1) )
    {
		objpt.resize(imgpt.size(),objpt[0]);
		cv::vector<cv::Mat> rvecs, tvecs;
		double err = cv::calibrateCamera(objpt, imgpt, targetSize, cameraMatrix, distCoeffs, rvecs, tvecs,	CV_CALIB_FIX_ASPECT_RATIO |
																											CV_CALIB_FIX_PRINCIPAL_POINT |
																											CV_CALIB_ZERO_TANGENT_DIST |
																											CV_CALIB_FIX_K1 |
																											CV_CALIB_FIX_K2 |
																											CV_CALIB_FIX_K3 |
																											CV_CALIB_FIX_K4 |
																											CV_CALIB_FIX_K5 |
																											CV_CALIB_FIX_K6);
		bool ok = cv::checkRange(cameraMatrix) && checkRange(distCoeffs);
		if(!getValue("enable undistort").toBool())
		{
			//reset distortion coefficents as undistort is disabled
			distCoeffs = cv::Mat::zeros(cv::Size(8, 1), CV_64F);
		}
		if(ok)
		{
			//grids detected & range is valid
			//write data to String for transmission
			output = QString::number(cameraMatrix.at<double>(cv::Point(0,0)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(1,0)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(2,0)))+";";
			output = output+QString::number(cameraMatrix.at<double>(cv::Point(0,1)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(1,1)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(2,1)))+";";
			output = output+QString::number(cameraMatrix.at<double>(cv::Point(0,2)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(1,2)))+";"+QString::number(cameraMatrix.at<double>(cv::Point(2,2)))+",";
			for(int i = 0; i < distCoeffs.cols; i++)
			{
				output = output+QString::number(distCoeffs.at<double>(cv::Point(i,0)))+";";
			}
			setValue("Camera Intrisics",output,false);

			if(getValue("writeout undistored image").toBool())
			{
				std::string path = QString(getValue("outputpath").toString()+"/undistorted_"+getValue("suffix").toString()+".jpg").toLatin1().data();
				cv::Mat image = cv::imread(imagePathes->at(0).toLatin1().data(), CV_LOAD_IMAGE_COLOR);
				cv::Mat temp = image.clone();
				cv::undistort(temp,image,cameraMatrix,distCoeffs);
				cv::imwrite(path,image);
			}
			Log::info("camera calibrated, range ckeck ok","CameraInstrinctCalibrator");
		}
		else
		{
			Log::error("camera not calibrated, range ckeck failed","CameraInstrinctCalibrator");
			printf("Error: not enough views for camera.");
		}
	}
	else
	{
		Log::error("camera not calibrated, grid not detected","CameraInstrinctCalibrator");
		printf("Error: not enough views for camera.");
	}
}// run
} // namespace CameraIntrinsicCalibrator
