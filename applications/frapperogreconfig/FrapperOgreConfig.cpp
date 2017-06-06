/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2011 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
-----------------------------------------------------------------------------
*/

//!
//! \file "FrapperOgreConfig.cpp"
//! \brief Stand alone application to configure Ogre render options.
//!		   Can be called from within Frapper and also individually.
//! 	   Overwrites ogre.cfg in the "config" folder.
//!
//! \author     Volker Helzle <volker.helzle@filmakademie.de>
//! \version    1.0
//! \date       16.02.2011 (last updated)
//!

#include "FrapperOgreConfig.h"

FrapperOgreConfig::FrapperOgreConfig(void)
    : m_root(0)
{
}

FrapperOgreConfig::~FrapperOgreConfig(void)
{    
    delete m_root;
}

bool FrapperOgreConfig::go(void)
{
#ifdef _DEBUG
    m_resourcesCfg = "./config/resources_d.cfg";
    m_pluginsCfg = "./config/plugins_d.cfg";
#else
    m_resourcesCfg = "./config/resources.cfg";
    m_pluginsCfg = "./config/plugins.cfg";
#endif

	// Create Log before Root.
	Ogre::LogManager * logManager = new Ogre::LogManager();
	logManager->createLog("defaultLog", true, false, true); 

	m_root = new Ogre::Root(m_pluginsCfg, "./config/ogre.cfg");

    // Load resource paths from config file.
    Ogre::ConfigFile configFile;
    configFile.load(m_resourcesCfg);

    // Go through all sections & settings in the file.
    Ogre::ConfigFile::SectionIterator sectionIter = configFile.getSectionIterator();

    Ogre::String sectionName, typeName, archName;
    while (sectionIter.hasMoreElements())
    {
        sectionName = sectionIter.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = sectionIter.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, sectionName);
        }
    }
	
	m_root->showConfigDialog();
    return true;
}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        FrapperOgreConfig app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
