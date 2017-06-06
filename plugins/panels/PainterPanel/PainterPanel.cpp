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
//! \file "PainterPanel.cpp"
//! \brief Implementation file for PainterPanel class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//!
//! \version    1.1
//! \date       02.10.2013 (created)
//!

#include "PainterPanel.h"
#include "PainterUndoCommands.h"

namespace PainterPanel {
	using namespace Frapper;

size_t PainterPanel::s_numInstances = 0;
    
///
/// Constructors and Destructors
///

//!
//! Constructor of the PainterPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
PainterPanel::PainterPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_PluginPanel, parent, flags),
	m_opacity( 255),
	m_penColor(Qt::white),
    m_brushColor(Qt::white),
    m_pen( QPen(Qt::white, 3)),
    m_brush( QBrush(Qt::white, Qt::NoBrush)),
	m_drawMode( PainterGraphicsView::NONE ),
	m_painterPanelNode(NULL),
	m_scene( NULL ),
	m_mainArea( new Ui::PainterPanelMainArea()),
	m_toolbar( new Ui::PainterPanelToolBar()),
	m_painterArea(new PainterGraphicsView(NULL))
{
	s_numInstances++;
	m_mainArea->setupUi( this );
	m_mainArea->paintLayout->addWidget(m_painterArea);
	m_historyListWidget = m_mainArea->historyListWidget;
	m_undoStack = new QUndoStack(this);
}


//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PainterPanel::~PainterPanel ()
{
	s_numInstances--;
}

///
/// Public Functions
///

void PainterPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
	/** Toolbar */
	QWidget* toolbarWidget = new QWidget( );
	m_toolbar->setupUi( toolbarWidget );
	mainToolBar->addWidget( toolbarWidget );


	/** listen to changes of selected items in the list */
	connect( m_historyListWidget,				SIGNAL(itemSelectionChanged()),								this, SLOT( historyListItemSelectionChanged()));

	/** listen to changes of GUI Elements */
	connect( m_mainArea->penSizeSpinBox,		SIGNAL( valueChanged(int)),									this, SLOT( onPenSizeValueChanged(int)));
	connect( m_mainArea->opacitySpinBox,		SIGNAL( valueChanged(int)),									this, SLOT( onPenOpacityValueChanged(int)));
	connect( m_mainArea->penColorSpinBox,		SIGNAL( valueChanged(int)),									this, SLOT( onPenColorValueChanged(int)));
	connect( m_mainArea->fillColorSpinBox,		SIGNAL( valueChanged(int)),									this, SLOT( onFillColorValueChanged(int)));
	connect( m_mainArea->fillGroupBox,			SIGNAL( toggled(bool)),										this, SLOT( onFillCheckToggled(bool)));
	connect( m_mainArea->deleteItemButton,		SIGNAL( clicked(bool)),										this, SLOT( deleteSelectedItems()));

	/** changes of draw mode */
	connect( m_toolbar->selectButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));
	connect( m_toolbar->paintButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));
	connect( m_toolbar->eraseButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));
	connect( m_toolbar->bezierButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));
	connect( m_toolbar->circleButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));
	connect( m_toolbar->squareButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));
	connect( m_toolbar->polygonButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));
	connect( m_toolbar->ellipseButton,			SIGNAL( clicked(bool)),										this, SLOT( buttonClicked()));

	/** changes of painter area */
	connect( m_painterArea,						SIGNAL( itemsMovedOrRotated()),								this, SLOT( onItemMovementOrRotation()));
	connect( m_painterArea,						SIGNAL( itemToCreateOnStack(QPointF)),						this, SLOT( onItemCreation(QPointF)));
	connect( m_painterArea,						SIGNAL( penChanged(QPen)),									this, SLOT( onPenChanged(QPen)));
	connect( m_painterArea,						SIGNAL( brushChanged(QBrush)),								this, SLOT( onBrushChanged(QBrush)));
	connect( m_painterArea,						SIGNAL( itemCanceled(QString)),								this, SLOT( removeItemFromHistory(QString)));
	connect( m_painterArea,						SIGNAL( drawModeChanged(PainterGraphicsView::DrawMode)),	this, SLOT( changeDrawMode(PainterGraphicsView::DrawMode)));
	connect( m_painterArea,						SIGNAL( selectionChanged(QList<QGraphicsItem*>)),			this, SLOT( onSceneSelectionChanged( QList<QGraphicsItem*>)));

	setPenColorSlider( m_penColor );
	setBrushColorSlider( m_brushColor );
	setOpacitySlider( QColor(m_opacity, m_opacity, m_opacity) );
}

void PainterPanel::selectedNodeChanged(Node *selectedNode)
{
	// Check if the selected node implements the PainterPanelNode
	PainterPanelNode* painterPanelNode = dynamic_cast<PainterPanelNode *>(selectedNode);

	// check interface implementation
	if( painterPanelNode && painterPanelNode->getNode())
	{
		// check if node is different from the current one
		if( painterPanelNode != m_painterPanelNode )
		{
			resetPanel();
			m_painterPanelNode = painterPanelNode;

			// attach to nodes painter panel scene
			m_scene = painterPanelNode->getScene();
			m_scene->setSelectionArea(m_scene->selectionArea(), Qt::ContainsItemShape);

			connect( m_scene, SIGNAL( itemEntrysIntoHistory(QList<QString>)),						this, SLOT( onEntrysIntoHistory(QList<QString>)));
			connect( m_scene, SIGNAL( useLUT(bool)), this, SLOT( setUseLUT(bool)));
			setUseLUT( m_scene->getUseLUT() );

			QApplication::processEvents();

			// setup painter area
			m_painterArea->setCurrentScene(m_scene);
			m_painterArea->viewport()->update();
			m_painterArea->adjustToBackground();
			m_mainArea->optionGroupBox->setEnabled(true);
			m_mainArea->itemGroupBox->setEnabled(true);		

			// force painter node to update to the current frame
			int frame = m_sceneModel->getCurrentFrame();
			//m_painterPanelNode->updateScene( frame );
			m_scene->updateGraphicsItemNames();

			m_painterPanelNode->getImage();
		}
	} 
	else 
	{
		resetPanel();
	}
}

void PainterPanel::onSceneModelSelectionChanged(bool selected)
{
	if (!selected)
		resetPanel();
}

void PainterPanel::onFrameChanged( int currentFrame )
{
	if( m_painterPanelNode && dynamic_cast<PainterPanelNode *>(m_painterPanelNode))
	{
		m_painterPanelNode->getImage();
	}
}

void PainterPanel::resetPanel()
{

	m_historyListWidget->clear();
	m_mainArea->optionGroupBox->setEnabled(false);
	m_mainArea->itemGroupBox->setEnabled(false);

	// disconnect current node signal
	if( dynamic_cast<PainterPanelNode *>(m_painterPanelNode)) {
		disconnect( m_scene, SIGNAL( itemEntrysIntoHistory(QList<QString>)),					this, SLOT( onEntrysIntoHistory(QList<QString>)));
		disconnect( m_scene, SIGNAL( useLUT(bool)), this, SLOT( setUseLUT(bool)));

		m_painterArea->setCurrentScene(NULL);
	}

	// delete handle to node
	m_painterPanelNode = NULL;

}

void PainterPanel::registerControl(NodeModel *nodeModel, SceneModel *sceneModel)
{
	m_nodeModel = nodeModel;
	m_sceneModel = sceneModel;

	// listen to changes of selected nodes
	connect( m_nodeModel,		SIGNAL(nodeSelected(Node *)),		this, SLOT( selectedNodeChanged(Node *)));
	connect( m_sceneModel,		SIGNAL(selectionChanged(bool)),		this, SLOT( onSceneModelSelectionChanged(bool)));
	connect( m_sceneModel,		SIGNAL(currentFrameSet(int)),		this, SLOT( onFrameChanged(int)));

	// listen to changes of the painter area and trigger update of viewport
	connect( m_painterArea,		SIGNAL(triggerRedraw()),			m_sceneModel, SLOT( redrawTriggered()));
	connect( this,				SIGNAL(triggerRedraw()),			m_sceneModel, SLOT( redrawTriggered()));

	// Get currently selected nodes and perform update
	foreach( QString name, m_sceneModel->getSelectedObjects())
	{
		Node *node = m_nodeModel->getNode(name);
		if( node != NULL )
			selectedNodeChanged(node);
	}
}

///
/// Private Slots
///

void PainterPanel::sceneChanged( QGraphicsScene *scene )
{
	if( !scene)
		return;

	m_historyListWidget->clear();

	foreach( QGraphicsItem* scene_item, scene->items())
	{
		BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(scene_item);
		if( bs_item )
		{
			QString name = bs_item->getName();
			if(name != "")
				m_historyListWidget->addItem( new QListWidgetItem(name));
		}
	}
}

void PainterPanel::deleteSelectedItems()
{
	if(m_scene && m_scene->getSelectedItems().length() >= 1)
		m_undoStack->push( new DeleteItemCommand(this, m_scene->getSelectedItems(), m_sceneModel->getCurrentFrame()));
}

void PainterPanel::historyListItemSelectionChanged()
{
	m_painterArea->blockSignals(true);
	QList<QListWidgetItem*> list = m_historyListWidget->selectedItems();
	m_scene->clearSelection();
	Q_FOREACH(QListWidgetItem* item, list)
    {
		QString name = item->text();
		BaseShapeItem* shapeItem = m_scene->getGraphicsItemByName(name);
		if( shapeItem )
			shapeItem->setSelected(true);
		else
			removeItemFromHistory(name);
    }
	m_painterArea->blockSignals(false);
}

void PainterPanel::changeDrawMode(PainterGraphicsView::DrawMode drawMode)
{
	uncheckAll();
	switch(drawMode) {

	case PainterGraphicsView::NONE:
		m_toolbar->selectButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::NONE);
		break;
	case PainterGraphicsView::PAINT:
		m_toolbar->paintButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::PAINT);
		break;
	case PainterGraphicsView::ERASE:
		m_toolbar->eraseButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::ERASE);
		break;
	case PainterGraphicsView::BEZIER_CURVE:
		m_toolbar->bezierButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::BEZIER_CURVE);
		break;
	case PainterGraphicsView::CIRCLE:
		m_toolbar->circleButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::CIRCLE);
		break;
	case PainterGraphicsView::RECT:
		m_toolbar->squareButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::RECT);
		break;
	case PainterGraphicsView::POLYGON:
		m_toolbar->polygonButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::POLYGON);
		break;
	case PainterGraphicsView::ELLIPSE:
		m_toolbar->ellipseButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::ELLIPSE);
		break;
	default:
		m_toolbar->selectButton->setChecked(true);
		m_painterArea->setDrawMode(PainterGraphicsView::NONE);
		break;
	} 
}

void PainterPanel::buttonClicked( )
{
	QPushButton* button = dynamic_cast<QPushButton*>(sender());
	if(button == m_toolbar->paintButton) {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::PAINT, "Paint Tool"));
	} else if(button == m_toolbar->eraseButton) {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::ERASE, "Erase Tool"));
	} else if(button == m_toolbar->bezierButton) {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::BEZIER_CURVE, "Bezier Tool"));
	} else if(button == m_toolbar->circleButton) {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::CIRCLE, "Circle Tool"));
	} else if(button == m_toolbar->squareButton) {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::RECT, "Rectangle Tool"));
	} else if(button == m_toolbar->polygonButton) {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::POLYGON, "Polygon Tool"));
	} else if(button == m_toolbar->ellipseButton) {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::ELLIPSE, "Ellipse Tool"));
	} else {
		m_undoStack->push(new ToolSelectCommand(this, m_drawMode, PainterGraphicsView::NONE, "Select Tool"));
	} 
}


void PainterPanel::onItemMovementOrRotation()
{
	this->blockSignals(true);
	m_scene->blockSignals(true);
	m_undoStack->blockSignals(true);
	m_painterArea->blockSignals(true);

	QPointF direction;
	QPointF oldPosition;
	float oldRotation;
	float newRotation;

	QList<QString> items = m_scene->getSelectedItems();

	if(!items.isEmpty())
	{
		QString item = items.at(0);
		BaseShapeItem* bs_item = m_scene->getGraphicsItemByName(item);

		if(bs_item) {
			oldPosition = bs_item->getSavedPosition();
			direction   = bs_item->getSavedDistance();
			oldRotation = bs_item->getSavedRotation();
			newRotation = bs_item->getRotation();

			if(oldRotation != newRotation) {
				m_undoStack->push( new RotateItemCommand(this, m_scene->getSelectedItems(), oldRotation, newRotation, m_sceneModel->getCurrentFrame()));
				bs_item->setSavedRotation(0.0);
			}

			if(direction != QPointF(0.0, 0.0)) {
				m_undoStack->push( new MoveItemCommand(this, m_scene->getSelectedItems(), direction, oldPosition, m_sceneModel->getCurrentFrame()));
				bs_item->setSavedPosition(QPointF(0.0, 0.0));
				bs_item->setSavedDistance(QPointF(0.0, 0.0));
			}
			
		}
	}
	
	

	this->blockSignals(false);
	m_scene->blockSignals(false);
	m_undoStack->blockSignals(false);
	m_painterArea->blockSignals(false);
}

void PainterPanel::keyPressEvent( QKeyEvent *event )
{
	this->blockSignals(true);
	m_scene->blockSignals(true);
	m_undoStack->blockSignals(true);
	m_painterArea->blockSignals(true);

	switch (event->key()) 
	{
		// D deletes the currently selected items
		case Qt::Key_D:
		case Qt::Key_Delete:
			deleteSelectedItems();
		break;

		if( event->modifiers() == Qt::ControlModifier )
		{
			case Qt::Key_Z:
				if(m_undoStack->canUndo()) {
					Log::info(QObject::tr("%1 undone.").arg(m_undoStack->undoText()), "Undo/Redo");
					m_undoStack->undo();
				}
			break;

			case Qt::Key_Y:
				if(m_undoStack->canRedo()) {
					Log::info(QObject::tr("%1 redone.").arg(m_undoStack->redoText()), "Undo/Redo");
					m_undoStack->redo();
				}
			break;
		}
	}

	this->blockSignals(false);
	m_scene->blockSignals(false);
	m_undoStack->blockSignals(false);
	m_painterArea->blockSignals(false);
}

ParameterGroup* PainterPanel::getItemParameterGroup()
{
	return m_painterPanelNode->getItemParameterGroup();
}

BaseShapeItem* PainterPanel::getGraphicsItemByName(QString name)
{
	return m_scene->getGraphicsItemByName(name);
}

void PainterPanel::uncheckAll()
{
	m_toolbar->selectButton->setChecked(false);
	m_toolbar->paintButton->setChecked(false);
	m_toolbar->eraseButton->setChecked(false);
	m_toolbar->bezierButton->setChecked(false);
	m_toolbar->circleButton->setChecked(false);
	m_toolbar->squareButton->setChecked(false);
	m_toolbar->polygonButton->setChecked(false);
	m_toolbar->ellipseButton->setChecked(false);
}

void PainterPanel::onSceneSelectionChanged(QList<QGraphicsItem*> selectedItems)
{
	m_historyListWidget->blockSignals(true);

	Q_FOREACH(QListWidgetItem* item, m_historyListWidget->selectedItems())
	{
		item->setSelected(false);
	}

    Q_FOREACH(QGraphicsItem* item, selectedItems)
    {
        BaseShapeItem* bs_item = dynamic_cast<BaseShapeItem*>(item);
		if(bs_item)
		{
			QList<QListWidgetItem*> list = m_historyListWidget->findItems(bs_item->getName(), Qt::MatchExactly);

			Q_FOREACH(QListWidgetItem* item, list)
			{
				item->setSelected(true);
			}
		}
    }
	m_historyListWidget->blockSignals(false);
}

void PainterPanel::onEntrysIntoHistory(QList<QString> nameList)
{
	foreach(QString item, nameList) {
		addItemToHistory(item);
	}
}

void PainterPanel::onItemCreation(QPointF pos)
{
	addItem(pos, (BaseShapeItem::ItemType) m_painterArea->getDrawMode(), "");
}

void PainterPanel::onPenChanged( QPen pen )
{
    m_pen = pen;
    m_penColor = pen.color();

	setPenColorSlider( pen.color());
	setOpacitySpinBox( pen.color().alpha() );
	
	m_mainArea->penSizeSlider->setValue( pen.width() );
	m_mainArea->penColorSlider->setValue( pen.color().red() );
}

void PainterPanel::onBrushChanged( QBrush brush )
{
    m_brush = brush;
    m_brushColor = brush.color();
	m_opacity = brush.color().alpha();

	setBrushColorSlider( brush.color());
	setOpacitySpinBox( brush.color().alpha() );

    bool filled = (brush.style() != Qt::NoBrush);
	m_mainArea->fillGroupBox->setChecked(filled);
    m_mainArea->fillColorSlider->setEnabled(filled);
    m_mainArea->fillColorSpinBox->setEnabled(filled);
}

void PainterPanel::changingPenColor( int color )
{
	QColor selectedColor = QColor(color, color, color);
    if (selectedColor.isValid() && selectedColor != m_penColor) {
        m_penColor = selectedColor;
		m_penColor.setAlpha(m_opacity);
        setPenColorSlider(m_penColor);
        m_pen.setColor(m_penColor);
        m_painterArea->changePen(m_pen);
    }
}

void PainterPanel::changingFillColor( int color )
{
	QColor selectedColor = QColor(color, color, color);
	if (selectedColor.isValid() && selectedColor != m_brushColor) {
		m_brushColor = selectedColor;
		m_brushColor.setAlpha(m_opacity);
		setBrushColorSlider(m_brushColor);
		m_brush.setColor(m_brushColor);
		m_painterArea->changeBrush(m_brush);
	}
}

void PainterPanel::changingPenSize( int size )
{
	m_pen.setWidth( size );
	m_mainArea->penSizeSlider->setValue( size );
	m_painterArea->changePen(m_pen);
}

void PainterPanel::addItemToHistory( QString name )
{
	if(name == "")
		return;

	QList<QListWidgetItem*> list = m_historyListWidget->findItems(name, Qt::MatchExactly);

	if(list.isEmpty()) {
		m_historyListWidget->addItem(name);
		m_scene->sceneHasChanged();
	}
	else {
		Log::error("Item [" + name + "] already exists in History!", "PainterPanel");
	}
	m_historyListWidget->sortItems(Qt::AscendingOrder);
}

void PainterPanel::removeItemFromHistory( QString name )
{
	if(name == "")
		return;

	QList<QListWidgetItem*> list = m_historyListWidget->findItems(name, Qt::MatchExactly);

	if(list.isEmpty()) {
		Log::error("Item [" + name + "] to delete, doesn't exist in History!", "PainterPanel");
	}
	else {
		Q_FOREACH(QListWidgetItem* item, list)
		{
			m_historyListWidget->removeItemWidget(item);
			delete item;
		}
		m_scene->sceneHasChanged();
	}
}

void PainterPanel::removeItem( QString name )
{
	// History List
	removeItemFromHistory(name);

	// Scene & View
	m_scene->deleteGraphicsItem(name);
	m_painterArea->setCurrentItem(NULL);
	m_scene->sceneHasChanged();
}

void PainterPanel::recreateItem( PainterPanelItemData data )
{
	BaseShapeItem* item = m_painterArea->recreateItem(data);
	addItemToHistory(item->getName());
	m_scene->sceneHasChanged();
}

QString PainterPanel::addItem( QPointF pos, BaseShapeItem::ItemType type, QString name )
{
	BaseShapeItem* item = m_painterArea->createItem(pos, type, name);
	if(item)
	{
		if(name == "")
			m_painterArea->setCurrentItem(item);
		addItemToHistory(item->getName());
		m_scene->sceneHasChanged();
		return item->getName();
	}
	return "";
}

void PainterPanel::changingFillStatus( bool value )
{
	m_mainArea->fillColorSlider->setEnabled(value);
	m_mainArea->fillColorSpinBox->setEnabled(value);
	m_mainArea->fillGroupBox->setChecked(value);

	if( value )
		m_brush.setStyle(Qt::SolidPattern);
	else
		m_brush.setStyle(Qt::NoBrush);

	m_painterArea->changeBrush(m_brush);
}

void PainterPanel::changingPenOpacity( int opacity )
{
	m_opacity = opacity;
	m_penColor.setAlpha( m_opacity );
	m_pen.setColor(m_penColor);
	m_brushColor.setAlpha( m_opacity );
	m_brush.setColor(m_brushColor);
	setOpacitySlider( QColor(opacity, opacity, opacity) );
	m_painterArea->changePen(m_pen);
	m_painterArea->changeBrush(m_brush);
}

void PainterPanel::onFillCheckToggled( bool checked )
{
	m_undoStack->push(new FillToogleCommand(this, checked, "Fill Status Change"));
}

void PainterPanel::onPenSizeValueChanged( int size )
{
	m_undoStack->push(new PenSizeCommand(this, m_pen.width(), size, "Pen Size Change"));
}

void PainterPanel::onPenColorValueChanged( int color )
{
	m_undoStack->push(new PenColorCommand(this, m_penColor.red(), color, "Pen Color Change"));
}

void PainterPanel::onPenOpacityValueChanged(int opacity )
{
	m_undoStack->push(new PenOpacityCommand(this, m_penColor.alpha(), opacity, "Pen Opacity Change"));
}

void PainterPanel::onFillColorValueChanged( int color )
{
	m_undoStack->push(new FillColorCommand(this, m_brushColor.red(), color, "Fill Color Change"));
}

void PainterPanel::setPenColorSlider( QColor color )
{
	m_mainArea->penColorSlider->setValue( color.red() );
	QColor sliderColor = (m_scene && m_scene->getUseLUT()) ? m_scene->lut(color.red()) : color;
	m_mainArea->penColorSlider->setStyleSheet( QString(" QSlider::handle:horizontal {background: rgb(%1, %2, %3); border: 1px solid #555; width: 18px; border-radius: 3px; margin: -2px 0;} QSlider::handle:horizontal:!enabled{background: #999; border: 1px solid #999; width: 18px; border-radius: 3px; margin: -2px 0;} ").arg(sliderColor.red()).arg(sliderColor.green()).arg(sliderColor.blue()));
}

void PainterPanel::setOpacitySlider( QColor color )
{
	m_mainArea->opacitySlider->setValue( color.red() );
	m_mainArea->opacitySlider->setStyleSheet( QString(" QSlider::handle:horizontal {background: rgb(%1, %2, %3); border: 1px solid #555; width: 18px; border-radius: 3px; margin: -2px 0;} QSlider::handle:horizontal:!enabled{background: #999; border: 1px solid #999; width: 18px; border-radius: 3px; margin: -2px 0;} ").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void PainterPanel::setBrushColorSlider( QColor color )
{
	m_mainArea->fillColorSlider->setValue( color.red() );
	QColor brushColor = (m_scene && m_scene->getUseLUT()) ? m_scene->lut(color.red()) : color;
    m_mainArea->fillColorSlider->setStyleSheet( QString(" QSlider::handle:horizontal {background: rgb(%1, %2, %3); border: 1px solid #555; width: 18px; border-radius: 3px; margin: -2px 0;} QSlider::handle:horizontal:!enabled{background: #999; border: 1px solid #999; width: 18px; border-radius: 3px; margin: -2px 0;} ").arg(brushColor.red()).arg(brushColor.green()).arg(brushColor.blue()));
}

void PainterPanel::setStandardPenColor( PainterGraphicsView::DrawMode drawMode )
{
    // define standard pen colors for draw modes
    switch( drawMode )
    {
        case PainterGraphicsView::PAINT:       m_penColor = Qt::blue; break;
        case PainterGraphicsView::LINE:        m_penColor = Qt::white; break;
        case PainterGraphicsView::RECT:        m_penColor = Qt::green; break;
        case PainterGraphicsView::CIRCLE:      m_penColor = Qt::red; break;
        case PainterGraphicsView::ELLIPSE:     m_penColor = Qt::magenta; break;    
        case PainterGraphicsView::POLYGON:    m_penColor = Qt::gray; break;
		case PainterGraphicsView::BEZIER_CURVE: m_penColor = Qt::cyan; break;
    }

	m_penColor = Qt::white;
	m_penColor.setAlpha(m_opacity);
    setPenColorSlider(m_penColor);
    m_pen.setColor(m_penColor);
    m_painterArea->changePen(m_pen);
}

void PainterPanel::setOpacitySpinBox( int value )
{
	m_mainArea->opacitySpinBox->setValue( value );
}

QString PainterPanel::getPanelTypeName()
{
	return "Painter Panel";
}

void PainterPanel::setUseLUT( bool val )
{
	setPenColorSlider(m_penColor);
	setBrushColorSlider(m_brushColor);
}

} // namespace PainterPanel