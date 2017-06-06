/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2012 Filmakademie Baden-Wuerttemberg, Institute of Animation 
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
//! \file "opencvInput.cpp"
//! \brief Implementation file for opencvInput class.
//!
//! \author     Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       29.03.2012 (last updated)
//!

///
/// Constructors and Destructors
///

#include "opencvRawInput.h"

//!
//! Constructor of the opencvInput class.
//!
//! \param name The name to give the new mesh node.
//!

opencvRawInput::opencvRawInput (QString name, ParameterGroup *parameterRoot) : CurveEditorDataNode(name, parameterRoot)
{
	m_imagePtr = 0;
	m_matrixParameter = new Frapper::GenericParameter("OpencvMatrix", 0);
	m_matrixParameter->setTypeInfo("cv::Mat");
	m_matrixParameter->setPinType(Frapper::Parameter::PT_Input);
	m_matrixParameter->setDirty(true);
	m_matrixParameter->setSelfEvaluating(false);
	parameterRoot->addParameter(m_matrixParameter);
	m_matrixParameter->setOnConnectFunction(SLOT(connectMatrixSlot()));
	m_matrixParameter->setOnDisconnectFunction(SLOT(disconnectMatrixSlot()));
}

//!
//! Destructor of the CameraInputNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
opencvRawInput::~opencvRawInput ()
{

}

///
/// Public Slots
///
//!
//! Is used if the generic Parameter is connected
//!
void opencvRawInput::connectMatrixSlot()
{
	m_matrixParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	if(m_matrixParameter->getConnectedParameter()->getDescription() == "RawMat"){
		if (m_matrixParameter) {
			
			while(!m_imagePtr){ 
				m_imagePtr = 0;
				if(m_matrixParameter->getConnectedParameter()->isDirty()){
					const QString sourceName = m_matrixParameter->getName();
					QVariant var = m_matrixParameter->getValue(true);
					if(!var.isValid())
						continue;
					m_imagePtr = var.value<cv::Mat*>();	
				}
			}
			m_matrixParameter->setSelfEvaluating(true);
			m_width = m_imagePtr->cols;
			m_height = m_imagePtr->rows;
			m_videoSize = m_width*m_height*m_imagePtr->channels();
			//do Stuff on connection
			pastConnectToMatrix();
			setProcessingFunction(m_matrixParameter->getName(), SLOT(processInputMatrix()));
			Frapper::Log::info("Matrix connected");
		}
	} else {
		m_matrixParameter->setSelfEvaluating(false);
		m_matrixParameter->setDirty(true);
		deleteConnection(m_matrixParameter->getConnectionMap().begin().value());
		Frapper::Log::error("This node needs OpencvRawMatrix as Input. Please check the connected Parameter");
	}
}

//!
//! Is used if the generic Parameter is disconnected
//!
void opencvRawInput::disconnectMatrixSlot()
{
	m_imagePtr = 0;
	m_matrixParameter->setDirty(true);
	m_matrixParameter->setSelfEvaluating(false);
	Frapper::Log::info("Matrix disconnected");
}


///
/// Protected Slots
///

//!
//! SLOT that is used on the matrixParameter
//!
void opencvRawInput::processInputMatrix()
{
	//doStuff
}


///
/// Private Method
///
void opencvRawInput::pastConnectToMatrix()
{
	//do stuff after connection hast established
}