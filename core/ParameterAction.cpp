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
//! \file "ParameterAction.cpp"
//! \brief Implementation file for ParameterAction class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    0.2
//! \date       27.11.2008 (last updated)
//!

#include "ParameterAction.h"
#include "ParameterTypeIcon.h"

namespace Frapper {

INIT_INSTANCE_COUNTER(ParameterAction)

///
/// Constructors and Destructors
///


//!
//! Constructor of the ParameterAction class.
//!
//! \param parameter The parameter the action represents.
//! \param parent The object that the action will be a child of.
//!
ParameterAction::ParameterAction ( Parameter *parameter, QObject *parent /* = 0 */ )
: QAction(parameter->getName(), parent)
{
    m_parameter = parameter;
    setIcon(ParameterTypeIcon::getIcon(Parameter::getTypeName(parameter->getType())));

	QString pinInfo = parameter->getName();
	pinInfo += "\nType: " + Parameter::getTypeName(parameter->getType());
	if (parameter->getMultiplicity() == Parameter::M_OneOrMore)
		pinInfo += " List";
	QString parameterDescription = parameter->getDescription();
	if (!parameterDescription.isEmpty())
		pinInfo += "\nDescription: " + parameterDescription;
	setToolTip(pinInfo);

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the ParameterAction class.
//!
ParameterAction::~ParameterAction(void)
{
    DEC_INSTANCE_COUNTER
}


///
/// Public Functions
///


//!
//! Returns the parameter the action represents.
//!
Parameter * ParameterAction::getParameter () const
{
    return m_parameter;
}

} // end namespace Frapper