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
//! \file "ViewportPanel.h"
//! \brief Header file for ViewportPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       17.06.2009 (last updated)
//!

#ifndef VIEWPORTPANEL_H
#define VIEWPORTPANEL_H

#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "ViewportWidget.h"
#include "ui_ViewportPanel.h"
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>


namespace Frapper {

//!
//! Class for a panel that contains a 3D viewport widget.
//!
class FRAPPER_GUI_EXPORT ViewportPanel : public ViewPanel, protected Ui::ViewportPanel
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the ViewportPanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    ViewportPanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the ViewportPanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ViewportPanel ();

public: // functions

    //!
    //! Fills the given tool bars in a panel frame with actions for the panel.
    //!
    //! \param mainToolBar The main tool bar to fill with actions.
    //! \param panelToolBar The panel tool bar to fill with actions.
    //!
    virtual void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

    //!
    //! Returns the viewport widget that is used for displaying the scene.
    //!
    //! \return the viewport widget that is used for displaying the scene.
    //!
    ViewportWidget * getViewportWidget () const;

    //!
    //! Sets the panel parameters. For load and save functionality.
    //! 
    void setPanelParameters ( const QMap<QString, QString> &parameterMap );

public slots: //

    ////!
    ////! Adds a new entry for the camera with the given name to the list of
    ////! available cameras.
    ////!
    ////! \param cameraName The name of the camera to add to the list of available cameras.
    ////!
    //void addCamera ( const QString &cameraName );

    ////!
    ////! Removes the entry for the camera with the given name from the list of
    ////! available cameras.
    ////!
    ////! \param cameraName The name of the camera to remove from the list of available cameras.
    ////!
    //void removeCamera ( const QString &cameraName );

    //!
    //! Updates the camera drop down menu.
    //!
    void updateCameras ( const QList<Node *> &cameraNodes, const QString &cameraName );

signals: //

    //!
    //! Signal that is emitted when the fullscreen mode was activated.
    //!
    void fullscreenEnabled ();

    //!
    //! Signal that is emitted when a new camera should be created initialized
    //! according to the given viewing parameters.
    //!
    //! \param viewingParameters The viewing parameters to use for the new camera.
    //!
    void newCameraRequested ( ViewingParameters *viewingParameters );

    //!
    //! Signal that is emitted when an existing camera should be selected.
    //!
    //! \param cameraName The name of the camera to select.
    //! \param toggle Flag that controls whether to toggle the selection of the camera with the given name.
    //!
    void selectCameraRequested ( const QString &cameraName, bool toggle );

    //!
    //! Signal that is emitted when the viewport's viewing parameters have
    //! changed, a custom camera is selected and the link view to camera option
    //! is enabled.
    //!
    //! \param cameraName The name of the camera to update with the viewing parameters.
    //! \param viewingParameters The viewing parameters to use for the camera with the given name.
    //!
    void updateCameraRequested ( const QString &cameraName, ViewingParameters *viewingParameters );

    //!
    //! Signal that is emitted when an existing camera should be deleted.
    //!
    //! \param cameraName The name of the camera to delete.
    //!
    void deleteCameraRequested ( const QString &cameraName );

    //!
    //! Signal that is emitted when an existing camera's viewing parameters
    //! should be applied to the viewport's viewing parameters.
    //!
    //! \param cameraName The name of the camera to use.
    //! \param viewingParameters The viewing parameters to modify.
    //!
    void applyCameraRequested ( const QString &cameraName, ViewingParameters *viewingParameters );

    //!
    //! Requests to update the list of cameras in the scene.
    //! 
    void requestCameraList ();

    //!
    //! Requests a redraw.
    //! 
    void triggerRebuild ();

protected: // events

    //!
    //! Handles key press events for the widget.
    //!
    //! \param event The description of the key event.
    //!
    void keyPressEvent ( QKeyEvent *event );

    //!
    //! Handles key release events for the widget.
    //!
    //! \param event The description of the key event.
    //!
    void keyReleaseEvent ( QKeyEvent *event );

protected slots: //

    //!
    //! Toggles displaying the viewport widget in fullscreen mode.
    //!
    //! \param enable Flag to enable or disable the fullscreen mode for the viewport widget.
    //!
    void toggleFullscreen ( bool enable );

private slots: //

    //!
    //! Creates a new camera node with the current viewing parameters.
    //!
    //! \param checked The action's checked state.
    //!
    void on_ui_addCameraAction_triggered ( bool checked = false );

    //!
    //! Requests an update of the currently selected camera if the action is
    //! currently checked.
    //!
    //! \param checked The action's checked state.
    //!
    void on_ui_linkViewAction_toggled ( bool checked );

    //!
    //! Requests the selection of an existing camera node.
    //!
    //! \param checked The action's checked state.
    //!
    void on_ui_selectCameraAction_triggered ( bool checked = false );

    //!
    //! Requests the deletion of an existing camera node.
    //!
    //! \param checked The action's checked state.
    //!
    void on_ui_removeCameraAction_triggered ( bool checked = false );

    //!
    //! Sets whether only overlays (no geometry) should be displayed in viewport.
    //!
    //! \param checked The action's checked state.
    //!
    void on_ui_overlayOnlyAction_toggled ( bool checked = false );

	//! Sets whether grid should be displayed in viewport.
	//!
	//! \param checked The action's checked state.
	//!
	void on_ui_gridAction_toggled ( bool checked = false );


	//! Sets whether orientation indicator should be displayed in viewport.
	//!
	//! \param checked The action's checked state.
	//!
	void on_ui_orientationIndicatorAction_toggled ( bool checked = false );

    //!
    //! Selects the camera to use for the viewport.
    //!
    //! \param cameraName The name of the camera to use.
    //!
    void selectCamera ( const QString &cameraName );

    //!
    //! Requests an update of the currently selected camera if the
    //! ui_linkViewAction is currently checked.
    //!
    //! \param viewingParameters The viewing parameters currently set in the viewport.
    //!
    void updateCamera ( ViewingParameters *viewingParameters );

    //!
    //! Selects the stage to display in the viewport corresponding to the
    //! triggered stage action.
    //!
    //! \param triggeredAction The action that was triggered.
    //!
    void selectStage ( /*QAction *triggeredAction*/ );

	//!
    //! Sets new fog configuration.
    //!
    void changeFog ( float value );

	//!
    //! Sets new fog configuration.
    //!
    void changeFog ( int value );

    //!
    //! Changes fog color.
    //!
    void changeFogColor ();

    //!
    //! Changes background color.
    //!
    void changeBackgroundColor ();


private: // data

    //!
    //! The viewport widget to use for displaying the scene.
    //!
    ViewportWidget *m_viewportWidget;

    //!
    //! Flag that states whether fullscreen mode is enabled.
    //!
    bool m_fullscreen;

	//!
    //! Line edit value for fog density.
    //!
    QDoubleSpinBox *m_fogSpinBox;

	//!
    //! Checkbox which enables/disables for in the associated viewport.
    //!
    QCheckBox *m_fogCheckBox;

    //!
    //! Color button for choosing fog color.
    //!
    QPushButton *m_fogColorButton;

    //!
    //! Color button for choosing background color.
    //!
    QPushButton *m_backgroundColorButton;

    //!
    //! The group that all the stage actions will be added to.
    //!
    QActionGroup *m_stageActionGroup;

    //!
    //! Camera selection group box.
    //!
    QComboBox *m_cameraComboBox;

    //!
    //! Flag that states whether the user has triggered the ui_addCameraAction.
    //!
    bool m_cameraCreator;

};

} // end namespace Frapper

#endif
