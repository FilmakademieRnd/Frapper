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
//! \file "String2BoolNode.h"
//! \brief Header file for String2BoolNode class.
//!
//! \author     Volker Helzle (volker.helzle@filmakademie.de)
//! \version    1.0
//! \date       02.2012 
//!
#ifndef STRING2BOOLNODE_H
#define STRING2BOOLNODE_H

#include "Node.h"
#include <QtGui>

namespace StringToBoolNode {
using namespace Frapper;

//!
//! Frapper Node, that generates one or multiple float values, that change over
//! time to represend a cycling action. Start at 0, peak at 1, end up at 0 again.
//! Every parameter is randomized, additionally a bool value is "fired" at each
//! begin of an Action Cycle (set to true for one frame).
//! The state of the node can be saved and loaded in an XML File.
//! Additional outputs with relative factors to the Master Value can be defined
//! and are also saveable.
//! If no XML File is defined, the general values of the node are still saved in
//! the scene - yet additional outputs are lost.
//!
class String2BoolNode : public Node
{
    Q_OBJECT

public: /// constructors

    //!
    //! Constructor of the String2BoolNode class.
    //!
    //! \param name The name to give the new mesh node.
    //!
    String2BoolNode ( QString name, ParameterGroup *parameterRoot );

public: /// destructors

    //!
    //! Destructor of the String2BoolNode class.
    //!
    ~String2BoolNode ();

public: /// static variables

public slots:

	//!
    //! compare input string with manually generated parameters and trigger corresponding output
    //!
	void delegateInput();

    //!
    //! outwards Variables are only updated on every new frame, not Timer Event
    //!
	void resetOutputs();

	//!
	//! outwards Variables are only updated on every new frame, not Timer Event
	//!
	void resetOutputsExceptChecked();

	//!
    //! is called every time the outputCount changes
    //!
	void updateOutputCount();

private: // private functions

	//!
    //! add another output slot
    //!
	void addOutput();

	//!
    //! delete newest output slot
    //!
	void deleteOutput();

private: // member variables

	QString m_StringIn;

	ParameterGroup *parameterRoot; // pointer to this Parameter Root

	QList <int> m_Outputs; // number of outputs

};

} // namespace StringToBoolNode 

#endif
