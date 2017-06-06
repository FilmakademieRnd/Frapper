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
//! \file "MidiTimerNode.cpp"
//! \brief Implementation file for TimerNode class.
//!
//! \author     Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       13.05.2014 (last updated)
//!

#include "MidiTimerNode.h"
#include <QDateTime>

namespace MidiTimerNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the MidiTimerNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
MidiTimerNode::MidiTimerNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
	m_midiIn(0)
{
    // store references to parameters for faster access
    data.m_pSystemTime = getNumberParameter("SystemTime");
    assert(data.m_pSystemTime);

    // Override default range given by NumberParameter
    data.m_pSystemTime->setMinValue(0);
    data.m_pSystemTime->setMaxValue(std::numeric_limits<int>::max()); // STL needs to be used here, as there is no analogon in Qt

	//create & setup Midi Input Interface
	m_midiIn = new RtMidiIn();
	data.numFrames = getValue("MidiFPS", true).toInt();
	data.times.resize(4);
	data.times[0] = 0;
	data.times[1] = 0;
	data.times[2] = 0;
	data.times[3] = 0;
	int nPorts = m_midiIn->getPortCount();
	if(nPorts > 0) {
		setValue("MidiPort", 0);
		m_midiIn->openPort(0);
		m_midiIn->setCallback( &midiMtcParser, &data );
		m_midiIn->ignoreTypes( false, false, false);
	}
	else {
		setValue("MidiPort", -1);
	}

	data.active = getBoolValue("Active");

    // Connect change functions of parameters with timer activation
	setChangeFunction("Active", SLOT(activateTimer()));

	// Connect change function for interval change
	setChangeFunction("MidiFPS", SLOT(changeFPS()));

	setProcessingFunction("switch", SLOT(activateTimer()));
}


//!
//! Destructor of the MidiTimerNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MidiTimerNode::~MidiTimerNode ()
{
    delete m_midiIn;
}

void MidiTimerNode::midiMtcParser( double deltatime, std::vector< unsigned char > *message, void *data )
{
	TimeParameters *midiData = static_cast<TimeParameters*>(data);
	if(midiData->active) {
		static const QTime qTime;
		const int messageType = static_cast<int>(message->at(0));
		const int messageIndex = static_cast<int>(message->at(1)) >> 4; // the high nibble: which quarter message is this (0...7).
		const int value = static_cast<int>(message->at(1)) & 0x0F; // the low nibble: value
		const int timeIndex = messageIndex >> 1; // which time component (frames, seconds, minutes or hours) is this
		const bool bNewFrame = messageIndex % 4 == 0;

		if(messageType == 241) { //if this is a MTC message
			if (midiData->numFrames != 24 && midiData->numFrames != 25 && midiData->numFrames != 30) {
				//std::cout << "Invalid Midi FPS! Using default: 25 \n";
				midiData->numFrames = 25;
			}

			// the time encoded in the MTC is 1 frame behind by the time we have received a new frame, so adjust accordingly
			if(bNewFrame) {
				midiData->times[0] += 1; //frames
				if(midiData->times[0] >= midiData->numFrames) {
					midiData->times[0] %= midiData->numFrames; //frames
					midiData->times[1] += 1; //second
					if(midiData->times[1] >= 60) {
						midiData->times[1] %= 60; //second
						midiData->times[2] += 1; //minute
						if(midiData->times[2] >= 60) {
							midiData->times[2] %= 60; //minute
							midiData->times[3] += 1; // hour
						}
					}
				}
				const int currentTime = qTime.msecsTo(QTime(midiData->times[3], midiData->times[2], midiData->times[1], (1000.0f/midiData->numFrames)*midiData->times[0]));
				midiData->m_pSystemTime->setValue(QVariant(currentTime), true);
			}
			if(messageIndex % 2 == 0)	// if this is lower nibble of time component
				midiData->times[timeIndex]    = value;
			else						// ... or higher nibble 
				midiData->times[timeIndex]    |=  value << 4;

			if(messageIndex == 7) {
				midiData->times[3] &= 0x1F;                               // only use lower 5 bits for hours (higher bits indicate SMPTE type)
				int smpteType = value >> 1;
				switch(smpteType) {
				case 0: midiData->numFrames = 24; break;
				case 1: midiData->numFrames = 25; break;
				case 2: midiData->numFrames = 30; break;
				case 3: midiData->numFrames = 30; break;
				default: midiData->numFrames = 100;
				}
			}
		}
	}
}

void MidiTimerNode::activateTimer()
{
	bool value = dynamic_cast<Parameter *>(sender())->getValue().toBool();
    if( value ) {
		data.active = true;
    } 
	else {
        data.active = false;
    }
}

void MidiTimerNode::changeFPS()
{
	data.numFrames = getValue("MidiFPS", true).toInt();
}

} // namespace MidiTimerNode 
