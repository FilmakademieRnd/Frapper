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
//! \file "MotionDataNode.cpp"
//! \brief Implementation file for MotionDataNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       03.07.2009 (last updated)
//!

#include "MotionDataNode.h"


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the MotionDataNode class.
//!
//! \param name The name for the new node.
//!
MotionDataNode::MotionDataNode ( const QString &name, ParameterGroup *parameterRoot ) :
CurveEditorDataNode(name, parameterRoot),
m_maxNumberParameters(0),
m_maxNumberKeys(0),
m_databaseGroup(new ParameterGroup("DataBase")),
m_infoGroup(0),
m_dataGroup(0)
{
    Parameter *outputDataParameter = Parameter::createGroupParameter("Out_DataBase", m_databaseGroup);
    outputDataParameter->setPinType(Parameter::PT_Output);
    outputDataParameter->setDirty(true);
    parameterRoot->addParameter(outputDataParameter);
    parameterRoot->addParameter(m_databaseGroup);

    Parameter *disabledParameters = getParameter("DisabledParameters");
    if (!disabledParameters) {
        disabledParameters = new Parameter("DisabledParameters", Parameter::T_String, "");
        disabledParameters->setVisible(false);
        parameterRoot->addParameter(disabledParameters);
    }
}


//!
//! Destructor of the MotionDataNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MotionDataNode::~MotionDataNode ()
{
    // the m_databaseGroup does not have to be destroyed here, as it is contained
    // in the node's parameter tree, which is destroyed in Node::~Node
}


///
/// Public Functions
///


//!
//! Returns the maximum number of parameters in the motion data set.
//!
//! \return The maximum number of parameters in the motion data set.
//!
inline unsigned int MotionDataNode::getMaxNumberParameters () const
{
    return m_maxNumberParameters;
}

//!
//! The maximum number of parameters contained in the motion data set.
//!
unsigned int MotionDataNode::getMaxNumberKeys () const
{
    return m_maxNumberKeys;
}


///
/// Protected Functions
///


//!
//! Clears all data from the motion data database.
//!
void MotionDataNode::clearDatabase ()
{
    ParameterGroup *rootGroup = getParameterRoot();

    if (m_dataGroup && m_databaseGroup->containsGroup(m_dataGroup->getName())) {
        m_databaseGroup->removeParameterGroup(m_dataGroup->getName());
        m_dataGroup = 0;
    }

    if (m_infoGroup && m_databaseGroup->containsGroup(m_infoGroup->getName())) {
        m_databaseGroup->removeParameterGroup(m_infoGroup->getName());
        m_infoGroup = 0;
    }

    // create the database parameter group as a child of the parameter root
    m_dataGroup = new ParameterGroup("Data");
    m_infoGroup = new ParameterGroup("Info");

    m_databaseGroup->addParameter(m_dataGroup);
    m_databaseGroup->addParameter(m_infoGroup);

    setCurveEditorGoup(m_dataGroup);

    notifyChange();
}

//!
//! Disables paramters in Database based on the DisabledParameters enumeration.
//!
void MotionDataNode::disableParameters ()
{
    Parameter *disabledParameters = getParameter("DisabledParameters");
    const QStringList &disabledParametersList = disabledParameters->getValueString().split(",", QString::SkipEmptyParts);
    if (disabledParametersList.empty())
        return;
    foreach (QString name, disabledParametersList) {
        AbstractParameter *parameter = m_dataGroup->getParameterGroup(name);
        if (parameter)
            parameter->setEnabled(false);
        else {
            parameter = m_dataGroup->getParameter(name);
            if (parameter)
                parameter->setEnabled(false);
            else
                Log::warning(QString("AbstractParameter %1 not found in database").arg(name), "MotionDataNode::updateDisabledLists");
        }
    }
    forcePanelUpdate();
}

//!
//! Updates a list containing the disabled parameter names 
//!
void MotionDataNode::updateDisabledParameters ()
{
    Parameter *disabledParameters = getParameter("DisabledParameters");
    disabledParameters->getValueString().clear();
    QStringList disabledParametersList; 

	if (m_dataGroup) {
		const AbstractParameter::List& mk_list = m_dataGroup->getParameterList();
		if (!mk_list.empty()) {
			foreach (AbstractParameter *mk, mk_list) {
				const AbstractParameter::List& kg_list = (static_cast<ParameterGroup *>(mk))->getParameterList();
				updateDisabledList(mk, disabledParametersList);
				foreach (AbstractParameter *kg, kg_list) {
					updateDisabledList(kg, disabledParametersList);
					if (kg->isGroup()) {
						const AbstractParameter::List& ky_list = (static_cast<ParameterGroup *>(kg))->getParameterList();
						foreach (AbstractParameter *ky, ky_list) {
							updateDisabledList(ky, disabledParametersList);
						}
					}
					else
						updateDisabledList(kg, disabledParametersList);
				}
			}
			disabledParameters->setValue(disabledParametersList.join(","));
		}
	}
}

//!
//! Check list containing the disabled parameters and add or remove the given
//!
inline void MotionDataNode::updateDisabledList (const AbstractParameter *parameter, QStringList &list)
{
    const QString parameterName = parameter->getName();
    if (!parameter->isEnabled())
        list.append(parameterName);
    else if (parameter->isEnabled() && list.contains(parameterName))
        list.removeOne(parameterName);
    else
        return;
}

} // end namespace Frapper