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
//! \file "PainterGraphicsView.h"
//! \brief Header file for PainterGraphicsView class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       16.01.2012 (last updated)
//!

#ifndef PAINTERGRAPHICSVIEW_H
#define PAINTERGRAPHICSVIEW_H

#include "FrapperPrerequisites.h"
#include "BaseGraphicsView.h"
#include "PainterGraphicsItems.h"
#include "PainterGraphicsScene.h"
#include "PainterPanelItemData.h"

#include "ParameterGroup.h"
#include "NumberParameter.h"
#include <QGraphicsView>
#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QWheelEvent>
#include <QListWidget>
#include <QUndoStack>


namespace PainterPanel {
	using namespace Frapper;

class PainterGraphicsView : public BaseGraphicsView
{
    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PainterGraphicsView class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    PainterGraphicsView ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the PainterGraphicsView class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PainterGraphicsView ();

    enum DrawMode { NONE = 0, PAINT, LINE, RECT, CIRCLE, ELLIPSE, POLYGON, POLYLINE, BEZIER_CURVE, ERASE, NUM_MODES };

public: // functions

    //!
    //! Set the current draw mode that defines which item is created 
    //! on interaction with the graphics view.
    //!
    //! \param drawmode The drawmode to be used
    //!
    void setDrawMode( DrawMode drawmode);

	//! Returns the current draw mode that defines which item is created 
	PainterGraphicsView::DrawMode getDrawMode();

	//! Corrects the cursor depending on the current drawmode
	void setCursor();

	//!
	//! Create Graphics Item
	//!
	BaseShapeItem* createItem( QPointF pos, BaseShapeItem::ItemType type, QString name );

	//!
	//! Recreate Graphics Item
	//!
	BaseShapeItem* recreateItem( PainterPanelItemData data );

    //!
    //! Set the current scene painter graphics scene to the given scene
    //! and listen to changes of this scene
    //!
    //! \param scene The scene to be used
    //!
    void setCurrentScene( PainterGraphicsScene *scene);

	//!
	//! Sets the current Item of the Scene
	//!
	void setCurrentItem( BaseShapeItem* item);

	//!
	//! Sets the current Item of the Scene
	//!
	BaseShapeItem* getCurrentItem();

public slots: //

    //!
    //! Call this Slot when the selected item in the scene changes
    //!
    void onSelectionChanged();

    //!
    //! Call this Slot when the brush changes
    //! Updates the currently selected items
    //!
    //! \param brush The new brush to be applied to the selected items
    //!
    void changeBrush( QBrush brush);

    //!
    //! Call this Slot when the pen changes
    //! Updates the currently selected items
    //!
    //! \param pen The new pen to be applied to the selected items
    //!
    void changePen( QPen pen);

	//!
	//! Call this Slot to adjust the viewport to the scenes background
	//!
	void adjustToBackground();

signals:

	//!
	//! Call this Signal when the selected item in the scene changes
	//!
	void itemsMovedOrRotated();

	//!
	//! Call this Signal when a Item should be created with a relating UndoCommand
	//!
	void itemToCreateOnStack( QPointF pos );

    //!
    //! This signal is emitted when the scene has changed
    //!
    void triggerRedraw();

	//!
	//! This signal is emitted when the user clicks the undo/redo shortcuts
	//!
	void undoClicked();

	//!
	//! This signal is emitted when the user clicks the undo/redo shortcuts
	//!
	void redoClicked();

    //!
    //! This signal is emitted when the drawmode has changed
    //!
    void drawModeChanged(PainterGraphicsView::DrawMode drawMode);

    //!
    //! This signal is emitted when the current pen has changed, 
    //! e.g. by selection
    //!
    void penChanged( QPen pen);

    //!
    //! This signal is emitted when the current brush has changed, 
    //! e.g. by selection
    //!
    void brushChanged( QBrush brush);

    //! 
    //! This signal is emitted when the scene selection changes
    //!
    void selectionChanged( QList<QGraphicsItem*> items );

	//! 
	//! This signal is emitted when the creation of a item is canceled
	//!
	void itemCanceled( QString item );


protected: // event handlers

	//!
	//! The overwritten event handler for the enter event.
	//!
	//! \param event The description of the enter event.
	//!
	virtual void enterEvent(QEvent *event);

	//!
    //! The overwritten event handler for the mouse move event.
    //!
    //! \param event The description of the mouse move event.
    //!
	virtual void mouseMoveEvent ( QMouseEvent * event );

    //!
    //! The overwritten event handler for the mouse press event.
    //!
    //! \param event The description of the mouse press event.
    //!
	virtual void mousePressEvent(QMouseEvent *event);

	//!
	//! The overwritten event handler for the mouse double click event.
	//!
	//! \param event The description of the mouse press event.
	//!
	virtual void mouseDoubleClickEvent( QMouseEvent *event );

    //!
    //! The overwritten event handler for the mouse release event.
    //!
    //! \param event The description of the mouse release event.
    //!
	virtual void mouseReleaseEvent(QMouseEvent *event);

	//!
	//! The overwritten event handler for the mouse wheel event.
	//!
	//! \param event The description of the mouse wheel event.
	//!
	virtual void wheelEvent( QWheelEvent *event );

    //!
    //! The overwritten event handler for the key press event.
    //!
    //! \param event The description of the key press event.
    //!
    virtual void keyPressEvent ( QKeyEvent *event );

protected: // functions

private slots:

private: // functions

    //!
    //! Step through the different draw modes
    //!
    void toggleDrawmode();

    //!
    //! Clear the current and remove all items
    //!
    void clearScene();

    //!
    //! Check if selected item is a paint item and use it as current item
    //!
	bool continuePaintItem( QPointF pos, DrawMode drawMode );

private: // data

	//!
	//! The painter graphics scene
	//!
	PainterGraphicsScene* m_scene;

    //!
    //! The graphics item which is currently created
    //!
    BaseShapeItem *m_currentItem;

    //!
    //! The current brush to apply to new graphics items
    //!
    QBrush m_brush;

    //!
    //! The current pen to apply to new graphics items
    //!
    QPen m_pen;
        
    //!
    //! The current draw mode
    //!
    DrawMode m_drawMode;

};

} // end namespace Frapper

#endif