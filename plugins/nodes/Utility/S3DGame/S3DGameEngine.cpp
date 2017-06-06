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
//! \file "S3DGameEngineEngine.cpp"
//! \brief Implementation file for S3DGameEngineEngine class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.03.2012
//!

#include "S3DGameEngine.h"
#include "OgreManager.h"
#include "OgreException.h"

namespace S3DGameNode {
using namespace Frapper;

///
/// Initialization of non-integral static const class members
///

const QString S3DGameEngine::PathPrefix = "Path";
const QString S3DGameEngine::StartPath  = "Path1";

///
/// Constructors and Destructors
///

//!
//! Constructor of the S3DGameEngine class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
S3DGameEngine::S3DGameEngine () 
{}

//!
//! Destructor of the S3DGameEngine class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
S3DGameEngine::~S3DGameEngine ()
{
    // Delete enemy objects
    foreach( Enemy* enemy, mEnemies )
    {
        delete enemy;
    }

    // Delete game events
    foreach( GameEvent* gameEvent, mGameEvents)
    {
        delete gameEvent;
    }

    // Delete event marker
    foreach( EventMarker* eventMarker, mEventMarkers )
    {
        if(eventMarker)
            OgreTools::deepDeleteSceneNode(eventMarker->Geometry(), OgreManager::getSceneManager(), true );
        delete eventMarker;
    }

    // destroy all previously created obstacles
    foreach( Obstacle* obstacle, mObstacles )
    {
        if(obstacle)
            OgreTools::deepDeleteSceneNode( obstacle->Geometry(), OgreManager::getSceneManager(), true );
    }

    foreach( AnimatedObjectMarker* marker, mAnimatedObjectMarkers ) {
        delete marker;
    }
    mAnimatedObjectMarkers.clear();

    mEnemies.clear();
    mGameEvents.clear();
    mEventMarkers.clear();
    mObstacles.clear();
}
///
/// Public Functions
///


Path* S3DGameEngine::GetPathByName( const QString &action )
{
    PathMap::iterator PathIter = mPaths.find( action );

    if( PathIter != mPaths.end())
        return PathIter.value();

    return NULL;
}

Path* S3DGameEngine::CreatePath( QString startMarkerName, QString endMarkerName, QString type )
{
    Path* resultPath = NULL;

    Ogre::SceneNode* startMarker = mMarker.value(startMarkerName, NULL);
    Ogre::SceneNode* endMarker    = mMarker.value(endMarkerName,   NULL);

    if( mMarker.contains(startMarkerName) && mMarker.contains(endMarkerName))
    {
        assert( startMarker );
        assert( endMarker );

        if( type == "Sphere" )
        {
            resultPath = new SpherePath( startMarker->getPosition(), endMarker->getPosition());
        }
        else if( type == "Line" )
        {
            resultPath = new LinePath( startMarker->getPosition(), endMarker->getPosition() );
        }
        else if( type == "Sling" )
        {
            resultPath = new SlingPath( startMarker->getPosition(), endMarker->getPosition(), startMarker->getOrientation() );
        }
        if( type == "Still" )
        {
            resultPath = new StillPath( startMarker->getPosition(), startMarker->getOrientation());
        }
    }
    else if( mMarker.contains(startMarkerName) )
    {
        assert( startMarker );

        if( type == "Still" )
        {
            resultPath = new StillPath( startMarker->getPosition(), startMarker->getOrientation());
        }
    }

    return resultPath;
}

void S3DGameEngine::ParseLogicSceneNode( Ogre::SceneNode * inputLogic )
{
    
    // parsing the the input scene node is done by
    // 1) Read in all markers, as they define where paths start and end
    // 2) Read in all paths, as they use the marker positions
    // 3) Read in all events, as they build up on the paths and connect them logically

    if( inputLogic )
    {
        // 1) Read markerPosition marker from scene
        ParseMarkers(inputLogic);

        // 2) Read paths from input logic scene node
        ParsePaths(inputLogic);

        // 3) After all paths are known, read events from input logic scene node
        //    Events connect paths by actions of players
        ParseEvents(inputLogic);
    }
}

void S3DGameEngine::ParseEnemies( SceneNode* sceneNode )
{
    // Find all scene nodes containing the string "Enemy"
    QList<Ogre::SceneNode*> enemySceneNodeList;
    OgreTools::getSceneNodesByName( sceneNode, "Enemy", enemySceneNodeList);

    // Grab exact name of enemy and use as identifier in hash map
    QRegExp rx("Enemy[0-9]+");

    foreach( Ogre::SceneNode* enemySceneNode, enemySceneNodeList )
    {
        if( enemySceneNode && rx.indexIn( QString::fromStdString( enemySceneNode->getName())) > -1)
        {
            QString enemyName = rx.cap(0);
            Enemy* enemy = new Enemy( enemySceneNode );

            mEnemies.insert( enemyName, enemy);
        }
    }
}


void S3DGameEngine::ParseMarkers( Ogre::SceneNode* sceneNode )
{
    mMarker.clear();

    assert( sceneNode );

    // get scene node with paths
    Ogre::Node* markerNode = NULL;
    try {
        markerNode = sceneNode->getChild( "MarkerNode" );
    } catch (...) {
        Log::debug( "The logic node does not contain a child node named \"MarkerNode\"", "S3DGameEngine::ParseMarkers");
        return;    
    }

    // Find all scene nodes containing the string "Marker"
    QList<Ogre::SceneNode*> markerSceneNodeList;
    OgreTools::getSceneNodesByName( sceneNode, "Marker", markerSceneNodeList);

    // Grab exact name of Marker and use as identifier in hash map
    QRegExp regex("Marker[0-9]+");

    // Read in marker scene nodes
    foreach( Ogre::SceneNode* markerSceneNode, markerSceneNodeList )
    {
        if( markerSceneNode && regex.indexIn( QString::fromStdString( markerSceneNode->getName())) > -1 )
        {
            QString markerName = regex.cap(0);
            mMarker.insert( markerName, markerSceneNode );
        }
    }
}

void S3DGameEngine::ParsePaths( Ogre::SceneNode* sceneNode )
{
    mPaths.clear();

    assert( sceneNode );

    // get scene node with paths
    Ogre::Node* pathsNode = NULL;
    try {
        pathsNode = sceneNode->getChild( "PathsNode" );
    } catch (...) {
        Log::debug( "The logic node does not contain a child node named \"PathsNode\"", "S3DGameEngine::ParsePaths");
        return;    
    }

    // Iterate through and process childs of paths node
    Ogre::SceneNode::ChildNodeIterator itPaths = pathsNode->getChildIterator();
    while ( itPaths.hasMoreElements() )
    {
        // Add OgreContainer to immediate child nodes
        Ogre::SceneNode* pChildNode = dynamic_cast<Ogre::SceneNode*>(itPaths.getNext());
        if( pChildNode )
        {
            // Ogre Exporter appends "Node" to each Maya Node
            QString pathName = QString::fromStdString(pChildNode->getName()).remove("Node");

            Ogre::UserObjectBindings& refUserObjectBindings = pChildNode->getUserObjectBindings();
            const Ogre::Any& refStartUO = refUserObjectBindings.getUserAny("START");
            const Ogre::Any& refEndUO   = refUserObjectBindings.getUserAny("END");
            const Ogre::Any& refTypeUO  = refUserObjectBindings.getUserAny("TYPE");
            const Ogre::Any& refLengthUO  = refUserObjectBindings.getUserAny("LENGTH");

            QString startMarkerName = ""; QString endMarkerName = ""; QString typeName =  "";
            float length = 0.1f;
            bool lengthOk = false;

            try
            {
                if( !refStartUO.isEmpty() )
                    startMarkerName = QString::fromStdString(Ogre::any_cast<Ogre::String>(refStartUO));
                if( !refEndUO.isEmpty() )
                    endMarkerName = QString::fromStdString(Ogre::any_cast<Ogre::String>(refEndUO));
                if( !refTypeUO.isEmpty() )
                    typeName = QString::fromStdString(Ogre::any_cast<Ogre::String>(refTypeUO));
                if( !refLengthUO.isEmpty() )
                    length = QString::fromStdString( Ogre::any_cast<Ogre::String>( refLengthUO )).toFloat(&lengthOk);
            }
            catch( Ogre::Exception& e)
            {
                Log::error( "Ogre::any_cast failed: "+QString::fromStdString( e.getDescription()), "S3DGameEngine::ParsePaths");
            }

            Path* newPath = CreatePath( startMarkerName, endMarkerName, typeName );

            if( newPath )
            {
                newPath->Name(pathName);
                if( lengthOk ) newPath->SetNormalizedLength( length );

                mPaths.insert( pathName, newPath );
                Log::debug("Path "+pathName+" of type "+typeName+" created!", "S3DGameEngine::ParsePaths");
            }
            else
            {
                Log::error("Creation of path "+pathName+" of type "+typeName+" failed!", "S3DGameEngine::ParsePaths");
            }
        }
    }
}

void S3DGameEngine::ParseEvents( Ogre::SceneNode* sceneNode )
{
    mGameEvents.clear();

    assert( sceneNode );

    // get scene node with events
    Ogre::SceneNode* eventsNode = NULL;
    try {
        eventsNode = dynamic_cast<Ogre::SceneNode*>(sceneNode->getChild( "EventsNode" ));
    } catch (...) {
        Log::debug( QString( "The logic node does not contain a child node named \"EventsNode\""));
        return;    
    }

    // iterate over all paths and look for events by name: Path%1_*
    PathMap::iterator pathsIter = mPaths.begin();
    while (pathsIter != mPaths.end()) 
    {
        QString pathName = pathsIter.key();
        Path* pPath = pathsIter.value();

        // Get all event scene nodes for current path
        QList<Ogre::SceneNode*> events;
        OgreTools::getSceneNodesByName( eventsNode, pathName+"_Event", events );

        int numEvents = 0;
        foreach( Ogre::SceneNode* pEvent, events )
        {
            if( pEvent )
            {
                numEvents++;

                QString eventName = QString::fromStdString( pEvent->getName()).remove("Node");

                // create gameEvent, read and process actions from event node
                GameEvent* gameEvent = new GameEvent(pEvent);
                Ogre::UserObjectBindings& refUserObjectBindings = pEvent->getUserObjectBindings();

                const Ogre::Any& jump      = refUserObjectBindings.getUserAny("JUMP");
                const Ogre::Any& move      = refUserObjectBindings.getUserAny("MOVE");
                const Ogre::Any& moveJump  = refUserObjectBindings.getUserAny("MOVEJUMP");
                const Ogre::Any& platform  = refUserObjectBindings.getUserAny("PLATFORM");
                const Ogre::Any& checkPoint = refUserObjectBindings.getUserAny("CHECKPOINT");

                try
                {
                    QString jumpName = "", moveName = "", moveJumpName = "",
                            platformName = "", checkPointStr = "";

                    if( !jump.isEmpty())
                    {
                        jumpName = QString::fromStdString( Ogre::any_cast<Ogre::String>(jump ));
                        gameEvent->Jump( GetPathByName( jumpName ));
                    }
                    if( !move.isEmpty())
                    {
                        moveName = QString::fromStdString( Ogre::any_cast<Ogre::String>(move ));
                        gameEvent->Move( GetPathByName( moveName ));
                    }
                    if( !moveJump.isEmpty())
                    {
                        moveJumpName = QString::fromStdString( Ogre::any_cast<Ogre::String>(moveJump ));
                        gameEvent->MoveJump( GetPathByName( moveJumpName ));
                    }
                    if( !platform.isEmpty())
                    {
                        platformName = QString::fromStdString( Ogre::any_cast<Ogre::String>(platform));
                        gameEvent->Platform( platformName );
                    }
                    if( !checkPoint.isEmpty() )
                    {
                        checkPointStr = QString::fromStdString( Ogre::any_cast<Ogre::String>(checkPoint));
                        gameEvent->Checkpoint( checkPointStr );
                    }

                    // append the event to the current path and iterate
                    pPath->Events().append(gameEvent);

                    // store game event
                    mGameEvents.append( gameEvent );

                    Log::debug( "Events of path "+pPath->Name()+": "+eventName+
                        ": JUMP->"+jumpName+"; MOVE->"+moveName+"; PLATFORM->"+platformName, "S3DGameEngine::ParseEvents");
                }
                catch( Ogre::Exception& e)
                {
                    Log::error( QString::fromStdString("Ogre::any_cast failed: "+e.getDescription()), "S3DGameEngine::ParseEvents");
                }
            }        
        }

        Log::debug( QString( pathName + " has %1 Events").arg(numEvents), "S3DGameEngine::ParseEvents");
        ++pathsIter;
    }
}

void S3DGameEngine::CreateEventMarker( Ogre::SceneNode* sceneNode, SceneNode* parent )
{
    Ogre::SceneManager* manager = OgreManager::getSceneManager();

    // destroy all previously created markers
    foreach( EventMarker* eventMarker, mEventMarkers )
    {
        if(eventMarker)
            OgreTools::deepDeleteSceneNode( eventMarker->Geometry(), manager, true );
    }
    mEventMarkers.clear();

    // Get marker geometry from input geometry scene node
    QList< Ogre::SceneNode* > eventMarkerList;
    Ogre::SceneNode *checkpointMarker = NULL, *jumpMarker = NULL, *platformMarker = NULL;
    
    OgreTools::getSceneNodesByName( sceneNode, "CheckpointMarker", eventMarkerList );
    if( !eventMarkerList.isEmpty() )
        checkpointMarker = eventMarkerList[0];
    eventMarkerList.clear();

    OgreTools::getSceneNodesByName( sceneNode, "JumpMarker", eventMarkerList );
    if( !eventMarkerList.isEmpty() )
        jumpMarker = eventMarkerList[0];
    eventMarkerList.clear();

    OgreTools::getSceneNodesByName( sceneNode, "EventMarker", eventMarkerList );
    if( !eventMarkerList.isEmpty() )
        platformMarker = eventMarkerList[0];
    eventMarkerList.clear();

    for( int i=0; i<mGameEvents.size(); i++)
    {
        // get game event
        GameEvent* g = mGameEvents.at(i);
        assert( g );

        if( g->Jump() || !g->Checkpoint().isEmpty() || !g->Platform().isEmpty() )
        {
            // create new instance of input marker geometry
            SceneNode* eventMarkerGeometryCopy = NULL;

            if( !g->Checkpoint().isEmpty())
                OgreTools::deepCopySceneNode( checkpointMarker, eventMarkerGeometryCopy, QString("EventMarkerCopy%1").arg(i), manager );
            else if( !g->Platform().isEmpty() && g->Platform() != "Door")
                OgreTools::deepCopySceneNode( platformMarker, eventMarkerGeometryCopy, QString("EventMarkerCopy%1").arg(i), manager );
            else if( g->Jump())
                OgreTools::deepCopySceneNode( jumpMarker, eventMarkerGeometryCopy, QString("EventMarkerCopy%1").arg(i), manager );

            if( !eventMarkerGeometryCopy )
                continue;

            // place marker at the event
            Vec3 position = g->SceneNode()->getPosition();
            eventMarkerGeometryCopy->setPosition( position );
            eventMarkerGeometryCopy->setOrientation(Vec3::UNIT_Y.getRotationTo(position));

            // Append new marker geometry to level geometry
            OgreManager::relocateSceneNode(eventMarkerGeometryCopy, parent );

            // store geometry in EventMarker class
            EventMarker* eventMarker = new EventMarker(eventMarkerGeometryCopy);
            mEventMarkers.append(eventMarker);
        }
    }
}

void S3DGameEngine::CreateObstacles( SceneNode * inputLogic, SceneNode* inputGeometry, SceneNode* parent )
{
    Ogre::SceneManager* manager = OgreManager::getSceneManager();

    assert( inputLogic );

    // destroy all previously created obstacles
    foreach( Obstacle* obstacle, mObstacles )
    {
        if(obstacle)
            OgreTools::deepDeleteSceneNode( obstacle->Geometry(), manager, true );
    }
    mObstacles.clear();

    // get scene node with events
    Ogre::SceneNode* obstaclesNode = NULL;
    try {
        obstaclesNode = dynamic_cast<Ogre::SceneNode*>( inputLogic->getChild( "ObstaclesNode" ));
    } catch (...) {
        Log::debug( QString( "The logic node does not contain a child node named \"ObstaclesNode\""));
        return;    
    }

    // get all children from the obstaclesNode
    QList< Ogre::SceneNode* > obstacleMarkerList;
    OgreTools::getSceneNodesByName( obstaclesNode, "Door", obstacleMarkerList );

    // this is for testing
    assert( obstacleMarkerList.size() > 0);

    // put a copy of the door obstacleMarker to every position
    int i=0;
    foreach( Ogre::SceneNode* obstacleMarker, obstacleMarkerList)
    {
        if( obstacleMarker )
        {
            Ogre::SceneNode* doorObstacleCopy = NULL;
            OgreTools::deepCopySceneNode( inputGeometry, doorObstacleCopy, QString("doorObstacleCopy%1").arg(i++), manager );

            doorObstacleCopy->setPosition( obstacleMarker->getPosition() );
            doorObstacleCopy->setOrientation( obstacleMarker->getOrientation());

            // Obtain obstacle name
            QString obstacleName = "Door";
            QRegExp regex("Door[0-9]+");
            if( regex.indexIn( QString::fromStdString( obstacleMarker->getName())) > -1 )
                obstacleName = regex.cap(0);

            // store geometry in EventMarker class
            Obstacle* obstacle = new Obstacle( doorObstacleCopy );
            mObstacles.insert( obstacleName, obstacle);

            OgreManager::relocateSceneNode( doorObstacleCopy, parent );
        }
    }
}

void S3DGameEngine::parseObjectMarker( Ogre::SceneNode * inputObjectMarker, QList<QString>& objectNames )
{
    // either use input geometry or look for scene node named "EventMarker" as geometry for the event markers
    assert( inputObjectMarker );

    Ogre::SceneManager* manager = OgreManager::getSceneManager();

    // destroy all previously created markers
    foreach( AnimatedObjectMarker* marker, mAnimatedObjectMarkers ) {
        delete marker;
    }
    mAnimatedObjectMarkers.clear();

    // Iterate through and process all object markers
    QList<Ogre::SceneNode*> objectMarkerNodes;
    OgreTools::getSceneNodesByName(inputObjectMarker, "", objectMarkerNodes);

    foreach( Ogre::SceneNode* pChildNode, objectMarkerNodes )
    {
        if( pChildNode )
        {
            QString objectName = "";
            QString visibleTo = "";

            Ogre::UserObjectBindings& refUserObjectBindings = pChildNode->getUserObjectBindings();

            const Ogre::Any& refObjectUO = refUserObjectBindings.getUserAny("OBJECT");
            const Ogre::Any& refVisibleUO = refUserObjectBindings.getUserAny("VISIBILITY");

            try {
                if( !refObjectUO.isEmpty() ) {
                    objectName = QString::fromStdString(Ogre::any_cast<Ogre::String>(refObjectUO));
                }
                if( !refVisibleUO.isEmpty() ) {
                    visibleTo = QString::fromStdString(Ogre::any_cast<Ogre::String>(refVisibleUO));
                }
            } catch( Ogre::Exception& e) {
                Log::error( "Ogre::any_cast failed: "+QString::fromStdString( e.getDescription()), "S3DGameEngine::parseObjectMarker");
            }

            if( objectName != "")
            {
                QString sceneNodeName = QString::fromStdString( pChildNode->getName() );
                AnimatedObjectMarker* marker = new AnimatedObjectMarker( pChildNode, sceneNodeName );
                marker->VisibleTo(visibleTo);

                mAnimatedObjectMarkers.insertMulti( objectName, marker);
                if( !objectNames.contains(objectName) )
                    objectNames.append(objectName);
            }
        }
    }
}

void S3DGameEngine::createAnimatedObjects( QString objectName, 
                                           Ogre::SceneNode* objectGeometry, 
                                           Ogre::SceneNode* player1, 
                                           Ogre::SceneNode* player2, 
                                           Ogre::SceneNode* both )
{

    if( !objectGeometry )
        return;

    Ogre::SceneManager* manager = OgreManager::getSceneManager();

    // Get markers according to object name
    QList<AnimatedObjectMarker*> markers = mAnimatedObjectMarkers.values(objectName);

    // destroy all previously created animated objects

    foreach( AnimatedObjectMarker* marker, markers )
    {
        OgreTools::deepDeleteSceneNode( marker->ObjectGeometry(), NULL, true );

        SceneNode* animatedObjectCopy = NULL;
        OgreTools::deepCopySceneNode( objectGeometry, animatedObjectCopy, marker->MarkerName(), manager );

        animatedObjectCopy->setPosition( marker->MarkerGeometry()->getPosition() );
        animatedObjectCopy->setOrientation( marker->MarkerGeometry()->getOrientation() );
        animatedObjectCopy->setScale( marker->MarkerGeometry()->getScale() );

        marker->ObjectGeometry( animatedObjectCopy);

        if( marker->VisibleTo() == "Player1" )
            OgreManager::relocateSceneNode(animatedObjectCopy, player1 );
        else if (marker->VisibleTo() == "Player2" )
            OgreManager::relocateSceneNode(animatedObjectCopy, player2 );
        else
            OgreManager::relocateSceneNode(animatedObjectCopy, both );
    }
}

} // namespace S3DGameNode 
