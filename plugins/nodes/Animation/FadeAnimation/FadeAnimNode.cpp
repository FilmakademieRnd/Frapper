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
//! \file "FadeAnimNode.cpp"
//! \brief Implementation file for FadeAnimNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date		09.10.2012
//!

#include "FadeAnimNode.h"
#include <QTime>

namespace FadeAnimNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the FadeAnimNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
FadeAnimNode::FadeAnimNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_currentTime(0),
    m_previousTime(0),
    m_multiplyFactor(1.0f),
    m_fadeInInterval(1000),
    m_fadeOutInterval(1000),
    m_FadeIn(NULL),
    m_FadeOut(NULL),
    m_update(false),
    m_FadeOutFromHalf(false),
    m_FadeInState(true)
{
    // store references to parameters for faster access
    m_FadeIn = dynamic_cast<NumberParameter *>( getParameter("FadeInAnimation"));
    assert( m_FadeIn);

    m_FadeOut = dynamic_cast<NumberParameter *>( getParameter("FadeOutAnimation"));
    assert( m_FadeOut);

    // Connect change functions of parameters with Slots
    setChangeFunction("FadeIn Interval", SLOT( onIntervalChanged()));
    setChangeFunction("FadeOut Interval", SLOT( onIntervalChanged()));

    setChangeFunction("Use FadeInAnimation only", SLOT( onOptionsChanged()));
    setChangeFunction("FadeOut by 0.5", SLOT( onOptionsChanged()));
    setChangeFunction("Hold while playing", SLOT( onOptionsChanged()));

    setChangeFunction("Multiply", SLOT( onMultiplyChanged()));
    setProcessingFunction("Play", SLOT( onPlay()));

    // store the current interval length and multiply factor
    m_FadeInState     = getBoolValue("Play");

    // init values
    onOptionsChanged();
    onIntervalChanged();
    onMultiplyChanged();

    // perform update on frame change signal
    connect( this, SIGNAL(frameChanged(int)), this, SLOT(onFrameChanged(int)));

    this->setSaveable(true);
}


//!
//! Destructor of the FadeAnimNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
FadeAnimNode::~FadeAnimNode ()
{
}

void FadeAnimNode::onFrameChanged( int n )
{
    if( !m_update)
        return;

    const int time = QTime().msecsTo(QTime::currentTime());
    float normalizedTime = 0.0f;

    if( m_FadeInState )
    {
        AdvanceTime( m_currentTime, time - m_previousTime, m_fadeInInterval);
        normalizedTime = m_currentTime / (float) m_fadeInInterval;

        // FadeIn/-Out in half the time
        if( m_FadeOutFromHalf )
            normalizedTime *= 0.5;

        m_FadeIn->setValue( QVariant( m_multiplyFactor * normalizedTime ), true);
        m_FadeOut->setValue( QVariant( 0), true);

        // check if at end of loop and set time to begin of loop
        if( m_currentTime == m_fadeInInterval)
        {
            // skip update if interval reached
            m_update = false;
        }
    }
    else
    {
        AdvanceTime( m_currentTime, time - m_previousTime, m_fadeOutInterval);
        normalizedTime = m_currentTime / (float) m_fadeOutInterval;

        // FadeIn/-Out in half the time
        if( m_FadeOutFromHalf )
            normalizedTime *= 0.5;

        if( m_UseFadeInAsOut ) 
        {
            if( m_FadeOutFromHalf )
                // FadeOut is from 0.5 to 1.0
                m_FadeIn->setValue( QVariant( m_multiplyFactor * (0.5 + normalizedTime)), true);
            else
                // FadeOut = FadeIn backwards
                m_FadeIn->setValue( QVariant( m_multiplyFactor * (1.0f-normalizedTime)), true);
        } 
        else 
        {
            m_FadeOut->setValue( QVariant( m_multiplyFactor * normalizedTime ), true);
            m_FadeIn->setValue( QVariant( 0), true);
        }

        if( m_currentTime == m_fadeOutInterval)
        {
            // skip update if interval reached
            m_update = false;

            // reset fade-out animation after cycle completed
            if( m_UseFadeInAsOut)
                m_FadeIn->setValue( QVariant( 0), true);
            else
                m_FadeOut->setValue( QVariant( 0), true);
        }

    }

    m_previousTime = time;
}

void FadeAnimNode::onPlay()
{
    bool playIt = false;

    // either reset&play each time "play" has changed
    if( m_holdToPlay )
    {
        // true => fadeIn, false => fadeOut
        m_FadeInState = getBoolValue("Play");
        playIt = true;
    } 
    // .. or only when play is true
    else if( getBoolValue("Play"))
    {
        m_FadeInState = !m_FadeInState;
        playIt = true;
    }

    if( playIt )
    {
        // reset&play
        m_currentTime = 0;       
       
        // store current time for time delta calculation
        m_previousTime = QTime().msecsTo(QTime::currentTime());

        // start timer
        m_update = true;
    }
}

void FadeAnimNode::onIntervalChanged()
{
    m_fadeInInterval = this->getIntValue("FadeIn Interval");
    m_fadeOutInterval = this->getIntValue("FadeOut Interval");
    m_fadeIntervalTotal = m_fadeInInterval + m_fadeOutInterval;
}

void FadeAnimNode::onMultiplyChanged()
{
    m_multiplyFactor = this->getDoubleValue("Multiply");
}

void FadeAnimNode::onOptionsChanged()
{
    m_UseFadeInAsOut  = getBoolValue("Use FadeInAnimation only");
    m_FadeOutFromHalf = getBoolValue("FadeOut by 0.5");
    m_holdToPlay      = getBoolValue("Hold while playing");
}

void FadeAnimNode::AdvanceTime( int &time, const int &step, const int &max )
{
    time += step;
    if( time > max )
        time = max;
}

} // namespace FadeAnimNode 
