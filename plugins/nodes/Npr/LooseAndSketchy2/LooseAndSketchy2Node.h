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
//! \file "LooseAndSketchy2Node.h"
//! \brief Header file for LooseAndSketchy2Node class.
//!
//! \author     Marc Spicker
//! \version    1.0
//! \date       03.02.2015 (last updated)
//!

#ifndef LOOSEANDSKETCHY2NODE_H
#define LOOSEANDSKETCHY2NODE_H

#include "RenderNode.h"
#include <gl/gl.h>
#include "Vector2d.h"

namespace LooseAndSketchy2Node {

using namespace Frapper;

//!
//! Base class for all render nodes.
//!
class LooseAndSketchy2Node : public RenderNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the LooseAndSketchy2Node class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    LooseAndSketchy2Node ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the LooseAndSketchy2Node class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~LooseAndSketchy2Node ();

public slots:

    //!
    //! Redraw of ogre scene has been triggered.
    //!
    virtual void redrawTriggered ();

	//!
	//! Force to rebuild the render target. 
	//! 
	virtual void rebuildRendertargets ();

private slots: // Private Slots

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

private: // Private Methods

	void computeLooseAndSketchy2();

	void progressLooseAndSketchy2();

    void renderLooseAndSketchy2();

private: // Private Helper Methods

	std::vector<Vector2d> m_seeds;
};

} // end namespace

#endif

