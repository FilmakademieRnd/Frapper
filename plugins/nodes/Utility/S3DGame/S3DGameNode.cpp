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
//! \file "S3DGameNode.cpp"
//! \brief Implementation file for S3DGameNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       07.03.2012
//!


#include "S3DGameNode.h"

#include "OgreTools.h"
#include "OgreContainer.h"
#include "OgreManager.h"

#include "OgreSceneNode.h"
#include "OgreMovableObject.h"


Q_DECLARE_METATYPE(Ogre::Quaternion)

namespace S3DGameNode {
using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the S3DGameNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
S3DGameNode::S3DGameNode ( const QString &name, ParameterGroup *parameterRoot ) :
    ViewNode(name, parameterRoot),
    mSceneManager(NULL),
    mPCModelTransform(NULL),
    mPCModelLights(NULL),
    mGameSceneNode(NULL),
    mCameraNode(NULL),
    mLevel(NULL),
    mLevelP1(NULL),
    mLevelP2(NULL),
    mLogic(NULL),    
    mObjectMarker(NULL),
    mEventMarker(NULL),
    mWaitIdle(NULL), 
    mJumpStart(NULL), 
    mJumpEnd(NULL), 
    mJumpIdle(NULL), 
    mWalk(NULL),
    mJump(NULL),
    mFall(NULL),
    mDive(NULL),
    mDeath(NULL),
    mPlatform1Anim(NULL),
    mPlatform2Anim(NULL),
    mPlatform3Anim(NULL),
    mP1ActionParameter(NULL),
    mP2ActionParameter(NULL),
    mStepSize(40),
    mIntervalLength(10000),
    mScale(0.5f),
    mAnimScale(100.0f),
    mLavaStep(0.01f),
    mJumpAnimLength(300),
    mWalkAnimLength(300),
    mFallAnimLength(500),
    mDeathAnimLength(1000),
    mPlatform1Loop( 1000), mPlatform2Loop(2000), mPlatform3Loop(2000)
{
    mSceneManager = OgreManager::getSceneManager();

    // setup game parameter change event
    this->getNumberParameter("PCScale")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("StepSize")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("AnimScale")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("IntervalLength")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("JumpAnimLength")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("WalkAnimLength")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("FallAnimLength")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("DeathAnimLength")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("LavaStep")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("Platform1Time")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("Platform2Time")->setChangeFunction(SLOT(updateGameParameter()));
    this->getNumberParameter("Platform3Time")->setChangeFunction(SLOT(updateGameParameter()));

    this->getParameter("Reset1")->setProcessingFunction(SLOT(ResetSlot()));
    this->getParameter("Reset2")->setProcessingFunction(SLOT(ResetSlot()));

    mPosition = this->getParameter("Position");
    mOrientation = this->getParameter("Orientation");

    assert(mPosition);
    assert(mOrientation);

    mP1ActionParameter = this->getParameter( "P1_Action" );
    mP2ActionParameter = this->getParameter( "P2_Action" );

    assert(mP1ActionParameter);
    assert(mP2ActionParameter);

    // Animations
    mWaitIdle = getParameter("WaitIdleAnim");
    mJumpIdle = getParameter("JumpIdleAnim");
    mJumpStart = getNumberParameter("JumpStartAnim");
    mJumpEnd = getNumberParameter("JumpEndAnim");
    mWalk = getNumberParameter("WalkAnim");
    mJump = getNumberParameter("JumpAnim");
    mFall = getNumberParameter("FallAnim");
    mDive = getNumberParameter("DiveAnim");
    mDeath = getNumberParameter("DeathAnim");
    mPlatform1Anim = getNumberParameter("Platform1Anim");
    mPlatform2Anim = getNumberParameter("Platform2Anim");
    mPlatform3Anim = getNumberParameter("Platform3Anim");
    mRings = getNumberParameter("Rings");
    mEndScreen = getNumberParameter("EndScreen");

    assert(mWaitIdle);
    assert(mJumpStart);
    assert(mJumpEnd);
    assert(mJumpIdle);
    assert(mWalk);
    assert(mJump);
    assert(mFall);
    assert(mDive);
    assert(mDeath);
    assert(mPlatform1Anim);
    assert(mPlatform2Anim);
    assert(mPlatform3Anim);
    assert(mRings);
    assert(mEndScreen);

    // processing functions
    this->setProcessingFunction("Level", SLOT(LoadLevel()));
    this->setProcessingFunction("Logic", SLOT(LoadLogic()));
    this->setProcessingFunction("Player1", SLOT(LoadLevel_P1()));
    this->setProcessingFunction("Player2", SLOT(LoadLevel_P2()));
    this->setProcessingFunction("PlayerCharacter", SLOT(LoadPlayerCharacter()));
    this->setProcessingFunction("PlayerCharacterLight", SLOT(LoadPlayerLight()));
    this->setProcessingFunction("ObjectMarker", SLOT(LoadObjectMarker()));

    this->setChangeFunction("ShowGameLogic", SLOT(ShowGameLogic()));
    this->setChangeFunction("ShowLightGeometry", SLOT(LoadPlayerLight()));

    // This affection is required for the dirty-chain
    this->addAffection("Time", "Timer");

    // Create a new SceneNode for the game
    mGameSceneNode = OgreManager::createSceneNode( m_name );
    mGameContainer = new OgreContainer(mGameSceneNode);
    mGameSceneNode->setUserAny( Ogre::Any(mGameContainer));

    // Create a new SceneNode for the movement of the player character
    mPCModelTransform = OgreManager::createSceneNode(m_name + "_PlayerCharacter");
    mPCModelContainer = new OgreContainer(mPCModelTransform);
    mPCModelTransform->setUserAny(Ogre::Any(mPCModelContainer));
    mGameSceneNode->addChild(mPCModelTransform);

    // Create a new SceneNode for the lights attached to the player character
    mPCModelLights = OgreManager::createSceneNode(m_name + "_PlayerLights");
    mPCModelLights->setUserAny( Ogre::Any( new OgreContainer( mPCModelLights)));
    mPCModelTransform->addChild(mPCModelLights);

    // Create a new SceneNode for the event markers
    mEventMarker = OgreManager::createSceneNode(m_name + "_EventMarker");
    mEventMarker->setUserAny( Ogre::Any( new OgreContainer( mEventMarker)));

    // Create a (3rd person) camera node and add to character node
    mCameraNode = OgreManager::createSceneNode(m_name + "_Camera");
    mCameraContainer = new OgreContainer(mCameraNode);
    mCameraNode->setUserAny(Ogre::Any(mCameraContainer));
    mCameraNode->setPosition(Ogre::Vector3(0,50,-100));

    // create scene nodes for the geometry output of Player 1
    mOutputGeometryP1 = OgreManager::createSceneNode( m_name + "_OutputP1" );
    mGeometryContainerP1 = new OgreContainer(mOutputGeometryP1);
    mOutputGeometryP1->setUserAny(Ogre::Any(mGeometryContainerP1));

    // create scene nodes for the geometry output of Player 1
    mOutputGeometryP2 = OgreManager::createSceneNode( m_name + "_OutputP2" );
    mGeometryContainerP2 = new OgreContainer(mOutputGeometryP2);
    mOutputGeometryP2->setUserAny(Ogre::Any(mGeometryContainerP2));

    // create scene nodes for the geometry output of both Players
    mOutputGeometryBoth = OgreManager::createSceneNode( m_name + "_OutputBoth" );
    mGeometryContainerBoth = new OgreContainer(mOutputGeometryBoth);
    mOutputGeometryBoth->setUserAny(Ogre::Any(mGeometryContainerBoth));

    Parameter* gameP1 = this->getParameter("Game_P1");
    assert( gameP1 );
    gameP1->setValue( QVariant::fromValue<Ogre::SceneNode*>(mOutputGeometryP1));
    gameP1->setProcessingFunction( SLOT(ProcessGeometryOutputP1()) );
    gameP1->addAffectingParameter( this->getParameter("Level") );
    gameP1->addAffectingParameter( this->getParameter("Player1") );
    gameP1->addAffectingParameter( this->getParameter("Logic") );
    gameP1->addAffectingParameter( this->getParameter("ObjectMarker") );
    gameP1->addAffectingParameter( this->getParameter("PlayerCharacter") );
    gameP1->addAffectingParameter( this->getParameter("PlayerCharacterLight") );

    Parameter* gameP2 = this->getParameter("Game_P2");
    assert( gameP2 );
    gameP2->setValue( QVariant::fromValue<Ogre::SceneNode*>(mOutputGeometryP2));
    gameP2->setProcessingFunction( SLOT(ProcessGeometryOutputP2()));
    gameP2->addAffectingParameter( this->getParameter("Level") );
    gameP2->addAffectingParameter( this->getParameter("Player2") );
    gameP2->addAffectingParameter( this->getParameter("Logic") );
    gameP2->addAffectingParameter( this->getParameter("ObjectMarker") );
    gameP2->addAffectingParameter( this->getParameter("PlayerCharacter") );
    gameP2->addAffectingParameter( this->getParameter("PlayerCharacterLight") );

    Parameter* gameBoth = this->getParameter("Game_Both");
    assert( gameBoth );
    gameBoth->setValue( QVariant::fromValue<Ogre::SceneNode*>(mOutputGeometryBoth));
    gameBoth->setProcessingFunction( SLOT(ProcessGeometryOutputBoth()));
    gameBoth->addAffectingParameter( this->getParameter("Level"));
    gameBoth->addAffectingParameter( this->getParameter("Logic"));
    gameBoth->addAffectingParameter( this->getParameter("ObjectMarker"));
    gameBoth->addAffectingParameter( this->getParameter("PlayerCharacter"));
    gameBoth->addAffectingParameter( this->getParameter("PlayerCharacterLight"));

    ParameterGroup* animatedObjectsGroup = this->getParameterGroup("Animated Objects");
    if( animatedObjectsGroup )
    {
        // Get the names of all existing input object parameters
        QList<QString> parameterNames;
        const AbstractParameter::List& animatedObjects = animatedObjectsGroup->getParameterList();
        for( int i=0; i<animatedObjects.size(); i++)
        {
            Parameter* parameter = dynamic_cast<Parameter*>(animatedObjects.at(i));
            if( parameter )
            {
                parameter->setPinType(Parameter::PT_Input);
                parameter->setVisible(true);
                parameter->setSelfEvaluating(true);

                gameP1->addAffectingParameter( parameter);
                gameP2->addAffectingParameter( parameter);
                gameBoth->addAffectingParameter( parameter);
                parameter->setProcessingFunction( SLOT( LoadAnimatedObject()));
            }
        }
    }

    this->getParameter("Copy Animated Objects")->setCommandFunction( SLOT(CopyAnimatedObjects()));
    this->getParameter("Reset")->setCommandFunction( SLOT( ResetGameRequest()));
 }

//!
//! Destructor of the S3DGameNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
S3DGameNode::~S3DGameNode ()
{
    // delete scene nodes created in the c'tor
    OgreTools::deepDeleteSceneNode( mEventMarker, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mPCModelTransform, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mLevel, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mLevelP1, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mLevelP2, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mLogic, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mObjectMarker, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mOutputGeometryP1, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mOutputGeometryP2, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mOutputGeometryBoth, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mCameraNode, mSceneManager, true);
    OgreTools::deepDeleteSceneNode( mGameSceneNode, mSceneManager, true);
 
}

///
/// Public Functions
///


//!
//! Rebuild the current SceneNode and return
//!
//!
// rebuild current sceneNode by reattaching all child nodes, which may currently be attached somewhere else
Ogre::SceneNode * S3DGameNode::getSceneNode()
{
    return mGameSceneNode;
}


///
/// Private Slots
///

//!
//! Slot which is called when player character model changes.
//!
void S3DGameNode::LoadPlayerCharacter()
{ 
    assert( mPCModelTransform );
    OgreManager::relocateSceneNode( mPCModelLights, NULL);

    // destroy all scene nodes and objects in the render scene
    OgreTools::deepDeleteSceneNode( mPCModelTransform, mSceneManager );

    Parameter *geometryParameter = this->getParameter("PlayerCharacter");

    foreach( QVariant parameterValue, geometryParameter->getValueList())
    {
        if( parameterValue.canConvert<Ogre::SceneNode *>() )
        {
            Ogre::SceneNode *inputGeometrySceneNode = parameterValue.value<Ogre::SceneNode *>();
            Ogre::SceneNode *inputGeometrySceneNodeCopy = 0;           

            OgreTools::deepCopySceneNode( inputGeometrySceneNode, inputGeometrySceneNodeCopy, QString("%1GeometrySceneNode").arg(m_name), mSceneManager );
            if (inputGeometrySceneNodeCopy) 
            {
                OgreManager::relocateSceneNode( inputGeometrySceneNodeCopy , mPCModelTransform);
            }
        }
    }

    OgreManager::relocateSceneNode( mPCModelLights, mPCModelTransform);
}


void S3DGameNode::LoadPlayerLight()
{
    assert( mPCModelLights );

    // destroy PCLights scene node
    OgreTools::deepDeleteSceneNode( mPCModelLights, mSceneManager );

    bool showLightGeometry = getBoolValue("ShowLightGeometry");

    Parameter *geometryParameter = this->getParameter("PlayerCharacterLight");
    foreach( QVariant parameterValue, geometryParameter->getValueList())
    {
        if( parameterValue.canConvert<Ogre::SceneNode *>() )
        {
            Ogre::SceneNode *inputLightSceneNode = parameterValue.value<Ogre::SceneNode *>();
            Ogre::SceneNode *inputLightSceneNodeCopy = 0;           

            OgreTools::deepCopySceneNode( inputLightSceneNode, inputLightSceneNodeCopy, m_name+"LightSceneNode", mSceneManager );
            if (inputLightSceneNodeCopy) 
            {
                if( !showLightGeometry )
                {
                    QList<Ogre::Entity*> entitites;
                    OgreTools::getAllEntities(inputLightSceneNodeCopy, entitites);
                    foreach( Ogre::Entity* entity, entitites )
                    {
                        inputLightSceneNodeCopy->detachObject(entity);
                        OgreTools::deleteUserAnyFromMovableObject(entity);
                        mSceneManager->destroyEntity(entity);
                    }
                }
                OgreManager::relocateSceneNode( inputLightSceneNodeCopy , mPCModelLights );
            }
        }
    }

    // remove all entities from PC lights
    {
    }
}

void S3DGameNode::LoadLevel()
{
    Ogre::SceneNode* inputLevel = getSceneNodeValue("Level");
    if( !inputLevel )
        return;

    // Store copy of input Level 
    OgreTools::deepDeleteSceneNode( mLevel, mSceneManager, true );
    OgreTools::deepCopySceneNode( inputLevel, mLevel, m_name + "_Level");

    OgreManager::relocateSceneNode(mLevel, mGameSceneNode);
}

void S3DGameNode::LoadLevel_P1()
{
    Ogre::SceneNode* inputLevelP1 = getSceneNodeValue("Player1");
    if( !inputLevelP1 )
        return;

    // Store copy of input Level of Player1
    OgreTools::deepDeleteSceneNode( mLevelP1, mSceneManager, true );
    OgreTools::deepCopySceneNode( inputLevelP1, mLevelP1, m_name + "_LevelP1");

    OgreManager::relocateSceneNode(mLevelP1, mGameSceneNode);
    processInputGeometryP1(mLevelP1);
}

void S3DGameNode::LoadLevel_P2()
{
    // Get Ogre::SceneNode from Level input parameter
    Ogre::SceneNode* inputLevelP2 = getSceneNodeValue("Player2");
    if( !inputLevelP2 )
        return;

    // Store copy of input Level of Player1
    OgreTools::deepDeleteSceneNode( mLevelP2, mSceneManager, true );
    OgreTools::deepCopySceneNode( inputLevelP2, mLevelP2, m_name + "_LevelP2");

    OgreManager::relocateSceneNode(mLevelP2, mGameSceneNode);
    processInputGeometryP2(mLevelP2);
}


void S3DGameNode::LoadLogic()
{
    // Load Logic only if sufficient input!
    if( !getParameter("Logic")->isConnected() )
        return;

    Ogre::SceneNode *inputLogic = getSceneNodeValue("Logic");
    if ( !inputLogic )
        return;

    // Store copy of input Logic for rendering
    if( mLogic )
    {
        OgreTools::deepDeleteSceneNode( mLogic, mSceneManager, true );
    }

    OgreTools::deepCopySceneNode( inputLogic, mLogic, m_name + "_Logic");

    // Optionally Attach logic SN to game SN
    ShowGameLogic();

    // This function is overwritten in the Game class
    processInputLogic( inputLogic );
}

void S3DGameNode::LoadObjectMarker()
{
    // Load Logic only if sufficient input!
    if( !getParameter("ObjectMarker")->isConnected() )
        return;

    Ogre::SceneNode *inputObjectMarker = getSceneNodeValue("ObjectMarker");
    if ( !inputObjectMarker )
        return;

    // Store copy of input Logic for rendering
    if( mObjectMarker )
    {
        OgreTools::deepDeleteSceneNode( mObjectMarker, mSceneManager, true );
    }

    OgreTools::deepCopySceneNode( inputObjectMarker, mObjectMarker, m_name + "_ObjectMarker");

    // Object marker processing
    processObjectMarker( inputObjectMarker );
}


void S3DGameNode::LoadAnimatedObject()
{

    GeometryParameter *parameter = dynamic_cast<GeometryParameter *>(sender());

    if( parameter )
    {
        QString objectName = parameter->getName();
        Ogre::SceneNode* objectGeometry = parameter->getSceneNode();
        if( objectGeometry )
            mEngine.createAnimatedObjects( objectName, objectGeometry, mLevelP1, mLevelP2, mLevel);
    }
}


void S3DGameNode::CopyAnimatedObjects()
{

    ParameterGroup* animatedObjectsGroup = this->getParameterGroup("Animated Objects");
    if( animatedObjectsGroup )
    {
        // Get the names of all existing input object parameters
        QList<QString> parameterNames;
        const AbstractParameter::List& animatedObjects = animatedObjectsGroup->getParameterList();
        for( int i=0; i<animatedObjects.size(); i++)
        {
            GeometryParameter* parameter = dynamic_cast<GeometryParameter*>(animatedObjects.at(i));
            if( parameter )
            {
                QString objectName = parameter->getName();
                Ogre::SceneNode* objectGeometry = parameter->getSceneNode();
                if( objectGeometry )
                    mEngine.createAnimatedObjects( objectName, objectGeometry, mLevelP1, mLevelP2, mLevel );
            }
        }
    }
    this->getParameter("Game_P1")->propagateDirty(true);
    this->getParameter("Game_P2")->propagateDirty(true);
    this->getParameter("Game_Both")->propagateDirty(true);
}

void S3DGameNode::UpdateCamera()
{
    if( mOrientation->isConnected())
    {
        mCameraNode->setOrientation( mPCModelTransform->getOrientation());
        mCameraNode->yaw( Ogre::Radian(Ogre::Math::PI));
        mOrientation->setValue( QVariant::fromValue<Ogre::Quaternion>( mCameraNode->getOrientation() ), true);
    }

    if( mPosition->isConnected() )
    {
        mCameraNode->setPosition( mPCModelTransform->getPosition());

        Ogre::Vector3 cameraTranslate = this->getVectorValue("CameraTranslate");
        mCameraNode->translate( cameraTranslate, Ogre::Node::TS_LOCAL );
        mPosition->setValue( QVariant::fromValue<Ogre::Vector3>( mCameraNode->getPosition()), true);
    }
}

void S3DGameNode::ShowGameLogic()
{
    if( this->getBoolValue("ShowGameLogic"))
    {
        OgreManager::relocateSceneNode(mLogic, mGameSceneNode);
        OgreManager::relocateSceneNode(mObjectMarker, mGameSceneNode);
    }
    else
    {
        OgreManager::relocateSceneNode(mLogic, NULL);
        OgreManager::relocateSceneNode(mObjectMarker, NULL);
    }
}

void S3DGameNode::ProcessGeometryOutputP1()
{
    assert( mOutputGeometryP1 );

    // clean up output
    OgreTools::deepDeleteSceneNode(mOutputGeometryP1, mSceneManager);


    Ogre::SceneNode* sceneNodeCopy = NULL;

    // add copy of character node
    if( mPCModelTransform )
    {
        OgreTools::deepCopySceneNode( mPCModelTransform, sceneNodeCopy, "P1", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryP1->addChild(sceneNodeCopy);
        }
    }

    // add copy of Level node
    if( mLevel )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mLevel, sceneNodeCopy, "P1", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryP1->addChild(sceneNodeCopy);
        }
    }

    // add copy of Level node
    if( mLevelP1 )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mLevelP1, sceneNodeCopy, "P1", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryP1->addChild(sceneNodeCopy);
        }
    }

    if( mEventMarker )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mEventMarker, sceneNodeCopy, "P1", mSceneManager );

        if( sceneNodeCopy )
        {
            mOutputGeometryP1->addChild(sceneNodeCopy);
        }
    }
}


void S3DGameNode::ProcessGeometryOutputP2()
{
    assert( mOutputGeometryP2 );

    // clean up output
    OgreTools::deepDeleteSceneNode(mOutputGeometryP2, mSceneManager);

    Ogre::SceneNode* sceneNodeCopy = NULL;

    // add copy of character node
    if( mPCModelTransform )
    {
        OgreTools::deepCopySceneNode( mPCModelTransform, sceneNodeCopy, "P2", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryP2->addChild(sceneNodeCopy);
        }
    }

    // add copy of Level node
    if( mLevel )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mLevel, sceneNodeCopy, "P2", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryP2->addChild(sceneNodeCopy);
        }
    }

    // add copy of Level node
    if( mLevelP2 )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mLevelP2, sceneNodeCopy, "P2", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryP2->addChild(sceneNodeCopy);
        }
    }

    if( mEventMarker )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mEventMarker, sceneNodeCopy, "P2", mSceneManager );

        if( sceneNodeCopy )
        {
            mOutputGeometryP2->addChild(sceneNodeCopy);
        }
    }
}

void S3DGameNode::ProcessGeometryOutputBoth()
{
    assert( mOutputGeometryBoth );

    // clean up output
    OgreTools::deepDeleteSceneNode(mOutputGeometryBoth, mSceneManager);

    Ogre::SceneNode* sceneNodeCopy = NULL;

    // add copy of character node
    if( mPCModelTransform )
    {
        OgreTools::deepCopySceneNode( mPCModelTransform, sceneNodeCopy, "Both", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryBoth->addChild(sceneNodeCopy);
        }
    }

    // add copy of Level node
    if( mLevel )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mLevel, sceneNodeCopy, "Both", mSceneManager);

        if( sceneNodeCopy )
        {
            mOutputGeometryBoth->addChild(sceneNodeCopy);
        }
    }

    if( mEventMarker )
    {
        sceneNodeCopy = NULL;
        OgreTools::deepCopySceneNode( mEventMarker, sceneNodeCopy, "Both", mSceneManager );

        if( sceneNodeCopy )
        {
            mOutputGeometryBoth->addChild(sceneNodeCopy);
        }
    }
}

void S3DGameNode::updateGameParameter()
{
    NumberParameter* param = dynamic_cast<NumberParameter*>(sender());
    if( param )
    {
        //<parameter name="StepSize" ...
        if( param->getName() == "StepSize")
            mStepSize = param->getValue(false).toInt();
        //<parameter name="IntervalLength" ...
        else if( param->getName() == "IntervalLength")
            mIntervalLength = param->getValue(false).toInt();
        //<parameter name="PCScale" ...
        else if( param->getName() == "PCScale")
            mScale = param->getValue(false).toFloat();
        //<parameter name="AnimScale" ...
        else if( param->getName() == "AnimScale")
            mAnimScale = param->getValue(false).toFloat();
        //<parameter name="LavaStep" ...
        else if( param->getName() == "LavaStep")
            mLavaStep = param->getValue(false).toFloat();
        //<parameter name="JumpAnimLength" ...
        else if( param->getName() == "JumpAnimLength")
            mJumpAnimLength = param->getValue(false).toFloat();
        //<parameter name="WalkAnimLength" ...
        else if( param->getName() == "WalkAnimLength")
            mWalkAnimLength = param->getValue(false).toInt();
        //<parameter name="FallAnimLength" ...
        else if( param->getName() == "FallAnimLength")
            mFallAnimLength = param->getValue(false).toInt();
        //<parameter name="DeathAnimLength" ...
        else if( param->getName() == "DeathAnimLength" )
            mDeathAnimLength = param->getValue(false).toInt();
        //<parameter name="Platform1Time" ...
        else if( param->getName() == "Platform1Time" )
            mPlatform1Loop = param->getValue(false).toInt();
        //<parameter name="Platform2Time" ...
        else if( param->getName() == "Platform2Time" )
            mPlatform2Loop = param->getValue(false).toInt();
        //<parameter name="Platform3Time" ...
        else if( param->getName() == "Platform3Time" )
            mPlatform3Loop = param->getValue(false).toInt();
    }
}

void S3DGameNode::createInputObjectParameter( const QList<QString>& objectNames )
{
    ParameterGroup* animatedObjectsGroup = this->getParameterGroup("Animated Objects");
    if( animatedObjectsGroup )
    {
        // Get the names of all existing input object parameters
        QList<QString> parameterNames;
        const AbstractParameter::List& animatedObjects = animatedObjectsGroup->getParameterList();
        for( int i=0; i<animatedObjects.size(); i++)
        {
            Parameter* parameter = dynamic_cast<Parameter*>(animatedObjects.at(i));
            if( parameter )
                parameterNames.append( parameter->getName() );
        }

        // Create all non-existant input parameters
        QList<QString>::const_iterator iter = objectNames.begin();
        while( iter != objectNames.end() )
        {
            QString objectName = *iter;
            if( parameterNames.contains( objectName ) )
            {
                parameterNames.removeAll( objectName );
            }
            else
            {
                // create new input parameter
                GeometryParameter* param = new GeometryParameter( objectName );
                param->setPinType(Parameter::PT_Input);
                param->setVisible(true);
                param->setSelfEvaluating(true);
                animatedObjectsGroup->addParameter(param);

                this->getParameter("Game_P1")->addAffectingParameter( param);
                this->getParameter("Game_P2")->addAffectingParameter( param);
                this->getParameter("Game_Both")->addAffectingParameter( param);
                param->setProcessingFunction( SLOT( LoadAnimatedObject()));
            }
            ++iter;
        }

        // delete all parameters which were not referenced
        iter = parameterNames.begin();
        while( iter != parameterNames.end() )
        {
            animatedObjectsGroup->removeParameter( *iter );
            ++iter;
        }
    }
}

void S3DGameNode::ResetGameRequest()
{
    ResetGame();
}

void S3DGameNode::ResetSlot()
{
    if( getBoolValue("Reset1") && getBoolValue("Reset2"))
        ResetGame();
}

void S3DGameNode::processInputGeometryP1( Ogre::SceneNode * inputGeometryP1 )
{
    mEngine.ParseEnemies( inputGeometryP1 );
}

void S3DGameNode::processInputGeometryP2( Ogre::SceneNode * inputGeometryP2 )
{
    mEngine.ParseEnemies( inputGeometryP2 );
}

void S3DGameNode::processObjectMarker( Ogre::SceneNode * inputObjectMarker )
{
    QList<QString> objectNames;
    mEngine.parseObjectMarker( inputObjectMarker, objectNames );
    createInputObjectParameter(objectNames);
}

void S3DGameNode::processInputLogic( Ogre::SceneNode * inputLogic )
{
    // use input SN instead of copy, as names of child nodes are changed by OT::deepCopySceneNode(..)!
    if( inputLogic )
    {
        // Read logic from input scene node
        mEngine.ParseLogicSceneNode(inputLogic);

        // Create Event marker from input scene node
        mEngine.CreateEventMarker(inputLogic, mEventMarker );

        // Reset game after logic has been processed
        ResetGame();
    }

}

} // namespace S3DGameNode 
