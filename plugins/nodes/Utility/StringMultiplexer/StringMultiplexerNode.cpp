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
//! \file "StringMultiplexerNode.h"
//! \brief Implementation file for StringMultiplexerNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       18.05.2011 (last updated)
//!

#include "StringMultiplexerNode.h"

namespace StringMultiplexerNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the StringMultiplexerNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
StringMultiplexerNode::StringMultiplexerNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
    // set affections and callback functions
    addAffection("Activate Input 1", "Output String");
    addAffection("Activate Input 2", "Output String");
    addAffection("Activate Input 3", "Output String");
    addAffection("Activate Input 4", "Output String");
    //addAffection("Input String 1", "Output String");
    //addAffection("Input String 2", "Output String");
    //addAffection("Input String 3", "Output String");
    //addAffection("Input String 4", "Output String");
    setProcessingFunction("Output String", SLOT(processOutputParameter()));

}


//!
//! Destructor of the StringMultiplexerNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
StringMultiplexerNode::~StringMultiplexerNode ()
{
}


///
/// Public Methods
///


//!
//! Process on output request.
//!
void StringMultiplexerNode::processOutputParameter ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;

    const QString &parameterName = parameter->getName();
    
    for (int i = 0; i < 10; ++i) {
        Parameter *parameter = getParameter("Input String " + QString::number(i));
        if(!parameter)
            return;
        
        if(parameter->getValue().toBool())
            setValue("Output String", true);
    }
}

} // namespace StringMultiplexerNode 
