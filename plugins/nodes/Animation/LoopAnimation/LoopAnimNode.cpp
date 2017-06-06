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
//! \file "LoopAnimNode.cpp"
//! \brief Implementation file for LoopAnimNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date		09.10.2012
//!

#include "LoopAnimNode.h"
#include <QTime>

namespace LoopAnimNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the LoopAnimNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
LoopAnimNode::LoopAnimNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_currentTime(0),
    m_previousTime(0),
    m_multiplyFactor(1.0f),
    m_Interval(1000),
    m_LoopAnimation(NULL),
    m_update(false),
    m_LoopState(true)
{
    // store references to parameters for faster access
    m_LoopAnimation = dynamic_cast<NumberParameter *>( getParameter("LoopAnimation"));
    assert( m_LoopAnimation);

    // Connect change functions of parameters with Slots
	setProcessingFunction("Play", SLOT( onPlay()));

    setChangeFunction("Interval", SLOT( onIntervalChanged()));
    setChangeFunction("Loop begin", SLOT( onIntervalChanged()));
    setChangeFunction("Loop end", SLOT( onIntervalChanged()));

    setChangeFunction("Hold while playing", SLOT( onOptionsChanged()));
    setChangeFunction("Multiply", SLOT( onMultiplyChanged()));

    // store the current interval length and multiply factor
    m_LoopState     = getBoolValue("Play");

    // init values
    onOptionsChanged();
    onIntervalChanged();
    onMultiplyChanged();

	// init play state
	onPlay();

    // perform update on frame change signal
    connect( this, SIGNAL(frameChanged(int)), this, SLOT(onFrameChanged(int)));

    this->setSaveable(true);
}


//!
//! Destructor of the LoopAnimNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
LoopAnimNode::~LoopAnimNode ()
{
}

void LoopAnimNode::onFrameChanged( int n )
{
    if( !m_update)
        return;

    const int time = QTime().msecsTo(QTime::currentTime());
    float normalizedTime = 0.0f;

    AdvanceTime( m_currentTime, time - m_previousTime, m_Interval);
    normalizedTime = m_currentTime / (float) m_Interval;

    m_LoopAnimation->setValue( QVariant( m_multiplyFactor * normalizedTime ), true);

    // check if at end of loop and set time to begin of loop
    if( m_LoopState && m_currentTime > m_loopEnd * m_Interval)
    {
        m_currentTime = m_loopBegin * m_Interval;
    } 
    else if( m_currentTime == m_Interval)
    {
        // skip update if interval reached
        m_LoopAnimation->setValue( QVariant( 0 ), true);
        m_update = false;
    }

    m_previousTime = time;
}

void LoopAnimNode::onPlay()
{
    // either reset&play each time "play" has changed
    if( m_holdToPlay )
    {
        // true => loopIn, false => loopOut
        m_LoopState = getBoolValue("Play");

		if( m_LoopState )
		{
			reset();
		}
	} 
    // .. or only when play is true
    else if( getBoolValue("Play"))
    {
        m_LoopState = !m_LoopState;

		if( m_LoopState )
		{
			reset();
		}
	}
}

void LoopAnimNode::onIntervalChanged()
{
    m_Interval   = getIntValue("Interval");
    m_loopBegin  = getFloatValue("Loop begin");
    m_loopEnd    = getFloatValue("Loop end");
}

void LoopAnimNode::onMultiplyChanged()
{
    m_multiplyFactor = this->getDoubleValue("Multiply");
}

void LoopAnimNode::onOptionsChanged()
{
    m_holdToPlay      = getBoolValue("Hold while playing");
}

void LoopAnimNode::AdvanceTime( int &time, const int &step, const int &max )
{
    time += step;
    if( time > max )
        time = max;
}

void LoopAnimNode::reset()
{
	// reset&play
	m_currentTime = 0;

	// store current time for time delta calculation
	m_previousTime = QTime().msecsTo(QTime::currentTime());

	// start timer
	m_update = true;
}

} // namespace LoopAnimNode 
