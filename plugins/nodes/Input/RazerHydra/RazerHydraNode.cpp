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
//! \file "RazerHydraNode.cpp"
//! \brief Implementation file for RazerHydraNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       14.06.2013 (last updated)
//!

#include "RazerHydraNode.h"

#include <QString>
#include <OgreImage.h>
#include <QFile>
#include <OgreMatrix3.h>
#include <OgreVector3.h>
#include <QVariantList>
#include <QTimer>

#include <sixense_math.hpp>

namespace RazerHydraNode {
using namespace Frapper;

///
/// Static member initialization
///

bool RazerHydraNode::m_controller_manager_setup = true;
QString RazerHydraNode::m_setupTextureFilename = "";

///
/// Constructors and Destructors
///


//!
//! Constructor of the RazerHydraNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
RazerHydraNode::RazerHydraNode ( QString name, ParameterGroup *parameterRoot ) :
	ImageNode(name, parameterRoot ),
		m_leftHandInitialOrientationEuler( Ogre::Vector3::ZERO),
		m_rightHandInitialOrientationEuler( Ogre::Vector3::ZERO),
		m_leftHandInitialOrientationQuat( Ogre::Quaternion::IDENTITY),
		m_rightHandInitialOrientationQuat( Ogre::Quaternion::IDENTITY),
		m_leftHandInitialPosition( Ogre::Vector3::ZERO),
		m_rightHandInitialPosition( Ogre::Vector3::ZERO)
{
	// create and setup timer
	m_timer = new QTimer();
	setChangeFunction("Timer Interval (ms)", SLOT(timerIntervalChanged()));
	connect( m_timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
	
	setChangeFunction("Bone Position", SLOT(nodeOptionsChanged()));
	setChangeFunction("Trigger Dirty", SLOT(nodeOptionsChanged()));
	setChangeFunction("Value Scale", SLOT(nodeOptionsChanged()));
	setChangeFunction("Position Scale", SLOT(nodeOptionsChanged()));
	setChangeFunction("Joystick Deadzone", SLOT(nodeOptionsChanged()));

	setChangeFunction("Enable Filtering",       SLOT(filteringOptionsChanged()));
	setChangeFunction("Filtering > Near Range", SLOT(filteringOptionsChanged()));
	setChangeFunction("Filtering > Far Range",  SLOT(filteringOptionsChanged()));
	setChangeFunction("Filtering > Near Value", SLOT(filteringOptionsChanged()));
	setChangeFunction("Filtering > Far Value",  SLOT(filteringOptionsChanged()));

	setChangeFunction("Rotation Mapping Left > Yaw Left",   SLOT(axisMappingChanged()));
	setChangeFunction("Rotation Mapping Left > Pitch Left", SLOT(axisMappingChanged()));
	setChangeFunction("Rotation Mapping Left > Roll Left",  SLOT(axisMappingChanged()));

	setChangeFunction("Rotation Mapping Right > Yaw Right",   SLOT(axisMappingChanged()));
	setChangeFunction("Rotation Mapping Right > Pitch Right", SLOT(axisMappingChanged()));
	setChangeFunction("Rotation Mapping Right > Roll Right",  SLOT(axisMappingChanged()));

	//setChangeFunction("Hydra Base Color", SLOT(nodeOptionChanged()));
	setProcessingFunction("Set Initial Orientation", SLOT(setInitialOrientation()));

	loadRazerImage();

	// update is triggered by viewport
	setProcessingFunction( m_outputImageName, SLOT(processOutputImage()));
	setValue(m_outputImageName, m_razerTexture);

	Log::debug("Initialize Sixense API", "RazerHydraNode::RazerHydraNode");
	int result = sixenseInit();
	if( result != SIXENSE_SUCCESS )
		Log::error("Sixense API could not be initialized", "RazerHydraNode::RazerHydraNode");

	// Init the controller manager. This makes sure the controllers are present, assigned to left and right hands, and that
	// the hemisphere calibration is complete.
	sixenseUtils::getTheControllerManager()->setGameType( sixenseUtils::ControllerManager::ONE_PLAYER_TWO_CONTROLLER );
	sixenseUtils::getTheControllerManager()->registerSetupCallback( RazerHydraNode::setup_callback );

	// set affections and callback functions
	setChangeFunction("Run", SLOT(runChanged()));

	// store references to output parameters for faster access
	getParameterReferences();

	// init timer interval
	timerIntervalChanged();

	// init bone options
	nodeOptionsChanged();

	// init rotation axis mapping
	axisMappingChanged();
}


//!
//! Destructor of the RazerHydraNode class.
//!
RazerHydraNode::~RazerHydraNode ()
{
	// request the thread to be stopped
	setValue("Run", false);

	m_timer->stop();
	
	Log::debug("Deinitialize Sixense API", "RazerHydraNode::RazerHydraNode");
	sixenseExit();
}


///
/// Public Functions
///

//!
//! Callback for ControllerManager to set initial position of controllers
//!
void RazerHydraNode::setup_callback ( sixenseUtils::ControllerManager::setup_step step )
{
	if( sixenseUtils::getTheControllerManager()->isMenuVisible() ) {

		// Turn on the flag that tells the graphics system to draw the instruction screen instead of the controller information.
		m_controller_manager_setup = true;

		// Ask the controller manager what the next instruction string should be.
		QString controller_manager_text_string( sixenseUtils::getTheControllerManager()->getStepString());

		Frapper::Log::info(controller_manager_text_string, "RazerHydraNode::setup_callback");

		// We could also load the supplied controllermanager textures using the filename: sixenseUtils::getTheControllerManager()->getTextureFileName();
		m_setupTextureFilename = QString(sixenseUtils::getTheControllerManager()->getTextureFileName());

	} else {

		// We're done with the setup, so hide the instruction screen.
		m_controller_manager_setup = false;
	}


}
///
/// Public Slots
///

//!
//! Starts or stops the thread for running the Wii tracker.
//!
//! Is called when the value of the run parameter has changed.
//!
void RazerHydraNode::runChanged ()
{
	if (getBoolValue("Run")) 
	{
		m_timer->start();
	} 
	else 
	{
		m_timer->stop();
	}
}

void RazerHydraNode::timerIntervalChanged()
{
	int interval = getValue("Timer Interval (ms)").toInt();
	m_timer->setInterval(interval);
}

void RazerHydraNode::timerEvent()
{
	readHydraData();
}


void RazerHydraNode::nodeOptionsChanged()
{
	m_outputBonePosition = getBoolValue("Bone Position");
	m_triggerDirty       = getBoolValue("Trigger Dirty");
	m_deadZone			 = getFloatValue("Joystick Deadzone");
	
	float newScale = getFloatValue("Value Scale");
	if( newScale > m_valueScale || newScale < m_valueScale )
	{
		m_valueScale = newScale;
		for( int i=0; i<NUM_JOYSTICK_POSITIONS; i++) {
			m_leftHandJoystickPositionParameters[i] ->setMaxValue( QVariant(m_valueScale));
			m_rightHandJoystickPositionParameters[i]->setMaxValue( QVariant(m_valueScale));
		}
		m_leftHandTriggerParameter              ->setMaxValue( QVariant(m_valueScale));
		m_rightHandTriggerParameter             ->setMaxValue( QVariant(m_valueScale));
		m_leftHandBoneParameter                 ->setMaxValue( QVariant(m_valueScale));
		m_rightHandBoneParameter                ->setMaxValue( QVariant(m_valueScale));
	}

	m_positionScale = getFloatValue("Position Scale");

	// not supported by razer hydra
	//QColor color = getColorValue("Hydra Base Color");
	//sixenseSetBaseColor( color.red(), color.green(), color.blue() );

}

// Filtering
void RazerHydraNode::filteringOptionsChanged()
{
	bool filteringEnabled = getBoolValue("Enable Filtering");
	sixenseSetFilterEnabled(filteringEnabled);
	getParameterGroup("Filtering")->setEnabled(filteringEnabled);

	if( filteringEnabled ) {
		float near_range = getFloatValue("Filtering > Near Range");
		float far_range  = getFloatValue("Filtering > Far Range");
		float near_val   = getFloatValue("Filtering > Near Value");
		float far_val    = getFloatValue("Filtering > Far Value");
		sixenseSetFilterParams(near_range, near_val, far_range, far_val);
		//sixenseGetFilterParams(&near_range, &near_val, &far_range, &far_val);
		//Frapper::Log::debug( QString("NV:%1, FV:%2, NR:%3, FR:%4").arg(near_value).arg(far_val).arg(near_range).arg(far_range));
	}
}

//!
//! Main function for vision handling for the virtual agent.
//! Will be run in an own thread.
//!
void RazerHydraNode::readHydraData ()
{
	sixenseSetActiveBase(0);
	sixenseAllControllerData controllerData;
	sixenseGetAllNewestData(&controllerData);
	sixenseUtils::getTheControllerManager()->update(&controllerData);

	// Ask the controller manager which controller is in the left hand and which is in the right
	int left_controller  = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1L );
	int right_controller = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1R );

	// don't update output parameter while in setup mode
	if( m_controller_manager_setup )
		return;

	checkButtonsPressed( &controllerData.controllers[left_controller],  m_leftHandButtonParameters);
	checkButtonsPressed( &controllerData.controllers[right_controller], m_rightHandButtonParameters);

	updateJoystickPosition( &controllerData.controllers[left_controller],  m_leftHandJoystickParameters,  m_leftHandJoystickPositionParameters );
	updateJoystickPosition( &controllerData.controllers[right_controller], m_rightHandJoystickParameters, m_rightHandJoystickPositionParameters );

	updateTrigger( &controllerData.controllers[left_controller],  m_leftHandTriggerParameter );
	updateTrigger( &controllerData.controllers[right_controller],  m_rightHandTriggerParameter );

	updateBone( &controllerData.controllers[left_controller],  m_leftHandAxisMapping,  m_leftHandBoneParameter, m_leftHandInitialOrientationEuler );
	updateBone( &controllerData.controllers[right_controller], m_leftHandaxisMapping, m_rightHandBoneParameter, m_rightHandInitialOrientationEuler );

	updateBoneQuat( &controllerData.controllers[left_controller],  m_leftHandAxisMapping,  m_leftHandBoneQuatParameter, m_leftHandInitialOrientationQuat, m_leftHandInitialPosition );
	updateBoneQuat( &controllerData.controllers[right_controller], m_leftHandaxisMapping, m_rightHandBoneQuatParameter, m_rightHandInitialOrientationQuat, m_leftHandInitialPosition );

}


void RazerHydraNode::checkButtonsPressed( sixenseControllerData* controllerData, Parameter* buttonParameters[] )
{
	buttonParameters[BUTTON_1]        ->setValue( QVariant((bool)(controllerData->buttons & SIXENSE_BUTTON_1)),        m_triggerDirty );
	buttonParameters[BUTTON_2]        ->setValue( QVariant((bool)(controllerData->buttons & SIXENSE_BUTTON_2)),        m_triggerDirty );
	buttonParameters[BUTTON_3]        ->setValue( QVariant((bool)(controllerData->buttons & SIXENSE_BUTTON_3)),        m_triggerDirty );
	buttonParameters[BUTTON_4]        ->setValue( QVariant((bool)(controllerData->buttons & SIXENSE_BUTTON_4)),        m_triggerDirty );
	buttonParameters[BUTTON_START]    ->setValue( QVariant((bool)(controllerData->buttons & SIXENSE_BUTTON_START)),    m_triggerDirty );
	buttonParameters[BUTTON_JOYSTICK] ->setValue( QVariant((bool)(controllerData->buttons & SIXENSE_BUTTON_JOYSTICK)), m_triggerDirty );
	buttonParameters[BUTTON_BUMPER]   ->setValue( QVariant((bool)(controllerData->buttons & SIXENSE_BUTTON_BUMPER)),   m_triggerDirty );
}

void RazerHydraNode::updateJoystickPosition( sixenseControllerData* controllerData, Parameter* JoystickPositionsDigital[], NumberParameter* JoystickPositions[] )
{
	float x = controllerData->joystick_x;
	float y = controllerData->joystick_y;

	if( x < 0)
	{
		JoystickPositions[JOYSTICK_LEFT] ->setValue( QVariant(-1.0f * m_valueScale * x), m_triggerDirty );
		JoystickPositions[JOYSTICK_RIGHT]->setValue( QVariant( 0.0f),                    m_triggerDirty );

		if( x < -m_deadZone ) // Dead Zone
		{
			JoystickPositionsDigital[JOYSTICK_LEFT] ->setValue( QVariant( true ),        m_triggerDirty );
			JoystickPositionsDigital[JOYSTICK_RIGHT]->setValue( QVariant( false ),       m_triggerDirty );
		}
	}
	else if( x > 0 )
	{
		JoystickPositions[JOYSTICK_LEFT] ->setValue( QVariant( 0.0f),                    m_triggerDirty );
		JoystickPositions[JOYSTICK_RIGHT]->setValue( QVariant( m_valueScale * x),        m_triggerDirty );

		if( x > m_deadZone ) // Dead Zone
		{
			JoystickPositionsDigital[JOYSTICK_LEFT] ->setValue( QVariant( false ),       m_triggerDirty );
			JoystickPositionsDigital[JOYSTICK_RIGHT]->setValue( QVariant( true ),        m_triggerDirty );
		}
	}
	else
	{
		JoystickPositions[JOYSTICK_LEFT] ->setValue( QVariant( 0.0f),                    m_triggerDirty );
		JoystickPositions[JOYSTICK_RIGHT]->setValue( QVariant( 0.0f),                    m_triggerDirty );

		JoystickPositionsDigital[JOYSTICK_LEFT] ->setValue( QVariant( false ),           m_triggerDirty );
		JoystickPositionsDigital[JOYSTICK_RIGHT]->setValue( QVariant( false ),           m_triggerDirty );
	}

	if( y < 0)
	{
		JoystickPositions[JOYSTICK_DOWN]->setValue( QVariant(-1.0f * m_valueScale * y),  m_triggerDirty );
		JoystickPositions[JOYSTICK_UP]  ->setValue( QVariant( 0.0f),                     m_triggerDirty );

		if( y < -m_deadZone ) // Dead Zone
		{
			JoystickPositionsDigital[JOYSTICK_DOWN]->setValue( QVariant( true ),         m_triggerDirty );
			JoystickPositionsDigital[JOYSTICK_UP]  ->setValue( QVariant( false ),        m_triggerDirty );
		}
	}
	else if( y > m_deadZone )
	{
		JoystickPositions[JOYSTICK_DOWN]->setValue( QVariant(0.0f),                      m_triggerDirty );
		JoystickPositions[JOYSTICK_UP]  ->setValue( QVariant(m_valueScale * y),          m_triggerDirty );

		if( y > 0.05 ) // Dead Zone
		{
			JoystickPositionsDigital[JOYSTICK_DOWN]->setValue( QVariant( false ),        m_triggerDirty );
			JoystickPositionsDigital[JOYSTICK_UP]  ->setValue( QVariant( true ),         m_triggerDirty );
		}
	}
	else
	{
		JoystickPositions[JOYSTICK_DOWN]->setValue( QVariant(0.0f),                      m_triggerDirty );
		JoystickPositions[JOYSTICK_UP]  ->setValue( QVariant(0.0f),                      m_triggerDirty );

		JoystickPositionsDigital[JOYSTICK_DOWN]->setValue( QVariant( false ),            m_triggerDirty );
		JoystickPositionsDigital[JOYSTICK_UP]  ->setValue( QVariant( false ),            m_triggerDirty );
	}
}

void RazerHydraNode::updateBone( sixenseControllerData* controllerData, RazerHydraAxisMapping axisMapping[], NumberParameter* boneParameter, Ogre::Vector3 offsetOrientation /*= Ogre::Vector3(0,0,0) */ )
{
	// orientation
	Ogre::Matrix3 rot_mat( controllerData->rot_mat );

	Ogre::Radian rx, ry, rz;
	rot_mat.ToEulerAnglesXYZ(rx, ry, rz);

	Ogre::Vector3 rot_vec ( rx.valueRadians(), ry.valueRadians(), rz.valueRadians());
	Ogre::Vector3 rot_vec_map = Ogre::Vector3::ZERO;
	calculateAxisMapping( axisMapping, rot_vec, rot_vec_map );

	Ogre::Vector3 rot = rot_vec_map - offsetOrientation;
	QVariantList values = QVariantList() << rot.x << rot.y << rot.z;


	// read out position of current controller
	Ogre::Vector3 pos;
	if( m_outputBonePosition ) {
		pos = m_positionScale * Ogre::Vector3(controllerData->pos);
	} else {
		pos = Ogre::Vector3( .0f, .0f, .0f);
	}

	values << pos.x << pos.y << pos.z;

	boneParameter->setValue( values, m_triggerDirty );
}

void RazerHydraNode::updateBoneQuat( sixenseControllerData* controllerData, RazerHydraAxisMapping axisMapping[], NumberParameter* boneParameter, Ogre::Quaternion offsetOrientation, Ogre::Vector3 offsetPosition )
{
	// orientation
	Ogre::Quaternion q;
	q.x = controllerData->rot_quat[0];
	q.y = controllerData->rot_quat[1];
	q.z = controllerData->rot_quat[2];
	q.w = controllerData->rot_quat[3];

	q = q * offsetOrientation.Inverse();
	QVariantList values = QVariantList() << q.w << q.x << q.y << q.z;

	// read out position of current controller
	Ogre::Vector3 pos;
	if( m_outputBonePosition ) {
		pos = m_positionScale * (Ogre::Vector3(controllerData->pos) - offsetPosition);
	} else {
		pos = Ogre::Vector3( .0f, .0f, .0f);
	}

	values << pos.x << pos.y << pos.z;
	boneParameter->setValue( values, m_triggerDirty );
}

void RazerHydraNode::updateTrigger( sixenseControllerData* controllerData, NumberParameter* triggerParameter )
{
	triggerParameter->setValue( QVariant( m_valueScale * controllerData->trigger), m_triggerDirty);
}

///
/// Private Functions
///


void RazerHydraNode::processOutputImage()
{
	if( m_controller_manager_setup && QFile::exists(m_setupTextureFilename))
	{
		Ogre::Image setupImage;
		setupImage.load( m_setupTextureFilename.toStdString(), "General");

		Ogre::TexturePtr setupTexture = Ogre::TextureManager::getSingleton().loadImage( 
									"RazerHydraSetupTexture", 
									Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
									setupImage, Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_A8R8G8B8 );

		setValue( m_outputImageName, setupTexture );
	}
	else
	{
		setValue(m_outputImageName, m_razerTexture);
	}
}

void RazerHydraNode::loadRazerImage()
{
	Ogre::Image razerImage;
	razerImage.load("razer-hydra.png", "General");

	m_razerTexture = Ogre::TextureManager::getSingleton().loadImage( "RazerHydraImage", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, razerImage,
		Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_A8R8G8B8);
}

void RazerHydraNode::getParameterReferences()
{
	// Left Controller parameters
	m_leftHandButtonParameters[ BUTTON_1 ]        = getParameter("Left Controller > Left Button 1");
	m_leftHandButtonParameters[ BUTTON_2 ]        = getParameter("Left Controller > Left Button 2");
	m_leftHandButtonParameters[ BUTTON_3 ]        = getParameter("Left Controller > Left Button 3");
	m_leftHandButtonParameters[ BUTTON_4 ]        = getParameter("Left Controller > Left Button 4");
	m_leftHandButtonParameters[ BUTTON_START ]    = getParameter("Left Controller > Left Button Start");
	m_leftHandButtonParameters[ BUTTON_JOYSTICK ] = getParameter("Left Controller > Left Button Joystick");
	m_leftHandButtonParameters[ BUTTON_BUMPER ]   = getParameter("Left Controller > Left Button Bumper");

	for( int i=0; i<NUM_BUTTONS; i++)
		assert( m_leftHandButtonParameters[i] );

	m_leftHandJoystickParameters[ JOYSTICK_UP ]    = getParameter("Left Controller > Left Joystick Up");
	m_leftHandJoystickParameters[ JOYSTICK_DOWN ]  = getParameter("Left Controller > Left Joystick Down");
	m_leftHandJoystickParameters[ JOYSTICK_LEFT ]  = getParameter("Left Controller > Left Joystick Left");
	m_leftHandJoystickParameters[ JOYSTICK_RIGHT ] = getParameter("Left Controller > Left Joystick Right");

	for( int i=0; i<NUM_JOYSTICK_POSITIONS; i++)
		assert( m_leftHandJoystickParameters[i] );

	m_leftHandJoystickPositionParameters[ JOYSTICK_UP ]    = getNumberParameter("Left Controller > Left Joystick Up Position");
	m_leftHandJoystickPositionParameters[ JOYSTICK_DOWN ]  = getNumberParameter("Left Controller > Left Joystick Down Position");
	m_leftHandJoystickPositionParameters[ JOYSTICK_LEFT ]  = getNumberParameter("Left Controller > Left Joystick Left Position");
	m_leftHandJoystickPositionParameters[ JOYSTICK_RIGHT ] = getNumberParameter("Left Controller > Left Joystick Right Position");

	for( int i=0; i<NUM_JOYSTICK_POSITIONS; i++)
		assert( m_leftHandJoystickPositionParameters[i] );

	m_leftHandTriggerParameter = getNumberParameter("Left Controller > Left Trigger");
	m_leftHandBoneParameter    = getNumberParameter("Left Controller > Left Hand Bone");
	m_leftHandBoneQuatParameter = getNumberParameter("Left Controller > Left Hand Bone Quat");

	assert(m_leftHandTriggerParameter);
	assert(m_leftHandBoneParameter);
	assert(m_leftHandBoneQuatParameter);

	// Right Controller parameters
	m_rightHandButtonParameters[ BUTTON_1 ]        = getParameter("Right Controller > Right Button 1");
	m_rightHandButtonParameters[ BUTTON_2 ]        = getParameter("Right Controller > Right Button 2");
	m_rightHandButtonParameters[ BUTTON_3 ]        = getParameter("Right Controller > Right Button 3");
	m_rightHandButtonParameters[ BUTTON_4 ]        = getParameter("Right Controller > Right Button 4");
	m_rightHandButtonParameters[ BUTTON_START ]    = getParameter("Right Controller > Right Button Start");
	m_rightHandButtonParameters[ BUTTON_JOYSTICK ] = getParameter("Right Controller > Right Button Joystick");
	m_rightHandButtonParameters[ BUTTON_BUMPER ]   = getParameter("Right Controller > Right Button Bumper");

	for( int i=0; i<NUM_BUTTONS; i++)
		assert( m_rightHandButtonParameters[i] );

	m_rightHandJoystickParameters[ JOYSTICK_UP ]    = getParameter("Right Controller > Right Joystick Up");
	m_rightHandJoystickParameters[ JOYSTICK_DOWN ]  = getParameter("Right Controller > Right Joystick Down");
	m_rightHandJoystickParameters[ JOYSTICK_LEFT ]  = getParameter("Right Controller > Right Joystick Left");
	m_rightHandJoystickParameters[ JOYSTICK_RIGHT ] = getParameter("Right Controller > Right Joystick Right");

	for( int i=0; i<NUM_JOYSTICK_POSITIONS; i++)
		assert( m_rightHandJoystickParameters[i] );

	m_rightHandJoystickPositionParameters[ JOYSTICK_UP ]    = getNumberParameter("Right Controller > Right Joystick Up Position");
	m_rightHandJoystickPositionParameters[ JOYSTICK_DOWN ]  = getNumberParameter("Right Controller > Right Joystick Down Position");
	m_rightHandJoystickPositionParameters[ JOYSTICK_LEFT ]  = getNumberParameter("Right Controller > Right Joystick Left Position");
	m_rightHandJoystickPositionParameters[ JOYSTICK_RIGHT ] = getNumberParameter("Right Controller > Right Joystick Right Position");

	for( int i=0; i<NUM_JOYSTICK_POSITIONS; i++)
		assert( m_rightHandJoystickPositionParameters[i] );

	m_rightHandTriggerParameter  = getNumberParameter("Right Controller > Right Trigger");
	m_rightHandBoneParameter     = getNumberParameter("Right Controller > Right Hand Bone");
	m_rightHandBoneQuatParameter = getNumberParameter("Right Controller > Right Hand Bone Quat");

	assert(m_rightHandBoneQuatParameter);
	assert(m_rightHandTriggerParameter);
	assert(m_rightHandBoneParameter);
}

void RazerHydraNode::axisMappingChanged()
{
	m_leftHandAxisMapping[AXIS_YAW]   = (RazerHydraAxisMapping) getEnumerationParameter("Rotation Mapping Left > Yaw Left")->getCurrentIndex();
	m_leftHandAxisMapping[AXIS_PITCH] = (RazerHydraAxisMapping) getEnumerationParameter("Rotation Mapping Left > Pitch Left")->getCurrentIndex();
	m_leftHandAxisMapping[AXIS_ROLL]  = (RazerHydraAxisMapping) getEnumerationParameter("Rotation Mapping Left > Roll Left")->getCurrentIndex();

	m_leftHandaxisMapping[AXIS_YAW]   = (RazerHydraAxisMapping) getEnumerationParameter("Rotation Mapping Right > Yaw Right")->getCurrentIndex();
	m_leftHandaxisMapping[AXIS_PITCH] = (RazerHydraAxisMapping) getEnumerationParameter("Rotation Mapping Right > Pitch Right")->getCurrentIndex();
	m_leftHandaxisMapping[AXIS_ROLL]  = (RazerHydraAxisMapping) getEnumerationParameter("Rotation Mapping Right > Roll Right")->getCurrentIndex();

}

void RazerHydraNode::setInitialOrientation()
{
	// skip while in setup mode
	if( m_controller_manager_setup )
		return;

	if( getBoolValue("Set Initial Orientation"))
	{
		int left_controller  = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1L );
		int right_controller = sixenseUtils::getTheControllerManager()->getIndex( sixenseUtils::ControllerManager::P1R );

		sixenseSetActiveBase(0);
		sixenseAllControllerData controllerData;
		sixenseGetAllNewestData(&controllerData);
		sixenseUtils::getTheControllerManager()->update(&controllerData);


		// orientation
		Ogre::Radian rx, ry, rz;
		Ogre::Matrix3 rot_mat;
		Ogre::Vector3 rot_vec;

		rot_mat = Ogre::Matrix3( controllerData.controllers[left_controller].rot_mat );
		rot_mat.ToEulerAnglesXYZ(rx, ry, rz);
		rot_vec = Ogre::Vector3( rx.valueRadians(), ry.valueRadians(), rz.valueRadians());
		calculateAxisMapping( m_leftHandAxisMapping, rot_vec, m_leftHandInitialOrientationEuler );

		rot_mat = Ogre::Matrix3( controllerData.controllers[right_controller].rot_mat );
		rot_mat.ToEulerAnglesXYZ(rx, ry, rz);
		rot_vec = Ogre::Vector3( rx.valueRadians(), ry.valueRadians(), rz.valueRadians());
		calculateAxisMapping( m_leftHandaxisMapping, rot_vec, m_rightHandInitialOrientationEuler );

		m_leftHandInitialOrientationQuat.x = controllerData.controllers[left_controller].rot_quat[0];
		m_leftHandInitialOrientationQuat.y = controllerData.controllers[left_controller].rot_quat[1];
		m_leftHandInitialOrientationQuat.z = controllerData.controllers[left_controller].rot_quat[2];
		m_leftHandInitialOrientationQuat.w = controllerData.controllers[left_controller].rot_quat[3];
			
		m_rightHandInitialOrientationQuat.x = controllerData.controllers[right_controller].rot_quat[0];
		m_rightHandInitialOrientationQuat.y = controllerData.controllers[right_controller].rot_quat[1];
		m_rightHandInitialOrientationQuat.z = controllerData.controllers[right_controller].rot_quat[2];
		m_rightHandInitialOrientationQuat.w = controllerData.controllers[right_controller].rot_quat[3];

		m_leftHandInitialPosition = Ogre::Vector3( controllerData.controllers[left_controller].pos );
		m_rightHandInitialPosition = Ogre::Vector3( controllerData.controllers[right_controller].pos );

		setValue("Set Initial Orientation", false);
	}
}

void RazerHydraNode::calculateAxisMapping( RazerHydraAxisMapping * axisMapping, const Ogre::Vector3& eulerIn, Ogre::Vector3& eulerOut )
{
	eulerOut = Ogre::Vector3::ZERO;

	switch( axisMapping[AXIS_YAW] )
	{
	case AXIS_X:       eulerOut.x += eulerIn.y; break;
	case AXIS_Y:       eulerOut.y += eulerIn.y; break;
	case AXIS_Z:       eulerOut.z += eulerIn.y; break;
	case AXIS_MINUS_X: eulerOut.x -= eulerIn.y; break;
	case AXIS_MINUS_Y: eulerOut.y -= eulerIn.y; break;
	case AXIS_MINUS_Z: eulerOut.z -= eulerIn.y; break;
	}

	switch( axisMapping[AXIS_PITCH] ) 
	{
	case AXIS_X:       eulerOut.x += eulerIn.x; break;
	case AXIS_Y:       eulerOut.y += eulerIn.x; break;
	case AXIS_Z:       eulerOut.z += eulerIn.x; break;
	case AXIS_MINUS_X: eulerOut.x -= eulerIn.x; break;
	case AXIS_MINUS_Y: eulerOut.y -= eulerIn.x; break;
	case AXIS_MINUS_Z: eulerOut.z -= eulerIn.x; break;
	}

	switch( axisMapping[AXIS_ROLL] ) 
	{
	case AXIS_X:       eulerOut.x += eulerIn.z; break;
	case AXIS_Y:       eulerOut.y += eulerIn.z; break;
	case AXIS_Z:       eulerOut.z += eulerIn.z; break;
	case AXIS_MINUS_X: eulerOut.x -= eulerIn.z; break;
	case AXIS_MINUS_Y: eulerOut.y -= eulerIn.z; break;
	case AXIS_MINUS_Z: eulerOut.z -= eulerIn.z; break;
	}
}


} // namespace RazerHydraNode
