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
//! \file "Index2IndexNode.cpp"
//! \brief Implementation file for Index2IndexNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    0.1
//! \date       09.08.2013 (last updated)
//!
//! \desc	This node receives a number of values (floats) which for one reason or another need to be mapped to another output. 
//! \desc	The user can manually introduce the corresponding output through the interface
//!

#include "Index2IndexNode.h"

namespace Index2IndexNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(Index2IndexNode)

///
/// Constructors and Destructors
///


//!
//! Constructor of the Index2IndexNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
Index2IndexNode::Index2IndexNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
	parameterRoot(parameterRoot)
{
	// Make General Values Savelable
	setSaveable(true);

	m_inputValues = new ParameterGroup("Inputs");
	parameterRoot->addParameter(m_inputValues);

	ParameterGroup *testGroup = new ParameterGroup("TestGroup");
	parameterRoot->addParameter(testGroup);

	m_threshold = getParameter("Threshold");

	// Set up a response to change of the number of outputs
	Parameter *outputCount = getParameter("Number of Parameters");
	outputCount->setChangeFunction(SLOT(setupParameters()));

	m_outIndex = getParameter("Index Output");
	m_outName = getParameter("Label Output");

	setupParameters();

	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the Index2IndexNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
Index2IndexNode::~Index2IndexNode ()
{
	DEC_INSTANCE_COUNTER
}


//!
//! Creates the input parameters for the node.
//!
void Index2IndexNode::setupParameters()
{
	Q_SLOT

		// get new "Number of Outputs"-Value from interface
		int count = getIntValue("Number of Parameters");

	if(count>m_Outputs.size()) {
		do {
			//
			// value greater than outputCount
			addOutput();

		} while(count>m_Outputs.size());
	} else while(count<m_Outputs.size()) {
		//
		// value smaller than outputCount
		deleteOutput();
	}
	forcePanelUpdate();
	
}

//!
//! add another output slot
//!
void Index2IndexNode::addOutput()
{
	// inputs
	NumberParameter *numberParameterin = new NumberParameter(QString("%1").arg(m_Outputs.size()), Parameter::T_Float, 0.0f);
	numberParameterin->setNode(this);
	numberParameterin->setPinType(Parameter::PT_Input);
	numberParameterin->setSelfEvaluating(true);
	numberParameterin->setMinValue(-1000.0);
	numberParameterin->setMaxValue(1000.0);
	numberParameterin->setProcessingFunction(SLOT(setParameters()));
	// increase m_inputValues
	m_inputValues->addParameter(numberParameterin);

	// get output parameter group
	ParameterGroup *m_OutputParameters = parameterRoot->getParameterGroup("Outputs");
	// set up new output parameter 
	Parameter *newOutput;
	// add float parameter
	newOutput = new Parameter(QString("out_%1").arg(m_Outputs.size()), Parameter::T_String, 0.0f);
	newOutput->setSelfEvaluating(true);
	newOutput->setVisible(true);
	m_OutputParameters->addParameter(newOutput); // add newParameter to group	
	// add string parameter
	newOutput = new Parameter(QString("name_%1").arg(m_Outputs.size()), Parameter::T_String, "");	
	newOutput->setPinType(Parameter::PT_Output);
	newOutput->setVisible(true);
	m_OutputParameters->addParameter(newOutput); // add newParameter to group

	// increase m_Outputs
	m_Outputs.append(1);

	// update node
	notifyChange();
}

//!
//! delete newest output slot
//!
void Index2IndexNode::deleteOutput()
{
	// get output parameter group
	ParameterGroup *m_OutputParameters = parameterRoot->getParameterGroup("Outputs");

	// delete parameter
	m_OutputParameters->removeParameter(QString("out_%1").arg(m_Outputs.size()-1));
	m_OutputParameters->removeParameter(QString("name_%1").arg(m_Outputs.size()-1));

	m_inputValues->removeParameter(QString("%1").arg(m_Outputs.size()-1));

	// decrease outputCount
	m_Outputs.removeLast();

	// update node
	notifyChange();
}


//!
//! Sets the parameter values.
//!
void Index2IndexNode::setParameters()
{
	Parameter *parameter = dynamic_cast<Parameter *>(sender());
	int inputIndex = parameter->getName().toInt();

	float threshold = m_threshold->getValue().toFloat();
	float inputValue = parameter->getValue().toFloat();

	if (inputValue > threshold)
	{	
		// get output parameter group
		ParameterGroup *m_OutputParameters = parameterRoot->getParameterGroup("Outputs");

		float outputVal = ((Parameter*)m_OutputParameters->getParameter("out_" + QString::number(inputIndex)))->getValue().toFloat();
		QString outputName = m_OutputParameters->getParameter("name_" + QString::number(inputIndex))->getValueString();		

		m_outIndex->setValue(QVariant(outputVal), true);		
		m_outName->setValue((QVariant(outputName)), true);

		//to reset the outputs
		bool clean = getParameter("Clean Outputs after 500 ms")->getValue().toBool();
		if (clean)
			QTimer::singleShot(500, this, SLOT(cleanOutputs()));		
	}
}

//!
//! set the "label output" and the "index output" to empty and 0, so they can be re-written with the same value
//!
void Index2IndexNode::cleanOutputs()
{
	m_outName->setValue((QVariant("")), false);
	m_outIndex->setValue(QVariant(0.0), false);	
}

//!
//! Sets the threshold to determine the visemes that are active.
//!
void Index2IndexNode::setThreshold()
{
	const int threshold = getFloatValue("Threshold");
	m_threshold->setValue(QVariant(threshold), false);
}




} // namespace Index2IndexNode 
