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
//! \file "PainterPanelNodeNode.cpp"
//! \brief Implementation file for PainterPanelNodeNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       14.10.2013 (created)
//!

#include "PainterPanelNode.h"
#include "PainterGraphicsScene.h"
#include "OgreTexture.h"
#include "OgreTools.h"
#include "ParameterGroup.h"
#include "PainterPanelItemData.h"

namespace PainterPanel {
using namespace Frapper;

 
///
/// Constructors and Destructors
///

//!
//! Constructor of the PainterPanelNode class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
PainterPanelNode::PainterPanelNode( const QString &name, ParameterGroup *parameterRoot ) :
	ImageNode(name, parameterRoot, true, "image")
{
	// update item parameter upon item changes in scene
	connect( &m_scene, SIGNAL( itemCreated(BaseShapeItem*)),	this, SLOT( createParameter(BaseShapeItem*)));
	connect( &m_scene, SIGNAL( itemChanged(BaseShapeItem*)),	this, SLOT( updateParameter(BaseShapeItem*)));
	connect( &m_scene, SIGNAL( itemKeyed(BaseShapeItem*)),		this, SLOT( keyParameter(BaseShapeItem*)));
	connect( &m_scene, SIGNAL( itemDeleted( QString )),			this, SLOT( deleteParameter( QString )));
	connect( &m_scene, SIGNAL( sceneChanged()),					this, SLOT( onSceneChangedSlot()));
}


//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PainterPanelNode::~PainterPanelNode ()
{
}

void PainterPanelNode::updateBackground()
{
	// update background image of scene
	Ogre::TexturePtr imgPtr = getBackground();

	if (!imgPtr.isNull()) {

		QImage image;
		OgreTools::OgreTextureToQImage( imgPtr, image);
		m_scene.setBackground( image );
	} else {
		m_scene.setBackground(QImage());
	}
}

void PainterPanelNode::updateMask()
{
	// update mask image
	Ogre::TexturePtr maskPtr = getMask();

	if( !maskPtr.isNull())
	{					
		QImage mask;
		OgreTools::OgreTextureToQImageMask( maskPtr, mask);
		m_scene.setMask( mask );
	} else {
		m_scene.setMask( QImage() );
	}

}

void PainterPanelNode::setLayer( int id, Ogre::TexturePtr& image, QColor color )
{
	QImage edgeLayer;
	Frapper::OgreTools::OgreTextureToQImageMask( image, edgeLayer, 0, color.rgba() );
	m_scene.setLayer(0, edgeLayer );
}

void PainterPanelNode::renderSceneToImage( Ogre::TexturePtr& image, size_t width /*= 0*/, size_t height /*= 0 */ )
{
	if( !image.isNull())
	{
		if( width == 0)  width = image->getWidth();
		if( height == 0) height = image->getHeight();

		QImage view( width, height, QImage::Format_ARGB32_Premultiplied );

		// render current view to QImage
		m_scene.renderToImage( view );

		// write QImage to Ogre Texture
		OgreTools::QImageToOgreTexture( view, image, image->getFormat() );
	}
}

void PainterPanelNode::createItems( int time )
{
	if( time < 0)
		time = m_timeParameter->getValue().toInt();

	ParameterGroup* itemGroup = getItemParameterGroup();

	if( itemGroup ) 
	{
		foreach( AbstractParameter* absParam, itemGroup->getParameterMap().values()) 
		{
			ParameterGroup* itemParameter = dynamic_cast<ParameterGroup*>( absParam );
			if( itemParameter )
			{
				createItem(itemParameter, time);
			}
		}
	}
}

void PainterPanelNode::updateItems( int time )
{
	if( time < 0)
		time = m_timeParameter->getValue().toInt();

	ParameterGroup* itemGroup = getItemParameterGroup();

	if( itemGroup ) 
	{
		foreach( AbstractParameter* absParam, itemGroup->getParameterMap().values()) 
		{
			ParameterGroup* itemParameter = dynamic_cast<ParameterGroup*>( absParam );
			if( itemParameter )
			{
				updateItem(itemParameter, time);
			}
		}
	}
}

void PainterPanelNode::itemParameterChanged()
{
	Parameter* nameParameter = dynamic_cast<Parameter*>( sender() );
	if( nameParameter )
	{
		ParameterGroup* itemParameter = getItemParameterGroup()->getParameterGroup( nameParameter->getValue().toString(), false );
		if( itemParameter ) {
			// use time from time slider
			int time = m_timeParameter->getValue().toInt();
			updateItem(itemParameter, time);
		}
	}
}

void PainterPanelNode::createParameter( BaseShapeItem* shapeItem )
{
	if( shapeItem ) {

		int time = m_timeParameter->getValue().toInt();

		// read item data from shape item
		PainterPanelItemData itemData( shapeItem, time );

		// write item data to item parameter group of current node
		itemData.toParameter( getItemParameterGroup() );
	}
}


void PainterPanelNode::updateParameter( BaseShapeItem* shapeItem )
{
	if( shapeItem ){

		int time = m_timeParameter->getValue().toInt();

		// read item data from shape item
		PainterPanelItemData itemData( shapeItem, time );

		// write item data to item parameter group of current node
		itemData.toParameter( getItemParameterGroup() );
	}
}

void PainterPanelNode::keyParameter( BaseShapeItem* shapeItem, int time )
{
	if( shapeItem ){

		if( time < 0)
			time = m_timeParameter->getValue().toInt();

		// read item data from shape item
		PainterPanelItemData itemData( shapeItem );

		// set key on item parameter in parameter group of current node
		itemData.setParameterKey( getItemParameterGroup(), time );
	}
}

void PainterPanelNode::deleteParameter( const QString& itemName )
{
	if( itemName.isEmpty() ) return;

	ParameterGroup* itemGroup = getItemParameterGroup();
	ParameterGroup* itemParameter = itemGroup->getParameterGroup( itemName, false );

	if( itemParameter ) {
		itemParameter->destroyAllParameters();
		itemGroup->removeParameterGroup( itemName );
	}
}

void PainterPanelNode::createItem( ParameterGroup* itemParameter, int time )
{
	if( time < 0)
		time = m_timeParameter->getValue().toInt();

	PainterPanelItemData itemData( itemParameter, time );
	m_scene.createGraphicsItem( itemData );
}

void PainterPanelNode::updateItem( ParameterGroup* itemParameter, int time )
{
	if( time < 0)
		time = m_timeParameter->getValue().toInt();

	PainterPanelItemData itemData( itemParameter, time );
	m_scene.updateGraphicsItem( itemData );
}

void PainterPanelNode::onSceneChangedSlot()
{
	onSceneChanged();
}

void PainterPanelNode::rebuildScene( int time )
{
	// block scene events while rebuilding the scene
	m_scene.blockSignals(true);

	// clear current scene content
	m_scene.clear();

	// update current background
	updateBackground();

	// update mask image
	updateMask();

	// create items for current frame
	createItems( time );
	
	m_scene.blockSignals(false);
}

void PainterPanelNode::updateScene( int time )
{
	// block scene events while updating the scene
	m_scene.blockSignals(true);

	// update current background
	updateBackground();

	// update mask image
	updateMask();

	// update items for current frame
	updateItems( time );

	m_scene.blockSignals(false);
}

void PainterPanelNode::setUseLUT( bool val )
{
	m_scene.setUseLUT(val);
	updateBackground();
	updateMask();
}

void PainterPanelNode::setBGDarkenFactor( float val )
{
	if( 0.0 <= val && val <= 1.0f)
		m_scene.DarkenFactor(val);
	updateBackground();
}

} // namespace PainterPanel