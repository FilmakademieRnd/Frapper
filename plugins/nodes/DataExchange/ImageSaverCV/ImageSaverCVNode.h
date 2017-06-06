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
//! \file "ImageSaverCVNode.h"
//! \brief Header file for ImageSaverCVNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       16.03.2014
//!

#ifndef IMAGESAVERCVNODE_H
#define IMAGESAVERCVNODE_H

#ifdef _WIN32
#include "cv.h"         // from OpenCV
#include "cxcore.h"     // from OpenCV
#include "highgui.h"    // from OpenCV, for CvCapture, cvReleaseCapture, cvCaptureFromCAM, cvGrabFrame, cvRetrieveFrame, ...
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

#include "Node.h"
#include <QtCore/QThread>
#include <QWaitCondition>
#include <QMutex>
//#include "InstanceCounterMacros.h"

namespace ImageSaverCVNode {
using namespace Frapper;

//!
//! Class representing nodes that save input images.
//!
//!
class ImageSaverCVNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the ImageFilterNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ImageSaverCVNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ImageFilterNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ImageSaverCVNode ();

private slots: //

    //!
    //! Saves the node's input images to file
    //!
    void saveImage ();

private: // methods
	
	//!
	//! Thread to get newFrame from Camera
	//!
	virtual void run();

private: // member variables

	//!
    //! Is termination of run-thread requested?
    //!
	bool m_endThreadRequested;

    //!
    //! Image counter
    //!
    int m_imageCounter;

	//!
	//! Is used to set the workerthread to sleep
	//!
	QWaitCondition m_sleepThread;

	//!
	//! Is used by moreFrames to syncronize Mainthread with workerthread
	//!
	QMutex m_frameVideoMutex;

	GenericParameter *m_inputImage;
	FilenameParameter *m_filenameParameter;
	NumberParameter *m_timeParameter;

	std::vector<int> m_compression_params;
};
Q_DECLARE_METATYPE(cv::Mat*);

} // namespace ImageSaverCVNode 

#endif
