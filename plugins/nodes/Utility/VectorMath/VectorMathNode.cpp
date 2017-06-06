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
//! \brief Implementation file for VectorMathNode class.
//!
//! \author     Felix Bucella <filmakademie@bucella.de>
//! \version    1.0
//! \date       17.10.2013 (last updated)
//!

#include "VectorMathNode.h"

namespace VectorMathNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the VectorMathNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
VectorMathNode::VectorMathNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	// Make General Values Savelable
	setSaveable(true);

	// Set the size of all numberparameters within this node to one at the beginning.
	m_nodeParamSize = 1;

	// Generate the parameters of this node.
	addInput(1);
	addOperation(1);
	addInput(2);
	addResultOut(1);

}


//!
//! Destructor of the VectorMathNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
VectorMathNode::~VectorMathNode ()
{
}


//!
//! add an operation Dropdown with the given id as name
//!
//! \param id The name for the parameter
//!
void VectorMathNode::addOperation(int id) {
	
	ParameterGroup* rootGroup = getParameterRoot();

	EnumerationParameter *enumParam = new EnumerationParameter(QString("Operation %1").arg(id), 0);
	QStringList opr;
	QStringList num; 
	opr.append("[+] Add");
	opr.append("[-] Subtract");
	opr.append("[*] Multiply");
	opr.append("[/] Divide");
	num.append(QString::number(1));
	num.append(QString::number(2));
	num.append(QString::number(3));
	num.append(QString::number(4));
	enumParam->setLiterals(QStringList() << opr);
	enumParam->setValues(QStringList() << num);
	rootGroup->addParameter(enumParam);
	
	enumParam->setChangeFunction(SLOT(processOutputParameter()));

	// update node
	notifyChange();
	forcePanelUpdate();
}


//!
//! add an input slot with the given id as name
//!
//! \param id The name for the parameter
//!
void VectorMathNode::addInput(int id) {

	ParameterGroup* rootGroup = getParameterRoot();
	
	// set up new input parameter 
	NumberParameter *numberParameter = new NumberParameter(QString("In %1").arg(id), Parameter::T_Float, 0.0f);
	numberParameter->setNode(this);
	numberParameter->setPinType(Parameter::PT_Input);
	//numberParameter->setSelfEvaluating(true);
	numberParameter->setMinValue(-10000.0);
	numberParameter->setMaxValue(10000.0);
	numberParameter->setSize(m_nodeParamSize);

	// set values to 0.0
	if (m_nodeParamSize > 1) {
		numberParameter->setValues(createValueList(0));
		numberParameter->setDefaultValue(createValueList(0));
	}

	rootGroup->addParameter(numberParameter);
	numberParameter->setVisible(true);

	numberParameter->setProcessingFunction(SLOT(processOutputParameter()));
	numberParameter->setOnCreateConnectionFunction(SLOT(setupParameters(Parameter *)));
	numberParameter->setOnConnectFunction(SLOT(markAsConnected()));
	numberParameter->setOnDisconnectFunction(SLOT(markAsDisconnected()));

	// update node
	notifyChange();
	forcePanelUpdate();
}


//!
//! add an output slot for the result with the given id as name
//!
//! \param id The name for the parameter
//!
void VectorMathNode::addResultOut(int id) {

	ParameterGroup* rootGroup = getParameterRoot();
	
	// set up new input parameter 
	NumberParameter *numberParameter = new NumberParameter(QString("Result %1").arg(id), Parameter::T_Float, 0.0f);
	numberParameter->setNode(this);
	numberParameter->setPinType(Parameter::PT_Output);
	numberParameter->setSelfEvaluating(true);
	numberParameter->setMinValue(-10000.0);
	numberParameter->setMaxValue(10000.0);
	numberParameter->setReadOnly(true);
	numberParameter->setSize(m_nodeParamSize);

	// set values to 0.0
	if (m_nodeParamSize > 1) {
		numberParameter->setValues(createValueList(0));
		numberParameter->setDefaultValue(createValueList(0));
	}

	rootGroup->addParameter(numberParameter);
	numberParameter->setVisible(true);

	numberParameter->setProcessingFunction(SLOT(processOutputParameter()));

	// update node
	notifyChange();
	forcePanelUpdate();
}


//!
//! Creates the input parameters for the node.
//!
void VectorMathNode::setupParameters(Parameter *sourceParameter) {

	NumberParameter *inParameter = dynamic_cast<NumberParameter *>(sender());
	if (!inParameter || !sourceParameter)
		return;

	// Get list of Parameters
	ParameterGroup* rootGroup = getParameterRoot();
	const AbstractParameter::List& paramList = rootGroup->getParameterList();

	// There must be no parameter connected
	for (int i = 0; i < paramList.size(); ++i) {
		if( static_cast<Parameter *>(paramList.at(i))->getType() == Parameter::T_Float) {
			if( static_cast<Parameter *>(paramList.at(i))->isConnected()) {
				return;
			}
		}
	}

	m_nodeParamSize = sourceParameter->getSize();
	inParameter->setSize(m_nodeParamSize);

	setSizeOfAllParams();
}


//!
//! set the size of all numberparameters within the node
//!
void VectorMathNode::setSizeOfAllParams() {

	ParameterGroup* rootGroup = getParameterRoot();

	// Get list of Parameters
	const AbstractParameter::List& paramList = rootGroup->getParameterList();

	Parameter* temp;

	// Set size of all input parameters
	for (int i = 0; i < paramList.size(); ++i){
		temp = static_cast<Parameter *>(paramList.at(i));
		// set values to 0.0
		if (temp->getType() == Parameter::T_Float){
			if (m_nodeParamSize > 1){

				temp->setSize(m_nodeParamSize);
				temp->setValues(createValueList(0));
				temp->setDefaultValue(createValueList(0));
			}
			else {
				temp->setValue(QVariant(0.0));
				temp->setDefaultValue(QVariant(0.0));
			}
		}
	}
}


//!
//! Should becalled when the parameter is connected. This function makes the parameter locked in the parameter panel
//!
void VectorMathNode::markAsConnected() {
	
	NumberParameter *inParameter = dynamic_cast<NumberParameter *>(sender());
	if (!inParameter)
		return;

	inParameter->setReadOnly(true);
}


//!
//! Should becalled when the parameter is disconnected. This function makes the parameter editable in the parameter panel
//!
void VectorMathNode::markAsDisconnected() {
	
	NumberParameter *inParameter = dynamic_cast<NumberParameter *>(sender());
	if (!inParameter)
		return;

	inParameter->setReadOnly(false);
}


//!
//! Generate a QVariantList with the neutral element
//!
QVariantList VectorMathNode::createValueList(const float value) {
	QVariantList final = QVariantList();
	for (int i = 0; i < m_nodeParamSize; ++i) {
		final.append(QVariant(value));
	}
	return final;
}


//!
//! Processes the result for the out parameter
//!
void VectorMathNode::processOutputParameter () {

	ParameterGroup* rootGroup = getParameterRoot();

	NumberParameter* oper1;
	NumberParameter* oper2;
	NumberParameter* result1;

	oper1 = getNumberParameter("In 1");
	oper2 = getNumberParameter("In 2");
	result1 = getNumberParameter("Result 1");

	EnumerationParameter *operatorEnum1 = getEnumerationParameter("Operation 1");

	QString value = operatorEnum1->getCurrentValue();
	int op = value.toFloat();

	
	switch (op)
	{
		case 1:
			addParams(oper1, oper2, result1);
			break;
		case 2:
			subtractParams(oper1, oper2, result1);
			break;
		case 3:
			multiplyParams(oper1, oper2, result1);
			break;
		case 4:
			divideParams(oper1, oper2, result1);
			break;
	}
}


//!
//! Add two numberparameters and save the result in the third parameter
//!
//! \param Operand The first operand for the calculation
//! \param Operand The second operand for the calculation
//! \param Result The result of the addition
//!
void VectorMathNode::addParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result) {
	if (m_nodeParamSize == 1) {
		result->setValue(QVariant(oper1->getValue().toFloat() + oper2->getValue().toFloat()), true);
	}
	else if ((m_nodeParamSize > 1)) {
		const QVariantList &operList1 = oper1->getValues();
		const QVariantList &operList2 = oper2->getValues();
		QVariantList resultList;
		for (int i=0; i<operList1.size(); ++i) {
			resultList.append(QVariant(operList1.at(i).toFloat() + operList2.at(i).toFloat()));
		}
		result->setValues(resultList, true);
	}
}


//!
//! Subtract the second numberparameter from the first and save the result in the third parameter
//!
//! \param Operand The first operand for the calculation
//! \param Operand The second operand for the calculation
//! \param Result The result of the subtraction
//!
void VectorMathNode::subtractParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result) {
	if (m_nodeParamSize == 1) {
		result->setValue(QVariant(oper1->getValue().toFloat() - oper2->getValue().toFloat()), true);
	}
	else if ((m_nodeParamSize > 1)) {
		const QVariantList &operList1 = oper1->getValues();
		const QVariantList &operList2 = oper2->getValues();
		QVariantList resultList;
		for (int i=0; i<operList1.size(); ++i) {
			resultList.append(QVariant(operList1.at(i).toFloat() - operList2.at(i).toFloat()));
		}
		result->setValues(resultList, true);
	}
}


//!
//! Multiply two numberparameters and save the result in the third parameter
//!
//! \param Operand The first operand for the calculation
//! \param Operand The second operand for the calculation
//! \param Result The result of the multiplication
//!
void VectorMathNode::multiplyParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result) {
	if (m_nodeParamSize == 1) {
		result->setValue(QVariant(oper1->getValue().toFloat() * oper2->getValue().toFloat()), true);
	}
	else if ((m_nodeParamSize > 1)) {
		const QVariantList &operList1 = oper1->getValues();
		const QVariantList &operList2 = oper2->getValues();
		QVariantList resultList;
		for (int i=0; i<operList1.size(); ++i) {
			resultList.append(QVariant(operList1.at(i).toFloat() * operList2.at(i).toFloat()));
		}
		result->setValues(resultList, true);
	}
}


//!
//! Divide the first numberparameter by teh second and save the result in the third parameter. Error if division by 0
//!
//! \param Operand The first operand for the calculation
//! \param Operand The second operand for the calculation
//! \param Result The result of the division
//!
void VectorMathNode::divideParams (NumberParameter* oper1, NumberParameter* oper2, NumberParameter* result) {
	if (m_nodeParamSize == 1) {
		if (oper2->getValue().toFloat() == 0) {
			Frapper::Log::error("Division by zero","VectorMathNode::subtractParams");
			result->setValue(QVariant(-9999));
			return;
		}
		result->setValue(QVariant(oper1->getValue().toFloat() / oper2->getValue().toFloat()), true);
	}
	else if ((m_nodeParamSize > 1)) {
		const QVariantList &operList1 = oper1->getValues();
		const QVariantList &operList2 = oper2->getValues();
		QVariantList resultList;
		for (int i=0; i<operList1.size(); ++i) {
			if (operList2.at(i).toFloat() == 0) {
				Frapper::Log::error("Division by zero","VectorMathNode::subtractParams");
				resultList.append(QVariant(-9999));
			} else {
				resultList.append(QVariant(operList1.at(i).toFloat() / operList2.at(i).toFloat()));
			}
		}
		result->setValues(resultList, true);
	}
}

} // namespace VectorMathNode 
