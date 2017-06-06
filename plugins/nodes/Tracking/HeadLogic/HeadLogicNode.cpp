/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2009 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "HeadLogicNode.h"
//! \brief Implementation file for HeadLogicNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.04.2009 (last updated)
//!

#include "HeadLogicNode.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <iostream>

namespace HeadLogicNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!

//! Constructor of the HeadLogicNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
HeadLogicNode::HeadLogicNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_timer(0),
    m_noFaceDetected(false),
    m_isSleeping(false),
    m_isStaring(false),
    m_isWakingUp(false),
    m_sleepingWeight(0.0),
    m_sleepingTimeLimit(QTime::currentTime())
{
    addAffection("trackX", "left");
    addAffection("trackX", "right");
    addAffection("trackX", "up");
    addAffection("trackX", "down");

    addAffection("trackY", "left");
    addAffection("trackY", "right");
    addAffection("trackY", "up");
    addAffection("trackY", "down");

    setProcessingFunction("left", SLOT(processOutputParameter()));
    setProcessingFunction("right", SLOT(processOutputParameter()));
    setProcessingFunction("up", SLOT(processOutputParameter()));
    setProcessingFunction("down", SLOT(processOutputParameter()));

    addAffection("smileIn", "smileOut");
    addAffection("angerIn", "angerOut");
    addAffection("mouthOpenIn", "mouthOpenOut");

    setProcessingFunction("smileOut", SLOT(processOutputParameter()));
    setProcessingFunction("angerOut", SLOT(processOutputParameter()));
    setProcessingFunction("mouthOpenOut", SLOT(processOutputParameter()));

    Parameter *faceDetectedParameter = getParameter("faceDetected");
    Parameter *genderParameter = getParameter("gender");
    Parameter *weightOtherParameter = getParameter("weightOtherClips");
    Parameter *playSleepingParameter = getParameter("playSleeping");

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), SLOT(updateTimer()));
    m_timer->start(33);
}


//!
//! Destructor of the HeadLogicNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
HeadLogicNode::~HeadLogicNode ()
{
}


///
/// Public Methods
///


//!
//! Processes the output parameter depending on its name .
//!
void HeadLogicNode::processOutputParameter ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;

    const QString &parameterName = parameter->getName();
    
    double trackX = getValue("trackX").toDouble();
    double trackY = getValue("trackY").toDouble();

    bool flipX = getValue("flipX").toBool();
    bool flipY = getValue("flipY").toBool();

    double multX = getValue("multX").toDouble();
	double multY = getValue("multY").toDouble();

    double clampX = getValue("clampX").toDouble();
    if ( clampX > 1.0 ) clampX = 1.0;
    else if ( clampX < 0.0 ) clampX = 0.0;
    double clampY = getValue("clampY").toDouble();
    if ( clampY > 1.0 ) clampY = 1.0;
    else if ( clampY < 0.0 ) clampY = 0.0;

    if(parameterName == "left" || parameterName == "right")
    {
        if ( trackX > 0.5 && trackX <= clampX )
        {
            if ( flipX )
            {
                setValue("right", QVariant((trackX - 0.5) * 2.0 * multX * 100.0 * (1 - m_sleepingWeight)));
                setValue("left", QVariant(0.0));
            }
            else
            {
                setValue("left", QVariant((trackX - 0.5) * 2.0 * multX * 100.0 * (1 - m_sleepingWeight)));
                setValue("right", QVariant(0.0));
            }
        }
        else if ( trackX >= (1 - clampX) && trackX <= 0.5 )
        {
            if ( flipX )
            {
                setValue("right", QVariant(0.0));
                setValue("left", QVariant((1 - trackX * 2.0) *  multX * 100.0 * (1 - m_sleepingWeight)));
            }
            else
            {
                setValue("left", QVariant(0.0));
                setValue("right", QVariant((1 - trackX * 2.0) * multX * 100.0 * (1 - m_sleepingWeight)));
            }

        }
    }
    else if(parameterName == "up" || parameterName == "down")
    {
        if ( trackY > (1 - clampY) && trackY <= 0.5 )
        {
            if ( flipY )
            {
                setValue("up", QVariant(0.0));
                setValue("down", QVariant((1 - trackY * 2.0) * multY * 100.0 * (1 - m_sleepingWeight)));
            }
            else
            {
                setValue("down", QVariant(0.0));
                setValue("up", QVariant((1 - trackY * 2.0) * multY * 100.0 * (1 - m_sleepingWeight)));
            }
        }
        else if ( trackY > 0.5 && trackY <= clampY )
        {
            if ( flipY )
            {
                setValue("up", QVariant((trackY - 0.5) * 2.0 * multY * 100.0 * (1 - m_sleepingWeight)));
                setValue("down", QVariant(0.0));
            }
            {
                setValue("down", QVariant((trackY - 0.5) * 2.0 * multY * 100.0 * (1 - m_sleepingWeight)));
                setValue("up", QVariant(0.0));
            }
        }
    }
    else if(parameterName == "mouthOpenOut")
    {
        if (m_isStaring)
            setValue("mouthOpenOut", 0.0);
        else
            setValue("mouthOpenOut", getValue("mouthOpenIn").toDouble() * (1 - m_sleepingWeight));
    }
    else if(parameterName == "smileOut")
    {
        if (m_isStaring)
            setValue("smileOut", 0.0);
        else
            setValue("smileOut", getValue("smileIn").toDouble() * (1 - m_sleepingWeight));
    }
    else if(parameterName == "angerOut")
    {
        setValue("angerOut", getValue("angerIn").toDouble() * (1 - m_sleepingWeight));
    }
}

///
/// Private Methods
///

//!
//! Update sleeping animation.
//!
void HeadLogicNode::updateSleep()
{
    int sleepingDelay = getIntValue("sleepingDelay", true);
    bool noFaceDetected = getValue("noFaceDetected", true).toBool();
    if (!m_isSleeping && !m_noFaceDetected && noFaceDetected) {
        m_noFaceDetected = true;
        m_sleepingTimeLimit = QTime::currentTime();
        m_sleepingTimeLimit = m_sleepingTimeLimit.addMSecs(sleepingDelay);
    }
    else if (!m_isSleeping && m_noFaceDetected && !noFaceDetected) {
        m_noFaceDetected = false;
    }
    else if (!m_isSleeping && m_noFaceDetected && QTime::currentTime() > m_sleepingTimeLimit) {
        m_isSleeping = true;
        setValue("playSleeping", true, true);
    }
    
    if (m_isSleeping && m_noFaceDetected && !noFaceDetected) {
        m_noFaceDetected = false;
        m_sleepingTimeLimit = QTime::currentTime();
        m_sleepingTimeLimit = m_sleepingTimeLimit.addMSecs(sleepingDelay);
    }
    else if (m_isSleeping && !m_noFaceDetected && noFaceDetected) {
        m_noFaceDetected = true;
    }
    else if (m_isSleeping && !m_noFaceDetected && !noFaceDetected && QTime::currentTime() > m_sleepingTimeLimit) {
        m_isWakingUp = true;
        m_isSleeping = false;
    }
    else if (m_isWakingUp) {
        if (m_sleepingWeight == 0.0) {
            setValue("playSleeping", false, true);
            m_isWakingUp = false;
        }
    }

    double sleepingStep = getValue("sleepingStep").toDouble();
    if (m_isSleeping) {
        m_sleepingWeight += sleepingStep;
        if (m_sleepingWeight > 1.0)
            m_sleepingWeight = 1.0;
        setValue("sleepingWeight", m_sleepingWeight, true);
        setValue("weightOtherClips", 1.0 - m_sleepingWeight, true);
		setValue("up", getValue("up").toDouble() * (1.0 - m_sleepingWeight), true);
		setValue("down", getValue("down").toDouble() * (1.0 - m_sleepingWeight), true);
		setValue("left", getValue("left").toDouble() * (1.0 - m_sleepingWeight), true);
		setValue("right", getValue("right").toDouble() * (1.0 - m_sleepingWeight), true);
        setValue("mouthOpenOut", getValue("mouthOpenIn").toDouble() * (1.0 - m_sleepingWeight), true);
        setValue("smileOut", getValue("smileIn").toDouble() * (1.0 - m_sleepingWeight), true);
    }
    else {
        m_sleepingWeight -= sleepingStep;
        if (m_sleepingWeight < 0.0)
            m_sleepingWeight = 0.0;
        setValue("sleepingWeight", m_sleepingWeight, true);
        setValue("weightOtherClips", 1.0 - m_sleepingWeight, true);
    }
}

//!
//! Update staring animation.
//!
void HeadLogicNode::updateStaring()
{
    double sleepingStep = getValue("sleepingStep").toDouble();
    if (!m_isStaring) {
        double anger = getDoubleValue("angerIn", true);
        if (anger > 80.0) {
            m_isStaring = true;
            setValue("playStaring", true, true);
        }
    }
    else {
        setValue("playStaring", false, true);
        double isStaring = getValue("isStaring", true).toBool();
        if (!isStaring)
            m_isStaring = false;
    }
}

///
/// Private Slots
///


//!
//! On timer timeout.
//!
void HeadLogicNode::updateTimer ()
{
    //if (!m_isStaring)
        updateSleep();
    //if (!m_isSleeping)
        updateStaring();
}

} // namespace HeadLogicNode 
