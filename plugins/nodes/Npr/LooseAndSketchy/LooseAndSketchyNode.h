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
//! \file "LooseAndSketchyNode.h"
//! \brief Header file for LooseAndSketchyNode class.
//!
//! \author     Thomas Luft <thomas.luft@web.de>
//! \version    1.0
//! \date       18.05.2009 (last updated)
//!

#ifndef LOOSEANDSKETCHYNODE_H
#define LOOSEANDSKETCHYNODE_H

#include "RenderNode.h"
//#include "ree/Stroke.h"
#include "Vector2d.h"
#include <gl/gl.h>

class Stroke
{
public:
    std::vector<Vector2d> vertices;
    Vector2d e0;
    float length;

public:
    void computeLength()
    {
        length = 0;
        for (int i = 0; i < vertices.size()-1; ++i)
        {
            length += (vertices[i] - vertices[i+1]).length();
        }
    }
};

using namespace Frapper;

//!
//! Base class for all render nodes.
//!
class LooseAndSketchyNode : public RenderNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the LooseAndSketchyNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    LooseAndSketchyNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the LooseAndSketchyNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~LooseAndSketchyNode ();

public slots:

    //!
    //! Redraw of ogre scene has been triggered.
    //!
    virtual void redrawTriggered ();

private slots: // Private Slots

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

private: // Private Methods

    //!
    //! Generate one loose and sketchy step.
    //!
    void progressLooseAndSketchy();

    //!
    //! Compute one loose and sketchy step.
    //!
    void computeLooseAndSketchy();

    //
    // Add a new stroke.
    //
    // \param stroke The stroke which should be added.
    //
    void addStroke(Stroke& stroke);

    //!
    //! Render the stroke geometry.
    //!
    void renderLooseAndSketchy();

private: // Private Helper Methods

    //!
    //! Converts 2D texture position in 1D array index.
    //!
    //! \param Position which should be converted.
    //! \return Index in gradient map array.
    //!
    unsigned int getGradientMapIndex(const Vector2d& pos);

    //!
    //! Returns value of gradient map at position pos.
    //!
    //! \param Position in gradient map.
    //! \return Value of gradient map at position pos.
    //!
    Vector2d getGradientMap(const Vector2d& pos);

    //float getGradientMapIntensity(const Vector2d& pos);
    
    //!
    //! Returns a random double value.
    //!
    //! \return Value of gradient map at position pos.
    //!
    double getRandom();

    //!
    //! Returns whether length of stroke s0 is greater than the
    //! length of s1.
    //!
    //! \return True if length(s0) > length(s1).
    //!
    static bool sortByLength(Stroke s0, Stroke s1);

private: // Private Member Variables

    //!
    //! Vector of strokes.
    //!
    std::vector<Stroke> m_strokes;
    
    //!
    //! Gradient map array.
    //!
    float *m_gradientMap;
};
#endif
