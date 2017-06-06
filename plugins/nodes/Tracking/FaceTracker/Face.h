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
//! \file "include/Face.h"
//! \brief Header file for Face class.
//!
//! Contains the class for representing a face detected in a scene.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//!             Stefan Habel <stefan.habel@filmakademie.de>
//! \version    0.1
//! \date       10.04.2008 (last updated)
//!
///

#ifndef Face_h
#define Face_h

#include "Helper.h"

namespace FaceTrackerNode {

//!
//! Class for representing a face detected in a scene.
//!
class Face
{

public:
    Face(RectNDC initialRect);
    ~Face();

public:
    PointNDC getPointOfAttraction();
    RectNDC getRect();
    float getProbability();
    bool checkDetectionRect(RectNDC detectionRect);
    void decreaseProbability();

private:
    RectNDC m_rect;
    float m_probability;

};

} // namespace FaceTrackerNode 

#endif // #define Face_h