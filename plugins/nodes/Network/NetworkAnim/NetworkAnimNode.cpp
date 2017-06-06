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
//! \file "NetworkAnimNode.cpp"
//! \brief Implementation file for NetworkAnimNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       07.12.2010 (last updated)
//!

#include "NetworkAnimNode.h"
#include "NumberParameter.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QTime>

namespace NetworkAnimNode {
using namespace Frapper;

INIT_INSTANCE_COUNTER(NetworkAnimNode)


///
/// Constructors and Destructors
///


//!
//! Constructor of the NetworkAnimNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
NetworkAnimNode::NetworkAnimNode ( QString name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot),
    m_timer(0),
    m_currentFrame(-1),
    m_udpSocket(0)
{   
    setChangeFunction("run", SLOT(toggleRun()));
    setChangeFunction("interval", SLOT(setInterval()));
    m_timer = new QTimer(this);    
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

//!
//! Destructor of the NetworkAnimNode class.
//!
NetworkAnimNode::~NetworkAnimNode ()
{
    m_timer->stop();
    m_animationChunks.clear();
    quit();
    wait();
}

///
/// Public Slots
///

//!
//! Function which is run in new thread.
//!
void NetworkAnimNode::run()
{
    unsigned int port = getValue("port").toUInt();
    m_udpSocket = new QUdpSocket();
    m_udpSocket->bind(port);
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()), Qt::DirectConnection);
    connect(this, SIGNAL(finished()), this, SLOT(threadFinished()), Qt::DirectConnection);
    exec();
}

///
/// Private Slots
///


//!
//! Slot which is called when new update interval is set.
//!
void NetworkAnimNode::setInterval()
{
    unsigned int interval = getValue("interval").toUInt();
    bool run = getBoolValue("run");
    m_timer->stop();
    if (run) {
        m_timer->start(interval);
    }
}

//!
//! Slot which is called when running flag on node is toggled.
//!
void NetworkAnimNode::toggleRun()
{
    bool run = getBoolValue("run");
    setInterval();
    if (run || !isRunning()) {
        start();   
    }
    else {
        quit();
    }
   
}

//!
//! Slot which is called when new datagrams are availabled on udp socket.
//!
void NetworkAnimNode::processPendingDatagrams()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        QMap<QString, float> animationChunk;
        QString payload(datagram);
        QStringList animationData = payload.split(" ");
        if (animationData.size() == 3) {
            int frame = animationData[0].toInt();
            QString animationName = animationData[1];
            float animationProgress = animationData[2].toFloat();
            if (frame != m_currentFrame) {
                m_animationChunks.enqueue(QMap<QString, float>());
                m_currentFrame = frame;
            }
            QMap<QString, float> &animationChunk = m_animationChunks.last();
            animationChunk.insert(animationName, animationProgress);
        }
    }
}

//!
//! Slot which is called when timer times out.
//!
void NetworkAnimNode::timeout ()
{
    // there must be at least 2 chunks in the queue
    if (m_animationChunks.size() > 1) {
        // set all values of one frame
        const QMap<QString, float> &animationMap = m_animationChunks.head();
        QMap<QString, float>::const_iterator iter;
        for (iter = animationMap.begin(); iter != animationMap.end(); iter++) {
            const QString &parameterName = iter.key();
            float value = iter.value();
            setValue(parameterName, value, true);
        }
        m_animationChunks.dequeue();
    }
}

//!
//! Slot which is called when thread has finished.
//!
void NetworkAnimNode::threadFinished()
{
    if (m_udpSocket) {
        delete m_udpSocket;
        m_udpSocket = 0;
    }
}

} // namespace NetworkAnimNode 
