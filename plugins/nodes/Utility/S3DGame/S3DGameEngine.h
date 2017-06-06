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
//! \file "S3DGameEngine.h"
//! \brief Header file for S3DGameEngine class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.03.2012
//!

#ifndef S3DGAMEENGINE_H
#define S3DGAMEENGINE_H

// Ogre
#include "OgreContainer.h"
#include "OgreTools.h"

// Game paths
#include "S3DGamePaths.h"


namespace S3DGameNode {
using namespace Frapper;

// forward declaration;
class SceneObject;
class EventMarker;
class Enemy;
class Obstacle;

// This class stores a game events. 
// An event consists of strings which define the actions of players
class GameEvent
{
public:
    GameEvent( Ogre::SceneNode* eventSceneNode)
        : mSceneNode(eventSceneNode),
        mJump(NULL),
        mMove(NULL),
        mMoveJump(NULL),
        mPlatform(""),
        mCheckpoint("")
    {
        // check valid pointer
        assert( mSceneNode );
        // Update the bounds of the event scene node
        mSceneNode->_updateBounds();
    };

    Path* Jump() const { return mJump; }
    void Jump(Path* val) { mJump = val; }
    Path* Move() const { return mMove; }
    void Move(Path* val) { mMove = val; }
    Path * MoveJump() const { return mMoveJump; }
    void MoveJump(Path * val) { mMoveJump = val; }
    QString Platform() const { return mPlatform; }
    void Platform( QString val) { mPlatform = val; }
    QString Checkpoint() const { return mCheckpoint; }
    void Checkpoint(QString val) { mCheckpoint = val; }
    Ogre::SceneNode* SceneNode() const { return mSceneNode; }

    // Check if this event is active for the given position
    bool isActive( Vec3 position )
    {
        return ( mSceneNode && mSceneNode->_getWorldAABB().contains(position) );
    }

private:
    Path *mJump, *mMove, *mMoveJump;
    QString mPlatform;
    Ogre::SceneNode* mSceneNode;
    QString mCheckpoint;
};



class AnimatedObjectMarker
{
public:
    AnimatedObjectMarker( SceneNode* markerGeometry, QString markerName )
    : mMarkerGeometry(markerGeometry), 
      mObjectGeometry(NULL),
      mVisibleTo(""),
      mMarkerName(markerName) {}

    ~AnimatedObjectMarker()
    {
    };

    void MarkerName(QString val) { mMarkerName = val; }
    void VisibleTo( QString val) { mVisibleTo = val; }
    void MarkerGeometry( SceneNode* val) { mMarkerGeometry = val; }
    void ObjectGeometry( SceneNode* val) { mObjectGeometry = val; }

    QString MarkerName() const { return mMarkerName; }
    QString VisibleTo() const { return mVisibleTo; }
    SceneNode* MarkerGeometry() const { return mMarkerGeometry; }
    SceneNode* ObjectGeometry() const { return mObjectGeometry; }
    
protected:
    SceneNode* mMarkerGeometry;
    SceneNode* mObjectGeometry;
    QString mMarkerName;
    QString mVisibleTo;
};



// Scene object that also updates its copies via ogre container
class SceneObject 
{
public:
    SceneObject( SceneNode* sceneNode) 
        : mGeometry(sceneNode), mContainer(NULL) 
    {
        assert(mGeometry);
        mContainer = OgreTools::getOgreContainer(sceneNode);
        assert(mContainer);
    }
    ~SceneObject() {};

    virtual void Animate() 
    {
        UpdateCopies();
    };

    void UpdateCopies()
    {
        if( mContainer )
            mContainer->updateCopies();
    }
    SceneNode* Geometry() const { return mGeometry; }
    void Geometry(SceneNode* val) { mGeometry = val; }

protected:
    SceneNode* mGeometry;
    OgreContainer* mContainer;
};

// Visual Marker for an event in the scene
class EventMarker : public SceneObject
{
public:
    EventMarker( SceneNode* sceneNode) 
        : SceneObject(sceneNode)
    {
    }
    ~EventMarker() {};

    void Animate()
    {
        if( mGeometry )
            this->mGeometry->yaw( Ogre::Radian( Ogre::Degree( 5 )), Ogre::Node::TS_LOCAL );
        UpdateCopies();
    }

private:
};

class Enemy : public SceneObject
{
public:
    Enemy( Ogre::SceneNode* geometry )
        : SceneObject( geometry )
    {
        assert( geometry );
        mDestroyed = false;
        mScale = geometry->getScale().x;
        mSpeed = Ogre::Math::RangeRandom(5.0f, 20.0f);
    }
    ~Enemy() {};

    void Destroy() 
    {
        mDestroyed = true;
    };
    void Animate() 
    {
        if( mGeometry )
        {
            mGeometry->yaw( Ogre::Radian( Ogre::Degree(mSpeed)), Ogre::Node::TS_LOCAL);
            if( mDestroyed )
            {
                float scale = mGeometry->getScale().x;
                if ( scale > 0.05f)
                {
                    scale -= 0.05f;
                    CLAMP_LOWER( scale, 0.0f );
                    mGeometry->setScale(Vec3(scale));
                }
            }
            UpdateCopies();
        }
    };
    void Reset()
    {
        if( mGeometry )
            mGeometry->setScale( Vec3(mScale));
        mDestroyed = false;
        UpdateCopies();
    }

    bool Destroyed() const { return mDestroyed; }
    void Destroyed(bool val) { mDestroyed = val; }

protected:
    bool             mDestroyed;
    float            mSpeed;
    float            mScale;
};

class Obstacle : public SceneObject
{
public:

    Obstacle( SceneNode* sceneNode )
        : SceneObject(sceneNode), 
        mOpen(false),
        mOpenState(0.0f)
    {}
    ~Obstacle() 
    {};

    void Animate()
    {
        // animate opening
        if( mOpen && mOpenState < 1.0f )
            mOpenState += 0.1f;
        else if (!mOpen && mOpenState > 0.0f )
            mOpenState -= 0.1f;
    }

    bool Open() const { return mOpen; }
    void Open(bool val) { mOpen = val; }
    void Reset() 
    { 
        mOpen = false; 
        mOpenState = 0.0f; 
        UpdateCopies();
    }

private:
    bool mOpen;
    float mOpenState;
};

class AnimatedDoor
{
public:

    AnimatedDoor( NumberParameter* param, float stepSize = 0.1f )
    : mParam(param), mOpen(false), mOpenState(0.0f), mStepSize( stepSize )
    {}
    ~AnimatedDoor()
    {};

    void Animate( float animScale = 100.0f )
    {
        // animate opening
        if( mOpen && mOpenState < 1.0f )
            mOpenState += mStepSize;
        else if (!mOpen && mOpenState > 0.0f )
            mOpenState -= mStepSize;

        CLAMP( mOpenState, 0.0f, 1.0f);
        if( mParam ) mParam->setValue( QVariant( mOpenState * animScale ), true );
    }

    void SetOpen( bool open ) { mOpen = open; }

private:
    NumberParameter* mParam;
    bool mOpen;
    float mOpenState, mStepSize;
};

//!
//! The S3DGameEngine class
//!
class S3DGameEngine
{
public:

public: // typedefs

    //!
    //! Positions of start & endnodes
    //!
    typedef QHash<QString, Path*> PathMap;
    typedef QHash<QString, Ogre::SceneNode*> MarkerMap;
    typedef QHash<QString, Enemy*> EnemyMap;
    typedef QHash<QString, Obstacle*> ObstacleMap;

    typedef QMap<QString, AnimatedObjectMarker*> AnimatedObjectsMap;
    static const QString PathPrefix;
    static const QString StartPath;

public: // constructors and destructors

    //!
    //! Constructor of the S3DGameNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    S3DGameEngine ();

    //!
    //! Destructor of the S3DGameNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~S3DGameEngine ();


public: // functions

    PathMap Paths() const { return mPaths; }
    MarkerMap Marker() const { return mMarker; }
    EnemyMap Enemies() const { return mEnemies; }
    ObstacleMap Obstacles() const { return mObstacles; }
    
    QList<GameEvent*> GameEvents() const { return mGameEvents; }
    QList<EventMarker*> EventMarkers() const { return mEventMarkers; }
    AnimatedObjectsMap AnimatedObjectMarkers() const { return mAnimatedObjectMarkers; }


private:

    PathMap mPaths;
    MarkerMap mMarker;
    EnemyMap mEnemies;
    ObstacleMap mObstacles;
    AnimatedObjectsMap mAnimatedObjectMarkers;
    QList<GameEvent*> mGameEvents;
    QList<EventMarker*> mEventMarkers;

public: //functions

    void ParseLogicSceneNode( SceneNode * inputLogic );
    void ParseEnemies( SceneNode* sceneNode );

    void parseObjectMarker( Ogre::SceneNode * inputObjectMarker, QList<QString>& objectNames );
    void createAnimatedObjects( QString objectName, Ogre::SceneNode* objectGeometry, Ogre::SceneNode* player1, Ogre::SceneNode* player2, Ogre::SceneNode* both );

    void CreateObstacles( SceneNode * inputLogic, SceneNode* inputGeometry, SceneNode* parent );
    void CreateEventMarker( Ogre::SceneNode* sceneNode, SceneNode* parent );

    void processInputGeometryP1( Ogre::SceneNode * inputGeometryP1 );
    void processInputGeometryP2( Ogre::SceneNode * inputGeometryP2 );
    void processInputGeometryBoth( Ogre::SceneNode * inputGeometryBoth );

    Path* GetPathByName( const QString &action );

private:

    void ParseMarkers(Ogre::SceneNode* sceneNode);
    void ParsePaths( Ogre::SceneNode* sceneNode);
    void ParseEvents(Ogre::SceneNode* sceneNode);

    Path* CreatePath( QString startMarkerName, QString endMarkerName, QString type );
};

} // namespace S3DGameNode 
#endif