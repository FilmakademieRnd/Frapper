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
//! \file "VectorMathNode.h"
//! \brief Header file for VectorMathNode class.
//!
//! \author     Felix Bucella <filmakademie@bucella.de>
//! \version    1.0
//! \date       17.10.2013 (last updated)
//!

#ifndef VECTORMATHNODE_H
#define VECTORMATHNODE_H

#include "Node.h"
#include <QHash>

namespace VectorMathNode {
using namespace Frapper;

//!
//! Class representing nodes for head animation logic in a 3D scene.
//!
class VectorMathNode : public Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the VectorMathNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    VectorMathNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the BorAnimationNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~VectorMathNode ();

private slots: //

    //!
    //! Processes the output parameter depending on its name .
    //!
    void processOutputParameter ();

	//!
    //! Creates the input parameters for the node.
    //!
	void setupParameters(Parameter *sourceParameter);

	//!
	//! add an operation Dropdown with the given id as name
	//!
	//! \param id The name for the parameter
	//!
	void addOperation(int id);

	//!
	//! Should becalled when the parameter is connected. This function makes the parameter locked in the parameter panel
	//!
	void markAsConnected();

	//!
	//! Should becalled when the parameter is disconnected. This function makes the parameter editable in the parameter panel
	//!
	void markAsDisconnected();


private: //functions

	//!
	//! set the size of all numberparameters within the node
	//!
	void setSizeOfAllParams();

	//!
	//! add another input slot
	//!
	void addInput();

	//!
	//! add an input slot with the given id as name
	//!
	//! \param id The name for the parameter
	//!
	void addInput(int id);

	//!
	//! add an output slot for the result with the given id as name
	//!
	//! \param id The name for the parameter
	//!
	void addResultOut(int id);

	//!
	//! Generate a QVariantList with the neutral element
	//!
	//! \param Value Create a list of given values
	//!
	QVariantList createValueList(const float value);

	//!
	//! Add two numberparameters and save the result in the third parameter
	//!
	//! \param Operand The first operand for the calculation
	//! \param Operand The second operand for the calculation
	//! \param Result The result of the addition
	//!
	void addParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result);

	//!
	//! Subtract the second numberparameter from the first and save the result in the third parameter
	//!
	//! \param Operand The first operand for the calculation
	//! \param Operand The second operand for the calculation
	//! \param Result The result of the subtraction
	//!
	void subtractParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result);

	//!
	//! Multiply two numberparameters and save the result in the third parameter
	//!
	//! \param Operand The first operand for the calculation
	//! \param Operand The second operand for the calculation
	//! \param Result The result of the multiplication
	//!
	void multiplyParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result);

	//!
	//! Divide the first numberparameter by teh second and save the result in the third parameter. Error if division by 0
	//!
	//! \param Operand The first operand for the calculation
	//! \param Operand The second operand for the calculation
	//! \param Result The result of the division
	//!
	void divideParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result);

private: // data

	ParameterGroup *m_inParamGroup;

	int m_nodeParamSize;
	

};

} // namespace VectorMathNode 

#endif
