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
//! \file "SimpleLightNode.cpp"
//! \brief Implementation file for SimpleLightNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       17.02.2015 (last updated)
//!

#include "SimpleLightNode.h"
#include "SceneNodeParameter.h"
#include "EnumerationParameter.h"
#include "NumberParameter.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include "Log.h"

namespace SimpleLightNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(SimpleLightNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the SimpleLightNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
SimpleLightNode::SimpleLightNode ( const QString &name, ParameterGroup *parameterRoot ) :
    LightNode(name, parameterRoot)
{
    // set up parameter callback functions
    setChangeFunction("Light Type", SLOT(applyParameters()));
 	setChangeFunction("Diffuse Color", SLOT(applyParameters()));
    setChangeFunction("Specular Color", SLOT(applyParameters()));
    setChangeFunction("Power", SLOT(applyParameters()));
    setChangeFunction("Casts Shadows", SLOT(applyParameters()));
    setChangeFunction("Inner Angle", SLOT(applyParameters()));
    setChangeFunction("Outer Angle", SLOT(applyParameters()));
    setChangeFunction("Attenuation > Range", SLOT(applyParameters()));
    setChangeFunction("Attenuation > Constant", SLOT(applyParameters()));
    setChangeFunction("Attenuation > Linear", SLOT(applyParameters()));
    setChangeFunction("Attenuation > Quadratic", SLOT(applyParameters()));
	setProcessingFunction("Disable Light", SLOT(applyParameters()));

	// create an OGRE light object
    QString lightName = QString("%1Light").arg(m_name);
    Ogre::Light *light = createLight(lightName);

    // initialize the light by calling the callback functions
	applyParameters();
}


//!
//! Destructor of the SimpleLightNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
SimpleLightNode::~SimpleLightNode ()
{
}


//!
//! Applies all additional parameters of the light node.
//!
//! Is called when any of the additional parameters of the light node has been
//! changed
//!
void SimpleLightNode::applyParameters ()
{
	if (m_lightList.empty())
        return;

	Ogre::Light *light = m_lightList[0];
    EnumerationParameter *lightTypeParameter = getEnumerationParameter("Light Type");
    if (lightTypeParameter) {
		const int selectedType = lightTypeParameter->getCurrentIndex();
        switch (selectedType) {
			case 0:  light->setType(Ogre::Light::LT_POINT); break;
			case 1:  light->setType(Ogre::Light::LT_DIRECTIONAL); break;
			case 2:  light->setType(Ogre::Light::LT_SPOTLIGHT); break;
			default: light->setType(Ogre::Light::LT_POINT);
		}
    }
    QColor dColor = getColorValue("Diffuse Color");
    light->setDiffuseColour(dColor.red()/255.0f, dColor.green()/255.0f, dColor.blue()/255.0f);
   
    QColor sColor = getColorValue("Specular Color");
    light->setSpecularColour(sColor.red()/255.0f, sColor.green()/255.0f, sColor.blue()/255.0f);
  
    float power = getFloatValue("Power");
	light->setPowerScale(power);

    bool castsShadows = getBoolValue("Casts Shadows");
    light->setCastShadows(castsShadows);
    light->setVisible(castsShadows);
  
    float innerAngle = getFloatValue("Inner Angle");
    light->setSpotlightInnerAngle(Ogre::Radian(Ogre::Degree(innerAngle)));
   
    float outerAngle = getFloatValue("Outer Angle");
    light->setSpotlightOuterAngle(Ogre::Radian(Ogre::Degree(outerAngle)));

	bool disabled = getBoolValue("Disable Light");
	light->setVisible(!disabled);

    ParameterGroup* attenuationGroup = getParameterGroup("Attenuation");
    if( attenuationGroup )
    {
        float range = attenuationGroup->getParameter("Range")->getValue().toFloat();
        float factor_const     = attenuationGroup->getParameter("Constant")->getValue().toFloat();
        float factor_linear    = attenuationGroup->getParameter("Linear")->getValue().toFloat();
        float factor_quadratic = attenuationGroup->getParameter("Quadratic")->getValue().toFloat();
        
        light->setAttenuation(range, factor_const, factor_linear, factor_quadratic);
    }

    m_ogreLightContainers[0]->updateCopies();
	triggerRedraw();
}

} // namespace SimpleLightNode
