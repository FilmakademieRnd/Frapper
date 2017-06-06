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
//! \file "WiiTrackerNode.cpp"
//! \brief Implementation file for WiiTrackerNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       19.05.2009 (last updated)
//!

#include "WiiTrackerNode.h"
#ifndef WIN32
	#include <unistd.h>
#endif
#include "ParameterGroup.h"

#include "Helper.h"

#define MAX_WIIMOTES 4

namespace WiiTrackerNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the WiiTrackerNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
WiiTrackerNode::WiiTrackerNode ( QString name, ParameterGroup *parameterRoot ) :
	Node(name, parameterRoot),
	m_numFound(0),
	m_numConnected(0),
	m_stopRequested(0)
{
	m_rollList.resize(4);
	m_pitchList.resize(4);

	for( int i=0; i<4; i++) 
	{
		for( int j=0; j<20; j++) 
		{
			m_rollList[i].append(0.0f);
            m_pitchList[i].append(0.0f);
		}
	}
	
	// set affections and callback functions
	setChangeFunction("run", SLOT(runChanged()));

    setChangeFunction("Wiimote 0 > wm0_motionSensing", SLOT( triggerMotionSensing()));
    setChangeFunction("Wiimote 1 > wm1_motionSensing", SLOT( triggerMotionSensing()));
    setChangeFunction("Wiimote 2 > wm2_motionSensing", SLOT( triggerMotionSensing()));
    setChangeFunction("Wiimote 3 > wm3_motionSensing", SLOT( triggerMotionSensing()));


 }


//!
//! Destructor of the WiiTrackerNode class.
//!
WiiTrackerNode::~WiiTrackerNode ()
{
	Log::debug("Stopping thread...", "WiiTrackerNode::~WiiTrackerNode");

	//wiiuse_disconnect(m_wiimotes[0]);
	//wiiuse_disconnect(m_wiimotes[1]);
	//wiiuse_shutdown();
	//quit();

	// request the thread to be stopped
	m_stopRequested = true;
	setValue("run", false);

	// wait for the thread to finish
	while (isRunning());
	quit();

	Log::debug("Thread has been stopped.", "WiiTrackerNode::~WiiTrackerNode");
}


///
/// Public Functions
///


//!
//! Main function for vision handling for the virtual agent.
//! Will be run in an own thread.
//!
void WiiTrackerNode::run ()
{
	while (true) {
		// check if the thread should be stopped
		if (m_stopRequested) 
		{
			for( int i=0; i<m_numConnected; i++)
				wiiuse_disconnect(m_wiimotes[i]);

			break;
		}

		if (wiiuse_poll(m_wiimotes, m_numConnected)) 
		{
			/*
			*    This happens if something happened on any wiimote.
			*    So go through each one and check if anything happened.
			*/
			for (int i=0; i < m_numConnected; ++i) 
			{
				switch (m_wiimotes[i]->event) 
				{
					case WIIUSE_EVENT:
						/* a generic event occured */
						handleEvent(m_wiimotes[i], i);

						//case WIIUSE_STATUS:
						//	/* a status event occured */
						//	handle_ctrl_status(wiimotes[i]);
						//	break;

					case WIIUSE_DISCONNECT:
					case WIIUSE_UNEXPECTED_DISCONNECT:
						/* the wiimote disconnected */
						//handle_disconnect(wiimotes[i]);
						break;

					case WIIUSE_READ_DATA:
						/*
						*	Data we requested to read was returned.
						*	Take a look at wiimotes[i]->read_req
						*	for the data.
						*/
						break;

					case WIIUSE_NUNCHUK_INSERTED:
						/*
						*	a nunchuk was inserted
						*	This is a good place to set any nunchuk specific
						*	threshold values.  By default they are the same
						*	as the wiimote.
						*/
						//wiiuse_set_nunchuk_orient_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 90.0f);
						//wiiuse_set_nunchuk_accel_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 100);
						printf("Nunchuk inserted.\n");
						break;

					case WIIUSE_CLASSIC_CTRL_INSERTED:
						printf("Classic controller inserted.\n");
						break;

					case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
						/* some expansion was inserted */
						//handle_ctrl_status(wiimotes[i]);
						printf("Guitar Hero 3 controller inserted.\n");
						break;

					case WIIUSE_NUNCHUK_REMOVED:
					case WIIUSE_CLASSIC_CTRL_REMOVED:
					case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
						/* some expansion was removed */
						//handle_ctrl_status(wiimotes[i]);
						printf("An expansion was removed.\n");
						break;

					default:
						break;
				}
			}
		}
		//msleep(4);
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
void WiiTrackerNode::runChanged ()
{
	if (getBoolValue("run")) 
	{
		if (!initialize()) 
		{
			setValue("run", false);
			return;
		}

		m_stopRequested = false;
		start();
	} 
	else 
	{
		m_stopRequested = true;
	}
}


//!
//!
//!
void WiiTrackerNode::timerUpdate ()
{
}


///
/// Private Functions
///


//!
//! Finds and connects with Wiimote controllers that are in discovery mode.
//!
//! \return True if at least one Wiimote controller could be connected, otherwise False.
//!
bool WiiTrackerNode::initialize ()
{
	// initialize the array of wiimote objects
	m_wiimotes = wiiuse_init(MAX_WIIMOTES);

	// find Wiimote devices that are in discovery mode using a timeout of 5 seconds
	m_numFound = wiiuse_find(m_wiimotes, MAX_WIIMOTES, 5);
	if (!m_numFound) 
	{
		Log::warning("No Wiimotes found.", "WiiTrackerNode::initialize");
		return false;
	}

	// connect to the Wiimotes found
	m_numConnected = wiiuse_connect( m_wiimotes, MAX_WIIMOTES);
	if (m_numConnected)
		Log::info(QString("Connected to %1 Wiimotes (of %2 found).").arg( m_numConnected ).arg(m_numFound), "WiiTrackerNode::initialize");
	else {
		Log::error("Failed to connect to any Wiimote.", "WiiTrackerNode::initialize");
		return false;
	}

	// set the LEDs and rumble for a second so it's easy to tell which wiimotes are connected (just like the wii does)
	wiiuse_set_leds( m_wiimotes[0], WIIMOTE_LED_1);
	wiiuse_set_leds( m_wiimotes[1], WIIMOTE_LED_2);
	wiiuse_set_leds( m_wiimotes[2], WIIMOTE_LED_3);
	wiiuse_set_leds( m_wiimotes[3], WIIMOTE_LED_4);

	for( int i=0; i<m_numConnected; i++ )
	{
		wiiuse_rumble( m_wiimotes[i], 1);

		// set initial parameters
        wiiuse_motion_sensing( m_wiimotes[i], getBoolValue( QString("Wiimote %1 > wm%1_motionSensing").arg(i)));
		wiiuse_set_ir( m_wiimotes[i], 0);
		wiiuse_set_flags( m_wiimotes[i], NULL, WIIUSE_SMOOTHING);

		Frapper::Log::info( "Battery level of Wiimote "+QString::number(i)+": "+QString::number( m_wiimotes[i]->battery_level), "WiiTrackerNode::initialize");
	}

	Sleep(200);

	for( int i=0; i<m_numConnected; i++ )
	{
		wiiuse_rumble( m_wiimotes[i], 0);
	}

	return true;
}


//!
//! Callback that handles a Wiimote event.
//!
//! Is called by the wiiuse library when an event occurs on the specified
//! Wiimote.
//!
//! \param wm The Wiimote controller that caused the event.
//!
void WiiTrackerNode::handleEvent( struct wiimote_t *wm, int wiimoteID )
{
	ParameterGroup* group = this->getParameterGroup( "Wiimote "+QString::number(wiimoteID));
	assert( group );
	QString wm_ = "wm"+QString::number(wiimoteID)+"_";
	
	group->setValue( wm_+"buttonA", IS_PRESSED( wm, WIIMOTE_BUTTON_A), true);
	group->setValue( wm_+"buttonB", IS_PRESSED( wm, WIIMOTE_BUTTON_B), true);
	group->setValue( wm_+"buttonUp", IS_PRESSED( wm, WIIMOTE_BUTTON_UP), true);
	group->setValue( wm_+"buttonDown", IS_PRESSED( wm, WIIMOTE_BUTTON_DOWN), true);
	group->setValue( wm_+"buttonLeft", IS_PRESSED( wm, WIIMOTE_BUTTON_LEFT), true);
	group->setValue( wm_+"buttonRight", IS_PRESSED( wm, WIIMOTE_BUTTON_RIGHT), true);
	group->setValue( wm_+"buttonMinus", IS_PRESSED( wm, WIIMOTE_BUTTON_MINUS), true);
	group->setValue( wm_+"buttonPlus", IS_PRESSED( wm, WIIMOTE_BUTTON_PLUS), true);
	group->setValue( wm_+"buttonOne", IS_PRESSED( wm, WIIMOTE_BUTTON_ONE), true);
	group->setValue( wm_+"buttonTwo", IS_PRESSED( wm, WIIMOTE_BUTTON_TWO), true);
	group->setValue( wm_+"buttonHome", IS_PRESSED( wm, WIIMOTE_BUTTON_HOME), true);

	/* motion plus test begin */
	//if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_ONE))
	//	wiiuse_set_motion_plus(wm, 1);
	//if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_TWO))
	//	wiiuse_set_motion_plus(wm, 0);
	//if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_HOME))
	//	m_motionPlusYaw = 0.0;

	// if the accelerometer is turned on then output angles
	if (WIIUSE_USING_ACC(wm)) 
	{
		// get roll value and smooth
        double roll = wm->orient.roll;
		m_rollList[wiimoteID].removeFirst();
		m_rollList[wiimoteID].append( roll);

        roll = 0.0;
		for (int i = 0; i < m_rollList[wiimoteID].size(); ++i) 
			roll += m_rollList[wiimoteID][i];
        roll /= m_rollList[wiimoteID].size();


		// get pitch value and smooth
        double pitch = wm->orient.pitch;
		m_pitchList[wiimoteID].removeFirst();
		m_pitchList[wiimoteID].append( pitch);
		
        pitch = 0.0;
		for (int i = 0; i < m_pitchList[wiimoteID].size(); ++i)
			pitch += m_pitchList[wiimoteID][i];
        pitch /= m_pitchList[wiimoteID].size();

        CLAMP( roll, -90.0, 90.0);
        CLAMP( pitch, -90.0, 90.0);

        //
		//roll += 90.0;
		//roll /= 180.0;

		//if(pitch > 90.0)
		//	pitch = 90.0;
		//else if(pitch < -90.0)
		//	pitch = -90.0;
		//
		//pitch += 90.0;
		//pitch /= 180.0;

		//Log::debug(QString("roll: %1, pitch: %2, yaw: %3").arg(roll).arg(pitch).arg(yaw), "WiiTrackerNode::handleEvent");

		double rollLeft = 0.0; /* = getValue("rollLeft").toDouble();*/
		double rollRight = 0.0; /* = getValue("rollRight").toDouble();*/
		double pitchFront = 0.0; /* = getValue("pitchFront").toDouble();*/
		double pitchBack = 0.0; /* = getValue("pitchBack").toDouble();*/


		if (roll >= 0 ) {
			rollRight = roll/ 90.0 * 100.0;
		} else {
			rollLeft = roll/ -90.0 * 100.0;
		}

		if ( pitch >= 0 ) {
			pitchFront = pitch/ 90.0 * 100.0;
		} else {
			pitchBack = pitch/ -90.0 * 100.0;
		}

		group->setValue( wm_+"roll", roll, true);
		group->setValue( wm_+"pitch", pitch, true);
		group->setValue( wm_+"rollLeft", rollLeft, true);
		group->setValue( wm_+"rollRight", rollRight, true);
		group->setValue( wm_+"pitchFront", pitchFront, true);
		group->setValue( wm_+"pitchBack", pitchBack, true);

	}

	// if IR tracking is enabled then output the coordinates on the virtual screen that the Wiimote is pointing to
	// (also make sure that we see at least 1 dot)
	if (WIIUSE_USING_IR(wm)) {
		// go through each of the 4 possible IR sources
		// NILZ: das hier macht alles kaputt
		//for (int i = 0; i < 4; ++i)
		//    // check if the source is visible
		//    if (wm->ir.dot[i].visible)
		////        Log::debug(QString("IR source %1: (%2, %3)").arg(i).arg(wm->ir.dot[0].x).arg(wm->ir.dot[i].y), "WiiTrackerNode::handleEvent");

		//double irDotDist = getDoubleValue("irDotDist");
		//if (wm->ir.dot[0].visible && wm->ir.dot[1].visible && !wm->ir.dot[2].visible && !wm->ir.dot[3].visible) {
		//	double xdiff;
		//	double ydiff;
		//	if(wm->ir.dot[0].x >= wm->ir.dot[1].x)
		//		xdiff = fabs((double) wm->ir.dot[0].x / 600 - wm->ir.dot[1].x / 600);
		//	else
		//		xdiff = fabs((double) wm->ir.dot[1].x / 600 - wm->ir.dot[0].x / 600);

		//	if(wm->ir.dot[0].y >= wm->ir.dot[1].y)
		//		ydiff = fabs((double) wm->ir.dot[0].y / 600 - wm->ir.dot[1].y / 600);
		//	else
		//		ydiff = fabs((double) wm->ir.dot[1].y / 600 - wm->ir.dot[0].y / 600);

		//	irDotDist = sqrt(xdiff * xdiff + ydiff * ydiff);
		//}
		//group->setValue("irDotDist", irDotDist * 3, true);

		//double irXPos = getDoubleValue("irXPos");
		//double irYPos = getDoubleValue("irYPos");
		//if (wm->ir.x > 0 && wm->ir.y > 0) {
		//	irXPos = ((double) wm->ir.x) / 600.0;
		//	irYPos = ((double) wm->ir.y) / 500.0;
		//	if (irXPos > 1.0)
		//		irXPos = 1.0;
		//	if (irYPos > 1.0)
		//		irYPos = 1.0;
		//}
		//group->setValue("irXPos", QVariant(irXPos), true);
		//group->setValue("irYPos", QVariant(irYPos), true);

		////printf("IR cursor: (%u, %u)\n", wm->ir.x, wm->ir.y);
		////printf("IR z distance: %f\n", wm->ir.z);
		////printf("Second IR cursor: (%f, %f)\n", irXPos, irYPos );
	}

	// show events specific to supported expansions
	//if (wm->exp.type == EXP_NUNCHUK) {
	//	struct nunchuk_t *nc = (nunchuk_t *) &wm->exp.nunchuk;
	//	bool buttonC = IS_PRESSED(nc, NUNCHUK_BUTTON_C);
	//	bool buttonZ = IS_PRESSED(nc, NUNCHUK_BUTTON_Z);
	//	double joyAngle = nc->js.ang;
	//	double joyMag = nc->js.mag;
	//	if (joyMag > 1.0)
	//		joyMag = 1.0;
	//	double nunJoyLeft = 0.0;
	//	double nunJoyRight = 0.0;

	//	Log::debug(QString("joyAngle: %1, joyMag: %2").arg(joyAngle).arg(joyMag), "WiiTrackerNode::handleEvent");

	//	if (joyAngle >= 227.0 && joyAngle <= 314.0 && joyMag >= 0.06) {
	//		//> Nunchuk joystick left
	//		nunJoyLeft = joyMag;
	//		nunJoyRight = 0.0;
	//	} else if (joyAngle <= 134.0 && joyAngle >= 42.0 && joyMag >= 0.06) {
	//		//> Nunchuk joystick right
	//		nunJoyLeft = 0.0;
	//		nunJoyRight = joyMag;
	//	} else if (joyMag <= 0.06) {
	//		nunJoyLeft = 0.0;
	//		nunJoyRight = 0.0;
	//	}

	//	group->setValue("nunJoyLeft", nunJoyLeft, true);
	//	group->setValue("nunJoyRight", nunJoyRight, true);

	//}
	//else if (wm->exp.type == EXP_MOTION_PLUS) {
	//	struct motion_plus_t* mp = (motion_plus_t*)&wm->exp.mp;
	//	//printf("drx  = %u\n", mp->rx);
	//	//printf("dry = %u\n", mp->ry);
	//	//printf("drz   = %u\n", mp->rz);
	//	printf("motion plus event\n");
	//	double yaw = mp->rz - 8180.0;
	//	if(abs(yaw) > 30.0) { 
	//		m_motionPlusYaw += yaw;
	//		if (m_motionPlusYaw > 0.0) {
	//			group->setValue("yawLeft", m_motionPlusYaw/2000.0, true);
	//			group->setValue("yawRight", 0.0, true);
	//		}
	//		else { 
	//			group->setValue("yawRight", -m_motionPlusYaw/2000.0, true);
	//			group->setValue("yawLeft", 0.0, true);
	//		}
	//	}
	//}
}


//!
//!    Callback that handles a Wiimote disconnection event.
//!
//! Is called when the power button is pressed or when the connection is
//! interrupted.
//!
//! \param wm The Wiimote controller that caused the event.
//!
void WiiTrackerNode::handleDisconnect ( wiimote *wm )
{
	Log::info(QString("Wiimote %1 has been disconnected.").arg(wm->unid), "WiiTrackerNode::handleDisconnect");
}


void WiiTrackerNode::onStopThread ()
{
	// disconnect the wiimotes
	//wiiuse_cleanup(m_wiimotes, MAX_WIIMOTES);
	m_numFound = 0;
	m_numConnected = 0;
}

void WiiTrackerNode::triggerMotionSensing()
{
    Parameter* param = dynamic_cast<Parameter*>(sender());
    if( param )
    {
        QRegExp regex("wm([0-3])_");

        int pos = regex.indexIn(param->getName());
        if (pos > -1) 
        {
            int wiimote_id = regex.cap(1).toInt();
            if( wiimote_id < m_numConnected )
            {
                bool state = param->getValue().toBool();
                wiiuse_motion_sensing( m_wiimotes[wiimote_id], state);
            }
        }
    }
}

} // namespace WiiTrackerNode
