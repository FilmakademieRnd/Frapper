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
//! \file "OgreManager.h"
//! \brief Header file for OgreManager class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef OGREMANAGER_H
#define OGREMANAGER_H


#include "FrapperPrerequisites.h"
#include "Ogre.h"
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QColor>
// check for Ogre version >= 1.9.0 to include overlay component
#if (OGRE_VERSION >= 0x010900)
#include "OgreOverlaySystem.h"
#endif

#if (OGRE_VERSION >= 0x010700)
    #define OgreStringVector Ogre::vector<Ogre::String>
#else
    #define OgreStringVector std::vector<Ogre::String>
#endif 

namespace Frapper {

//!
//! Static class containing OGRE utility functions.
//!
class FRAPPER_CORE_EXPORT OgreManager
{

private: // type definitions


public: // static functions

    //!
    //! Creates a render window.
    //!
    //! \param name The name to use for the render window.
    //! \param width The width to use for the render window.
    //! \param height The height to use for the render window.
    //! \param miscParams Additional parameters to pass on to the render window.
    //! \return The created render window.
    //!
    static Ogre::RenderWindow * createRenderWindow ( const Ogre::String &name = "OGRE Render Window", int width = 800, int height = 600, const Ogre::NameValuePairList *miscParams = 0 );

    //!
    //! Executes the OGRE configuration dialog.
    //!
    static void execConfigDialog ();

    //!
    //! Initializes private static data of the OgreManager.
    //!
    static void initialize ( const Ogre::String &pluginFileName = "config/plugins.cfg", const Ogre::String &configFileName = "config/ogre.cfg", const Ogre::String &resourceFileName = "config/resources.cfg", const Ogre::String &logFileName = "logs/ogre.log" );

    //!
    //! Frees all resources that were used by the OgreManager.
    //!
    static void finalize ();

    //!
    //! Returns the OGRE root object.
    //!
    //! \return The OGRE root object.
    //!
    static Ogre::Root * getRoot ();

    //!
    //! Returns the OGRE scene manager.
    //!
    //! \return The OGRE scene manager.
    //!
    static Ogre::SceneManager * getSceneManager ();

    //!
    //! Creates a new scene node with the given name using the default scene
    //! manager.
    //!
    //! \param name The name for the scene node.
    //! \return The new scene node, or 0 if the scene node could not be created.
    //!
    static Ogre::SceneNode * createSceneNode ( const QString &name );

    //!
    //! Deletes a specific scene node with the given name using the default scene
    //! manager.
    //!
    //! \param sceneNode The scene node to be deleted.
    //!
    static void deleteSceneNode ( Ogre::SceneNode *sceneNode );

    //!
    //! Detach a given SceneNode from its current parent and attach to another SceneNode
    //!
    //! \param sceneNode The SceneNode to be relocated.
    //! \param newParent Where the SceneNode will be attached
    //!
    static Ogre::SceneNode* relocateSceneNode ( Ogre::SceneNode *sceneNode, Ogre::SceneNode *newParent );

    //!
    //! Deletes all attached objects of a scene node with the given name using the default scene
    //! manager.
    //!
    //! \param sceneNode The scene node with attached objects to be deleted
    //!
    static void deleteAttachedObjects( Ogre::SceneNode* sceneNode );

    //!
    //! Returns whether fullscreen mode is currently enabled.
    //!
    //! \return True if fullscreen mode is currently enabled, otherwise False.
    //!
    static bool isFullscreen ();

    //!
    //! Set Fullscreen.
    //!
    static void setFullscreen ( bool fullscreen );

	//!
    //! Returns the name of the current render system.
	//!
	//! \return The name of the current render system.
    //!
    static QString getRenderSystemName ( );

    //!
    //! Returns the given Qt color value as an OGRE color value.
    //!
    //! \param color The Qt color value to convert.
    //! \return The OGRE color value corresponding to the given Qt color value.
    //!
    static Ogre::ColourValue convertColor ( const QColor &color );

    //!
    //! Unload resource using given resource manager.
    //!
    //! \param resMgr The reource manager.
    //! \param resourceName The resource to unload.
    //!
    static void unloadResource ( Ogre::ResourceManager *resMgr, const std::string &resourceName );

    //!
    //! Load resource using given resource manager.
    //!
    //! \param resMgr The reource manager.
    //! \param resourceName The resource to load.
    //! \param resourceGroup The resource group.
    //!
    static void loadResource ( Ogre::ResourceManager *resMgr, const std::string &resourceName, const std::string &resourceGroup );

    //!
    //! Unload materials in material listed file.
    //!
    //! \param filename The material filename.
    //!
    static void unloadMaterials ( const std::string &filename );

    //!
    //! Unload vertex programs in file.
    //!
    //! \param filename The vertex program filename.
    //!
    static void unloadVertexPrograms ( const std::string &filename );

    //!
    //! Unload fragment programs file.
    //!
    //! \param filename The fragment program filename.
    //!
    static void unloadFragmentPrograms ( const std::string &filename );

    //!
    //! Reload material.
    //!
    //! \param materialName The material's name.
    //! \param groupName The group name.
    //! \param filename The filename.
    //! \param parseMaterialScript Parse the material script or not.
    //!
    static void reloadMaterial ( const std::string &materialName, const std::string &groupName, const std::string &filename, bool parseMaterialScript );

private: // static data

    //!
    //! The OGRE root object.
    //!
    static Ogre::Root *s_root;

    //!
    //! The OGRE scene manager.
    //!
    static Ogre::SceneManager *s_sceneManager;

#if (OGRE_VERSION >= 0x010900)
	//!
	//! The OGRE overlay system for ogre version 1.9+.
	//!
	static Ogre::OverlaySystem *s_overlaySystem;
#endif

    //!
    //! Flag that states whether the OgreManager has been initialized.
    //!
    static bool s_initialized;

    //!
    //! Flag that states whether next created render window should be fullscreen or not.
    //!
    static bool s_fullscreen;

	//!
	//! The Name of the currently used render system
	//!
	static Ogre::String s_rendersystemName;

};

} // end namespace Frapper

#endif
