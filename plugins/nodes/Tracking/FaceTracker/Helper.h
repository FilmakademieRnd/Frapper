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
//! \file "Helper.h"
//! \brief Helper funtions.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       26.08.2009 (last updated)
//!

#ifndef Helper_h
#define Helper_h

#include <cstdlib>

namespace FaceTrackerNode {

///
///   CONSTANTS
///


//!
//! Epsilon value to use for comparing float values.
//! \par Example:
//! <code>if (abs(f) &lt; EPSILON) ...</code>
//!
//! \see Vision
//!
static const float EPSILON = 0.000000000001f;


///
///   STRUCTURED TYPES
///


//!
//! Data structure for a point coordinate in normalized device coordinates.
//! (ranging from 0.0 to 1.0 on each axis)
//!
//! \see Behavior::Message::Attractor, Awareness::Module::Vision, Behavior::Core
//!
typedef struct PointNDC {
    float x;    //!< The normalized x coordinate of the point.
    float y;    //!< The normalized y coordinate of the point.
} PointNDC;


//!
//! Data structure for the position and size of a rectangular shape in normalized device coordinates.
//! (specified by its top-left point and its width and height)
//!
//! \see Awareness::Module::Vision
//!
typedef struct RectNDC {
    float x;    //!< The normalized x coordinate of the rectangle's top-left point.
    float y;    //!< The normalized y coordinate of the rectangle's top-left point.
    float w;    //!< The rectangle's normalized width.
    float h;    //!< The rectangle's normalized height.
} RectNDC;

//!
//! Data structure for an area in normalized coordinates that attracts the focus point (the point for the agent to look at).
//!
//! \see Behavior, AttractorMessage
//!
typedef struct Attractor {
    PointNDC focusPoint;    //!< The coordinates of the point of interest of the attractor.
    float    size;          //!< The relative size of the attractor's area (corresponds to the size of a detected face relative to the size of the image taken by the webcam).
} Attractor;

///
///   FACTORY FUNCTIONS
///


//!
//! Creates a point with the given normalized coordinates.
//!
//! \param x    the normalized x coordinate of the point
//! \param y    the normalized y coordinate of the point
//! \return A point with the given normalized coordinates.
//!
static PointNDC CreatePointNDC ( float x, float y )
{
    PointNDC p;
    p.x = x;
    p.y = y;
    return p;
}

   ///                    ///////////////////////////////////////////////////////////////////////////////////////
  ///   RANDOM NUMBERS   ///////////////////////////////////////////////////////////////////////////////////////
 ///                    ///////////////////////////////////////////////////////////////////////////////////////


//!
//! Returns a random number between 0 and the given max value.
//!
//! \param max  the maximum value
//! \return A random number between 0 and the given maximum.
//!
static unsigned int getRandom ( unsigned int max )
{
    return (unsigned int) ((float) rand() / RAND_MAX * max);
}


//!
//! Returns a random float number between 0.0f and the given max value.
//!
//! \param max  the maximum value
//! \return A random float number between 0.0f and the given maximum.
//!
static float getRandom ( float max )
{
    return ((float) rand() / RAND_MAX * max);
}


//!
//! Returns a random float number between 0.0f and 1.0f.
//!
//! \return A random float number between 0.0f and 1.0f.
//!
static float getRandom ()
{
    return getRandom(1.0f);
}

} // namespace FaceTrackerNode 

#endif
