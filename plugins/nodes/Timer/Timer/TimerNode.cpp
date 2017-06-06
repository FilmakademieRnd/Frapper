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
//! \file "TimerNode.cpp"
//! \brief Implementation file for TimerNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       16.12.2011 (last updated)
//!

#include "TimerNode.h"
#include <QDateTime>
#include <QTimer>

namespace TimerNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the TimerNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TimerNode::TimerNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
        m_pTimer(NULL),
        m_pSystemTime(NULL),
        m_pNormalizedTime(NULL),
        m_pNormalizedTimeInterval(NULL),
        m_pUpdate(NULL),
        m_baseTime(0)
{
    // store references to parameters for faster access
    m_pSystemTime = dynamic_cast<NumberParameter *>(getParameter("SystemTime"));
    assert(m_pSystemTime);

    m_pNormalizedTime = dynamic_cast<NumberParameter *>(getParameter("NormalizedTime"));
    assert(m_pNormalizedTime);

    m_pNormalizedTimeInterval = dynamic_cast<NumberParameter *>(getParameter("NormalizedTimeInterval"));
    assert(m_pNormalizedTimeInterval);

    m_pUpdate = getParameter("Update");
    assert(m_pUpdate);

    // Override default range given by NumberParameter
    m_pSystemTime->setMinValue(0);
    m_pSystemTime->setMaxValue(std::numeric_limits<int>::max()); // STL needs to be used here, as there is no analogon in Qt

    // create a new QTimer
    m_pTimer = new QTimer(this);
    connect (m_pTimer, SIGNAL(timeout()), this, SLOT(timerEvent()));

	// create system time element
	m_sysTime = new QTime();
	m_sysTime->start();

    // Connect change functions of parameters with timer activation
	setChangeFunction("Active", SLOT(activateTimer()));

	// Connect change function for interval change
	setChangeFunction("Interval", SLOT(changeInterval()));

	// Connect change function for interval change
	setChangeFunction("PerSecond", SLOT(changePerSecond()));

	setProcessingFunction("switch", SLOT(activateTimer()));
}


//!
//! Destructor of the TimerNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TimerNode::~TimerNode ()
{
    delete m_pTimer;
}

void TimerNode::timerEvent()
{
    // get current system time in msec
    const int currentTime = m_sysTime->msecsTo(QTime::currentTime());

    // check if current time is within range of the normalized time
    const int normalizedTimeInterval = m_pNormalizedTimeInterval->getValue().toInt();
	int time = currentTime - m_baseTime;
    if ((time) > normalizedTimeInterval ) {
        m_baseTime = currentTime;
		time = currentTime - m_baseTime;
	}

    const float normalizedTime = time / (float) normalizedTimeInterval;

	// write times to parameters
    m_pSystemTime->setValue(QVariant(currentTime), true);
    m_pNormalizedTime->setValue(QVariant(normalizedTime), true);

    // trigger update chain
    m_pUpdate->getValue(true);
}

void TimerNode::activateTimer()
{
    bool value = dynamic_cast<Parameter *>(sender())->getValue().toBool();
    if( value ) {
        m_pTimer->start(getIntValue("Interval"));
	}
	else {
        m_pTimer->stop();
        m_baseTime = 0;
    }
}

void TimerNode::changeInterval()
{
	const int interval = getIntValue("Interval");
	setValue("PerSecond", (int)(1000.f/interval));
	m_pTimer->setInterval(interval);
}

void TimerNode::changePerSecond()
{
	const int interval = 1000.f/getIntValue("PerSecond");
	setValue("Interval", interval);
	m_pTimer->setInterval(interval);
}

} // namespace TimerNode 
