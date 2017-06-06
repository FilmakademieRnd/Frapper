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
//! \file "HeadLogicNode.h"
//! \brief Header file for HeadLogicNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.04.2009 (last updated)
//!

#ifndef HEADLOGICNODE_H
#define HEADLOGICNODE_H

#include "Node.h"
#include <QtCore/QTimer>
#include <QtCore/QTime>

namespace HeadLogicNode {
using namespace Frapper;

//!
//! Class representing nodes for head animation logic in a 3D scene.
//!
class HeadLogicNode : public Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the HeadLogicNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    HeadLogicNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the BorAnimationNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~HeadLogicNode ();

private: //methods

    //!
    //! Update sleeping animation.
    //!
    void updateSleep();

    //!
    //! Update sleeping animation.
    //!
    void updateStaring();

private slots: //

    //!
    //! Processes the output parameter depending on its name .
    //!
    void processOutputParameter ();


private slots: //

    //!
    //! On timer timeout.
    //!
    void updateTimer();

private: // data

    QTimer *m_timer;
    bool m_isSleeping;
    bool m_isStaring;
    bool m_isWakingUp;
    double m_sleepingWeight;
    bool m_noFaceDetected;
    QTime m_sleepingTimeLimit;

};

} // namespace HeadLogicNode 

#endif
