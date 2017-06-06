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
//! \file "DepthFuse.cpp"
//! \brief Rectification of 2 Image with given Rotation & Translation
//!
//! \author		Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!

#include "DepthFuse.h"
#include "qmessagebox.h"

namespace DepthFuse {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the DepthFuse class.
//!
//! \param name The name to give the new mesh node.
//!
DepthFuse::DepthFuse ( QString name, ParameterGroup *parameterRoot) :
	ImageNode(name, parameterRoot)
{
	m_filenameParameter = new FilenameParameter("Output Textfilename","");	
	m_filenameParameter->setType(FilenameParameter::FT_Save);
	m_filenameParameter->setFilters("Textfile (*.txt)");
	parameterRoot->addParameter(m_filenameParameter);

	setCommandFunction("Calculate!", SLOT(run()));
}

//!
//! Destructor of the DepthFuse class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
DepthFuse::~DepthFuse ()
{
}


///
/// Private Slots
///
void DepthFuse::run()
{
	float thresh = 1;
	float depthThresh = 40;

	//depth Maps
	cv::Mat dMap1, dMap2, dMap3, dMap;
	//confidence maps
	cv::Mat cMap1, cMap2, cMap3;
	//amplitude image
	cv::Mat aMap;
	//global depth field
	std::vector<std::vector<float>> globalC(0);

	//compute gradient of depth field to identify depth discontiuties & noise
	cv::Mat gradient, temp;
	temp = cv::Mat::zeros(cv::Size(dMap3.cols,dMap3.rows),CV_32FC1);
	gradient = cv::Mat::zeros(cv::Size(dMap3.cols,dMap3.rows),CV_32FC1);
	cv::Mat kernel = cv::Mat::zeros(cv::Size(3,1),CV_32FC1);
	kernel.at<float>(cv::Point(0,0)) = -1.0;
	kernel.at<float>(cv::Point(2,0)) = 1.0;
	cv::filter2D(dMap3,gradient,CV_32FC1,kernel);
	kernel = kernel.t();
	cv::filter2D(dMap3,temp,CV_32FC1,kernel);
	gradient = gradient+temp;
	gradient = cv::abs(gradient);
	cv::threshold(gradient,cMap3,depthThresh,1,CV_32FC1);
	cv::invert(cMap3,cMap3);

	//threshhold amplitude image
	cv::threshold(aMap,aMap,thresh,1,CV_32FC1);

	//combine the two confidance maps
	cMap3 = cMap3+aMap;
	cv::threshold(cMap3,cMap3,thresh,1,CV_32FC1);

	//dilate image
	cv::Mat dilateKernel = cv::Mat::ones(3,3,CV_32F);
	cv::dilate(cMap3,cMap3,dilateKernel,cv::Point(1,1));

	//get valid disparities
	for(int y = 0; y < dMap1.rows; y++)
	{
		for(int x = 0; x < dMap1.rows; x++)
		{
			int c1 = ((unsigned char*)(cMap1.data))[y*cMap1.step1()+x*cMap1.channels()];
			int c2 = ((unsigned char*)(cMap2.data))[y*cMap2.step1()+x*cMap2.channels()];
			int c3 = ((unsigned char*)(cMap3.data))[y*cMap3.step1()+x*cMap3.channels()];
			if(c1 == 255 && c2 == 255 && c3 == 1)
			{
					std::vector<float> depth(3);
					depth[0] = ((float*)(dMap1.data))[y*dMap1.step1()+x*dMap1.channels()];
					depth[1] = ((float*)(dMap2.data))[y*dMap2.step1()+x*dMap2.channels()];
					depth[2] = ((float*)(dMap3.data))[y*dMap3.step1()+x*dMap3.channels()];
					globalC.push_back(depth);
			}
		}
	}

	//compute matching to first disparity map
	cv::Mat A1 = cv::Mat::ones(cv::Size(1,globalC.size()),CV_32FC1);
	cv::Mat A2 = cv::Mat::ones(cv::Size(1,globalC.size()),CV_32FC1);
	cv::Mat B = cv::Mat::ones(cv::Size(1,globalC.size()),CV_32FC1);
	for(int i = 0; i < globalC.size(); i++)
	{
		((float*)(A1.data))[i*A1.step1()] = globalC.at(i)[0];
		((float*)(A2.data))[i*A2.step1()] = globalC.at(i)[1];
		((float*)(B.data))[i*B.step1()] = globalC.at(i)[2];
	}
	cv::Mat X1,X2;
	cv::solve(A1,A2,X1,cv::DECOMP_QR);
	cv::solve(A1,A2,X2,cv::DECOMP_QR);
	dMap1 = dMap1*X1.at<float>(0)+X1.at<float>(1);
	dMap2 = dMap2*X2.at<float>(0)+X2.at<float>(1);

	//fuse the depth maps
	dMap = cv::Mat::zeros(dMap1.size(),CV_32F);
	for(int y = 0; y < dMap.rows; y++)
	{
		for(int x = 0; x < dMap.rows; x++)
		{
			int c1 = ((unsigned char*)(cMap1.data))[y*cMap1.step1()+x*cMap1.channels()];
			int c2 = ((unsigned char*)(cMap2.data))[y*cMap2.step1()+x*cMap2.channels()];
			int c3 = ((unsigned char*)(cMap3.data))[y*cMap3.step1()+x*cMap3.channels()];
			if(c3 == 1)
			{
				((float*)(dMap.data))[y*dMap.step1()+x*dMap.channels()] = ((float*)(dMap3.data))[y*dMap3.step1()+x*dMap3.channels()];
			}
			else if(c2 == 255)
			{
				((float*)(dMap.data))[y*dMap.step1()+x*dMap.channels()] = ((float*)(dMap2.data))[y*dMap2.step1()+x*dMap2.channels()];
			}
			else if(c1 == 255)
			{
				((float*)(dMap.data))[y*dMap.step1()+x*dMap.channels()] = ((float*)(dMap2.data))[y*dMap1.step1()+x*dMap1.channels()];
			}
			else
			{
				((float*)(dMap.data))[y*dMap.step1()+x*dMap.channels()] = (((float*)(dMap2.data))[y*dMap1.step1()+x*dMap1.channels()]+((float*)(dMap2.data))[y*dMap2.step1()+x*dMap2.channels()])/2;
			}
		}
	}
	setValue("output",QVariant::fromValue<std::vector<cv::Mat>>(dMap),true);
}// run

} // namespace DepthFuse