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
//! \file "SceneModel.h"
//! \brief Header file for SceneModel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       21.11.2013 (last updated)
//!

#ifndef SCENEMODEL_H
#define SCENEMODEL_H

#include "NodeModel.h"
#include "NodeGraphicsItem.h"
#include "NodeGroupGraphicsItem.h"
#include "NodeBackDropGraphicsItem.h"
#include "ViewNode.h"
#include "NumberParameter.h"
#include "ViewingParameters.h"
#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QGraphicsScene>
#include <QtCore/QMutex>

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

// COLLADA
#define NO_BOOST
#include <dae.h>
#include <dom/domCOLLADA.h>

namespace Frapper {

//!
//! Class representing the model of the scene containing objects like meshes,
//! cameras and lights.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph SceneModel {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     SceneModel [label="SceneModel",fillcolor="grey75"];
//!     QObject -> SceneModel;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT SceneModel : public QObject
{

    Q_OBJECT

public: // static functions

	//!
	//! Set the file name of the loaded scene file
	//!
	//! \param sceneFileName The file name of the loaded scene.
	//!
	static void setSceneFileName ( QString sceneFileName );

	//!
	//! Get the file name of the loaded scene file
	//!
	//! \return The filename of the current loaded scene
	//!
	static QString getSceneFileName ();

	//!
	//! Set the scenes working directory
	//!
	//! \param workingDirectory The working directory for the scene
	//!
	static void setWorkingDirectory ( QString workingDirectory );

	//!
	//! Get the working directory of the loaded scene file
	//!
	//! \return The working directory of the current scene
	//!
	static QString getWorkingDirectory ();

	//!
	//! Opens the select working directory dialog
	//!
	static QString selectWorkingDirectory();

private: // static data

	//!
	//! The file name of the loaded scene
	//!
	static QString s_sceneFileName;

	//!
	//! The working directory of the loaded scene
	//!
	static QString s_sceneWorkingDirectory;

public: // constructors and destructors

    //!
    //! Constructor of the SceneModel class.
    //!
    SceneModel ();

    //!
    //! Destructor of the SceneModel class.
    //!
    ~SceneModel ();

public: // functions

    //!
    //! Returns the name of the scene.
    //!
    //! \return The name of the scene.
    //!
    QString getName () const;

    //!
    //! Sets the name of the scene.
    //!
    //! \param name The name of the scene.
    //!
    void setName (const QString &name);

    //!
    //! Returns the node model that holds all scene objects like meshes,
    //! cameras and lights.
    //!
    //! \return The node model that holds all scene objects.
    //!
    NodeModel * getNodeModel () const;

    //!
    //! Returns the selection model to use for synchronizing selections in all
    //! views.
    //!
    //! \return The selection model to use for synchronizing selections.
    //!
    QItemSelectionModel * getSelectionModel () const;

    //!
    //! Returns the graphics graphics item associated with a node by name.
    //!
    //! \return The graphics item associated with the node.
    //!
    QGraphicsItem * getGraphicsItem ( const QString &name ) const;

    //!
    //! Returns the graphics scene to use for all network views.
    //!
    //! \return The graphics scene to use for all network views.
    //!
    QGraphicsScene * getGraphicsScene () const;

    //!
    //! Returns a list of actions representing scene time controls.
    //!
    //! \return A list of actions representing scene time controls.
    //!
    QList<QAction *> getControlActions () const;

    //!
    //! Builds a list of object names from the current selection.
    //!
    //! \return A list of names of the currently selected objects.
    //!
    QStringList getSelectedObjects ( bool onlyConnections = false  ) const;

	//!
	//! Duplicates the selected node items.
	//!
	void duplicateNode ();

    //!
    //! Returns the frame step currently set for the scene.
    //!
    //! \return The frame step currently set for the scene.
    //!
    int getFrameStep () const;

    //!
    //! Returns the frame rate currently set for the scene.
    //!
    //! \return The frame rate currently set for the scene.
    //!
    int getFrameRate () const;

    //!
    //! Fills the given frame index variables with the frame range available
    //! for the scene time.
    //!
    //! \param firstFrame The index of the first frame in the scene's time range.
    //! \param lastFrame The index of the last frame in the scene's time range.
    //!
    void getFrameRange ( int& firstFrame, int& lastFrame ) const;

    //!
    //! Returns the index of the current frame in the scene's time.
    //!
    //! \return The index of the current frame in the scene's time.
    //!
    int getCurrentFrame () const;

    //!
    //! Returns the index of the start frame in the scene's time.
    //!
    //! \return The index of the start frame in the scene's time.
    //!
    int getStartFrame () const;

    //!
    //! Returns the index of the end frame in the scene's time.
    //!
    //! \return The index of the end frame in the scene's time.
    //!
    int getEndFrame () const;

    //!
    //! Returns the index of the in frame in the scene's time.
    //!
    //! \return The index of the in frame in the scene's time.
    //!
    int getInFrame () const;

    //!
    //! Returns the index of the out frame in the scene's time.
    //!
    //! \return The index of the out frame in the scene's time.
    //!
    int getOutFrame () const;

    //!
    //! Creates the OGRE scene root scene node.
    //!
    void createSceneRoot ();

    //!
    //! Creates the scene objects by parsing the COLLADA element tree contained
    //! in the given root element.
    //!
    //! \param rootElement The root element of the tree to build the scene from.
    //!
    void createScene ( daeElement *rootElement );

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
    QString createObject ( const QString &typeName, const QString &name = "", const QPointF &position = QPointF(-1, -1), bool select = false, bool visible = true );

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
    void connectParameters ( Node *sourceNode, const QString &sourceParameterName, Node *targetNode, const QString &targetParameterName, bool visible = true );

    //!
    //! Connects all parameters with the same name of two nodes.
    //!
    //! The parameters to be connected must have the same parameter type.
    //!
    //! \param sourceNode The source node for the new connections.
    //! \param targetNode The target node for the new connections.
    //!
    void connectParametersByName ( Node *sourceNode, Node *targetNode );

    //!
    //! Creates a COLLADA element tree corresponding to the objects currently
    //! contained in the scene.
    //!
    //! \param parentElement The element under which to create the COLLADA element tree representing the scene.
    //!
    void createDaeElements ( daeElement *parentElement );

    //!
    //! Creates a COLLADA elements representing global scene properties.
    //!
    //! \param parentElement The element under which to create the COLLADA element tree representing the scene.
    //!
    void createSceneDAEProperties ( daeElement *parentElement ) const;

    //!
    //! Deletes all objects contained in the scene.
    //!
    void clear ();

	void emitRebuildRendertargets();


    public slots: //

        //!
        //! Creates a connection between the given parameters.
        //!
        //! The parameters to be connected must have the same parameter type.
        //!
        //! \param sourceParameter The output parameter of the source node to connect.
        //! \param targetParameter The input parameter of the target node to connect.
        //!
        void connectParameters ( Parameter *sourceParameter, Parameter *targetParameter );

        //!
        //! Create connections between two nodes (connect by name).
        //!
        //! \param sourceNode The source node.
        //! \param targetNode The target node.
        //!
        void connectParameters ( Node *sourceNode, Node *targetNode );

		//!
		//! Creates a connection between the parameters given by name and node
		//!
		//! The parameters to be connected must have the same parameter type.
		//!
		//! \param sourceNodeName The name of the source node.
		//! \param sourceParameterName The name of the parameter of the source node to connect.
		//! \param targetNodeName The name of the target node.
		//! \param targetParameterName The name of the parameter of the target node to connect.
		//!
		void connectParameters ( QString sourceNodeName, QString sourceParameterName, QString targetNodeName, QString targetParameterName );

        ////!
        ////! Creates a new camera with parameters initialized according to the given
        ////! viewing parameters.
        ////!
        ////! \param viewingParameters A collection of parameters for viewing the 3D scene.
        ////!
        void createCamera ( ViewingParameters *viewingParameters );

        ////!
        ////! Deletes an existing camera.
        ////!
        ////! \param cameraName The name of the camera to delete.
        ////!
        void deleteCamera ( const QString &cameraName );

        //!
        //! Applies the given viewing parameters to the camera with the given name.
        //!
        //! \param cameraName The name of the camera to update.
        //! \param viewingParameters The viewing parameters to apply to the camera with the given name.
        //!
        void updateCamera ( const QString &cameraName, ViewingParameters *viewingParameters );

        //!
        //! Sends the list of camereas when requested.
        //!
        void cameraListRequested ();

        //!
        //! Applies the viewing parameters of the camera with the given name to the
        //! given viewing parameters.
        //!
        //! \param cameraName The name of the camera to use.
        //! \param viewingParameters The viewing parameters to modify.
        //!
        void applyCamera ( const QString &cameraName, ViewingParameters *viewingParameters );

        //!
        //! Deletes a given object.
        //!
        void deleteObject ( const QString &name );

        //!
        //! Selects and deselects a given object.
        //!
        void selectDeselectObject ( const QString &name );

        //!
        //! Deletes the currently selected objects from the scene.
        //!
        void deleteSelected ();

		//!
		//! Creates a backdrop node item for the selected nodes.
		//! \param name the name of the backdrop node
		//! \param position the position of the backdrop node
		//! \param width the width of the backdrop node
		//! \param height the height of the backdrop node
		//! \param backgroundColor the background color of the backdrop node
		//! \param textColor the color of the description text
		//! \param text the description text
		//! \param textSize the size of the description text
		//!
		void backDrop (const QString name = "", const QPointF position = QPointF(0.f, 0.f), const qreal width = 200, const qreal height = 200, const QColor backgroundColor = QColor(48,48,48), const QColor textColor = QColor(255, 255, 255), const QString text = "... and a fitting description here. Here are also HTML Tags supported.", const int textSize = 11, const QString head = "Write a headline...", const int headSize = 22, const bool lockChange = false );

		//!
		//! Creates a group node item out of the selected nodes.
		//!
		void groupSelected ();

		//!
		//! Creates a group node item out of nodes with the given name.
		//!
		//! \param nodeNames The list of node names that sould be grouped.
		//!
		void groupNodes ( const QList<Node *> &nodeList, const QList<QPointF> &posList );

		//!
		//! Creates a list of nodes out of the selected group node.
		//!
		void ungroupSelected ();

        //!
        //! Selects all objects in the scene.
        //!
        void selectAll ();

        //!
        //! Sets the evaluation flag of the selected objects to True.
        //!
        void evaluateSelectedObjects ();

        //!
        //! Sets the evaluation flag of the selected objects to False.
        //!
        void ignoreSelectedObjects ();

        //!
        //! Updates the selection model according to the currently selected graphics
        //! items in the graphics scene.
        //!
        //! Is called when the selection in the graphics scene has changed.
        //!
        void graphicsSceneSelectionChanged ();

        //!
        //! Updates the m_graphicsItemList in case a node has changed its name. 
        //!
        void changeNodeName(const QString &name);

        //!
        //! Selects the object with the given name or toggles the selection state
        //! of the object with the given name.
        //!
        //! \param name The name of the object to select.
        //! \param toggle Flag that controls whether to toggle the selection of the object with the given name.
        //!
        void selectObject ( const QString &name, bool toggle = false );

        //!
        //! Deselects the object with the given name.
        //!
        //! \param name The name of the object to deselect.
        //!
        void deselectObject ( const QString &name = "" );

        //!
        //! Sets the index of the current frame in the scene's time.
        //!
        //! \param index The new index of the current frame in the scene's time.
        //!
        inline void setCurrentFrame ( int index, bool realtime = false );

        //!
        //! Triggers set frame one time.
        //!
        void updateFrame ();

        //!
        //! Sets the index of the start frame in the scene's time.
        //!
        //! \param index The new index of the start frame in the scene's time.
        //!
        void setStartFrame ( int index );

        //!
        //! Sets the index of the end frame in the scene's time.
        //!
        //! \param index The new index of the end frame in the scene's time.
        //!
        void setEndFrame ( int index );

        //!
        //! Sets the index of the in frame in the scene's time.
        //!
        //! \param index The new index of the in frame in the scene's time.
        //!
        void setInFrame ( int index );

        //!
        //! Sets the index of the out frame in the scene's time.
        //!
        //! \param index The new index of the out frame in the scene's time.
        //!
        void setOutFrame ( int index );

        //!
        //! Sets the frame step to use for the scene.
        //!
        //! \param frameStep The frame step to use for the scene.
        //!
        void setFrameStep ( int frameStep );

        //!
        //! Sets the frame rate to use for the scene.
        //!
        //! \param frameRate The frame rate to use for the scene.
        //!
        void setFrameRate ( int frameRate );

        //!
        //! Adjusts the given viewing parameters so that the currently selected
        //! objects are centered in the view.
        //!
        //! All objects will be framed if no objects are currently selected.
        //!
        //! \param viewingParameters The viewing parameters to modify.
        //!
        void frameSelectedObjects ( ViewingParameters *viewingParameters );

        //!
        //! Sets the node that currently has the view flag set.
        //!
        //! \param stageIndex The index of the stage for which to set the view node (currently not used).
        //! \param viewNode The new node that is currently viewed.
        //!
        void setViewNode ( unsigned int stageIndex, ViewNode *viewNode );

        //!
        //! Rebuilds the OGRE scene by obtaining the Ogre scene node from the current
        //! view node.
        //!
        void rebuildScene ();

        //!
        //! Redraws the OGRE scene when active view node exists.
        //!
        void redrawTriggered ();

        //!
        //! Sets the scene coordinates to use when creating a new node.
        //!
        //! \param scenePosition The position in scene coordinates to use for a new node.
        //!
        void setNewNodePosition ( const QPointF &scenePosition );

        //!
        //! Adds a keyframe for the given number parameter at the current frame
        //! index.
        //!
        //! \param numberParameter The number parameter to add a keyframe for.
        //!
		void addKey ( NumberParameter *numberParameter );

		//!
		//! Gives the node with the given old name the given new name.
		//!
		//! \param oldName The old name of the scene object.
		//! \param newName The new name of the scene object.
		//!
		void renameNode ( const QString &oldName, const QString &newName );

signals: //

        //!
        //! Signal that is emitted when the scene name has changed.
        //!
        //! \param name The scene name.
        //!
        void sceneNameChanged ( const QString &name );

        //!
        //! Signal that is emitted when the realtime mode has changed.
        //!
        //! \param realtime Flag that states whether realtime mode is enabled or not.
        //!
        void realtimeModeToggled ( bool realtime );

        //!
        //! Signal that is emitted when the selection has changed.
        //!
        //! \param objectsSelected Flag that states whether objects in the scene are selected.
        //!
        void selectionChanged ( bool objectsSelected );

        //!
        //! Signal that is emitted when the index of the current frame in the
        //! scene's time has been changed.
        //!
        //! \param index The new index of the current frame in the scene's time.
        //!
        void currentFrameSet ( int index );

        //!
        //! Signal that is emitted when the index of the start frame in the scene's
        //! time has been changed.
        //!
        //! \param index The new index of the start frame in the scene's time.
        //!
        void startFrameSet ( int index );

        //!
        //! Signal that is emitted when the index of the end frame in the scene's
        //! time has been changed.
        //!
        //! \param index The new index of the end frame in the scene's time.
        //!
        void endFrameSet ( int index );

        //!
        //! Signal that is emitted when the index of the in frame in the scene's
        //! time has been changed.
        //!
        //! \param index The new index of the in frame in the scene's time.
        //!
        void inFrameSet ( int index );

        //!
        //! Signal that is emitted when the index of the out frame in the scene's
        //! time has been changed.
        //!
        //! \param index The new index of the out frame in the scene's time.
        //!
        void outFrameSet ( int index );

        //!
        //! Signal that is emitted when the frame step for the scene has been
        //! changed.
        //!
        //! \param frameStep The new frame step to use for the scene.
        //!
        void frameStepSet ( int frameStep );

        //!
        //! Signal that is emitted when the frame rate for the scene has been
        //! changed.
        //!
        //! \param frameRate The new frame rate to use for the scene.
        //!
        void frameRateSet ( int frameRate );

        //!
        //! Signal that is emitted when the internal state of the scene model has
        //! changed.
        //!
        void modified ();

		//!
		//! Signal emitted when ogre scene node has changed in a
		//! a way that only scene redraw is needed. Fires before redraw.
		//!
		void triggerPreRedraw();

        //!
        //! Signal emitted when ogre scene node has changed in a
        //! a way that only scene redraw is needed. No structural change.
        //!
        void triggerRedraw ();

        //!
        //! Signal emitted when render targets should be updated.
        //!
        void renderingTriggered ();

        //!
        //! Signal that is emitted when the image to be viewed has been changed.
        //!
        //! \param stageIndex The index of the stage to which the image belongs.
        //! \param image The image that is to be viewed.
        //!
        void imageSet ( unsigned int stageIndex, Ogre::TexturePtr image );

        //!
        //! Signal that is emitted when a new object has been created.
        //!
        //! \param objectName The name of the created object.
        //!
        void objectCreated ( const QString &objectName );

        ////!
        ////! Signal that is emitted when a new camera has been created.
        ////!
        ////! \param cameraName The name of the created camera.
        ////!
        //void cameraCreated ( const QString &cameraName );

        ////!
        ////! Signal that is emitted when an existing camera has been deleted.
        ////!
        ////! \param cameraName The name of the deleted camera.
        ////!
        //void cameraDeleted ( const QString &cameraName );

        //!
        //! Signal that is emitted when the list of cameras has changed.
        //!
        //! \param cameraNames List of cameras in the scene.
        //! \param cameraName Name of new created camera.
        //!
        void camerasUpdated ( const QList<Node *> &cameraNodes, const QString &cameraName );

		//!
        //! Signal that is emitted when the scene loading begins.
        //!
        void loadStarted ();

        //!
        //! Signal that is emitted when the parameter loading is ready.
        //!
        void loadSceneElementsReady ();

        //!
        //! Signal that is emitted when the scene loading is ready.
        //!
        void loadReady ();

        //!
        //! Signal that is emitted when the scene saving begins.
        //!
        void saveStarted ();

		//!
		//! Signal that is emitted when the scene saving has finished.
		//!
		void saveFinished ();

		//!
		//! Signal that is emitted when a viewport panel has been destroyed.
		//!
		void rebuildRendertargets ();

protected: // events

    //!
    //! Handles timer events for the scene model.
    //!
    //! \param event The description of the timer event.
    //!
    virtual void timerEvent ( QTimerEvent *event );

    private slots: //

        //!
        //! Updates the scene model according to the given item selections.
        //!
        //! Is called when the selection in the selection model has changed.
        //!
        //! \param selected The list of selected items.
        //! \param deselected The list of deselected items.
        //!
        void selectionModelSelectionChanged ( const QItemSelection &selected, const QItemSelection &deselected );

        //!
        //! Removes the connection that has been destroyed from the list of
        //! connections.
        //!
        //! Is called when a connection has been destroyed.
        //!
        void connectionDestroyed ( Connection *connection);

        //!
        //! Event handler that is called when the in frame action has been
        //! triggered.
        //!
        //! \param checked The state of the action.
        //!
        void on_m_inFrameAction_triggered ( bool checked = false );

        //!
        //! Event handler that is called when the previous key action has been
        //! triggered.
        //!
        //! \param checked The state of the action.
        //!
        void on_m_previousKeyAction_triggered ( bool checked = false );

        //!
        //! Event handler that is called when the previous frame action has been
        //! triggered.
        //!
        //! \param checked The state of the action.
        //!
        void on_m_previousFrameAction_triggered ( bool checked = false );

        //!
        //! Event handler that is called when the play action has been toggled.
        //!
        //! \param checked The state of the play action.
        //!
        void on_m_playAction_toggled ( bool checked );

        //!
        //! Event handler that is called when the realtime action has been toggled.
        //!
        //! \param checked The state of the action.
        //!
        void on_m_realtimeAction_toggled ( bool checked );

        //!
        //! Event handler that is called when the next frame action has been
        //! triggered.
        //!
        //! \param checked The state of the action.
        //!
        void on_m_nextFrameAction_triggered ( bool checked = false );

        //!
        //! Event handler that is called when the next key action has been
        //! triggered.
        //!
        //! \param checked The state of the action.
        //!
        void on_m_nextKeyAction_triggered ( bool checked = false );

        //!
        //! Event handler that is called when the out frame action has been
        //! triggered.
        //!
        //! \param checked The state of the action.
        //!
        void on_m_outFrameAction_triggered ( bool checked = false );

private: // functions

    //!
    //! Initializes the actions that are contained in the scene model.
    //!
    void initializeActions ();

    //!
    //! Enables or disables the timeline actions according to the current frame
    //! index.
    //!
    void updateActions ();

	//!
	//! Deletes a connection between parameters.
	//!
	//! \param name The of the connection to be deleted
	//! \param deleteGraphicItem Switch to determine if the connection graphics
	//! item should be deleted together with the connection or not.
	//!
    void deleteConnection (const QString &name, bool deleteGraphicItem = false );

	//!
	//! Creates the connection graphics item for the visual representation of a connection.
	//!
	//! \param connection The connection for which the graphics item should be generated.
	//!
	void createConnectionItem (Connection *connection); 

    //!
    //! Decodes the given position for node graphics items from a COLLADA scene
    //! file.
    //!
    //! \param position The position value to decode into a Qt position.
    //! \param nodeName The name of the node whose position value to decode (used for log messages only).
    //! \return The position corresponding to the given position value.
    //!
    QPointF decodePosition ( std::string position, const QString &nodeName );

    //!
    //! Creates COLLADA elements corresponding to the parameters contained in
    //! the given parameter group and adds them to the given parent element.
    //!
    //! \param parameterGroup The parameter group to create COLLADA elements from.
    //! \param parentElement The element under which to create the COLLADA elements representing the parameter group.
    //!
    void createDaeElements ( ParameterGroup *parameterGroup, daeElement *parentElement ) const;

	//!
    //! Creates COLLADA extra elements corresponding to the given node element. 
    //!
    //! \param The corresponding node element.
    //! \return The created, frapper specific extra element.
    //!
	daeElement *createDaeExtraElement ( daeElement *nodeElement ) const;

	//!
    //! Converts a collade DOM- and all child elements into a Qt DOM element. 
    //!
    //! \param element The corresponding collada element.
    //! \return The created, frapper specific Qt DOM element.
    //!
	void convertToQDom ( daeElement *parameterElement, QDomElement& domElement ) const;

	//!
    //! Creates COLLADA Techniques corresponding to the nodes or node groups. 
    //!
    //! \param node The node to create COLLADA techniques from.
    //! \param techniqueElement The node element under which to create the COLLADA techniques.
    //!
	void createDaeNodeElements (Node *node, daeElement *techniqueElement, const QPointF &position) const;

	//!
    //! Creates COLLADA elements corresponding to the parameters. 
    //!
    //! \param node The node to create COLLADA elements from.
    //! \param parameterGroup The parameter group to create COLLADA elements from.
    //! \param parametersElement The element under which to create the COLLADA node elements.
    //!
    void createParameters(Node *node, ParameterGroup *parameterGroup, daeElement *parametersElement) const;

	//!
	//! Calls the change function of the parameter.
	//!
	//! \param parameter A pointer to the parameter.
	//!
	void callChangeFunction(Parameter *parameter) const;

	//!
	//! Returns the NodeGroupGraphicsItem containing the node with the given name.
	//!
	//! \return The NodeGroupGraphicsItem containing the node with the given name.
	//!
	NodeGroupGraphicsItem *findGroupByNodeName(const QString &nodeName) const;

	//!
	//! Deletes the nodes graphics irems.
	//!
	//! \param node The node which graphics items should be deleted.
	//! 
	void deleteNodeGraphicsItems(const QString &nodeName);

private: // data
	
	//!
    //! The number of newly created nodes to be used in the name of new nodes.
    //!
    int m_newNodeIndex;

	//!
    //! The number of frames to advance the animation by each play back step.
    //!
    int m_frameStep;

	//!
    //! The ID of the scene timer.
    //!
    int m_timerId;

    //!
    //! The node model that holds all scene objects like meshes, cameras and
    //! lights.
    //!
    NodeModel *m_nodeModel;

    //!
    //! The selection model to use for synchronizing selections in all views.
    //!
    QItemSelectionModel *m_selectionModel;

    //!
    //! The graphics scene to use for all network views.
    //!
    QGraphicsScene *m_graphicsScene;

	//!
    //! The parameter containing the index of the current frame and the global
    //! frame range.
    //!
    NumberParameter *m_frameParameter;

    //!
    //! The parameter defining the indices of the in and out frames.
    //!
    NumberParameter *m_frameRangeParameter;

    //!
    //! The parameter containing the number of frames per second.
    //!
    NumberParameter *m_fpsParameter;

	//!
    //! The parent scene node for all scene objects to be displayed in
    //! viewports.
    //!
    //! \todo Stefan: [Stages] To be replaced by a root scene node per stage.
    //!
    Ogre::SceneNode *m_sceneRoot;

	//!
    //! The last timestamp.
    //!
    QTime m_lastTimestamp;

	//!
    //! The position to use for new nodes.
    //!
    QPointF m_newNodePosition;

	//!
    //! The name of the current scene.
    //!
    //!
    QString m_name;

	//!
    //! The list of camera nodes currently set up in the scene.
    //!
    QList<Node *> m_cameraNodes;

    //!
    //! The node that currently has the view flag set.
    //!
    //! \todo Stefan: [Stages] To be replaced by a view node per stage.
    //!
    QList<ViewNode *> m_viewNodeList;

	//!
    //! The Map of connections currently set up in the scene.
    //!
    QHash<unsigned int, Connection *> m_connectionMap;

    //!
    //! A map for referencing graphics items by name.
    //!
    QHash<QString, QGraphicsItem *> m_graphicsItemMap;

    //!
    //! A map for referencing graphics items (connections) by name.
    //!
    QHash<QString, QGraphicsItem *> m_connectionGraphicsItemMap;


private: // actions

	//!
    //! The action duplicating a node.
    //!
	QAction *m_duplicateNodeAction;

    //!
    //! The action for jumping to the in frame of the animation.
    //!
    QAction *m_inFrameAction;

    //!
    //! The action for jumping to the previous key of the selected channel.
    //!
    QAction *m_previousKeyAction;

    //!
    //! The action for selecting the previous frame in the animation.
    //!
    QAction *m_previousFrameAction;

    //!
    //! The action for starting and stopping animation playback.
    //!
    QAction *m_playAction;

    //!
    //! The action for starting and stopping realtime animation playback.
    //!
    QAction *m_realtimeAction;

    //!
    //! The action for selecting the next frame in the animation.
    //!
    QAction *m_nextFrameAction;

    //!
    //! The action for jumping to the next key of the selected channel.
    //!
    QAction *m_nextKeyAction;

    //!
    //! The action for jumping to the out frame of the animation.
    //!
    QAction *m_outFrameAction;

    //!
    //! The action representing a separator for separating the playback options.
    //!
    QAction *m_separatorAction;

    //!
    //! The action for grouping playback options.
    //!
    QAction *m_playOptionsAction;

    //!
    //! The action representing the play looping option.
    //!
    QAction *m_playLoopingAction;

    //!
    //! The action representing the play once option.
    //!
    QAction *m_playOnceAction;

};

} // end namespace Frapper

#endif
