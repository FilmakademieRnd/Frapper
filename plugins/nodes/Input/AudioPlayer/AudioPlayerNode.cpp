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
//! \file "AudioPlayerNode.cpp"
//! \brief Implementation file for AudioPlayerNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       13.01.2013 (last updated)
//!

#include "AudioPlayerNode.h"
#include "Log.h"

#include <math.h>

#include <QTimerEvent>
#include <QTime>

namespace AudioPlayerNode {
	using namespace Frapper;

#define TIME_INTERVAL 1000	// 1/sec
#define SPEC_INTERVAL 20	// 50/sec

INIT_INSTANCE_COUNTER(AudioPlayerNode)


	///
	/// Constructors and Destructors
	///


	//!
	//! Constructor of the AudioPlayerNode class.
	//!
	//! \param name The name for the new node.
	//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
	//!
	AudioPlayerNode::AudioPlayerNode ( const QString &name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
	m_specTimerId(-1),
	m_timeTimerId(-1),
	m_specOut1(getNumberParameter("Spec1"), getIntValue("S1 Upper Bound"), getIntValue("S1 Lower Bound")),
	m_specOut2(getNumberParameter("Spec2"), getIntValue("S2 Upper Bound"), getIntValue("S2 Lower Bound")),
	m_specOut3(getNumberParameter("Spec3"), getIntValue("S3 Upper Bound"), getIntValue("S3 Lower Bound")),
	m_specOut4(getNumberParameter("Spec4"), getIntValue("S4 Upper Bound"), getIntValue("S4 Lower Bound"))
{
	setChangeFunction("Audio File", SLOT(loadAudioFile()));

	setCommandFunction("Play", SLOT(controlAudioFile()));
	setCommandFunction("Pause", SLOT(controlAudioFile()));
	setCommandFunction("Stop", SLOT(controlAudioFile()));
	setProcessingFunction("Play Track", SLOT(controlAudioFile()));
	setProcessingFunction("Stop Track", SLOT(controlAudioFile()));
	setProcessingFunction("Volume", SLOT(controlVolumeFile()));

	setChangeFunction("S1 Upper Bound",  SLOT(setBounds()));
	setChangeFunction("S2 Upper Bound",  SLOT(setBounds()));
	setChangeFunction("S3 Upper Bound",  SLOT(setBounds()));
	setChangeFunction("S4 Upper Bound",  SLOT(setBounds()));

	setChangeFunction("S1 Lower Bound",  SLOT(setBounds()));
	setChangeFunction("S2 Lower Bound",  SLOT(setBounds()));
	setChangeFunction("S3 Lower Bound",  SLOT(setBounds()));
	setChangeFunction("S4 Lower Bound",  SLOT(setBounds()));

	// initialize BASS
	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
		Log::error("Can't initialize device", "AudioPlayerNode::AudioPlayerNode");
	}



	INC_INSTANCE_COUNTER
}


//!
//! Destructor of the AudioPlayerNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
AudioPlayerNode::~AudioPlayerNode ()
{
	killTimer(m_timeTimerId);
	killTimer(m_specTimerId);
	BASS_Free();
	DEC_INSTANCE_COUNTER
}


///
/// Private Functions
///


///
/// Private Slots
///


//!
//! Loads a Bass conform audio data file.
//!
//! Is called when the value of the audio file parameter has changed.
//!
//! \return true if the audio file data was successfully loaded from file, false otherwise.
//!
bool AudioPlayerNode::loadAudioFile ()
{
	FilenameParameter *parameter = dynamic_cast<FilenameParameter *>(sender());
	if (!parameter) 
		return false;

	const QString path = parameter->getValue().toString();

	if ( !(m_chan = BASS_StreamCreateFile(0, path.toStdString().c_str(), 0, 0, BASS_SAMPLE_LOOP)) &&
		!(m_chan = BASS_MusicLoad(0, path.toStdString().c_str(), 0, 0, BASS_MUSIC_RAMP|BASS_SAMPLE_LOOP, 1)) ) {
			Log::error(QString("Could not load the audio file %1.").arg(path), "AudioPlayerNode::loadAudioFile");

			return false; // Can't load the file
	}

	return true;
}

//!
//! Starts or stops the audio playback.
//!
void AudioPlayerNode::controlAudioFile ()
{
	if (m_chan) {
		const QString name = dynamic_cast<Parameter *>(sender())->getName();
		const bool loop = getBoolValue("Loop");

		if (name == "Play") {
			if (BASS_ChannelIsActive(m_chan)) {
				killTimer(m_timeTimerId);
				killTimer(m_specTimerId);
				BASS_ChannelStop(m_chan);
			}
			else {
				BASS_ChannelPlay(m_chan, loop);
				m_timeTimerId = startTimer(TIME_INTERVAL);
				m_specTimerId = startTimer(SPEC_INTERVAL);
			}
		}
		// boolean value for play
		else if (name == "Play Track") {
			bool play = getParameter("Play Track")->getValue().toBool();
			if (play)
			{
				BASS_ChannelPlay(m_chan, loop);
				m_timeTimerId = startTimer(TIME_INTERVAL);
				m_specTimerId = startTimer(SPEC_INTERVAL);
			}
			else {				
				killTimer(m_timeTimerId);
				killTimer(m_specTimerId);
				BASS_ChannelStop(m_chan);			
			}
		}
		// end
		else {
			BASS_ChannelStop(m_chan);
			BASS_ChannelSetPosition(m_chan, 0, BASS_POS_BYTE);
			killTimer(m_timeTimerId);
			killTimer(m_specTimerId);
			getParameter("Time")->setValue(QString("%1:%2:%3").arg(0).arg(0, 2, 10, QLatin1Char('0')).arg(0, 2, 10, QLatin1Char('0')));
		}
	}
}

//!
//! Starts or stops the audio playback.
//!
void AudioPlayerNode::controlVolumeFile ()
{
	if (m_chan) {
		const QString name = dynamic_cast<Parameter *>(sender())->getName();

		if (name == "Volume") {
			float volumeValue = getParameter("Volume")->getValue().toFloat();
			//BASS_SetVolume(volumeValue);
			BASS_ChannelSetAttribute(m_chan, BASS_ATTRIB_VOL, volumeValue);
		}
	}
}
//!
//! Sets the bounds for the analizer.
//!
void AudioPlayerNode::setBounds ()
{
	NumberParameter *parameter = dynamic_cast<NumberParameter *>(sender());
	const QString name = parameter->getName();
	const int value = parameter->getValue().toInt();

	specData *data = 0;

	if (name.contains("S1"))
		data = &m_specOut1;
	else if (name.contains("S2"))
		data = &m_specOut2;
	else if (name.contains("S3"))
		data = &m_specOut3;
	else if (name.contains("S4"))
		data = &m_specOut4;

	if (data)
		if (name.contains("Upper"))
			data->upper = value;
		else if (name.contains("Lower"))
			data->lower = value;
}


///
/// Protected Events
///


//!
//! Handles timer events for the Node.
//!
//! \param event The description of the timer event.
//!
void AudioPlayerNode::timerEvent ( QTimerEvent *event )
{
	const int id = event->timerId();

	if (id == m_specTimerId) {
		float fft[128];
		BASS_ChannelGetData(m_chan,fft,BASS_DATA_FFT256); // get the FFT data

		register float value = 0.0f;
		for (int i=m_specOut1.lower; i<m_specOut1.upper; ++i)
			value += sqrt(fft[i])*3.0f;
		m_specOut1.parameter->setValue(QVariant(value/(m_specOut1.upper-m_specOut1.lower)), true);

		value = 0.0f;
		for (int i=m_specOut2.lower; i<m_specOut2.upper; ++i)
			value += sqrt(fft[i])*3.0f;
		m_specOut2.parameter->setValue(QVariant(value/(m_specOut1.upper-m_specOut1.lower)), true);

		value = 0.0f;
		for (int i=m_specOut3.lower; i<m_specOut3.upper; ++i)
			value += sqrt(fft[i])*3.0f;
		m_specOut3.parameter->setValue(QVariant(value/(m_specOut1.upper-m_specOut1.lower)), true);

		value = 0.0f;
		for (int i=m_specOut4.lower; i<m_specOut4.upper; ++i)
			value += sqrt(fft[i])*3.0f;
		m_specOut4.parameter->setValue(QVariant(value/(m_specOut1.upper-m_specOut1.lower)), true);
	}
	else if (id == m_timeTimerId) {
		const QWORD pos = BASS_ChannelGetPosition(m_chan, BASS_POS_BYTE);
		float time = BASS_ChannelBytes2Seconds(m_chan, pos);
		const int seconds = ((int) time) % 60;
		time /= 60.0f;
		const int minutes = ((int) time) % 60;
		time /= 60.0f;
		const int hours = (int) time;

		getParameter("Time")->setValue(QString("%1:%2:%3").arg(hours).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')));
	}
}

} // end namespace
