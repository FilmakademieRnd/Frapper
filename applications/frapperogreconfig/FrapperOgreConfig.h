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
//! \file "FrapperOgreConfig.h"
//! \brief Stand alone application to configure Ogre render options.
//!		   Can be called from within Frapper and also individually.
//!		   Overwrites ogre.cfg in the "config" folder.
//!
//! \author     Volker Helzle <volker.helzle@filmakademie.de>
//! \version    1.0
//! \date       16.02.2011 (last updated)
//!


#ifndef FRAPPEROGRECONFIG_H
#define FRAPPEROGRECONFIG_H

#include <Ogre.h>
#include <OgreLogManager.h>


class FrapperOgreConfig : public Ogre::WindowEventListener
{

public:
    FrapperOgreConfig(void);
    virtual ~FrapperOgreConfig(void);
    bool go(void);

protected:
    Ogre::Root *m_root;    
    Ogre::String m_resourcesCfg;
    Ogre::String m_pluginsCfg;
};

#endif

