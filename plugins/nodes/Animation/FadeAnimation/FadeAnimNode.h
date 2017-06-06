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
//! \file "FadeAnimNode.h"
//! \brief Header file for FadeAnimNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date		09.10.2012
//!

#ifndef FADEANIMNODE_H
#define FADEANIMNODE_H

#include "Node.h"
#include <QTimer>

namespace FadeAnimNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class FadeAnimNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the FadeAnimNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    FadeAnimNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the FadeAnimNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~FadeAnimNode ();

protected: // events

private slots:	

    //!
    //! This slot is called if timer fires
    //!
    void onFrameChanged(int n);

    //! 
    //! Slot for the interval changed event
    //! 
    void onIntervalChanged();

    //! 
    //! Slot for the multiply changed event
    //! 
    void onMultiplyChanged();

    //! 
    //! Slot for the play faded event
    //! 
    void onPlay();

    //! 
    //! Slot called by fadeout option parameter
    //! 
    void onOptionsChanged();

private: //funtions

    void AdvanceTime( int &time, const int &step, const int &max );
private: // data

    //! References to parameters for direct read and write access
    NumberParameter *m_FadeIn;
    NumberParameter *m_FadeOut;
    
    bool        m_FadeInState;
    bool        m_UseFadeInAsOut;
    bool        m_FadeOutFromHalf;
    bool        m_update;
    bool        m_holdToPlay;
    float       m_multiplyFactor;

    int         m_fadeInInterval;
    int         m_fadeOutInterval;
    int         m_fadeIntervalTotal;

    int         m_currentTime;
    int         m_previousTime;
};

} // namespace FadeAnimNode 

#endif
