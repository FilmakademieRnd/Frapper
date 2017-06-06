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

#ifndef S3DGAMEPATHS_H
#define S3DGAMEPATHS_H

#include "Helper.h"

// shortcut definitions
typedef Ogre::Vector3    Vec3;
typedef Ogre::Quaternion Quat;  
typedef Ogre::SceneNode  SceneNode;

namespace S3DGameNode {
using namespace Frapper;

    //!
    //! The S3DGamePath classes
    //!

    // forward declaration;
    class Path;
    class SpherePath;
    class StillPath;
    class JumpPath;
    class LinePath;
    class SlingPath;
    class FallPath;
    class GameEvent;

    // Base path type. 
    // A path is used to move the character through the level.
    // It needs to implement the GetOrientation() and GetPosition() functions
    // for an alpha value between 0 and 1.
    class Path
    {
    public: 

        enum PathType { BASE, STILL, SPHERE, JUMP, LINE, SLING, FALL };

        Path() {};
        Path( Vec3 position, Quat orientation )
         : mPosition(position), mOrientation(orientation), mLength(1.0f) {};
        Path( Vec3 startPosition, Vec3 endPosition )
         : mPosition(startPosition), mStartPos(startPosition), mEndPos(endPosition) {};
        Path( Vec3 startPosition, Vec3 endPosition, Quat orientation )
         : mPosition(startPosition), mStartPos(startPosition), mEndPos(endPosition), mOrientation(orientation) {};
        virtual ~Path() {};

        // virtual functions, to be overwriten in derived classes
        virtual Quat GetOrientation( const float& alpha ) { return mOrientation;}
        virtual Vec3 GetPosition( const float& alpha ) { return mPosition;}
        virtual Vec3 GetStartPos() const { return mStartPos;}
        virtual Vec3 GetEndPos() const { return mEndPos;}
        virtual float GetNormalizedLength() { return mLength;}
        virtual void SetNormalizedLength( float val) { mLength = val;}

        // The path type is used to distinguish between the different types of paths
        virtual PathType GetPathType() { return BASE; }

        // Every path can have several events
        QList<GameEvent*>& Events() { return mEvents; }

        // The name is only used for convenience
        QString Name() const { return mName; }
        void Name( QString val) { mName = val; }

    protected:

        Vec3 mPosition, mStartPos, mEndPos;
        Quat mOrientation;
        float mLength;
        QString mName;
        QList<GameEvent*> mEvents;
    };

    //
    // Simplest type of path. The character stands still on a fixed position with fixed orientation
    //
    class StillPath : public Path
    {
    public:

        StillPath( Vec3 position, Quat orientation) 
        : Path( position, orientation ) {}

        virtual PathType GetPathType() { return STILL;}
    };

    //
    // FallPath is like still path, but longer. 
    // Its used to play a fall animation while the camera is fixed at a certain position
    //
    class FallPath : public StillPath
    {
    public:
        FallPath( Vec3 startPosition, Quat orientation )
        : StillPath( startPosition, orientation )
        { mLength = 20.0f; };

        virtual PathType GetPathType() { return FALL;}
    };

    //
    // SpherePath is the most used type of path in the game
    // It describes the movement of the character over a sphere
    //
    class SpherePath : public Path
    {
    public:
        SpherePath( Vec3 startPosition, Vec3 endPosition, Vec3 origin = Vec3(0,0,0) )
          : Path( startPosition, endPosition ), mO(origin)
        {
            // translate positions into local coord system
            mStartPos = startPosition - origin;
            mEndPos = mEndPos - origin;

            mAngle = mStartPos.angleBetween( mEndPos );

            // Zero angles are _evil_!
            Ogre::Radian epsAngle( std::numeric_limits<float>::epsilon());
            CLAMP_LOWER( mAngle, epsAngle ); 

            mLength = mStartPos.length() * mAngle.valueRadians(); // b = r * a
            mNormal = mStartPos.crossProduct(mEndPos).normalisedCopy();
        };

        virtual Vec3 GetPosition( const float& alpha )
        {
            Quat rotation( alpha*mAngle, mNormal );
            rotation.normalise();

            // rotate to new position according to alpha
            Vec3 newPosition = rotation * mStartPos;
            newPosition.normalise();

            // Linear interpolate height between start and end position
            float height = LINEAR_INTERPOLATE( mStartPos.length(), mEndPos.length(), alpha);

            // scale normalized position according to current height
            mPosition = height * newPosition;

            // Translate result back into global coord system
            return mPosition+mO;
        }

        virtual Quat GetOrientation( const float& alpha )
        {
            Quat orientation;

            // calculate PC up-vector, last calculated position
            Vec3 up = mPosition;
            up.normalise();

            //// calculate PC view direction
            Vec3 dir = Ogre::Plane(up, 0.0f).projectVector( mEndPos - mStartPos );
            dir.normalise();

            // calculate right vector
            Vec3 right = up.crossProduct(dir);
            right.normalise();

            mOrientation = Quat(right, up, dir);
            mOrientation.normalise();

            return mOrientation;
        }
        virtual PathType GetPathType() { return SPHERE;}

    protected:

        // Cartesian Coordinates
        Ogre::Radian mAngle;
        Vec3 mNormal;
        Vec3 mO;
    };

    //
    // A JumpPath is created dynamically between two other paths
    // Previously, the jump was done by adjusting the height of the character,
    // while he moves along the path. In the current version,
    // the height adjustment is baked into the character animation
    //
    class JumpPath : public SpherePath
    {
    public:
        JumpPath( Vec3 startPosition, Quat startOrientation, 
                  Vec3 endPosition, Quat endOrientation )
        : SpherePath( startPosition, endPosition ),
          mStartOrientation(startOrientation),
          mEndOrientation(endOrientation)
        {};

        //virtual Vec3 GetPosition( const float& alpha )
        //{
        //    // get updated position on sphere path
        //    Vec3 direction = SpherePath::GetPosition(alpha);

        //    // Add height for jump..
        //    direction.normalise();

        //    // scale direction vector according to position [0..1] and arc length of path
        //    // the function for the scale factor is [http://www.wolframalpha.com/input/?i=-4(x(x-1))y,x=0to1,y=1to5]
        //    Vec3 heightVector = direction * -4.0f * (alpha*(alpha-1)) * 0.5f * mLength * mHeightScale;
        //    

        //    return mPosition + heightVector;
        //};

        //
        // Use nlerp to get a smooth transition between the beginning and the ending orientation
        //
        virtual Quat GetOrientation( const float& alpha )
        {
            mOrientation = Quat::nlerp( alpha, mStartOrientation, mEndOrientation, true );
            return mOrientation;
        };

        virtual PathType GetPathType() { return JUMP;}

        void setLengthScale( float lengthScale ) { mLength *=lengthScale; }

    protected:
        Quat mStartOrientation, mEndOrientation;
    };

    //
    // The LinePath describes straight movement on a line 
    // from starting to ending position 
    //
    class LinePath : public Path
    {
    public:
        LinePath( Vec3 startPosition, Vec3 endPosition )
            : Path( startPosition, endPosition )
        {
            mLength = (mEndPos-mStartPos).length();
                        
            while( mLength < 0.01f)
            {
                // if start ~~ end, scale end a litte bit
                mEndPos *= 1.01f;    
                mLength = (mEndPos-mStartPos).length();
            }
            
            // calculate orientation on line path
            Vec3 up = mStartPos;
            Vec3 dir = mEndPos - mStartPos;
            Vec3 right = up.crossProduct(dir);
            up.normalise(); 
            dir.normalise(); 
            right.normalise();

            mOrientation = Quat(right, up, dir);
            mOrientation.normalise();
        };

        virtual Vec3 GetPosition( const float& alpha ) { return LINEAR_INTERPOLATE( mStartPos, mEndPos, alpha);};
        virtual PathType GetPathType() { return LINE;}

    protected:

    };

    class SlingPath : public Path
    {
    public:
        SlingPath( Vec3 startPosition, Vec3 endPosition, Quat orientation )
        : Path( startPosition, endPosition, orientation ) 
        {};

        virtual Vec3 GetPosition( const float& alpha ) 
        { return LINEAR_INTERPOLATE( mStartPos, mEndPos, alpha);};

        virtual PathType GetPathType() 
        { return SLING;}
    };

} // namespace S3DGameNode 
#endif