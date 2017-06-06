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
//! \file "OgreTools.h"
//! \brief Header file for OgreTools class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef OGRETOOLS_H
#define OGRETOOLS_H

#include "FrapperPrerequisites.h"
#include "Parameter.h"
#include "Ogre.h"
#include "OgreContainer.h"

namespace Frapper {



//!
//! Static class with helper functions working on OGRE objects.
//!
class FRAPPER_CORE_EXPORT OgreTools
{

public: // static functions

	static Ogre::Entity* cloneEntity( Ogre::Entity *entity, const QString &name, Ogre::SceneManager *sceneManager );

    //!
    //! Clones an Ogre::MovableObject.
    //!
    //! Is needed because OGRE does not provide clone functions for cameras and
    //! lights.
    //!
    //! \param movableObject The object to clone.
    //! \param name The name to use for the object.
    //! \param sceneManager The scene manager to use for creating the object.
    //! \return The cloned object.
    //!
    static Ogre::MovableObject * cloneMovableObject ( Ogre::MovableObject *movableObject, const QString &name, Ogre::SceneManager *sceneManager = 0 );

    //!
    //! Creates a deep copy of the given scene node.
    //!
    //! \param sceneNode The scene node to copy.
    //! \param sceneNodeCopy The scene node to add the copied objects to (will be created if 0).
    //! \param namePrefix The prefix to use for names of copied objects.
    //! \param sceneManager The scene manager to use for creating the object.
    //!
    static void deepCopySceneNode ( Ogre::SceneNode *sceneNode, Ogre::SceneNode *&sceneNodeCopy, const QString &namePrefix, Ogre::SceneManager *sceneManager = 0 );

    //!
    //! Creates a copy of the given scene node.
    //!
    //! \param sceneNode The scene node to copy.
    //! \param name The name to use for the copied scene node.
    //! \param sceneManager The scene manager to use for creating the scene node.
    //! \return A copy of the given scene node.
    //!
    static Ogre::SceneNode * copySceneNode ( Ogre::SceneNode *sceneNode, const QString &name, Ogre::SceneManager *sceneManager = 0 );

    //!
    //! Deletes a whole scene node tree (including attached objects).
    //!
    //! \param sceneNode The scene node containing the tree to delete.
    //! \param sceneManager The scene manager to use for destroying the scene node.
    //! \param deleteRoot Flag to control whether to delete the given scene node.
    //!
    static void deepDeleteSceneNode ( Ogre::SceneNode *sceneNode, Ogre::SceneManager *sceneManager = 0, bool deleteRoot = false );

    //!
    //! Delete user any from the given scene node / movable object
    //!
    //! \param sceneNode The scene node containing the tree to delete.
    //! \param sceneManager The scene manager to use for destroying the scene node.
    //! \param deleteRoot Flag to control whether to delete the given scene node.
    //!
    static void deleteUserAnyFromSceneNode( Ogre::SceneNode *sceneNode );
    static void deleteUserAnyFromMovableObject( Ogre::MovableObject *movableObject );

    //!
    //! Returns the first entity attached to the given scene node.
    //!
    //! \param sceneNode The scene node to return the entity from.
    //! \return The first entity attached to the given scene node.
    //!
    static Ogre::Entity * getFirstEntity ( Ogre::SceneNode *sceneNode );

    //!
    //! Returns the first camera attached to the given scene node.
    //!
    //! \param sceneNode The scene node to return the camera from.
    //! \return The first camera attached to the given scene node.
    //!
    static Ogre::Camera * getFirstCamera ( Ogre::SceneNode *sceneNode );

    //!
    //! Returns the first light attached to the given scene node.
    //!
    //! \param sceneNode The scene node to return the light from.
    //! \return The first light attached to the given scene node.
    //!
    static Ogre::Light * getFirstLight ( Ogre::SceneNode *sceneNode );

    //!
    //! As the name suggests, this method returns the first attached object of a scene node, if there is one
    //!
    //! \param sceneNode The scene node to get the attached object from
    //! \return The first movable object attached to the given scene node.
    //!
    static Ogre::MovableObject* OgreTools::getFirstMovableObject( Ogre::SceneNode *sceneNode );


    //!
    //! Returns the all lights attached to the given scene node.
    //!
    //! \param sceneNode The scene node to return the light from.
    //! \param lightsList The list where the lights are stored in.
    //!
    static void OgreTools::getAllLights( Ogre::SceneNode* sceneNode, QList<Ogre::Light*>& lightsList );

    //!
    //! Returns the all cameras attached to the given scene node.
    //!
    //! \param sceneNode The scene node to return the cameras from.
    //! \param camerasList The list where the cameras are stored in.
    //!
    static void OgreTools::getAllCameras( Ogre::SceneNode* sceneNode, QList<Ogre::Camera*>& camerasList );

    //!
    //! Returns the all entity attached to the given scene node.
    //!
    //! \param sceneNode The scene node to return the entity from.
    //! \param entityList The list where the entity are stored in.
    //!
    static void OgreTools::getAllEntities( Ogre::SceneNode* sceneNode, QList<Ogre::Entity*>& entityList );


    //!
    //! Returns all child scene nodes of a given scene node whose name contains the given string
    //!
    //! \param sceneNode The scene node to start the search at.
    //! \return A list of scene nodes containing the given name.
    //!
    static void getSceneNodesByName( Ogre::SceneNode* root, QString name, QList<Ogre::SceneNode*>& sceneNodes );

    //!
    //! Returns a pointer to the OgreContainer of a given scene node.
    //!
    //! \param sceneNode The scene node to get the OgreContainer for
    //!
    static OgreContainer* getOgreContainer( Ogre::SceneNode* sceneNode);

    //!
    //! Destroy the node's Ogre resource group.
    //!
    static void destroyResourceGroup ( const QString &name );

    //!
    //! Create the node's Ogre resource group.
    //!
    static void createResourceGroup ( const QString &name, const QString &path );

    //!
    //! Copy Ogre Texture to QImage
	//!
	//! Converts Ogre::PF_L8 format to QImage::Indexed8, which does not support transparency
    //!
    static void OgreTextureToQImage( const Ogre::TexturePtr& src, QImage &dst );

	//!
	//! Copy Ogre Texture to QImage and masks out (transparent) pixels with the given maskColor.
	//!
	//! If an outColor other than 0 is given, the resulting mask will have this color
	//!
	static void OgreTextureToQImageMask( const Ogre::TexturePtr& src, QImage &dst, QRgb maskColor = 0, QRgb outColor = 0 );

    //!
    //! Copy QImage to Ogre Texture
    //!
    static void QImageToOgreTexture( const QImage &src, Ogre::TexturePtr& dst, const Ogre::PixelFormat& format = Ogre::PF_A8R8G8B8 );

public:
	//! 
	//! This class implements a locker for OgreHardwareBuffer
	//! The Buffer is locked on creation and unlocked on destruction
	//!
	//! See http://www.stroustrup.com/bs_faq2.html#finally for why we may need this
	//!
	class HardwareBufferLocker
	{
		Ogre::HardwarePixelBufferSharedPtr m_buffer;

	public:
		// C'tor, locks buffer ressource
		HardwareBufferLocker(Ogre::HardwarePixelBufferSharedPtr buffer, Ogre::HardwareBuffer::LockOptions lockOpts = Ogre::HardwareBuffer::HBL_DISCARD)
			: m_buffer(buffer) 
		{ 
			//while (m_buffer->isLocked()) {};
			m_buffer->lock(lockOpts);
		}

		// Destructor, unlocks buffer ressource
		~HardwareBufferLocker()
		{ 
			m_buffer->unlock();
		}

		// return current lock on locked ressource
		const Ogre::PixelBox& getCurrentLock() 
		{ 
			return m_buffer->getCurrentLock(); 
		}
	};

private: // static functions

    //!
    //! Returns the first movable object of the given type name contained in
    //! the given scene node.
    //!
    //! \param sceneNode The scene node to find the first object in.
    //! \param typeName The name of the type of objects to look for.
    //! \return The first movable object of the given type name contained in the given scene node.
    //!
    template <typename T>
    static T findFirstObject ( Ogre::SceneNode *sceneNode );

    //!
    //! Returns all movable objects of the given type contained in
    //! the given scene node and its ancestors.
    //!
    //! \param sceneNode The scene node to find all objects in.
    //! \param typeName The name of the type of objects to look for.
    //! \return The first movable object of the given type name contained in the given scene node.
    //!
    template <typename T>
    static void findAllObjects ( Ogre::SceneNode *sceneNode, QList<T>& objectsList );

    //!
    //! Delete user any from given object / scene node
    //!
    //! \typename T The type to delete the user any from
    //! \param t The object to delete the user any from
    //!
    template <typename T>
    static void deleteUserAnyFrom( T t );
};

} // end namespace Frapper

#endif
