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
//! \file "Controller.cpp"
//! \brief Implementation file for Controller class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       27.07.2012 (last updated)
//!

#include "Controller.h"
#include "ViewportPanel.h"
#include "DocumentationPanel.h"
#include "NetworkPanel.h"
#include "HierarchyPanel.h"
#include "ParameterPanel.h"
#include "CurveEditorPanel.h"
#include "TimelinePanel.h"
#include "ViewingParameters.h"


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the Controller class.
//!
//! \param model The model to use for the views.
//!
Controller::Controller ( SceneModel *model ) :
    m_sceneModel(model)
{
}


///
/// Public Functions
///


//!
//! Registers the given view panel to display the registered model.
//!
//! \param viewPanel The view panel to display the registered model.
//!
void Controller::registerViewPanel ( ViewPanel *viewPanel )
{
    // get the scene model objects from the model
    NodeModel *nodeModel = m_sceneModel->getNodeModel();
    QItemSelectionModel *selectionModel = m_sceneModel->getSelectionModel();
    QGraphicsScene *graphicsScene = m_sceneModel->getGraphicsScene();

    switch (viewPanel->getViewPanelType()) {
        case ViewPanel::T_Viewport:
            {
                ViewportPanel *viewportPanel = qobject_cast<ViewportPanel *>(viewPanel);
                ViewportWidget *viewportWidget = viewportPanel->getViewportWidget();

                // redraw the viewport when the time, the scene model, or the object selection in the scene have changed
                QObject::connect(m_sceneModel, SIGNAL(triggerRedraw()), viewportWidget, SLOT(render()));
                QObject::connect(m_sceneModel, SIGNAL(selectionChanged(bool)), viewportWidget, SLOT(render()));

                QObject::connect(viewportPanel, SIGNAL(newCameraRequested(ViewingParameters *)), m_sceneModel, SLOT(createCamera(ViewingParameters *)));
                QObject::connect(viewportPanel, SIGNAL(selectCameraRequested(const QString &, bool)), m_sceneModel, SLOT(selectObject(const QString &, bool)));
                QObject::connect(viewportPanel, SIGNAL(deleteCameraRequested(const QString &)), m_sceneModel, SLOT(deleteCamera(const QString &)));
                
				QObject::connect(m_sceneModel, SIGNAL(camerasUpdated(const QList<Node *> &, const QString &)), viewportPanel, SLOT(updateCameras(const QList<Node *> &, const QString &)));
                QObject::connect(viewportPanel, SIGNAL(requestCameraList()), m_sceneModel, SLOT(cameraListRequested()));
                QObject::connect(viewportPanel, SIGNAL(updateCameraRequested(const QString &, ViewingParameters *)), m_sceneModel, SLOT(updateCamera(const QString &, ViewingParameters *)));
                QObject::connect(viewportPanel, SIGNAL(applyCameraRequested(const QString &, ViewingParameters *)), m_sceneModel, SLOT(applyCamera(const QString &, ViewingParameters *)));
				QObject::connect(viewportPanel, SIGNAL(triggerRebuild()), m_sceneModel, SLOT(rebuildScene()));
                //QObject::connect(viewportPanel, SIGNAL(triggerRebuild()), m_sceneModel, SIGNAL(rebuildRendertargets()));

                // set up connection for displaying an image in the viewport
                QObject::connect(m_sceneModel, SIGNAL(imageSet(unsigned int, Ogre::TexturePtr)), viewportWidget, SLOT(setImage(unsigned int, Ogre::TexturePtr)));

                // notify the scene model when an object in the viewport has been selected
                QObject::connect(viewportWidget, SIGNAL(objectSelected(const QString &, bool)), m_sceneModel, SLOT(selectObject(const QString &, bool)));

                // relay requests to frame selected objects to the scene model
                QObject::connect(viewportWidget, SIGNAL(frameSelectedObjectsRequested(ViewingParameters *)), m_sceneModel, SLOT(frameSelectedObjects(ViewingParameters *)));
                
                QObject::connect(viewportWidget, SIGNAL(renderingTriggered()), m_sceneModel, SIGNAL(renderingTriggered()));

				m_sceneModel->emitRebuildRendertargets();
            }
            break;
        case ViewPanel::T_NetworkEditor:
            {
                NetworkPanel *networkPanel = qobject_cast<NetworkPanel *>(viewPanel);
                NetworkGraphicsView *networkGraphicsView = networkPanel->getNetworkGraphicsView();
                networkGraphicsView->setScene(graphicsScene);

                // store the scene coordinates at which a context menu has been requested as the position to use for a new node
				QObject::connect(networkGraphicsView, SIGNAL(contextMenuRequested(const QPointF &)), m_sceneModel, SLOT(setNewNodePosition(const QPointF &)));
                QObject::connect(networkGraphicsView, SIGNAL( deleteSelectedNode() ), m_sceneModel, SLOT( deleteSelected()));

            }
            break;
        case ViewPanel::T_HierarchyEditor:
            {
                QTreeView *sceneTreeView = qobject_cast<HierarchyPanel *>(viewPanel)->getTreeView();
                sceneTreeView->setModel(nodeModel);
                sceneTreeView->setSelectionModel(selectionModel);
                sceneTreeView->expandAll();

                // expand all items in the scene tree view when another scene object has been created
                QObject::connect(m_sceneModel, SIGNAL(objectCreated(const QString &)), sceneTreeView, SLOT(expandAll()));
            }
            break;
        case ViewPanel::T_ParameterEditor:
            {
                ParameterPanel *parameterPanel = qobject_cast<ParameterPanel *>(viewPanel);

                QObject::connect(nodeModel, SIGNAL(nodesSelected(const QList<Node *> &)), parameterPanel, SLOT(showNodes(const QList<Node *> &)));
                QObject::connect(nodeModel, SIGNAL(nodeDeleted(const QString &)), parameterPanel, SLOT(deleteNode(const QString &)));
                //QObject::connect(m_sceneModel, SIGNAL(selectionChanged(bool)), parameterPanel, SLOT(updateParameters(bool)));
                QObject::connect(parameterPanel, SIGNAL(objectNameChangeRequested(const QString &, const QString &)), nodeModel, SLOT(renameNode(const QString &, const QString &)));
                QObject::connect(parameterPanel, SIGNAL(objectNameChangeRequested(const QString &, const QString &)), m_sceneModel, SLOT(renameNode(const QString &, const QString &)));
                QObject::connect(parameterPanel, SIGNAL(parameterChangeRequested(const QString &, const QString &, const QVariant &)), nodeModel, SLOT(changeParameter(const QString &, const QString &, const QVariant &)));
                QObject::connect(parameterPanel, SIGNAL(changeParameterPinTypeRequested(const QString &, const QString &, const Parameter::PinType)), nodeModel, SLOT(changeParameterPinType(const QString &, const QString &, const Parameter::PinType)));
                QObject::connect(parameterPanel, SIGNAL(changeParameterSelfEvaluationRequested(const QString &, const QString &, const bool)), nodeModel, SLOT(changeParameterSelfEvaluation(const QString &, const QString &, const bool)));
            }
            break;
        case ViewPanel::T_CurveEditor:
            {
				CurveEditorPanel *curveEditorPanel = qobject_cast<CurveEditorPanel *>(viewPanel);

				curveEditorPanel->setInFrame(m_sceneModel->getInFrame());
				curveEditorPanel->setOutFrame(m_sceneModel->getOutFrame());
				curveEditorPanel->setCurrentFrame(m_sceneModel->getCurrentFrame());
                QObject::connect(nodeModel, SIGNAL(nodeSelected(Node *)), curveEditorPanel, SLOT(buildTree(Node *)));
                QObject::connect(m_sceneModel, SIGNAL(selectionChanged(bool)), curveEditorPanel, SLOT(updateTree(bool)));
                QObject::connect(m_sceneModel, SIGNAL(currentFrameSet(int)), curveEditorPanel, SLOT(setCurrentFrame(int)));
				QObject::connect(m_sceneModel, SIGNAL(inFrameSet(int)), curveEditorPanel, SLOT(setInFrame(int)));
                QObject::connect(m_sceneModel, SIGNAL(outFrameSet(int)), curveEditorPanel, SLOT(setOutFrame(int)));
                QObject::connect(curveEditorPanel, SIGNAL(drag()), m_sceneModel, SLOT(updateFrame()));

                // display the curves of the first selected node in the new curve panel
                QStringList selectedNodes = m_sceneModel->getSelectedObjects();
                if (selectedNodes.size() > 0) {
                    Node *selectedNode = nodeModel->getNode(selectedNodes.at(0));
                    curveEditorPanel->buildTree(selectedNode);
                }
            }
            break;
        case ViewPanel::T_Timeline:
            {
                TimelinePanel *timelinePanel = qobject_cast<TimelinePanel *>(viewPanel);

                // set the frame range for the timeline panel
                int frameStep = m_sceneModel->getFrameStep();
                int frameRate = m_sceneModel->getFrameRate();
                int firstFrame;
                int lastFrame;
                m_sceneModel->getFrameRange(firstFrame, lastFrame);
                timelinePanel->setFrameStep(frameStep);
                timelinePanel->setFrameRate(frameRate);
                timelinePanel->setFrameRange(firstFrame, lastFrame);

                // set the time frame indices for the timeline panel
                timelinePanel->setStartFrame(m_sceneModel->getStartFrame());
                timelinePanel->setEndFrame(m_sceneModel->getEndFrame());
                timelinePanel->setInFrame(m_sceneModel->getInFrame());
                timelinePanel->setOutFrame(m_sceneModel->getOutFrame());
                timelinePanel->setCurrentFrame(m_sceneModel->getCurrentFrame());
                timelinePanel->setStartFrame(m_sceneModel->getStartFrame());

                // fill the timeline panel's control tool bar with timeline control actions
                timelinePanel->fillControlToolBar(m_sceneModel->getControlActions());

                // set up signal/slot connections from the timeline panel to the scene model
                QObject::connect(timelinePanel, SIGNAL(currentFrameSet(int)), m_sceneModel, SLOT(setCurrentFrame(int)));
                QObject::connect(timelinePanel, SIGNAL(startFrameSet(int)), m_sceneModel, SLOT(setStartFrame(int)));
                QObject::connect(timelinePanel, SIGNAL(endFrameSet(int)), m_sceneModel, SLOT(setEndFrame(int)));
                QObject::connect(timelinePanel, SIGNAL(inFrameSet(int)), m_sceneModel, SLOT(setInFrame(int)));
                QObject::connect(timelinePanel, SIGNAL(outFrameSet(int)), m_sceneModel, SLOT(setOutFrame(int)));
                QObject::connect(timelinePanel, SIGNAL(frameStepSet(int)), m_sceneModel, SLOT(setFrameStep(int)));
                QObject::connect(timelinePanel, SIGNAL(frameRateSet(int)), m_sceneModel, SLOT(setFrameRate(int)));

                // set up signal/slot connections from the scene model to the timeline panel
                QObject::connect(m_sceneModel, SIGNAL(currentFrameSet(int)), timelinePanel, SLOT(setCurrentFrame(int)));
                QObject::connect(m_sceneModel, SIGNAL(startFrameSet(int)), timelinePanel, SLOT(setStartFrame(int)));
                QObject::connect(m_sceneModel, SIGNAL(endFrameSet(int)), timelinePanel, SLOT(setEndFrame(int)));
                QObject::connect(m_sceneModel, SIGNAL(inFrameSet(int)), timelinePanel, SLOT(setInFrame(int)));
                QObject::connect(m_sceneModel, SIGNAL(outFrameSet(int)), timelinePanel, SLOT(setOutFrame(int)));
                QObject::connect(m_sceneModel, SIGNAL(frameStepSet(int)), timelinePanel, SLOT(setFrameStep(int)));
                QObject::connect(m_sceneModel, SIGNAL(frameRateSet(int)), timelinePanel, SLOT(setFrameRate(int)));
                QObject::connect(m_sceneModel, SIGNAL(realtimeModeToggled(bool)), timelinePanel, SLOT(toggleRealtimeMode(bool)));
            }
            break;
            case ViewPanel::T_Documentation:
            {
                DocumentationPanel *documentationPanel = qobject_cast<DocumentationPanel *>(viewPanel);
                documentationPanel->setDocument(m_sceneModel->getName());
                QObject::connect(m_sceneModel, SIGNAL(sceneNameChanged(const QString &)), documentationPanel, SLOT(setDocument(const QString &)));
            }
            break;	

		case ViewPanel::T_PluginPanel:
			{
				// Plugin Panels
				viewPanel->registerControl(nodeModel, m_sceneModel);
			}
			break;
    }
}

} // end namespace Frapper