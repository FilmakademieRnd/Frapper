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
//! \file "ObjectTextDisplay.cpp"
//! \brief Header file for ObjectTextDisplay class.
//!

#include "ObjectTextDisplay.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreSingleton.h>


namespace Frapper {

Ogre::Vector3 ObjectTextDisplay::GetBoneWorldPosition(Ogre::Entity* ent, Ogre::Bone* bone)
{
	Ogre::Vector3 world_position = bone->_getDerivedPosition();

	//multiply with the parent derived transformation
	Ogre::Node *pParentNode = ent->getParentNode();
	Ogre::SceneNode *pSceneNode = ent->getParentSceneNode();
	while (pParentNode != NULL)
	{
		//process the current i_Node
		if (pParentNode != pSceneNode)
		{
			//this is a tag point (a connection point between 2 entities). which means it has a parent i_Node to be processed
			world_position = pParentNode->_getFullTransform() * world_position;
			pParentNode = pParentNode->getParent();
		}
		else
		{
			//this is the scene i_Node meaning this is the last i_Node to process
			world_position = pParentNode->_getFullTransform() * world_position;
			break;
		}
	}
	return world_position;
}



ObjectTextDisplay::ObjectTextDisplay ( Ogre::String name, Ogre::Bone* p, Ogre::Camera *c, Ogre::Entity *ent )
{
	m_p = p;
	m_ent = ent;
	m_c = c;
	m_enabled = false;
	m_text = "";

	//create an overlay for later
	m_pOverlay = Ogre::OverlayManager::getSingleton().create(name);
	m_pContainer = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", name + "container"));

	m_pOverlay->add2D(m_pContainer);

	m_pText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", name + "shapeNameText");
	m_pText->setDimensions(1.0, 1.0);
	m_pText->setMetricsMode(Ogre::GMM_PIXELS);
	m_pText->setPosition(0, 0);

	m_pText->setParameter("font_name", "BlueHighway");
	m_pText->setParameter("char_height", "10");
	m_pText->setParameter("horz_align", "center");
	m_pText->setColour(Ogre::ColourValue(1.0, 1.0, 1.0));

	m_pContainer->addChild(m_pText);
	m_pOverlay->show();
}

ObjectTextDisplay::~ObjectTextDisplay() {

	// overlay cleanup -- Ogre would clean this up at app exit but if your app
	// tends to create and delete these objects often it's a good idea to do it here.
	/*
	m_pOverlay->hide();
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	m_pContainer->removeChild("shapeNameText");
	m_pOverlay->remove2D(m_pContainer);
	overlayManager->destroyOverlayElement(m_pText);
	overlayManager->destroyOverlayElement(m_pContainer);
	overlayManager->destroy(m_pOverlay);
	*/
}

void ObjectTextDisplay::enable(bool enable) {
	m_enabled = enable;
	if (enable)
		m_pOverlay->show();
	else
		m_pOverlay->hide();
}

void ObjectTextDisplay::setText(const Ogre::String& text) {
	m_text = text;
	m_pText->setCaption(m_text);
}

void ObjectTextDisplay::update()
{
	if (!m_enabled)
		return;

	//Ogre::Vector3 v = m_p->_getDerivedPosition() ;
	Ogre::Vector3 v = GetBoneWorldPosition(m_ent, m_p);
	Ogre::Matrix4 pM = m_c->getProjectionMatrix();
	Ogre::Matrix4 vM = m_c->getViewMatrix();
	Ogre::Vector3 ScreenPos = pM * vM * v;

	float screenX = ScreenPos.x;
	float screenY = ScreenPos.y;

	ScreenPos.x = (ScreenPos.x)/2;
	ScreenPos.y = (ScreenPos.y)/2;
	float max_x = ScreenPos.x;
	float min_y = -ScreenPos.y;

	m_pContainer->setPosition(max_x, min_y + 0.5);

	//test 
	/*
	Ogre::String nameEntity = m_p->getName();

	if (nameEntity.compare("rArm") == 0)
	{
		// sin necesidad de click
		float pixelX = ((screenX + 1) / 2) * 1912.0;
		std::cout << "rArm " << std::endl; 
		std::cout << "screenX : " << screenX << std::endl;
		//std::cout << "max_x : " << max_x << std::endl;
		std::cout << "pixelX : " << pixelX << std::endl;

		float scaledPixelX = (640 * pixelX) / 1912.0;
		scaledPixelX = scaledPixelX + 636.0;

		std::cout << "scaledPixelX : " << scaledPixelX << std::endl;
	}

	if (nameEntity.compare("lArm") == 0)
	{
		std::cout << "lArm " << std::endl; 
		float pixelX = ((screenX + 1) / 2) * 1912.0;
		std::cout << "screenX : " << screenX << std::endl;
		//std::cout << "max_x : " << max_x << std::endl;
		std::cout << "pixelX : " << pixelX << std::endl;

		
	}

	if (nameEntity.compare("Chest") == 0)
	{
		std::cout << "Chest " << std::endl; 
		float pixelX = ((screenX + 1) / 2) * 1912.0;
		std::cout << "screenX : " << screenX << std::endl;
		//std::cout << "max_x : " << max_x << std::endl;
		std::cout << "pixelX : " << pixelX << std::endl;

		float scaledPixelX = (640 * pixelX) / 1912.0;
		scaledPixelX = scaledPixelX + 636.0;

		std::cout << "scaledPixelX : " << scaledPixelX << std::endl;
	}
	*/
	
}



void ObjectTextDisplay::destroyOverlay()
{
	m_pOverlay->hide();
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	

	m_pContainer->removeChild(m_pOverlay->getName() + "shapeNameText");
	m_pOverlay->remove2D(m_pContainer);
	overlayManager->destroyOverlayElement(m_pText);
	overlayManager->destroyOverlayElement(m_pContainer);
	overlayManager->destroy(m_pOverlay);
	
}



/***
ObjectTextDisplay::ObjectTextDisplay ( const Ogre::MovableObject *p, const Ogre::Camera *c, Ogre::String name )
{
	m_p = p;
	m_c = c;
	m_enabled = false;
	m_text = "";

	//create an overlay for later
	m_pOverlay = Ogre::OverlayManager::getSingleton().create(name);
	m_pContainer = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", name + "container"));

	m_pOverlay->add2D(m_pContainer);

	m_pText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", name + "shapeNameText");
	m_pText->setDimensions(1.0, 1.0);
	m_pText->setMetricsMode(Ogre::GMM_PIXELS);
	m_pText->setPosition(0, 0);

	m_pText->setParameter("font_name", "BlueHighway");
	m_pText->setParameter("char_height", "13");
	m_pText->setParameter("horz_align", "center");
	m_pText->setColour(Ogre::ColourValue(1.0, 1.0, 1.0));

	m_pContainer->addChild(m_pText);
	m_pOverlay->show();
}


ObjectTextDisplay::~ObjectTextDisplay() {

	// overlay cleanup -- Ogre would clean this up at app exit but if your app
	// tends to create and delete these objects often it's a good idea to do it here.

	m_pOverlay->hide();
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	m_pContainer->removeChild("shapeNameText");
	m_pOverlay->remove2D(m_pContainer);
	overlayManager->destroyOverlayElement(m_pText);
	overlayManager->destroyOverlayElement(m_pContainer);
	overlayManager->destroy(m_pOverlay);
}

void ObjectTextDisplay::enable(bool enable) {
	m_enabled = enable;
	if (enable)
		m_pOverlay->show();
	else
		m_pOverlay->hide();
}

void ObjectTextDisplay::setText(const Ogre::String& text) {
	m_text = text;
	m_pText->setCaption(m_text);
}

void ObjectTextDisplay::update()
{
	if(!m_enabled)
		return;

	//get the projection of the object's AABB into screen space
	const Ogre::AxisAlignedBox &bbox = m_p->getWorldBoundingBox(true);
	Ogre::Matrix4 mat = m_c->getViewMatrix();

	const Ogre::Vector3 *corners = bbox.getAllCorners();

	float min_x = 1.0f, max_x = 0.0f, min_y = 1.0f, max_y = 0.0f;

	//expand the screen-space bounding-box so that it completely encloses the object's AABB
	for (int i=0; i<8; i++)
	{
		Ogre::Vector3 corner = corners[i];

		//multiply the AABB corner vertex by the view matrix to get a camera-space vertex
		corner = mat * corner;

		//make 2D relative/normalized coords from the view-space vertex 
		//by dividing out the Z (depth) factor --- approximation
		float x = corner.x /corner.z + 0.5;
		float y = corner.y /corner.z + 0.5;

		if (x < min_x)
			min_x = x;

		if (x > max_x)
			max_x = x;

		if (y < min_y)
			min_y = y;

		if (y > max_y)
			max_y = y;
	}

	// we now have relative screen-space coords for the object's bounding box; here
	// we need to center the text above the BB on the top edge. The line that defines
	// this top edge is (min_x, min_y) to (max_x, min_y)

	//m_pContainer->setPosition(min_x, min_y);
	m_pContainer->setPosition(1-max_x, min_y);  // Edited by alberts: This code works for me
	m_pContainer->setDimensions(max_x - min_x, 0.1); // 0.1, just "because"
}
*****/

}
