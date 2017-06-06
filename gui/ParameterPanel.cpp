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
//! \file "ParameterPanel.cpp"
//! \brief Implementation file for ParameterPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakafemie.de>
//! \version    1.0
//! \date       25.02.2013 (last updated)
//!

#include "ParameterPanel.h"
#include "ParameterTabPage.h"
#include "DoubleSlider.h"
#include "NodeFactory.h"
#include "WidgetFactory.h"
#include "WidgetPlugin.h"
#include "ParameterPlugin.h"
#include "Log.h"
#include <QMetaMethod>
#include <QTabWidget>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QMenu>
#include <QClipboard>


namespace Frapper {


///
/// Constructors and Destructors
///


//!
//! Constructor of the ParameterPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
ParameterPanel::ParameterPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_ParameterEditor, parent, flags),
    m_editedWidget(0),
    m_nodeTypeLabel(new QLabel(this)),
    m_nodeTypeLabelAction(0),
    m_nodeNameEdit(new QLineEdit(this)),
    m_nodeNameEditAction(0),
	m_parameterAdvancedLabel(new QLabel(this)),
	m_parameterAdvancedLabelAction(0),
	m_parameterAdvancedCheckBox(new QCheckBox(this)),
	m_parameterAdvancedCheckBoxAction(0),
	m_searchEdit(new QLineEdit(this)),
    m_searchTimer(0),
	m_nodeTapWidget(new QTabWidget(this))
{
    // initialize the node type and name widgets
    m_nodeTypeLabel->setIndent(20);
    m_nodeNameEdit->setStyleSheet(
        "margin-right: 2px;"
        "height: 20px;"
    );

	m_parameterAdvancedLabel->setIndent(20);
	m_parameterAdvancedLabel->setText("Show adv. Tab:");

    // initialize the search edit widget
    m_searchEdit->setToolTip(tr("Parameter Search"));
    m_searchEdit->setStatusTip(tr("Enter a name or part of a name of parameters to display"));
    m_searchEdit->setEnabled(true);

    // set object names for QMetaObject::connectSlotsByName to work (called in setupUi function)
    m_nodeNameEdit->setObjectName("m_nodeNameEdit");
    m_searchEdit->setObjectName("m_searchEdit");
	m_parameterAdvancedLabel->setObjectName("m_parameterAdvancedLabel");
	
    setupUi(this);

	ui_formLayout->addRow(m_nodeTapWidget);
	connect(m_nodeTapWidget, SIGNAL(currentChanged( int )), SLOT(nodeTapChanged( int )));
}



//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ParameterPanel::~ParameterPanel ()
{
}


///
/// Public Slots
///


//!
//! Displays the parameters and values for the given nodes.
//!
//! \param nodes The list whose nodes to display in the parameter view.
//!
void ParameterPanel::showNodes ( const QList <Node *> &nodes )
{
	static const int maxSize = 5;
	QList<Node *> newNodes;

	if (!nodes.empty()) {
		m_nodeTapWidget->hide();

		// determine nodes to insert
		foreach(Node *node, nodes) {
			const bool parametersChanged = node->parametersChanged();
			if (!m_nodes.contains(node->getName()) || parametersChanged) {
				if (parametersChanged)
					deleteNode(node->getName());
				newNodes.append(node);
			}
		}
	
		int deleteLast = 0;
		const int freeSlots = maxSize - m_nodes.size();
		if (newNodes.size() > freeSlots)
			deleteLast = newNodes.size() - freeSlots;
		if (deleteLast > maxSize)
			deleteLast = maxSize;

		// delete last nodes if necessary
		for(int i=0; i<deleteLast; i++)
			deleteNode(m_nodes.keys().last());

		// determine max number of node taps
		(newNodes.size() < maxSize) ? deleteLast = newNodes.size() : deleteLast = maxSize;

		for (int i=0; i<deleteLast; ++i) {
			const QString &name = newNodes[i]->getName();
			m_nodes.insert(name, newNodes[i]);
			ParameterTabPage *nodePage = new ParameterTabPage(m_nodeTapWidget, 0);
			nodePage->setProperty("nodeName", name);
			const int tapIndex = m_nodeTapWidget->addTab(nodePage, name);
			m_nodeTapIndices.append(name);
			QFormLayout *formLayout = new QFormLayout(nodePage);
			showParameters(newNodes[i], formLayout);
		}

		updateLabels(nodes.last());

		const int tapIndex = m_nodeTapIndices.indexOf(nodes.last()->getName());
		m_nodeTapWidget->setCurrentIndex(tapIndex);

		m_nodeTapWidget->show();
	}

	const bool tapsEmpty = m_nodes.empty();
	// show or hide the node type and name widgets through their actions
	if (m_nodeTypeLabelAction) {
		m_nodeTypeLabelAction->setVisible(!tapsEmpty);
		m_nodeTypeLabelAction->setEnabled(!tapsEmpty);
	}
	if (m_nodeNameEditAction) {
		m_nodeNameEditAction->setVisible(!tapsEmpty);
		m_nodeNameEditAction->setEnabled(!tapsEmpty);
	}

    // skip the rest of the function if no node has been set
	if (tapsEmpty) {
        m_nodeTypeLabel->setText("");
        m_nodeNameEdit->setText("");
	}
}


//!
//! Displays the parameters and values for the given node.
//!
//! \param node The node whose parameters to display in the parameter view.
//!
void ParameterPanel::showParameters ( Node *node, QFormLayout *formLayout )
{
    // get a list of parameters that match the search text
	const Parameter::List &filteredParameters = node->getParameterRoot()->filterParameters(m_searchEdit->text().toCaseFolded(), true);

	QFormLayout *advancedFormLayout = 0;

	const bool advanced = m_parameterAdvancedCheckBox->isChecked();
	if ( advanced ) {
		QTabWidget *tabWidget = new QTabWidget(this);
		formLayout->addRow(tabWidget);
		ParameterTabPage *normalPage = new ParameterTabPage(tabWidget, 0);
		ParameterTabPage *advancedPage = new ParameterTabPage(tabWidget, 0);
		tabWidget->addTab(normalPage, "Standard");
		tabWidget->addTab(advancedPage, "Advanced");
		formLayout = new QFormLayout(normalPage);
		advancedFormLayout = new QFormLayout(advancedPage);
	}

    if (filteredParameters.size() > 0) {
        // build a temporary parameter group for the search results
        ParameterGroup *searchParameterGroup = new ParameterGroup("Search Results");
		searchParameterGroup->setNode(node);
		foreach (const AbstractParameter *parameter, filteredParameters)
            searchParameterGroup->addParameter(const_cast<AbstractParameter *>(parameter));

        // add rows for the parameters found
        addRows(searchParameterGroup, formLayout);
		if (advanced)
			addAdvancedRows(searchParameterGroup, advancedFormLayout);

        // destroy the temporary parameter group
        searchParameterGroup->clear();
        delete searchParameterGroup;
        searchParameterGroup = 0;
    } 
	else if (m_searchEdit->text().isEmpty()){
        // add rows for all parameters
        addRows(node->getParameterRoot(), formLayout);
		if (advanced)
			addAdvancedRows(node->getParameterRoot(), advancedFormLayout);
	}
}


//!
//! Slot that is called when the selection in the scene model has changed.
//!
//! \param objectsSelected Flag that states whether objects in the scene are selected.
//!
void ParameterPanel::updateParameters ( const bool objectsSelected )
{
    if (objectsSelected)
        return;

    // clear and reset the parameter panel
	//clear();
    showNodes(QList<Node *>());

    // update the search edit and reset action without triggering signals
    //m_searchEdit->blockSignals(true);
    //m_searchEdit->setText("");
    //ui_resetSearchAction->setEnabled(false);
    //m_searchEdit->blockSignals(false);
}


//!
//! Deletes the node with the given name from the internal node map
//! and the node tap and all of his childs.
//!
//! \param name The name of the node to be deleted.
//!
void ParameterPanel::deleteNode ( const QString &name )
{
	for (int i=0; i<m_nodeTapIndices.size(); i++) {
		const QWidget *widget = m_nodeTapWidget->widget(i);
		const QString &nodeName = widget->property("nodeName").toString();
			if (nodeName == name) {
				m_nodeTapWidget->removeTab(i);
				deletefromItemMap(widget);
				delete widget;
				break;
			}
	}

	QHash<QString, Node *>::iterator nodeIter = m_nodes.find(name);
	if (nodeIter != m_nodes.end()) {
		m_nodes.remove(name);
		m_nodeTapIndices.removeOne(name);
	}
}


///
/// Public Functions
///


//!
//! Fills the given tool bars in a panel frame with actions for the panel.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void ParameterPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
    // add widgets to main tool bar
    m_nodeTypeLabelAction = mainToolBar->addWidget(m_nodeTypeLabel);
    m_nodeNameEditAction = mainToolBar->addWidget(m_nodeNameEdit);
	m_parameterAdvancedLabelAction = mainToolBar->addWidget(m_parameterAdvancedLabel);
	m_parameterAdvancedCheckBoxAction = mainToolBar->addWidget(m_parameterAdvancedCheckBox); 

    // make the node type and name edit widgets invisible
    m_nodeTypeLabelAction->setVisible(false);
    m_nodeTypeLabelAction->setEnabled(false);
    m_nodeNameEditAction->setVisible(false);
    m_nodeNameEditAction->setEnabled(false);
	m_parameterAdvancedLabelAction->setVisible(true);
	m_parameterAdvancedCheckBoxAction->setVisible(true);
	m_parameterAdvancedLabelAction->setEnabled(true);
	m_parameterAdvancedCheckBoxAction->setEnabled(true);

    // create the search icon label
    QLabel *searchIconLabel = new QLabel(this);
    searchIconLabel->setFixedWidth(20);
    searchIconLabel->setStyleSheet(
        "background-image: url(:/searchIcon);"
        "background-repeat: no-repeat;"
        "background-origin: content;"
        "background-position: left center;"
        "margin-right: 2px;"
    );
    searchIconLabel->setToolTip(tr("Parameter Search"));

    // add the parameter search controls to the panel tool bar
    panelToolBar->addWidget(searchIconLabel);
    panelToolBar->addWidget(m_searchEdit);
    panelToolBar->addAction(ui_resetSearchAction);
    panelToolBar->addSeparator();

	// setup signal-slot connections
	connect(m_parameterAdvancedCheckBox, SIGNAL(toggled( bool )), SLOT(advancedCheckBoxToggled( bool )));
}

///
/// Protected Events
///


//!
//! Event handler that reacts to timer events.
//!
//! \param event The description of the timer event.
//!
void ParameterPanel::timerEvent ( QTimerEvent *event )
{
    if (event->timerId() == m_searchTimer) {
        killTimer(m_searchTimer);
        m_searchTimer = 0;

        filterParameters();
    }
}

//!
//! Event filter, allows to disable value changes caused by the mouse wheel
//!
//! \param object the object with the installed event filter.
//! \param event the description of the event.
//!
 bool ParameterPanel::eventFilter(QObject *object,  QEvent *event)
 {
	 if(event->type() == QEvent::Wheel)		 
          return true;
	 return false;
 }


///
/// Private Slots
///


//!
//! Causes the panel to reinitiate the widgets.
//!
//!
//! \param checked The state of the advanced check box.
//!
void ParameterPanel::advancedCheckBoxToggled( const bool checked )
{
	QList<Node *> nodes(m_nodes.values());
	clear();
	showNodes(nodes);
}

//!
//! Requests a name change for the currently edited node and displays the
//! successfully changed name in the node name edit widget.
//!
//! Is called when editing the node's name has finished.
//!
void ParameterPanel::on_m_nodeNameEdit_editingFinished ()
{
	const int tapIndex = m_nodeTapWidget->currentIndex();
	if (tapIndex > -1) {
		const QString newName = m_nodeNameEdit->text();
		const QString oldName = m_nodeTapIndices.at(tapIndex);
	
		if ((!newName.isEmpty() && !oldName.isEmpty()) &&
			newName != oldName) {
			Node *node = m_nodes.find(oldName).value();

			// make sure a node is currently edited
			if (!node) {
				Log::warning("No node is currently edited. This function should not have been called.", "ParameterPanel::on_m_nodeNameEdit_editingFinished");
				return;
			}

			// notify connected objects that the currently edited node should be renamed
			emit objectNameChangeRequested(oldName, newName);
			deleteNode(oldName);

			m_nodes.insert(newName, node);
			ParameterTabPage *nodePage = new ParameterTabPage(m_nodeTapWidget, 0);
			nodePage->setProperty("nodeName", newName);
			const int tapIndex = m_nodeTapWidget->addTab(nodePage, newName);
			m_nodeTapIndices.append(newName);
			QFormLayout *formLayout = new QFormLayout(nodePage);
			showParameters(node, formLayout);
		} 
		else
				Log::warning("Please enter a valid name.", "ParameterPanel::on_m_nodeNameEdit_editingFinished");
	}
}


//!
//! Starts the timer that launches the parameter filtering search after a
//! specific delay.
//!
//! Is called when the text in the search edit widget has changed.
//!
//! \param text The text that has been entered in the search edit widget.
//!
void ParameterPanel::on_m_searchEdit_textChanged ( const QString &text )
{
    // stop a previously started search timer
    if (m_searchTimer != 0)
        killTimer(m_searchTimer);

    m_searchTimer = startTimer(1000);

    ui_resetSearchAction->setEnabled(!text.isEmpty());
}


//!
//! Cancels a running parameter search and resets the parameter search
//! settings for the currently edited node.
//!
//! Is called when the reset search action has been triggered.
//!
//! \param checked The state of the action (unused).
//!
void ParameterPanel::on_ui_resetSearchAction_triggered ( const bool checked /*= false */ )
{
    // stop the search timer if it is currently running
    if (m_searchTimer != 0) {
        killTimer(m_searchTimer);
        m_searchTimer = 0;
    }

    // reset the parameter search widgets
    m_searchEdit->setText("");

    // reset the parameter search for the node
	foreach (Node *node, m_nodes)
		node->setSearchText("");

    // display all parameters for the node
	QList<Node *> nodes(m_nodes.values());
	clear();
    showNodes(nodes);
}


//!
//! Fills the tab page of the given index with widgets for editing the
//! parameters that the tab page represents.
//!
//! Is called when the index of the current tab in a tab widget has
//! changed.
//!
//! \param index The index of the tab page to fill with editing widgets.
//!
void ParameterPanel::loadTabPage ( const int index )
{
    QTabWidget *tabWidget = qobject_cast<QTabWidget *>(sender());
    if (!tabWidget)
        return;

    ParameterTabPage *parameterTabPage = dynamic_cast<ParameterTabPage *>(tabWidget->widget(index));
    if (parameterTabPage) {
        // check if the tab page has already been loaded
        if (!parameterTabPage->isLoaded()) {
            // create a new form layout for the tab page and fill it with editing widgets for the page's parameters
            QFormLayout *formLayout = new QFormLayout(parameterTabPage);
            parameterTabPage->setLayout(formLayout);
			if (parameterTabPage->isAdvanced())
				addAdvancedRows(parameterTabPage->getParameterGroup(), formLayout);
			else
				addRows(parameterTabPage->getParameterGroup(), formLayout);
            // mark the parameter tab page as being loaded
            parameterTabPage->setLoaded();
        }
    } else
        Log::warning(QString("The tab page with index %1 can not be loaded. The widget of the given index is not a parameter tab page.").arg(index), "ParameterPanel::loadTabPage");
}


//!
//! Opens a context menu at the given position with actions for the label.
//!
//! Is called when a context menu is requested for a label widget.
//!
//! \param position The position at which to open the context menu.
//!
void ParameterPanel::labelContextMenuRequested ( const QPoint &position )
{
    QLabel *label = qobject_cast<QLabel *>(sender());
    if (!label)
        return;

    QMenu contextMenu ("Text Info", this);
    QAction *copyAction = contextMenu.addAction(QIcon(":/copyIcon"), tr("&Copy"));
    if (contextMenu.exec(label->mapToGlobal(position)) == copyAction)
        QApplication::clipboard()->setText(label->text());
}


///
/// Private Slots for updating parameter values when widgets change
///


//!
//! Applies an edited boolean value to the parameter represented by the
//! checkbox widget calling this slot.
//!
//! Is called when a checkbox representing a parameter has been toggled.
//!
//! \param checked The state of the check box.
//!
void ParameterPanel::checkBoxToggled ( const bool checked )
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    if (!checkBox)
        return;

    const QString &parameterName = checkBox->property("parameterName").toString();
    const QString &nodeName = checkBox->property("nodeName").toString();
    QVariant parameterValue = QVariant::fromValue(checked);
	
	Parameter *parameter = m_nodes.value(nodeName)->getParameter(parameterName);
	if (parameter) {
		parameter->executeCommand();
	}

	requestParameterChange(parameterName, nodeName, parameterValue);
}


//!
//! Applies an edited numeric value to the parameter represented by the
//! spinbox widget calling this slot.
//!
//! Is called when editing the value of a spin box representing a parameter
//! has finished.
//!
void ParameterPanel::spinBoxEditingFinished ()
{
    QSpinBox *spinBox = qobject_cast<QSpinBox *>(sender());
    if (!spinBox)
        return;

    const QString &parameterName = spinBox->property("parameterName").toString();
    const QString &nodeName = spinBox->property("nodeName").toString();
    Parameter::Type parameterType = (Parameter::Type) spinBox->property("parameterType").toInt();
    QVariant parameterValue;
    if (parameterType == Parameter::T_Int)
        parameterValue = QVariant::fromValue(spinBox->value());
    else if (parameterType == Parameter::T_UnsignedInt)
        parameterValue = QVariant::fromValue((unsigned int) spinBox->value());
    else {
        Log::warning(QString("Parameter \"%1\" has an unsupported parameter type (%2) for spin boxes.").arg(parameterName).arg(parameterType), "ParameterPanel::spinBoxEditingFinished");
        return;
    }

    requestParameterChange(parameterName, nodeName, parameterValue);
}


//!
//! Applies an edited numeric value to the parameter represented by the
//! slider widget calling this slot.
//!
//! Is called when the value of a slider representing a parameter has
//! changed.
//!
void ParameterPanel::sliderValueChanged ()
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    if (!slider)
        return;

    const QString &parameterName = slider->property("parameterName").toString();
    const QString &nodeName = slider->property("nodeName").toString();
    Parameter::Type parameterType = (Parameter::Type) slider->property("parameterType").toInt();
    QVariant parameterValue;
    if (parameterType == Parameter::T_Int)
        parameterValue = QVariant::fromValue(slider->value());
    else if (parameterType == Parameter::T_UnsignedInt)
        parameterValue = QVariant::fromValue((unsigned int) slider->value());
    else {
        Log::warning(QString("Parameter \"%1\" has an unsupported parameter type (%2) for sliders.").arg(parameterName).arg(parameterType), "ParameterPanel::sliderValueChanged");
        return;
    }

    requestParameterChange(parameterName, nodeName, parameterValue);
}


//!
//! Applies an edited floating point numeric value to the parameter
//! represented by the spinbox widget calling this slot.
//!
//! Is called when editing the value of a double spin box representing a
//! parameter has finished.
//!
void ParameterPanel::doubleSpinBoxEditingFinished ()
{
    QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox *>(sender());
    if (!doubleSpinBox)
        return;

    const QString &parameterName = doubleSpinBox->property("parameterName").toString();
    const QString &nodeName = doubleSpinBox->property("nodeName").toString();
    QVariant parameterValue = QVariant::fromValue(doubleSpinBox->value());

    requestParameterChange(parameterName, nodeName, parameterValue);
}


//!
//! Applies an edited floating point numeric value to the parameter
//! represented by the slider widget calling this slot.
//!
//! Is called when the value of a double slider representing a parameter
//! has changed.
//!
void ParameterPanel::doubleSliderValueChanged ()
{
    DoubleSlider *slider = qobject_cast<DoubleSlider *>(sender());
    if (!slider)
        return;

    const QString &parameterName = slider->property("parameterName").toString();
    const QString &nodeName = slider->property("nodeName").toString();
    QVariant parameterValue = QVariant::fromValue(slider->doubleValue());

    requestParameterChange(parameterName, nodeName, parameterValue);
}


//!
//! Applies an edited string value to the parameter represented by the line
//! edit widget calling this slot.
//!
//! Is called when editing the text of a line edit widget representing a
//! parameter has finished.
//!
void ParameterPanel::lineEditEditingFinished ()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    if (!lineEdit)
        return;

    const QString &parameterName = lineEdit->property("parameterName").toString();
    const QString &nodeName = lineEdit->property("nodeName").toString();
    QVariant parameterValue = QVariant::fromValue(lineEdit->text());

    requestParameterChange(parameterName, nodeName, parameterValue);
}


//!
//! Displays a dialog for selecting a file for the edited parameter.
//!
//! Is called when the browse button for a filename parameter has been
//! clicked.
//!
void ParameterPanel::browseButtonClicked ()
{
    QPushButton *browseButton = qobject_cast<QPushButton *>(sender());
    if (!browseButton)
        return;

    const QString &parameterName = browseButton->property("parameterName").toString();
	const QString &nodeName = browseButton->property("nodeName").toString();
	FilenameParameter *filenameParameter = dynamic_cast<FilenameParameter *>(m_nodes.value(nodeName)->getParameter(parameterName));
    if (!filenameParameter) {
        Log::warning(QString("Filename parameter \"%1\" could not be obtained from node \"%2\".").arg(parameterName).arg(nodeName), "ParameterPanel::browseButtonClicked");
        return;
    }

	QString filename = filenameParameter->getValue().toString();
	QFileInfo fi( filename );

	// derive path from filename, use working directory if filename is empty
	QString path;
	if( filename.isEmpty() || !fi.exists())
		path = SceneModel::getWorkingDirectory();
	else
		if( fi.isDir())
			path = fi.absoluteFilePath();
		else
			path = fi.absolutePath();

	// display the file selection dialog
	FilenameParameter::Type filenameType = filenameParameter->getType();
	QString filters = filenameParameter->getFilters();
	if (filenameType == FilenameParameter::FT_Save)
		filename =  QFileDialog::getSaveFileName(0, QString(tr("Select %1")).arg(parameterName), path, filters);
	else if (filenameType == FilenameParameter::FT_Open)
		filename = QFileDialog::getOpenFileName(0, QString(tr("Select %1")).arg(parameterName), path, filters);
	else if (filenameType == FilenameParameter::FT_Dir)
		filename = QFileDialog::getExistingDirectory(0, QString(tr("Select %1")).arg(parameterName), path);

	// fix path separators
	filename = QDir::fromNativeSeparators(filename);
	
    if (!filename.isEmpty()) {

		// make filename relative, if possible
		filename = FilenameParameter::relativeFilePath(filename);

        if (m_widgetMap.contains(nodeName+"@"+parameterName)) {
            QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(m_widgetMap.value(nodeName+"@"+parameterName));
            if (lineEdit)
                lineEdit->setText(filename);
        }
		emit parameterChangeRequested(nodeName, parameterName, QVariant::fromValue(filename));
    }
}


//!
//! Reloads the file that the edited parameter represents.
//!
//! Is called when the reload button for a filename parameter has been
//! clicked.
//!
void ParameterPanel::reloadButtonClicked ()
{
    QPushButton *reloadButton = qobject_cast<QPushButton *>(sender());
    if (!reloadButton)
        return;

    const QString &parameterName = reloadButton->property("parameterName").toString();
    const QString &nodeName = reloadButton->property("nodeName").toString();
	Parameter *parameter = m_nodes.value(nodeName)->getParameter(parameterName);
    if (parameter) {
        parameter->executeCommand();
        parameter->propagateDirty();
        parameter->setDirty(false);
    }
}


//!
//! Displays a dialog for selecting a color for the edited parameter.
//!
//! Is called when the color button representing a color parameter has been
//! clicked.
//!
void ParameterPanel::selectColor ()
{
    QPushButton *colorButton = qobject_cast<QPushButton *>(sender());
    if (!colorButton)
        return;

    const QString &parameterName = colorButton->property("parameterName").toString();
    const QString &nodeName = colorButton->property("nodeName").toString();

    // display the color selection dialog with the current color selected
	QColor previousColor = m_nodes.value(nodeName)->getColorValue(parameterName);
    QColor selectedColor = QColorDialog::getColor(previousColor, NULL, "Color Picker", QColorDialog::ShowAlphaChannel);
    if (selectedColor.isValid() && selectedColor != previousColor) {
        requestParameterChange(parameterName, nodeName, QVariant::fromValue<QColor>(selectedColor));
    }
}


//!
//! Applies a changed combo box item selection to the edited parameter.
//!
//! Is called when the index of the currently selected item of a combo box
//! representing an enumeration parameter has changed.
//!
//! \param index The index of the item currently selected in the combo box.
//!
void ParameterPanel::comboBoxIndexChanged ( const int index )
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (!comboBox)
        return;

    const QString &parameterName = comboBox->property("parameterName").toString();
    const QString &nodeName = comboBox->property("nodeName").toString();
    QVariant parameterValue = QVariant::fromValue(index);

    requestParameterChange(parameterName, nodeName, parameterValue);
}


//!
//! Requests the execution of the command represented by the clicked
//! button.
//!
//! Is called when the button representing a command parameter is clicked.
//!
void ParameterPanel::commandButtonClicked ()
{
    QPushButton *commandButton = qobject_cast<QPushButton *>(sender());
    if (!commandButton)
        return;

    const QString &parameterName = commandButton->property("parameterName").toString();
    const QString &nodeName = commandButton->property("nodeName").toString();
	Parameter *commandParameter = m_nodes.value(nodeName)->getParameter(parameterName);
    if (commandParameter)
        commandParameter->executeCommand();
    else
        Log::warning(QString("Command parameter \"%1\" could not be obtained from node \"%2\".").arg(parameterName).arg(nodeName), "ParameterPanel::commandButtonClicked");
}


//!
//! Applies a changed combo box item selection to the edited parameter type.
//!
//!
//! \param index The index of the parameter type currently selected in the combo box.
//!
void ParameterPanel::parameterPinTypeChanged ( const int index )
{
	QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    
	if (comboBox) {
		Parameter::PinType pinType = static_cast<Parameter::PinType>(index-1); 
		const QString &parameterName = comboBox->property("parameterName").toString();
		const QString &nodeName = comboBox->property("nodeName").toString();
		emit changeParameterPinTypeRequested(nodeName, parameterName, pinType);
	}
}


//!
//! Applies a changed combo box item selection fur use as processing function of the patameter.
//!
//!
//! \param index The index of the parameters processing function currently selected in the combo box.
//!
void ParameterPanel::parameterProcessingFunctionChanged ( const int index )
{
	QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    
	if (comboBox) {
		QString processingFunction = ""; 
		if (comboBox->currentText() != "None")
			processingFunction = QString("1%1").arg(comboBox->currentText());
		const QString &parameterName = comboBox->property("parameterName").toString();
		const QString &nodeName = comboBox->property("nodeName").toString();
		m_nodes.value(nodeName)->setProcessingFunction(parameterName, processingFunction.toLatin1());
	}
}


//!
//! Applies a changed combo box item selection fur use as change function of the patameter.
//!
//!
//! \param index The index of the parameters change function currently selected in the combo box.
//!
void ParameterPanel::parameterChangeFunctionChanged ( const int index )
{
	QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    
	if (comboBox) {
		QString changeFunction = ""; 
		if (comboBox->currentText() != "None")
			changeFunction = QString("1%1").arg(comboBox->currentText());
		const QString &parameterName = comboBox->property("parameterName").toString();
		const QString &nodeName = comboBox->property("nodeName").toString();
		m_nodes.value(nodeName)->setChangeFunction(parameterName, changeFunction.toLatin1());
	}
}


//!
//! Toggels the parameters self evaluating flag
//!
//! Is called when a checkbox representing a parameter has been toggled.
//!
//! \param checked The state of the check box.
//!
void ParameterPanel::selfEvaluatingToggled ( const bool checked )
{
	 QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());

	 if (checkBox) { 
		const QString &parameterName = checkBox->property("parameterName").toString();
		const QString &nodeName = checkBox->property("nodeName").toString();
		emit changeParameterSelfEvaluationRequested(nodeName, parameterName, checked);
	 }
}


///
/// Private Slots for updating widgets when parameter values change
///


//!
//! Updates the widget showing a boolean parameter's value.
//!
//! Is called when the value of a boolean parameter has changed.
//!
void ParameterPanel::updateBooleanWidget ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;

	const QString &parameterName = parameter->getName() + "@" + parameter->getNode()->getName();
	bool enabled = parameter->isEnabled();

    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the first widget in the list of widgets (which should be a check box)
        QCheckBox *checkBox = dynamic_cast<QCheckBox *>(widgets[0]);
        if (checkBox) {
            bool parameterValue = parameter->getValue().toBool();
            checkBox->setChecked(parameterValue);
			checkBox->setEnabled(enabled);
        }
    }
}


//!
//! Updates the widgets showing an integer number parameter's value.
//!
//! Is called when the value of an integer number parameter has changed.
//!
void ParameterPanel::updateIntegerWidgets ()
{
    NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(sender());
    if (!numberParameter)
        return;

    Parameter::Size size = numberParameter->getSize();
    if (size == 1)
        updateIntegerWidgets(-1, numberParameter);
    else
        for (Parameter::Size i = 0; i < size; ++i)
            updateIntegerWidgets((int) i, numberParameter);
}


//!
//! Updates the widgets showing a integer integer number parameter's value.
//!
//!
//! \param index The index of the value that has changed in the list of integer point values.
//!
void ParameterPanel::updateIntegerWidgets ( int index )
{
	NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(sender());

	if (numberParameter)
		updateIntegerWidgets(index, numberParameter);
}


//!
//! Updates the widgets showing the component with the given index of an
//! integer number parameter.
//!
//! Is called when one of the values of an integer number parameter that
//! contains a list of values has changed.
//!
//! \param index The index of the value that has changed in the list of integer values.
//!
void ParameterPanel::updateIntegerWidgets ( const int index, NumberParameter *numberParameter )
{
    // get name and value from parameter
    QVariant parameterValue = numberParameter->getValue();
	bool enabled = numberParameter->isEnabled();

    // get name and value from parameter
	QString parameterName = numberParameter->getName();
	// adjust parameter name according to given value index
    if (index > -1)
        parameterName = QString("%1[%2]").arg(parameterName).arg(index);

	parameterName = parameterName +"@"+ numberParameter->getNode()->getName();

    // check if a widget for the parameter exists
	const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the unsigned integer value from the parameter value variant
        int value;
        if (index == -1)
            value = parameterValue.toInt();
        else
            value = parameterValue.toList().value(index).toInt();

        // iterate over the widgets for displaying the unsigned integer number parameter
        for (int i = 0; i < widgets.size(); ++i)
            if (widgets[i] != m_editedWidget) {
                // check if the widget is a spin box
                QSpinBox *spinBox = dynamic_cast<QSpinBox *>(widgets[i]);
                if (spinBox) {
                    // update the spin box for the parameter
                    spinBox->blockSignals(true);
                    spinBox->setValue(value);
					spinBox->setEnabled(enabled);
                    spinBox->blockSignals(false);
                } else {
                    // check if the widget is a slider
                    QSlider *slider = dynamic_cast<QSlider *>(widgets[i]);
                    if (slider) {
                        // update the slider for the parameter
                        slider->blockSignals(true);
                        slider->setValue(value);
						slider->setEnabled(enabled);
                        slider->blockSignals(false);
                    }
                }
            }
    }
}


//!
//! Updates the widgets showing an unsigned integer number parameter's
//! value.
//!
//! Is called when the value of an unsigned integer number parameter has
//! changed.
//!
void ParameterPanel::updateUnsignedIntegerWidgets ()
{
    NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(sender());
    if (!numberParameter)
        return;

    Parameter::Size size = numberParameter->getSize();
    if (size == 1)
        updateUnsignedIntegerWidgets(-1, numberParameter);
    else
        for (Parameter::Size i = 0; i < size; ++i)
            updateUnsignedIntegerWidgets((int) i, numberParameter);
}


//!
//! Updates the widgets showing a unsigned integer integer number parameter's value.
//!
//!
//! \param index The index of the value that has changed in the list of unsigned integer values.
//!
void ParameterPanel::updateUnsignedIntegerWidgets ( int index )
{
	NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(sender());

	if (numberParameter)
		updateUnsignedIntegerWidgets(index, numberParameter);
}


//!
//! Updates the widgets showing the component with the given index of an
//! unsigned integer number parameter.
//!
//! Is called when one of the values of an unsigned integer number
//! parameter that contains a list of values has changed.
//!
//! \param index The index of the value that has changed in the list of unsigned integer values.
//!
void ParameterPanel::updateUnsignedIntegerWidgets ( const int index, NumberParameter *numberParameter )
{
    // get name and value from parameter
    QVariant parameterValue = numberParameter->getValue();
	bool enabled = numberParameter->isEnabled();

	// get name and value from parameter
	QString parameterName = numberParameter->getName();
	// adjust parameter name according to given value index
	if (index > -1)
		parameterName = QString("%1[%2]").arg(parameterName).arg(index);

	parameterName = parameterName +"@"+ numberParameter->getNode()->getName();

	// check if a widget for the parameter exists
    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the unsigned integer value from the parameter value variant
        unsigned int value;
        if (index == -1)
            value = parameterValue.toUInt();
        else
            value = parameterValue.toList().value(index).toUInt();

        // iterate over the widgets for displaying the unsigned integer number parameter
        for (int i = 0; i < widgets.size(); ++i)
            if (widgets[i] != m_editedWidget) {
                // check if the widget is a spin box
                QSpinBox *spinBox = dynamic_cast<QSpinBox *>(widgets[i]);
                if (spinBox) {
                    // update the spin box for the parameter
                    spinBox->blockSignals(true);
                    spinBox->setValue(value);
					spinBox->setEnabled(enabled);
                    spinBox->blockSignals(false);
                } else {
                    // check if the widget is a slider
                    QSlider *slider = dynamic_cast<QSlider *>(widgets[i]);
                    if (slider) {
                        // update the slider for the parameter
                        slider->blockSignals(true);
                        slider->setValue(value);
						slider->setEnabled(enabled);
                        slider->blockSignals(false);
                    }
                }
            }
    }
}


//!
//! Updates the widgets showing a floating point number parameter's value.
//!
//! Is called when the value of a floating point number parameter has
//! changed.
//!
void ParameterPanel::updateFloatingPointWidgets ()
{
    NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(sender());

	if (numberParameter) {
		Parameter::Size size = numberParameter->getSize();
		if (size == 1)
			updateFloatingPointWidgets(-1, numberParameter);
		else
			for (Parameter::Size i = 0; i < size; ++i)
				updateFloatingPointWidgets((int) i, numberParameter);
	}
}


//!
//! Updates the widgets showing a floating point number parameter's value.
//!
//!
//! \param index The index of the value that has changed in the list of floating values.
//!
void ParameterPanel::updateFloatingPointWidgets ( int index )
{
	NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(sender());

	if (numberParameter)
		updateFloatingPointWidgets(index, numberParameter);
}


//!
//! Updates the widgets showing the component with the given index of a
//! floating point number parameter.
//!
//! Is called when one of the values of a floating point number
//! parameter that contains a list of values has changed.
//!
//! \param index The index of the value that has changed in the list of floating point values.
//! \param numberParameter The index of the value that has changed in the list of floating point values.
//!
void ParameterPanel::updateFloatingPointWidgets ( const int index, NumberParameter *numberParameter )
{
    // get name and value from parameter
	QString parameterName = numberParameter->getName();
	// adjust parameter name according to given value index
    if (index > -1)
        parameterName = QString("%1[%2]").arg(parameterName).arg(index);

	parameterName = parameterName +"@"+ numberParameter->getNode()->getName();
    QVariant parameterValue = numberParameter->getValue();
	bool enabled = numberParameter->isEnabled();

    // check if a widget for the parameter exists
    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the double value from the parameter value variant
        double value;
        if (index == -1)
            value = parameterValue.toDouble();
        else
            if (parameterValue.canConvert<Ogre::Vector3>())
                value = parameterValue.value<Ogre::Vector3>()[index];
            else
                value = parameterValue.toList().value(index).toDouble();

        // iterate over the widgets for displaying the unsigned integer number parameter
        for (int i = 0; i < widgets.size(); ++i)
            if (widgets[i] != m_editedWidget) {
                // check if the widget is a double spin box
                QDoubleSpinBox *doubleSpinBox = dynamic_cast<QDoubleSpinBox *>(widgets[i]);
                if (doubleSpinBox) {
                    // update the double spin box for the parameter
                    doubleSpinBox->blockSignals(true);
                    doubleSpinBox->setValue(value);
					doubleSpinBox->setEnabled(enabled);
                    doubleSpinBox->blockSignals(false);
                } else {
                    // check if the widget is a double slider
                    DoubleSlider *doubleSlider = dynamic_cast<DoubleSlider *>(widgets[i]);
                    if (doubleSlider) {
                        // update the double slider for the parameter
                        doubleSlider->blockSignals(true);
                        doubleSlider->setDoubleValue(value);
						doubleSlider->setEnabled(enabled);
                        doubleSlider->blockSignals(false);
                    }
                }
            }
    }
}


//!
//! Updates the widget showing a string parameter's value.
//!
//! Is called when the value of a string parameter has changed.
//!
void ParameterPanel::updateStringWidget ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;
	
	const QString &parameterName = parameter->getName() +"@"+ parameter->getNode()->getName();
	const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the first widget in the list of widgets (which should be a line edit)
		bool enabled = parameter->isEnabled();
        QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(widgets[0]);
        if (lineEdit) {
            QString parameterValue = parameter->getValue().toString();
            lineEdit->setText(parameterValue);
			lineEdit->setEnabled(enabled);
        }
    }
}


//!
//! Updates the widgets showing a filename parameter's value.
//!
//! Is called when the value of a filename parameter has changed.
//!
void ParameterPanel::updateFilenameWidgets ()
{
    FilenameParameter *parameter = dynamic_cast<FilenameParameter *>(sender());
    if (!parameter)
        return;

    const QString &parameterName = parameter->getName() +"@"+ parameter->getNode()->getName();
    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
		QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(widgets[0]);
		if (lineEdit) {
			QString parameterValue = parameter->getValue().toString();
			parameterValue = FilenameParameter::relativeFilePath(parameterValue);
			lineEdit->setText(parameterValue);
		}
		assert( widgets.size() > 1 );
		QLabel *fileNotFoundWarning = dynamic_cast<QLabel *>(widgets[1]);
		if( fileNotFoundWarning ) {
			fileNotFoundWarning->setVisible( parameter->FileNotFoundError() );
		}
    }
}


//!
//! Updates the widget showing a color parameter's value.
//!
//! Is called when the value of a color parameter has changed.
//!
void ParameterPanel::updateColorWidget ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;

    const QString &parameterName = parameter->getName() +"@"+ parameter->getNode()->getName();
    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the first widget in the list of widgets (which should be a push button)
        QPushButton *colorButton = dynamic_cast<QPushButton *>(widgets[0]);
        if (colorButton) {
            QColor parameterValue = parameter->getValue().value<QColor>();
            colorButton->setStyleSheet(QString("background: rgb(%1, %2, %3);").arg(parameterValue.red()).arg(parameterValue.green()).arg(parameterValue.blue()));
        }
    }
}


//!
//! Updates the widget showing an enumeration parameter's value.
//!
//! Is called when the value of an enumeration parameter has changed.
//!
void ParameterPanel::updateEnumerationWidget ()
{
    EnumerationParameter *parameter = dynamic_cast<EnumerationParameter *>(sender());
    if (!parameter)
        return;

    const QString &parameterName = parameter->getName() +"@"+ parameter->getNode()->getName();
    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the first widget in the list of widgets (which should be a combo box)
        QComboBox *comboBox = dynamic_cast<QComboBox *>(widgets[0]);
        if (comboBox) {
            comboBox->blockSignals(true);
            comboBox->setCurrentIndex(parameter->getCurrentIndex());
            comboBox->blockSignals(false);
        }
    }
}


//!
//! Updates the label showing a text info parameter's value.
//!
//! Is called when the value of a text info parameter has changed.
//!
void ParameterPanel::updateTextInfoWidget ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;

    const QString &parameterName = parameter->getName() +"@"+ parameter->getNode()->getName();
    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the first widget in the list of widgets (which should be a label)
        QLabel *label = dynamic_cast<QLabel *>(widgets[0]);
        if (label) {
            QString parameterValue = parameter->getValue().toString();
            label->setText(parameterValue);
        }
    }
}


//!
//! Updates the label showing info about the image parameter's image.
//!
//! Is called when the value of an image parameter has changed.
//!
void ParameterPanel::updateImageWidget ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;

    const QString &parameterName = parameter->getName() +"@"+ parameter->getNode()->getName();
    const QWidgetList &widgets = m_widgetMap.values(parameterName);
	if (widgets.size() > 0) {
        // get the first widget in the list of widgets (which should be a label)
        QLabel *label = dynamic_cast<QLabel *>(widgets[0]);
        if (label) {
            if (parameter->getPinType() == Parameter::PT_Input)
                if (parameter->isConnected()) {
                    // show details of the connected image in a label widget
					Ogre::TexturePtr inputTexture = parameter->getNode()->getTextureValue(parameterName);
                    if (!inputTexture.isNull()) {
                        unsigned int width = (unsigned int) inputTexture->getWidth();
                        unsigned int height = (unsigned int) inputTexture->getHeight();
                        QString pixelFormatName = Ogre::PixelUtil::getFormatName(inputTexture->getFormat()).c_str();
                        label->setText(QString("Image dimensions: %1x%2\nPixel format: %3").arg(width).arg(height).arg(pixelFormatName));
                    }
                } else
                    label->setText("No input image connected.\n");
        }
    }
}


//!
//! Updates the node name edit text, if the actual nede tap changes.
//!
void ParameterPanel::nodeTapChanged( const int index )
{
	const QString &nodeName = m_nodeTapWidget->tabText(index);
	QHash<QString, Node *>::Iterator nodeIter = m_nodes.find(nodeName);

	if (nodeIter != m_nodes.end())
		updateLabels(nodeIter.value());
}


///
/// Private Functions
///


//!
//! Updates all label with the Informations providet by the given node.
//!
//! \parameter node The node which informations should be shown.
//!
void ParameterPanel::updateLabels( Node *node )
{
	// display the type of the selected node in the node type label and the name of the selected node in the node name edit
    m_nodeTypeLabel->setText(node->getTypeName() + ":");
    m_nodeNameEdit->setText(node->getName());
    QString typeIconName;
    if (node->isTypeUnknown()) {
        typeIconName = ":/warningIcon";
        m_nodeTypeLabel->setToolTip(tr("There is no node type plugin available for the type of this node.\nEditing parameters of this node has been deactivated."));
    } else {
        typeIconName = NodeFactory::getCategoryIconName(node->getTypeName());
        m_nodeTypeLabel->setToolTip("");
    }
    m_nodeTypeLabel->setStyleSheet(QString(
        "background-image: url(%1);"
        "background-repeat: no-repeat;"
        "background-origin: content;"
        "background-position: left center;"
        "margin-left: 4px;"
        "margin-right: 2px;"
        ).arg(typeIconName)
    );
}


//!
//! Activates the custom context menu for the given label widget.
//!
//! \param label The label widget to activate a custom context menu for.
//!
void ParameterPanel::activateCustomContextMenu ( QLabel *label )
{
    if (!label) {
        Log::error("The given label widget is invalid.", "ParameterPanel::activateCustomContextMenu");
        return;
    }

    label->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(label, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(labelContextMenuRequested(const QPoint &)));
}


//!
//! Updates the given widget's palette so that it is painted with a Window
//! color background instead of a Base color background.
//!
//! \param widget The widget to gray out.
//!
void ParameterPanel::grayOut ( QWidget *widget )
{
    if (!widget) {
        Log::error("The given widget is invalid.", "ParameterPanel::grayOut");
        return;
    }

    QPalette widgetPalette = widget->palette();
    widgetPalette.setColor(QPalette::Base, widgetPalette.color(QPalette::Window));
    widget->setPalette(widgetPalette);
}


//!
//! Removes all widgets from the layout.
//!
void ParameterPanel::clear ()
{
    // delete all widgets from the scroll area
    QList<QWidget *> widgets = ui_scrollAreaWidgetContents->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets)
		widget->deleteLater();

    foreach (QWidget *widget, m_widgetMap)
		if (widget)
			widget->deleteLater();

    m_widgetMap.clear();
	m_nodes.clear();
	m_nodeTapIndices.clear();

	// recreate node tap widget
	m_nodeTapWidget = new QTabWidget(this);
	ui_formLayout->addRow(m_nodeTapWidget);
	connect(m_nodeTapWidget, SIGNAL(currentChanged( int )), SLOT(nodeTapChanged( int )));
}


//!
//! Adds widgets for editing the parameters of the given parameter group to
//! the form layout.
//!
//! \param parameterGroup The parameter group to create editing widgets for.
//! \param formLayout The form layout to add editing widgets to.
//!
void ParameterPanel::addRows ( ParameterGroup *parameterGroup, QFormLayout *formLayout )
{
	const AbstractParameter::List& parameterList = parameterGroup->getParameterList();
	QTabWidget *tabWidget = 0;

	for (int i = 0; i < parameterList.size(); ++i) {
		AbstractParameter *abstractParameter = parameterList.at(i);
		if (abstractParameter->isGroup()) {
			ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(abstractParameter);
			if( !parameterGroup->isEmpty() && parameterGroup->isVisible() ) {
				if ( !tabWidget ) {
					// create a tab widget to contain a new form layout for the group
					tabWidget = new QTabWidget(this);
					connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(loadTabPage(int)));
					// add the tab widget to the main form layout
					formLayout->addRow(tabWidget);
				}
				ParameterTabPage *page = new ParameterTabPage(tabWidget, parameterGroup);
				tabWidget->addTab(page, parameterGroup->getName());
			}
		} 
		else {
			Parameter *parameter = dynamic_cast<Parameter *>(abstractParameter);
			if (parameter->isVisible() && (parameter->getPinType() != Parameter::PT_Output || parameter->getType() != Parameter::T_Image))
				addRows(parameter, formLayout);
			tabWidget = 0;
		}
	}
}


//!
//! Adds widgets for editing the given parameter to the form layout.
//!
//! \param parameter The parameter to create editing widgets for.
//! \param formLayout The form layout to add editing widgets to.
//!
void ParameterPanel::addRows ( Parameter *parameter, QFormLayout *formLayout )
{
	// get parameter properties
	const QString &parameterName = parameter->getName();
	Node *node = parameter->getNode();
	Parameter::Type parameterType = parameter->getType();
	bool enabled = parameter->isEnabled();
	bool readOnly = parameter->isReadOnly();
	bool isConnected = parameter->isConnected();

	// get the parameter's value while also triggering propagation
	QVariant parameterValue = node->getValue(parameterName, true);

	// create widgets for the row for editing the parameter's value depending on its type
	if (parameter->getSize() == 1) {
		// depending on the type of parameter either a simple widget or a layout with several widgets is created
		QWidget *widget = 0;
		QLayout *layout = 0;
		switch (parameterType) {
		case Parameter::T_Bool:
			//label->setText(""); // don't show a label for boolean parameters
			widget = createBooleanWidget(parameter);
			break;
		case Parameter::T_Int:
			layout = createIntegerLayout(dynamic_cast<NumberParameter *>(parameter));
			break;
		case Parameter::T_UnsignedInt:
			layout = createUnsignedIntegerLayout(dynamic_cast<NumberParameter *>(parameter));
			break;
		case Parameter::T_Float:
			layout = createFloatingPointLayout(dynamic_cast<NumberParameter *>(parameter));
			break;
		case Parameter::T_String:
			widget = createStringWidget(parameter);
			break;
		case Parameter::T_Filename:
			layout = createFilenameLayout(dynamic_cast<FilenameParameter *>(parameter));
			break;
		case Parameter::T_Color:
			widget = createColorWidget(parameter);
			connect(parameter, SIGNAL(valueChanged()), SLOT(updateColorWidget()));
			break;
		case Parameter::T_Enumeration:
			widget = createEnumerationWidget(dynamic_cast<EnumerationParameter *>(parameter));
			connect(parameter, SIGNAL(valueChanged()), SLOT(updateEnumerationWidget()));
			break;
		case Parameter::T_TextInfo:
			widget = createTextInfoWidget(parameter);
			break;
		case Parameter::T_Label:
			widget = new QLabel(parameter->getValue().toString());
			//widget->setStyleSheet( "background-color:" + parameter->getTypeColor( Parameter::T_Label).name()); // defined in default.stylesheet
			widget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
			((QLabel*)widget)->setWordWrap(true);
			break;
		case Parameter::T_Generic:
			widget = createGenericWidget(parameter);
			break;
		case Parameter::T_Command:
			//label->setText(""); // don't show a label for command parameters
			widget = createCommandWidget(parameter);
			break;
		case Parameter::T_Geometry:
			//widget = new QLabel(parameterValue.toString());
			return;
			break;
		case Parameter::T_Light:
			//widget = new QLabel(parameterValue.toString());
			return;
			break;
		case Parameter::T_Camera:
			//widget = new QLabel(parameterValue.toString());
			return;
			break;
		case Parameter::T_Image:
			//label->setText(label->text() + "\n");
			widget = createImageWidget(parameter);
			connect(parameter, SIGNAL(valueChanged()), SLOT(updateImageWidget()));
			break;
		case Parameter::T_Group:
			{
				ParameterGroup *parameterGroup = node->getGroupValue(parameter->getName());
				if (parameterGroup) {
					int numberOfParameters = parameterGroup->getParameterList().size();
					widget = new QLabel(QString("Number of contained parameters: %1").arg(numberOfParameters));
				}
			}
			break;
		case Parameter::T_PlugIn:
			widget = createPluginWidget(parameter);
			break;
		default:
			Log::warning(QString("Could not create editing widgets for parameter \"%1\": Type \"%2\" not supported yet.").arg(parameterName).arg(Parameter::getTypeName(parameterType)), "ParameterPanel::addRows");
		}

		if( parameterType == Parameter::T_Label ) {
			formLayout->addRow(widget);
		} else {
			// create the label for the row
			QHBoxLayout *hBoxLayout = new QHBoxLayout;
			QGroupBox *groupBox = new QGroupBox(parameterName, this);
			// disabled because of new style sheet
			/*groupBox->setStyleSheet("QGroupBox {background-color: rgb(220, 220, 220); border-radius: 5px; margin-top: 2ex;}\
			QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top left; padding: 0px 15px;}\
			QSlider {background-color: rgb(220, 220, 220);}");*/

			// add the widget (if it exists) to the form layout
			if (widget) {
				//formLayout->addRow(label, widget);
				//groupBox->addWidget(widget);
				//hBoxLayout->addWidget(label);
				hBoxLayout->addWidget(widget);

				// add the layout (if it exists) to the form layout
			} 
			if (layout) {
				//formLayout->addRow(label, layout);
				//groupBox->addLayout(layout);
				//hBoxLayout->addWidget(label);
				hBoxLayout->addLayout(layout);
			}

			QString descriptionText = parameter->getDescription();
			if( !descriptionText.isEmpty() ) {
				// Show ?-Icon right of parameter to show description
				QLabel* helpIcon = new QLabel();
				helpIcon->setPixmap(QPixmap(":/helpIcon"));
				helpIcon->setToolTip(descriptionText);
				helpIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed ); // make label as large as icon at all time
				hBoxLayout->addWidget(helpIcon);

				groupBox->setToolTip( descriptionText );
				groupBox->setWhatsThis(descriptionText);
			}

			groupBox->setLayout(hBoxLayout);
			formLayout->addRow(groupBox);
		}
	} else {
		// create the label for the row
		QHBoxLayout *hBoxLayout = new QHBoxLayout;
		QGroupBox *groupBox = new QGroupBox(parameterName, this);
		/*groupBox->setStyleSheet("QGroupBox {background-color: rgb(220, 220, 220); border-radius: 5px; margin-top: 2ex;}\
		QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top left; padding: 0px 15px;}\
		QSlider {background-color: rgb(220, 220, 220);}");*/
		//formLayout->addRow(label, createVectorLayout(parameter));
		hBoxLayout->addLayout(createVectorLayout(parameter));
		groupBox->setLayout(hBoxLayout);
		formLayout->addRow(groupBox);

		QString descriptionText = parameter->getDescription();
		if( !descriptionText.isEmpty() ) {
			// Show ?-Icon right of parameter to show description
			QLabel* helpIcon = new QLabel();
			helpIcon->setPixmap(QPixmap(":/helpIcon"));
			helpIcon->setToolTip(descriptionText);
			helpIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed ); // make label as large as icon at all time
			hBoxLayout->addWidget(helpIcon);

			groupBox->setToolTip( descriptionText );
			groupBox->setWhatsThis(descriptionText);
		}
	}
}


//!
//! Adds widgets for editing the parameters of the given parameter group to
//! the form layout.
//!
//! \param parameterGroup The parameter group to create editing widgets for.
//! \param formLayout The form layout to add editing widgets to.
//!
void ParameterPanel::addAdvancedRows ( ParameterGroup *parameterGroup, QFormLayout *formLayout )
{
	const AbstractParameter::List& parameterList = parameterGroup->getParameterList();
	QTabWidget *tabWidget = 0;

	for ( int i = 0; i < parameterList.size(); ++i ) {
		AbstractParameter *abstractParameter = parameterList.at(i);
		if ( abstractParameter->isGroup() ) {
			ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(abstractParameter);
			if( !parameterGroup->isEmpty() && parameterGroup->isVisible()){
				if ( !tabWidget ) {
					// create a tab widget to contain a new form layout for the group
					tabWidget = new QTabWidget(this);
					connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(loadTabPage(int)));
					// add the tab widget to the main form layout
					formLayout->addRow(tabWidget);
				}
				ParameterTabPage *page = new ParameterTabPage(tabWidget, parameterGroup, true);
				tabWidget->addTab(page, parameterGroup->getName());
			}
		} 
		else {
			Parameter *parameter = dynamic_cast<Parameter *>(abstractParameter);
			if ( parameter->isVisible() )
				addAdvancedRows(parameter, formLayout);
			tabWidget = 0;
		}
	}
}


//!
//! Adds widgets for editing the given parameter to the form layout.
//!
//! \param parameter The parameter to create editing widgets for.
//! \param formLayout The form layout to add editing widgets to.
//!
void ParameterPanel::addAdvancedRows ( Parameter *parameter, QFormLayout *formLayout )
{
    // get parameter properties
    const QString &parameterName = parameter->getName();
	const QColor color = parameter->getTypeColor(parameter->getType()).darker();
	
	QLayout *layout = createParameterAdvancedLayout(parameter);

    // create the label for the row
    QHBoxLayout *hBoxLayout = new QHBoxLayout;
	QGroupBox *groupBox = new QGroupBox(parameterName, this);
    groupBox->setStyleSheet( QString("QGroupBox {background-color: rgb(%1, %2, %3); border-radius: 5px; margin-top: 2ex;}\
									 QGroupBox::title {subcontrol-origin: margin; subcontrol-position: top left; padding: 0px 15px;}").arg(color.red()).arg(color.green()).arg(color.blue()) );

    hBoxLayout->addLayout(layout);
    groupBox->setLayout(hBoxLayout);
    formLayout->addRow(groupBox);
}


//!
//! Creates a widget for editing a boolean parameter.
//!
//! \param parameter The parameter to create an editing widget for.
//! \return A widget for editing a boolean parameter.
//!
QWidget * ParameterPanel::createBooleanWidget ( Parameter *parameter )
{
    // get parameter properties
    const QString &parameterName = parameter->getName();
	const QString &nodeName = parameter->getNode()->getName();

    // create an editing widget for the parameter
    QCheckBox *checkBox = new QCheckBox(this);
    checkBox->setProperty("parameterName", parameterName);
	checkBox->setProperty("nodeName", nodeName);
    checkBox->setChecked(parameter->getValue().toBool());

    m_widgetMap.insertMulti(parameterName+"@"+nodeName, checkBox);

    // set up connections for synching the widgets with the parameter
    connect(parameter, SIGNAL(valueChanged()), SLOT(updateBooleanWidget()));
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled(bool)), Qt::QueuedConnection);

    return checkBox;
}


//!
//! Creates a layout with widgets for editing an integer parameter.
//!
//! \param numberParameter The number parameter to create an editing widget for.
//! \param index The index of the value to edit (for parameters that contain lists of values).
//! \return A layout with widgets for editing an integer parameter.
//!
QLayout * ParameterPanel::createIntegerLayout ( NumberParameter *numberParameter, int index /* = 0 */ )
{
    // make sure the given number parameter is valid
    if (!numberParameter) {
        Log::error("The number parameter given is invalid.", "ParameterPanel::createIntegerLayout");
        return 0;
    }

    // get parameter properties
    const QString &name = numberParameter->getName();
	const QString &nodeName = numberParameter->getNode()->getName();
    Parameter::Type parameterType = numberParameter->getType();
    bool enabled = numberParameter->isEnabled();
    bool readOnly = numberParameter->isReadOnly();
    int minValue = numberParameter->getMinValue().toInt();
    int maxValue = numberParameter->getMaxValue().toInt();
    QVariant parameterValue = numberParameter->getValue();
    int stepSize = numberParameter->getStepSize().toInt();
    QString unit = numberParameter->getUnit();
    NumberParameter::InputMethod inputMethod = numberParameter->getInputMethod();
    bool isList = parameterValue.type() == QVariant::List;

    // process parameter value
    int value = 0;
    if (isList)
        value = parameterValue.toList().value(index).toInt();
    else
        value = parameterValue.toInt();

    // process parameter name
    QString parameterName = name;
    if (isList)
        parameterName = QString("%1[%2]").arg(name).arg(index);

    // create the layout and widgets
    QHBoxLayout *layout = new QHBoxLayout();
    QSpinBox *spinBox = 0;
    QSlider *slider = 0;
    if (inputMethod == NumberParameter::IM_SliderPlusSpinBox || inputMethod == NumberParameter::IM_SpinBox) {
        spinBox = new QSpinBox(this);
		spinBox->installEventFilter(this);
        spinBox->setProperty("parameterName", parameterName);
		spinBox->setProperty("nodeName", nodeName);
        spinBox->setProperty("parameterType", (int) parameterType);
        spinBox->setMinimum(minValue);
        spinBox->setMaximum(maxValue);
        spinBox->setSingleStep(stepSize);
        spinBox->setValue(value);
        if (!unit.isEmpty())
            spinBox->setSuffix(unit);
        spinBox->setEnabled(enabled);
        spinBox->setReadOnly(readOnly);
        if (enabled && readOnly)
            grayOut(spinBox);

        layout->addWidget(spinBox);
        m_widgetMap.insertMulti(parameterName+"@"+nodeName, spinBox);
    }
    if (inputMethod == NumberParameter::IM_SliderPlusSpinBox || inputMethod == NumberParameter::IM_Slider) {
        slider = new QSlider(this);
		slider->installEventFilter(this);
        slider->setOrientation(Qt::Horizontal);
        slider->setProperty("parameterName", parameterName);
		slider->setProperty("nodeName", nodeName);
        slider->setProperty("parameterType", (int) parameterType);
        slider->setMinimum(minValue);
        slider->setMaximum(maxValue);
        slider->setSingleStep(stepSize);
        slider->setValue(value);
        slider->setEnabled(enabled && !readOnly);

        layout->addWidget(slider);
        m_widgetMap.insertMulti(parameterName+"@"+nodeName, slider);
    }

    // set up connections for syncing the widgets with the parameter
    if (isList) {
        // only set up one valueChanged() connection (for the parameter's first component)
        if (index == 0)
            connect(numberParameter, SIGNAL(valueChanged(int)), SLOT(updateIntegerWidgets(int)));
    }
	connect(numberParameter, SIGNAL(valueChanged()), SLOT(updateIntegerWidgets()));
    connect(numberParameter, SIGNAL(enabledChanged()), SLOT(updateIntegerWidgets()));

    // set up connections for syncing the parameter with the widgets
    if (spinBox)
        connect(spinBox, SIGNAL(editingFinished()), SLOT(spinBoxEditingFinished()), Qt::QueuedConnection);
    if (slider)
        connect(slider, SIGNAL(valueChanged(int)), SLOT(sliderValueChanged()), Qt::QueuedConnection);

    return layout;
}


//!
//! Creates a layout with widgets for editing an unsigned integer
//! parameter.
//!
//! \param numberParameter The number parameter to create an editing widget for.
//! \param index The index of the value to edit (for parameters that contain lists of values).
//! \return A layout with widgets for editing an unsigned integer parameter.
//!
QLayout * ParameterPanel::createUnsignedIntegerLayout ( NumberParameter *numberParameter, int index /* = 0 */ )
{
    // make sure the given number parameter is valid
    if (!numberParameter) {
        Log::error("The number parameter given is invalid.", "ParameterPanel::createUnsignedIntegerLayout");
        return 0;
    }

    // get parameter properties
    const QString &name = numberParameter->getName();
	const QString &nodeName = numberParameter->getNode()->getName();
    Parameter::Type parameterType = numberParameter->getType();
    bool enabled = numberParameter->isEnabled();
    bool readOnly = numberParameter->isReadOnly();
    unsigned int minValue = numberParameter->getMinValue().toUInt();
    unsigned int maxValue = numberParameter->getMaxValue().toUInt();
    QVariant parameterValue = numberParameter->getValue();
    unsigned int stepSize = numberParameter->getStepSize().toUInt();
    QString unit = numberParameter->getUnit();
    NumberParameter::InputMethod inputMethod = numberParameter->getInputMethod();
    bool isList = numberParameter->getValue().type() == QVariant::List;

    // process parameter value
    unsigned int value = 0;
    if (isList)
        value = parameterValue.toList().value(index).toUInt();
    else
        value = parameterValue.toUInt();

    // process parameter name
    QString parameterName = name;
    if (isList)
        parameterName = QString("%1[%2]").arg(name).arg(index);

    // create the layout and widgets
    QHBoxLayout *layout = new QHBoxLayout();
    QSpinBox *spinBox = 0;
    QSlider *slider = 0;
    if (inputMethod == NumberParameter::IM_SliderPlusSpinBox || inputMethod == NumberParameter::IM_SpinBox) {
        spinBox = new QSpinBox(this);
		spinBox->installEventFilter(this);
        spinBox->setProperty("parameterName", parameterName);
		spinBox->setProperty("nodeName", nodeName);
        spinBox->setProperty("parameterType", (int) parameterType);
        spinBox->setMinimum(minValue);
        spinBox->setMaximum(maxValue);
        spinBox->setSingleStep(stepSize);
        spinBox->setValue(value);
        if (!unit.isEmpty())
            spinBox->setSuffix(unit);
        spinBox->setEnabled(enabled);
        spinBox->setReadOnly(readOnly);
        if (enabled && readOnly)
            grayOut(spinBox);

        layout->addWidget(spinBox);
        m_widgetMap.insertMulti(parameterName+"@"+nodeName, spinBox);
    }
    if (inputMethod == NumberParameter::IM_SliderPlusSpinBox || inputMethod == NumberParameter::IM_Slider) {
        slider = new QSlider(this);
		slider->installEventFilter(this);
        slider->setOrientation(Qt::Horizontal);
        slider->setProperty("parameterName", parameterName);
        slider->setProperty("nodeName", nodeName);
        slider->setProperty("parameterType", (int) parameterType);
        slider->setMinimum(minValue);
        slider->setMaximum(maxValue);
        slider->setSingleStep(stepSize);
        slider->setValue(value);
        slider->setEnabled(enabled && !readOnly);

        layout->addWidget(slider);
        m_widgetMap.insertMulti(parameterName+"@"+nodeName, slider);
    }

    // set up connections for syncing the widgets with the parameter
    if (isList) {
        // only set up one valueChanged() connection (for the parameter's first component)
        if (index == 0)
            connect(numberParameter, SIGNAL(valueChanged(int)), SLOT(updateUnsignedIntegerWidgets(int)));
    }
	connect(numberParameter, SIGNAL(valueChanged()), SLOT(updateUnsignedIntegerWidgets()));
    connect(numberParameter, SIGNAL(enabledChanged()), SLOT(updateUnsignedIntegerWidgets()));

    // set up connections for syncing the parameter with the widgets
    if (spinBox)
        connect(spinBox, SIGNAL(editingFinished()), SLOT(spinBoxEditingFinished()), Qt::QueuedConnection);
    if (slider)
        connect(slider, SIGNAL(valueChanged(int)), SLOT(sliderValueChanged()), Qt::QueuedConnection);

    return layout;
}


//!
//! Creates a layout with widgets for editing a floating point parameter.
//!
//! \param numberParameter The number parameter to create an editing widget for.
//! \param index The index of the value to edit (for parameters that contain lists of values).
//! \return A layout with widgets for editing a floating point parameter.
//!
QLayout * ParameterPanel::createFloatingPointLayout ( NumberParameter *numberParameter, int index /* = 0 */ )
{
    // make sure the given number parameter is valid
    if (!numberParameter) {
        Log::error("The number parameter given is invalid.", "ParameterPanel::createFloatingPointLayout");
        return 0;
    }

    // get parameter properties
    const QString &name = numberParameter->getName();
	const QString &nodeName = numberParameter->getNode()->getName();
    bool enabled = numberParameter->isEnabled();
    bool readOnly = numberParameter->isReadOnly();
    double minValue = numberParameter->getMinValue().toDouble();
    double maxValue = numberParameter->getMaxValue().toDouble();
    QVariant parameterValue = numberParameter->getValue();
    double stepSize = numberParameter->getStepSize().toDouble();
    QString unit = numberParameter->getUnit();
    NumberParameter::InputMethod inputMethod = numberParameter->getInputMethod();
    bool isList = (parameterValue.type() == QVariant::List) || parameterValue.canConvert<Ogre::Vector3>();

    // process parameter value
    double value = 0.0;
    if (isList) {
        if (parameterValue.canConvert<Ogre::Vector3>())
            value = parameterValue.value<Ogre::Vector3>()[index];
        else
            value = parameterValue.toList().value(index).toDouble();
    } else
        value = parameterValue.toDouble();

    // process parameter name
    QString parameterName = name;
    if (isList)
        parameterName = QString("%1[%2]").arg(name).arg(index);

    // create the layout and widgets
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(8);

    // NILZ: Add layers for add or mult
    //QPushButton *layerButton = new QPushButton(this);
    //layerButton->setText("+");
    //layerButton->setFixedSize(15, 15);
    //layout->addWidget(layerButton);

    QDoubleSpinBox *doubleSpinBox = 0;
    DoubleSlider *doubleSlider = 0;
    if (inputMethod == NumberParameter::IM_SliderPlusSpinBox || inputMethod == NumberParameter::IM_SpinBox) {
        doubleSpinBox = new QDoubleSpinBox(this);
		doubleSpinBox->installEventFilter(this);
        doubleSpinBox->setProperty("parameterName", parameterName);
        doubleSpinBox->setProperty("nodeName", nodeName);
        doubleSpinBox->setMinimum(minValue);
        doubleSpinBox->setMaximum(maxValue);
        doubleSpinBox->setSingleStep(stepSize);
		doubleSpinBox->setDecimals(3);
        doubleSpinBox->setValue(value);
        doubleSpinBox->setMinimumWidth(66);
        if (!unit.isEmpty())
            doubleSpinBox->setSuffix(unit);
        doubleSpinBox->setEnabled(enabled);
        doubleSpinBox->setReadOnly(readOnly);
        if (enabled && readOnly)
            grayOut(doubleSpinBox);
        else
            if (numberParameter->isAnimated())
                doubleSpinBox->setStyleSheet("background-color: lightgreen;");

        layout->addWidget(doubleSpinBox);
        m_widgetMap.insertMulti(parameterName+"@"+nodeName, doubleSpinBox);		
    }
    if (inputMethod == NumberParameter::IM_SliderPlusSpinBox || inputMethod == NumberParameter::IM_Slider) {
        doubleSlider = new DoubleSlider(this);
		doubleSlider->installEventFilter(this);
        doubleSlider->setOrientation(Qt::Horizontal);
        doubleSlider->setProperty("parameterName", parameterName);
        doubleSlider->setProperty("nodeName", nodeName);
        doubleSlider->setDoubleMinimum(minValue);
        doubleSlider->setDoubleMaximum(maxValue);
        doubleSlider->setDoubleSingleStep(stepSize);
        doubleSlider->setDoubleValue(value);
        doubleSlider->setEnabled(enabled && !readOnly);
        layout->addWidget(doubleSlider);
        m_widgetMap.insertMulti(parameterName+"@"+nodeName, doubleSlider);
	}

    // set up connections for syncing the widgets with the parameter
    if (isList) {
        // only set up one valueChanged() connection (for the parameter's first component)
        if (index == 0)
            connect(numberParameter, SIGNAL(valueChanged(int)), SLOT(updateFloatingPointWidgets(int)));
    }
	connect(numberParameter, SIGNAL(valueChanged()), SLOT(updateFloatingPointWidgets()));
    connect(numberParameter, SIGNAL(enabledChanged()), SLOT(updateFloatingPointWidgets()));

    // set up connections for syncing the parameter with the widgets
    if (doubleSpinBox)
        connect(doubleSpinBox, SIGNAL(editingFinished()), SLOT(doubleSpinBoxEditingFinished()), Qt::QueuedConnection);
    if (doubleSlider)
        connect(doubleSlider, SIGNAL(doubleValueChanged(double)), SLOT(doubleSliderValueChanged()), Qt::QueuedConnection);

    return layout;
}


//!
//! Creates a widget for editing a string parameter or displaying a read-only
//! parameter's value.
//!
//! \param parameter The string parameter to create an editing widget for.
//! \return A widget for editing a string parameter.
//!
QWidget * ParameterPanel::createStringWidget ( Parameter *parameter )
{
    // get parameter properties
    const QString &parameterName = parameter->getName();
	const QString &nodeName = parameter->getNode()->getName();
    const QString &parameterValue = parameter->getValue().toString();
    bool enabled = parameter->isEnabled();
    bool readOnly = parameter->isReadOnly();

    // create an editing widget for the string
    QLineEdit *lineEdit = new QLineEdit(parameterValue, this);
    lineEdit->setProperty("parameterName", parameterName);
    lineEdit->setProperty("nodeName", nodeName);
    lineEdit->setEnabled(enabled);
    lineEdit->setReadOnly(readOnly);
    if (enabled && readOnly)
        grayOut(lineEdit);

    m_widgetMap.insertMulti(parameterName+"@"+nodeName, lineEdit);

    // set up connections for synching the widgets with the parameter
    connect(parameter, SIGNAL(valueChanged()), SLOT(updateStringWidget()));
    connect(lineEdit, SIGNAL(editingFinished()), SLOT(lineEditEditingFinished()), Qt::QueuedConnection);

    return lineEdit;
}


//!
//! Creates a layout with widgets for editing a filename parameter.
//!
//! \param filenameParameter The filename parameter to create an editing widget for.
//! \return A layout with widgets for editing a filename parameter.
//!
QLayout * ParameterPanel::createFilenameLayout ( FilenameParameter *filenameParameter )
{
    // make sure the given filename parameter is valid
    if (!filenameParameter) {
        Log::error("The filename parameter given is invalid.", "ParameterPanel::createFilenameLayout");
        return 0;
    }

    // get parameter properties
    const QString &parameterName = filenameParameter->getName();
    const QString &nodeName = filenameParameter->getNode()->getName();
    QString filename = FilenameParameter::relativeFilePath( filenameParameter->getValue().toString());
    bool enabled = filenameParameter->isEnabled();
    bool readOnly = filenameParameter->isReadOnly();

	QLabel *fileNotFoundWarning = new QLabel(this);
	fileNotFoundWarning->setPixmap(QPixmap(":/warningIcon"));
	fileNotFoundWarning->setToolTip("The file could not be located in the current working directory!");
	fileNotFoundWarning->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed ); // make label as large as icon at all time
	fileNotFoundWarning->setProperty("parameterName", parameterName);
	fileNotFoundWarning->setProperty("nodeName", nodeName);
	fileNotFoundWarning->setVisible( filenameParameter->FileNotFoundError());
	m_widgetMap.insertMulti(parameterName+"@"+nodeName, fileNotFoundWarning);

    // create an editing widget for the filename
    QLineEdit *lineEdit = new QLineEdit(filename, this);
    lineEdit->setProperty("parameterName", parameterName);
    lineEdit->setProperty("nodeName", nodeName);
    lineEdit->setEnabled(enabled);
    lineEdit->setReadOnly(readOnly);
    if (enabled && readOnly)
        grayOut(lineEdit);

    m_widgetMap.insertMulti(parameterName+"@"+nodeName, lineEdit);

    // create a browse button
    QPushButton *browseButton = new QPushButton(tr("Browse..."), this);
    browseButton->setProperty("parameterName", parameterName);
    browseButton->setProperty("nodeName", nodeName);
    browseButton->setEnabled(enabled);

    // create a reload button
    QPushButton *reloadButton = new QPushButton(tr("Reload"), this);
    reloadButton->setProperty("parameterName", parameterName);
    reloadButton->setProperty("nodeName", nodeName);

    // set up connections for synching the widgets with the parameter
    connect(filenameParameter, SIGNAL(valueChanged()), SLOT(updateFilenameWidgets()));
	connect(lineEdit, SIGNAL(editingFinished()), SLOT(lineEditEditingFinished()), Qt::QueuedConnection);
    connect(browseButton, SIGNAL(clicked()), SLOT(browseButtonClicked()), Qt::QueuedConnection);
    connect(reloadButton, SIGNAL(clicked()), SLOT(reloadButtonClicked()), Qt::QueuedConnection);

    // create the layout with the created filename widgets as children
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(lineEdit, 1);
	layout->addWidget(fileNotFoundWarning);
    layout->addWidget(browseButton);
    layout->addWidget(reloadButton);
    return layout;
}


//!
//! Creates a widget for editing a color parameter.
//!
//! \param parameter The color parameter to create an editing widget for.
//! \return A widget for editing a color parameter.
//!
QWidget * ParameterPanel::createColorWidget ( Parameter *parameter )
{
    // get parameter properties
    const QString &parameterName = parameter->getName();
	const QString &nodeName = parameter->getNode()->getName();
    QColor color = parameter->getValue().value<QColor>();

    // create an editing widget for the color
    QPushButton *colorButton = new QPushButton();
    colorButton->setProperty("parameterName", parameterName);
    colorButton->setProperty("nodeName", nodeName);
    colorButton->setMaximumWidth(75);
    colorButton->setStatusTip(tr("Click to select a color"));
    colorButton->setStyleSheet(QString("background: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));

    m_widgetMap.insertMulti(parameterName+"@"+nodeName, colorButton);

    // set up connections for synching the widgets with the parameter
    connect(colorButton, SIGNAL(clicked(bool)), SLOT(selectColor()), Qt::QueuedConnection);

    return colorButton;
}


//!
//! Creates a widget for editing an enumeration parameter.
//!
//! \param enumerationParameter The enumeration parameter to create an editing widget for.
//! \return A widget for editing an enumeration parameter.
//!
QWidget * ParameterPanel::createEnumerationWidget ( EnumerationParameter *enumerationParameter )
{
    // make sure the given enumeration parameter is valid
    if (!enumerationParameter) {
        Log::error("The enumeration parameter given is invalid.", "ParameterPanel::createEnumerationWidget");
        return 0;
    }

    // get parameter properties
    const QString &parameterName = enumerationParameter->getName();
	const QString &nodeName = enumerationParameter->getNode()->getName();
    const QStringList &literals = enumerationParameter->getLiterals();
    const QStringList &values = enumerationParameter->getValues();
    int parameterValue = enumerationParameter->getValue().toInt();

    // create an editing widget for the enumeration
    QComboBox *comboBox = new QComboBox(this);
    comboBox->setProperty("parameterName", parameterName);
    comboBox->setProperty("nodeName", nodeName);
    for (int i = 0; i < literals.size(); ++i)
        if (literals.at(i) == Parameter::EnumerationSeparator)
            comboBox->insertSeparator(i);
        else {
            if (values.at(i).isEmpty())
                comboBox->insertItem(i, literals.at(i));
            else
                comboBox->insertItem(i, QString("%1 (%2)").arg(literals.at(i)).arg(values.at(i)));
        }
    comboBox->setCurrentIndex(parameterValue);

    m_widgetMap.insertMulti(parameterName+"@"+nodeName, comboBox);

    // set up connections for synching the widgets with the parameter
    connect(comboBox, SIGNAL(currentIndexChanged(int)), SLOT(comboBoxIndexChanged(int)), Qt::QueuedConnection);

    return comboBox;
}


//!
//! Creates a label widget for displaying the text info contained in the
//! given parameter.
//!
//! \param parameter The text info parameter to create a label widget for.
//! \return A label widget for displaying the text info contained in the given parameter.
//!
QWidget * ParameterPanel::createTextInfoWidget ( Parameter *parameter )
{
    // get parameter properties
    const QString &parameterName = parameter->getName();
	const QString &nodeName = parameter->getNode()->getName();
    const QString &parameterValue = parameter->getValue().toString();

    // create a display widget for the text info
    QLabel *label = new QLabel(parameterValue);
    label->setProperty("parameterName", parameterName);
    label->setProperty("nodeName", nodeName);
	label->setWordWrap(true);
    activateCustomContextMenu(label);

    m_widgetMap.insertMulti(parameterName+"@"+nodeName, label);

    // set up connections for synching the widgets with the parameter
    connect(parameter, SIGNAL(valueChanged()), SLOT(updateTextInfoWidget()));

    return label;
}


//!
//! Creates a label widget for displaying the type info contained in the
//! given parameter.
//!
//! \param parameter The text info parameter to create a label widget for.
//! \return A label widget for displaying the type info contained in the given parameter.
//!
QWidget * ParameterPanel::createGenericWidget ( Parameter *parameter )
{
	GenericParameter *genericParameter = dynamic_cast<GenericParameter *>(parameter);

    // get parameter properties
	QString parameterType;
    const QString &parameterName = parameter->getName();
	const QString &nodeName = parameter->getNode()->getName();

	if (genericParameter)
		parameterType = genericParameter->getTypeInfo();
	else
		parameterType = "No GenericParameter";

    // create a display widget for the text info
    QLabel *label = new QLabel(parameterType);
    label->setProperty("parameterName", parameterName);
    label->setProperty("nodeName", nodeName);
    activateCustomContextMenu(label);

    m_widgetMap.insertMulti(parameterName+"@"+nodeName, label);

    // set up connections for synching the widgets with the parameter
    connect(parameter, SIGNAL(valueChanged()), SLOT(updateTextInfoWidget()));

    return label;
}


//!
//! Creates a button widget for executing the command represented by the
//! given parameter.
//!
//! \param parameter The command parameter to create a button widget for.
//! \return A widget for execting the command represented by the given parameter.
//!
QWidget * ParameterPanel::createCommandWidget ( Parameter *parameter )
{
    // get parameter properties
    const QString &parameterName = parameter->getName();
	const QString &nodeName = parameter->getNode()->getName();

    // create an button widget for the command
    QPushButton *button = new QPushButton(parameterName);
    button->setProperty("parameterName", parameterName);
    button->setProperty("nodeName", nodeName);

    // set up connection for executing the command
    connect(button, SIGNAL(clicked(bool)), SLOT(commandButtonClicked())); //, Qt::QueuedConnection);

    return button;
}


//!
//! Creates a widget displaying information about the image represented by
//! the given parameter.
//!
//! \param parameter The image parameter to create a widget for.
//! \return A widget displaying information about the image represented by the given parameter, or 0 if not applicable.
//!
QWidget * ParameterPanel::createImageWidget ( Parameter *parameter )
{
    QLabel *label = 0;
    if (parameter->getPinType() == Parameter::PT_Input)
        if (parameter->isConnected()) {
            // show details of the connected image in a label widget
			Ogre::TexturePtr inputTexture = parameter->getNode()->getTextureValue(parameter->getName());
            if (!inputTexture.isNull()) {
                unsigned int width = (unsigned int) inputTexture->getWidth();
                unsigned int height = (unsigned int) inputTexture->getHeight();
                QString pixelFormatName = Ogre::PixelUtil::getFormatName(inputTexture->getFormat()).c_str();
                label = new QLabel(QString("Image dimensions: %1x%2\nPixel format: %3").arg(width).arg(height).arg(pixelFormatName));
            }
        } else
            label = new QLabel("No input image connected.\n");

    // check if the label exists and if so, activate the custom context menu for the label
    if (label)
        activateCustomContextMenu(label);

    return label;
}


//!
//! Creates a layout with widgets for editing a parameter that stores
//! several values of a specific type.
//!
//! \param parameter The vector parameter to create a layout with editing widgets for.
//! \return A layout with widgets for editing a parameter that stores several values of a specific type.
//!
QLayout * ParameterPanel::createVectorLayout ( Parameter *parameter )
{
    // get parameter properties
    Parameter::Type parameterType = parameter->getType();
    QVariant parameterValue = parameter->getValue();
    QVariantList values = parameterValue.toList();

    // create the layout
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(16);

    // check if the parameter is a numeric parameter
    if (parameterType == Parameter::T_Int || parameterType == Parameter::T_UnsignedInt || parameterType == Parameter::T_Float) {
        NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(parameter);
        // check if the number parameter contains its values in an OGRE vector
        unsigned int numberOfValues = values.size();
        if (parameterValue.canConvert<Ogre::Vector3>())
            numberOfValues = 3;

        // iterate over the number of values
        for (unsigned int i = 0; i < numberOfValues; ++i) {
            QLayout *layout = 0;
            switch (parameterType) {
                case Parameter::T_Int:
                    layout = createIntegerLayout(numberParameter, i);
                    break;
                case Parameter::T_UnsignedInt:
                    layout = createUnsignedIntegerLayout(numberParameter, i);
                    break;
                case Parameter::T_Float:
                    layout = createFloatingPointLayout(numberParameter, i);
                    break;
            }
            if (layout)
                hboxLayout->addLayout(layout);
        }
    } else {
        // add a new label with the custom context menu to the layout
        QLabel *label = new QLabel(QString("Number of values in list: %1").arg(values.size()));
        activateCustomContextMenu(label);
        hboxLayout->addWidget(label);
    }

    return hboxLayout;
}


//!
//! Creates a layout with widgets for editing a parameters advanced properties.
//!
//! \param parameter The parameter to create a layout with editing widgets for.
//! \return A layout with widgets for editing a parameters advanced properties.
//!
QLayout * ParameterPanel::createParameterAdvancedLayout ( Parameter *parameter )
{
    // layout
    QHBoxLayout *hboxLayout = new QHBoxLayout();

	// widgets for parameter infos
	const Parameter::Type parameterType = parameter->getType();
	const int multiplicity = parameter->getMultiplicity();
	const unsigned int size = parameter->getSize();
	const QString &parameterName = parameter->getName();
	const QString &nodeName = parameter->getNode()->getName();
	QLabel *typeLabel = new QLabel("Type: " + Parameter::getTypeName(parameterType));
	QLabel *sizeLabel = new QLabel("Size: " + QString::number(size));
	QLabel *multiplicityLabel = new QLabel("Mult.: " + QString::number(multiplicity));
	
	// layout widgets
	QLabel *separatorLabel = new QLabel("");
	separatorLabel->setFrameShape(QFrame::VLine);
	separatorLabel->setFrameShadow(QFrame::Sunken);
	separatorLabel->setFrameShadow(QFrame::Sunken);
	separatorLabel->setMidLineWidth(1);
	QLabel *separatorLabel1 = new QLabel("");
	separatorLabel1->setFrameShape(QFrame::VLine);
	separatorLabel1->setFrameShadow(QFrame::Sunken);
	separatorLabel1->setFrameShadow(QFrame::Sunken);
	separatorLabel1->setMidLineWidth(1);

	// widget for self-evaluation
	const bool selfEvaluatung = parameter->isSelfEvaluating();
    QCheckBox *checkBox = new QCheckBox(this);
	checkBox->setText("Self-Evaluation\n");
    checkBox->setProperty("parameterName", parameterName);
    checkBox->setProperty("nodeName", nodeName);
	checkBox->setChecked(selfEvaluatung);

	// widget for pin-type
	const int pinType = parameter->getPinType();
    static const QStringList literals = QStringList() << "None" << "In" << "Out";
	QComboBox *pinTypecomboBox = createComboBox(literals);
	pinTypecomboBox->setProperty("parameterName", parameterName);
	pinTypecomboBox->setProperty("nodeName", nodeName);
    pinTypecomboBox->setCurrentIndex(pinType+1);
	pinTypecomboBox->setMaximumWidth(75);
	//if (pinType != -1)
	//	pinTypecomboBox->setDisabled(true);

	// create list of slots
	QStringList signatureList = QStringList() << "None";
	static const int slotOffset = 8;
	int count, changeIndex, processingIndex, auxProcessingIndex;
	count = changeIndex = processingIndex = auxProcessingIndex = 0;
	const QByteArray &changeFunction = parameter->getChangeFunction();
	const QByteArray &processingFunction = parameter->getProcessingFunction();
	const QByteArray &auxProcessingFunction = parameter->getAuxProcessingFunction();
	const QMetaObject *qMObject = parameter->getNode()->metaObject();
	for ( int i=0; i<qMObject->methodCount(); ++i ) {
		const QMetaMethod &mMethod = qMObject->method(i);
		if ( mMethod.methodType() == QMetaMethod::Slot ) {
			if (count > slotOffset) {
#if QT_VERSION >= 0x050000
				const QByteArray &signature = mMethod.methodSignature();
#else
				const QByteArray &signature = mMethod.signature();
#endif
				signatureList.append(signature);
				if (!changeFunction.isEmpty() && 
						changeFunction.contains(signature))
					changeIndex = count-slotOffset;
				else if (!processingFunction.isEmpty() && 
						processingFunction.contains(signature))
					processingIndex = count-slotOffset;
				else if (!auxProcessingFunction.isEmpty() && 
						auxProcessingFunction.contains(signature))
					auxProcessingIndex = count-slotOffset;
			}
			count++;
		}
	}

	// widget for change function
	QComboBox *changeComboBox = createComboBox(signatureList);
	changeComboBox->setProperty("parameterName", parameterName);
	changeComboBox->setProperty("nodeName", nodeName);
    changeComboBox->setCurrentIndex(changeIndex);
	//changeComboBox->setMaximumWidth(100);
	changeComboBox->setToolTip("set change function");
	
	// widget for processing function
	QComboBox *processingComboBox = createComboBox(signatureList);
	processingComboBox->setProperty("parameterName", parameterName);
	processingComboBox->setProperty("nodeName", nodeName);
	processingComboBox->setCurrentIndex(processingIndex);
	//processingComboBox->setMaximumWidth(100);
	processingComboBox->setToolTip("set processing function");

	// set up connections for synchronising the widgets with the parameter
    connect(checkBox, SIGNAL(toggled(bool)), SLOT(selfEvaluatingToggled(bool)), Qt::QueuedConnection);
    connect(pinTypecomboBox, SIGNAL(currentIndexChanged(int)), SLOT(parameterPinTypeChanged(int)), Qt::QueuedConnection);
	connect(processingComboBox, SIGNAL(currentIndexChanged(int)), SLOT(parameterProcessingFunctionChanged(int)), Qt::QueuedConnection);
	connect(changeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(parameterChangeFunctionChanged(int)), Qt::QueuedConnection);

	// add widgets to layout
	hboxLayout->addWidget(typeLabel);
	hboxLayout->addWidget(separatorLabel);
	hboxLayout->addWidget(sizeLabel);
	hboxLayout->addWidget(separatorLabel1);
	hboxLayout->addWidget(multiplicityLabel);
	hboxLayout->addWidget(checkBox);
	hboxLayout->addWidget(pinTypecomboBox);
	hboxLayout->addWidget(changeComboBox);
	hboxLayout->addWidget(processingComboBox);
    return hboxLayout;
}


//!
//! Ctreates and initialices a combo box with the given litera- and value list.
//!
//! \param literals The string list containing the literals.
//! \param values The string list containing the values.
//!
//! \return The initialiced combo box.
//!
QComboBox * ParameterPanel::createComboBox(const QStringList &literals)
{
	QComboBox *comboBox = new QComboBox(this);
	for (int i = 0; i < literals.size(); ++i) {
		if (literals.at(i) == Parameter::EnumerationSeparator)
			comboBox->insertSeparator(i);
		else
			comboBox->insertItem(i, literals.at(i));
	}
	return comboBox;
}


//!
//! Notifies connected objects that a parameter of the currently edited
//! object should be changed.
//!
//! \param parameterName The name of the parameter to change.
//! \param value The new value for the parameter.
//!
void ParameterPanel::requestParameterChange ( const QString &parameterName, const QString &nodeName, const QVariant &value )
{
    m_editedWidget = sender();

	emit parameterChangeRequested(nodeName, parameterName, value);

    m_editedWidget = 0;
}


//!
//! Filters the parameters displayed in the panel by the search text
//! entered in the search edit widget.
//!
void ParameterPanel::filterParameters ()
{
	foreach(Node *node, m_nodes)
		node->setSearchText(m_searchEdit->text());
	QList<Node *> nodes(m_nodes.values());
	clear();
	showNodes(nodes);
}


//!
//! Creates a plugin widget
//!
//! \param parameter the parameter of the widget
//! \return The plugin widget
//!
QWidget * ParameterPanel::createPluginWidget (Parameter *parameter)
{
	ParameterPlugin *parameterPlugin =  dynamic_cast<ParameterPlugin *>(parameter);

	if(!parameterPlugin){
		Log::error(QString("Cannot convert parameter."), "ParameterPanel::createPluginWidget");
		QWidget * widget = new QWidget(this);
		return widget;
	}

	const QString &call = parameterPlugin->getCall();
	const QString &parameterName = parameterPlugin->getName();
	const QString &nodeName = parameterPlugin->getNode()->getName();

	
	WidgetPlugin *widgetPlugin = WidgetFactory::createWidget(call, parameterPlugin, this);

	m_widgetMap.insertMulti(parameterName+"@"+nodeName, widgetPlugin);

	return widgetPlugin;
}


//!
//! Removes the Widget and all childs from internal widget map
//!
//! \param name The name of the widget to be removed from map.
//!
void ParameterPanel::deletefromItemMap ( const QObject *object )
{
	const QObjectList &children = object->children();
	foreach(const QObject *child, children) {
		deletefromItemMap(child);
		const QString &nodeName = child->property("nodeName").toString();
		const QString &parameterName = child->property("parameterName").toString();
		if (!parameterName.isEmpty() && !nodeName.isEmpty())
			m_widgetMap.remove(parameterName+"@"+nodeName);
	}
}

} // end namespace Frapper