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
//! \file "S3DGame.h"
//! \brief Header file for S3DGame class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.03.2012
//!

#ifndef S3DGAME_H
#define S3DGAME_H

#include "OgreContainer.h"
#include "OgreTools.h"

#include "S3DGameNode.h"

#include <QTime>

namespace S3DGameNode {
using namespace Frapper;

//!
//! The S3DGame class
//!
class S3DGame : public S3DGameNode
{
    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the S3DGameNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    S3DGame ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the S3DGameNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~S3DGame ();


public: // functions

private slots:

    //!
    //! Slot which is called on new frame
    //!
    void updateGame();

    //!
    //! Slot which is called on timer update.
    //!
    void timerEvent();

    //!
    //! Executed when scene loading is ready.
    //!
    virtual void loadReady();

private: //data

    //!
    //! Storage for the normalized timer input.
    //!
    float m_normalizedTime;

    Path* mCurrentPath;
    Path* mNextPath;
    Path* mCheckpoint;

    // action flags
    bool mP1Action;
    bool mP2Action;

    // animation flags and timer
    bool mPlayDeath;

    int mDeathAnimTimer;
    int mWalkAnimTimer;
    int mFallAnimTimer;
    int mJumpAnimTimer;

    // Player character timing for movement
    int mPCTime, mPCLoopTime, mPCStep;

    // Platform timings
    bool mPlatform1, mPlatform2, mPlatform3;
    int mPlatform1Time, mPlatform2Time, mPlatform3Time;
    float mPlatform1Alpha, mPlatform2Alpha, mPlatform3Alpha;

    // Timing of endscreen
    int mEndScreenTime, mEndScreenTimeMax;

    float mLavaTimer;

    // The animated door object
    AnimatedDoor* mDoor;

    Ogre::Vector3 mOrigin;

    // flag used when a jump path needs to be created as next path
    bool mCreateJump, mCreateFall, mCreateDive;

    QTime mTime;
    int mElapsed;

protected: //functions

    virtual void ResetGame();

private:

    void UpdateCharacterPosition( Path* path, float alpha, float height );
    void ProcessEvents();

    void UpdateWorldObjects();
    void AdvanceToNextPath( Path* overNextPath = NULL);


    bool CheckReady();
    void AdvanceTime(  int &time, const int &step, const int &max );
    void AdvanceTimeReset(  int &time, const int &step, const int &max );
    void AdvanceTimeReset(  float &time, const float &step, const float &max );
    void PlayDeathAnim();
    void UpdatePlatforms();

    void SetTransparency( QString materialName, float alpha );

    float getPlatformTransparency( float alpha );
    float getPlatformMoveAlpha( float alpha );

    void UpdateCharacter();
    void AnimateLava();
};

} // namespace S3DGameNode 
#endif