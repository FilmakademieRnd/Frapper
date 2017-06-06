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
//! \file "TriggerAnimNode.h"
//! \brief Header file for TriggerAnimNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       18.05.2009 (last updated)
//!

#ifndef TRIGGERANIMNODE_H
#define TRIGGERANIMNODE_H

#include "Node.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace TriggerAnimNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class TriggerAnimNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the TriggerAnimNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TriggerAnimNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the TriggerAnimNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TriggerAnimNode ();

protected: // events

private slots:	

    //!
    //! Slot for the update function, which is called on each frame
    //!
    void onFrameChanged();

    void Reset();

    //! 
    //! Slot for the interval changed event
    //! 
    void onIntervalChanged();

    //! 
    //! Slot for the multiply changed event
    //! 
    void onMultiplyChanged();

    //! 
    //! Slot for the play triggered event
    //! 
    void onPlayTriggered();

    //! 
    //! Slot for the loop event
    //! 
    void onLoopTriggered();

	//!
	//! Slot for the Reset event
	//!
	void onResetChanged();

private: //funtions

private: // data

    // References to parameters for direct read and write access
    NumberParameter *mpNormalizedTime;

    bool        mIsPlaying;
    float       mMultiplyFactor;
    int         mInterval;
    int         mBaseTime;
    int         mPreviousTime;
	bool		mReset;
	bool		mIsFinished;
};

} // namespace TriggerAnimNode 

#endif
