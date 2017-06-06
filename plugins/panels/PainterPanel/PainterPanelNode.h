/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "PainterPanelNode.h"
//! \brief Interface Header file for Nodes that are controlled by the PainterPanel class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       22.05.2012 (last updated)
//!

#ifndef PAINTERPANELNODE_H
#define PAINTERPANELNODE_H

#include "PainterPanelExports.h"
#include "ImageNode.h"
#include "OgreTexture.h"

#include "PainterGraphicsScene.h"

namespace PainterPanel {

//!
//! Class representing a view for displaying and editing parameters of nodes.
//!
class PAINTER_PANEL_EXPORT PainterPanelNode : public Frapper::ImageNode
{
	Q_OBJECT

public: // constructors and destructors

	//!
	//! Constructor of the PainterPanelNode class.
	//!
	PainterPanelNode( const QString &name, ParameterGroup *parameterRoot );

	//!
	//! Destructor of the PainterPanelNode class.
	virtual ~PainterPanelNode();

public: // pure virtual functions, need to be implemented in subclasses

	//!
	//! Get the background image of the node
	//!
	virtual Ogre::TexturePtr getBackground() = 0;

	//!
	//! Get the mask image of the node
	//!
	virtual Ogre::TexturePtr getMask() = 0;

	//!
	//! Get the node parameter group that stores values of painter panel items
	//!
	virtual Frapper::ParameterGroup* getItemParameterGroup() = 0;

	//!
	//! Get the Node that uses this interface, e.g. for Signal/Slot processing
	//!
	virtual Frapper::Node* getNode() = 0;

	//!
	//! React on changes of the scene
	//!
	virtual void onSceneChanged() = 0;

public: // functions

	//!
	//! Get the background image of the node
	//!
	PainterGraphicsScene* getScene() {return &m_scene;};

	//!
	//! Update the scene to a certain frame
	//! \param time If given, set scene items to frame time
	//!
	void updateScene( int time = -1);

	//!
	//! Update the scene background
	//!
	void updateBackground();

	//!
	//! Update the scene mask
	//!
	void updateMask();

	//!
	//! Add/Set layer in scene
	//!
	void setLayer( int id, Ogre::TexturePtr& image, QColor color );

	//!
	//! create and add graphics items to the scene
	//!
	void createItems( int time = -1);

	//!
	//! update items
	//!
	void updateItems( int time = -1);

	//!
	//! Render current scene to mask
	//!
	void renderSceneToImage( Ogre::TexturePtr& image, size_t width = 0, size_t height = 0 );

	//!
	//! Set usage of LUT
	//!
	void setUseLUT( bool val );

	//!
	//! set darken factor of background when in LUT mode
	//!
	void setBGDarkenFactor( float val );

protected slots:

	//!
	//! Update item when one of its parameters changes
	//!
	void itemParameterChanged();

	//!
	//! Create parameter that stores item as frapper parameter
	//! \param shapeItem The item to create the parameter from
	//!
	void createParameter( BaseShapeItem* shapeItem );

	//!
	//! Update parameter values according to given item
	//! \param shapeItem The item to update the parameter from
	//!
	void updateParameter( BaseShapeItem* shapeItem );

	//!
	//! Key parameter values according to given item
	//! \param shapeItem The item to key the parameter from
	//!
	void keyParameter( BaseShapeItem* shapeItem, int time = -1 );

	//!
	//! delete frapper parameters from given scene item
	//!
	void deleteParameter( const QString& name );

	//!
	//! This slot calls onSceneChanged() which needs to be implemented by child classes
	//! 
	void onSceneChangedSlot();

	//!
	//! Remove all items from scene and reset background and mask image
	//!
	void rebuildScene( int time = -1);

private: // functions


	//!
	//! create graphics item from parameter
	//!
	void PainterPanelNode::createItem( Frapper::ParameterGroup* itemParameter, int time = -1 );

	//!
	//! update graphics item from parameter
	//!
	void PainterPanelNode::updateItem( Frapper::ParameterGroup* itemParameter, int time = -1 );

protected:

	//!
	//! The PainterGraphicsScene associated with this PainterPanelNode
	//!
	PainterGraphicsScene	m_scene;

};

} // end namespace PainterPanel

#endif
