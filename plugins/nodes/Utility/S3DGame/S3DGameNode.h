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
//! \file "S3DGameNode.h"
//! \brief Header file for S3DGameNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       01.02.2012
//!

#ifndef S3DGAMENODE_H
#define S3DGAMENODE_H

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

// includes, Frapper
#include "Log.h"
#include "GeometryNode.h"
#include "ParameterGroup.h"
#include "AbstractParameter.h"
#include "ViewNode.h"

// Qt
#include <QQueue>

// Game Engine
#include "S3DGameEngine.h"

namespace S3DGameNode {
using namespace Frapper;

//!
//! S3DGameNode class: Frapper view node, handles parameters and events
//!
class S3DGameNode : public ViewNode
{
    Q_OBJECT


public: // constructors and destructors

    //!
    //! Constructor of the S3DGameNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    S3DGameNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the S3DGameNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~S3DGameNode ();


public: // functions

    //!
    //! Returns the scene node that contains scene objects created or modified
    //! by this node.
    //!
    //! \return The scene node containing objects created or modified by this node.
    //!
    virtual Ogre::SceneNode * getSceneNode ();


private slots:

    // Common function to update member variables 
    // according to game parameters. 
    //
    // To add a new game parameter, do the following:
    // 1) Add parameter to s3dgame.xml
    // 2) Set change function of parameter to this function in c'tor
    // 3) Add member variable and update in this function
    void updateGameParameter();

    //!
    //! Slot which is called when player character model changes.
    //!
    void LoadPlayerCharacter();

    //!
    //! Slot which is called when the light attached to the player character changes.
    //!
    void LoadPlayerLight();

    //!
    //! Slot called when geometry of level changes.
    //!
    void LoadLevel();

    //!
    //! Slot called when geometry of player 1 changes.
    //!
    void LoadLevel_P1();

    //!
    //! Slot called when geometry of player 2 changes.
    //!
    void LoadLevel_P2();

    //!
    //! Slot called when object marker geometry changes.
    //!
    void LoadObjectMarker();

    //!
    //! Slot called to load animated objects and place them by the object markers.
    //!
    void LoadAnimatedObject();

    //!
    //! Slot called when level logic changes.
    //!
    void LoadLogic();

    //!
    //! Show/Hide Geometry input from logic node
    //!
    void ShowGameLogic();

    //!
    //! Process output geometry of player 1
    //!
    void ProcessGeometryOutputP1();

    //!
    //! Process output geometry of player 2
    //!
    void ProcessGeometryOutputP2();

    //!
    //! Process output geometry of both players
    //!
    void ProcessGeometryOutputBoth();

    void ResetGameRequest();

    void ResetSlot();

protected: //data

    //!
    //! The S3DGame engine, handles parsing of input geometry
    //!
    S3DGameEngine mEngine;

    //!
    //! Reference to the ogre scene manager
    //!
    Ogre::SceneManager *mSceneManager;

    //!
    //! Scene nodes for the game
    //!
    Ogre::SceneNode *mGameSceneNode;
    Ogre::SceneNode *mOutputGeometryP1;
    Ogre::SceneNode *mOutputGeometryP2;
    Ogre::SceneNode *mOutputGeometryBoth;

    //!
    //! Scene nodes for player character and light transformation.
    //!
    Ogre::SceneNode *mPCModelTransform;
    Ogre::SceneNode *mPCModelLights;

    //!
    //! Scenes node for the level geometry
    //!
    Ogre::SceneNode *mLevel;
    Ogre::SceneNode *mLevelP1;
    Ogre::SceneNode *mLevelP2;

    //!
    //! Scene node for the level
    //!
    Ogre::SceneNode *mLogic;
    Ogre::SceneNode *mObjectMarker;
    Ogre::SceneNode *mEventMarker; 

    //!
    //! Scene node for the chase camera
    //!
    Ogre::SceneNode *mCameraNode;

    //!
    //! OgreContainer for the scene node copy update
    //!
    OgreContainer *mGeometryContainerP1;
    OgreContainer *mGeometryContainerP2;
    OgreContainer *mGeometryContainerBoth;
    OgreContainer *mGameContainer;
    OgreContainer *mCameraContainer;
    OgreContainer *mPCModelContainer;

    // Input action parameter
    Parameter *mP1ActionParameter, *mP2ActionParameter;

    // Animation length parameter
    int mJumpAnimLength, 
        mWalkAnimLength, 
        mDeathAnimLength, 
        mFallAnimLength;

    int mPlatform1Loop, mPlatform2Loop, mPlatform3Loop;

    // local correspondences to GUI parameters
    float mScale, 
          mStepSize, 
          mIntervalLength, 
          mAnimScale, 
          mLavaStep;

    // Output animation number parameter
    NumberParameter *mJumpStart, 
                    *mJumpEnd, 
                    *mWalk, 
                    *mJump,
                    *mDeath, 
                    *mFall,
                    *mDive,
                    *mPlatform1Anim,
                    *mPlatform2Anim,
                    *mPlatform3Anim,
                    *mRings,
                    *mEndScreen;

    // Output parameter to set loop animations
    Parameter *mWaitIdle, *mJumpIdle;

    // Output Camera Parameter, correspond to the position and orientation of the player character
    Parameter *mPosition, *mOrientation;

protected: //functions

    void UpdateCamera();

    // create a new input parameter for each animated object in the scene
    void createInputObjectParameter( const QList<QString>& objectNames );

    //! Process output geometry of both players
    void CopyAnimatedObjects();

    // process scene nodes
    void processInputGeometryP1( Ogre::SceneNode * inputGeometryP1 );
    void processInputGeometryP2( Ogre::SceneNode * inputGeometryP2 );
    void processObjectMarker( Ogre::SceneNode * inputObjectMarker );
    void processInputLogic( Ogre::SceneNode * inputLogic );

    // pure virtual function, used to reset the game on certain events
    virtual void ResetGame()=0;
};

} // namespace S3DGameNode 
#endif