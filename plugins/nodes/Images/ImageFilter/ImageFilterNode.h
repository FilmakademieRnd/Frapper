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
//! \file "ImageFilterNode.h"
//! \brief Header file for ImageFilterNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       15.04.2009 (last updated)
//!

#ifndef IMAGEFILTERNODE_H
#define IMAGEFILTERNODE_H

#include "ImageNode.h"
#include "InstanceCounterMacros.h"


namespace ImageFilterNode {
using namespace Frapper;

//!
//! Class representing nodes that filter input images.
//!
//! \todo Stefan: Invalidate the image cache when the input image is changed.
//!
class ImageFilterNode : public ImageNode
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
    ImageFilterNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ImageFilterNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ImageFilterNode ();

private slots: //

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

    //!
    //! Resets the color correction parameters to their default values.
    //!
    void resetColorCorrection ();

    //!
    //! Marks the cache as being invalid so that it is cleared the next time
    //! the output image is processed.
    //!
    void invalidateCache ();

};

} // namespace ImageFilterNode

#endif
