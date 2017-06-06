/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "RazerHydraNode.h"
//! \brief Header file for RazerHydraNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       14.06.2013 (last updated)
//!

#ifndef RAZERHYDRANODE_H
#define RAZERHYDRANODE_H

#include "ImageNode.h"

#include "sixense.h"
#include "sixense_math.hpp"
#include <sixense_utils/derivatives.hpp>
#include <sixense_utils/button_states.hpp>
#include <sixense_utils/event_triggers.hpp>
#include <sixense_utils/controller_manager/controller_manager.hpp>

#include <QMap>

// Forward Declarations
namespace Frapper {
	class Parameter;
	class NumberParameter;
}

namespace RazerHydraNode {
using namespace Frapper;

enum RazerHydraButtons {
	BUTTON_1          = 0,
	BUTTON_2          = 1,
	BUTTON_3          = 2,
	BUTTON_4          = 3,
	BUTTON_START      = 4,
	BUTTON_JOYSTICK   = 5,
	BUTTON_BUMPER     = 6,
	NUM_BUTTONS		  = 7
};

enum RazerHydraJoystickPositions {
	JOYSTICK_UP    = 0,
	JOYSTICK_DOWN  = 1,
	JOYSTICK_LEFT  = 2,
	JOYSTICK_RIGHT = 3,
	NUM_JOYSTICK_POSITIONS
};

typedef enum {
	AXIS_X = 0,
	AXIS_Y,
	AXIS_Z,
	AXIS_MINUS_X,
	AXIS_MINUS_Y,
	AXIS_MINUS_Z
} RazerHydraAxisMapping;

enum RazerHydraRotationAxis {
	AXIS_YAW = 0,
	AXIS_PITCH,
	AXIS_ROLL,
	NUM_ROTATION_AXIS
};


//!
//! Class representing nodes that can utilize the Razer Hydra
//!
class RazerHydraNode : public ImageNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the RazerHydraNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    RazerHydraNode ( QString name, ParameterGroup *parameterRoot );

	void getParameterReferences();

    //!
    //! Destructor of the RazerHydraNode class.
    //!
    ~RazerHydraNode ();

	//!
	//! Callback for ControllerManager to set initial position of controllers
	//!
	static void setup_callback ( sixenseUtils::ControllerManager::setup_step step );

public: // functions

    virtual void readHydraData ();

public slots: //

    //!
    //! Starts or stops the thread for running the Wii tracker.
    //!
    //! Is called when the value of the run parameter has changed.
    //!
    void runChanged ();

    //!
    //! Is triggered of timer event.
    //!
    void timerEvent();

	void timerIntervalChanged();

	void processOutputImage();

	void nodeOptionsChanged();

	void filteringOptionsChanged();
	
	void axisMappingChanged();

	void setInitialOrientation();

private: // functions

	void loadRazerImage();

	void checkButtonsPressed( sixenseControllerData* controllerData, Parameter* buttonParameters[] );
	void updateJoystickPosition( sixenseControllerData* controllerData, Parameter* JoystickPositionsDigital[], NumberParameter* JoystickPositions[] );
	void updateBone( sixenseControllerData* controllerData, RazerHydraAxisMapping axisMapping[], NumberParameter* boneParameter, Ogre::Vector3 offsetOrientation = Ogre::Vector3(0,0,0) );
	void updateBoneQuat( sixenseControllerData* controllerData, RazerHydraAxisMapping axisMapping[], NumberParameter* boneParameter, Ogre::Quaternion offsetOrientation, Ogre::Vector3 offsetPosition );
	void updateTrigger( sixenseControllerData* controllerData, NumberParameter* triggerParameter );

	void calculateAxisMapping( RazerHydraAxisMapping * axisMapping, const Ogre::Vector3& eulerIn, Ogre::Vector3& eulerOut );


private: // data

	static bool m_controller_manager_setup;
	static QString m_setupTextureFilename;

	Ogre::TexturePtr m_razerTexture;
	
	// map controller id + button id to frapper button output parameter
	Parameter*        m_leftHandButtonParameters[NUM_BUTTONS];
	Parameter*        m_leftHandJoystickParameters[NUM_JOYSTICK_POSITIONS];
	NumberParameter*  m_leftHandJoystickPositionParameters[NUM_JOYSTICK_POSITIONS];
	NumberParameter*  m_leftHandTriggerParameter;
	NumberParameter*  m_leftHandBoneParameter;
	NumberParameter*  m_leftHandBoneQuatParameter;


	Parameter*        m_rightHandButtonParameters[NUM_BUTTONS];
	Parameter*        m_rightHandJoystickParameters[NUM_JOYSTICK_POSITIONS];
	NumberParameter*  m_rightHandJoystickPositionParameters[NUM_JOYSTICK_POSITIONS];
	NumberParameter*  m_rightHandTriggerParameter;
	NumberParameter*  m_rightHandBoneParameter;
	NumberParameter*  m_rightHandBoneQuatParameter;

	RazerHydraAxisMapping m_leftHandAxisMapping[NUM_ROTATION_AXIS];
	RazerHydraAxisMapping m_leftHandaxisMapping[NUM_ROTATION_AXIS];

	bool m_outputBonePosition;

	bool m_triggerDirty;

	float m_valueScale;

	float m_positionScale;

	QTimer* m_timer;

	Ogre::Vector3 m_leftHandInitialOrientationEuler;
	Ogre::Vector3 m_rightHandInitialOrientationEuler;

	Ogre::Vector3 m_leftHandInitialPosition;
	Ogre::Vector3 m_rightHandInitialPosition;

	Ogre::Quaternion m_leftHandInitialOrientationQuat;
	Ogre::Quaternion m_rightHandInitialOrientationQuat;

	float m_deadZone;
};

} // namespace RazerHydraNode

#endif
