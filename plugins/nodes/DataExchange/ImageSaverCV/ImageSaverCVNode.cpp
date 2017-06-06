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
//! \file "ImageSaverCVNode.cpp"
//! \brief Implementation file for ImageSaverCVNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       16.03.2014
//!

#include "ImageSaverCVNode.h"
#include "Log.h"
#include <QDateTime>

namespace ImageSaverCVNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(ImageSaverCVNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the ImageSaverCVNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
//! /todo Christopher: Add user interface for saving options (e.g. compression quality, bit depth, alpha options etc.)
//!
ImageSaverCVNode::ImageSaverCVNode ( const QString &name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
	m_imageCounter(0),
	m_endThreadRequested(false)
{
	int cp[] = {CV_IMWRITE_JPEG_QUALITY, 50};
	m_compression_params = std::vector<int>(cp, cp+(sizeof(cp)/sizeof(cp[0])));

	// set filename type to 'Save' for all filenames
	m_filenameParameter = new FilenameParameter("Filename","");	
	m_filenameParameter->setType(FilenameParameter::FT_Save);
	m_filenameParameter->setFilters("Bitmap (*.bmp);; JPEG (*.jpg);; PNG (*.png);; TIFF (*.tif)");
	parameterRoot->addParameter(m_filenameParameter);

	m_inputImage = new GenericParameter("cvImage", 0); //gernerate a new GenericParameter
	m_inputImage->setTypeInfo("cv::Mat");
	m_inputImage->setDescription("ColorMat");
	m_inputImage->setPinType(Frapper::Parameter::PT_Input);
	m_inputImage->setSelfEvaluating(true);
	parameterRoot->addParameter(m_inputImage);
	m_inputImage->setProcessingFunction(SLOT(saveImage()));

	m_timeParameter = new NumberParameter("Time", Parameter::T_Int, 0);
	m_timeParameter->setMaxValue(std::numeric_limits<int>::max());
	m_timeParameter->setPinType(Parameter::PT_Input);
	m_timeParameter->setVisible(false);
	m_timeParameter->setSelfEvaluating(true);
	parameterRoot->addParameter(m_timeParameter);

	start(QThread::LowestPriority);

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the ImageSaverCVNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ImageSaverCVNode::~ImageSaverCVNode ()
{
	m_endThreadRequested = true;
    DEC_INSTANCE_COUNTER
}


///
/// Private Slots
///

//!
//! Saves the a texture to a file.
//!
void ImageSaverCVNode::saveImage()
{
	m_sleepThread.wakeOne();
	//// the base filename
	//const QStringList &filename = m_filenameParameter->getValue().toString().split('.');
	//QString timeString = "";
	//
	//const int inTime = m_timeParameter->getValue().toInt();

	//if (inTime > 0) {
	//	div_t qr = div(inTime, 1000);
	//	const int ms = qr.rem;
	//	qr = div(qr.quot, 60);
	//	const int s  = qr.rem;
	//	qr = div(qr.quot, 60);
	//	const int m  = qr.rem;
	//	const int h  = qr.quot;

	//	timeString = QString("%1%2_%3_%4")
	//		.arg(h, 2, 10, QLatin1Char('0'))
	//		.arg(m, 2, 10, QLatin1Char('0'))
	//		.arg(s, 2, 10, QLatin1Char('0'))
	//		.arg(ms, 4, 10, QLatin1Char('0'));
	//}

	//if (filename.size() > 1) {
	//	cv::Mat *cvImage = m_inputImage->getValue().value<cv::Mat*>();

	//	if (cvImage && cvImage->data) {
	//		const QString finalFilename = QString("%1_%2_%3.%4")
	//			.arg(filename.at(0))
	//			.arg(QString::number(m_imageCounter++))
	//			.arg(timeString)
	//			.arg(filename.at(1));

	//		cv::Mat cvImageFormated;
	//		// channel shift R<->B
	//		//cvtColor(*cvImage, cvImageFormated, CV_BGR2RGB);

	//		QImage qImage = QImage(cvImageFormated.data, cvImage->cols, cvImage->rows, QImage::Format_RGB888);
	//		qImage.save(finalFilename, filename.at(1).toStdString().c_str(), 95);
	//	}
	//}
}

//!
//! Thread to get newFrame from Camera
//!
void ImageSaverCVNode::run()
{
	forever {
		//if stopping of thread has been requested, stop the thread
		if ( m_endThreadRequested ) { 
			quit();
			return;		
		}

		//QMutexLocker locker(&m_frameVideoMutex); //lock mutex
		m_sleepThread.wait(&m_frameVideoMutex);

		// the base filename
		const QStringList &filename = m_filenameParameter->getValue().toString().split('.');
		QString timeString = "";

		const int inTime = m_timeParameter->getValue().toInt();

		if (inTime > 0) {
			div_t qr = div(inTime, 1000);
			const int ms = qr.rem;
			qr = div(qr.quot, 60);
			const int s  = qr.rem;
			qr = div(qr.quot, 60);
			const int m  = qr.rem;
			const int h  = qr.quot;

			timeString = QString("%1%2_%3_%4")
				.arg(h, 2, 10, QLatin1Char('0'))
				.arg(m, 2, 10, QLatin1Char('0'))
				.arg(s, 2, 10, QLatin1Char('0'))
				.arg(ms, 4, 10, QLatin1Char('0'));
		}

		if (filename.size() > 1) {
			cv::Mat *cvImage = m_inputImage->getValue().value<cv::Mat*>();

			if (cvImage && cvImage->data) {
				const QString finalFilename = QString("%1_%2_%3.%4")
					.arg(filename.at(0))
					.arg(QString::number(m_imageCounter++))
					.arg(timeString)
					.arg(filename.at(1));

				// old qt writeout...
				//// channel shift R<->B
				//cv::Mat cvImageFormated;
				//cvtColor(*cvImage, cvImageFormated, CV_BGR2RGB);

				//QImage qImage = QImage(cvImage->data, cvImage->cols, cvImage->rows, QImage::Format_RGB888);
				//qImage.save(finalFilename, filename.at(1).toStdString().c_str(), 95);

				cv::imwrite(finalFilename.toStdString(), *cvImage, m_compression_params);
			}
		}
	}
}


} // namespace ImageSaverCVNode 
