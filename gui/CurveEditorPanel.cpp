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
//! \file "CurveEditorPanel.cpp"
//! \brief Implementation file for CurveEditorPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       24.08.2010 (last updated)
//!

#include "CurveEditorPanel.h"
#include "KeyGraphicsItem.h"
#include "CurveEditorDataNode.h"
#include <qlabel.h>
#include <qsplitter.h>


namespace Frapper {

Q_DECLARE_METATYPE(AbstractParameter *)
Q_DECLARE_METATYPE(NumberParameter *)


///
/// Constructors and Destructors
///


//!
//! Constructor of the CurveEditorPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
CurveEditorPanel::CurveEditorPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_CurveEditor, parent, flags),
	m_node(0),
	m_curveEditorGraphicsView(new CurveEditorGraphicsView(this)),
	m_valueSpinBox(new QDoubleSpinBox(this)),
	m_normalizeSpinBox(new QDoubleSpinBox(this)),
	m_scaleSpinBox(new QDoubleSpinBox(this)),
	m_curveCheckBox(new QCheckBox(this)),
	m_typeComboBox(new QComboBox(this)),
	m_timelinePosition(0),
	m_keyType(Key::KT_Linear)
{
    setupUi(this);
   
    m_hboxLayout = new QHBoxLayout();
    m_hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));

    m_muteIcon.addFile(QString::fromUtf8(":/muteAllIcon"), QSize(), QIcon::Normal, QIcon::Off);

    m_dataTree = new QTreeWidget(this);
    m_dataTree->setObjectName(QString::fromUtf8("dataTreeWidget"));
	m_dataTree->setIconSize(QSize(13,13));
	m_dataTree->setColumnCount(2);
	m_dataTree->setColumnWidth(0, 168);
	m_dataTree->setColumnWidth(1, 25);
	QStringList columnNames;
	columnNames << "Data Tree" << " ";
	m_dataTree->setHeaderLabels(columnNames);
    m_dataTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataTree->setSelectionMode(QAbstractItemView::ExtendedSelection);

	m_curveCheckBox->setObjectName("curveCheckBox");

	QSplitter *splitter = new QSplitter;
	splitter->addWidget(m_dataTree);
    splitter->addWidget(m_curveEditorGraphicsView);

    m_hboxLayout->addWidget(splitter);

	ui_vboxLayout->addLayout(m_hboxLayout);

	m_valueSpinBox->setMinimum(-1.5);
	m_valueSpinBox->setMaximum(1.5);
	m_valueSpinBox->setDecimals(2);
	m_valueSpinBox->setSingleStep(0.01);

	m_scaleSpinBox->setValue(1.0);
	m_scaleSpinBox->setMinimum(-10.0);
	m_scaleSpinBox->setMaximum(10.0);
	m_scaleSpinBox->setDecimals(2);
	m_scaleSpinBox->setSingleStep(0.01);

	m_normalizeSpinBox->setValue(1.0);
	m_normalizeSpinBox->setMinimum(0.01);
	m_normalizeSpinBox->setMaximum(10.0);
	m_normalizeSpinBox->setDecimals(2);
	m_normalizeSpinBox->setSingleStep(0.01);

	static const QStringList literals = QStringList() << "Step" << "Linear" << "Bezier";
	for (int i = 0; i < literals.size(); ++i) {
		if (literals.at(i) == Parameter::EnumerationSeparator)
			m_typeComboBox->insertSeparator(i);
		else
			m_typeComboBox->insertItem(i, literals.at(i));
	}
	m_typeComboBox->setCurrentIndex(1);

    QObject::connect(m_dataTree, SIGNAL(itemSelectionChanged()), this, SLOT(showCurves()));
	QObject::connect(m_curveEditorGraphicsView, SIGNAL(drag()), this, SIGNAL(drag()));
	QObject::connect(m_curveEditorGraphicsView, SIGNAL(selectedKeyTypeChanged(int)), this, SLOT(selectedKeyTypeChanged(int)));

	connect(ui_muteAction, SIGNAL(triggered()), SLOT(muteSelectedParameterGroups()));
	connect(ui_muteAllAction, SIGNAL(triggered()), SLOT(muteAllParameterGroups()));
	connect(ui_unmuteAllAction, SIGNAL(triggered()), SLOT(unmuteAllParameterGroups()));

	connect(ui_homeAction, SIGNAL(triggered()), m_curveEditorGraphicsView, SLOT(homeView()));
    connect(ui_frameAllAction, SIGNAL(triggered()), m_curveEditorGraphicsView, SLOT(frameAll()));
    connect(ui_frameSelectedAction, SIGNAL(triggered()), m_curveEditorGraphicsView, SLOT(frameSelected()));
	connect(ui_scrollbarsAction, SIGNAL(toggled(bool)), m_curveEditorGraphicsView, SLOT(toggleScrollbars(bool)));

	connect(ui_setKeysAction, SIGNAL(triggered()), this, SLOT(changeKeyValues()));
	connect(ui_scaleKeysAction, SIGNAL(triggered()), this, SLOT(scaleKeyValues()));
	connect(ui_normKeysAction, SIGNAL(triggered()), this, SLOT(normalizeKeyValues()));
	connect(m_curveCheckBox, SIGNAL(toggled(bool)), m_curveEditorGraphicsView, SLOT(toggleShowEnabledOnly(bool)));

    connect(m_typeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(keyTypeChanged(int)), Qt::QueuedConnection);
}


//!
//! Destructor of the CurveEditorPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
CurveEditorPanel::~CurveEditorPanel ()
{
}


///
/// Public Functions
///


//!
//! Fills the given tool bars with actions for the curve editor panel.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void CurveEditorPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
	QLabel *onlyUnmutedLabel = new QLabel(tr("Show unmuted only "), this);

	mainToolBar->addAction(ui_muteAction);
	mainToolBar->addSeparator();
	mainToolBar->addAction(ui_muteAllAction);
	mainToolBar->addAction(ui_unmuteAllAction);
	mainToolBar->addSeparator();
	mainToolBar->addAction(ui_setKeysAction);
	mainToolBar->widgetForAction(ui_setKeysAction)->setFixedWidth(50);
	mainToolBar->addWidget(m_valueSpinBox);
	mainToolBar->addSeparator();
	mainToolBar->addAction(ui_scaleKeysAction);
	mainToolBar->widgetForAction(ui_scaleKeysAction)->setFixedWidth(60);
	mainToolBar->addWidget(m_scaleSpinBox);
	mainToolBar->addSeparator();
	mainToolBar->addAction(ui_normKeysAction);
	mainToolBar->widgetForAction(ui_normKeysAction)->setFixedWidth(60);
	mainToolBar->addWidget(m_normalizeSpinBox);
	mainToolBar->addSeparator();
	mainToolBar->addWidget(m_typeComboBox);
	
	panelToolBar->addWidget(onlyUnmutedLabel);
	panelToolBar->addWidget(m_curveCheckBox);
	panelToolBar->addSeparator();
	panelToolBar->addAction(ui_homeAction);
    panelToolBar->addAction(ui_frameSelectedAction);
    panelToolBar->addAction(ui_frameAllAction);
	panelToolBar->addSeparator();
	panelToolBar->addAction(ui_scrollbarsAction);
	panelToolBar->addSeparator();
}


///
/// Public Slots
///

//!
//! Slot that is called when the selection in the scene model has changed.
//!
//! \param objectsSelected Flag that states whether objects in the scene are selected.
//!
void CurveEditorPanel::updateTree ( bool objectsSelected )
{
    if (objectsSelected)
        return;

    // clear and reset the parameter panel
    buildTree(0);
}

//!
//! Initializes the Selection Tree
//!
//! \param node The node whose animated parameters to display in the curve editor.
//!
void CurveEditorPanel::buildTree ( Node *node )
{
	m_node = node;
	m_dataTree->clear();

	CurveEditorDataNode *curveEditorDataNode = dynamic_cast<CurveEditorDataNode *>(node);
	
	if (curveEditorDataNode) {
		ParameterGroup* database = curveEditorDataNode->getCurveEditorGoup();
		fillTree(database, m_dataTree->invisibleRootItem());
	}
}


//!
//! Displays the curves of the given node in the curve editor.
//!
//!
void CurveEditorPanel::showCurves ()
{
    m_curveEditorGraphicsView->showCurves(&m_dataTree->selectedItems());
    m_curveEditorGraphicsView->buildScene();
}


//!
//! Sets the index of the current frame in the scene's time.
//!
//! \param index The new index of the current frame in the scene's time.
//!
void CurveEditorPanel::setCurrentFrame ( const int index )
{
	m_timelinePosition = index;
    m_curveEditorGraphicsView->setCurrentFrame(index);
}


//!
//! Sets the index of the in frame in the scene's time.
//!
//! \param index The new index of the in frame in the scene's time.
//!
void CurveEditorPanel::setInFrame ( const int index )
{
	m_curveEditorGraphicsView->setInFrame(index);
}

//!
//! Sets the index of the out frame in the scene's time.
//!
//! \param index The new index of the out frame in the scene's time.
//!
void CurveEditorPanel::setOutFrame ( const int index )
{
	m_curveEditorGraphicsView->setOutFrame(index);
}


//!
//! Sets the value of the selected Keys.
//!
void CurveEditorPanel::changeKeyValues ()
{
	QList<QGraphicsItem *> &selectedItems = m_curveEditorGraphicsView->scene()->selectedItems();
	const double value = m_valueSpinBox->value();
	
	if (selectedItems.empty())
		selectedItems = m_curveEditorGraphicsView->scene()->items();
	
	const float sceneScale = m_curveEditorGraphicsView->m_scaleFactor.height();

	foreach (QGraphicsItem *item, selectedItems) {
		KeyGraphicsItem *keyItem = dynamic_cast<KeyGraphicsItem *>(item);
		if (keyItem) {
			Key *key = item->data(0).value<Key *>();
			key->keyValue.setValue(static_cast<float>(value));
			const float newPosY = m_curveEditorGraphicsView->scene()->height() / 2.0f - value * sceneScale;
			keyItem->setPos(keyItem->x(),newPosY);
		}
	}
	emit drag();
}


//!
//! Scales the value of the selected Keys.
//!
void CurveEditorPanel::scaleKeyValues ()
{
	QList<QGraphicsItem *> &selectedItems = m_curveEditorGraphicsView->scene()->selectedItems();
	const double scale = m_scaleSpinBox->value();
	
	if (selectedItems.empty())
		selectedItems = m_curveEditorGraphicsView->scene()->items();
	
	const float sceneScale = m_curveEditorGraphicsView->m_scaleFactor.height();

	foreach (QGraphicsItem *item, selectedItems) {
		KeyGraphicsItem *keyItem = dynamic_cast<KeyGraphicsItem *>(item);
		if (keyItem) {
			Key *key = item->data(0).value<Key *>();
			const float keyValue = key->keyValue.toFloat();
			key->keyValue.setValue<float>(keyValue * scale);
			const float newPosY = m_curveEditorGraphicsView->scene()->height() / 2.0f - (keyValue * scale) * sceneScale;
			keyItem->setPos(keyItem->x(),newPosY);
		}
	}
	emit drag();
}


//!
//! Scales the values of the shown curve to a maximum of one.
//!
void CurveEditorPanel::normalizeKeyValues ()
{
#ifdef __GNUC__
float max = -HUGE_VAL;
#else
float max = -HUGE;
#endif
	
	const double scale = m_normalizeSpinBox->value();
	const float sceneScale = m_curveEditorGraphicsView->m_scaleFactor.height();
	QList<QGraphicsItem *> &items = m_curveEditorGraphicsView->scene()->selectedItems();

	if (items.empty())
		items = m_curveEditorGraphicsView->scene()->items();

	// find max value
	foreach (QGraphicsItem *item, items) {
		const KeyGraphicsItem *keyItem = dynamic_cast<KeyGraphicsItem *>(item);
		if (keyItem) {
			const Key *key = item->data(0).value<Key *>();
			const float keyValue = abs(key->keyValue.toFloat());
			if (keyValue > max)
				max = keyValue;
		}
	}

	// scale values and move objects to new position
	foreach (QGraphicsItem *item, items) {
		KeyGraphicsItem *keyItem = dynamic_cast<KeyGraphicsItem *>(item);
		if (keyItem) {
			Key *key = item->data(0).value<Key *>();
			const float keyValue = key->keyValue.toFloat();
			key->keyValue = static_cast<float>(keyValue / max * scale);
			keyItem = static_cast<KeyGraphicsItem *>(item);
			const float newPosY = m_curveEditorGraphicsView->scene()->height() / 2.0f - (keyValue / max*scale) * sceneScale;
			keyItem->setPos(keyItem->x(),newPosY);
		}
	}
	emit drag();
}


//!
//! Mutes/unmutes the in treeView selected Parameters
//!
void CurveEditorPanel::muteSelectedParameterGroups ()
{
	AbstractParameter *currentParameter = 0;
	const QList<QTreeWidgetItem *> &itemList = m_dataTree->selectedItems();

	foreach (QTreeWidgetItem *item, itemList) {
		currentParameter = item->data(1,0).value<AbstractParameter *>();
		if (currentParameter->isEnabled()) {
			if (item != itemList.last())
				currentParameter->setEnabled(false, false);
			else
				currentParameter->setEnabled(false, true);
			item->setIcon(1, m_muteIcon);
			muteItems(item);
		}
		else {
			if (item != itemList.last())
				currentParameter->setEnabled(true, false);
			else
				currentParameter->setEnabled(true, true);
			item->setIcon(1, QIcon());
			unmuteItems(item);
		}
	}
	emit drag();
	m_dataTree->repaint();
}


//!
//! Mutes all treeView Parameters
//!
void CurveEditorPanel::muteAllParameterGroups ()
{
	AbstractParameter *currentParameter = 0;
	QTreeWidgetItem  *item, *parent;
	const QList<QTreeWidgetItem *> &itemList = m_dataTree->selectedItems();
	
	if (!itemList.isEmpty() && (parent = itemList.at(0)->parent())) {
		const unsigned int count = parent->childCount();
		for (unsigned int i=0; i<count; ++i) {
			item = parent->child(i);
			currentParameter = item->data(1, 0).value<AbstractParameter *>();
			if (i!=count-1)
				currentParameter->setEnabled(false, false);
			else
				currentParameter->setEnabled(false, true);
			muteItems(item);
		}
	}
	else {
		const int count = m_dataTree->topLevelItemCount();
		for (int i=0; i<count; ++i) {
			item = m_dataTree->topLevelItem(i);
			currentParameter = item->data(1, 0).value<AbstractParameter *>();
			if (i!=count-1)
				currentParameter->setEnabled(false, false);
			else
				currentParameter->setEnabled(false, true);
			muteItems(item);
		}
	}
	emit drag();
	m_dataTree->repaint();
}


//!
//! Unmutes all treeView Parameters
//!
void CurveEditorPanel::unmuteAllParameterGroups ()
{
	AbstractParameter *currentParameter = 0;
	QTreeWidgetItem  *item, *parent;
	const QList<QTreeWidgetItem *> &itemList = m_dataTree->selectedItems();
	
	if (!itemList.isEmpty() && (parent = itemList.at(0)->parent())) {
		const int count = parent->childCount();
		for (int i=0; i<count; ++i) {
			item = parent->child(i);
			currentParameter = item->data(1, 0).value<AbstractParameter *>();
			if (i!=count-1)
				currentParameter->setEnabled(true, false);
			else
				currentParameter->setEnabled(true, true);
			unmuteItems(item);
		}
	}
	else {
		const int count = m_dataTree->topLevelItemCount();
		for (int i=0; i<count; ++i) {
			item = m_dataTree->topLevelItem(i);
			currentParameter = item->data(1, 0).value<AbstractParameter *>();
			if (i!=count-1)
				currentParameter->setEnabled(true, false);
			else
				currentParameter->setEnabled(true, true);
			unmuteItems(item);
		}
	}
	emit drag();
	m_dataTree->repaint();
}



///
/// Privata Functions
///


//!
//! Fills the tree view with node data groups
//!
//! \param rootData the root parameter group containing the data
//! \param rootItem the QTreeWidgetItem to add the data
//!

//!
//! Changes color and icon of selected treeView Items and chids
//!
void CurveEditorPanel::muteItems(QTreeWidgetItem  *item)
{
	item->setIcon(1, m_muteIcon);
	item->setTextColor(0, QColor::fromRgb(200, 0, 0));
	if (item->childCount() > 0)
		for (int i = 0; i < item->childCount(); ++i)
			muteItems(item->child(i));
}


//!
//! Changes color and icon of selected treeView Items and chids
//!
void CurveEditorPanel::unmuteItems(QTreeWidgetItem  *item)
{
	item->setIcon(1, QIcon());
	item->setTextColor(0, QColor::fromRgb(0, 0, 0));
	if (item->childCount() > 0)
		for (int i = 0; i < item->childCount(); ++i)
			unmuteItems(item->child(i));
}


//!
//! Deletes the selected keys of the selected number parameter.
//!
void CurveEditorPanel::deleteSelectedKeys()
{
	QList<QGraphicsItem *> &selectedItems = m_curveEditorGraphicsView->scene()->selectedItems();
	
	if (!selectedItems.empty()) {
		foreach (QGraphicsItem *item, selectedItems) {
			if (dynamic_cast<TangentGraphicsItem *>(item))
				continue;
			KeyGraphicsItem *keyItem = dynamic_cast<KeyGraphicsItem *>(item);
			if (keyItem) {
				Key *key = item->data(0).value<Key *>();
				NumberParameter *numberParameter = static_cast<NumberParameter *>(key->parent);
				numberParameter->removeKey(*key);
			}
		}
		showCurves();
	}
}


//!
//! Inserts a new key in the selected number parameter
//! at the current timeline position.
//!
void CurveEditorPanel::insertKey()
{
	const QList<QTreeWidgetItem *> &itemList = m_dataTree->selectedItems();

	foreach (QTreeWidgetItem *item, itemList) {
		AbstractParameter *parameter = item->data(1,0).value<AbstractParameter *>();
		if (parameter->isGroup()) {
			const AbstractParameter::List& parameterList = dynamic_cast<ParameterGroup *>(parameter)->getParameterList();
			foreach (AbstractParameter *abstractParameter, parameterList) {
				NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(abstractParameter);
				if (numberParameter && numberParameter->getSize() == 1)
					numberParameter->addKey(m_timelinePosition, m_keyType);
			}
		}
		else {
			NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(parameter);
			if (numberParameter && numberParameter->getSize() == 1)
				numberParameter->addKey(m_timelinePosition, m_keyType);
		}
	}
	showCurves();
}


//!
//! Updates color and icon of selected treeView Items and chids
//!
void CurveEditorPanel::updateItems()
{
	const AbstractParameter *currentParameter = static_cast<AbstractParameter *>(sender());
	QTreeWidgetItem *treeItem = m_dataTree->findItems(currentParameter->getName(), Qt::MatchRecursive).at(0);

	if (currentParameter->isEnabled()) {
		treeItem->setIcon(1, QIcon());
		treeItem->setTextColor(0, QColor::fromRgb(0, 0, 0));
	}
	else {
		treeItem->setIcon(1, m_muteIcon);
		treeItem->setTextColor(0, QColor::fromRgb(200, 0, 0));
	}
}


//!
//! Changes the key type.
//!
void CurveEditorPanel::keyTypeChanged (int index)
{
	QComboBox *comboBox = qobject_cast<QComboBox *>(sender());

	if (comboBox) {
		m_keyType = static_cast<Key::KeyType>(index);

		QList<QGraphicsItem *> &selectedItems = m_curveEditorGraphicsView->scene()->selectedItems();

		if (!selectedItems.empty()) {
			foreach (QGraphicsItem *item, selectedItems) {
				if (dynamic_cast<TangentGraphicsItem *>(item))
					continue;
				KeyGraphicsItem *keyItem = dynamic_cast<KeyGraphicsItem *>(item);
				if (keyItem) {
					Key *key = item->data(0).value<Key *>();
					key->type = m_keyType;
				}
			}
			emit drag();
			showCurves();
		}
	}
}


//!
//! Changes only the combo box literal.
//!
void CurveEditorPanel::selectedKeyTypeChanged( int index )
{
	m_typeComboBox->blockSignals(true);
	m_typeComboBox->setCurrentIndex(index);
	m_typeComboBox->blockSignals(false);
}


void CurveEditorPanel::fillTree( ParameterGroup *rootData, QTreeWidgetItem *rootItem )
{
	const AbstractParameter::List& data = rootData->getParameterList();
	NumberParameter *numparameter = 0;
	ParameterGroup *parameterGroup = 0;
	QVariant variant;
	QTreeWidgetItem *newItem = 0;

	foreach (AbstractParameter *parameter, data) {
		numparameter = dynamic_cast<NumberParameter *>(parameter);
		parameterGroup = dynamic_cast<ParameterGroup *>(parameter);
		if ( (numparameter && numparameter->getSize() == 1) || 
			(parameterGroup && ( parameterGroup->contains(Parameter::T_Float) || parameterGroup->contains(Parameter::T_Group) )) ) {
				newItem = new QTreeWidgetItem(rootItem, QStringList(parameter->getName()));
				connect(parameter, SIGNAL(enabledChanged()), this, SLOT(updateItems()));
				variant.setValue<AbstractParameter *>(parameter);
				newItem->setData(1, 0, variant);
				rootItem->addChild(newItem);
				if (parameterGroup) {
					fillTree(parameterGroup, newItem);
					newItem->setTextColor(0, QColor::fromRgb(128, 128, 128));
				}
				if (!parameter->isEnabled()) {
					newItem->setIcon(1, m_muteIcon);
					newItem->setTextColor(0, QColor::fromRgb(200, 0, 0));
				}
		}
	}
}


///
/// Protected Events
///


//!
//! Handles key press events for the widget.
//!
//! \param event The description of the key event.
//!
void CurveEditorPanel::keyPressEvent ( QKeyEvent *event )
{
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    switch (event->key()) {
        case Qt::Key_A:
            ui_frameAllAction->trigger();
            break;
        case Qt::Key_F:
            ui_frameSelectedAction->trigger();
            break;
        case Qt::Key_H:
        case Qt::Key_Home:
            ui_homeAction->trigger();
            break;
        case Qt::Key_T:
            ui_scrollbarsAction->toggle();
            break;
		case Qt::Key_D:
			deleteSelectedKeys();
			break;
		case Qt::Key_S:
			insertKey();
			break;
        default:
            ViewPanel::keyPressEvent(event);
    }
}

} // end namespace Frapper