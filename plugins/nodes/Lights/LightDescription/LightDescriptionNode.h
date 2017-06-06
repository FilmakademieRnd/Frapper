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
//! \file "LightDescriptionNode.h"
//! \brief Header file for LightDescriptionNode class.
//!
//! \author     Simon Spielmann simon.spielmann@filmakademie.de>
//! \author     Markus Rapp <markus.rapp@filmakademie.de>
//! \version    0.2
//! \date       27.05.2015 (last updated)
//!

#ifndef LIGHTDESCRIPTIONNODE_H
#define LIGHTDESCRIPTIONNODE_H

#include "LightNode.h"


namespace LightDescriptionNode {
using namespace Frapper;

//!
//! Class representing nodes for multiple lights, which are based on a light description txt file.
//!
class LightDescriptionNode : public LightNode
{

	Q_OBJECT
	ADD_INSTANCE_COUNTER

public: // constructors and destructors

	//!
	//! Constructor of the LightNode class.
	//!
	//! \param name The name for the new node.
	//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
	//!
	LightDescriptionNode ( const QString &name, ParameterGroup *parameterRoot );

	//!
	//! Destructor of the LightNode class.
	//!
	//! Defined virtual to guarantee that the destructor of a derived class
	//! will be called if the instance of the derived class is saved in a
	//! variable of its parent class type.
	//!
	virtual ~LightDescriptionNode ();

public: // functions


private slots: //

	//!
	//! Loads an XML reference data file for hairs and geometry.
	//!
	//! Is called when the value of the Reference Data File parameter has
	//! changed.
	//!
	//! \return True if the reference data was successfully loaded from file, otherwise False.
	//!
	bool loadLightDescriptionFileHair();

	//!
	//! Creates the Lights in the scene.
	//!
	void createLights();

	//!
	//! Applies parameter changes to all lights.
	//!
	void applyParameters();

	//!
	//! Switch for all lights.
	//!
	void switchLights();


private: // data

	//!
	//! The greater than function for light power sorting
	//!
	//! \param d1 First input pair
	//! \param d2 Second input pair
	//!
	//! \return Result if d1 is greater than d2
	//!
	static bool greaterThan(const QPair<Ogre::Vector3, Ogre::Vector3> &d1, const QPair<Ogre::Vector3, Ogre::Vector3> &d2);

	//!
	//! The list containing the lights data
	//!
	QList<QPair<Ogre::Vector3, Ogre::Vector3>> m_lightDataList;
};

} // namespace LightNode

#endif
