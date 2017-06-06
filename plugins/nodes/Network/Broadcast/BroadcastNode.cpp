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
//! \file "BroadcastNode.cpp"
//! \brief Implementation file for MeshNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#include "BroadcastNode.h"
#include "NumberParameter.h"
#include "OgreManager.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDir>

namespace BroadcastNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(BroadcastNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the BroadcastNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
BroadcastNode::BroadcastNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_timer(0),
    m_sequenceIdLow(0),
    m_sequenceIdHigh(0)
{
    // Set animation timer.
    m_timer = new QTimer(this);
    m_udpSocket = new QUdpSocket(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(updateTimer()));

    // Set change functions.
    Parameter *hostAddressParameter = getParameter("hostAddress");
    if (hostAddressParameter)
        hostAddressParameter->setChangeFunction(SLOT(setHostAddress()));
    Parameter *portParameter = getParameter("port");
    if (portParameter)
        portParameter->setChangeFunction(SLOT(setPort()));
    Parameter *intervalParameter = getParameter("interval");
    if (intervalParameter)
        intervalParameter->setChangeFunction(SLOT(setInterval()));
    Parameter *runParameter = getParameter("run");
    if (runParameter)
        runParameter->setChangeFunction(SLOT(toggleRun()));

    setHostAddress();
    setPort();
    setInterval();
}

//!
//! Destructor of the BroadcastNode class.
//!
BroadcastNode::~BroadcastNode (){};

///
/// Public Slots
///

//!
//! Slot which is called when new host address is set.
//!
void BroadcastNode::setHostAddress()
{
    QString hostAddress = getStringValue("hostAddress");
    if (!m_hostAddress.setAddress(hostAddress))
        m_hostAddress.clear();
}

//!
//! Slot which is called when new port is set.
//!
void BroadcastNode::setPort()
{ 
    unsigned int port = getValue("port").toUInt();
    if (port < 65536)
        m_port = port;
}

//!
//! Slot which is called when new update interval is set.
//!
void BroadcastNode::setInterval()
{
    unsigned int interval = getValue("interval").toUInt();
    bool isRunning = getBoolValue("run");
    if (isRunning) {
        m_timer->stop();
        m_timer->start(interval);
    }
}

//!
//! Slot which is called when running flag on node is toggled.
//!
void BroadcastNode::toggleRun()
{
    bool run = getBoolValue("run");
    unsigned int interval = getValue("interval").toUInt();
    if (run) {
        m_sequenceIdLow = 0;
        m_sequenceIdHigh = 0;
        m_timer->stop();
        m_timer->start(interval);
    }
    else
        m_timer->stop();
}

//!
//! Slot which is called on timer timeout.
//!
void BroadcastNode::updateTimer ()
{
    QByteArray datagram;
    datagram.append((char) 128);
    datagram.append((char) 105);
    
    if (m_sequenceIdLow < 255)
        m_sequenceIdLow++;
    else {
        m_sequenceIdLow = 0;
        if(m_sequenceIdHigh < 255)
            m_sequenceIdHigh++;
        else
            m_sequenceIdHigh = 0;
    }

    datagram.append((char) m_sequenceIdHigh);
    datagram.append((char) m_sequenceIdLow);

    datagram.append((char) 0);
    datagram.append((char) 0);
    datagram.append((char) 0);
    datagram.append((char) 0);
    datagram.append((char) 0);
    datagram.append((char) 0);
    datagram.append((char) 0);
    datagram.append((char) 0);

    //std::cout << datagram.size() << std::endl;

    ParameterGroup *parameterGroup = getParameterRoot();
    QList<AbstractParameter *> parameterList = parameterGroup->filterParameters("", true, true);
    for (int i = 0; i < parameterList.size(); ++i)
    {
        Parameter *parameter = (Parameter*) parameterList[i];
        if (!parameter)
            continue;
        QString parameterName = parameter->getName();
        if (parameterName.left(5) == "input") {
            if (parameter->getType() == Parameter::T_Float) {
                double value = parameter->getValue(true).toDouble();
                float floatValue = (float) value;
                //datagram.append((char*) &floatValue, sizeof(float));
                datagram += QByteArray::number(value, 'f', 4);
            }
            if (parameter->getType() == Parameter::T_UnsignedInt) {
                int value = parameter->getValue(true).toInt();
                datagram += QByteArray::number(value);
            }
        }
    }
    m_udpSocket->writeDatagram(datagram.data(), datagram.size(), m_hostAddress, m_port);
}

} // namespace BroadcastNode 

