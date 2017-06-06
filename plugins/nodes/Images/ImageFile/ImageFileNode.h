/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "ImageFileNode.h"
//! \brief Header file for ImageFileNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       03.04.2009 (last updated)
//!

#ifndef IMAGEFILENODE_H
#define IMAGEFILENODE_H

#include "ImageNode.h"
#include "InstanceCounterMacros.h"

namespace ImageFileNode {
using namespace Frapper;

//!
//! Class representing nodes that load image data from file.
//!
class ImageFileNode : public ImageNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the ImageFileNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ImageFileNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ImageFileNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ImageFileNode ();

private slots: //

    //!
    //! Handler function that is called when the value of the "Use Image Sequence"
    //! parameter has been changed.
    //!

    void useImageSequenceToggled();

	//!
	//! Handler function that is called when the value of the "Scale in %"
	//! parameter has been changed.
	//!
	//! Sets the member variable "m_scaleValue" which is used in 
	//! processOutputImage to calculate the new scale factor.
	//!
	void resizeImageChanged ();

	//!
	//! Handler function that is called when the value of the "Reset Image Size"
	//! button has been pressed.
	//!
	//! Sets the "Scale in %" back to 100%.
	//!
	void resetScalePressed ();

    //!
    //! Handler function that is called when the value of the image filename
    //! parameter has been changed.
    //!
    //! Detects whether the selected image file is part of a sequence of image
    //! files and sets the enabled state of parameters for image sequences
    //! accordingly.
    //!
    void filenameChanged ();

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

	//!
    //! Processes the node's default image.
    //!
    void processDefaultImage ();

private: // functions

    //!
    //! Decodes the given name of an image file from a sequence into its
    //! prefix, index and suffix.
    //!
    //! \param filename The filename to decode.
    //! \param prefix [out] The extracted prefix part of the filename.
    //! \param indexString [out] The extracted index part of the filename.
    //! \param index [out] The extracted index part of the filename converted to an integer number.
    //! \param suffix [out] The extracted prefix part of the filename.
    //!
    void decodeFilename ( const QString &filename, int *index, QString *prefix = 0, QString *indexString = 0, QString *suffix = 0 );

	//!
	//! The node's default image
	//!
	Ogre::Image m_defaultImage;
	QSize m_defaultSize;
	int m_scaleValue;
};

} // namespace ImageFileNode

#endif
