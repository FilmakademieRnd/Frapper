/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2009 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "SceneNodeParameter.h"
//! \brief Header file for SceneNodeParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef SCENENODEPARAMETER_H
#define SCENENODEPARAMETER_H

#include "Parameter.h"
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace Frapper {

//!
//! Template for a class representing parameters containing scene objects.
//!
template <Parameter::Type T>
class FRAPPER_CORE_EXPORT SceneNodeParameter : public Parameter
{

public: // static data

    //!
    //! The default value for all scene node parameters.
    //!
    static const QVariant DefaultValue;

public: // constructors and destructors

    //!
    //! Constructor of the SceneNodeParameter class.
    //!
    //! \param name The name of the parameter.
    //!
    SceneNodeParameter ( const QString &name );

    //!
    //! Copy constructor of the SceneNodeParameter class.
    //!
    //! \param parameter The parameter to copy.
    //!
    SceneNodeParameter ( const SceneNodeParameter<T> &parameter );

    //!
    //! Destructor of the SceneNodeParameter class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~SceneNodeParameter ();

public: // functions

    //!
    //! Creates an exact copy of the parameter.
    //!
    //! \return An exact copy of the parameter.
    //!
    virtual AbstractParameter * clone ();

    //!
    //! Returns the OGRE scene node contained in this parameter.
    //!
    //! \return The OGRE scene node contained in this parameter.
    //!
    Ogre::SceneNode * getSceneNode ();

private: // functions

    //!
    //! Creates the scene node that is contained in the scene node parameter.
    //!
    void createSceneNode ();

    //!
    //! Destroys the scene node that is contained in the scene node parameter.
    //!
    void destroySceneNode ();

};


///
/// Template Class Declarations
///


//!
//! Type definition for geometry scene node parameters.
//!
typedef FRAPPER_CORE_EXPORT SceneNodeParameter<Parameter::T_Geometry> GeometryParameter;

//!
//! Type definition for light scene node parameters.
//!
typedef FRAPPER_CORE_EXPORT SceneNodeParameter<Parameter::T_Light> LightParameter;

//!
//! Type definition for camera scene node parameters.
//!
typedef FRAPPER_CORE_EXPORT SceneNodeParameter<Parameter::T_Camera> CameraParameter;

} // end namespace Frapper

#endif
