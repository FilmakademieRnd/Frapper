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
//! \file "ImageSaverOgreNode.cpp"
//! \brief Implementation file for ImageSaverOgreNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       16.03.2014
//!

#include "ImageSaverOgreNode.h"
#include "Log.h"
#include "OgreTools.h"
#include <QDateTime>

namespace ImageSaverOgreNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(ImageSaverOgreNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the ImageSaverOgreNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
//! /todo Christopher: Add user interface for saving options (e.g. compression quality, bit depth, alpha options etc.)
//!
ImageSaverOgreNode::ImageSaverOgreNode ( const QString &name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
	m_imageCounter(0)
{
	// set filename type to 'Save' for all filenames
	m_filenameParameter = new FilenameParameter("Filename","");	
	m_filenameParameter->setType(FilenameParameter::FT_Save);
	m_filenameParameter->setFilters("Bitmap (*.bmp);; JPEG (*.jpg);; PNG (*.png);; TIFF (*.tif)");
	parameterRoot->addParameter(m_filenameParameter);

	//gernerate a new GenericParameter
	m_inputImage = Parameter::createImageParameter("ImageIn");
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

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the ImageSaverOgreNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ImageSaverOgreNode::~ImageSaverOgreNode ()
{
    DEC_INSTANCE_COUNTER
}


///
/// Private Functions
///


///
/// Private Slots
///

//!
//! Saves the a texture to a file.
//!
void ImageSaverOgreNode::saveImage()
{
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
		Ogre::TexturePtr ogreImage = m_inputImage->getValue().value<Ogre::TexturePtr>();

		if (!ogreImage.isNull()) {
			const QString finalFilename = QString("%1_%2_%3.%4")
				.arg(filename.at(0))
				.arg(QString::number(m_imageCounter++))
				.arg(timeString)
				.arg(filename.at(1));

			OgreTools::HardwareBufferLocker hbl(ogreImage->getBuffer(), Ogre::HardwareBuffer::HBL_NO_OVERWRITE); // use HBL_NORMAL if any problems occur
			
			const Ogre::PixelBox &readrefpb = hbl.getCurrentLock();    
			uchar *readrefdata = static_cast<uchar*>(readrefpb.data);

			Ogre::Image img;
			img = img.loadDynamicImage(readrefdata, ogreImage->getWidth(), ogreImage->getHeight(), ogreImage->getFormat());    
			img.save(finalFilename.toStdString());
		}
	}
}


} // namespace ImageSaverOgreNode 
