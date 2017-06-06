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
//! \file "ObjectTextDisplay.h"
//! \brief Header file for ObjectTextDisplay class.
//!
//! \author     Volker Helzle <volker.helzle@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       26.11.2012 (last updated)
//!

#ifndef OBJECTTEXTDISPLAY_H
#define OBJECTTEXTDISPLAY_H

#include "Node.h"

#include <Ogre.h>
#include <OgreFont.h>
#include <OgreFontManager.h>


namespace Frapper {

//!
//! Class representing nodes that can contain OGRE entities with animation.
//!
class ObjectTextDisplay : public QObject
{

		Q_OBJECT
    

public: // constructors and destructor

    //!
    //! Constructor of the ObjectTextDisplay class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ObjectTextDisplay ( Ogre::String name, Ogre::Bone* p, Ogre::Camera *c, Ogre::Entity *ent );
	//ObjectTextDisplay(const Ogre::MovableObject *p, const Ogre::Camera *c, Ogre::String name);

    //!
    //! Destructor of the ObjectTextDisplay class.
    //!
	~ObjectTextDisplay() ;

public slots:


	void enable(bool enable);

	void setText(const Ogre::String& text);

	void update();

	Ogre::Vector3 GetBoneWorldPosition(Ogre::Entity* ent, Ogre::Bone* bone);
	void destroyOverlay();

protected:
	
	//const Ogre::MovableObject *m_p;
	Ogre::Bone *m_p;
	const Ogre::Camera *m_c;
	Ogre::Entity *m_ent;
	bool m_enabled;
	Ogre::Overlay *m_pOverlay;
	Ogre::OverlayElement *m_pText;
	Ogre::OverlayContainer *m_pContainer;
	Ogre::String m_text;

	
};

}

#endif
