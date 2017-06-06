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
//! \file "MidiTimerNode.h"
//! \brief Header file for TimerNode class.
//!
//! \author     Jonas Trottnow <jonas.trottnow@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       13.05.2014 (last updated)
//!

#ifndef MIDITIMERNODE_H
#define MIDITIMERNODE_H

#include "Node.h"
#include "RtMidi.h"
#include "qvector.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

// forward declaration
class QTimer;

namespace MidiTimerNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class MidiTimerNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

private: // structures
	
	//!
    //! Data structure for MIDI callback.
    //!
	struct TimeParameters
	{
		//! 
		//! Active flag.
		//! 
		bool active;

		//! 
		//! Number of frames per second.
		//! Start off with arbitrary high number until we receive it.
		//! 
		int numFrames;	 
		
		//!
		//! Pointer to the nodes output time parameter.
		//!
		NumberParameter *m_pSystemTime;

		//!
		//! This buffer holds the 4 time componens (frames, seconds, minutes, hours).
		//!
		QVector<int> times;
	};


public: // constructors and destructors

    //!
    //! Constructor of the MidiTimerNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    MidiTimerNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the MidiTimerNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~MidiTimerNode ();

private slots:

    //! 
    //! Slot for the timer (de-)activation
    //! 
    void activateTimer();

	//! 
    //! Slot for the timer inteval in x/second change
    //! 
	void changeFPS();


private: //funtions
	
	//!
	//! Parser for Midi Messages to Timecode
	//!
	static void midiMtcParser( double deltatime, std::vector< unsigned char > *message, void *data );

private: // data

    //!
	//! Midi Interface
	//!
	RtMidiIn *m_midiIn;
		
	//!
	//! Struct of following data for static function midiMtcParser
	//!
	TimeParameters data;
};

} // namespace MidiTimerNode 

#endif
