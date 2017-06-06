/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2012 Filmakademie Baden-Wuerttemberg, Institute of Animation 
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
//! \file "opencvRawInput.h"
//! \brief Header file for  class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       29.03.2012 (last updated)
//!
#ifndef OPENCVRAWINPUT_H
#define OPENCVRAWINPUT_H
#include "CurveEditorDataNode.h"
#include "GenericParameter.h"
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


using namespace Frapper;

class opencvRawInput : public CurveEditorDataNode
{
	Q_OBJECT
public: //constructors and destructors both are protected that only derivaded classes can use the constructor and destructor
    //!
    //! Constructor of the opencvRawInput class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    opencvRawInput (QString name, ParameterGroup *parameterRoot);

    //!
    //! Destructor of the opencvRawInput class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual inline ~opencvRawInput ();
	
	private slots: //slots
	//!
    //! connectMatrixSlot checks if the connected Parameter is valid
    //!
	void connectMatrixSlot();

	//!
	//! Is used if the generic Parameter is disconnected
	//!
	void disconnectMatrixSlot();

	protected slots: 
	//!
	//! SLOT that is used on the matrixParameter
	//!
	virtual void processInputMatrix();

	protected: //method

	//!
	//! This Method is used to do stuff if connectMatrixSlot() has succeed
	//!
	virtual void pastConnectToMatrix();

	protected: //data
	//!
	//! Pointer to the cv::Mat 
	//!
	cv::Mat* m_imagePtr;

	//!
	//! The width of the videoinput
	//!
	int m_width;

	//!
	//! The height of the videoinput
	//!
	int m_height;

	//!
	//! Is used to save the size of the videoinput -> width*height*(numberOfchannels)
	//!
	int m_videoSize;

	//!
	//! The Input Parameter
	//!
	Frapper::GenericParameter *m_matrixParameter;
};
Q_DECLARE_METATYPE(cv::Mat*);
#endif