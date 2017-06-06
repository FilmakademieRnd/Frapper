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
//! \file "ColorCorrectionNode.h"
//! \brief Header file for ColorCorrectionNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       06.08.2013 (last updated)
//!

#ifndef COLORCORRECTIONNODE_H
#define COLORCORRECTIONNODE_H

#include "CompositorNode.h"
#include "InstanceCounterMacros.h"

namespace ColorCorrectionNode {
using namespace Frapper;

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class ColorCorrectionNode : public CompositorNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the ColorCorrectionNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ColorCorrectionNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ColorCorrectionNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ColorCorrectionNode ();


private slots:

    //!
    //! Changes the output pixel format.
    //!
    void changeOutputPixelFormat ();

	//!
    //! Creates a lookup table as a texture based on the lut parameter curves.
    //!
    void updateLutTexture ();

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

private: // members

	//!
	//! The LUT parameters used for color correction
	//!
	NumberParameter *m_lutR, *m_lutG, *m_lutB, *m_lutA;

	//!
	//! The LUT texture used for color correction
	//!
	Ogre::TexturePtr m_lutTexture;

    //!
    //! The pixel format of the output texture.
    //!
    Ogre::PixelFormat m_pixelFormat;

	//!
    //! The value for switchung the shaders.
    //!
	QString m_operationValue;

};

} // namespace ColorCorrectionNode
#endif
