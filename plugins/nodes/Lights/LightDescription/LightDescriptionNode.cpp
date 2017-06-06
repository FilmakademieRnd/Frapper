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
//! \file "LightDescriptionNode.cpp"
//! \brief Implementation file for LightDescriptionNode class.
//!
//! \author     Simon Spielmann simon.spielmann@filmakademie.de>
//! \author     Markus Rapp <markus.rapp@filmakademie.de>
//! \version    0.2
//! \date       27.05.2015 (last updated)
//!

#include "LightDescriptionNode.h"
#include "OgreTools.h"

#include <QtCore/QFile>

namespace LightDescriptionNode {
using namespace Frapper;



///
/// Constructors and Destructors
///


//!
//! Constructor of the LightDescriptionNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
LightDescriptionNode::LightDescriptionNode ( const QString &name, ParameterGroup *parameterRoot ) :
	LightNode(name, parameterRoot)
{
	// set up parameter callback functions
	setCommandFunction("Light Description File", SLOT(loadLightDescriptionFileHair()));
	setChangeFunction("Light Description File", SLOT(loadLightDescriptionFileHair()));
	setProcessingFunction("Disable Light", SLOT(switchLights()));
	setChangeFunction("Light Power", SLOT(applyParameters()));
	setChangeFunction("Set Number of Lights", SLOT(applyParameters()));
}


//!
//! Destructor of the LightDescriptionNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
LightDescriptionNode::~LightDescriptionNode ()
{
}


///
/// Public Functions
///


///
/// Private Functions
///


///
/// Private Slots
///


//!
//! Loads an XML reference data file.
//!
//! Is called when the value of the Reference Data File parameter has
//! changed.
//!
//! \return True if the reference data was successfully loaded from file, otherwise False.
//!
bool LightDescriptionNode::loadLightDescriptionFileHair ()
{
	bool result = false;
	QString path = getStringValue("Light Description File");
	QFile inFile(path);

	// parse the file and fill the pos/pow pair into a list
	if (inFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_lightDataList.clear();

		QPair<Ogre::Vector3, Ogre::Vector3> newData(Ogre::Vector3(0,0,0), Ogre::Vector3(0,0,0));
		while (!inFile.atEnd())
		{
			QString line = inFile.readLine();
			line.replace("  ", " "); // ensure that no double white spaces exist
			QString prefix = line.section(":", 0, 0);
			if ((prefix == "Dir") || (prefix == "Direction")) {
				Ogre::Vector3 pos(
					line.section(" ", 1, 1).toFloat(),
					line.section(" ", 2, 2).toFloat(),
					line.section(" ", 3, 3).toFloat() );
				newData.first = pos;
			}
			else if ((prefix == "Pow") || (prefix == "Color")) {
				Ogre::Vector3 pow(
					line.section(" ", 1, 1).toFloat(),
					line.section(" ", 2, 2).toFloat(),
					line.section(" ", 3, 3).toFloat() );
				newData.second = pow;
			}
			if (newData.first.length() != 0 && newData.second.length() != 0) {
				m_lightDataList.append(newData);
				newData = QPair<Ogre::Vector3, Ogre::Vector3>(Ogre::Vector3(0,0,0), Ogre::Vector3(0,0,0));
			}
		}

		// sort the list: greatest pow first
		qSort(m_lightDataList.begin(), m_lightDataList.end(), greaterThan);

		// get number of light and show it in editor
		setValue("Number of Lights", QString("%L1").arg(m_lightDataList.length()), true);

		// set the max value for the number of light slider
		getNumberParameter("Set Number of Lights")->setMaxValue(m_lightDataList.length());
		setValue("Set Number of Lights", m_lightDataList.length(), true);
		forcePanelUpdate();
		createLights();
		applyParameters();
	}
	else
	{
		Log::error(QString("Document import failed. \"%1\" not found.").arg(path), "AnimatableMeshHairNode::loadLightDescriptionFileHair");
	}
	inFile.close();

	return result;
}


//!
//! Creates the Lights in the scene
//!
void LightDescriptionNode::createLights()
{
	if (m_lightDataList.empty())
		return;

	const int nbrLights = getIntValue("Set Number of Lights");

	// destroy old lights
	destroyAllLights();

	// iterate through directional light list and create all directional lights
	unsigned int count = 0;
	const float max = m_lightDataList.first().second.length();
	for (QList<QPair<Ogre::Vector3, Ogre::Vector3>>::iterator iter = m_lightDataList.begin(); iter != m_lightDataList.end(); ++iter, ++count) {
		const Ogre::Vector3 lightDirection = iter->first;
		const Ogre::Vector3 lightPower = iter->second / max;
		const float length = lightPower.length();
		Ogre::Light *light = createLight(m_name + "_Light" + QString::number(count));
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(-lightDirection);
		light->setDiffuseColour(lightPower.x, lightPower.y, lightPower.z);
		light->setSpecularColour(lightPower.x, lightPower.y, lightPower.z);

		if (count == nbrLights-1) 
			break;
	}

	getParameter(m_outputLightName)->propagateDirty();
}


//!
//! Switch for all lights.
//!
void LightDescriptionNode::switchLights()
{
	const bool disableLight = getBoolValue("Disable Light");
	
	foreach(Ogre::Light *light, m_lightList)
		light->setVisible(!disableLight);

	foreach(OgreContainer *container, m_ogreLightContainers)
		container->updateCopies();
	
	triggerRedraw();
}


//!
//! Applies parameter changes to all lights.
//!
void LightDescriptionNode::applyParameters()
{
	const float lightScale = getFloatValue("Light Power");
	const int nbrLights = getIntValue("Set Number of Lights");

	unsigned int count = 0;
	foreach(Ogre::Light *light, m_lightList) {
		light->setPowerScale(lightScale);
		light->setVisible(true);
		if (count >= nbrLights) 
				light->setVisible(false);
		count++;
	}
	
	foreach(OgreContainer *container, m_ogreLightContainers)
		container->updateCopies();
	
	triggerRedraw();
}


//!
//! The greater than function for light power sorting
//!
//! \param d1 First input pair
//! \param d2 Second input pair
//!
//! \return Result if d1 is greater than d2
//!
bool LightDescriptionNode::greaterThan(const QPair<Ogre::Vector3, Ogre::Vector3> &d1, const QPair<Ogre::Vector3, Ogre::Vector3> &d2)
{
	return d1.second.length() > d2.second.length();
}

} // namespace LightDescriptionNode
