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
//! \file "SceneModel.cpp"
//! \brief Implementation file for SceneModel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.11.2013 (last updated)
//!

#include "SceneModel.h"
#include "NodeFactory.h"
#include "NumberParameter.h"
#include "ConnectionGraphicsItem.h"
#include "RenderNode.h"
#include "OgreManager.h"
#include "Log.h"
#include <QItemSelection>
#include <QProgressDialog>
#include <QFileDialog>
#include <QtCore/QFileInfo>

namespace Frapper {

///
/// Private Static Data
///


//!
QString SceneModel::s_sceneFileName = "";

//!
QString SceneModel::s_sceneWorkingDirectory = "";

///
/// Static Functions
///

void SceneModel::setSceneFileName( QString sceneFileName )
{
	s_sceneFileName = sceneFileName;
}

QString SceneModel::getSceneFileName()
{
	return s_sceneFileName;
}

void SceneModel::setWorkingDirectory( QString workingDirectory )
{
	s_sceneWorkingDirectory = workingDirectory;
}

QString SceneModel::getWorkingDirectory()
{
	return s_sceneWorkingDirectory;
}

QString SceneModel::selectWorkingDirectory()
{
	QString currentWorkingDirectory = s_sceneWorkingDirectory;
	QString workingDirectory = QFileDialog::getExistingDirectory(0, QString(tr("Select Working Directory")), currentWorkingDirectory);

	if( workingDirectory == "" || currentWorkingDirectory == workingDirectory ) {
		Log::debug("Working directory was not changed.", "Application::selectWorkingDirectory");
	} else {
		Log::debug("Working directory changed to "+workingDirectory+".", "Application::selectWorkingDirectory");
		SceneModel::setWorkingDirectory(QFileInfo(workingDirectory).absoluteFilePath());
	}
	return workingDirectory;
}


///
/// Constructors and Destructors
///

//!
//! Constructor of the SceneModel class.
//!
SceneModel::SceneModel () :
m_name(""),
m_nodeModel(new NodeModel()),
m_selectionModel(new QItemSelectionModel(m_nodeModel)),
m_graphicsScene(new QGraphicsScene(this)),
m_newNodeIndex(0),
m_frameParameter(new NumberParameter("Frame", Parameter::T_Int, 1)),
m_frameRangeParameter(new NumberParameter("FrameRange", Parameter::T_Int, 0)),
m_frameStep(1),
m_fpsParameter(new NumberParameter("FPS", Parameter::T_UnsignedInt, 25)),
//m_viewNode(0),
m_timerId(0),
m_sceneRoot(0),
m_newNodePosition(QPointF()),
// timeline control actions
m_inFrameAction(new QAction(QIcon(":/firstFrameIcon"), tr("Jump to In Frame"), this)),
m_previousKeyAction(new QAction(QIcon(":/previousKeyIcon"), tr("Jump to Previous Keyframe"), this)),
m_previousFrameAction(new QAction(QIcon(":/previousFrameIcon"), tr("Go to Previous Frame"), this)),
m_playAction(new QAction(QIcon(":/playIcon"), tr("Play"), this)),
m_realtimeAction(new QAction(QIcon(":/realtimeIcon"), tr("Realtime Animation"), this)),
m_nextFrameAction(new QAction(QIcon(":/nextFrameIcon"), tr("Go to Next Frame"), this)),
m_nextKeyAction(new QAction(QIcon(":/nextKeyIcon"), tr("Jump to Next Keyframe"), this)),
m_outFrameAction(new QAction(QIcon(":/lastFrameIcon"), tr("Jump to Out Frame"), this)),
m_separatorAction(new QAction(this)),
m_playOptionsAction(new QAction(tr("Options"), this)),
m_playLoopingAction(new QAction(QIcon(":/playLoopingIcon"), tr("Play Looping"), this)),
m_playOnceAction(new QAction(QIcon(":/playOnceIcon"), tr("Play Once"), this))
{
    // initialize time parameters
    m_frameParameter->setMinValue(1);
    m_frameParameter->setMaxValue(450);
    m_frameRangeParameter->setMinValue(m_frameParameter->getMinValue());
    m_frameRangeParameter->setMaxValue(m_frameParameter->getMaxValue());
    m_fpsParameter->setMinValue(1);
    m_fpsParameter->setMaxValue(450);

    // initialize view node list
    for (unsigned int i = 0; i < NUMBER_OF_STAGES; ++i) {
        m_viewNodeList.append(0);
    }

    // set up selection-specific signal/slot connections
    connect(m_nodeModel, SIGNAL(clearSelectionRequested()), m_selectionModel, SLOT(clear()));
    connect(m_selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), SLOT(selectionModelSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_graphicsScene, SIGNAL(selectionChanged()),SLOT(graphicsSceneSelectionChanged()));

    // relay the modified signal from the node model
    connect(m_nodeModel, SIGNAL(modified()), SIGNAL(modified()));

    // set the dimensions of the graphics scene
    static const int SceneWidth = 8000;
    static const int SceneHeight = 8000;
    m_graphicsScene->setSceneRect(QRectF(-SceneWidth / 2, -SceneHeight / 2, SceneWidth, SceneHeight));

    m_graphicsScene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);

    // initialize scene model actions
    initializeActions();
}


//!
//! Destructor of the SceneModel class.
//!
SceneModel::~SceneModel ()
{
    // stop playing the animation if it is still running
    if (m_playAction->isChecked())
        m_playAction->setChecked(false);

    // destroy the play options action menu
    QMenu *playOptionsMenu = m_playOptionsAction->menu();
    if (playOptionsMenu) {
        m_playOptionsAction->setMenu(0);
        delete playOptionsMenu;
    }

    // deactivate graphics scene selection changed handler
    disconnect(m_graphicsScene, SIGNAL(selectionChanged()), this, SLOT(graphicsSceneSelectionChanged()));

    // remove all objects from the scene
    clear();

    // destroy selection model
    Log::debug("Destroying selection model...", "SceneModel::~SceneModel");
    delete m_selectionModel;
    m_selectionModel = 0;

    // destroy scene model
    Log::debug("Destroying scene model...", "SceneModel::~SceneModel");
    delete m_nodeModel;
    m_nodeModel = 0;

    // destroy graphics scene
    Log::debug("Destroying graphics scene...", "SceneModel::~SceneModel");
    delete m_graphicsScene;
    m_graphicsScene = 0;

    // destroy time parameters
    Log::debug("Destroying time parameters...", "SceneModel::~SceneModel");
    delete m_frameParameter;
    m_frameParameter = 0;
    delete m_frameRangeParameter;
    m_frameRangeParameter = 0;
    delete m_fpsParameter;
    m_fpsParameter = 0;
}


///
/// Public Functions
///

//!
//! Returns the name of the scene.
//!
//! \return The name of the scene.
//!
QString SceneModel::getName () const
{
    return m_name;
}

//!
//! Sets the name of the scene.
//!
//! \param name The name of the scene.
//!
void SceneModel::setName (const QString &name)
{
    m_name = name;
    emit sceneNameChanged(name);
}

//!
//! Returns the standard item model that holds all scene objects like meshes,
//! cameras and lights.
//!
//! \return The standard item model that holds all scene objects.
//!
NodeModel * SceneModel::getNodeModel () const
{
    return m_nodeModel;
}


//!
//! Returns the selection model to use for synchronizing selections in all
//! views.
//!
//! \return The selection model to use for synchronizing selections.
//!
QItemSelectionModel * SceneModel::getSelectionModel () const
{
    return m_selectionModel;
}


//!
//! Returns the graphics graphics item associated with a node by name.
//!
//! \return The graphics item associated with the node.
//!
QGraphicsItem * SceneModel::getGraphicsItem ( const QString &name ) const
{
	QHash<QString, QGraphicsItem *>::const_iterator itemIter = m_graphicsItemMap.constFind(name);
	
	// check if there is a group item
	if (itemIter == m_graphicsItemMap.end()) {
		const QString &groupName = m_nodeModel->findGroupByNodeName(name);
		if (!groupName.isEmpty())
			itemIter = (m_graphicsItemMap.constFind(groupName));
		else
			return 0;
	}

	return itemIter.value();
}


//!
//! Returns the graphics scene to use for all network views.
//!
//! \return The graphics scene to use for all network views.
//!
QGraphicsScene * SceneModel::getGraphicsScene () const
{
    return m_graphicsScene;
}


//!
//! Returns a list of actions representing scene time controls.
//!
//! \return A list of actions representing scene time controls.
//!
QList<QAction *> SceneModel::getControlActions () const
{
    return QList<QAction *>()
        << m_inFrameAction
        << m_previousKeyAction
        << m_previousFrameAction
        << m_playAction
        << m_nextFrameAction
        << m_nextKeyAction
        << m_outFrameAction
        << m_separatorAction
        << m_playOptionsAction
        << m_separatorAction
        << m_realtimeAction
        ;
}


//!
//! Builds a list of object names from the current selection.
//!
//! \return A list of names of the currently selected objects.
//!
QStringList SceneModel::getSelectedObjects ( bool onlyConnections /* = false */ ) const
{
    QStringList result;
    QItemSelection selection = m_selectionModel->selection();
    for (int i = 0; i < selection.size(); ++i) {
        QModelIndex parentModelIndex = selection.at(i).parent();
        if (parentModelIndex.isValid()) {
            QStandardItem *parentItem = m_nodeModel->itemFromIndex(parentModelIndex);
            // do not include connections or other objects
            if (onlyConnections) {
                if (parentItem->text() != "Connections")
                    continue;
            }
            else {
                if (parentItem->text() == "Connections")
                    continue;
            }
            for (int n = 0; n < selection.at(i).height(); ++n) {
                QStandardItem *item = parentItem->child(selection.at(i).top() + n);
                if (item)
					if( !item->data().isNull() )
						result << item->data().toString();
					else
						result << item->text();
            }
        }
    }
    return result;
}


//!
//! Returns the frame step currently set for the scene.
//!
//! \return The frame step currently set for the scene.
//!
int SceneModel::getFrameStep () const
{
    return m_frameStep;
}


//!
//! Returns the frame rate currently set for the scene.
//!
//! \return The frame rate currently set for the scene.
//!
int SceneModel::getFrameRate () const
{
    return m_fpsParameter->getValue().toInt();
}


//!
//! Fills the given frame index variables with the frame range available
//! for the scene time.
//!
//! \param firstFrame The index of the first frame in the scene's time range.
//! \param lastFrame The index of the last frame in the scene's time range.
//!
void SceneModel::getFrameRange ( int& firstFrame, int& lastFrame ) const
{
    firstFrame = -10000000;
    lastFrame = 10000000;
}


//!
//! Returns the index of the current frame in the scene's time.
//!
//! \return The index of the current frame in the scene's time.
//!
int SceneModel::getCurrentFrame () const
{
    return m_frameParameter->getValue().toInt();
}


//!
//! Returns the index of the start frame in the scene's time.
//!
//! \return The index of the start frame in the scene's time.
//!
int SceneModel::getStartFrame () const
{
    return m_frameParameter->getMinValue().toInt();
}


//!
//! Returns the index of the end frame in the scene's time.
//!
//! \return The index of the end frame in the scene's time.
//!
int SceneModel::getEndFrame () const
{
    return m_frameParameter->getMaxValue().toInt();
}


//!
//! Returns the index of the in frame in the scene's time.
//!
//! \return The index of the in frame in the scene's time.
//!
int SceneModel::getInFrame () const
{
    return m_frameRangeParameter->getMinValue().toInt();
}


//!
//! Returns the index of the out frame in the scene's time.
//!
//! \return The index of the out frame in the scene's time.
//!
int SceneModel::getOutFrame () const
{
    return m_frameRangeParameter->getMaxValue().toInt();
}


//!
//! Creates the OGRE scene root scene node.
//!
void SceneModel::createSceneRoot ()
{
    Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    if (sceneManager)
        m_sceneRoot = sceneManager->getRootSceneNode()->createChildSceneNode("sceneRoot");
}


//!
//! Creates the scene objects by parsing the COLLADA element tree contained
//! in the given root element.
//!
//! \param rootElement The root element of the tree to build the scene from.
//!
void SceneModel::createScene ( daeElement *rootElement )
{
    // make sure the given root element is valid
    if (!rootElement) {
        Log::warning("The given root element is invalid.", "SceneModel::createScene");
        return;
    }

    // display a progress dialog for creating the scene
    QProgressDialog progressDialog (tr("Loading scene..."), tr("Abort"), 0, 100);
    progressDialog.setWindowTitle(tr("Open Scene"));
    progressDialog.setWindowModality(Qt::ApplicationModal);

    // load scene properties
    daeElement *sceneElement = rootElement->getChild("scene");
    if (sceneElement) {
        daeElement *extraPropertiesElement = sceneElement->getChild("extra");
        if (extraPropertiesElement) {
            daeElement *techniquePropertiesElement = extraPropertiesElement->getChild("technique");
            if (techniquePropertiesElement) {
                daeElement *timelineElement = techniquePropertiesElement->getChild("timeline");
                if (timelineElement) {
                    QString frameString = QString::fromStdString(timelineElement->getAttribute("frame"));
                    QString frameInString = QString::fromStdString(timelineElement->getAttribute("frameIn"));
                    QString frameOutString = QString::fromStdString(timelineElement->getAttribute("frameOut"));
                    QString frameRangeInString = QString::fromStdString(timelineElement->getAttribute("frameRangeIn"));
                    QString frameRangeOutString = QString::fromStdString(timelineElement->getAttribute("frameRangeOut"));
                    QString fpsString = QString::fromStdString(timelineElement->getAttribute("fps"));
					setEndFrame(frameOutString.toInt());
                    setStartFrame(frameInString.toInt());
					setOutFrame(frameRangeOutString.toInt());
                    setInFrame(frameRangeInString.toInt());
					setCurrentFrame(frameString.toInt());
                    setFrameRate(fpsString.toInt());
                }
				daeElement *workingDirElement = techniquePropertiesElement->getChild("workingDirectory");
				if (workingDirElement) {

					QString workingDirString = QString::fromStdString(workingDirElement->getAttribute("path"));
					if( QDir().exists(workingDirString))
						setWorkingDirectory(workingDirString);
				}
            }
        }
    }

    // obtain scene library element
    daeElement *sceneLibraryElement = rootElement->getChild("library_visual_scenes");
    if (!sceneLibraryElement) {
        Log::error("The given COLLADA document does not contain a \"library_visual_scenes\" element.", "SceneModel::createScene");
        return;
    }

    // obtain visual scene element
    daeElement *visualSceneElement = sceneLibraryElement->getChild("visual_scene");
    if (!visualSceneElement) {
        Log::error("The given COLLADA document does not contain a \"visual_scene\" element.", "SceneModel::createScene");
        return;
    }

    // parse scene elements
	QList<QPair<QList<Node *>, QList<QPointF>>> groupList;
	QList<Node *> nodesToGroup;
	QList<QPointF> nodePositions;
    daeTArray<daeSmartRef<daeElement>> &sceneElements = visualSceneElement->getChildren();
    progressDialog.setMaximum((int) sceneElements.getCount());
    for (size_t i = 0; i < sceneElements.getCount(); ++i) {
        daeElement *sceneElement = sceneElements.get(i);
        if (QString(sceneElement->getTypeName()) == "node") {
			const bool isGrouped = (bool) QString::fromStdString(sceneElement->getAttribute("name")).toInt();
            progressDialog.setLabelText(QString(tr("Creating node \"%1\"...")).arg(sceneElement->getID()));
            daeElement *extraElement = sceneElement->getChild("extra");
            if (extraElement) {
                daeElement *techniqueElement = extraElement->getChild("technique");
                if (techniqueElement) {
					daeTArray<daeSmartRef<daeElement>> &nodeElements = techniqueElement->getChildren();
					nodesToGroup.clear();
					nodePositions.clear();
					for (size_t i = 0; i < nodeElements.getCount(); ++i) {
						daeElement *nodeElement = nodeElements.get(i);
						if ( (QString(nodeElement->getElementName()) == "nodeItem") || (QString(nodeElement->getElementName()) == "frapper_node") ) {
							QString &nodeName = QString::fromStdString(nodeElement->getAttribute("name"));
							const QString &typeName = QString::fromStdString(nodeElement->getAttribute("type"));
							const QPointF &position = decodePosition(nodeElement->getAttribute("position"), nodeName);
							const unsigned int stageIndex = QString::fromStdString(nodeElement->getAttribute("stageIndex")).toInt();
							const bool isViewed = (bool) QString::fromStdString(nodeElement->getAttribute("isViewed")).toInt();

							// for old style scene format
							if (nodeName.isEmpty())
								nodeName = QString::fromStdString(sceneElement->getAttribute("id"));


							QString objectName = "";
							// create a new scene object with the given type, name and position
							if (typeName != "Backdrop") {
								objectName = createObject(typeName, nodeName, position, false);
							}
							else {

								qreal width = 200.0;
								qreal height = 200.0;
								QColor bgColor = QColor(48, 48, 48); 
								QColor textColor = QColor(255, 255, 255); 
								QString head = "You can write a headline here...";
								QString text = "... and you can put a description here";
								int textSize = 11;
								int headSize = 22;
								bool lockChange = false;
								// parse parameters of node
								daeElement *parametersElement = nodeElement->getChild("parameters");
								// for old style scene format
								if (!parametersElement)
									parametersElement = nodeElement->getChild("frapper_parameters");
									
								if (parametersElement) {
									daeTArray<daeSmartRef<daeElement>> &parameterElements = parametersElement->getChildren();
									QList<QString> parameterList;
									for (size_t i = 0; i < parameterElements.getCount(); ++i) {

										daeElement *parameterElement = parameterElements.get(i);
										const QString &name = QString::fromStdString(parameterElement->getAttribute("name"));
										const QString &value = QString::fromStdString(parameterElement->getAttribute("value"));

										if(name == "Width") {
											width = QVariant(value).toReal();
										} else if(name == "Height") {
											height = QVariant(value).toReal();
										} else if(name == "Background Color") {
											QString separator;
											if (value.contains(", "))
												separator = ", ";
											else if (value.contains(" "))
												separator = " ";
											const QStringList colorStrings = value.split(separator);
											QColor color;
											if( colorStrings.size()>=1 ) color.setRed(   colorStrings[0].toInt());
											if( colorStrings.size()>=2 ) color.setGreen( colorStrings[1].toInt());
											if( colorStrings.size()>=3 ) color.setBlue(  colorStrings[2].toInt());
											if( colorStrings.size()>=4 ) color.setAlpha( colorStrings[3].toInt());
											bgColor = color;
										} else if(name == "Description") {
											text = QString(value);
										} else if(name == "Headline") {
											head = QString(value);
										} else if(name == "Description Size") {
											textSize = QVariant(value).toInt();
										} else if(name == "Headline Size") {
											headSize = QVariant(value).toInt();
										} else if(name == "Lock Changes") {
											lockChange = QVariant(value).toBool();
										} else if(name == "Text Color") {
											QString separator;
											if (value.contains(", "))
												separator = ", ";
											else if (value.contains(" "))
												separator = " ";
											const QStringList colorStrings = value.split(separator);
											QColor color;
											if( colorStrings.size()>=1 ) color.setRed(   colorStrings[0].toInt());
											if( colorStrings.size()>=2 ) color.setGreen( colorStrings[1].toInt());
											if( colorStrings.size()>=3 ) color.setBlue(  colorStrings[2].toInt());
											if( colorStrings.size()>=4 ) color.setAlpha( colorStrings[3].toInt());
											textColor = color;
										}
									}
								}
								backDrop(nodeName, position, width, height, bgColor, textColor, text, textSize, head, headSize, lockChange);
							}

							// check if the scene object was created successfully
							if (!objectName.isEmpty() && typeName != "Backdrop") {
								Node *node = m_nodeModel->getNode(objectName);
								if (node) {
									node->loadStarted();
									// set the stage index
									ViewNode *viewNode = dynamic_cast<ViewNode *>(node);
									if (viewNode) {
										if (stageIndex > 0) {
											viewNode->setStageIndex(stageIndex);
											viewNode->setView(isViewed);
										}
									}
									// parse parameters of node
									daeElement *parametersElement = nodeElement->getChild("parameters");
									// for old style scene format
									if (!parametersElement)
										parametersElement = nodeElement->getChild("frapper_parameters");

									if (parametersElement) {
										createParameters(node, node->getParameterRoot(), parametersElement);
									}
									// need to update the camera again so that viewing parameters can be applied
									if (node->getTypeName() == "Camera") {
										emit camerasUpdated(m_cameraNodes, node->getName());
									}

									if (isGrouped) {
										nodesToGroup.append(node);
										nodePositions.append(position);
									}
								} else
									Log::warning(QString("The node \"%1\" could not be obtained from the node model.").arg(objectName), "SceneModel::createScene");
							} else
								Log::warning(QString("The scene object \"%1\" could not be created.").arg(sceneElement->getID()), "SceneModel::createScene");
						} else
							Log::warning(QString("The node \"%1\" does not contain a \"node\" element.").arg(sceneElement->getID()), "SceneModel::createScene");
					} // end for nodeElements
					if (isGrouped)
						groupList.append(QPair<QList<Node *>, QList<QPointF>>(nodesToGroup, nodePositions));
                } else
                    Log::warning(QString("The node \"%1\" does not contain a \"technique\" element.").arg(sceneElement->getID()), "SceneModel::createScene");
            } else
                Log::warning(QString("The node \"%1\" does not contain an \"extra\" element.").arg(sceneElement->getID()), "SceneModel::createScene");
        }
        progressDialog.setValue((int) i);
	}
	emit loadSceneElementsReady();

    // parse connection elements
    daeElement *extraElement = visualSceneElement->getChild("extra");
    progressDialog.setLabelText(QString(tr("Creating connections...")));
    progressDialog.setValue(0);
    if (extraElement) {
        daeElement *techniqueElement = extraElement->getChild("technique");
        if (techniqueElement) {
            daeTArray<daeSmartRef<daeElement>> &connectionElements = techniqueElement->getChildren();
            progressDialog.setMaximum((int) connectionElements.getCount());
            for (size_t i = 0; i < connectionElements.getCount(); ++i) {
                daeElement *connectionElement = connectionElements.get(i);
				if ( QString(connectionElement->getElementName()).contains("connection") ) {
                    const QString &sourceNodeName = QString::fromStdString(connectionElement->getAttribute("sourceNode"));
                    const QString &sourceParameterName = QString::fromStdString(connectionElement->getAttribute("sourceParameter"));
                    const QString &targetNodeName = QString::fromStdString(connectionElement->getAttribute("targetNode"));
                    const QString &targetParameterName = QString::fromStdString(connectionElement->getAttribute("targetParameter"));
                    Node *sourceNode = m_nodeModel->getNode(sourceNodeName);
                    Node *targetNode = m_nodeModel->getNode(targetNodeName);
                    connectParameters(sourceNode, sourceParameterName, targetNode, targetParameterName);
                }
                progressDialog.setValue((int) i);
            }
        }
    }

    // iterate over the list of all graphics items contained in the scene
    QList<QGraphicsItem *> graphicsItems = m_graphicsItemMap.values();
    foreach (QGraphicsItem *item, graphicsItems) {
        // check if the current item is a node graphics item
        NodeGraphicsItem *nodeGraphicsItem = dynamic_cast<NodeGraphicsItem *>(item);
		NodeBackDropGraphicsItem *nodeBGGraphicsItem = dynamic_cast<NodeBackDropGraphicsItem *>(item);
        if (nodeGraphicsItem) {
            nodeGraphicsItem->refresh();
		} else if (nodeBGGraphicsItem) {
			nodeBGGraphicsItem->update();
		}
	}
	for (int i=0; i<groupList.size(); i++) {
		QPair<QList<Node *>, QList<QPointF>> listData = groupList.at(i);
		groupNodes(listData.first,  listData.second);
	}

    emit loadReady();
}

//!
//! Creates COLLADA elements corresponding to the parameters. 
//!
//! \param node The node to create COLLADA elements from.
//! \param parameterGroup The parameter group to create COLLADA elements from.
//! \param parametersElement The element under which to create the COLLADA node elements.
//!
void SceneModel::createParameters(Node *node, ParameterGroup *parentGroup, daeElement *parametersElement) const
{
    daeTArray<daeSmartRef<daeElement>> &parameterElements = parametersElement->getChildren();
	QList<QPair<QString, QString>> parameterList;
    for (size_t i = 0; i < parameterElements.getCount(); ++i) {
        daeElement *parameterElement = parameterElements.get(i);
		if (QString(parameterElement->getElementName()).contains("group") && node->isSaveable()) {
            const QString &parameterName = QString::fromStdString(parameterElement->getAttribute("name"));
            ParameterGroup *descentParameterGroup;
            if (parentGroup->containsGroup(parameterName))
                descentParameterGroup = parentGroup->getParameterGroup(parameterName);
            else {
                descentParameterGroup = new ParameterGroup(parameterName);
                parentGroup->addParameter(descentParameterGroup);
            }
            createParameters(node, descentParameterGroup, parameterElement);
        }
		else if (QString(parameterElement->getElementName()).contains("parameter")) {	
			// load the gui switchable types
			const QString &name = QString::fromStdString(parameterElement->getAttribute("name"));
			const QString &type = QString::fromStdString(parameterElement->getAttribute("type"));
			const QString &value = QString::fromStdString(parameterElement->getAttribute("value"));
			QByteArray changeFunction = parameterElement->getAttribute("chFunc").c_str();
			QByteArray procFunction = parameterElement->getAttribute("proFunc").c_str();
			QByteArray auxFunction = parameterElement->getAttribute("auxFunc").c_str();
            daeElement *keysElement = parameterElement->getChild("keys");

			Parameter* parameter = parentGroup->getParameter(name);

			if (!parameter ) {
				if( !type.isEmpty()) {
					// initialize and set the parameter and his values
					QDomDocument doc;
					QDomElement domElement = doc.createElement(parameterElement->getElementName());
					convertToQDom( parameterElement, domElement);
					parameter = Parameter::create(domElement);
					if( parameter )					
						parentGroup->addParameter(parameter);
					else
						continue;
				} else {
					// can't create parameter without type. Go on with the next parameter element
					continue;
				}
			}

			// from now on we need a valid parameter!
			if( parameter ) {

				// load the gui switchable types for static parameter
				const QString &selfEval = QString::fromStdString(parameterElement->getAttribute("selfEval"));
				const QString &pinType = QString::fromStdString(parameterElement->getAttribute("pin"));
				const QString &enabled = QString::fromStdString(parameterElement->getAttribute("enabled"));
				if (!selfEval.isEmpty())
					parameter->setSelfEvaluating(selfEval.toInt());
				if (!pinType.isEmpty())
					parameter->setPinType(static_cast<Parameter::PinType>(pinType.toInt()));

				if (enabled == "0" || enabled == "false")
					parentGroup->setParameterEnabled(name, false);
				else
					parentGroup->setParameterEnabled(name, true);
				// set the gui switchable types
				//if (!value.isEmpty())
				//	parentGroup->setValue(name, value); // call ParameterGroup::setValue() converts QString to value of right type
				if (!changeFunction.isEmpty()) {
					if (changeFunction == "0")
						changeFunction.clear();
					parameter->setChangeFunction(changeFunction);
				}
				if (!procFunction.isEmpty()) {
					if (procFunction == "0")
						procFunction.clear();
					parameter->setProcessingFunction(procFunction);
				}
				if (!auxFunction.isEmpty()) {
					if (auxFunction == "0")
						auxFunction.clear();
					parameter->setAuxProcessingFunction(auxFunction);
				}
				// store name of parameter for later execution of change function
				const Parameter::Type parameterType = parameter->getType();
				if (parameterType == Parameter::T_Filename ||
					parameterType == Parameter::T_Directory) {
					if (!value.isEmpty())
						parentGroup->setValue(name, value); //<Name,Value>
					callChangeFunction(parameter);
				}
				else
					parameterList.append(QPair<QString, QString>(name, value));
			}

			// add number parameters keys if exist
			if (keysElement) {
				NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(parameter);
				if (numberParameter) {
					const QString &timeScale = QString::fromStdString(parameterElement->getAttribute("tScale"));
					if (!timeScale.isEmpty())
						numberParameter->setTimeStepSize(timeScale.toFloat());
					daeTArray<daeSmartRef<daeElement>> &keyElements = keysElement->getChildren();
					if (!numberParameter->isEmpty())
						numberParameter->clearKeys();
					for (size_t j = 0; j < keyElements.getCount(); ++j) {
						daeElement *keyElement = keyElements.get(j);
						const Key::KeyType type = static_cast<Key::KeyType>(QString::fromStdString(keyElement->getAttribute("type")).toInt());
						const QString &keyIndex = QString::fromStdString(keyElement->getAttribute("index"));

						QVariant keyValue;

						// key is color, then separate string
						if(parameter->getType() == Parameter::T_Color) 
						{
							QString value = QString::fromStdString(keyElement->getAttribute("value"));
							QString separator;
							if (value.contains(", "))
								separator = ", ";
							else if (value.contains(" "))
								separator = " ";

							const QStringList colorStrings = value.split(separator);
							QColor color;
							if( colorStrings.size()>=1 ) color.setRed(   colorStrings[0].toInt());
							if( colorStrings.size()>=2 ) color.setGreen( colorStrings[1].toInt());
							if( colorStrings.size()>=3 ) color.setBlue(  colorStrings[2].toInt());
							if( colorStrings.size()>=4 ) color.setAlpha( colorStrings[3].toInt());
							keyValue = QVariant(color);
						}
						// or not then use whole string
						else 
						{
							keyValue = (QString::fromStdString(keyElement->getAttribute("value")).toFloat());
						}

						if (type == Key::KT_Bezier) {
							const QString &keyTIndex = QString::fromStdString(keyElement->getAttribute("tIndex"));
							const QString &keyTValue = QString::fromStdString(keyElement->getAttribute("tValue"));
							numberParameter->addKeyPresorted(Key(keyIndex.toFloat(), keyValue, keyTIndex.toFloat(), keyTValue.toFloat()));
						}
						else
							numberParameter->addKeyPresorted(Key(keyIndex.toFloat(), keyValue, type));
					}
				}
			}
		}
	}
	// set all other parameters values at the end
	for(QList<QPair<QString, QString>>::const_iterator iter=parameterList.constBegin(); 
		iter!=parameterList.constEnd(); 
		iter++) {
			if (!iter->second.isEmpty())
				parentGroup->setValue(iter->first, iter->second); //<Name,Value>
	}
	// call all other parameters change functions AFTER all parameters are created
	for(QList<QPair<QString, QString>>::const_iterator iter=parameterList.constBegin(); 
		iter!=parameterList.constEnd(); 
		iter++) {
			callChangeFunction(parentGroup->getParameter(iter->first));  //<Name,Value>
	}
}


//!
//! Calls the change function of the parameter.
//!
//! \param parameter A pointer to the parameter.
//!
void SceneModel::callChangeFunction(Parameter *parameter) const
{
	if( parameter ) {
		parameter->executeChange();
		//if (parameter->getType() == Parameter::PT_None)	// SEIM: to force triggering of processing function, if parameter is selfevaluating 
			parameter->propagateDirty();
	}
}


//!
//! Deletes the nodes graphics items.
//!
//! \param node The node which graphics items should be deleted.
//! 
void SceneModel::deleteNodeGraphicsItems(const QString &nodeName)
{
	QGraphicsItem *graphicsItem = getGraphicsItem(nodeName);
	NodeGraphicsItem *nodeGraphicsItem = dynamic_cast<NodeGraphicsItem*>(graphicsItem);
	NodeBackDropGraphicsItem *nodeBDGraphicsItem = dynamic_cast<NodeBackDropGraphicsItem*>(graphicsItem);
	if (nodeGraphicsItem) {
		const QList<ConnectionGraphicsItem *> &connectionItems = nodeGraphicsItem->getConnectionItemList();
		foreach (ConnectionGraphicsItem *connectionGraphicsItem, connectionItems) {
			// remove the node graphics item from the graphics scene
			if (m_graphicsScene)
				m_graphicsScene->removeItem(connectionGraphicsItem);

			// remove the connection graphics item from the connected node graphics items
			NodeGraphicsItem *startNodeItem = connectionGraphicsItem->getStartNodeItem();
			if (startNodeItem)
				startNodeItem->removeConnectionItem(connectionGraphicsItem);

			// remove the node graphics item from the graphics scene
			NodeGraphicsItem *endNodeItem = connectionGraphicsItem->getEndNodeItem();
			if (endNodeItem)
				endNodeItem->removeConnectionItem(connectionGraphicsItem);

			m_connectionGraphicsItemMap.remove(connectionGraphicsItem->getName());

			// delete the graphics item and remove it from the item map
			connectionGraphicsItem->blockSignals(true);
			delete connectionGraphicsItem;
		}
		// delete the graphics item and remove it from the item map
		m_graphicsItemMap.remove(nodeName);
		// remove the node graphics item from the graphics scene
		if (m_graphicsScene)
			m_graphicsScene->removeItem(nodeGraphicsItem);
		nodeGraphicsItem->deleteConnectionsWithItem(false);
		delete nodeGraphicsItem;
		nodeGraphicsItem = 0;
	}
	// delete node backdrop graphics item
	else if(nodeBDGraphicsItem) {
		// remove the node graphics item from the graphics scene
		if (m_graphicsScene)
			m_graphicsScene->removeItem(nodeBDGraphicsItem);
		// delete the graphics item and remove it from the item map
		m_graphicsItemMap.remove(nodeBDGraphicsItem->getName());
		delete nodeBDGraphicsItem;
		nodeBDGraphicsItem = 0;
	}
}

//!
//! Creates a scene object of the given type with the given name with a
//! node graphics item at the given location and optionally selects it.
//!
//! \param typeName The name of the type of object to create.
//! \param name The name to give to the object to add to the model.
//! \param position The position for the object's node graphics item.
//! \param select Flag to control whether to select the new node.
//! \param visible Flag to control whether the object should be visible in the scene model.
//! \return The name of the created object, or an empty string if the object could not be created.
//!
QString SceneModel::createObject ( const QString &typeName, const QString &name /* = "" */, const QPointF &position /* = QPointF(-1, -1) */, bool select /* = false */, bool visible /* = true */ )
{
    QString nodeName (name);
    if (nodeName == "") {
        // find a name for the node
        QStringList nodeNames = m_nodeModel->getNodeNames();
        nodeName = QString("%1%2").arg(typeName[0].toLower()).arg(typeName.mid(1));
        int index = 2;
        while (nodeNames.contains(nodeName))
            nodeName = QString("%1%2%3").arg(typeName[0].toLower()).arg(typeName.mid(1)).arg(index++);
    } 
	else {
		QStringList nodeNames = m_nodeModel->getNodeNames();
		//nodeName = QString("%1%2").arg(nodeName[0].toLower()).arg(nodeName.mid(1));
		nodeName = QString("%1%2").arg(nodeName[0]).arg(nodeName.mid(1));
		int index = 2;
		while (nodeNames.contains(nodeName))
			nodeName = QString("%1%2").arg(nodeName.left(nodeName.indexOf(QRegExp("[0-9]")))).arg(index++);
	}

        // create a node with the given type and name
        Node *node = m_nodeModel->createNode(typeName, nodeName, visible);
        if (!node)
            return QString("");

        // set up time dependencies for the node's parameters
        node->setUpTimeDependencies(m_frameParameter, m_frameRangeParameter );

		// node will be notified just before redraw
		connect(this, SIGNAL(triggerPreRedraw()), node, SLOT(onPreRedraw()));

        // render one frame connection
        connect(node, SIGNAL(updateFrame()), this, SLOT(updateFrame())); 

        // node will be notified when frame has changed
        connect(this, SIGNAL(currentFrameSet(int)), node, SIGNAL(frameChanged(int)));

        // create the delete object connection
        connect(node, SIGNAL(sendDeleteConnection(Connection *)), SLOT(connectionDestroyed(Connection *)));

		// create the delete object connection
		connect(node, SIGNAL(requestCreateConnection(QString, QString, QString, QString)), SLOT(connectParameters(QString, QString, QString, QString)));

        // create the update fake connection select->deselect node
		connect(node, SIGNAL(selectDeselectObject(const QString &)), SLOT(selectDeselectObject(const QString &)));

        // create the loading ready connection to nodes
        connect(this, SIGNAL(loadReady()), node, SLOT(loadReady()));
		connect(this, SIGNAL(loadSceneElementsReady()), node, SLOT(loadSceneElementsReady()));
		connect(this, SIGNAL(saveStarted()), node, SLOT(saveStarted()));
        connect(this, SIGNAL(saveFinished()), node, SLOT(saveFinished()));

        // check if the created node is a camera node
        // NILZ: TODO: use dynamic cast instead -> introduce cameraNode interface class
        if (node->getTypeName() == "Camera") {
            m_cameraNodes.append(node);
            emit camerasUpdated(m_cameraNodes, node->getName());
        }

        // check if the created node is a view node
        ViewNode *viewNode = dynamic_cast<ViewNode *>(node);
        if (viewNode) {
            // set up connections for the view node
            connect(viewNode, SIGNAL(viewSet(unsigned int, ViewNode *)), SLOT(setViewNode(unsigned int, ViewNode *)));
            connect(viewNode, SIGNAL(triggerRedraw()), SLOT(redrawTriggered()));
            connect(viewNode, SIGNAL(viewNodeUpdated()), SLOT(rebuildScene()));
        }

		// check if the created node is a geometry render node
		RenderNode *renderNode = dynamic_cast<RenderNode *>(node);
		if (renderNode) {
			connect(this, SIGNAL(rebuildRendertargets()), renderNode, SLOT(rebuildRendertargets()));
		}

        // check if the object should be visible in the scene model
        if (visible) {
            // create graphics item as a graphical representation for the node
            QPointF nodePosition (position);
            if (nodePosition == QPointF(-1, -1))
                if (!m_newNodePosition.isNull())
                    nodePosition = m_newNodePosition;
                else
                    nodePosition = QPointF(m_newNodeIndex * 20, m_newNodeIndex * 20);

			NodeGraphicsItem *graphicsItem = new NodeGraphicsItem(node, nodePosition, NodeFactory::getColor(typeName));
			connect(graphicsItem, SIGNAL(connectionRequested(Parameter *, Parameter *)), SLOT(connectParameters(Parameter *, Parameter *)));
			connect(graphicsItem, SIGNAL(connectionRequested(Node *, Node *)), SLOT(connectParameters(Node *, Node *)));
			connect(node, SIGNAL(nodeNameChanged(const QString &)), SLOT(changeNodeName(const QString &)));
			if (m_graphicsScene)
				m_graphicsScene->addItem(graphicsItem);
			m_graphicsItemMap.insert(node->getName(), graphicsItem);



            // keep track of the index for creating new nodes
            ++m_newNodeIndex;
        }

        // select the object if requested by parameter
        if (select)
            selectObject(node->getName());

        // notify connected objects that an object has been created
        emit objectCreated(node->getName());

        Log::info(QString("Object created: \"%1\"").arg(node->getName()), "SceneModel::createObject");

        return node->getName();
}


//!
//! Creates a group node item out of the selected nodes.
//!
void SceneModel::groupSelected ()
{
	const QStringList &nodeNames = getSelectedObjects();

	// check if more than one object is selected
	if (nodeNames.size() > 1) {
		QList<Node *> nodeList;
		QList<QPointF> posList;
		Node *selectedNode = 0;

		foreach(const QString &objectName, nodeNames) {
			selectedNode = m_nodeModel->getNode(objectName);
			if (selectedNode) {
				QGraphicsItem *nodeItem = getGraphicsItem(objectName);
				NodeGroupGraphicsItem *groupItem = dynamic_cast<NodeGroupGraphicsItem *>(nodeItem);
				if (nodeItem && !groupItem) {
					const QPointF &pos = nodeItem->pos();
					Node* node = m_nodeModel->getNode(objectName);
					nodeList.append(node);
					posList.append(pos);
				}
			}
		}
		groupNodes(nodeList, posList);
	}
}

//!
//! Creates a group node item out of the selected nodes.
//!
void  SceneModel::backDrop (const QString name /* = "" */, const QPointF position /* = (QPointF(0.f, 0.f) */, const qreal width /* = 200 */, const qreal height /* = 200 */, const QColor backgroundColor /* = QColor(48,48,48) */, const QColor textColor /* = QColor::white */, const QString text /* = "" */, const int textSize /* = 11 */, const QString head /* = "" */, const int headSize /* = 22 */, const bool lockChange /* = false */ )
{

	QString nodeName = QString(name);
	QString typeName = "BackDrop";

	if (nodeName == "") {
		// find a name for the node
		QStringList nodeNames = m_nodeModel->getNodeNames();
		nodeName = QString("%1%2").arg(typeName[0].toLower()).arg(typeName.mid(1));
		int index = 2;
		while (nodeNames.contains(nodeName))
			nodeName = QString("%1%2%3").arg(typeName[0].toLower()).arg(typeName.mid(1)).arg(index++);
	} else {
		// check if a node of the given name already exists
		if (m_nodeModel->getNode(name)) {
			Log::error(QString("An object named \"%1\" already exists.").arg(name), "SceneModel::createObject");
			return;
		}
	}

	QPointF nodePosition = QPointF(position);
	if (nodePosition.isNull())
		nodePosition = QPointF(m_newNodeIndex * 20, m_newNodeIndex * 20);

	// create a node with the given type and name
	Node *node = m_nodeModel->createBackDrop(nodeName, nodePosition, width, height, backgroundColor, textColor, text, textSize, head, headSize, lockChange);

	// create the update fake connection select->deselect node
	connect(node, SIGNAL(selectDeselectObject(const QString &)), SLOT(selectDeselectObject(const QString &)));
	// create the loading ready connection to nodes
	connect(this, SIGNAL(loadReady()), node, SLOT(loadReady()));
	connect(this, SIGNAL(loadSceneElementsReady()), node, SLOT(loadSceneElementsReady()));
	connect(this, SIGNAL(saveStarted()), node, SLOT(saveStarted()));
	connect(this, SIGNAL(saveFinished()), node, SLOT(saveFinished()));

	NodeBackDropGraphicsItem *graphicsItem = new NodeBackDropGraphicsItem(node->getName(), node, nodePosition, width, height );
	connect(node, SIGNAL(nodeNameChanged(const QString &)), SLOT(changeNodeName(const QString &)));
	connect(node, SIGNAL(graphicDrawSignal()), graphicsItem, SLOT(redraw()));
	if (m_graphicsScene)
		m_graphicsScene->addItem(graphicsItem);
	m_graphicsItemMap.insert(node->getName(), graphicsItem);

	graphicsItem->redraw();

	// keep track of the index for creating new nodes
	++m_newNodeIndex;

	// select the object
	selectObject(node->getName());

	// notify connected objects that an object has been created
	emit objectCreated(node->getName());
}

//!
//! Creates a group node item out of nodes with the given name.
//!
//! \param nodeNames The list of node names that should be grouped.
//!
void SceneModel::groupNodes (const QList<Node *> &nodeList, const QList<QPointF> &posList)
{
	m_nodeModel->beginUpdate();

	// create nodegroup graphics item
	NodeGroupGraphicsItem *graphicsItem = 0;
	graphicsItem = new NodeGroupGraphicsItem("nodeGroup", nodeList, posList, QColor(48, 48, 48));

	// inform the node model that these nodes has been grouped
	m_nodeModel->groupNodes(graphicsItem->getName(), nodeList);

	connect(graphicsItem, SIGNAL(connectionRequested(Parameter *, Parameter *)), SLOT(connectParameters(Parameter *, Parameter *)));
	connect(graphicsItem, SIGNAL(connectionRequested(Node *, Node *)), SLOT(connectParameters(Node *, Node *)));

	if (m_graphicsScene) {
		const QString &groupName = graphicsItem->getName();
		m_graphicsScene->addItem(graphicsItem);
		m_graphicsItemMap.insert(groupName, graphicsItem);
	}

	deselectObject("");

	// reconnect graphics item
	if (graphicsItem) {
		// connect the object's graphics items with a new connection graphics item
		QList<Connection::ID> idList;
		foreach(const Node *node, nodeList) {
			// delete the nodes connection items
			deleteNodeGraphicsItems(node->getName());
			AbstractParameter::List &parameters = node->getParameterRoot()->getAllParameters();
			foreach(AbstractParameter *abstParameter, parameters) {
				Parameter *parameter = dynamic_cast<Parameter *>(abstParameter);
				if (parameter && parameter->isConnected()) {
					const Connection::Map &connectionMap = parameter->getConnectionMap();
					foreach(Connection *connection, connectionMap) {
						const Connection::ID connectionID = connection->getId();
						Node *sourceNode = connection->getSourceParameter()->getNode();
						Node *targetNode = connection->getTargetParameter()->getNode();
						if ( !idList.contains(connectionID) && (
							( !nodeList.contains(sourceNode) &&  nodeList.contains(targetNode) ) ||
							( !nodeList.contains(targetNode) &&  nodeList.contains(sourceNode) )) ) {
								createConnectionItem(connection);
								idList.append(connectionID);
						}
					}
				}
			}
		}
	}

	m_nodeModel->endUpdate();

	// keep track of the index for creating new nodes
	++m_newNodeIndex;
}


//!
//! Creates a list of nodes out of the selected group node.
//!
void SceneModel::ungroupSelected ()
{
	const QStringList &selectedObjects = getSelectedObjects();

	foreach(const QString &name, selectedObjects) {
		// create node graphics item
		NodeGroupGraphicsItem *nodeGroupItem = dynamic_cast<NodeGroupGraphicsItem *>(getGraphicsItem(name));

		if (nodeGroupItem) {
			const QString &groupName = nodeGroupItem->getName();
			const QList<Node *> &nodeList = m_nodeModel->getGroupedNodes(groupName);
			const QHash<QString, QPointF> nodePositions = nodeGroupItem->getNodeItemPositions();
			const QPointF &oldPos = nodeGroupItem->pos();
			foreach (Node* node, nodeList) {
				if (node) {
					const QString &type = node->getTypeName();
					const QString &name = node->getName();
					const QPointF pos = nodePositions.value(name) + oldPos;

					NodeGraphicsItem *graphicsItem = new NodeGraphicsItem(node, pos, NodeFactory::getColor(type));
					connect(graphicsItem, SIGNAL(connectionRequested(Parameter *, Parameter *)), SLOT(connectParameters(Parameter *, Parameter *)));
					connect(graphicsItem, SIGNAL(connectionRequested(Node *, Node *)), SLOT(connectParameters(Node *, Node *)));
					connect(node, SIGNAL(nodeNameChanged(const QString &)), SLOT(changeNodeName(const QString &)));
					if (m_graphicsScene)
						m_graphicsScene->addItem(graphicsItem);
					m_graphicsItemMap.insert(name, graphicsItem);

					// keep track of the index for creating new nodes
					++m_newNodeIndex;
				}
			}
			// delete all related node graphics items
			deleteNodeGraphicsItems(groupName);
			m_nodeModel->unGroupNodes(groupName);
		}
	}

	// connect the object's graphics items with a new connection graphics item
	QList<Connection::ID> idList;
	foreach(const QString &name, selectedObjects) {
		Node *node = m_nodeModel->getNode(name);
		AbstractParameter::List &parameters = node->getParameterRoot()->getAllParameters();
		foreach(AbstractParameter *abstParameter, parameters) {
			Parameter *parameter = dynamic_cast<Parameter *>(abstParameter);
			if (parameter && parameter->isConnected()) {
				const Connection::Map &connectionMap = parameter->getConnectionMap();
				foreach(Connection *connection, connectionMap) {
					const Connection::ID connectionID = connection->getId();
					if (!idList.contains(connectionID)) {
						createConnectionItem(connection);
						idList.append(connectionID);
					}
				}
			}
		}
		QStandardItem *item = m_nodeModel->getStandardItemNode(name);
		m_selectionModel->select(item->index(), QItemSelectionModel::Select);
	}
}


//!
//! Creates a connection between the parameters with the given names of the
//! given nodes.
//!
//! The parameters to be connected must have the same parameter type.
//!
//! \param sourceNode The source node for the new connection.
//! \param sourceParameterName The name of the output parameter of the source node.
//! \param targetNode The target node for the new connection.
//! \param targetParameterName The name of the input parameter of the target node.
//!
void SceneModel::connectParameters ( Node *sourceNode, const QString &sourceParameterName, Node *targetNode, const QString &targetParameterName, bool visible /*= true*/ )
{
    // make sure source and target nodes are valid
    if (!sourceNode) {
        Log::error("No source node provided.", "SceneModel::connectParameters");
        return;
    }
    if (!targetNode) {
        Log::error("No target node provided.", "SceneModel::connectParameters");
        return;
    }

    // obtain source and target parameters for the new connection
    Parameter *sourceParameter = sourceNode->getParameter(sourceParameterName);
    Parameter *targetParameter = targetNode->getParameter(targetParameterName);

    // make sure the source parameter is valid
    if (!sourceParameter)
        // check if the source node has an unknown node type
        if (sourceNode->isTypeUnknown()) {
            // use the target parameter's type for the source parameter if possible
            Parameter::Type sourceParameterType = Parameter::T_Unknown;
            if (targetParameter && targetParameter->getType() != Parameter::T_Unknown)
                sourceParameterType = targetParameter->getType();

            // create a dummy parameter and use it for creating a connection
            sourceParameter = Parameter::create(sourceParameterName, sourceParameterType);
            sourceParameter->setPinType(Parameter::PT_Output);
            sourceNode->getParameterRoot()->addParameter(sourceParameter);
        } else {
            Log::error(QString("Source parameter \"%1.%2\" not found.").arg(sourceNode->getName()).arg(sourceParameterName), "SceneModel::connectParameters");
            return;
        }
    else if (sourceParameter->getPinType() != Parameter::PT_Output) {
        Log::error(QString("Source parameter \"%1.%2\" is not an output parameter.").arg(sourceNode->getName()).arg(sourceParameterName), "SceneModel::connectParameters");
        return;
    }
	emit sourceParameter->connectionCreated(targetParameter);

    // make sure the target parameter is valid
    if (!targetParameter)
        // check if the target node has an unknown node type
        if (targetNode->isTypeUnknown()) {
            // use the source parameter's type for the target parameter if possible
            Parameter::Type targetParameterType = Parameter::T_Unknown;
            if (sourceParameter && sourceParameter->getType() != Parameter::T_Unknown)
                targetParameterType = sourceParameter->getType();

            // create a dummy parameter and use it for creating a connection
            targetParameter = Parameter::create(targetParameterName, targetParameterType);
            targetParameter->setPinType(Parameter::PT_Input);
            targetNode->getParameterRoot()->addParameter(targetParameter);
        } else {
            Log::error(QString("Target parameter \"%1.%2\" not found.").arg(targetNode->getName()).arg(targetParameterName), "SceneModel::connectParameters");
            return;
        }
    else if (targetParameter->getPinType() != Parameter::PT_Input) {
        Log::error(QString("Target parameter \"%1.%2\" is not an input parameter.").arg(targetNode->getName()).arg(targetParameterName), "SceneModel::connectParameters");
        return;
    }
	emit targetParameter->connectionCreated(sourceParameter);

    // make sure source and target parameters have a common type
    if (sourceParameter->getType() != targetParameter->getType()) {
        Log::error(QString("Source and target parameters have different types: \"%1.%2\" [%3] and \"%4.%5\" [%6].")
            .arg(sourceNode->getName(), sourceParameterName, Parameter::getTypeName(sourceParameter->getType()),
            targetNode->getName(), targetParameterName, Parameter::getTypeName(targetParameter->getType())), "SceneModel::connectParameters");
        return;
    }

    // make sure target parameters have correct multplicity
    if (targetParameter->getNumberOfConnections() > 0 && targetParameter->getMultiplicity() == targetParameter->getNumberOfConnections()) {
        Log::error(QString("Target parameter already has the maximum of %1 incoming connections [%2].")
            .arg(targetParameter->getName(), "SceneModel::connectParameters"));
        return;
    }

	// make sure target parameters have correct size
	if (sourceParameter->getSize() != targetParameter->getSize()) {
		Log::error(QString("Source and target parameters have different sizes: \"%1.%2\" [%3] and \"%4.%5\" [%6].")
						.arg(sourceNode->getName()).arg(sourceParameterName).arg(sourceParameter->getSize())
						.arg(targetNode->getName()).arg(targetParameterName).arg(targetParameter->getSize()), 
						"SceneModel::connectParameters");
		return;
    }

    // create a new connection connecting the given parameters
    Connection *connection = m_nodeModel->createConnection(sourceParameter, targetParameter);
	sourceParameter->addConnection(connection);
	targetParameter->addConnection(connection);

    // connect the object's graphics items with a new connection graphics item
    if (visible)
		createConnectionItem(connection);

	emit targetParameter->connectionEstablished(connection->getId());
	emit sourceParameter->connectionEstablished(connection->getId());
	targetParameter->propagateDirty();
}


//!
//! Connects all parameters with the same name of two nodes.
//!
//! The parameters to be connected must have the same parameter type.
//!
//! \param sourceNode The source node for the new connections.
//! \param targetNode The target node for the new connections.
//!
void SceneModel::connectParametersByName ( Node *sourceNode, Node *targetNode )
{
    // make sure source and target nodes are valid
    if (!sourceNode) {
        Log::error("No source node provided.", "SceneModel::connectParameters");
        return;
    }
    if (!targetNode) {
        Log::error("No target node provided.", "SceneModel::connectParameters");
        return;
    }

    // obtain lists of source and target parameters to connect
    Parameter::List sourceParameterList = sourceNode->getParameterRoot()->filterParameters("", true, true);
    Parameter::List targetParameterList = targetNode->getParameterRoot()->filterParameters("", true, true);

    // iterate over the list of source parameters
    foreach (const AbstractParameter *sourceParameter, sourceParameterList) {
        const QString &sourceName = sourceParameter->getName();
        // iterate over the list of target parameters
        foreach (const AbstractParameter *targetParameter, targetParameterList) {
            const QString &targetName = targetParameter->getName();
            // check if source and target parameter names match
            if (sourceName == targetName) {
                connectParameters(sourceNode, sourceName, targetNode, targetName);
                continue;
            }
        }
    }

    // iterate over the list of all graphics items contained in the scene
    QList<QGraphicsItem *> graphicsItems = m_graphicsItemMap.values();
    foreach (QGraphicsItem *item, graphicsItems) {
        // check if the current item is a node graphics item
        NodeGraphicsItem *nodeGraphicsItem = dynamic_cast<NodeGraphicsItem *>(item);
		NodeBackDropGraphicsItem *nodeBGGraphicsItem = dynamic_cast<NodeBackDropGraphicsItem *>(item);
        if (nodeGraphicsItem)
            nodeGraphicsItem->update();
		else if (nodeBGGraphicsItem)
			nodeBGGraphicsItem->update();
    }
}


//!
//! Creates a COLLADA element tree corresponding to the objects currently
//! contained in the scene.
//!
//! \param parentElement The element under which to create the COLLADA element tree representing the scene.
//!
void SceneModel::createDaeElements ( daeElement *parentElement )
{
    // make sure the given parent element is valid
    if (!parentElement) {
        Log::error("The given parent element is invalid.", "SceneModel::createDaeElements");
        return;
    }

    emit saveStarted();

	const QStringList &goupNames = m_nodeModel->getGroupNames();
	const QList<Node *> &ungroupedNodes = m_nodeModel->getUngroupedNodes();

    // iterate over the list of all gouped and ungrouped nodes contained in the scene model

	// ...for all ungrouped nodes
    foreach (Node *node, ungroupedNodes) {
		const QString &nodeName = node->getName();

		daeElement *nodeElement = parentElement->add("node");
		nodeElement->setAttribute("id", nodeName.toUtf8());
		nodeElement->setAttribute("name", "0");
		
		// create an extra element for the node element
        daeElement *techniqueElement = createDaeExtraElement(nodeElement);

		const QGraphicsItem *nodeItem = getGraphicsItem(nodeName);
		if (nodeItem)
			createDaeNodeElements(node, techniqueElement, nodeItem->pos());
		else
			Log::error(QString("The graphics item %1 does not exist.").arg(nodeName), "SceneModel::createDaeElements");
	}

	// ... and for all grouped nodes
	foreach (const QString &groupName, goupNames) {
		const QList<Node *> &groupedNodes = m_nodeModel->getGroupedNodes(groupName);
		
		daeElement *nodeElement = parentElement->add("node");
		nodeElement->setAttribute("id", groupName.toUtf8());
		nodeElement->setAttribute("name", "1");
		
		// create an extra element for the node element
        daeElement *techniqueElement = createDaeExtraElement(nodeElement);

		const NodeGroupGraphicsItem *groupItem = dynamic_cast<NodeGroupGraphicsItem *>((getGraphicsItem(groupName)));
		if (groupItem) {
			foreach (Node *node, groupedNodes) {
				const QPointF &position = groupItem->getNodeItemPosition(node->getName());
				createDaeNodeElements(node, techniqueElement, position);
			}
		}
	}

    // create an extra element for the connections elements
    daeElement *techniqueElement = createDaeExtraElement(parentElement);

    // iterate over the list of connections contained in the scene model
    const QList<Connection *> &connections = m_nodeModel->getConnections();
    foreach (Connection *connection, connections) {
        // get details of connection
        Parameter *sourceParameter = connection->getSourceParameter();
        Parameter *targetParameter = connection->getTargetParameter();
        QString sourceNodeName = sourceParameter->getNode()->getName();
        QString sourceParameterName = sourceParameter->getName();
        QString targetNodeName = targetParameter->getNode()->getName();
        QString targetParameterName = targetParameter->getName();

        // create DAE elements representing the connection
        daeElement *frapperConnectionElement = techniqueElement->add("connection");
        frapperConnectionElement->setAttribute("sourceNode", sourceNodeName.toUtf8());
        frapperConnectionElement->setAttribute("sourceParameter", sourceParameterName.toUtf8());
        frapperConnectionElement->setAttribute("targetNode", targetNodeName.toUtf8());
        frapperConnectionElement->setAttribute("targetParameter", targetParameterName.toUtf8());
    }

	emit saveFinished();
}

//!
//! Creates COLLADA extra elements corresponding to the given parent element. 
//!
//! \param parentElement The corresponding parent element.
//! \return The created, frapper specific extra element.
//!
daeElement *SceneModel::createDaeExtraElement (daeElement *parentElement) const
{
	daeElement *extraElement = parentElement->add("extra");
    daeElement *techniqueElement = extraElement->add("technique");
    techniqueElement->setAttribute("profile", "frapper");

	return techniqueElement;
}

//!
//! Converts a collade DOM- and all child elements into a Qt DOM element. 
//!
//! \param element The corresponding collada element.
//! \return The created, frapper specific Qt DOM element.
//!
void SceneModel::convertToQDom ( daeElement *parameterElement, QDomElement& domElement ) const
{
	if (QString(parameterElement->getElementName()).contains("parameter")) {
		const daeTArray<daeElement::attr> &attributes = parameterElement->getAttributes();
		for (size_t i=0; i<attributes.getCount(); i++) {
			const daeElement::attr &attribute = attributes[i];
			QString name = QString::fromStdString(attribute.name);
			if (name == "multi")
				name = "multiplicity";
			else if (name == "selfEval")
				name = "selfEvaluating";
			domElement.setAttribute(QString::fromStdString(attribute.name), QString::fromStdString(attribute.value));
		}
	}
}

//!
//! Creates COLLADA elements corresponding to the nodes oder node groups. 
//!
//! \param node The node to create COLLADA elements from.
//! \param parentElement The element under which to create the COLLADA node elements.
//!
void SceneModel::createDaeNodeElements (Node *node, daeElement *techniqueElement, const QPointF &position) const
{
	const QString nodePosition = QString("%1 %2").arg(position.x()).arg(position.y());
	
	daeElement *frapperNodeElement = techniqueElement->add("nodeItem");
	frapperNodeElement->setAttribute("name", node->getName().toUtf8());
	frapperNodeElement->setAttribute("type", node->getTypeName().toUtf8());
	frapperNodeElement->setAttribute("position", nodePosition.toUtf8());

	ViewNode *viewNode = dynamic_cast<ViewNode *>(node);
	if (viewNode) {
		unsigned int stageIndex = viewNode->getStageIndex();
		QString stageName = QString::number(stageIndex);
		frapperNodeElement->setAttribute("stageIndex", stageName.toUtf8());
		if (viewNode->isViewed())
			frapperNodeElement->setAttribute("isViewed", "1");
		else
			frapperNodeElement->setAttribute("isViewed", "0");
	}

	daeElement *frapperParametersElement = frapperNodeElement->add("parameters");

	// create DAE element representing the node's parameters
	createDaeElements(node->getParameterRoot(), frapperParametersElement);
}


//!
//! Creates a COLLADA elements representing global scene properties.
//!
//! \param parentElement The element under which to create the COLLADA element tree representing the scene.
//!
void SceneModel::createSceneDAEProperties ( daeElement *parentElement ) const
{
    // Add global scene parameters
    daeElement *techniqueElement = parentElement->add("technique");
    techniqueElement->setAttribute("profile", "frapper");
    daeElement *timelineElement = techniqueElement->add("timeline");
    timelineElement->setAttribute("frame", QString::number((int) m_frameParameter->getValue().toDouble()).toStdString().c_str());
    timelineElement->setAttribute("frameIn", QString::number((int) m_frameParameter->getMinValue().toDouble()).toStdString().c_str());
    timelineElement->setAttribute("frameOut", QString::number((int) m_frameParameter->getMaxValue().toDouble()).toStdString().c_str());
    timelineElement->setAttribute("frameRangeIn", QString::number((int) m_frameRangeParameter->getMinValue().toDouble()).toStdString().c_str());
    timelineElement->setAttribute("frameRangeOut", QString::number((int) m_frameRangeParameter->getMaxValue().toDouble()).toStdString().c_str());
    timelineElement->setAttribute("fps", QString::number((int) m_fpsParameter->getValue().toDouble()).toStdString().c_str());

	// Add Element for working directory with path attribute
	daeElement *workingDirElement = techniqueElement->add("workingDirectory");
	workingDirElement->setAttribute("path", getWorkingDirectory().toStdString().c_str());
}


//!
//! Deletes all objects contained in the scene.
//!
void SceneModel::clear ()
{
    // select all nodes in the node model without ever emitting the nodeSelected() signal
    m_nodeModel->blockSignals(true);
    selectAll();
    m_nodeModel->blockSignals(false);

    // delete all selected objects
    deleteSelected();

    // unview all view nodes
    QList<ViewNode *> viewNodeList;
    for (unsigned int i = 0; i < NUMBER_OF_STAGES; ++i) {
        ViewNode *currentViewNode = m_viewNodeList[i];
        viewNodeList.append(currentViewNode);
        if (currentViewNode) {
            currentViewNode->setView(false);
        }
    }

    m_newNodeIndex = 0;

    emit selectionChanged(false);
}

void SceneModel::emitRebuildRendertargets()
{
	emit rebuildRendertargets();
}


///
/// Public Slots
///


//!
//! Creates a connection between the given parameters.
//!
//! The parameters to be connected must have the same parameter type.
//!
//! \param sourceParameter The output parameter of the source node to connect.
//! \param targetParameter The input parameter of the target node to connect.
//!
void SceneModel::connectParameters ( Parameter *sourceParameter, Parameter *targetParameter )
{
    // make sure source and target parameters are valid
    if (!sourceParameter || !targetParameter) {
        Log::error("Source or target parameter invalid.", "SceneModel::connectParameters");
        return;
    }

    connectParameters(sourceParameter->getNode(), sourceParameter->getName(), targetParameter->getNode(), targetParameter->getName());
}

//!
//! Create connections between two nodes (connect by name).
//!
//! \param sourceNode The source node.
//! \param targetNode The target node.
//!
void SceneModel::connectParameters ( Node *sourceNode, Node *targetNode )
{
    // make sure source and target nodes are valid
    if (!sourceNode || !targetNode) {
        Log::error("Source or target node invalid.", "SceneModel::connectParameters");
        return;
    }

    connectParametersByName(sourceNode, targetNode);
}

//!
//! Creates a connection between the parameters given by name and node
//!
//! The parameters to be connected must have the same parameter type.
//!
//! \param sourceParameterNode The name of the source node.
//! \param sourceParameterName The name of the parameter of the source node to connect.
//! \param targetParameterNode The name of the target node.
//! \param targetParameterName The name of the parameter of the target node to connect.
//!
void SceneModel::connectParameters ( QString sourceNodeName, QString sourceParameterName, QString targetNodeName, QString targetParameterName )
{
	Node *sourceNode = m_nodeModel->getNode(sourceNodeName);
	Node *targetNode = m_nodeModel->getNode(targetNodeName);

	if( sourceNode && targetNode )
		connectParameters( sourceNode, sourceParameterName, targetNode, targetParameterName);
}

//!
//! Creates a new camera with parameters initialized according to the given
//! viewing parameters.
//!
//! \param viewingParameters A collection of parameters for viewing the 3D scene.
//!
void SceneModel::createCamera ( ViewingParameters *viewingParameters )
{
    if (!viewingParameters) {
        Log::error("Invalid viewing parameters given.", "SceneModel::createCamera");
        return;
    }

    QString cameraName = createObject("Camera", "", QPointF(), true);
    if (cameraName.isEmpty()) {
        Log::error("Camera could not be created.", "SceneModel::createCamera");
        return;
    }

    Node *cameraNode = m_nodeModel->getNode(cameraName);
    viewingParameters->applyTo(cameraNode);

    // notify connected objects that a camera has been created
    //emit cameraCreated(cameraNode->getName());
}


//!
//! Deletes an existing camera.
//!
//! \param cameraName The name of the camera to delete.
//!
void SceneModel::deleteCamera ( const QString &cameraName )
{
    //// delete the camera from the model
    //m_nodeModel->beginUpdate();
    //QGraphicsItem *graphicsItem = m_graphicsItemMap[cameraName];
    //if (graphicsItem) {
    //    // remove the graphics item from the graphics scene
    //    if (m_graphicsScene)
    //        m_graphicsScene->removeItem(graphicsItem);
    //    // delete the graphics item and remove it from the item map
    //    delete graphicsItem;
    //    m_graphicsItemMap.remove(cameraName);
    //}
    //m_nodeModel->deleteNode(cameraName);
    //m_nodeModel->endUpdate();

    //// notify connected objects that the camera has been deleted
    //Node *node = m_nodeModel->getNode(cameraName);
    //m_cameraNodes.removeAll(node);
    //emit cameraUpdated(m_cameraNodes, "");
    selectObject("");
    selectObject(cameraName);
    deleteSelected();
}


//!
//! Applies the given viewing parameters to the camera with the given name.
//!
//! \param cameraName The name of the camera to update.
//! \param viewingParameters The viewing parameters to apply to the camera with the given name.
//!
void SceneModel::updateCamera ( const QString &cameraName, ViewingParameters *viewingParameters )
{
    if (!viewingParameters) {
        Log::error("Invalid viewing parameters given.", "SceneModel::createCamera");
        return;
    }

    Node *cameraNode = m_nodeModel->getNode(cameraName);
    if (cameraNode) {
		viewingParameters->applyTo(cameraNode);
		redrawTriggered();
    }
}

//!
//! Sends the list of camereas when requested.
//!
void SceneModel::cameraListRequested()
{
    emit camerasUpdated(m_cameraNodes, "");
}


//!
//! Applies the viewing parameters of the camera with the given name to the
//! given viewing parameters.
//!
//! \param cameraName The name of the camera to use.
//! \param viewingParameters The viewing parameters to modify.
//!
void SceneModel::applyCamera ( const QString &cameraName, ViewingParameters *viewingParameters )
{
    if (!viewingParameters) {
        Log::error("Invalid viewing parameters given.", "SceneModel::applyCamera");
        return;
    }

    Node *cameraNode = m_nodeModel->getNode(cameraName);
    if (!cameraNode) {
        Log::error(QString("Could not obtain camera \"%1\".").arg(cameraName), "SceneModel::applyCamera");
        return;
    }

    viewingParameters->applyFrom(cameraNode);
}

//!
//! Deletes a given object.
//!
void SceneModel::deleteObject ( const QString &name ) {
    selectObject("");
    //selectObject(name);
    const QStandardItem *item = m_nodeModel->getStandardItemConnection(name);
    m_selectionModel->select(item->index(), QItemSelectionModel::Select);
    deleteSelected();
    selectObject("");
}

//!
//! Selects and deselects a given object.
//!
void SceneModel::selectDeselectObject ( const QString &name ) {

	if( getSelectedObjects().contains(name))
	{
		selectObject("");
		selectObject(name);
	}
}

//!
//! Deletes the currently selected objects from the scene.
//!
void SceneModel::deleteSelected ()
{
    // unview all view nodes
    QList<QString> viewNodeList;
    for (unsigned int i = 0; i < NUMBER_OF_STAGES; ++i) {
        ViewNode *currentViewNode = m_viewNodeList[i];
        if (currentViewNode) {
            viewNodeList.append(currentViewNode->getName());
            currentViewNode->setView(false);
        }
    }
	
	m_nodeModel->beginUpdate();

    QStringList nodeObjectsToDelete = getSelectedObjects();
    if (nodeObjectsToDelete.size() > 0) {
        // deselect all objects
        m_selectionModel->clear();
		 
        // display a progress dialog for deleting the selected objects
        QProgressDialog nodeProgressDialog (tr("Deleting objects..."), QString(), 0, nodeObjectsToDelete.size());
        nodeProgressDialog.setWindowTitle(tr("Delete"));
        nodeProgressDialog.setWindowModality(Qt::ApplicationModal);
        int progress = 0;

        // delete the previously selected objects from the model
        for (int i = 0; i < nodeObjectsToDelete.size(); ++i) {
            QString name = nodeObjectsToDelete.at(i);
            Node* node = m_nodeModel->getNode(name);

			if (!node)
				continue;

            // obtain the node graphics item from the node graphics item map
			QGraphicsItem *graphicsItem = getGraphicsItem(name);
			NodeGraphicsItem *nodeGraphicsItem = dynamic_cast<NodeGraphicsItem*>(graphicsItem);
			NodeBackDropGraphicsItem *nodeBGGraphicsItem = dynamic_cast<NodeBackDropGraphicsItem*>(graphicsItem);

			if (nodeGraphicsItem) {
				// remove the node graphics item from the graphics scene
				if (m_graphicsScene)
					m_graphicsScene->removeItem(nodeGraphicsItem);

				// if item is group, ungroup the nodes first
				NodeGroupGraphicsItem *nodeGroupItem = dynamic_cast<NodeGroupGraphicsItem *>(nodeGraphicsItem);
				if (nodeGroupItem) {
					const QString &itemName = nodeGroupItem->getName();
					m_nodeModel->unGroupNodes(itemName);
					m_graphicsItemMap.remove(itemName);
				}
				else {
					// delete the graphics item and remove it from the item map
					m_graphicsItemMap.remove(name);
				}
				delete nodeGraphicsItem;
				nodeGraphicsItem = 0;
			}

            // delete the node graphics item
            else if (nodeBGGraphicsItem) {
				// remove the node graphics item from the graphics scene
				if (m_graphicsScene)
					m_graphicsScene->removeItem(nodeBGGraphicsItem);

				// delete the graphics item and remove it from the item map
				m_graphicsItemMap.remove(nodeBGGraphicsItem->getName());
				delete nodeBGGraphicsItem;
				nodeBGGraphicsItem = 0;
            }

            // if deleted node is a camera node emit cameraDeleted signal
            // (needed for example to remove camera from drop down box in
            // ViewportPanel)
            //Node* node = m_nodeModel->getNode(name);
            if (node) {
                const QString &nodeTypeName = node->getTypeName();
                if (nodeTypeName == "Camera") {
                    m_cameraNodes.removeAll(node);
                    emit camerasUpdated(m_cameraNodes, "");
                }
            }

            // delete the node from the node model
            m_nodeModel->deleteNode(name);

            ++progress;
            nodeProgressDialog.setValue(progress);
        }
    }

    QStringList connectionObjectsToDelete = getSelectedObjects(true);
    if (connectionObjectsToDelete.size() > 0) {
        // deselect all objects
        m_selectionModel->clear();

        // display a progress dialog for deleting the selected objects
        QProgressDialog connectionProgressDialog (tr("Deleting connections..."), QString(), 0, connectionObjectsToDelete.size());
        connectionProgressDialog.setWindowTitle(tr("Delete"));
        connectionProgressDialog.setWindowModality(Qt::ApplicationModal);
        int progress = 0;

        // delete the previously selected objects from the model
        for (int i = 0; i < connectionObjectsToDelete.size(); ++i) {
            const QString &name = connectionObjectsToDelete.at(i);
            // delete connection
            deleteConnection(name, true);
            ++progress;
            connectionProgressDialog.setValue(progress);
        }
    }

	m_nodeModel->endUpdate();

    if (nodeObjectsToDelete.size() == 0 && connectionObjectsToDelete.size() == 0) {
        Log::debug("Nothing to delete has been selected.", "SceneModel::deleteSelected");
    }

    // view all view nodes
    for (int i = 0; i < viewNodeList.size(); ++i) {
        ViewNode *viewNode = dynamic_cast<ViewNode*>(m_nodeModel->getNode(viewNodeList[i]));
        if (viewNode)
            viewNode->setView(true);
    }
}


//!
//! Selects all objects in the scene.
//!
void SceneModel::selectAll ()
{
    // block nodeSelected() signals from the node model
    m_nodeModel->beginSelectingAll();

    QItemSelection selection;
    for (int i = 0; i < m_nodeModel->invisibleRootItem()->rowCount(); ++i) {
        QStandardItem *categoryItem = m_nodeModel->invisibleRootItem()->child(i);
        if (categoryItem->hasChildren()) {
            QModelIndex firstChildIndex = categoryItem->child(0)->index();
            QModelIndex lastChildIndex = categoryItem->child(categoryItem->rowCount() - 1)->index();
            selection << QItemSelectionRange(firstChildIndex, lastChildIndex);
        }
    }
    m_selectionModel->select(selection, QItemSelectionModel::Select);

    // unblock nodeSelected() signals from the node model
    m_nodeModel->endSelectingAll();
}


//!
//! Sets the evaluation flag of the selected objects to True.
//!
void SceneModel::evaluateSelectedObjects ()
{
    QStringList selectedObjects = getSelectedObjects();

    // display a progress dialog for evaluating the selected objects
    QProgressDialog progressDialog (tr("Evaluating objects..."), QString(), 0, selectedObjects.size());
    progressDialog.setWindowTitle(tr("Evaluate"));
    progressDialog.setWindowModality(Qt::ApplicationModal);
    int progress = 0;

    // delete the previously selected objects from the model
    m_nodeModel->beginUpdate();
    for (int i = 0; i < selectedObjects.size(); ++i) {
        QString name = selectedObjects.at(i);
        Node *node = m_nodeModel->getNode(name);
        if (node)
            node->setEvaluate(true);

        ++progress;
        progressDialog.setValue(progress);
    }
    m_nodeModel->endUpdate();
}


//!
//! Sets the evaluation flag of the selected objects to False.
//!
void SceneModel::ignoreSelectedObjects ()
{
    QStringList selectedObjects = getSelectedObjects();

    // display a progress dialog for ignoring the selected objects
    QProgressDialog progressDialog (tr("Ignoring objects..."), QString(), 0, selectedObjects.size());
    progressDialog.setWindowTitle(tr("Ignore"));
    progressDialog.setWindowModality(Qt::ApplicationModal);
    int progress = 0;

    // delete the previously selected objects from the model
    m_nodeModel->beginUpdate();
    for (int i = 0; i < selectedObjects.size(); ++i) {
        QString name = selectedObjects.at(i);
        Node *node = m_nodeModel->getNode(name);
        if (node)
            node->setEvaluate(false);

        ++progress;
        progressDialog.setValue(progress);
    }
    m_nodeModel->endUpdate();
}


//!
//! Updates the selection model according to the currently selected graphics
//! items in the graphics scene.
//!
//! Is called when the selection in the graphics scene has changed.
//!
void SceneModel::graphicsSceneSelectionChanged ()
{
    if (!m_graphicsScene)
        return;

    QItemSelection selected;
    QItemSelection deselected;


	// build list of items to select and items to deselect according to the
	// selected graphics items in the graphics scene
	const QList<QGraphicsItem *> &allGraphicsItems = m_graphicsScene->items();
	const QList<QGraphicsItem *> &selectedGraphicsItems = m_graphicsScene->selectedItems();
	NodeGraphicsItem *nodeGraphicsItem = 0;
	NodeGroupGraphicsItem *nodeGroupGraphicsItem = 0;
	ConnectionGraphicsItem *connectionGraphicsItem = 0;
	NodeBackDropGraphicsItem *backdropGraphicsItem = 0;
	QList<QStandardItem *> items;

	for (int i = 0; i < allGraphicsItems.size(); ++i) {
		nodeGraphicsItem = 0;
		nodeGroupGraphicsItem = 0;
		connectionGraphicsItem = 0;
		items.clear();
		if (nodeGroupGraphicsItem = dynamic_cast<NodeGroupGraphicsItem *>(allGraphicsItems.at(i))) {
			const QList<Node *> &nodes = nodeGroupGraphicsItem->getNodes();
			foreach(const Node *node, nodes)
				items.append(m_nodeModel->getStandardItemNode(node->getName()));
		}
		else if (nodeGraphicsItem = dynamic_cast<NodeGraphicsItem *>(allGraphicsItems.at(i)))
			items.append(m_nodeModel->getStandardItemNode(nodeGraphicsItem->getNode()->getName()));
		else if (connectionGraphicsItem = dynamic_cast<ConnectionGraphicsItem *>(allGraphicsItems.at(i)))
			items.append(m_nodeModel->getStandardItemConnection(QString::number(connectionGraphicsItem->getConnection()->getId())));
		else if (backdropGraphicsItem = dynamic_cast<NodeBackDropGraphicsItem *>(allGraphicsItems.at(i)))
			items.append(m_nodeModel->getStandardItemNode(backdropGraphicsItem->getNode()->getName()));
		foreach (const QStandardItem *item, items)
			if (item) {
				if (selectedGraphicsItems.contains(allGraphicsItems.at(i)))
					selected.select(item->index(), item->index());
				else
					deselected.select(item->index(), item->index());
			}
	}

    // update the selection in the selection model
    m_selectionModel->select(deselected, QItemSelectionModel::Deselect);
    m_selectionModel->select(selected, QItemSelectionModel::Select);
}

//!
//! Updates the m_graphicsItemList in case a node has changed its name. 
//!
void SceneModel::changeNodeName (const QString &name)
{
    Node *node = dynamic_cast<Node *>(sender());
    if (!node)
        return;
	NodeGraphicsItem *graphicsItem = dynamic_cast<NodeGraphicsItem*>(getGraphicsItem(name));
	NodeBackDropGraphicsItem *nodeBGGraphicsItem = dynamic_cast<NodeBackDropGraphicsItem*>(getGraphicsItem(name));
    if (graphicsItem) {
		m_graphicsItemMap.remove(graphicsItem->getName());
        m_graphicsItemMap[node->getName()] = graphicsItem;
    } else if (nodeBGGraphicsItem) {
		m_graphicsItemMap.remove(nodeBGGraphicsItem->getName());
		m_graphicsItemMap[node->getName()] = nodeBGGraphicsItem;
	}
}

//!
//! Selects the object with the given name or toggles the selection state
//! of the object with the given name.
//!
//! \param name The name of the object to select.
//! \param toggle Flag that controls whether to toggle the selection of the object with the given name.
//!
void SceneModel::selectObject ( const QString &name, bool toggle /*= false*/ )
{
    if (name == "")
        m_selectionModel->clear();
    else {
        Node *node = m_nodeModel->getNode(name);
        QItemSelectionModel::SelectionFlags command;
        if (toggle)
            command = QItemSelectionModel::Toggle;
        else if (m_selectionModel->hasSelection())
            command = QItemSelectionModel::ClearAndSelect;
        else
            command = QItemSelectionModel::Select;
        if (node) {            
            QStandardItem *item = m_nodeModel->getStandardItemNode(name);
            m_selectionModel->select(item->index(), command);
        }
        // NILZ: TODO: Better create a getConnection() function in NodeModel.
        else {
            QStandardItem *item = m_nodeModel->getStandardItemConnection(name);
            if (item)
                m_selectionModel->select(item->index(), command);
        }
        //QStringList selectedObjects = getSelectedObjects();
        //if (selectedObjects.size() > 0) {
        //    QStandardItem *item = m_nodeModel->getStandardItemNode(selectedObjects[0]);
        //    m_selectionModel->select(item->index(), QItemSelectionModel::Select);
        //}
    }
}


//!
//! Deselects the object with the given name.
//!
//! \param name The name of the object to deselect.
//!
void SceneModel::deselectObject ( const QString &name /* = "" */ )
{
    if (name == "")
        m_selectionModel->clear();
    else {
        Node *node = m_nodeModel->getNode(name);
        if (node) {
            QStandardItem *item = m_nodeModel->getStandardItemNode(name);
            m_selectionModel->select(item->index(), QItemSelectionModel::Deselect);
        }
    }
}


//!
//! Sets the index of the current frame in the scene's time.
//!
//! \param index The new index of the current frame in the scene's time.
//!
inline void SceneModel::setCurrentFrame ( int index, bool realtime /* = false */ )
{
    if (!realtime && index == m_frameParameter->getValue().toInt())
        return;

    m_frameParameter->setValue(index);
    m_frameParameter->propagateDirty();

    m_frameParameter->setDirty(false);

    updateActions();
    emit currentFrameSet(index);
	redrawTriggered();
}


//!
//! Triggers set frame one time.
//!
void SceneModel::updateFrame ()
{
    setCurrentFrame(getCurrentFrame(), true);
}


//!
//! Sets the index of the start frame in the scene's time.
//!
//! \param index The new index of the start frame in the scene's time.
//!
void SceneModel::setStartFrame ( int index )
{
    if (index == m_frameParameter->getMinValue().toInt())
        return;

    m_frameParameter->setMinValue(index);
    updateActions();
    emit startFrameSet(index);
}


//!
//! Sets the index of the end frame in the scene's time.
//!
//! \param index The new index of the end frame in the scene's time.
//!
void SceneModel::setEndFrame ( int index )
{
    if (index == m_frameParameter->getMaxValue().toInt())
        return;

    m_frameParameter->setMaxValue(index);
    updateActions();
    emit endFrameSet(index);
}


//!
//! Sets the index of the in frame in the scene's time.
//!
//! \param index The new index of the in frame in the scene's time.
//!
void SceneModel::setInFrame ( int index )
{
    if (index == m_frameRangeParameter->getMinValue().toInt())
        return;

    m_frameRangeParameter->setMinValue(index);

    if (m_frameParameter->getValue().toInt() < index)
        setCurrentFrame(index);
    else
        updateActions();

    emit inFrameSet(index);
}


//!
//! Sets the index of the out frame in the scene's time.
//!
//! \param index The new index of the out frame in the scene's time.
//!
void SceneModel::setOutFrame ( int index )
{
    if (index == m_frameRangeParameter->getMaxValue().toInt())
        return;

    m_frameRangeParameter->setMaxValue(index);

    if (m_frameParameter->getValue().toInt() > index)
        setCurrentFrame(index);
    else
        updateActions();

    emit outFrameSet(index);
}


//!
//! Sets the frame step to use for the scene.
//!
//! \param frameStep The frame step to use for the scene.
//!
void SceneModel::setFrameStep ( int frameStep )
{
    m_frameStep = frameStep;

    emit frameStepSet(frameStep);
}


//!
//! Sets the frame rate to use for the scene.
//!
//! \param frameRate The frame rate to use for the scene.
//!
void SceneModel::setFrameRate ( int frameRate )
{
    if (frameRate == m_fpsParameter->getValue().toInt())
        return;

    m_fpsParameter->setValue(frameRate);

    // check if the animation is currently played back
    if (m_timerId) {
        // stop the timer and restart it with the new frame rate
        killTimer(m_timerId);
        m_timerId = startTimer(1000.0 / frameRate);
    }

    emit frameRateSet(frameRate);
}


//!
//! Adjusts the given viewing parameters so that the currently selected
//! objects are centered in the view.
//!
//! All objects will be framed if no objects are currently selected.
//!
//! \param viewingParameters The viewing parameters to modify.
//!
void SceneModel::frameSelectedObjects ( ViewingParameters *viewingParameters )
{
    //QStringList objectsToFrame = getSelectedObjects();
    //if (objectsToFrame.size() == 0) {
    //    // frame the currently viewed node
    //    NullNode *nullNode = dynamic_cast<NullNode *>(m_viewNode);
    //    if (nullNode) {
    //        Ogre::Vector3 translation = nullNode->getTranslation();
    //        center->x = translation.x;
    //        center->y = translation.y;
    //        center->z = translation.z;
    //        *viewRadius = 2 * nullNode->getBoundingSphereRadius();
    //    }
    //    return;
    //}

    //// calculate the combined center point and obtain bounding radii
    //Ogre::Vector3 combinedCenter = Ogre::Vector3(0, 0, 0);
    //QList<Ogre::Vector3> objectPositions;
    //QList<double> boundingRadii;
    //for (int i = 0; i < objectsToFrame.size(); ++i) {
    //    QString name = objectsToFrame.at(i);
    //    Node *node = m_nodeModel->getNode(name);
    //    NullNode *nullNode = dynamic_cast<NullNode *>(node);
    //    if (nullNode) {
    //        Ogre::Vector3 translation = nullNode->getTranslation();
    //        objectPositions << translation;
    //        combinedCenter += translation;
    //        boundingRadii << nullNode->getBoundingSphereRadius();
    //    }
    //}
    //combinedCenter /= objectsToFrame.size();
    //center->x = combinedCenter.x;
    //center->y = combinedCenter.y;
    //center->z = combinedCenter.z;

    //// calculate the view radius according to the bounding radii obtained
    //*viewRadius = 0;
    //double maxDifference = 0;
    //for (int i = 0; i < objectPositions.size(); ++i) {
    //    double difference = (objectPositions[i] - combinedCenter).length();
    //    if (difference >= maxDifference) {
    //        double radius = difference + 2 * boundingRadii[i];
    //        if (radius > *viewRadius)
    //            *viewRadius = radius;
    //        maxDifference = difference;
    //    }
    //}
}


//!
//! Sets the node that currently has the view flag set.
//!
//! \param stageIndex The index of the stage for which to set the view node (currently not used).
//! \param viewNode The new node that is currently viewed.
//!
void SceneModel::setViewNode ( unsigned int stageIndex, ViewNode *viewNode )
{
    if (m_viewNodeList.size() < stageIndex)
        return;

    ViewNode *&currentViewNode = m_viewNodeList[stageIndex-1];
    if (viewNode != currentViewNode) {
        if (currentViewNode && viewNode && currentViewNode->getStageIndex() == viewNode->getStageIndex()) {
            currentViewNode->setView(false);
        }

        if (!viewNode)
            // notify connected objects that no image is viewed anymore
            emit imageSet(stageIndex, Ogre::TexturePtr());

        currentViewNode = viewNode;

        // build the scene
        rebuildScene();
    }
}


//!
//! Obtains the OGRE scene node from the current view node and adds it as a
//! child to the scene root scene node.
//!
void SceneModel::rebuildScene ()
{
    if (m_sceneRoot)
        // clear the scene
        m_sceneRoot->removeAllChildren();

    for (unsigned int i = 0; i < NUMBER_OF_STAGES; ++i) {
        ViewNode *currentViewNode = m_viewNodeList[i];
        Ogre::TexturePtr image;
        if (currentViewNode) {
            // obtain the scene node and image from the current view node
            Ogre::SceneNode *sceneNode = currentViewNode->getSceneNode();
			image = currentViewNode->getImage();

            // process scene node
            if (image.isNull() && sceneNode) {
                // remove the scene node from its parent scene node (if exists)
                Ogre::SceneNode *parentSceneNode = sceneNode->getParentSceneNode();
                if (parentSceneNode)
                    parentSceneNode->removeChild(sceneNode);

                if (m_sceneRoot)
                    // add the scene node as a child to the scene root scene node
                    m_sceneRoot->addChild(sceneNode);
            }
        }
        // notify connected objects that the image to be viewed has changed
        emit imageSet(i+1, image);
    }
    // notify connected objects that the scene has been modified
    // emit modified(); disabled because of file open bug !!!! seim !!!!
}


//!
//! Redraws the OGRE scene when active view node exists.
//!
void SceneModel::redrawTriggered ()
{
	emit triggerPreRedraw();

    bool activeViewNode = 0;
    for (unsigned int i = 0; i < NUMBER_OF_STAGES; ++i) {
        ViewNode *viewNode = this->m_viewNodeList[i];
		activeViewNode = activeViewNode || ((viewNode != 0));
        if (viewNode) {
            viewNode->updateImage();
        }
    }
    if (activeViewNode) {
        emit triggerRedraw();
    }
}


//!
//! Sets the scene coordinates to use when creating a new node.
//!
//! \param scenePosition The position in scene coordinates to use for a new node.
//!
void SceneModel::setNewNodePosition ( const QPointF &scenePosition )
{
    m_newNodePosition = scenePosition;
}


//!
//! Adds a keyframe for the given number parameter at the current frame
//! index.
//!
//! \param numberParameter The number parameter to add a keyframe for.
//!
void SceneModel::addKey ( NumberParameter *numberParameter )
{
    numberParameter->addKey(m_frameParameter->getValue().toInt());
    m_frameParameter->addAffectedParameter(numberParameter);
}


//!
//! Gives the node with the given old name the given new name.
//!
//! \param oldName The old name of the scene object.
//! \param newName The new name of the scene object.
//!
void SceneModel::renameNode ( const QString &oldName, const QString &newName )
{
	QHash<QString, QGraphicsItem *>::iterator itemIter = m_graphicsItemMap.find(oldName);
	if (itemIter != m_graphicsItemMap.end()) {
		QGraphicsItem *itemCopy = itemIter.value();
		m_graphicsItemMap.remove(oldName);
		m_graphicsItemMap.insert(newName, itemCopy);
	}
}


///
/// Protected Events
///


//!
//! Handles timer events for the scene model.
//!
//! \param event The description of the timer event.
//!
void SceneModel::timerEvent ( QTimerEvent *event )
{   
	// increment the index of the current frame by the currently set frame step value
	int currentFrame = m_frameParameter->getValue().toInt();

	// check if realtime mode is activated and if, just set the same frame
	bool realtime = m_realtimeAction->isChecked();
	if (realtime) {
		setCurrentFrame(currentFrame, true);
		return;
	}
	currentFrame += m_frameStep;

	// get the index of the last frame in the animation
	int lastFrame = m_frameRangeParameter->getMaxValue().toInt();

	// check if the last frame in the animation has been reached
	if (currentFrame > lastFrame && !m_playOnceAction->isChecked())
		// rewind the animation
		m_inFrameAction->trigger();
	else {
		if (currentFrame <= lastFrame)
			setCurrentFrame(currentFrame);
		// check if the last frame in the animation has been reached
		if (currentFrame >= lastFrame && m_playOnceAction->isChecked())
			// stop the animation
			m_playAction->toggle();
	}
}


///
/// Private Slots
///


//!
//! Updates the scene model according to the given item selections.
//!
//! Is called when the selection in the selection model has changed.
//!
//! \param selected The list of selected items.
//! \param deselected The list of deselected items.
//!
void SceneModel::selectionModelSelectionChanged ( const QItemSelection &selected, const QItemSelection &deselected )
{
    // prevent the graphics scene from sending the selectionChanged signal
    m_graphicsScene->blockSignals(true);

    m_nodeModel->setSelected(deselected, false);
    m_nodeModel->setSelected(selected, true);

    m_graphicsScene->blockSignals(false);

    // notify connected objects that the selection has changed
    emit selectionChanged(selected.size() > 0);
}


//!
//! Removes the connection that has been destroyed from the list of
//! connections.
//!
//! Is called when a connection has been destroyed.
//!
void SceneModel::connectionDestroyed ( Connection *connection )
{
	if (connection) {
		bool deleteGraphicsItem = false;
		const QString &connectionName = QString::number(connection->getId());

		if (m_connectionGraphicsItemMap.constFind(connectionName) != m_connectionGraphicsItemMap.constEnd())
			deleteGraphicsItem = true;

		m_nodeModel->beginUpdate();
		deleteConnection(connectionName, deleteGraphicsItem);
		m_nodeModel->endUpdate();
	}
}


//!
//! Event handler that is called when the in frame action has been
//! triggered.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_inFrameAction_triggered ( bool checked /* = false */ )
{
    setCurrentFrame(m_frameRangeParameter->getMinValue().toInt());
}


//!
//! Event handler that is called when the previous key action has been
//! triggered.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_previousKeyAction_triggered ( bool checked /* = false */ )
{
    Log::na("SceneModel::on_m_previousKeyAction_triggered");
}


//!
//! Event handler that is called when the previous frame action has been
//! triggered.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_previousFrameAction_triggered ( bool checked /* = false */ )
{
    // stop playing the animation if it is running
    if (m_playAction->isChecked())
        m_playAction->setChecked(false);

    int firstFrame = m_frameRangeParameter->getMinValue().toInt();
    int currentFrame = m_frameParameter->getValue().toInt();
    currentFrame -= m_frameStep;
    if (currentFrame < firstFrame)
        currentFrame = firstFrame;

    setCurrentFrame(currentFrame);
}


//!
//! Event handler that is called when the play action has been toggled.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_playAction_toggled ( bool checked )
{
    if (checked) {
        // rewind the animation if it should be played once and it reached the end
        if (m_playOnceAction->isChecked() && m_frameParameter->getValue().toInt() == m_frameRangeParameter->getMaxValue().toInt())
            m_inFrameAction->trigger();

        // start the timer
        int fps = m_fpsParameter->getValue().toInt();
        m_timerId = startTimer(1000.0 / fps);

        // update the play action's icon
        m_playAction->setIcon(QIcon(":/playingIcon"));

    } else {
        // stop the timer
        killTimer(m_timerId);
        m_timerId = 0;

        // update the play action's icon
        m_playAction->setIcon(QIcon(":/playIcon"));
    }
}

//!
//! Duplicates the selected node items.
//!
void SceneModel::duplicateNode ()
{
	const QStringList &selectedObjects = getSelectedObjects();
	foreach(const QString &objectName, selectedObjects) {
		Node *node = m_nodeModel->getNode(objectName);
		if (node) {
			const QGraphicsItem *graphicsItem = m_graphicsItemMap[objectName];
			if (graphicsItem) {
				const QString &type = node->getTypeName();
				const QString &newNodeName = createObject(type, node->getName(), graphicsItem->pos()+QPointF(16,16));
				deselectObject(objectName);
				selectObject(newNodeName, true);
			}
			else
				continue;
		}
	}
}

//!
//! Event handler that is called when the realtime action has been toggled.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_realtimeAction_toggled ( bool checked )
{
    if (checked) {
        // disable other action
        m_inFrameAction->setEnabled(false);
        m_previousKeyAction->setEnabled(false);
        m_previousFrameAction->setEnabled(false);
        m_playAction->setEnabled(false);
        m_nextFrameAction->setEnabled(false);
        m_nextKeyAction->setEnabled(false);
        m_outFrameAction->setEnabled(false);
        m_separatorAction->setEnabled(false);
        m_playOptionsAction->setEnabled(false);
        m_playLoopingAction->setEnabled(false);
        m_playOnceAction->setEnabled(false);

        // start the timer
        int fps = m_fpsParameter->getValue().toInt();
        m_timerId = startTimer(1000.0 / fps);
        m_lastTimestamp = QTime::currentTime();
        // update the play action's icon
        //m_realtimeAction->setIcon(QIcon(":/realtimeIcon"));
        m_realtimeAction->setIcon(QIcon());
    } else {
        // stop the timer
        killTimer(m_timerId);
        m_timerId = 0;

        // update the play action's icon
        m_realtimeAction->setIcon(QIcon(":/realtimeIcon"));

        // enable other actions
        m_inFrameAction->setEnabled(true);
        m_previousKeyAction->setEnabled(true);
        m_previousFrameAction->setEnabled(true);
        m_playAction->setEnabled(true);
        m_nextFrameAction->setEnabled(true);
        m_nextKeyAction->setEnabled(true);
        m_outFrameAction->setEnabled(true);
        m_separatorAction->setEnabled(true);
        m_playOptionsAction->setEnabled(true);
        m_playLoopingAction->setEnabled(true);
        m_playOnceAction->setEnabled(true);
    }
    emit realtimeModeToggled(checked);
}


//!
//! Event handler that is called when the next frame action has been
//! triggered.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_nextFrameAction_triggered ( bool checked /* = false */ )
{
    // stop playing the animation if it is running
    if (m_playAction->isChecked())
        m_playAction->setChecked(false);

    int lastFrame = m_frameRangeParameter->getMaxValue().toInt();
    int currentFrame = m_frameParameter->getValue().toInt();
    currentFrame += m_frameStep;
    if (currentFrame > lastFrame)
        currentFrame = lastFrame;

    setCurrentFrame(currentFrame);
}


//!
//! Event handler that is called when the next key action has been
//! triggered.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_nextKeyAction_triggered ( bool checked /* = false */ )
{
    Log::na("SceneModel::on_m_nextKeyAction_triggered");
}


//!
//! Event handler that is called when the out frame action has been
//! triggered.
//!
//! \param checked The state of the action.
//!
void SceneModel::on_m_outFrameAction_triggered ( bool checked /* = false */ )
{
    setCurrentFrame(m_frameRangeParameter->getMaxValue().toInt());
}


///
/// Private Functions
///


//!
//! Initializes the actions that are contained in the scene model.
//!
void SceneModel::initializeActions ()
{  
	// initialize in frame action
    m_inFrameAction->setObjectName("m_inFrameAction");
    m_inFrameAction->setStatusTip(tr("Jump to In Frame [Home]"));
    m_inFrameAction->setToolTip(tr("In Frame"));
    m_inFrameAction->setShortcut(Qt::Key_Home);
    m_inFrameAction->setShortcutContext(Qt::ApplicationShortcut);
    m_inFrameAction->setAutoRepeat(false);

    // initialize previous keyframe action
    m_previousKeyAction->setObjectName("m_previousKeyAction");
    m_previousKeyAction->setStatusTip(tr("Jump to Previous Keyframe [Ctrl+Left]"));
    m_previousKeyAction->setToolTip(tr("Previous Keyframe"));
    m_previousKeyAction->setShortcut(QKeySequence(tr("Ctrl+Left", "Previous Keyframe")));
    m_previousKeyAction->setShortcutContext(Qt::ApplicationShortcut);

    // initialize previous frame action
    m_previousFrameAction->setObjectName("m_previousFrameAction");
    m_previousFrameAction->setStatusTip(tr("Go to Previous Frame [Left]"));
    m_previousFrameAction->setToolTip(tr("Previous Frame"));
    m_previousFrameAction->setShortcut(Qt::Key_Left);
    m_previousFrameAction->setShortcutContext(Qt::ApplicationShortcut);

    // initialize play action
    m_playAction->setObjectName("m_playAction");
    m_playAction->setCheckable(true);
    m_playAction->setStatusTip(tr("Play back the animation [Space]"));
    m_playAction->setToolTip(tr("Play"));
    m_playAction->setShortcut(Qt::Key_Space);
    m_playAction->setShortcutContext(Qt::ApplicationShortcut);
    m_playAction->setAutoRepeat(false);

    // initialize realtime action
    m_realtimeAction->setObjectName("m_realtimeAction");
    m_realtimeAction->setCheckable(true);
    m_realtimeAction->setStatusTip(tr("Activate the realtime mode [R]"));
    m_realtimeAction->setToolTip(tr("Realtime Animation"));
    m_realtimeAction->setText(tr(">>"));
    m_realtimeAction->setShortcut(Qt::Key_R);
    m_realtimeAction->setShortcutContext(Qt::ApplicationShortcut);
    m_realtimeAction->setAutoRepeat(false);

    // initialize next frame action
    m_nextFrameAction->setObjectName("m_nextFrameAction");
    m_nextFrameAction->setStatusTip(tr("Go to Next Frame [Right]"));
    m_nextFrameAction->setToolTip(tr("Next Frame"));
    m_nextFrameAction->setShortcut(Qt::Key_Right);
    m_nextFrameAction->setShortcutContext(Qt::ApplicationShortcut);

    // initialize next keyframe action
    m_nextKeyAction->setObjectName("m_nextKeyAction");
    m_nextKeyAction->setStatusTip(tr("Jump to Next Keyframe [Ctrl+Right]"));
    m_nextKeyAction->setToolTip(tr("Next Keyframe"));
    m_nextKeyAction->setShortcut(QKeySequence(tr("Ctrl+Right", "Next Keyframe")));
    m_nextKeyAction->setShortcutContext(Qt::ApplicationShortcut);

    // initialize out frame action
    m_outFrameAction->setObjectName("m_outFrameAction");
    m_outFrameAction->setStatusTip(tr("Jump to Out Frame [End]"));
    m_outFrameAction->setToolTip(tr("Out Frame"));
    m_outFrameAction->setShortcut(Qt::Key_End);
    m_outFrameAction->setShortcutContext(Qt::ApplicationShortcut);
    m_outFrameAction->setAutoRepeat(false);

    // initialize separator action
    m_separatorAction->setSeparator(true);

    // initialize play looping action
    m_playLoopingAction->setObjectName("m_playLoopingAction");
    m_playLoopingAction->setCheckable(true);
    m_playLoopingAction->setChecked(true);
    m_playLoopingAction->setStatusTip(tr("Rewind the animation at the out frame"));
    m_playLoopingAction->setToolTip(tr("Play Once"));

    // initialize play once action
    m_playOnceAction->setObjectName("m_playOnceAction");
    m_playOnceAction->setCheckable(true);
    m_playOnceAction->setStatusTip(tr("Stop playing the animation at the out frame"));
    m_playOnceAction->setToolTip(tr("Play Once"));

    // create looping mode group
    QActionGroup *loopingModeGroup = new QActionGroup(this);
    loopingModeGroup->addAction(m_playLoopingAction);
    loopingModeGroup->addAction(m_playOnceAction);

    // initialize play options action
    m_playOptionsAction->setMenu(new QMenu("Play Options"));
    m_playOptionsAction->menu()->addActions(loopingModeGroup->actions());

    // set up signal/slot connections for the actions by their object name
    QMetaObject::connectSlotsByName(this);

    // enable or disable the timeline actions according to the current frame index
    updateActions();
}


//!
//! Enables or disables the timeline actions according to the current frame
//! index.
//!
void SceneModel::updateActions ()
{
    if (m_realtimeAction && m_realtimeAction->isEnabled())
        return;

    int currentFrame = m_frameParameter->getValue().toInt();
    int minimumFrame = m_frameRangeParameter->getMinValue().toInt();
    int maximumFrame = m_frameRangeParameter->getMaxValue().toInt();

    m_inFrameAction->setEnabled(currentFrame > minimumFrame);
    m_previousKeyAction->setEnabled(false);
    m_previousFrameAction->setEnabled(currentFrame > minimumFrame);
    m_playAction->setEnabled(minimumFrame < maximumFrame);
    m_nextFrameAction->setEnabled(currentFrame < maximumFrame);
    m_nextKeyAction->setEnabled(false);
    m_outFrameAction->setEnabled(currentFrame < maximumFrame);
}

//!
//! Deletes a connection between parameters.
//!
//! \param name The of the connection to be deleted
//! \param deleteGraphicItem Switch to determine if the connection graphics
//! item should be deleted together with the connection or not.
//!
void SceneModel::deleteConnection ( const QString &name, bool deleteGraphicItem /*= false*/ )
{
	// obtain the connection graphics item from the connection graphics item map
	Connection *connection = m_nodeModel->getConnection(name);

	ConnectionGraphicsItem *connectionGraphicsItem = 0;
	QHash<QString, QGraphicsItem *>::Iterator graphicsItem = m_connectionGraphicsItemMap.find(name);
	if (graphicsItem != m_connectionGraphicsItemMap.end())
		connectionGraphicsItem = dynamic_cast<ConnectionGraphicsItem *>(graphicsItem.value());

    // delete the node graphics item
    if (connectionGraphicsItem && deleteGraphicItem) {
		// remove the node graphics item from the graphics scene
        if (m_graphicsScene)
            m_graphicsScene->removeItem(connectionGraphicsItem);

        // remove the connection graphics item from the connected node graphics items
        NodeGraphicsItem *startNodeItem = connectionGraphicsItem->getStartNodeItem();
        if (startNodeItem)
            startNodeItem->removeConnectionItem(connectionGraphicsItem);

        NodeGraphicsItem *endNodeItem = connectionGraphicsItem->getEndNodeItem();
        if (endNodeItem)
            endNodeItem->removeConnectionItem(connectionGraphicsItem);

		// remove the graphics item from the item map
		m_connectionGraphicsItemMap.remove(name);
        
		// delete the graphics item 
		// !!the destructor will call deleteConnection again to 
		// finaly delete the connection within the else if branch!!
		deleteConnection(name);
    }

	else if (connection) {
		Parameter *sourceParameter = connection->getSourceParameter();
		Parameter *targetParameter = connection->getTargetParameter();
		const int connectionID = connection->getId();
		
		if (targetParameter)
			emit targetParameter->connectionDestroyed(connectionID);  // send this bevore deleting connection!

		// delete the connection from the node model
		m_nodeModel->deleteConnection(name);
	
		if (sourceParameter)
			emit sourceParameter->connectionDestroyed(connectionID);  // seim: investigate for new node processing!
	}
}

//!
//! Creates the connection graphics item for the visual representation of a connection.
//!
//! \param connection The connection for which the graphics item should be generated.
//!
void SceneModel::createConnectionItem (Connection *connection)
{
	const Parameter *sourceParameter = connection->getSourceParameter();
	const Parameter *targetParameter = connection->getTargetParameter();
	NodeGraphicsItem *sourceGraphicsItem = dynamic_cast<NodeGraphicsItem*>(getGraphicsItem(sourceParameter->getNode()->getName()));
	NodeGraphicsItem *targetGraphicsItem = dynamic_cast<NodeGraphicsItem*>(getGraphicsItem(targetParameter->getNode()->getName()));

	ConnectionGraphicsItem *connectionItem = new ConnectionGraphicsItem(QString::number(connection->getId()), Parameter::getTypeColor(sourceParameter->getType()), QPointF(0.0, 0.0), QPointF(0.0, 0.0));
	m_graphicsScene->addItem(connectionItem);
	connectionItem->setConnection(connection);
	connectionItem->setStartNodeItem(sourceGraphicsItem);
	connectionItem->setEndNodeItem(targetGraphicsItem);
	m_connectionGraphicsItemMap[QString::number(connection->getId())] = connectionItem;
	connect(connectionItem, SIGNAL(destroyed(Connection *)), SLOT(connectionDestroyed(Connection *)));

	sourceGraphicsItem->refresh();
	targetGraphicsItem->refresh();
}

//!
//! Decodes the given position for node graphics items from a COLLADA scene
//! file.
//!
//! \param position The position value to decode into a Qt position.
//! \param nodeName The name of the node whose position value to decode (used for log messages only).
//! \return The position corresponding to the given position value.
//!
QPointF SceneModel::decodePosition ( std::string position, const QString &nodeName )
{
    QPointF result;

    // split the position value into its parts
    QStringList parts = QString::fromStdString(position).split(" ");
    if (parts.size() == 2) {
        bool ok = true;

        // convert X value
        float x = parts[0].toFloat(&ok);
        if (ok)
            result.setX(x);
        else
            Log::warning(QString("Node \"%1\": The node's X position is not a valid floating-point number: \"%2\"").arg(nodeName).arg(parts[0]), "SceneModel::decodePosition");

        // convert Y value
        float y = parts[1].toFloat(&ok);
        if (ok)
            result.setY(y);
        else
            Log::warning(QString("Node \"%1\": The node's Y position is not a valid floating-point number: \"%2\"").arg(nodeName).arg(parts[1]), "SceneModel::decodePosition");
    } else
        Log::warning(QString("Node \"%1\": The position value does not contain two floating-point numbers: \"%2\"").arg(nodeName).arg(QString::fromStdString(position)), "SceneModel::decodePosition");

    return result;
}


//!
//! Creates COLLADA elements corresponding to the parameters contained in
//! the given parameter group and adds them to the given parent element.
//!
//! \param parameterGroup The parameter group to create COLLADA elements from.
//! \param parentElement The element under which to create the COLLADA elements representing the parameter group.
//!
void SceneModel::createDaeElements ( ParameterGroup *parameterGroup, daeElement *parentElement ) const
{
    // make sure the given parameter group and parent element are valid
    if (!parameterGroup || !parentElement) {
        Log::error("The given parameter group or parent element is invalid.", "SceneModel::createDaeElements");
        return;
    }

    // iterate over the list of parameters contained in the given parameter group
    const AbstractParameter::List& parameterList = parameterGroup->getParameterList();
    for (int i = 0; i < parameterList.size(); ++i) {
        // check if the abstract parameter from the list is a group
        AbstractParameter *abstractParameter = parameterList.at(i);
        if (abstractParameter->isGroup()) {
            daeElement *frapperGroupElement = parentElement->add("group");
            frapperGroupElement->setAttribute("name", abstractParameter->getName().toStdString().c_str());
            // recursively create DAE elements for the nested parameter group
            if (parameterGroup->getNode()->isSaveable())
                createDaeElements(dynamic_cast<ParameterGroup *>(abstractParameter), frapperGroupElement);
            else
                createDaeElements(dynamic_cast<ParameterGroup *>(abstractParameter), parentElement);
        }
        else {
            // skip text info, command parameters, and parameters holding default values
            Parameter *parameter = dynamic_cast<Parameter *>(abstractParameter);
            NumberParameter *numberParameter = dynamic_cast<NumberParameter *>(parameter);
            if ( (parameter->getNode()->isSaveable()) && 
				parameter->getType() != Parameter::T_TextInfo &&
                parameter->getType() != Parameter::T_Label &&
                parameter->getType() != Parameter::T_Command && 
                (!parameter->hasDefaultValue() || (numberParameter && numberParameter->isAnimated() && parameter->getNode()->isSaveable()))) {
                    // get details of parameter
					const QString &name = parameter->getName();
					const QString &value = parameter->getValueString();
					const QString &type = Parameter::getTypeName(parameter->getType());
					const QString &size = QString::number(parameter->getSize());
					const QString &multi = QString::number(parameter->getMultiplicity());
					const QString &state = QString::number(parameter->isEnabled());
					const QString &readOnly = QString::number(parameter->isReadOnly());
					const QString &visible = QString::number(parameter->isVisible());
					const QString &selfEval = QString::number(parameter->isSelfEvaluating());
					const QString &pinType = QString::number(parameter->getPinType());
					QString changeFunction = parameter->getChangeFunction();
					QString procFunction = parameter->getProcessingFunction();
					QString auxProcFunction = parameter->getAuxProcessingFunction();
					// create a DAE element representing the parameter
					daeElement *frapperParameterElement = parentElement->add("parameter");
					frapperParameterElement->setAttribute("name", name.toStdString().c_str());
					frapperParameterElement->setAttribute("value",value.toStdString().c_str());
					frapperParameterElement->setAttribute("type",type.toStdString().c_str());
					frapperParameterElement->setAttribute("size",size.toStdString().c_str());
					frapperParameterElement->setAttribute("multi",multi.toStdString().c_str());
					frapperParameterElement->setAttribute("enabled",state.toStdString().c_str());
					frapperParameterElement->setAttribute("readOnly",readOnly.toStdString().c_str());
					frapperParameterElement->setAttribute("visible",visible.toStdString().c_str());
					frapperParameterElement->setAttribute("selfEval",selfEval.toStdString().c_str());
					frapperParameterElement->setAttribute("pin",pinType.toStdString().c_str());
					// save parameters processing functions
					if (changeFunction.isEmpty())
						changeFunction = "0";
					frapperParameterElement->setAttribute("chFunc",changeFunction.toStdString().c_str());
					if (procFunction.isEmpty())
						procFunction = "0";
					frapperParameterElement->setAttribute("proFunc",procFunction.toStdString().c_str());
					if (auxProcFunction.isEmpty())
						auxProcFunction = "0";
					frapperParameterElement->setAttribute("auxFunc",auxProcFunction.toStdString().c_str());
					// save keys
					if (numberParameter && numberParameter->isAnimated() && numberParameter->getNode()->isSaveable()) {
						const float timeScale = numberParameter->getTimeStepSize();
						if (timeScale != 1) 
							frapperParameterElement->setAttribute("tScale", QString::number(timeScale).toStdString().c_str());
						daeElement *keysElement = frapperParameterElement->add("keys");
						const QList<Key> &keyList = numberParameter->getKeys();
						foreach (const Key &key, keyList) {
							daeElement *keyElement = keysElement->add("key");
							keyElement->setAttribute("type", QString::number(key.type).toStdString().c_str());
							keyElement->setAttribute("index", QString::number(key.index).toStdString().c_str());


							if(parameter->getType() == Parameter::T_Color) {
								QColor color = key.keyValue.value<QColor>();
								keyElement->setAttribute("value", QString("%1 %2 %3 %4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()).toStdString().c_str());
							}
							else {
								keyElement->setAttribute("value", QString::number(key.keyValue.toFloat()).toStdString().c_str());
							}

							if (key.type == Key::KT_Bezier) {
								keyElement->setAttribute("tIndex", QString::number(key.tangentIndex).toStdString().c_str());
								keyElement->setAttribute("tValue", QString::number(key.tangentValue).toStdString().c_str());
							}
						}
                    }
            }
        }
    }
}

} // end namespace Frapper