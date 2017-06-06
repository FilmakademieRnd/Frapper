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
//! \file "pointcloud2DepthCue.h"
//! \brief Header file for pointcloud2DepthCue class.
//!
//! \author		Michael Bussler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \author		Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!
//!

#include "Log.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include "ImageNode.h"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "QMutex"
#include "QProgressDialog"
#include "qstringlist.h"
#include "qvector.h"
#include "qdir.h"
#include "qtextstream.h"
#include "qfileinfo.h"

//#include <Eigen/Core>
//#include <Eigen/LU>

namespace pointcloud2DepthCue {
using namespace Frapper;

//!
//! Class representing pointcloud2DepthCue
//!
class pointcloud2DepthCue : public ImageNode
{
    Q_OBJECT

//typedef Eigen::Vector3d Vec3;
//typedef Eigen::Matrix<double, 3, 3> Mat3;

public: /// constructors and destructors

    //!
    //! Constructor of the pointcloud2DepthCue class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    pointcloud2DepthCue ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the pointcloud2DepthCue class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual ~pointcloud2DepthCue ();

	//void decomposeP(const cv::Mat &P, cv::Mat *Kp, cv::Mat *Rp, cv::Mat *tp);
	
private slots:

	//!
	//! Updates the output image
	//!
	void run();
	

private: //data
	//PointCloud as cv::Mat (for matrixmultiplication)
	cv::Mat pointCloud;
	//PointCloud as Vector (for read in)
	QVector<cv::Mat> pointCloudInitial;
	QVector<QVector<int>> pointCloudVisibilities;
	QVector<cv::Mat> cameras;

};

} // namespace pointcloud2DepthCue
