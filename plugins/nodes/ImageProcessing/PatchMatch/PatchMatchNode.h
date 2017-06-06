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
//! \file "PatchMatchNode.h"
//! \brief Header file for PatchMatchNode class.
//!
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
#include "qvector2d.h"
#include "qelapsedtimer.h"
#include "qmath.h"

namespace PatchMatchNode {
using namespace Frapper;

//!
//! Typedefiniton of 2 element vector holding xy coordinate
//!
typedef cv::Vec<unsigned short, 2> Vec2us;

//!
//! Class representing PatchMatchNode
//!
class PatchMatchNode : public ImageNode
{
    Q_OBJECT

public: /// constructors and destructors

    //!
    //! Constructor of the PatchMatchNode class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    PatchMatchNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the PatchMatchNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual ~PatchMatchNode ();
	
private slots:

	//!
	//! Updates the output image
	//!
	void run();

private:
	//!
	//! get Distance of two pixels
	//!
	float getMatchingCost(std::vector<cv::Mat> *img1, std::vector<cv::Mat> *img2, cv::Mat *img1gradient, cv::Mat *img2gradient,int x1, int y1, int patchSize, float edgeGamma, float balanceAlpha, float tauCol, float tauGrad, float a, float b, float c, int maxDisparity, bool direction, float pitch, float xDirection );

	//!
	//! get L1 Norm of 2 rgb Pixels of splitted CV_8U image
	//!
	float normL1(std::vector<cv::Mat> *img1, std::vector<cv::Mat> *img2, int x1, int y1, int x2, int y2);

	//!
	//! get L1 Norm of 2 rgb Pixels of splitted CV_8U image with subpixel precision in second coordinates
	//!
	float normL1subPixel(std::vector<cv::Mat> *img1, std::vector<cv::Mat> *img2, int x1, int y1, float x2, float y2);

private: //data
	QStringList *cam1;
	QStringList *cam2;
	FilenameParameter *m_filenameParameter;


};
Q_DECLARE_METATYPE(std::vector<cv::Mat>);

} // namespace PatchMatchNode
