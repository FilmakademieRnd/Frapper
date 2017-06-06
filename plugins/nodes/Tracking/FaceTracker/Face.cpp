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

///
//!
//! \file "src\Face.cpp"
//! \brief Implementation of Face class.
//!
//! Contains the class for representing a face detected in a scene.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//!			    most parts from Stefan Habel <stefan.habel@filmakademie.de>
//! \version    0.1
//! \date       10.04.2008 (last updated)
//!
///

#include "Face.h"

#include <math.h>   // for float abs(float)

namespace FaceTrackerNode {

///
///   DEFINES
///


//!
//! Threshold in normalized device coordinates, that is used when comparing rects.
//! (two rects are considered equal if their coordinates differ in a number smaller than the threshold)
//!
#define THRESHOLD 0.1f


//!
//! Value of decreasing the probability of a face detection by time.
//!
#define PROBABILITY_DECREASE_PER_TIME_STEP 0.1f


//!
//! Value of increasing the probability of a face detection when a face is detected.
//!
#define PROBABILITY_INCREASE_PER_DETECTION 0.2f


///
///   (DE-)CONSTRUCTION
///


//!
//! Constructor.
//! Initializes private member variables.
//!
//! \param initialRect  the rectangle of the face's initial detection in NDC
//!
Face::Face(RectNDC initialRect)
{
    m_rect        = initialRect;
    m_probability = PROBABILITY_INCREASE_PER_DETECTION;
}


//!
//! Destructor.
//!
Face::~Face()
{
    // empty
}


///
///   PUBLIC FUNCTIONS
///


//!
//! Returns the point of attraction for the face, which is located approximately centered between the eyes.
//! (The x coordinate of the point of attraction is located in the center of the rectangle, and the y coordinate
//! is located at about 40% of the rectangle's height.)
//!
//! \return The point of attraction for the face in normalized device coordinates.
//!
PointNDC Face::getPointOfAttraction()
{
    PointNDC p;
    p.x = m_rect.x + m_rect.w / 2;
    p.y = m_rect.y + m_rect.h * 0.4f;
    return p;
}


//!
//! Returns the rectangular normalized device coordinates of the face's last detection.
//!
//! \return The rectangle of the face's last detection in NDC.
//!
RectNDC Face::getRect()
{
    return m_rect;
}


//!
//! Returns the probability of the face object actually representing a face in the scene.
//!
//! \return The probability value.
//!
float Face::getProbability()
{
    return m_probability;
}


//!
//! Checks if the given detected face rect is considered a detection of this face.
//!
//! \param detectionRect    the rectangle to test for a match with the current NDC of this face
//! \return True, if the rect is considered a detection of this face, otherwise False.
//!
bool Face::checkDetectionRect(RectNDC detectionRect)
{
    // check if the given detection rect is considered equal to the face rect
    bool result = abs(detectionRect.x - m_rect.x) < THRESHOLD
               && abs(detectionRect.y - m_rect.y) < THRESHOLD
               && abs(detectionRect.w - m_rect.w) < THRESHOLD
               && abs(detectionRect.h - m_rect.h) < THRESHOLD;
    if (result) {
        // update the member rect's coordinates with the coordinates of the given rect
        m_rect = detectionRect;
        // increase probability
        if (m_probability < 1.0f) {
            m_probability += PROBABILITY_INCREASE_PER_DETECTION;
            if (m_probability > 1.0f)
                m_probability = 1.0f;
        }
    }
    return result;
}


//!
//! Decreases the probability value of a face detection by a constant value.
//! Is used to decrease probability by time, if no more matches of face detection rects occur.
//!
void Face::decreaseProbability ()
{
    if (m_probability > 0.0f) {
        m_probability -= PROBABILITY_DECREASE_PER_TIME_STEP;
        if (m_probability < 0.0f)
            m_probability = 0.0f;
    }
}

} // namespace FaceTrackerNode 
