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
//! \file "PainterPanel.h"
//! \brief Header file for PainterPanel class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author		Stefan Seibert <stefan.seibert@filmakademie.de>
//! \version    2.0
//! \date       02.10.2013 (created)
//! \date       23.01.2014 (edited)
//!

#ifndef PAINTERPANEL_H
#define PAINTERPANEL_H

// includes, local classes
#include "PainterGraphicsView.h"
#include "PainterGraphicsItems.h"
#include "PainterPanelNode.h"
#include "PainterPanelItemData.h"
#include "ui_PainterPanelMainArea.h"
#include "ui_PainterPanelToolBar.h"

// includes, Frapper
#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "Parameter.h"
#include "ImageNode.h"
#include "RenderNode.h"
#include "Log.h"

// includes QT
#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QGraphicsItem>
#include <QHBoxLayout>
#include <QItemSelection>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QUndoStack>
#include <QVBoxLayout>

namespace PainterPanel {
using namespace Frapper;

class ToolSelectCommand;
class PenSizeCommand;
class PenColorCommand;
class PenOpacityCommand;
class FillColorCommand;
class FillToogleCommand;
class CreateItemCommand;
class DeleteItemCommand;

//!
//! Class representing a view for displaying and editing parameters of nodes.
//!
class PainterPanel : public ViewPanel
{

    Q_OBJECT

    // Declare ViewNode as friend class to access node functions
    friend class ViewNode;

public: // constructors and destructors

    //!
    //! Constructor of the PainterPanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    PainterPanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the PainterPanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PainterPanel ();

public: // functions


	//! Wrapper for the scene method
	BaseShapeItem* getGraphicsItemByName(QString name);

	//! Returns the top parameter group of the current pp node
	ParameterGroup* getItemParameterGroup();

	//!
	//! Connects the panel with the scene.
	//!
	//! \param *nodeModel NodeModel of the scene
	//! \param *sceneModel SceneModel of the scene
	//!
	virtual void registerControl(NodeModel *nodeModel, SceneModel *sceneModel);

	//!
	//! Fills the given tool bars with actions for the PainterPanel view.
	//!
	//! \param mainToolBar The main tool bar to fill with actions.
	//! \param panelToolBar The panel tool bar to fill with actions.
	//!
	void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

	//! Public Access for changing the pen size, should only be 
	//! accessed by the related undo/redo actions directly
	void changingPenSize( int size );

	//! Public Access for changing the pen color, should only be 
	//! accessed by the related undo/redo actions directly
	void changingPenColor( int color );

	//! Public Access for changing the fill color, should only be 
	//! accessed by the related undo/redo actions directly
	void changingFillColor( int color );

	//! Public Access for changing the pen opacity, should only be 
	//! accessed by the related undo/redo actions directly
	void changingPenOpacity( int opacity );

	//! Public Access for changing the fill status, should only be 
	//! accessed by the related undo/redo actions directly
	void changingFillStatus( bool value );

	//! Public Access for removing an item 
	void removeItem( QString name );

	//! Public Access for adding a item to the history
	void addItemToHistory( QString name );

	//! Public Access for adding a item
	QString addItem( QPointF pos, BaseShapeItem::ItemType type, QString name );

	//! Public Access for recreating a item
	void recreateItem( PainterPanelItemData data );

public: // static member

    //!
    //! Instance counter, used to clear the above list of items after the last instance has been destroyed
    //!
    static size_t s_numInstances;

signals:
	//!
	//! Signal that an item was deleted from the scene
	//!
	void itemDeleted( QString itemName );

	//!
	//! This signal is emitted when the scene has changed
	//!
	void triggerRedraw();

public slots:

	//!
	//! Slot which is called when the user changes the draw mode via combo box
	//!
	void changeDrawMode(PainterGraphicsView::DrawMode drawMode);

protected slots:


	//!
	//! The slot for being called from view when the mouse is released after movement or rotation
	//!
	void onItemMovementOrRotation();

	//!
	//! Is called when a item should be deleted from history
	//!
	//! \param name the name of the item that should be removed from history
	//!
	void removeItemFromHistory( QString name );

	//!
	//! Is called when a node is selected
	//!
	//! \param selecedNode the selected which was selected
	//!
	void selectedNodeChanged(Node * selectedNode);

	//!
	//! Updates the scene and the panel if the node selection changes
	//!
	//! \param enabled True if selected, false otherwise
	//!
	void onSceneModelSelectionChanged(bool selected);

	void onFrameChanged( int currentFrame );

	//! This method is called when the selected items should be deleted
	void deleteSelectedItems();

	//!
	//! Is called when the displayed scene changes
	//!
	//! \param scene The new scene that is displayed
	//!
	void sceneChanged( QGraphicsScene *scene);

    //!
    //! Slot which is called when an item in the history list is selected
    //!
	void historyListItemSelectionChanged ();

	//! Finds out which button in the GUI has been clicked
	void buttonClicked ();

    //!
    //! Slot which is called when the scene selection changes
    //!
    void onSceneSelectionChanged(QList<QGraphicsItem*> selectedItems);

    //!
    //! Slot which is called when the current pen changes, 
    //! e.g. by selecting a different item in the scene
    //!
    void onPenChanged( QPen pen );

    //!
    //! Slot which is called when the current brush changes, 
    //! e.g. by selecting a different item in the scene
    //!
    void onBrushChanged( QBrush brush );
    
	//!
    //! Slot which is called when checked state of the fill check box changes
    //!
    void onFillCheckToggled( bool checked );

	//!
	//! Slot which is called when the user selects a different pen color
	//!
	void onPenColorValueChanged( int color );

    //!
    //! Slot which is called when the user selects a different pen size
    //!
    void onPenSizeValueChanged( int size );

	//!
	//! Slot which is called when the opacity changes
	//!
	void onPenOpacityValueChanged( int opacity );

    //!
    //! Slot which is called when the user selects a different brush color
    //!
    void onFillColorValueChanged( int color );

	//!
	//! Is called when a item has to be created
	//!
	void onItemCreation( QPointF pos );

	//!
	//! Is called when a item needs to added to the history list
	//!
	void onEntrysIntoHistory( QList<QString> nameList );

	//!
	//! Enable or disable usage of LUT in PainterPanel
	//!
	void setUseLUT( bool val);

private: 

	//!
	//! Resets the painter panel to its initial state
	//!
	void resetPanel();

    //!
    //! Sets the color of the pen color button
    //!
    void setPenColorSlider( QColor color);

	//!
	//! Sets the color of the pen color button
	//!
	void setOpacitySlider( QColor color);

    //!
    //! Sets the color of the brush color button
    //!
    void setBrushColorSlider( QColor color);

	//!
	//! Sets the value of the opacity spin box
	//!
	void setOpacitySpinBox( int value );

    //! 
    //! Set the standard pen color for the selected draw mode
    //! 
    void setStandardPenColor( PainterGraphicsView::DrawMode drawMode );

	//!
	//! Sets all Buttons of the Toolbar in unchecked Mode
	//!
	void uncheckAll();

	virtual QString getPanelTypeName();

protected:


private: // data

	//!
	//! The overwritten event handler for the key press event.
	//!
	//! \param event The description of the key press event.
	//!
	virtual void keyPressEvent ( QKeyEvent *event );

	//!
	//! Node model
	//!
	NodeModel *m_nodeModel;

	//!
	//! Scene model
	//!
	SceneModel *m_sceneModel;

	//!
	//! The graphics scene
	//!
	PainterGraphicsScene* m_scene;

	//!
	//! The Undo Stack for saving undo/redo actions
	//!
	QUndoStack* m_undoStack;

	//!
	//! selected node
	//!
	PainterPanelNode *m_painterPanelNode;
    
    //! The draw mode
    PainterGraphicsView::DrawMode m_drawMode;

	//! current pen, brush, pencolor & brushcolor
    int m_opacity;
	QColor m_penColor;
    QColor m_brushColor;
    QBrush m_brush;
    QPen m_pen;

	//! The list widget holding the images of the currently selected node
	QListWidget *m_historyListWidget;

	//! The UI Toolbar
	Ui::PainterPanelToolBar *m_toolbar;

	//! The UI Main Area
	Ui::PainterPanelMainArea* m_mainArea;

	//! The painter area, a QGaphicsView
	PainterGraphicsView* m_painterArea;

};

} // end namespace PainterPanel

#endif
