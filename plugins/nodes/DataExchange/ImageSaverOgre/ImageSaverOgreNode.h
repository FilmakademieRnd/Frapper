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
//! \file "ImageSaverOgreNode.h"
//! \brief Header file for ImageSaverOgreNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       16.03.2014
//!

#ifndef IMAGESAVEROGRENODE_H
#define IMAGESAVEROGRENODE_H


#include "Node.h"

namespace ImageSaverOgreNode {
using namespace Frapper;

//!
//! Class representing nodes that save input images.
//!
//!
class ImageSaverOgreNode : public Node
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
    ImageSaverOgreNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ImageFilterNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ImageSaverOgreNode ();

private: // functions


private slots: //

    //!
    //! Saves the node's input images to file
    //!
    void saveImage ();

private: // member variables

    //!
    //! Image counter
    //!
    int m_imageCounter;

	Parameter *m_inputImage;
	FilenameParameter *m_filenameParameter;
	NumberParameter *m_timeParameter;
};

} // namespace ImageSaverOgreNode 

#endif
