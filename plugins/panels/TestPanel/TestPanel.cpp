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
//! \file "TestPanel.cpp"
//! \brief Implementation file for TestPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       07.12.2009 (last updated)
//!

#include "TestPanel.h"
#include "ParameterTabPage.h"
#include "DoubleSlider.h"
#include "NodeFactory.h"
#include "Log.h"


namespace Frapper {

///
/// Constructors and Destructors
///

//!
//! Constructor of the TestPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
TestPanel::TestPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    ViewPanel(ViewPanel::T_PluginPanel, parent, flags)
{

	// Creation of labels and List Widgets

	QLabel *labelNodes = new QLabel("Nodes:", this);
	labelNodes->setGeometry(20, 10,100,20);

	m_ListNodes = new QListWidget(this);
	m_ListNodes->setGeometry(10, 30, 100, 280);

	QLabel *labelInputParameters = new QLabel("Input Parameters:", this);
	labelInputParameters->setGeometry(160, 10, 150,20);
	m_InputParameters = new QListWidget(this);
	m_InputParameters->setGeometry(150, 30, 400, 80);

	QLabel *labelParameters = new QLabel("Parameters:", this);
	labelParameters->setGeometry(160,110,150,20);
	m_NodeParameters = new QListWidget(this);
	m_NodeParameters->setGeometry(150, 130, 400, 80);

	QLabel *labelOutputParameters = new QLabel("Output Parameters:", this);
	labelOutputParameters->setGeometry(160,210,150,20);
	m_OutputParameters = new QListWidget(this);
	m_OutputParameters->setGeometry(150, 230, 400, 80);

	// connection of a signal to a slot

	QObject::connect(m_ListNodes, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(nodeSelected(QListWidgetItem *)));

	m_description = false;
}


//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TestPanel::~TestPanel ()
{
	delete m_ListNodes;
	delete m_InputParameters;
	delete m_NodeParameters;
	delete m_OutputParameters;
}

///
/// Public Funcitons
///


//!
//! Connects the panel with the scene.
//!
//! \param *nodeModel NodeModel of the scene
//! \param *sceneModel SceneModel of the scene
//!
void TestPanel::registerControl(NodeModel *nodeModel, SceneModel *sceneModel)
{
	m_nodeModel = nodeModel;
	m_sceneModel = sceneModel;
	
	QObject::connect(m_nodeModel, SIGNAL(nodeSelected(Node *)), this, SLOT(updateSelectedNode(Node *)));
	QObject::connect(m_sceneModel, SIGNAL(modified()), this, SLOT(update()));
	QObject::connect(m_sceneModel, SIGNAL(objectCreated(const QString &)), this, SLOT(update()));
	QObject::connect(m_sceneModel, SIGNAL(selectionChanged(bool)), this, SLOT(update()));
	
	update();	
}


//!
//! Fills the given tool bars with actions for the TestPanel view.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void TestPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
	QAction *ui_descriptionAction;
	ui_descriptionAction = new QAction(this);
    ui_descriptionAction->setObjectName(QString::fromUtf8("ui_descriptionAction"));
    ui_descriptionAction->setCheckable(true);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/infoIcon"), QSize(), QIcon::Normal, QIcon::Off);
    ui_descriptionAction->setIcon(icon1);
	ui_descriptionAction->setToolTip("Show Description");

	connect(ui_descriptionAction, SIGNAL(toggled(bool)), this, SLOT(showDiscription(bool)));

	mainToolBar->addAction(ui_descriptionAction);
}

///
/// Private Slots
///

//!
//! Is called a node is selected
//!
//! \param selecedNode the selected wich was selected
//!
void TestPanel::updateSelectedNode(Node * selectedNode)
{
	m_node = selectedNode;

	m_InputParameters->clear();
	m_OutputParameters->clear();
	m_NodeParameters->clear();

	QString nodeName = selectedNode->getName();
	QString typeName = selectedNode->getTypeName();

	const AbstractParameter::List& parameterList = selectedNode->getParameterRoot()->getParameterList();

	 for (int i = 0; i < parameterList.size(); ++i) {
		AbstractParameter *abstractParameter = parameterList.at(i);
		if (abstractParameter->isGroup()) {
            ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(abstractParameter);
			QString groupName = parameterGroup->getName();

			QListWidgetItem *newItem = new QListWidgetItem;
			newItem->setText(groupName);
			newItem->setIcon(QIcon(":/hierarchyEditorIcon"));
			newItem->setStatusTip("Parameter Group");
			newItem->setBackgroundColor(QColor(210,210,210));

			const AbstractParameter::List& abstractParameterGroup =  parameterGroup->getParameterList();

			QListWidgetItem *subItem = NULL;
			Parameter::PinType pinType = Parameter::PT_None;

			for(int j = 0; j < abstractParameterGroup.size(); ++j){
				AbstractParameter *abstractParameterInGroup = abstractParameterGroup.at(j);
				Parameter *parameterInGroup = dynamic_cast<Parameter *>(abstractParameterInGroup);
				pinType = parameterInGroup->getPinType();
				subItem = addItem(parameterInGroup, true);
			}	

			QListWidget *list;
	
			if(pinType == Parameter::PT_Input)
				list = m_InputParameters;	
			
			if(pinType == Parameter::PT_Output)
				list = m_OutputParameters;
			
			if(pinType == Parameter::PT_None)
				list = m_NodeParameters;
				
			int row = list->row(subItem) - abstractParameterGroup.size();
			row++;

			if(row < 0)
				row = 0;

			list->insertItem(row, newItem);

		} else {
			Parameter *parameter = dynamic_cast<Parameter *>(abstractParameter);
			addItem(parameter, false);	
		}
	 }

	 update();
}

//!
//! Adds a item to the list
//!
//! \param parameter Parameter to be added to a list
//! \param groupElement true if element of a parameter group
//! \return the created QListWidgetItem
//!
QListWidgetItem* TestPanel::addItem(Parameter *parameter, bool groupElement)
{
	QString parameterName = parameter->getName();
	Parameter::PinType pin = parameter->getPinType();
	Parameter::Type parameterType = parameter->getType();
	bool parameterConnected = parameter->isConnected();

	QListWidgetItem *newItem = new QListWidgetItem;

	QString description;	

	switch(parameterType){
		case(Parameter::T_Bool):
			description = QString(" (bool)");
			newItem->setIcon(QIcon(":/preferencesIcon"));
		break;
		case(Parameter::T_Camera):
			description = QString(" (camera)");
			newItem->setIcon(QIcon(":/cameraIcon"));
		break;
		case(Parameter::T_Float):
			description = QString(" (float)");
			newItem->setIcon(QIcon(":/stage9Icon"));
		break;
		case(Parameter::T_Int):
			description = QString(" (integer)");
			newItem->setIcon(QIcon(":/stage1Icon"));
		break;
		case(Parameter::T_Color):
			description = QString(" (color)");
			newItem->setIcon(QIcon(":/paletteIcon"));
		break;
		case(Parameter::T_Light):
			description = QString(" (light)");
			newItem->setIcon(QIcon(":/lightIcon"));
		break;
		case(Parameter::T_Geometry):
			description = QString(" (geometry)");
			newItem->setIcon(QIcon(":/geometryIcon"));
		break;
		case(Parameter::T_Image):
			description = QString(" (image)");
			newItem->setIcon(QIcon(":/stereoIcon"));
		break;
		case(Parameter::T_String):
			description = QString(" (string)");
			newItem->setIcon(QIcon(":/documentationIcon"));
		break;
		case(Parameter::T_PlugIn):
			{
			description = QString(" (plugin \"");
			newItem->setIcon(QIcon(":/pluginIcon"));
			// Use this if frapper also knows dynamic widgets
			ParameterPlugin *parameterPlugin =  dynamic_cast<ParameterPlugin *>(parameter);
			description.append(parameterPlugin->getCall());
			description.append("\")");
			}

		break;
		default:
			description = QString(" (undefined type)");
			newItem->setIcon(QIcon(":/tagIcon"));
	}

	if(parameterConnected)
		description.append(QString(" (connected)"));

	if(m_description)
		parameterName.append(description);

	if(groupElement){
		parameterName.insert(0,QString("   "));
		newItem->setBackgroundColor(QColor(230,230,230));
	}

	newItem->setText(parameterName);

	if(pin == Parameter::PT_Input){
		newItem->setToolTip("Input Parameter");
		m_InputParameters->addItem(newItem);
	}

	if(pin == Parameter::PT_Output){
		newItem->setToolTip("Output Parameter");
		m_OutputParameters->addItem(newItem);
	}

	if(pin == Parameter::PT_None){
		newItem->setToolTip("No Pin Type");
		m_NodeParameters->addItem(newItem);
	}	

	return newItem;
}

//!
//! Updates the panel if the node model changes
//!
void TestPanel::update()
{
	m_ListNodes->clear();

	QStringList nodeNames = m_nodeModel->getNodeNames();
	QStringList selectedNodes = m_sceneModel->getSelectedObjects();

	for(int i = 0; i < nodeNames.length(); ++i){
		QString nodeName = nodeNames.at(i);
		QListWidgetItem *newItem = new QListWidgetItem;

		if(selectedNodes.contains(nodeNames.at(i)))
			newItem->setBackgroundColor(QColor(150,255,150));

		newItem->setText(nodeName);
		m_ListNodes->addItem(newItem);	
	}

	if(selectedNodes.isEmpty()){
		m_InputParameters->clear();
		m_OutputParameters->clear();
		m_NodeParameters->clear();
	}

}

//!
//! Updates the scene and the panel if a node is selected in the panel
//!
void TestPanel::nodeSelected(QListWidgetItem * listItem)
{
	QString nodeName = listItem->text();
	Node *selectedNode = m_nodeModel->getNode(nodeName);
	m_sceneModel->selectObject(nodeName);
}


//!
//! Sets the description mode
//!
//! \param description boolean value of the action´s state
//!
void TestPanel::showDiscription(bool description)
{
	m_description = description;
	updateSelectedNode(m_node);
}

} // end namespace
