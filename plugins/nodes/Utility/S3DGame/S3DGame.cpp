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
//! \file "S3DGame.cpp"
//! \brief Implementation file for S3DGame class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.03.2012
//!

#include "S3DGame.h"
#include "OgreContainer.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include "NumberParameter.h"

namespace S3DGameNode {
using namespace Frapper;

///
/// Initialization of non-integral static const class members
///

///
/// Constructors and Destructors
///

//!
//! Constructor of the S3DGame class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
S3DGame::S3DGame ( const QString &name, ParameterGroup *parameterRoot ) :
    S3DGameNode(name, parameterRoot),
    mCurrentPath(NULL),
    mNextPath(NULL),
    mCheckpoint(NULL),
    mPCTime(0),
    mPCLoopTime(1000), 
    mPCStep(0),
    mJumpAnimTimer(0),  
    mWalkAnimTimer(0),
    mFallAnimTimer(0),
    mDeathAnimTimer(0),  
    m_normalizedTime(0.0f),
    mP1Action(false), 
    mP2Action(false),
    mCreateJump(false), 
    mCreateFall(false), 
    mCreateDive(false),
    mPlayDeath(false),
    mPlatform1Time(0), mPlatform2Time(0), mPlatform3Time(0), 
    mPlatform1(true), mPlatform2(true), mPlatform3(true),
    mPlatform1Alpha(1.0f), mPlatform2Alpha(1.0f), mPlatform3Alpha(1.0f),
    mLavaTimer(0.0f),
    mEndScreenTime(0),
    mEndScreenTimeMax(15),
    mElapsed(0)
{
    // Connect the frapper-timebar signal with the update function of the game
    connect(this, SIGNAL(frameChanged(int)), SLOT(updateGame()));
    this->setProcessingFunction("Timer", SLOT(timerEvent()));

    // Create door animation object and connect with animation parameter
    mDoor = new AnimatedDoor( this->getNumberParameter("DoorOpenAnim"), 0.02f);
}

//!
//! Destructor of the S3DGame class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
S3DGame::~S3DGame ()
{
    // Jump paths are created dynamically an therefore needs deletion
    if( mCurrentPath && mCurrentPath->Name().contains("Jump to"))
        delete mCurrentPath;

    mCurrentPath = NULL;

    delete mDoor;
}
///
/// Public Functions
///


///
/// Private Slots
///

//!
//! Slot which is called on timer update.
//!
void S3DGame::timerEvent()
{
    // Check interaction of players

    // set action flag here, will be unset after events processed
    if( mP1ActionParameter->getValue(true).toBool() )
    {
        if( !mEndScreenTime )
            mP1Action = true;

        if( !mP1ActionParameter->isConnected() )
            mP1ActionParameter->setValue(QVariant(false), true);
    }

    if( mP2ActionParameter->getValue(true).toBool() )
    {
        if( !mEndScreenTime )
            mP2Action = true;

        //if( !mP2ActionParameter->isConnected() )
        //    mP2ActionParameter->setValue(QVariant(false), true);
    }
}

//!
//! Slot which is called on timer update.
//!
void S3DGame::updateGame()
{

    if( !CheckReady() )
    {
        return;
    }

    if( mEndScreenTime > 0 )
    {
        // End screen fade in
        if( mEndScreenTime < mEndScreenTimeMax)
            mEndScreenTime++;
        mEndScreen->setValue(QVariant(mEndScreenTime / (float) mEndScreenTimeMax * mAnimScale ), true);

        // output time
        QTime time = QTime(0,0,0,0).addMSecs( mElapsed );
        this->setValue("Stopwatch", "YOUR TIME IS "+time.toString("mm:ss.zzz")+"!", true);
        return;
    }

    // update stopwatch
    int elapsed = mTime.elapsed();
    if( elapsed - mElapsed > 200 )
    {
        mElapsed = elapsed;
        QTime time = QTime(0,0,0,0).addMSecs( elapsed );
        this->setValue("Stopwatch", time.toString("mm:ss.zzz"), true);
    }


    // Update Platforms according to timing
    UpdatePlatforms();

    // Update Enemies, obstacles, ...
    UpdateWorldObjects();

    // Movement of character: on Jump|Fall|Sling the PC moves automatically
    if( mCurrentPath->GetPathType() == Path::JUMP || 
        mCurrentPath->GetPathType() == Path::FALL || 
        mCurrentPath->GetPathType() == Path::SLING )
    {
        mPCStep = mStepSize;
    }
    else
    {
        // otherwise the movement is defined by the action of the player
        if( mP2Action )
            mPCStep = mStepSize;
        else
            mPCStep = 0;
    }
    
    // assure upper & lower bounds for step size
    CLAMP(mPCStep, 0, mStepSize);

    // actually move character forward
    AdvanceTime( mPCTime, mPCStep, mPCLoopTime);

    // update character position, animations, ...
    UpdateCharacter();

    // Check for any event on the current path for the current position
    ProcessEvents();

    // Create next path depending on events that have been taken place
    if( mCreateJump )
    {
        assert( mNextPath && "Next path must be valid!" );

        JumpPath* jump = new JumpPath( mPCModelTransform->getPosition(), mPCModelTransform->getOrientation(),
                                       mNextPath->GetPosition(0.0), mNextPath->GetOrientation(0.0));

        jump->Name("Jump to "+mNextPath->Name());

        if( mCurrentPath->GetPathType() == Path::STILL && mNextPath->GetPathType() == Path::STILL )
            jump->setLengthScale(2.0f);

        Path* targetPath = mNextPath;
        mNextPath = jump;
        AdvanceToNextPath( targetPath );
    }
    else if( mCreateFall )
    {
        // let PC fall of current path
        mNextPath = new FallPath( mPCModelTransform->getPosition(), mPCModelTransform->getOrientation());
        mNextPath->Name("Jump from "+mCurrentPath->Name());

        AdvanceToNextPath();
    }
    else if( mPCTime == mPCLoopTime )
    {
        if( mNextPath )
        {
            AdvanceToNextPath();
        }
        else
        {
            // end of the road ...
            if( mCurrentPath->GetPathType() == Path::SPHERE || 
                mCurrentPath->GetPathType() == Path::LINE    )
            {
                // dive from sphere path ...
                mNextPath = new FallPath( mPCModelTransform->getPosition(), mPCModelTransform->getOrientation());
                mNextPath->Name("Dive from "+mCurrentPath->Name());
                mCreateDive = true;

                AdvanceToNextPath();
            }
            else
            {
                // If the next path is invalid, the players didn't react properly
                // and the PC died! :( ( This may also indicate an error in the game logic... ;)
                // -> End this function and try to recover from the start
                if( mCheckpoint != NULL )
                {
                    Frapper::Log::error("Restart from checkpoint", "S3DGame::updateGame");
                    mNextPath = mCheckpoint;
                    AdvanceToNextPath();
                }
                else
                {
                    ResetGame();
                }
            }
        }
    }
        
    // reset input actions
    mP1Action = false;
    mP2Action = false;

};


void S3DGame::UpdateCharacter()
{
    float alphaPC = mPCTime / (float) mPCLoopTime;

    // PC Animation when idle, should only be set to FALSE in the following!
    bool waitIdle = true;

    if( mCurrentPath->GetPathType() == Path::FALL )
    {
        AdvanceTime( mFallAnimTimer, mStepSize, mFallAnimLength );
        CLAMP_UPPER( mFallAnimTimer, mFallAnimLength );

        // Write animation states
        if( mCreateDive ) {
            mDive->setValue( QVariant( mFallAnimTimer / (float) mFallAnimLength * mAnimScale ), true);
        } else {
            mFall->setValue( QVariant( mFallAnimTimer / (float) mFallAnimLength * mAnimScale ), true);
        }

        waitIdle = false;
    }
    else
    {
        mFallAnimTimer = 0;
        mFall->setValue( QVariant( 0), true);
        mDive->setValue( QVariant( 0), true);
        mCreateDive = false;
    }

    // Set value for PC walk animation cycle
    if( mP2Action &&
      ( mCurrentPath->GetPathType() == Path::SPHERE || 
        mCurrentPath->GetPathType() == Path::LINE ))
    {
        AdvanceTimeReset( mWalkAnimTimer, mPCStep, mWalkAnimLength );
        waitIdle = false;
    }
    else
    {
        mWalkAnimTimer = 0;
    }
    mWalk->setValue( QVariant( mWalkAnimTimer / (float) mWalkAnimLength * mAnimScale ), true);

    // When PC is on a jump path, the animations played according to 
    // the progress of the PC on that path
    if( mCurrentPath->GetPathType() == Path::JUMP )
    {
        waitIdle = false;
        mJump->setValue( QVariant( mPCTime / (float) (mPCLoopTime*1.33f) * mAnimScale ), true);
    }
    // Here, the jump animation is performed by the players action
    // The PC is only allowed to jump if it is on a sphere, line or still path
    // Once the animation starts, it is played until the end
    else if(( mP1Action || mJumpAnimTimer > 0 ) &&
            ( mCurrentPath->GetPathType() == Path::SPHERE || 
              mCurrentPath->GetPathType() == Path::LINE   ||
              mCurrentPath->GetPathType() == Path::STILL  ))
    {
        waitIdle = false;
        AdvanceTimeReset( mJumpAnimTimer, mStepSize, mJumpAnimLength );
        mJump->setValue( QVariant( mJumpAnimTimer / (float) mJumpAnimLength * mAnimScale ), true);
    }
    else
    {
        mJumpAnimTimer = 0;
        mJump->setValue( QVariant( 0 ), true);
    }

    // Update Position, orientation, scale of PC
    UpdateCharacterPosition( mCurrentPath, alphaPC, 0.0f );

    mWaitIdle->setValue( QVariant( waitIdle ), true);
}

void S3DGame::ProcessEvents()
{
    QListIterator<GameEvent*> eventIter( mCurrentPath->Events() );

    bool doorOpen = false;
    while( eventIter.hasNext() )
    {
        GameEvent* g = eventIter.next();

        // check if event is active for the current PC markerPosition
        if( g && g->isActive( mPCModelTransform->getPosition()) )
        {
            // HACK to show end screen if checkpoint of current event says "finish"
            if( !g->Checkpoint().isEmpty())
            {
                if( g->Checkpoint() == "finish" && !mEndScreenTime)
                {
                    mEndScreenTime = 1;
                    // measure time
                    mElapsed = mTime.elapsed();
                }
                else
                    mCheckpoint = mCurrentPath;
            }

            // check platforms
            if( !g->Platform().isEmpty())
            {
                if( g->Platform() == "Door" )
                    doorOpen = true;
                else if(( g->Platform() == "Platform1" && !mPlatform1 ) ||
                        ( g->Platform() == "Platform2" && !mPlatform2 ) ||
                        ( g->Platform() == "Platform3" && !mPlatform3 ) )
                {
                    // player is on platform and platform is gone -> create fall path
                    mCreateFall = true;
                    mNextPath = NULL;
                }
            }

            if( mP1Action && g->Jump())
            {
                mCreateJump = true;
                mNextPath = g->Jump();
            }

            if( mP2Action && g->Move() )
            {
                mNextPath = g->Move();
            }

            if( mP1Action && mP2Action && g->MoveJump())
            {
                mCreateJump = true;
                mNextPath = g->MoveJump();
            }

            // choose default action. If NULL, the game ends!
            if( !mNextPath )
            {
                if( mCurrentPath->GetPathType() == Path::STILL )
                {
                    // Just keep on standing on a still path
                    mNextPath = mCurrentPath;
                }
                else
                {
                    // Auto move to the next path, e.g. to connect sphere paths
                    mNextPath = g->Move();
                }
            }
        }
    }
    mDoor->SetOpen(doorOpen);
}

void S3DGame::AdvanceToNextPath( Path* overNextPath /*= NULL*/ )
{
    // store current path to delete it eventually later on
    Path* previousPath = mCurrentPath;

    // go on with the next path
    mCurrentPath = mNextPath;

    // there was a previous path that should come after the current
    // this happens mostly when a jump path was created in between the current and the next path
    mNextPath = overNextPath;

    // Reset PC Timing
    mPCTime = 0;

    // Jump and fall paths are created dynamically an therefore needs deletion
    if( previousPath ) {
        if( previousPath->GetPathType() == Path::JUMP ) {
            delete previousPath;

            // set jump anim length on jump path to 0.75,
            // as the PC then touches the ground in animation
            mJumpAnimTimer = mJumpAnimLength*0.75f;
        }
        else if( previousPath->GetPathType() == Path::FALL ) {
            delete previousPath;
        }
    }    

    // Rest path creation flags
    mCreateFall = false;
    mCreateJump = false;

    // Reset Anim timer
    mJumpAnimTimer = 0;
    mWalkAnimTimer = 0;
    mFallAnimTimer = 0;

    if( mCurrentPath )
    {
        // Set new time interval depending on normalized length of current path
        mPCLoopTime = mIntervalLength * mCurrentPath->GetNormalizedLength();
        CLAMP_LOWER( mPCLoopTime, 1);

        // HACK to blend out tutorial
        if( mCurrentPath->Name() == "Path2" )
            mRings->setValue(QVariant(0.0f), true);
    }

}

void S3DGame::UpdateCharacterPosition( Path* path, float alpha, float height )
{
    // update position
    Vec3 position = path->GetPosition( alpha );

    // Position of PC on current path
    mPCModelTransform->setPosition( position * (1+height));

    // Orientation of PC
    mPCModelTransform->setOrientation( path->GetOrientation( alpha ));

    // Scale of PC
    mPCModelTransform->setScale(mScale, mScale, mScale);

    mPCModelContainer->updateCopies();

    S3DGameNode::UpdateCamera();
}

void S3DGame::PlayDeathAnim()
{
    AdvanceTimeReset( mDeathAnimTimer, mStepSize, mDeathAnimLength );
    mDeath->setValue( QVariant( mAnimScale * mDeathAnimTimer / (float)mDeathAnimLength ), true);

    if( mDeathAnimTimer == 0 )
    {
        mPlayDeath = false;
        ResetGame();
    }
}

void S3DGame::loadReady()
{
    Node::loadReady();
    
    // Trigger Loading of animated objects after loading of scene and connections has finished
    CopyAnimatedObjects();

    // update the game once everything is loaded
    updateGame();
}

bool S3DGame::CheckReady()
{
    return mCurrentPath && mPCModelTransform && mLogic && mLevel;
}

void S3DGame::ResetGame()
{
    Log::error("Reset Game!");

    QString startPath = this->getStringValue("StartPath");
    mNextPath = mEngine.GetPathByName( startPath );
    if( !mNextPath )
        mNextPath = mEngine.GetPathByName( S3DGameEngine::StartPath );

    mRings->setValue(QVariant(100.0f), true);
    mEndScreen->setValue(QVariant(0.0f), true);
    mEndScreenTime = 0;
    mCheckpoint = NULL;
    AdvanceToNextPath();

    mTime.start();
    mElapsed = 0;
}


void S3DGame::AdvanceTime( int &time, const int &step, const int &max )
{
    time += step;
    if( time > max )
        time = max;

}

void S3DGame::AdvanceTimeReset( int &time, const int &step, const int &max )
{
    time += step;
    if( time > max )
        if( time-max < step )
            time = max;
        else
            time = 0;
}

void S3DGame::AdvanceTimeReset( float &time, const float &step, const float &max )
{
    time += step;
    if( time > max )
        if( time-max < step-0.01 )
            time = max;
        else
            time = 0;
}

void S3DGame::UpdatePlatforms()
{
    // Platform of Type 1: Platform which is moving up and down
    int platform1Loop = mPlatform1 ? 2*mPlatform1Loop : mPlatform1Loop;
    int platform2Loop = mPlatform2 ? 2*mPlatform2Loop : mPlatform2Loop;
    int platform3Loop = mPlatform3 ? 2*mPlatform3Loop : mPlatform3Loop;

    AdvanceTimeReset( mPlatform1Time, mStepSize, platform1Loop);
    
    // switch platform state if cycle completed
    if( mPlatform1Time == 0 ) mPlatform1 = !mPlatform1;

    if( mPlatform1 )
        mPlatform1Alpha = 0.0f; // Platform 1 is animated: @0.0 -> Up; @1.0 -> Down
    else
        mPlatform1Alpha = getPlatformMoveAlpha( mPlatform1Time / (float) platform1Loop );

    mPlatform1Anim->setValue( QVariant( mPlatform1Alpha * mAnimScale ), true);

    // Platform of Type 2: Platform which is moving up and down
    AdvanceTimeReset( mPlatform2Time, mStepSize, platform2Loop );
    if( mPlatform2Time == 0 ) mPlatform2 = !mPlatform2;

    if( mPlatform2 )
        mPlatform2Alpha = 0.0f;
    else
        mPlatform2Alpha = getPlatformMoveAlpha(mPlatform2Time / (float) platform2Loop );

    mPlatform2Anim->setValue( QVariant( mPlatform2Alpha * mAnimScale ), true);
        
    AdvanceTimeReset( mPlatform3Time, mStepSize, platform3Loop );
    if( mPlatform3Time == 0 ) mPlatform3 = !mPlatform3;

    if( mPlatform3 )
        mPlatform3Alpha = getPlatformTransparency(mPlatform3Time / (float) platform3Loop );
    else
        mPlatform3Alpha = 0.0f;

    SetTransparency( "platformShader", mPlatform3Alpha );
}

float S3DGame::getPlatformMoveAlpha( float alpha )
{
    if( alpha < 0.2f )
    {
        return alpha / 0.2f; // Move down
    } 
    else if( alpha < 0.8f )
    {
        return 1.0f; // fully down
    }
    else /* 0.8 < alpha < 1.0  */
    {
        return 1-((alpha-0.8f)/0.2f); // Move up
    }
}

float S3DGame::getPlatformTransparency( float alpha )
{
    if( alpha < 0.2f )
    {
        return alpha / 0.2f; // fade in
    } 
    else if( alpha < 0.8f )
    {
        return 1.0f; // fully visible
    }
    else /* 0.8 < alpha < 1.0  */
    {
        return 1-((alpha-0.8f)/0.2f); // fade out
    }
}


void S3DGame::UpdateWorldObjects()
{
    // update animation states of world assets
    foreach( Enemy* enemy, mEngine.Enemies() )
        enemy->Animate();

    foreach( EventMarker* eventMarker, mEngine.EventMarkers() )
        eventMarker->Animate();

    foreach( Obstacle* obstacle, mEngine.Obstacles() )
    {
        obstacle->Animate();
    }

    if( mDoor )
        mDoor->Animate(mAnimScale);

    AnimateLava();
}

void S3DGame::AnimateLava()
{
    AdvanceTimeReset( mLavaTimer, mLavaStep, Ogre::Math::TWO_PI);

    Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName("LavaShader");
    if( !materialPtr.isNull() )
    {
        Ogre::Pass* pass = materialPtr->getTechnique(0)->getPass(0); //1st pass, first texture unit

        if ( pass && pass->hasFragmentProgram()) 
        {
            Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
            if ( !fpParams.isNull() && fpParams->_findNamedConstantDefinition("timer"))
            {
                fpParams->setNamedConstant("timer", (Ogre::Real) sinf(mLavaTimer) );
            }
        }
    }
    else
    {
        Log::error("LavaShader materialName not found!", "S3DGame::AnimateLava");
    }
}

void S3DGame::SetTransparency( QString materialName, float alpha )
{
    Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName( materialName.toStdString() );
    if( !materialPtr.isNull() )
    {
        Ogre::TextureUnitState* ptus = materialPtr->getTechnique(0)->getPass(0)->getTextureUnitState(0); //1st pass, first texture unit
        ptus->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, alpha );
    }
    else
        Log::debug( materialName + " materialName not found!", "S3DGame::UpdatePlatforms");
}

} // namespace S3DGameNode 
