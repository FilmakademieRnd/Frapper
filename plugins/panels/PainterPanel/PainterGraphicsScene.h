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
//! \file "PainterGraphicsScene.h"
//! \brief Header file for PainterGraphicsScene class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       06.09.2012 (last updated)
//!

#ifndef PAINTERGRAPHICSSCENE_H
#define PAINTERGRAPHICSSCENE_H

#include "FrapperPrerequisites.h"
#include "PainterGraphicsItems.h"
#include "PainterPanelItemData.h"
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QBitmap>

namespace PainterPanel {

class PainterGraphicsScene: public QGraphicsScene
{
	Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PainterGraphicsScene class.
    //!
    //!
	PainterGraphicsScene ();

    //!
    //! Destructor of the PainterGraphicsScene class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PainterGraphicsScene ();

	//!
	//! public function for triggering a scene change
	//!
	void sceneHasChanged();

signals: 

	//!
	//! This signal is emitted when the scene has changed
	//!
	void sceneChanged();

	//!
	//! This signal is emitted when the history list is updated with new names
	//!
	void itemEntrysIntoHistory( QList<QString> nameList );

	//!
	//! This signal is emitted when a new item was created
	//!
	void itemCreated( BaseShapeItem* item );

	//!
	//! This signal is emitted when an item was changed
	//!
	void itemChanged( BaseShapeItem* item );

	//!
	//! This signal is emitted when an item was changed
	//!
	void itemKeyed( BaseShapeItem* item );

	//! 
	//! This signal is emitted when the parameters from a scene item should be deleted
	//!
	void itemDeleted( QString itemName );

	//!
	//! This signal is emitted when a LUT should be used to display the scene and items
	//!
	void useLUT( bool val);

public slots:

	//!
	//!
	//!
	void keySelectedItems();

public: //functions

	//!
	//! Returns all selected Items as a String List
	//!
	QList<QString> getSelectedItems();

	//!
	//! Get color value from lookup table
	//!
	const QRgb lut( int i ) const;

    //!
	//! Set the background image as none selectable graphics pixmap item
    //!
    void setBackground( QImage image );

	//!
	//! Set the mask image as none selectable graphics pixmap item
	//!
	void setMask( QImage mask );

	//!
	//! Set the layer image with id as none selectable graphics pixmap item
	//!
	void setLayer( int id, QImage image );

    //!
    //! Get the background image as graphics pixmap item
    //!
    QGraphicsItem* getBackground();

	//!
	//! Create a new GraphicsItem at the given position
	//! with the given item type
	//!
	//! \param pos The initial position of the new graphics item
	//! \param itemType the item type of the graphics item to create
	//! \param name The name of the item to create. Will be generated of omitted.
	//!
	BaseShapeItem* createGraphicsItem( QPointF pos, BaseShapeItem::ItemType itemType, QString name = "", QPen pen = QPen(), QBrush brush = QBrush() );
	//!
	//! Create a new GraphicsItem with the given values
	//! and adds it to the current scene
	//!
	//! \param pos The initial position of the new graphics item
	//!
	BaseShapeItem* createGraphicsItem( const PainterPanelItemData& itemData );

	//!
	//! Updates a GraphicsItem according to the given parameters
	//!
	//! \param position The position of the graphics item
	//! \param rotation The rotation of the graphics item
	//! \param points   The points that define the graphics item
	//! \param controlPoints Additional (optional) control points of the graphics item
	//!
	void updateGraphicsItem( const PainterPanelItemData& itemData );

	//!
	//! This function is called for restoring all item names in the history widget
	//!
	void updateGraphicsItemNames( );

	//!
	//! Get the graphics item with the given name
	//! \param name The name of the graphics item
	//!
	BaseShapeItem* getGraphicsItemByName( const QString& name );

	//!
	//! Delete the graphics item with the given name from the scene
	//! \param name The name of the graphics item
	//!
	void deleteGraphicsItem( QString name );

	//!
	//! Clear the whole scene and delete all items
	//!
	void clearItems();

    //!
    //! Render the content of the scene to a QImage
    //! The size is determined by the background image
    //!
    void renderToImage( QImage &image, bool withBackground=false );

	//!
	//! Override QGraphicsScenes clear method to handle background an mask pointer
	//!
	void clear();

	//!
	//! Called when an item is modified externally
	//!
	void signalItemChanged( BaseShapeItem* );

	//!
	//! Set use of LUT
	//!
	void setUseLUT(bool val);

	const bool getUseLUT() const;

	//!
	//! set darken factor of background when in LUT mode
	//!
	void DarkenFactor(float val) { m_bgDarkenFactor = val; }

protected slots:

	//!
	//!
	//!
	void sceneItemHasChanged();

private: // data
            
    //! the background item
    QGraphicsItem *m_background;

	QGraphicsItem* m_mask;
	QGraphicsItem* m_maskLUT;

	QMap<int, QGraphicsPixmapItem*> m_layers;

	//! static instance counter for naming items
	static int s_itemInstanceCount[BaseShapeItem::NUM_ITEMTYPES];

	bool m_useLUT;
	float m_bgDarkenFactor;

}; //PainterGraphicsScene

} // end namespace PainterPanel

#endif // PAINTERGRAPHICSSCENE_H