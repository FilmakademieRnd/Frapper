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
//! \file "pointcloud2DepthCue.cpp"
//! \brief Camera Input
//!
//! \author		Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!

#include "pointcloud2DepthCue.h"
#include "qmessagebox.h"

namespace pointcloud2DepthCue {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the pointcloud2DepthCue class.
//!
//! \param name The name to give the new mesh node.
//!
pointcloud2DepthCue::pointcloud2DepthCue ( QString name, ParameterGroup *parameterRoot) :
	ImageNode(name, parameterRoot)
{

	setCommandFunction("Calculate!", SLOT(run()));
}

//!
//! Destructor of the pointcloud2DepthCue class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
pointcloud2DepthCue::~pointcloud2DepthCue ()
{
}

///
/// Private Slots
///


void pointcloud2DepthCue::run()
{
	bool featureBasedSelection = getValue("featureBasedSelection",false).toBool();
	int imageWidth = getValue("imageWidth",false).toInt();
	int imageHeight = getValue("imageHeight",false).toInt();
	QString outputPath = getValue("Output Path",false).toString();
	QString basePath = getValue("SfM_Output Folder Location",false).toString()+"/SfM_Output/";

	if(outputPath.isEmpty() ||  basePath.isEmpty())
	{
		return;
	}
	
	// get camera Files
	QDir cameraFolder(basePath+"cameras");
	cameraFolder.setNameFilters(QStringList()<<"*.bin");
	QStringList cameraPathes = cameraFolder.entryList(QDir::Files);
	//read camera projection matrices from .bin files
	for(int i = 0; i < cameraPathes.size(); i++)
	{
		cv::Mat projMatrix = cv::Mat::zeros(cv::Size(4,4),CV_64FC1);
		std::vector<double> val;
		val.resize(12);
		std::string path = QString(cameraFolder.absolutePath()+"/"+cameraPathes.at(i)).toLatin1().data();
		std::ifstream in(path.c_str(),std::ios::in|std::ios::binary);
		if (in.is_open())	{
			in.read((char*)&val[0],(std::streamsize)12*sizeof(double));
			projMatrix.at<double>(cv::Point(0,0)) = val.at(0);
			projMatrix.at<double>(cv::Point(0,1)) = val.at(1);
			projMatrix.at<double>(cv::Point(0,2)) = val.at(2);
			projMatrix.at<double>(cv::Point(1,0)) = val.at(3);
			projMatrix.at<double>(cv::Point(1,1)) = val.at(4);
			projMatrix.at<double>(cv::Point(1,2)) = val.at(5);
			projMatrix.at<double>(cv::Point(2,0)) = val.at(6);
			projMatrix.at<double>(cv::Point(2,1)) = val.at(7);
			projMatrix.at<double>(cv::Point(2,2)) = val.at(8);
			projMatrix.at<double>(cv::Point(3,0)) = val.at(9);
			projMatrix.at<double>(cv::Point(3,1)) = val.at(10);
			projMatrix.at<double>(cv::Point(3,2)) = val.at(11);
			projMatrix.at<double>(cv::Point(3,3)) = 1;
		}
		cameras.push_back(projMatrix);
	}
	//get Point Cloud File
	QString line;
	QStringList splitedLine;
	QFile pointCloudFile(basePath+"clouds/calib.ply");
	if (pointCloudFile.open(QIODevice::ReadOnly)) {
		QTextStream in(&pointCloudFile);
		int counter = 0;
		while( !in.atEnd() )
		{
			counter++;
			//skip first 15 lines (meta data)
			if(counter < 16)
			{
				in.readLine();
				continue;
			}
			line = in.readLine();
			splitedLine = line.split(" ",QString::SkipEmptyParts);
			cv::Mat point = cv::Mat::zeros(cv::Size(1,3),CV_32F);
			point.at<float>(cv::Point(0,0)) = splitedLine[0].toFloat();
			point.at<float>(cv::Point(0,1)) = splitedLine[1].toFloat();
			point.at<float>(cv::Point(0,2)) = splitedLine[2].toFloat();
			QVector<int> visibility;
			for(int k = 7; k < splitedLine.size(); k++)
			{
				visibility.push_back(splitedLine[k].toInt());
			}
			pointCloudVisibilities.push_back(visibility);
			pointCloudInitial.push_back(point);
		}
	}
	//convert PointCloud Vector to openCV Matrix
	pointCloud = cv::Mat::ones(cv::Size(4,pointCloudInitial.size()),CV_64FC1);
	for(int k = 0; k < pointCloudInitial.size(); k++)
	{
		pointCloud.at<double>(cv::Point(0,k)) = pointCloudInitial.at(k).at<float>(cv::Point(0,0));
		pointCloud.at<double>(cv::Point(1,k)) = pointCloudInitial.at(k).at<float>(cv::Point(0,1));
		pointCloud.at<double>(cv::Point(2,k)) = pointCloudInitial.at(k).at<float>(cv::Point(0,2));
	}
	pointCloudInitial.resize(0);
	//projection
	for(int i = 0; i < cameras.size() ;i++)
	{
		cv::Mat projectedPC;
		cv::transpose(pointCloud,projectedPC);
		//project by Matrixmultiplication
		cv::gemm(cameras.at(i),projectedPC,1,NULL,NULL,projectedPC);
		cv::transpose(projectedPC,projectedPC);
		//initialzing values for depth calculation
		double cameraDeterminante = cv::determinant(cameras.at(i)(cv::Range(0, 0), cv::Range(2, 2)));
		int cameraSign;
		cameraDeterminante < 0 ? cameraSign = -1 : cameraSign = 1;
		double camTNorm = pow(cameras.at(i).at<double>(2,0),2)+pow(cameras.at(i).at<double>(2,1),2)+pow(cameras.at(i).at<double>(2,2),2);
		camTNorm = cv::pow(camTNorm,0.5);
		//calculate depth
		cv::Mat depths = cv::Mat::zeros(cv::Size(1,projectedPC.rows),CV_64FC1);
		if(camTNorm != 0)
		{
			depths = (cameraSign * projectedPC.col(2)) / camTNorm;
		}

		//depth value optimization
		double depthMean = 0;
		double counter = 0;
		cv::Mat sortedDepth;
		float confidence = 0.99f;
		cv::sort(depths, sortedDepth, CV_SORT_EVERY_COLUMN);
		int maxDepthPos = static_cast<int>((float)sortedDepth.rows*(confidence));
		double maxDepth = sortedDepth.at<double>(cv::Point(0,maxDepthPos));

		for(int p = 0; p < depths.rows; p++)
		{
			double depthValue = depths.at<double>(cv::Point(0,p));
			if(depthValue > 0 && depthValue < maxDepth)
			{
				counter++;
				depthMean += depthValue;
			}
		}
		depthMean = depthMean/counter;

		//write depth to image
		cv::Mat depthCue = cv::Mat::zeros(cv::Size(imageWidth,imageHeight),CV_8UC1);
		for(int k = 0; k < projectedPC.rows; k++)
		{
			//get position
			cv::Mat point = projectedPC.row(k);
			point /= projectedPC.at<double>(cv::Point(3,k));
			cv::Point pos(0,0);
			pos = cv::Point(static_cast<int>(qRound(projectedPC.at<double>(cv::Point(0,k))/projectedPC.at<double>(cv::Point(2,k)))),
							static_cast<int>(qRound(projectedPC.at<double>(cv::Point(1,k)))/projectedPC.at<double>(cv::Point(2,k))));
			//if inside image
			if((pos.x > 0) && (pos.x < depthCue.cols) && (pos.y > 0) && (pos.y < depthCue.rows))
			{
				//get Depth
				double depth = (100.0/depthMean)*depths.at<double>(cv::Point(0,k));
				if (depth > 255) depth = 255;
				if(depth > 0)
				{
					//write Pixel to image
					if(featureBasedSelection)
					{
						//only in this camera obtained Features are used
						if(pointCloudVisibilities.at(k).contains(i+1))
							depthCue.at<uchar>(pos) = static_cast<uchar>(depth);
					}
					else
					{
						depthCue.at<uchar>(pos) = static_cast<uchar>(depth);
					}
				}
			}
		}
		std::string fileName = QString(outputPath+"/"+QFileInfo(cameraPathes.at(i)).baseName()+".png").toLatin1().data();
		cv::imwrite(fileName,depthCue);
	}

}// run

/*
void pointcloud2DepthCue::decomposeP(const cv::Mat &P, cv::Mat *Kp, cv::Mat *Rp, cv::Mat *tp) {
  // Decompose using the RQ decomposition HZ A4.1.1 pag.579.
  cv::Mat K = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	K.at<double>(cv::Point(0,0)) = P.at<double>(cv::Point(0,0));
	K.at<double>(cv::Point(0,1)) = P.at<double>(cv::Point(0,1));
	K.at<double>(cv::Point(0,2)) = P.at<double>(cv::Point(0,2));
	K.at<double>(cv::Point(1,0)) = P.at<double>(cv::Point(1,0));
	K.at<double>(cv::Point(1,1)) = P.at<double>(cv::Point(1,1));
	K.at<double>(cv::Point(1,2)) = P.at<double>(cv::Point(1,2));
	K.at<double>(cv::Point(2,0)) = P.at<double>(cv::Point(2,0));
	K.at<double>(cv::Point(2,1)) = P.at<double>(cv::Point(2,1));
	K.at<double>(cv::Point(2,2)) = P.at<double>(cv::Point(2,2));

  cv::Mat Q = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
  Q.at<double>(cv::Point(0,0)) = 1.0;
  Q.at<double>(cv::Point(1,1)) = 1.0;
  Q.at<double>(cv::Point(2,2)) = 1.0;

  // Set K(2,1) to zero.
  if (K.at<double>(cv::Point(1,2)) != 0) {
    double c = -K.at<double>(cv::Point(2,2));
    double s = K.at<double>(cv::Point(1,2));
    double l = sqrt(c * c + s * s);
    c /= l; s /= l;
    cv::Mat Qx = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	Qx.at<double>(cv::Point(0,0)) = 1.0;
	Qx.at<double>(cv::Point(1,1)) = c;
	Qx.at<double>(cv::Point(2,1)) = -s;
	Qx.at<double>(cv::Point(1,2)) = s;
	Qx.at<double>(cv::Point(2,2)) = c;
	cv::gemm(K,Qx,1,NULL,NULL,K);
	cv::Mat QxTransposed = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	cv::transpose(Qx,QxTransposed);
	cv::gemm(QxTransposed,Q,1,NULL,NULL,Q);
  }
  // Set K(2,0) to zero.
  if (K.at<double>(cv::Point(0,2)) != 0) {
    double c = K.at<double>(cv::Point(2,2));
    double s = K.at<double>(cv::Point(0,2));
    double l = sqrt(c * c + s * s);
    c /= l; s /= l;
	cv::Mat Qy = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	Qy.at<double>(cv::Point(0,0)) = c;
	Qy.at<double>(cv::Point(2,0)) = s;
	Qy.at<double>(cv::Point(1,1)) = 1;
	Qy.at<double>(cv::Point(0,2)) = -s;
	Qy.at<double>(cv::Point(2,2)) = c;
    cv::gemm(K,Qy,1,NULL,NULL,K);
	cv::Mat QyTransposed = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	cv::transpose(Qy,QyTransposed);
	cv::gemm(QyTransposed,Q,1,NULL,NULL,Q);
  }
  // Set K(1,0) to zero.
  if (K.at<double>(cv::Point(0,1)) != 0) {
    double c = -K.at<double>(cv::Point(1,1));
    double s = K.at<double>(cv::Point(0,1));
    double l = sqrt(c * c + s * s);
    c /= l; s /= l;
	cv::Mat Qz = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	Qz.at<double>(cv::Point(0,0)) = c;
	Qz.at<double>(cv::Point(1,0)) = -s;
	Qz.at<double>(cv::Point(0,1)) = s;
	Qz.at<double>(cv::Point(1,1)) = c;
	Qz.at<double>(cv::Point(2,2)) = 1;
	cv::gemm(K,Qz,1,NULL,NULL,K);
	cv::Mat QzTransposed = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	cv::transpose(Qz,QzTransposed);
	cv::gemm(QzTransposed,Q,1,NULL,NULL,Q);
  }

  cv::Mat R = Q;

  // Ensure that the diagonal is positive and R determinant == 1.
  if (K.at<double>(cv::Point(2,2)) < 0) {
    K = -K;
    R = -R;
  }
  if (K.at<double>(cv::Point(1,1)) < 0) {
	cv::Mat S = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	S.at<double>(cv::Point(0,0)) = 1;
	S.at<double>(cv::Point(1,1)) = -1;
	S.at<double>(cv::Point(2,2)) = 1;
	cv::gemm(K,S,1,NULL,NULL,K);
	cv::gemm(S,R,1,NULL,NULL,R);
  }
  if (K.at<double>(cv::Point(0,0)) < 0) {
	cv::Mat S = cv::Mat::zeros(cv::Size(3,3),CV_64FC1);
	S.at<double>(cv::Point(0,0)) = -1;
	S.at<double>(cv::Point(1,1)) = 1;
	S.at<double>(cv::Point(2,2)) = 1;
	cv::gemm(K,S,1,NULL,NULL,K);
	cv::gemm(S,R,1,NULL,NULL,R);
  }

  // Compute translation.
  Eigen::Matrix<double,3,3> KEigen;
  KEigen(0,0) = K.at<double>(cv::Point(0,0));
  KEigen(1,0) = K.at<double>(cv::Point(0,1));
  KEigen(2,0) = K.at<double>(cv::Point(0,2));
  KEigen(0,1) = K.at<double>(cv::Point(1,0));
  KEigen(1,1) = K.at<double>(cv::Point(1,1));
  KEigen(2,1) = K.at<double>(cv::Point(1,2));
  KEigen(0,2) = K.at<double>(cv::Point(2,0));
  KEigen(1,2) = K.at<double>(cv::Point(2,1));
  KEigen(2,2) = K.at<double>(cv::Point(2,2));

  Vec3 PTranslationEigen;
  PTranslationEigen(0) = P.at<double>(cv::Point(3,0));
  PTranslationEigen(1) = P.at<double>(cv::Point(3,1));
  PTranslationEigen(2) = P.at<double>(cv::Point(3,2));

  Eigen::Matrix<double,3,3> REigen;
  REigen(0,0) = R.at<double>(cv::Point(0,0));
  REigen(1,0) = R.at<double>(cv::Point(0,1));
  REigen(2,0) = R.at<double>(cv::Point(0,2));
  REigen(0,1) = R.at<double>(cv::Point(1,0));
  REigen(1,1) = R.at<double>(cv::Point(1,1));
  REigen(2,1) = R.at<double>(cv::Point(1,2));
  REigen(0,2) = R.at<double>(cv::Point(2,0));
  REigen(1,2) = R.at<double>(cv::Point(2,1));
  REigen(2,2) = R.at<double>(cv::Point(2,2));

  Eigen::PartialPivLU<Mat3> lu(KEigen);
  Vec3 tEigen = lu.solve(PTranslationEigen);

  cv::Mat t = cv::Mat::zeros(cv::Size(1,3),CV_64FC1);
  t.at<double>(cv::Point(0,0)) = tEigen(0);
  t.at<double>(cv::Point(0,1)) = tEigen(1);
  t.at<double>(cv::Point(0,2)) = tEigen(2);

  if(REigen.determinant()<0) {
    R = -R;
    t = -t;
  }

  // scale K so that K(2,2) = 1
  K = K / K.at<double>(cv::Point(2,2));

  *Kp = K;
  *Rp = R;
  *tp = t;
}*/

} // namespace pointcloud2DepthCue
