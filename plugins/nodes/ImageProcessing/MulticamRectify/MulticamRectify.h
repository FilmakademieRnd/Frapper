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
//! \file "MulticamRectify.h"
//! \brief Header file for MulticamRectify class.
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
//#include "cv.h"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "QMutex"
#include "QProgressDialog"
#include "qstringlist.h"
#include "qvector.h"

namespace MulticamRectify {
using namespace Frapper;
using namespace cv;

//!
//! Class representing MulticamRectify
//!
class MulticamRectify : public ImageNode
{
    Q_OBJECT

public: /// constructors and destructors

    //!
    //! Constructor of the MulticamRectify class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    MulticamRectify ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the MulticamRectify class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual ~MulticamRectify ();
	
private slots:

	//!
	//! Updates the output image
	//!
	void run();
	

private: //data
	QVector<QStringList> *imagePathes;
	int minFrameCount;
	FilenameParameter *m_filenameParameter;

};
Q_DECLARE_METATYPE(std::vector<cv::Mat>);

} // namespace MulticamRectify
