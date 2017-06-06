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
//! \file "AudioPlayerNode.h"
//! \brief Header file for AudioPlayerNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       13.01.2013 (last updated)
//!

#ifndef AUDIOPLAYERNODE_H
#define AUDIOPLAYERNODE_H

#include "FrapperPrerequisites.h"
#include "Node.h"

#include <QPair>

#include "bass.h"

namespace AudioPlayerNode {
	using namespace Frapper;

//!
//! Class representing nodes that can load solver reference data.
//!
class AudioPlayerNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

protected:
	class specData 
	{
	public:
		NumberParameter *parameter;
		int upper;
		int lower;
	public:
		inline specData()
		{
		}
		inline specData(NumberParameter *parameter, int upper, int lower)
			: parameter(parameter), upper(upper), lower(lower) 
		{
		}
	};

public: // constructors and destructors

    //!
    //! Constructor of the AudioPlayerNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AudioPlayerNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AudioPlayerNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~AudioPlayerNode ();


private: // functions


private slots: //

    //!
    //! Loads a Bass conform audio data file.
    //!
    //! Is called when the value of the audio file parameter has changed.
    //!
    //! \return true if the audio file data was successfully loaded from file, false otherwise.
    //!
    bool loadAudioFile ();

	//!
    //! Starts or stops the audio playback.
    //!
    void controlAudioFile ();

	//!
	//! Controls the volume of the audio playback.
	//!
	void controlVolumeFile ();

	//!
    //! Sets the bounds for the analizer.
    //!
	void setBounds ();

protected: // events

    //!
    //! Handles timer events for the Node.
    //!
    //! \param event The description of the timer event.
    //!
    virtual void timerEvent ( QTimerEvent *event );

private: // data

	unsigned long m_chan;

	int m_specTimerId, m_timeTimerId;

	specData m_specOut1, m_specOut2, m_specOut3, m_specOut4;
};

} // end namespace

#endif
