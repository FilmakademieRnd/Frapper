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
//! \file "PatchMatchNode.cpp"
//! \brief Rectification of 2 Image with given Rotation & Translation
//!
//! \author		Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!

#include "PatchMatchNode.h"
#include "qmessagebox.h"

namespace PatchMatchNode {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the PatchMatchNode class.
//!
//! \param name The name to give the new mesh node.
//!
PatchMatchNode::PatchMatchNode ( QString name, ParameterGroup *parameterRoot) :
	ImageNode(name, parameterRoot)
{
	m_filenameParameter = new FilenameParameter("Output Textfilename","");	
	m_filenameParameter->setType(FilenameParameter::FT_Save);
	m_filenameParameter->setFilters("Textfile (*.txt)");
	parameterRoot->addParameter(m_filenameParameter);

	cam1 = new QStringList(0);
	cam2 = new QStringList(0);

	setCommandFunction("Calculate!", SLOT(run()));
}

//!
//! Destructor of the PatchMatchNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PatchMatchNode::~PatchMatchNode ()
{
}


///
/// Private Slots
///
void PatchMatchNode::run()
{
	//patchMatch general parameters
	int patchSize = 35;
	int interationsCount = 3;
	//stereo patchMatch parameters (cost calculation)
	float edgeGamma = 10.0f;
	float balanceAlpha = 0.9f;
	float tauCol = 10.0f;
	float tauGrad = 2.0f;
	//additional implementation parameters
	int maxDisparity = 70;
	int maxDifLeftRight = 2;
	bool colorHistEqualization = FALSE;
	bool grayHistEqualization = FALSE;
	cv::Size targetSize(1000,563);

	cv::Mat fundamentalMatrix = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);;

	//read parameter values from GUI
	patchSize = getValue("patchSize",true).toInt();
	interationsCount = getValue("interationsCount",true).toInt();
	edgeGamma = getValue("edgeGamma",true).toFloat();
	balanceAlpha = getValue("balanceAlpha",true).toFloat();
	tauCol = getValue("tauColor",true).toFloat();
	tauGrad = getValue("tauGradient",true).toFloat();
	maxDisparity = getValue("maxDisparity",true).toInt();
	maxDifLeftRight = getValue("maxDifferenceLeftRight",true).toInt();
	colorHistEqualization = getValue("colorHistEqualization",true).toBool();
	grayHistEqualization = getValue("grayHistEqualization",true).toBool();

	//camera package containing at position:
	//0: 	mat 1 for rectification
	//1: 	mat 2 for rectification
	//2:	pitch of epipolar lines
	std::vector<cv::Mat> camPackage = getValue("Camera package",true).value<std::vector<cv::Mat>>();

	//load file Pathes
	QString pathListString = getValue("principal camera",true).toString();
	QStringList pCam = QStringList(pathListString.split(";",QString::SkipEmptyParts));

	pathListString = getValue("satellite camera",true).toString();
	QStringList sCam = QStringList(pathListString.split(";",QString::SkipEmptyParts));

	/*if(sCam.length() == 0 || pCam.length() == 0)
	{
		Log::error("not enough cameras connected","PatchMatch");
		return;
	}*/

	//load pitch
	//pitch of epipolar line going from satellit camera to principle camera
	//xDirection is direction in which pitch goes
	float pitch = 0;
	float xDirection = 1;

	//TODO: connect Parameters to images & make for loop

	//assumption: first image is principle camera, second image is satellit image
	cv::Mat img1 = cv::imread(pCam.at(0).toLatin1().data(),CV_LOAD_IMAGE_COLOR);
	cv::Mat img2 = cv::imread(sCam.at(0).toLatin1().data(),CV_LOAD_IMAGE_COLOR);
	//cv::Mat img1 = cv::imread("C:/Users/jtrottnow/Desktop/cones/im2.png",CV_LOAD_IMAGE_COLOR);
	//cv::Mat img2 = cv::imread("C:/Users/jtrottnow/Desktop/cones/im6.png",CV_LOAD_IMAGE_COLOR);

	//resize all images to target size before further computation
	cv::Mat temp = cv::Mat::zeros(targetSize,CV_8UC3);
	double factor = (double)targetSize.height/(double)img1.rows;
	cv::resize(img1,img1,cv::Size(),factor,factor,cv::INTER_CUBIC);
	factor = (double)targetSize.height/(double)img2.rows;
	maxDisparity = maxDisparity*factor;
	cv::resize(img2,img2,cv::Size(),factor,factor,cv::INTER_CUBIC);
	cv::Rect roi( (targetSize.width-img1.cols)/2 ,0, img1.cols, img1.rows );
	cv::Mat destinationROI = temp( roi );
	img1.copyTo( destinationROI );
	temp.copyTo(img1);
	temp = cv::Mat::zeros(targetSize,CV_8UC3);
	roi = cv::Rect( (targetSize.width-img2.cols)/2 ,0, img2.cols, img2.rows );
	destinationROI = temp( roi );
	img2.copyTo( destinationROI );
	temp.copyTo(img2);

	//apply rectification maps
	if(!camPackage.empty())
	{
		cv::remap(img1,img1,camPackage[0],camPackage[1],cv::INTER_LINEAR);
		cv::remap(img2,img2,camPackage[3],camPackage[4],cv::INTER_LINEAR);
		pitch = camPackage[2].at<float>(0,0);
		xDirection = camPackage[2].at<float>(0,1);
		pitch = 0.76f;
		xDirection = -1;
	}

	//precompute grayscale gradient images
	cv::Mat img1gray, img2gray, img1gradient, img2gradient;
	img1gradient = cv::Mat::zeros(cv::Size(img1.cols,img1.rows),CV_32FC1);
	img2gradient = cv::Mat::zeros(cv::Size(img2.cols,img2.rows),CV_32FC1);
	cv::cvtColor(img1,img1gray,CV_BGR2GRAY);
	cv::cvtColor(img2,img2gray,CV_BGR2GRAY);
	cv::Mat kernel = cv::Mat::zeros(cv::Size(3,1),CV_32FC1);
	kernel.at<float>(cv::Point(0,0)) = -1.0;
	kernel.at<float>(cv::Point(2,0)) = 1.0;
	cv::filter2D(img1gray,img1gradient,CV_32FC1,kernel);
	cv::filter2D(img2gray,img2gradient,CV_32FC1,kernel);
	temp = cv::Mat::zeros(cv::Size(img1.cols,img1.rows),CV_32FC1);
	cv::filter2D(img1gray,temp,CV_32FC1,kernel.t());
	img1gradient = img1gradient+temp;
	cv::filter2D(img2gray,temp,CV_32FC1,kernel.t());
	img2gradient = img2gradient+temp;

	double min, max;
	cv::imwrite("C:/Users/jtrottnow/Desktop/teststage/img1gradient.exr",img1gradient);
	cv::imwrite("C:/Users/jtrottnow/Desktop/teststage/img2gradient.exr",img2gradient);

	//split up image into color channels for easier access
	std::vector<cv::Mat> img1BGR(3);
	std::vector<cv::Mat> img2BGR(3);
	cv::split(img1,img1BGR);
	cv::split(img2,img2BGR);

	//perform Histogram equalization for improved pattern recognition & color similarity between images
	if(colorHistEqualization)
	{
		for(int i = 0; i < 3; i++)
		{
			cv::equalizeHist( img1BGR[i], img1BGR[i] );
			cv::equalizeHist( img2BGR[i], img2BGR[i] );
		}
	}
	else if(grayHistEqualization)
	{
		cv::Mat img1YRB;
		cv::Mat img2YRB;
		cv::merge(img1BGR,img1YRB);
		cv::merge(img2BGR,img2YRB);
		cv::cvtColor(img1YRB,img1YRB,CV_BGR2YCrCb);
		cv::cvtColor(img1YRB,img2YRB,CV_BGR2YCrCb);

        std::vector<cv::Mat> img1YRBsplitted;
        cv::split(img1YRB,img1YRBsplitted);

		std::vector<cv::Mat> img2YRBsplitted;
        cv::split(img2YRB,img2YRBsplitted);

		cv::equalizeHist(img1YRBsplitted[0], img1YRBsplitted[0]);
		cv::equalizeHist(img2YRBsplitted[0], img2YRBsplitted[0]);

		cv::merge(img1YRBsplitted,img1YRB);
		cv::merge(img2YRBsplitted,img2YRB);
		cvtColor(img1YRB,img1YRB,CV_YCrCb2BGR);
		cvtColor(img2YRB,img2YRB,CV_YCrCb2BGR);

		cv::split(img1YRB,img1BGR);
		cv::split(img2YRB,img2BGR);
	}

	cv::merge(img1BGR,img1);
	cv::merge(img2BGR,img2);
	cv::imwrite("C:/Users/jtrottnow/Desktop/teststage/img1.jpg",img1);
	cv::imwrite("C:/Users/jtrottnow/Desktop/teststage/img2.jpg",img2);

	std::vector<cv::Mat> planeField(2);
	planeField[0]= cv::Mat::zeros(cv::Size(img1BGR[0].cols,img1BGR[0].rows),CV_32FC3);
	planeField[1]= cv::Mat::zeros(cv::Size(img1BGR[0].cols,img1BGR[0].rows),CV_32FC3);
	std::vector<cv::Mat> distField(2);
	distField[0] = cv::Mat::zeros(cv::Size(img1BGR[0].cols,img1BGR[0].rows),CV_32FC1);
	distField[1] = cv::Mat::zeros(cv::Size(img1BGR[0].cols,img1BGR[0].rows),CV_32FC1);
	std::vector<cv::Mat> confidenceField(2);
	confidenceField[0] = cv::Mat::zeros(cv::Size(img1BGR[0].cols,img1BGR[0].rows),CV_8UC1);
	confidenceField[1] = cv::Mat::zeros(cv::Size(img1BGR[0].cols,img1BGR[0].rows),CV_8UC1);

	//fill distanceField wit random Numbers
	cv::randu(distField[0],0,maxDisparity); 
	cv::randu(distField[1],0,maxDisparity);

	//initialize images
	//calculate ramdom planes (left & right)
	for(int i = 0; i < 2; i++)
	{
		#pragma omp parallel for
		for(int y = 0; y < img1BGR[0].rows; y++) 
		{
			for (int x = 0; x < img1BGR[0].cols; x++) 
			{
				//calculate random plane based on x and y coordinate
				//get random disparity calculated previousely
				float z = ((float*)(distField[i].data))[y*distField[i].step1()+x*distField[i].channels()+0];
				cv::Vec3f randVector;
				cv::randu(randVector,0.000000000001,1);
				//make it unit vector
				float factor = sqrt((randVector(0)*randVector(0))+(randVector(1)*randVector(1))+(randVector(2)*randVector(2)));
				randVector /= factor;
				//a
				((float*)(planeField[i].data))[y*planeField[i].step1()+x*planeField[i].channels()+0] = -randVector(0)/randVector(2);
				//b
				((float*)(planeField[i].data))[y*planeField[i].step1()+x*planeField[i].channels()+1] = -randVector(1)/randVector(2);
				//c
				((float*)(planeField[i].data))[y*planeField[i].step1()+x*planeField[i].channels()+2] = (randVector(0)*(float)x+randVector(1)*(float)y+randVector(2)*z)/randVector(2);
			}
		}
	}
	cv::minMaxLoc(distField[0], &min, &max);
	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/0_0_initialRandom_left_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[0]);
	cv::minMaxLoc(distField[1], &min, &max);
	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/0_0_initialRandom_right_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[1]);

	//run the main algorithm <<interationsCount>> times
	//each iterationstep will refine the result
	//add two more rounds where only spatial propagation is executed to smooth final result
	for(int iteration = 0; iteration < interationsCount+2; iteration++)
	{
		cv::Point firstPixel;
		cv::Point lastPixel;
		int direction;

		//switch top->bottom and bottom->top row-major walkthrough each iteration
		if(iteration%2 == 1)
		{
			direction = -1;
			firstPixel = cv::Point(img2BGR[0].cols-1,img2BGR[0].rows-1);
			lastPixel = cv::Point(-1,-1);
		}
		else
		{
			direction = 1;
			firstPixel = cv::Point(0,0);
			lastPixel = cv::Point(img2BGR[0].cols,img1BGR[0].rows);
		}
		//spatial propagation
		//left & right frame
		#pragma omp parallel for
		for(int i = 0; i < 2; i++)
		{
			//walk over entire image
			for(int y = firstPixel.y; y != lastPixel.y; y += direction) 
			{
				for (int x = firstPixel.x; x != lastPixel.x; x += direction) 
				{
					int pos = y*planeField[i].step1()+x*planeField[i].channels();
					//get current costs for matching
					float a = ((float*)(planeField[i].data))[pos];
					float b = ((float*)(planeField[i].data))[pos+1];
					float c = ((float*)(planeField[i].data))[pos+2];
					float currentCost = 0;
					if(i == 0)
						currentCost = getMatchingCost(&img1BGR, &img2BGR, &img1gradient, &img2gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection);
					else
						currentCost = getMatchingCost(&img2BGR ,&img1BGR, &img2gradient, &img1gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection);
					
					if((y > 0 && direction == 1) || (y < planeField[i].rows-1 && direction == -1))
					{
						//check if left (even iteration) or right (odd iteration) pixels's plane is better guess
						a = ((float*)(planeField[i].data))[(y-direction)*planeField[i].step1()+(x)*planeField[i].channels()+0];
						b = ((float*)(planeField[i].data))[(y-direction)*planeField[i].step1()+(x)*planeField[i].channels()+1];
						c = ((float*)(planeField[i].data))[(y-direction)*planeField[i].step1()+(x)*planeField[i].channels()+2];
						float newCost = 0;
						if(i == 0)
							newCost = getMatchingCost(&img1BGR, &img2BGR, &img1gradient, &img2gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );
						else
							newCost = getMatchingCost(&img2BGR ,&img1BGR, &img2gradient, &img1gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );
						if(newCost < currentCost)
						{
							currentCost = newCost;
							((float*)(planeField[i].data))[pos] = a;
							((float*)(planeField[i].data))[pos+1] = b;
							((float*)(planeField[i].data))[pos+2] = c;
							((float*)(distField[i].data))[y*distField[i].step1()+x*distField[i].channels()] = a*(float)x+b*(float)y+c;
						}
					}
					if((x > 0 && direction == 1) || (y < planeField[i].cols-1 && direction == -1))
					{
						//check if top (even iteration) or bottom (odd iteration) pixels's plane is better guess
						a = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x-direction)*planeField[i].channels()+0];
						b = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x-direction)*planeField[i].channels()+1];
						c = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x-direction)*planeField[i].channels()+2];
						float newCost = 0;
						if(i == 0)
							newCost = getMatchingCost(&img1BGR, &img2BGR, &img1gradient, &img2gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );
						else
							newCost = getMatchingCost(&img2BGR ,&img1BGR, &img2gradient, &img1gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );
						if(newCost < currentCost)
						{
							((float*)(planeField[i].data))[pos] = a;
							((float*)(planeField[i].data))[pos+1] = b;
							((float*)(planeField[i].data))[pos+2] = c;
							((float*)(distField[i].data))[y*distField[i].step1()+x*distField[i].channels()] = a*(float)x+b*(float)y+c;
						}
					}
				}
			}
		}
		
		cv::minMaxLoc(distField[0], &min, &max);
		cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/"+QString::number(iteration)+"_1_spatialProp_left_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[0]);
		cv::minMaxLoc(distField[1], &min, &max);
		cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/"+QString::number(iteration)+"_1_spatialProp_right_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[1]);
		
		//suppress further execution in last two rounds
		if(iteration < interationsCount)
		{
			//view propagation
			//right & left frame
			for(int i = 1; i > -1; i--)
			{
				//walk over entire image
				#pragma omp parallel for
				for(int y = 0; y <= img2BGR[0].rows-1; y++) 
				{
					int otherImage = !(bool)i;
					for (int x = 0; x <= img2BGR[0].cols-1; x++) 
					{
						//for each pixel in the image
						//get corresponding pixel's x value
						float disparity = ((float*)(distField[i].data))[y*distField[i].step1()+x*distField[i].channels()];
						//calculate with floating point precision but round to int to get real pixel
						//delta offsets based on disparity & pitch
						float deltaX = ((((float)direction*2.0f)-1.0f)*(xDirection*sqrt((disparity*disparity)/(1+(pitch*pitch)))));
						//added to the current position
						int currentX = qRound((float)x-deltaX);
						int currentY = qRound((float)y-(xDirection*deltaX*pitch));

						//check if within image dimensions
						if(currentX >= 0 && currentX < img2BGR[0].cols && currentY >= 0 && currentY < img2BGR[0].rows)
						{
							int pos = currentY*planeField[otherImage].step1()+currentX*planeField[otherImage].channels();
							//get current plane & cost
							float a2 = ((float*)(planeField[otherImage].data))[pos];
							float b2 = ((float*)(planeField[otherImage].data))[pos+1];
							float c2 = ((float*)(planeField[otherImage].data))[pos+2];
							float currentCost = 0;
							if(i == 1)
								currentCost =  getMatchingCost(&img1BGR, &img2BGR, &img1gradient, &img2gradient,currentX, currentY, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a2, b2, c2, maxDisparity, !(bool)i, pitch, xDirection );
							else
								currentCost =  getMatchingCost(&img2BGR, &img1BGR, &img2gradient, &img1gradient,currentX, currentY, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a2, b2, c2, maxDisparity, !(bool)i, pitch, xDirection );

							//get plane of the corresponding pixel (correspondance is defined by 2nd image)
							float a = ((float*)(planeField[i].data))[y*planeField[i].step1()+x*planeField[i].channels()];
							float b = ((float*)(planeField[i].data))[y*planeField[i].step1()+x*planeField[i].channels()+1];
							float c = ((float*)(planeField[i].data))[y*planeField[i].step1()+x*planeField[i].channels()+2];

							//transform plane to the first image
							//adopt the normal vector
							cv::Vec3f normalVector(a,b,-1);
							float factor = sqrt((a*a)+(b*b)+1);
							normalVector /= factor;

							a = -normalVector(0)/normalVector(2);
							b = -normalVector(1)/normalVector(2);
							c = (normalVector(0)*(float)currentX+normalVector(1)*(float)currentY+normalVector(2)*disparity)/normalVector(2);

							float corresCost = 0;
							if(i == 0)
								corresCost =  getMatchingCost(&img1BGR, &img2BGR, &img1gradient, &img2gradient,currentX, currentY, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, !(bool)i, pitch, xDirection );
							else
								corresCost =  getMatchingCost(&img2BGR, &img1BGR, &img2gradient, &img1gradient,currentX, currentY, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, !(bool)i, pitch, xDirection );

							//check if the corrseponding pixel in other image has higher costs
							if(corresCost < currentCost)
							{
								//if so, replace this pixels plane & disparity (in the other image) with the obtained one
								((float*)(planeField[otherImage].data))[pos] = a;
								((float*)(planeField[otherImage].data))[pos+1] = b;
								((float*)(planeField[otherImage].data))[pos+2] = c;
								((float*)(distField[otherImage].data))[currentY*distField[otherImage].step1()+currentX*distField[otherImage].channels()] = a*currentX+b*currentY+c;
							}
						}
					}
				}
			}
			cv::minMaxLoc(distField[0], &min, &max);
			cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/"+QString::number(iteration)+"_2_viewProp_left_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[0]);
			cv::minMaxLoc(distField[1], &min, &max);
			cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/"+QString::number(iteration)+"_2_viewProp_right_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[1]);

			//TODO: (Maybe): temporal propagation
			{}

			//plane refinement
			//right & left frame
			for(int i = 0; i < 2; i++)
			{
				//walk over entire image
				#pragma omp parallel for
				for(int y = 0; y < img2BGR[0].rows; y++) 
				{
					for (int x = 0; x < img2BGR[0].cols; x++) 
					{
						float maxDeltaZ = maxDisparity/2;
						float maxDeltaN = 1;

						while(maxDeltaZ >= 0.1f)
						{
							int pos = y*planeField[i].step1()+x*planeField[i].channels();
							//calculate random delta offsets for disparity and plane
							cv::Vec2f random;
							cv::randu(random,-maxDeltaZ,maxDeltaZ);
							float deltaZ = random(0);
							//float deltaZ = qrand() % ((qRound(maxDeltaZ*1000000.0f) + 1) - (-qRound(maxDeltaZ*1000000.0f))) + (-qRound(maxDeltaZ*1000000.0f));
							//deltaZ /= 1000000.0f;
							cv::Vec3f deltaN(0,0,0);
							cv::randu(deltaN,-maxDeltaN,maxDeltaN);
							//get original data
							float z = ((float*)(distField[i].data))[y*distField[i].step1()+x*distField[i].channels()];
							float a = ((float*)(planeField[i].data))[pos+0];
							float b = ((float*)(planeField[i].data))[pos+1];
							float c = ((float*)(planeField[i].data))[pos+2];
							float currentCost = 0;
							if(i == 0)
								currentCost = getMatchingCost(&img1BGR, &img2BGR, &img1gradient, &img2gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );
							else
								currentCost = getMatchingCost(&img2BGR ,&img1BGR, &img2gradient, &img1gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );
							//convert to Normal & point representation
							cv::Vec3f normalVector(a,b,-1.0f);
							//make normal vector unit vector
							float factor = sqrt((a*a)+(b*b)+1.0f);
							normalVector /= factor;
							//generate new plane based on random deltas
							z = z+deltaZ;
							normalVector = normalVector+deltaN;
							factor = sqrt((normalVector(0)*normalVector(0))+(normalVector(1)*normalVector(1))+(normalVector(2)*normalVector(2)));
							normalVector /= factor;
							//convert back to d=ax+by+c plane representation
							//a
							a = -normalVector(0)/normalVector(2);
							b = -normalVector(1)/normalVector(2);
							c = (normalVector(0)*(float)x+normalVector(1)*(float)y+normalVector(2)*z)/normalVector(2);
							float newCost = 0;
							if(i == 0)
								newCost = getMatchingCost(&img1BGR, &img2BGR, &img1gradient, &img2gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );
							else
								newCost = getMatchingCost(&img2BGR ,&img1BGR, &img2gradient, &img1gradient, x, y, patchSize, edgeGamma, balanceAlpha, tauCol, tauGrad, a, b, c, maxDisparity, i, pitch, xDirection );

							if(newCost < currentCost)
							{
								((float*)(planeField[i].data))[pos] = a;
								((float*)(planeField[i].data))[pos+1] = b;
								((float*)(planeField[i].data))[pos+2] = c;
								((float*)(distField[i].data))[y*distField[i].step1()+x*distField[i].channels()] = z;
							}
							maxDeltaN /= 2.0f;
							maxDeltaZ /= 2.0f;
						}
					}
				}
			}

			cv::minMaxLoc(distField[0], &min, &max);
			cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/"+QString::number(iteration)+"_3_planeRefinement_left_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[0]);
			cv::minMaxLoc(distField[1], &min, &max);
			cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/"+QString::number(iteration)+"_3_planeRefinement_right_"+QString::number(min)+"_"+QString::number(max)+".exr").toLatin1().data(),distField[1]);
		}
	}

	//post-processing
		//consistency check
	//right & left frame
	for(int i = 0; i < 2; i++)
	{
		//walk over entire image
		#pragma omp parallel for
		for(int y = 0; y < img2BGR[0].rows; y++) 
		{
			for (int x = 0; x < img2BGR[0].cols; x++) 
			{
				float disparity12 = ((float*)(distField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()+0];
				int otherImage = !(bool)i;
				int x2 = qRound((float)x+(((float)i*2.0f)-1.0f)*disparity12);
				if(x2 > 0 && x2 < img2BGR[0].cols)
				{
					float disparity21 = ((float*)(distField[otherImage].data))[(y)*distField[otherImage].step1()+x2*distField[otherImage].channels()+0];
					if(qAbs(disparity12-disparity21) <= maxDifLeftRight)
					{
						((unsigned char*)(confidenceField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()+0] = 255;
					}
				}
			}
		}
	}

	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/confidenceField_left.jpg").toLatin1().data(),confidenceField[0]);
	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/confidenceField_right.jpg").toLatin1().data(),confidenceField[1]);

	//fill up inconsistencies
	for(int i = 0; i < 2; i++)
	{
		//walk over entire image
		#pragma omp parallel for
		for(int y = 0; y < img2BGR[0].rows; y++) 
		{
			bool foundLeft, foundRight;
			int left, right;
			float aLeft, bLeft, cLeft, aRight, bRight, cRight, disparity1, disparity2;
			for (int x = 0; x < img2BGR[0].cols; x++) 
			{
				if(((unsigned char*)(confidenceField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()+0] == 0)
				{
					foundLeft = false;
					foundRight = false;
					left = x-1;
					right = x+1;
					//search for closest valid pixel to the right and left
					while(!foundLeft && left >= 0)
					{
						if(((unsigned char*)(confidenceField[i].data))[(y)*distField[i].step1()+(left)*distField[i].channels()+0] == 255)
						{
							foundLeft = true;
							aLeft = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(left)*planeField[i].channels()+0];
							bLeft = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(left)*planeField[i].channels()+1];
							cLeft = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(left)*planeField[i].channels()+2];
							disparity1 = aLeft*x+bLeft*y+cLeft;
						}
						left--;
					}
					while(!foundRight && right < img2BGR[0].cols)
					{
						if(((unsigned char*)(confidenceField[i].data))[(y)*distField[i].step1()+(right)*distField[i].channels()+0] == 255)
						{
							foundRight = true;
							aRight = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(right)*planeField[i].channels()+0];
							bRight = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(right)*planeField[i].channels()+1];
							cRight = ((float*)(planeField[i].data))[(y)*planeField[i].step1()+(right)*planeField[i].channels()+2];
							disparity2 = aRight*x+bRight*y+cRight;
						}
						right++;
					}
					//get planes of closest pixels
					if(foundLeft && foundRight)
					{
						if(disparity1 > disparity2)
						{
							//take right plane as approximation
							((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+0] = aRight;
							((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+1] = bRight;
							((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+2] = cRight;
							((float*)(distField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()+0] = disparity2;
						}
						else
						{
							//take left plane as approximation
							((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+0] = aLeft;
							((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+1] = bLeft;
							((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+2] = cLeft;
							((float*)(distField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()+0] = disparity1;
						}
					}
					else if(foundRight)
					{
						//take right plane as approximation
						((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+0] = aRight;
						((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+1] = bRight;
						((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+2] = cRight;
						((float*)(distField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()+0] = disparity2;
					}
					else if(foundLeft)
					{
						//take left plane as approximation
						((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+0] = aLeft;
						((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+1] = bLeft;
						((float*)(planeField[i].data))[(y)*planeField[i].step1()+(x)*planeField[i].channels()+2] = cLeft;
						((float*)(distField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()+0] = disparity1;
					}
				}
			}
		}
	}

	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/final_without_median_left.exr").toLatin1().data(),distField[0]);
	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/final_without_median_right.exr").toLatin1().data(),distField[1]);

	//offset to get from the upper left pixel of a patch to the middle pixel (center of patch)
	int offset = qFloor(patchSize/2);

	//apply weighted median filter to the approximated disparities to eliminate horizontal streaks
	cv::Mat tempDist = cv::Mat::zeros(distField[0].size(),distField[0].type());
	for(int i = 0; i < 2; i++)
	{
		//walk over entire image
		#pragma omp parallel for
		for(int y = 0; y < img2BGR[0].rows; y++) 
		{
			for (int x = 0; x < img2BGR[0].cols; x++) 
			{
				if(((unsigned char*)(confidenceField[i].data))[(y)*distField[i].step1()+(x)*distField[i].channels()] == 0)
				{
					QVector<float> window;
					float singleWeight;
					int qx, qy;
					for(int dx = -offset; dx <= offset; dx++) //x-direction
					{
						for (int dy = -offset; dy <= offset; dy++) //y-direction
						{
							qx = x+dx;
							qy = y+dy;
							if(qx > 0 && qy > 0 && qx < distField[i].cols && qy < distField[i].rows)
							{
								//if(((unsigned char*)(confidenceField[i].data))[(qy)*distField[i].step1()+(qx)*distField[i].channels()] == 1)
								//{
									singleWeight = exp(-(normL1(&img1BGR, &img2BGR,qx, qy, x, y)/(edgeGamma*10.0f)))*10.0f;
									float disparity = ((float*)(distField[i].data))[qy*distField[i].step1()+qx*distField[i].channels()];
									for(int i = 0; i < qRound(singleWeight) ; i++)
									{
										window.push_back(disparity);
									}
								//}
							}
						}
					}
					if(window.size() > 0)
					{
						qSort(window);
						((float*)(tempDist.data))[y*distField[i].step1()+x*distField[i].channels()] = window.at(qRound(window.size()/2));
					}
				}
				else
				{
					((float*)(tempDist.data))[y*distField[i].step1()+x*distField[i].channels()] = ((float*)(distField[i].data))[y*distField[i].step1()+x*distField[i].channels()];
				}
			}
		}
		tempDist.copyTo(distField[i]);
	}

	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/final_with_median_left.exr").toLatin1().data(),distField[0]);
	cv::imwrite(QString("C:/Users/jtrottnow/Desktop/teststage/final_with_median_right.exr").toLatin1().data(),distField[1]);

	std::vector<cv::Mat> outputPack(2);
	outputPack.at(0) = distField[0];
	outputPack.at(1) = confidenceField[0];

	setValue("depthmap + confidence field",QVariant::fromValue<std::vector<cv::Mat>>(outputPack),true);
}// run


float PatchMatchNode::getMatchingCost(std::vector<cv::Mat> *img1, std::vector<cv::Mat> *img2, cv::Mat *img1gradient, cv::Mat *img2gradient,int x1, int y1, int patchSize, float edgeGamma, float balanceAlpha, float tauCol, float tauGrad, float a, float b, float c, int maxDisparity, bool direction, float pitch, float xDirection )
{
	float distance = 0;
	float weight, colorNormL1, dispNormL1, grayDiff, p, disparity, q2x, q2y;
	int qx, qy, counter;
	counter = 0;
	int offset = qFloor(patchSize/2);
	//walk over entire patch
	for(int dx = -offset; dx <= offset; dx++) //x-direction
	{
		for (int dy = -offset; dy <= offset; dy++) //y-direction
		{
			qx = x1+dx;
			qy = y1+dy;
			//check if pixel is within image dimensions
			if(qx >= 0 && qx < img1->at(0).cols && qy >= 0 && qy < img1->at(0).rows)
			{
				//corresponding pixel in other image
				disparity = a*(float)qx+b*(float)qy+c;
				//catch invalid disparities
				if (disparity < 0 || disparity > maxDisparity) return std::numeric_limits<float>::max();
				//calculate corresponding pixels position based on the pitch of the epipolar lines
				//delta offsets based on disparity & pitch
				q2x = ((((float)direction*2.0f)-1.0f)*(xDirection*sqrt((disparity*disparity)/(1+(pitch*pitch)))));
				q2y = xDirection*pitch*q2x;
				//added to the current position
				q2x += (float)qx;
				q2y += (float)qy;
				//q2 = (float)qx+(((float)direction*2)-1)*disparity;
				//check if matching pixel is within image dimensions
				if(q2x >= 0 && q2x < img2->at(0).cols-1 && q2y >= 0 && q2y < img2->at(0).rows-1)
				{
					//weight applied to overcome edge-flattening problems (adaptive support weight) -> computes likelihood that 2 pixels lie on the same plane
					//color similar -> high value -> pixels lie on same plane
					colorNormL1 = normL1(img1, img1, x1, y1, qx, qy);
					weight = exp(-(colorNormL1/edgeGamma));
					//dissimilarity p between pixel of left and right image
					//alpha is balancing factor between color and gradient term
					dispNormL1 = normL1subPixel(img1, img2,qx, qy, q2x, q2y);
					//absulute difference of gray-value gradients at q and q2
					float factorX = (q2x-qFloor(q2x))*0.5f;
					float factorY = (q2y-qFloor(q2y))*0.5f;
					grayDiff = qAbs( ((float*)(img1gradient->data))[qy*img1gradient->step1()+qx*img1gradient->channels()]
										-(	 ((float*)(img2gradient->data))[qFloor(q2y)*img2gradient->step1()+qFloor(q2x)*img2gradient->channels()]*(1-factorX)*(1-factorY)
											+((float*)(img2gradient->data))[qFloor(q2y)*img2gradient->step1()+qCeil(q2x)*img2gradient->channels()]*(factorX)*(1-factorY)
											+((float*)(img2gradient->data))[qCeil(q2y)*img2gradient->step1()+qFloor(q2x)*img2gradient->channels()]*(1-factorX)*(factorY)
											+((float*)(img2gradient->data))[qCeil(q2y)*img2gradient->step1()+qCeil(q2x)*img2gradient->channels()]*(factorX)*(factorY)
										 )
									);
					p = (1-balanceAlpha)*qMin(dispNormL1,tauCol)+balanceAlpha*qMin(grayDiff,tauGrad);
					distance += weight*p;
					counter++;
				}
			}
		}
	}
	//catch case when no pixels have been compared
	if(counter == 0) return std::numeric_limits<float>::max();
	distance /= counter;
	return distance;
}

float PatchMatchNode::normL1subPixel(std::vector<cv::Mat> *img1, std::vector<cv::Mat> *img2, int x1, int y1, float x2, float y2)
{
	int steps = img1->at(0).step1();
	int channels = img1->at(0).channels();
	int x2Floor = qFloor(x2)*channels;
	int x2Ceil = qCeil(x2)*channels;
	int y2Rounded = qRound(y2)*steps;
	float factor = x2-qFloor(x2);
	return	  qAbs((float)((unsigned char*)(img1->at(0).data))[y1*steps+x1]-(((unsigned char*)(img2->at(0).data))[y2Rounded+x2Floor]*(1-factor)+((unsigned char*)(img2->at(0).data))[y2Rounded+x2Ceil]*factor))
			+ qAbs((float)((unsigned char*)(img1->at(1).data))[y1*steps+x1]-(((unsigned char*)(img2->at(1).data))[y2Rounded+x2Floor]*(1-factor)+((unsigned char*)(img2->at(1).data))[y2Rounded+x2Ceil]*factor))
			+ qAbs((float)((unsigned char*)(img1->at(2).data))[y1*steps+x1]-(((unsigned char*)(img2->at(2).data))[y2Rounded+x2Floor]*(1-factor)+((unsigned char*)(img2->at(2).data))[y2Rounded+x2Ceil]*factor));
}

float PatchMatchNode::normL1(std::vector<cv::Mat> *img1, std::vector<cv::Mat> *img2, int x1, int y1, int x2, int y2)
{
	float factor = x2-qFloor(x2);
	int steps = img1->at(0).step1();
	int channels = img1->at(0).channels();
	int fistCoordinate = y1*steps+x1*channels;
	int secondCoordinate = y2*steps+x2*channels;
	return	  qAbs((float)((unsigned char*)(img1->at(0).data))[fistCoordinate]-((unsigned char*)(img2->at(0).data))[secondCoordinate])
			+ qAbs((float)((unsigned char*)(img1->at(1).data))[fistCoordinate]-((unsigned char*)(img2->at(1).data))[secondCoordinate])
			+ qAbs((float)((unsigned char*)(img1->at(2).data))[fistCoordinate]-((unsigned char*)(img2->at(2).data))[secondCoordinate]);
}

} // namespace PatchMatchNode