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
//! \file "TriggerAnimNode.cpp"
//! \brief Implementation file for TriggerAnimNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       16.12.2011 (last updated)
//!

#include "TriggerAnimNode.h"

#include <QTime>

namespace TriggerAnimNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the TriggerAnimNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TriggerAnimNode::TriggerAnimNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    mpNormalizedTime(NULL),
    mIsPlaying(false),
    mMultiplyFactor(1.0f),
    mInterval(1000),
    mBaseTime(0),
    mPreviousTime(0)
{
    // store references to parameters for faster access
    mpNormalizedTime  = dynamic_cast<NumberParameter *>(getParameter("NormalizedTime"));
    assert(mpNormalizedTime);

    // Connect change functions of parameters with Slots
    setChangeFunction("Interval", SLOT( onIntervalChanged()));
    setChangeFunction("Multiply", SLOT( onMultiplyChanged()));
	setProcessingFunction("Play",     SLOT( onPlayTriggered()));
    setProcessingFunction("Loop",     SLOT( onLoopTriggered()));
	setChangeFunction("Always Reset to Zero", SLOT( onResetChanged()));

    // use frame change to trigger update
    connect( this, SIGNAL(frameChanged(int)), this, SLOT(onFrameChanged()));

    // Add affections for the dirty-chain
    //this->addAffection("Play", "NormalizedTime");
    //this->addAffection("Loop", "NormalizedTime");

    // store the current interval length and multiply factor
    mInterval = getIntValue("Interval");
    mMultiplyFactor = getDoubleValue("Multiply");
	mIsPlaying = getBoolValue("Play") || getBoolValue("Loop");

	mReset = getBoolValue("Always Reset to Zero");
	mIsFinished = getBoolValue("Is Finished");

    this->setSaveable(true);
}


//!
//! Destructor of the TriggerAnimNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TriggerAnimNode::~TriggerAnimNode ()
{
}

void TriggerAnimNode::onFrameChanged()
{
    // calculate current time delta between frames 
    int currentTime = QTime().msecsTo(QTime::currentTime());

    // Only perform update if playing..
    if( mIsPlaying )
    {
        // check if base time within interval
        if( mBaseTime > mInterval )
        {
            // Stop Animation only if not in loop
            mIsPlaying = this->getBoolValue("Loop", true);
            
			if(mReset)
				Reset();
        }

        // set new value and propagate
        float normalizedTime = mMultiplyFactor * mBaseTime / (float) mInterval;

        // Set output parameter and propagate
        mpNormalizedTime->setValue( QVariant( normalizedTime ), true);

        // advance base time
        mBaseTime += currentTime - mPreviousTime;
    }

    mPreviousTime = currentTime;
}

void TriggerAnimNode::onPlayTriggered()
{
	Reset();

    if( !mIsPlaying && this->getBoolValue("Play"))
    {
        mIsPlaying = true;
		setValue("Is Finished", false, true);
    }
}

void TriggerAnimNode::onLoopTriggered()
{
    // Disable "Loop" should stop the animation immediately
      mIsPlaying = ( !mIsPlaying || this->getBoolValue("Loop"));

    Reset();

    // set output value to zero when loop canceled
    if( !mIsPlaying )
	{
        mpNormalizedTime->setValue( QVariant( 0.0f ), true);
	}
}

void TriggerAnimNode::onIntervalChanged()
{
    // update timer interval
    mInterval = this->getIntValue("Interval");
}

void TriggerAnimNode::onMultiplyChanged()
{
    // update timer interval
    mMultiplyFactor = this->getDoubleValue("Multiply");
}

void TriggerAnimNode::Reset()
{
    mBaseTime = 0.0f;

	if (!mIsPlaying)
		setValue("Is Finished", true, true);
	else
		setValue("Is Finished", false, true);
}

void TriggerAnimNode::onResetChanged()
{
	mReset = this->getBoolValue("Always Reset to Zero");
}

} // namespace TriggerAnimNode 
