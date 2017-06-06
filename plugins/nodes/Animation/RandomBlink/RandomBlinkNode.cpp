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
//! \file RandomBlinkNode.cpp
//! \date 02.04.2008 10:47:19 (GMT+2)
//! \version 1.0
//!

#include "RandomBlinkNode.h"


#include <QtCore/QFile>
#include <QtCore/QDir>

#include <cstdio>       // for sprintf_s in RandomBlinkNode::ShowDebugInfo()

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

namespace RandomBlinkNode {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the RandomBlinkNode class.
//!
//! \param name The name to give the new mesh node.
//!
//! \todo Better use std::string instead of QString
RandomBlinkNode::RandomBlinkNode ( QString name, ParameterGroup *parameterRoot ) :
Node(name, parameterRoot),
m_timestamp(0),
m_lastTimestamp(0),
m_updown(1),
m_blinkActive(false),
m_timer(0),
randomAmplitude(0)
{
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), SLOT(timerUpdate()));
    m_timer->start(20);
}


///
/// Public Destructors
///


//!
//! Destructor of the RandomBlinkNode class.
//!
RandomBlinkNode::~RandomBlinkNode ()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
}

///
/// Public Slots
///

void RandomBlinkNode::timerUpdate()
{
    if ( !m_blinkActive )
    {
        int freq = getIntValue("freq");
        if (freq > 0) {
            int rand = std::rand() % freq + 1;
            if ( rand == 1 )
            {
                m_blinkActive = true;
				
				// "Fire" start of active blinking (only once for every blink)
				setValue("fire", true, true);

            }
        }
	} else {
		// reset fire, if this is not the first process of blink
		setValue("fire", false, true);
	}

    if(!m_blinkActive)
        return;

	double progress = getDoubleValue("progress");
    double stepsize = getDoubleValue("stepsize");
	float amplitude = getDoubleValue("amplitude", true);
	float amplitudeRandom = getDoubleValue("amplitudeRandomFactor", true);

if( m_updown == 1 )
    {
		if((amplitude += randomAmplitude) > 1.0) amplitude = 1.0;
        if(progress + m_updown * stepsize >= amplitude *100)
        {
            progress = 100*amplitude;
            m_updown = -1;
        }
        else
            progress += m_updown * stepsize;
    }
    else if( m_updown == -1 )
    {
        if( progress + m_updown * stepsize <= 0.0 )
        {
            progress = 0.0;
            m_updown = 1;
            m_blinkActive = false;

			// set the next randomAmplitude
			if (amplitudeRandom){
				randomAmplitude = amplitude*amplitudeRandom*200;
				randomAmplitude = std::rand() % static_cast<int>(randomAmplitude);
				randomAmplitude -= (amplitude*amplitudeRandom*100);
				randomAmplitude /= 100;
			}
        }
        else
            progress += m_updown * stepsize;
    }

    double weight = getDoubleValue("weight", true);
    setValue("progress", progress * weight, true);
}

} // namespace RandomBlinkNode
