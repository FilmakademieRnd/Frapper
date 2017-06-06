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
//! \file RandomBlinkNode.h
//! \date 02.04.2008 10:47:19 (GMT+2)
//! \version 1.0
//!
#ifndef RANDOMBLINKNODE_H
#define RANDOMBLINKNODE_H

#include "Node.h"
#include <QtCore/QTimer>

namespace RandomBlinkNode {
using namespace Frapper;

//!
//! Class in the Borealis application representing nodes that can
//! contai OGRE entities with animation.
//!
class RandomBlinkNode : public Node
{

    Q_OBJECT

public: /// constructors

    //!
    //! Constructor of the RandomBlinkNode class.
    //!
    //! \param name The name to give the new mesh node.
    //!
    RandomBlinkNode ( QString name, ParameterGroup *parameterRoot );

public: /// destructors

    //!
    //! Destructor of the RandomBlinkNode class.
    //!
    ~RandomBlinkNode ();

public slots:

    //!
    //! Is triggered of timer event.
    //!
    void timerUpdate ();


private:
    unsigned long m_timestamp;
    unsigned long m_lastTimestamp;
    int m_updown;
    bool m_blinkActive;
	float randomAmplitude;
    QTimer *m_timer;
};

} // namespace RandomBlinkNode

#endif
