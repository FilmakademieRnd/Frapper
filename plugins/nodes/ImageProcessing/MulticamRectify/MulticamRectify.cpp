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
//! \file "MulticamRectify.cpp"
//! \brief Camera Input
//!
//! \author		Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!

#include "MulticamRectify.h"
#include "qmessagebox.h"

namespace MulticamRectify {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the MulticamRectify class.
//!
//! \param name The name to give the new mesh node.
//!
MulticamRectify::MulticamRectify ( QString name, ParameterGroup *parameterRoot) :
	ImageNode(name, parameterRoot)
{
	m_filenameParameter = new FilenameParameter("Output Textfilename","");	
	m_filenameParameter->setType(FilenameParameter::FT_Save);
	m_filenameParameter->setFilters("Textfile (*.txt)");
	parameterRoot->addParameter(m_filenameParameter);

	imagePathes = new QVector<QStringList>(0);

	setCommandFunction("Calibrate!", SLOT(run()));
}

//!
//! Destructor of the MulticamRectify class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MulticamRectify::~MulticamRectify ()
{
}

///
/// Private Slots
///


void MulticamRectify::run()
{
	//output variables
	std::vector<cv::Mat> cam12(5);
	std::vector<cv::Mat> cam13(5);
	std::vector<cv::Mat> cam14(5);

	// variables for directed pitch (pitch of epipolar line)
	float pitch = 0;
	float xDirection = 0;

	//vector storing transformed image matches
	std::vector< cv::Point2f > transformed_matches(0);

	//maps used to rectify sattelit camera's image
	cv::Mat map11,map12,map21,map22;

	//to be calculated parameters
	cv::Mat R, T, E, F;
	cv::Mat R0, R1, P0, P1, Q;

	//use Grid images or try estimation through Feature points
	bool useGrid = true;

	//choose rectification method
	bool useUncalibratedRectification = getValue("use rectify uncalibrated").toBool();
	//Target image Size
	cv::Size targetSize(1920,1080);
	targetSize.width = getValue("target width",true).toInt();
	targetSize.height = getValue("target height",true).toInt();

	cv::Mat cameraMatrix[4], distCoeffs[4];
	//set cameraMatrix & distortion Coefficients for each camera;
	//	|1 0 w/2|
	//  |0 1 h/2|		&	[0, 0, 0, 0, 0, 0, 0, 0]
	//	|0 0 1|
    for(int i = 0; i < 4; i++ )
    {
        cameraMatrix[i] = cv::Mat_<double>::eye(3,3);
		cameraMatrix[i].at<double>(cv::Point(2,0)) = targetSize.width/2;
		cameraMatrix[i].at<double>(cv::Point(2,1)) = targetSize.height/2;
        distCoeffs[i] = cv::Mat_<double>::zeros(8,1);
    }


	//load file Pathes
	QString pathListString = getValue("Camera Grids principal",true).toString();
	QStringList cam1 = QStringList(pathListString.split(";",QString::SkipEmptyParts));
	imagePathes->push_back(cam1);

	pathListString = getValue("Camera Grids 2",true).toString();
	QStringList cam2 = QStringList(pathListString.split(";",QString::SkipEmptyParts));
	imagePathes->push_back(cam2);

	pathListString = getValue("Camera Grids 3",true).toString();
	QStringList cam3 = QStringList(pathListString.split(";",QString::SkipEmptyParts));
	imagePathes->push_back(cam3);

	pathListString = getValue("Camera Grids 4",true).toString();
	QStringList cam4 = QStringList(pathListString.split(";",QString::SkipEmptyParts));
	imagePathes->push_back(cam4);

	//load camera intrinsics
	QString intrinsicString = "";
	QStringList camIntrinsic(0);
	intrinsicString = getValue("Camera Intrinsic principal",true).toString();
	camIntrinsic = QStringList(intrinsicString.split(",",QString::SkipEmptyParts));
	if(camIntrinsic.length() == 2)
	{
		QStringList camMat = camIntrinsic.at(0).split(";",QString::SkipEmptyParts);
		QStringList distortionCoeffs = camIntrinsic.at(1).split(";",QString::SkipEmptyParts);
		if(camMat.length() == 9)
		{
			cameraMatrix[0] = (Mat_<double>(3,3) <<
							camMat[0].toDouble() , camMat[1].toDouble(), camMat[2].toDouble(),
							camMat[3].toDouble() , camMat[4].toDouble(), camMat[5].toDouble(),
							camMat[6].toDouble() , camMat[7].toDouble(), camMat[8].toDouble());
		}
		if(distortionCoeffs.length() > 4)
		{
			if(distortionCoeffs.length() == 8)
			{
			distCoeffs[3] = (Mat_<double>(8,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble(), distortionCoeffs[5].toDouble(), distortionCoeffs[6].toDouble() , distortionCoeffs[7].toDouble());
			}
			if(distortionCoeffs.length() == 5)
			{
			distCoeffs[3] = (Mat_<double>(5,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble());
			}
		}
	}

	intrinsicString = "";
	camIntrinsic = QStringList(0);
	intrinsicString = getValue("Camera Intrinsic 2",true).toString();
	camIntrinsic = QStringList(intrinsicString.split(",",QString::SkipEmptyParts));
	if(camIntrinsic.length() == 2)
	{
		QStringList camMat = camIntrinsic.at(0).split(";",QString::SkipEmptyParts);
		QStringList distortionCoeffs = camIntrinsic.at(1).split(";",QString::SkipEmptyParts);
		if(camMat.length() == 9)
		{
			cameraMatrix[1] = (Mat_<double>(3,3) <<
							camMat[0].toDouble() , camMat[1].toDouble(), camMat[2].toDouble(),
							camMat[3].toDouble() , camMat[4].toDouble(), camMat[5].toDouble(),
							camMat[6].toDouble() , camMat[7].toDouble(), camMat[8].toDouble());
		}
		if(distortionCoeffs.length() > 4)
		{
			if(distortionCoeffs.length() == 8)
			{
			distCoeffs[3] = (Mat_<double>(8,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble(), distortionCoeffs[5].toDouble(), distortionCoeffs[6].toDouble() , distortionCoeffs[7].toDouble());
			}
			if(distortionCoeffs.length() == 5)
			{
			distCoeffs[3] = (Mat_<double>(5,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble());
			}
		}
	}

	intrinsicString = "";
	camIntrinsic = QStringList(0);
	intrinsicString = getValue("Camera Intrinsic 3",true).toString();
	camIntrinsic = QStringList(intrinsicString.split(",",QString::SkipEmptyParts));
	if(camIntrinsic.length() == 2)
	{
		QStringList camMat = camIntrinsic.at(0).split(";",QString::SkipEmptyParts);
		QStringList distortionCoeffs = camIntrinsic.at(1).split(";",QString::SkipEmptyParts);
		if(camMat.length() == 9)
		{
			cameraMatrix[2] = (Mat_<double>(3,3) <<
							camMat[0].toDouble() , camMat[1].toDouble(), camMat[2].toDouble(),
							camMat[3].toDouble() , camMat[4].toDouble(), camMat[5].toDouble(),
							camMat[6].toDouble() , camMat[7].toDouble(), camMat[8].toDouble());
		}
		if(distortionCoeffs.length() > 4)
		{
			if(distortionCoeffs.length() == 8)
			{
			distCoeffs[3] = (Mat_<double>(8,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble(), distortionCoeffs[5].toDouble(), distortionCoeffs[6].toDouble() , distortionCoeffs[7].toDouble());
			}
			if(distortionCoeffs.length() == 5)
			{
			distCoeffs[3] = (Mat_<double>(5,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble());
			}
		}
	}

	intrinsicString = "";
	camIntrinsic = QStringList(0);
	intrinsicString = getValue("Camera Intrinsic 4",true).toString();
	camIntrinsic = QStringList(intrinsicString.split(",",QString::SkipEmptyParts));
	if(camIntrinsic.length() == 2)
	{
		QStringList camMat = camIntrinsic.at(0).split(";",QString::SkipEmptyParts);
		QStringList distortionCoeffs = camIntrinsic.at(1).split(";",QString::SkipEmptyParts);
		if(camMat.length() == 9)
		{
			cameraMatrix[3] = (Mat_<double>(3,3) <<
							camMat[0].toDouble() , camMat[1].toDouble(), camMat[2].toDouble(),
							camMat[3].toDouble() , camMat[4].toDouble(), camMat[5].toDouble(),
							camMat[6].toDouble() , camMat[7].toDouble(), camMat[8].toDouble());
		}
		if(distortionCoeffs.length() > 4)
		{
			if(distortionCoeffs.length() == 8)
			{
			distCoeffs[3] = (Mat_<double>(8,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble(), distortionCoeffs[5].toDouble(), distortionCoeffs[6].toDouble() , distortionCoeffs[7].toDouble());
			}
			if(distortionCoeffs.length() == 5)
			{
			distCoeffs[3] = (Mat_<double>(5,1) <<
							distortionCoeffs[0].toDouble() , distortionCoeffs[1].toDouble(), distortionCoeffs[2].toDouble(), distortionCoeffs[3].toDouble(), 
							distortionCoeffs[4].toDouble());
			}
		}
	}

	// output file Name
	std::string outputFilename = m_filenameParameter->getValue().toString().toLatin1().data();
	minFrameCount = imagePathes->at(0).length();
	//compute minimal number of frames per camera
	for (int i = 1; i < 4; i++)
	{
		if(imagePathes->at(i).length() < minFrameCount) 
			minFrameCount = imagePathes->at(i).length();
	}

	// rows-1 & colums-1 of captured grid
	int boardWidth = getValue("Chessboard Columns", true).toInt()-1;
	int boardHeight = getValue("Chessboard Rows", true).toInt()-1;
	cv::Size boardSize(boardWidth,boardHeight);

	//vector storing board coordiantes of each frame of each camera
	cv::vector<cv::vector<cv::Point2f> > imgpt[4];
	//temporary variables storing currently processed image
	cv::Mat viewGray;

	for(int i = 0; i < 4; i++ )
        imgpt[i].resize(minFrameCount);

	//find Chessboard
	//each frame
    for(int i = 0; i < minFrameCount; i++ )
    {
		//each camera
        for(int k = 0; k < 4; k++ )
        {
   			viewGray = cv::imread(imagePathes->at(k).at(i).toLatin1().data(), CV_LOAD_IMAGE_GRAYSCALE);
			//resize all images to target size before
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
                    imgpt[k][i].resize(ptvec.size());
                    std::copy(ptvec.begin(), ptvec.end(), imgpt[k][i].begin());
                }
            }
        }
    }

    cv::vector<cv::vector<cv::Point3f> > objpt(1);
    cv::vector<cv::vector<cv::Point2f> > imgptnew;
	objpt[0].resize(0);
	//setup chessboard corners (clean)
	for( int i = 0; i < boardSize.height; i++ )
	{
		for( int j = 0; j < boardSize.width; j++ )
		{
			objpt[0].push_back(cv::Point3f(float(i),float(j), 0.0f));
		}
	}

    cv::vector<cv::vector<cv::Point2f> > imgptSatellit;
    
	//calibrate (2,1) , (3,1) and (4,1) pairs
    for(int c = 2; c <= 4; c++ )
    {
		//get all valid relations
        imgptnew.clear();
        imgptSatellit.clear();

		for(int i = 0; i < (int)qMin(imgpt[0].size(), imgpt[c-1].size()); i++ )
            if( !imgpt[0][i].empty() && !imgpt[c-1][i].empty() )
            {
                imgptnew.push_back(imgpt[0][i]);
                imgptSatellit.push_back(imgpt[c-1][i]);
            }

		//check if enough grids have been detected
		if( imgptnew.size() < 2 )
		{
			printf("Error: not enough shared views for cameras 1 and %d\n", c);
			useGrid = false;
		}

		if(useGrid)
		{
			objpt.resize(imgptnew.size(),objpt[0]);

			//Output Variables
			/*std::cout << "cam1: " << cameraMatrix[0] << std::endl;
			std::cout << "dist1: " << distCoeffs[0] << std::endl;
			std::cout << "cam2: " << cameraMatrix[c-1] << std::endl;
			std::cout << "dist2: " << distCoeffs[c-1] << std::endl;*/
			double err = cv::stereoCalibrate(objpt, imgptSatellit, imgptnew, cameraMatrix[c-1], distCoeffs[c-1],
										cameraMatrix[0], distCoeffs[0],
										targetSize, R, T, E, F,
										cv::TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5),
										CV_CALIB_USE_INTRINSIC_GUESS | 
										CV_CALIB_FIX_PRINCIPAL_POINT | 
										CV_CALIB_ZERO_TANGENT_DIST | 
										CV_CALIB_FIX_ASPECT_RATIO | 
										CV_CALIB_FIX_K1 | 
										CV_CALIB_FIX_K2 | 
										CV_CALIB_FIX_K3 | 
										CV_CALIB_FIX_K4 | 
										CV_CALIB_FIX_K5 |
										CV_CALIB_FIX_K6);

			/*std::cout << "cam1_after: " << cameraMatrix[0] << std::endl;
			std::cout << "dist1_after: " << distCoeffs[0] << std::endl;
			std::cout << "cam2_after: " << cameraMatrix[c-1] << std::endl;
			std::cout << "dist2_after: " << distCoeffs[c-1] << std::endl;
			std::cout << "translation: " << T << std::endl;*/

			cv::stereoRectify(cameraMatrix[0],distCoeffs[0],cameraMatrix[c-1],distCoeffs[c-1],targetSize,R,T,R0,R1,P0,P1,Q,CV_CALIB_ZERO_DISPARITY);
			//compensate rotation
			cv::Mat pcam_selected = cv::Mat::zeros(4,1,CV_32FC2);
			cv::Mat scam_selected = cv::Mat::zeros(4,1,CV_32FC2);

			//extract 4 edge points
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+0*pcam_selected.channels()+0] = 0;
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+0*pcam_selected.channels()+1] = 0;
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+1*pcam_selected.channels()+0] = targetSize.width;
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+1*pcam_selected.channels()+1] = 0;
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+2*pcam_selected.channels()+0] = 0;
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+2*pcam_selected.channels()+1] = targetSize.height;
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+3*pcam_selected.channels()+0] = targetSize.width;
			((float*)(pcam_selected.data))[0*pcam_selected.step1()+3*pcam_selected.channels()+1] = targetSize.height;

			pcam_selected.copyTo(scam_selected);

			//apply previousely calculated transformations on the points (that have also been applied to the images by initUndistortRectifyMap)
			cv::undistortPoints(pcam_selected,pcam_selected,cameraMatrix[0],distCoeffs[0],R0,P0);
			cv::undistortPoints(scam_selected,scam_selected,cameraMatrix[c-1],distCoeffs[c-1],R1,P1);
		
			//calculate rotation & scaling
			float scaling = 0;
			cv::Mat rotation = cv::Mat::eye(3,3,CV_32F);
			for( int i = 1; i < 4; i++ )
			{
				float xDiff = qAbs(((float*)(pcam_selected.data))[0*pcam_selected.step1()+0*pcam_selected.channels()+0] - ((float*)(pcam_selected.data))[0*pcam_selected.step1()+i*pcam_selected.channels()+0]);
				float yDiff = qAbs(((float*)(pcam_selected.data))[0*pcam_selected.step1()+0*pcam_selected.channels()+1] - ((float*)(pcam_selected.data))[0*pcam_selected.step1()+i*pcam_selected.channels()+1]);
				float distanceOriginal = sqrtf((xDiff*xDiff)+(yDiff*yDiff));
				xDiff = qAbs(((float*)(pcam_selected.data))[0*pcam_selected.step1()+0*pcam_selected.channels()+0] - ((float*)(pcam_selected.data))[0*pcam_selected.step1()+i*pcam_selected.channels()+0]);
				yDiff = qAbs(((float*)(pcam_selected.data))[0*pcam_selected.step1()+0*pcam_selected.channels()+1] - ((float*)(pcam_selected.data))[0*pcam_selected.step1()+i*pcam_selected.channels()+1]);
				float distanceNew = sqrtf((xDiff*xDiff)+(yDiff*yDiff));
				scaling += distanceNew/distanceOriginal;
			}
			scaling /= 3;

			//apply scaling & rotation
			cv::Mat targetCameraMatrix = cv::Mat::zeros(3,3,cameraMatrix[0].type());
			cameraMatrix[0].copyTo(targetCameraMatrix);
			targetCameraMatrix = targetCameraMatrix*rotation;
			((double*)(targetCameraMatrix.data))[0*targetCameraMatrix.step1()+0*targetCameraMatrix.channels()+0] *= scaling;
			((double*)(targetCameraMatrix.data))[1*targetCameraMatrix.step1()+1*targetCameraMatrix.channels()+0] *= scaling;
			std::cout << targetCameraMatrix << std::endl;

			//calculate directed pitch
			int n = 0;
			for(int j = 0; j < imgptnew.size(); j++)
			{
				for(int i = 0; i < (boardSize.height*boardSize.width); i++)
				{
					cv::Mat source = cv::Mat::zeros(1,1,CV_32FC2);;
					((float*)(source.data))[0*source.step1()+0*source.channels()+0] = imgptSatellit[j].at(i).x;
					((float*)(source.data))[0*source.step1()+0*source.channels()+1] = imgptSatellit[j].at(i).y;
					cv::undistortPoints(source,source,cameraMatrix[c-1],distCoeffs[c-1],R,targetCameraMatrix);
					float xOffset = (imgptnew[j].at(i).x - ((float*)(source.data))[0*source.step1()+0*source.channels()+0]);
					//get the direction of the pitch (to the left or to the right)
					xDirection += ((xOffset >= 0) - (xOffset < 0));
					if(xOffset != 0)
					{
						//offset in x direction available
						pitch += (imgptnew[j].at(i).y - ((float*)(source.data))[0*source.step1()+0*source.channels()+1]) / xOffset;
					}
					else
					{
						//special case: offset in x direction = 0 (catch division through 0)
						pitch += std::numeric_limits<float>::max();
					}
					n++;
				}
			}
			if(n > 0)
			{
				pitch /= n;
			}
			xDirection = ((xDirection >= 0) - (xDirection < 0));

			cv::initUndistortRectifyMap(cameraMatrix[0],distCoeffs[0],R0,P0,targetSize,CV_32FC1,map11,map12);
			cv::initUndistortRectifyMap(cameraMatrix[c-1],distCoeffs[c-1],R1,P1,targetSize,CV_32FC1,map21,map22);
		}

		cv::Mat i1 = cv::imread(imagePathes->at(0).at(0).toLatin1().data(), CV_LOAD_IMAGE_COLOR);
		cv::Mat i2 = cv::imread(imagePathes->at(c-1).at(0).toLatin1().data(), CV_LOAD_IMAGE_COLOR);

		if(!useGrid)
		{
			float nndr = 0.7f;

			std::vector< cv::KeyPoint > vec_keypoints1, vec_keypoints2;
			cv::Mat m_desc1, m_desc2;
			std::vector< std::vector< cv::DMatch > > matches1, matches2;
			//vector holding the best nearest neighbor per descriptor
			std::vector< cv::DMatch > good_matches;
			//vector holding points of matches
			std::vector< cv::Point2f > matchpoints_1,matchpoints_2;

			cv::SiftFeatureDetector detector; // 0.05, 5.0 
			cv::SiftDescriptorExtractor descriptor; // 3.0 

			detector.detect(i1,vec_keypoints1);
			descriptor.compute(i1,vec_keypoints1,m_desc1);

			detector.detect(i2,vec_keypoints2);
			descriptor.compute(i2,vec_keypoints2,m_desc2);

			cv::BFMatcher matcher(cv::NORM_L2,false);

			matcher.knnMatch(m_desc1,m_desc2,matches1,2);
			matcher.knnMatch(m_desc2,m_desc1,matches2,2);

			std::cout << "Matches found a->b: " << QString::number(matches1.size()).toLatin1().data() << std::endl;
			std::cout << "Matches found b->a: " << QString::number(matches2.size()).toLatin1().data() << std::endl;

			//determine the best nearest neighbor match for matches1
			for (size_t i = 0; i < matches1.size(); ++i)
			{
				if (matches1[i].size() > 1)
				{
					if(matches1[i][0].distance/matches1[i][1].distance > nndr)
					{
						matches1[i].clear();
					}
				}
				else
				{
					matches1[i].clear();
				}
			}
			//determine the best nearest neighbor match for matches2
			for (size_t i = 0; i < matches2.size(); ++i)
			{
				if (matches2[i].size() > 1)
				{
					if(matches2[i][0].distance/matches2[i][1].distance > nndr)
					{
						matches2[i].clear();
					}
				}
				else
				{
					matches2[i].clear();
				}
			}

			//perform symetric check
			for (size_t i = 0; i < matches1.size(); ++i)
			{
				if(matches1[i].size() < 2)
					continue;
				for (size_t j = 0; j < matches2.size(); ++j)
				{
					if(matches2[j].size() < 2)
						continue;
					if(matches1[i][0].queryIdx == matches2[j][0].trainIdx && matches2[j][0].queryIdx == matches1[i][0].trainIdx)
					{
						good_matches.push_back(cv::DMatch(matches1[i][0].queryIdx,matches1[i][0].trainIdx,matches1[i][0].distance));
						break;
					}
				}
			}

			//get corresponding points
			for( int i = 0; i < good_matches.size(); i++ )
			{
				//-- Get the keypoints from the good matches
				matchpoints_1.push_back( vec_keypoints1[ good_matches[i].queryIdx ].pt );
				matchpoints_2.push_back( vec_keypoints2[ good_matches[i].trainIdx ].pt );
			}

			std::cout << "Good matches: " << QString::number(good_matches.size()).toLatin1().data() <<std::endl;

			/*cv:Mat img_matches;
			cv::drawMatches(i1,vec_keypoints1,i2,vec_keypoints2,good_matches,img_matches);
			cv::imwrite("C:/Users/jtrottnow/Desktop/matches.jpg",img_matches);*/

			//comptute fundamental matrix
			F = findFundamentalMat(matchpoints_1, matchpoints_2, CV_FM_RANSAC);

			/*cv::Mat epilinesDrawing = cv::Mat::zeros(i2.size(),i2.type());
			i2.copyTo(epilinesDrawing);
			
			cv::vector<cv::Vec3f> lines;
			cv::computeCorrespondEpilines(matchpoints_1,1,F,lines);

			for (cv::vector<cv::Vec3f>::const_iterator it = lines.begin(); it!=lines.end(); ++it)
			{
				cv::line(epilinesDrawing,
						cv::Point(0,-(*it)[2]/(*it)[1]),
						cv::Point(epilinesDrawing.cols,-((*it)[2] + (*it)[0]*epilinesDrawing.cols)/(*it)[1]),
						cv::Scalar(255,255,255));
			}

			cv::imwrite("C:/Users/jtrottnow/Desktop/epiDraw.jpg",epilinesDrawing);*/

			Mat H1, H2;
			stereoRectifyUncalibrated(matchpoints_1, matchpoints_2, F, targetSize, H1, H2, 3);

			R0 = cameraMatrix[0].inv()*H1*cameraMatrix[0];
			R1 = cameraMatrix[1].inv()*H2*cameraMatrix[1];
			P0 = cameraMatrix[0];
			P1 = cameraMatrix[1];

			//DEBUG
			/*cv::Mat Rnull = cameraMatrix[0].inv()*H1*cameraMatrix[0];
			cv::Mat Rone = cameraMatrix[0].inv()*H2*cameraMatrix[0];
			cv::Mat m1,m2, img1, img2;
			cv::initUndistortRectifyMap(cameraMatrix[0],distCoeffs[0],Rnull,P0,targetSize,CV_32FC1,m1,m2);
			cv::remap(i1,img1,m1,m2,cv::INTER_CUBIC);
			cv::imwrite("C:/Users/jtrottnow/Desktop/i1.jpg",img1);
			cv::initUndistortRectifyMap(cameraMatrix[c-1],distCoeffs[c-1],Rone,P1,targetSize,CV_32FC1,m1,m2);
			cv::remap(i2,img2,m1,m2,cv::INTER_CUBIC);
			cv::imwrite("C:/Users/jtrottnow/Desktop/i2.jpg",img2);*/

			perspectiveTransform( matchpoints_2, transformed_matches, H1.inv()*H2);

			//calculate directed pitch
			float n = 0;
			float xOffsetStore = 0;
			for(int j = 0; j < transformed_matches.size(); j++)
			{
				float xOffset = transformed_matches.at(j).x - matchpoints_1.at(j).x;
				if(xOffset > xOffsetStore)
				{
					xOffsetStore = xOffset;
					xDirection = ((xOffset >= 0) - (xOffset < 0));
					pitch = (transformed_matches.at(j).y - matchpoints_1.at(j).y) / qAbs(xOffset);
				}
				//get the direction of the pitch (to the left or to the right)
				/*xDirection = ((xOffset >= 0) - (xOffset < 0));
				if(xOffset != 0)
				{
					//offset in x direction available
					pitch += (transformed_matches.at(j).y - matchpoints_1.at(j).y) / qAbs(xOffset);
				}
				else
				{
					//special case: offset in x direction = 0 (catch division through 0)
					pitch += std::numeric_limits<float>::max();
				}
				n++;*/
			}
			if(n > 0)
			{
				pitch /= n;
			}

			/*for (int i = 0; i < transformed_matches.size(); i++)
			{
				cv::circle(i1,matchpoints_1[i], 2,cv::Scalar(255,0,0));
				cv::imwrite("C:/Users/jtrottnow/Desktop/rect1_matches.jpg",i1);
			}*/

			cv::initUndistortRectifyMap(cameraMatrix[0],distCoeffs[0],R0,P0,targetSize,CV_32FC1,map11,map12);
			cv::initUndistortRectifyMap(cameraMatrix[c-1],distCoeffs[c-1],R1,P1,targetSize,CV_32FC1,map21,map22);
		}

		/*cv::Mat i2_rect;
		cv::remap(i2,i2_rect,map_one,map_two,cv::INTER_CUBIC);

		for (int i = 0; i < transformed_matches.size(); i++)
		{
			cv::circle(i2_rect,transformed_matches[i], 2,cv::Scalar(255,255,255));
		}

		cv::imwrite("C:/Users/jtrottnow/Desktop/rect2.jpg",i2_rect);*/

		cv::Mat searchPath = (Mat_<float>(1,2) << pitch, xDirection);

		//initalize outputs
		if(c == 2)
		{
			map11.copyTo(cam12.at(0));
			map12.copyTo(cam12.at(1));
			searchPath.copyTo(cam12.at(2));
			map21.copyTo(cam12.at(3));
			map22.copyTo(cam12.at(4));
		}
		else if(c == 3)
		{
			map11.copyTo(cam13.at(0));
			map12.copyTo(cam13.at(1));
			searchPath.copyTo(cam13.at(2));
			map21.copyTo(cam13.at(3));
			map22.copyTo(cam13.at(4));
		}
		else
		{
			map11.copyTo(cam14.at(0));
			map12.copyTo(cam14.at(1));
			searchPath.copyTo(cam14.at(2));
			map21.copyTo(cam14.at(3));
			map22.copyTo(cam14.at(4));
		}
	}

	setValue("Camera 1->2 package",QVariant::fromValue<std::vector<cv::Mat>>(cam12),true);
	setValue("Camera 1->3 package",QVariant::fromValue<std::vector<cv::Mat>>(cam13),true);
	setValue("Camera 1->4 package",QVariant::fromValue<std::vector<cv::Mat>>(cam14),true);
}// run

} // namespace MulticamRectify
