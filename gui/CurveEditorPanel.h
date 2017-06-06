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
//! \file "CurveEditorPanel.h"
//! \brief Header file for CurveEditorPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       24.08.2010 (last updated)
//!

#ifndef CURVEEDITORPANEL_H
#define CURVEEDITORPANEL_H

#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "ui_CurveEditorPanel.h"
#include "CurveEditorGraphicsView.h"
#include "Node.h"
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>


namespace Frapper {

//!
//! Class for a panel that contains a curve editor widget for displaying
//! number parameters
//!
class FRAPPER_GUI_EXPORT CurveEditorPanel : public ViewPanel, protected Ui::CurveEditorPanel
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the CurveEditorPanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    CurveEditorPanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the CurveEditorPanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CurveEditorPanel ();

public: // functions

    //!
    //! Fills the given tool bars with actions for the curve editor panel.
    //!
    //! \param mainToolBar The main tool bar to fill with actions.
    //! \param panelToolBar The panel tool bar to fill with actions.
    //!
    virtual void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

public slots: //

    //!
    //! Slot that is called when the selection in the scene model has changed.
    //!
    //! \param objectsSelected Flag that states whether objects in the scene are selected.
    //!
    void updateTree ( bool objectsSelected );

    //!
    //! Initializes the Selection Tree
    //!
    //! \param node The node whose animated parameters to display in the curve editor.
    //!
    void buildTree ( Node *node );

    //!
    //! Displays the curves of the given node in the curve editor.
    //!
    //!
    void showCurves ();

    //!
    //! Sets the index of the current frame in the scene's time.
    //!
    //! \param index The new index of the current frame in the scene's time.
    //!
    void setCurrentFrame ( const int index );

	//!
	//! Sets the index of the in frame in the scene's time.
	//!
	//! \param index The new index of the in frame in the scene's time.
	//!
	void setInFrame ( const int index );

	//!
	//! Sets the index of the out frame in the scene's time.
	//!
	//! \param index The new index of the out frame in the scene's time.
	//!
	void setOutFrame ( const int index );

	//!
    //! Sets the value of the selected Keys.
    //!
	void changeKeyValues ();

	//!
	//! Scales the value of the selected Keys.
	//!
	void scaleKeyValues ();

	//!
	//! Scales the values of the shown curve to a maximum of one.
	//!
	void normalizeKeyValues ();

	//!
    //! Mutes/unmutes the in treeView selected Parameters.
    //!
    void muteSelectedParameterGroups ();

	//!
	//! Mutes/unmutes all treeView Parameters.
	//!
	void muteAllParameterGroups ();

	//!
	//! Unmutes all treeView Parameters.
	//!
	void unmuteAllParameterGroups ();

	//!
	//! Updates color and icon of selected treeView Items and chids.
	//!
	void updateItems();

	//!
	//! Changes the key type.
	//!
	void keyTypeChanged ( int index );

	//!
	//! Changes only the combo box literal.
	//!
	void selectedKeyTypeChanged( int index );

signals:
	//!
	//! Signal that is emitted when a drag event is emited.
	//!
	void drag();


private: // functions

	//!
	//! Changes color and icon of selected treeView Items and chids.
	//!
	void muteItems(QTreeWidgetItem  *item);

	//!
	//! Changes color and icon of selected treeView Items and chids.
	//!
	void unmuteItems(QTreeWidgetItem  *item);

	//!
	//! Deletes the selected keys of the selected number parameter.
	//!
	void deleteSelectedKeys();

	//!
	//! Inserts a new key in the selected number parameter
	//! at the current timeline position.
	//!
	void insertKey();

    //!
    //! Fills the tree view with node data groups.
    //!
    //! \param root the root parameter group of the selected node.
    //!
    void fillTree( ParameterGroup *rootData, QTreeWidgetItem *rootItem );


protected: // events

    //!
    //! Handles key press events for the widget.
    //!
    //! \param event The description of the key event.
    //!
    void keyPressEvent ( QKeyEvent *event );


private: // data

	//!
	//! The Current key type.
	//!
	Key::KeyType m_keyType;

	//!
	//! The timeline position index.
	//!
	int m_timelinePosition;

	//!
	//! The currently selected node.
	//!
	Node *m_node;

	//!
    //! Line edit value for key value overwrite.
    //!
    QDoubleSpinBox *m_normalizeSpinBox;

	//!
    //! Line edit value for key value overwrite.
    //!
    QDoubleSpinBox *m_valueSpinBox;

	//!
    //! Line edit value for key value overwrite.
    //!
    QDoubleSpinBox *m_scaleSpinBox;

	//!
    //! Checkbox which enables/disables view flag for enabled/disabled parameters.
    //!
    QCheckBox *m_curveCheckBox;

	//!
    //! Combobox to choose the key type.
    //!
    QComboBox *m_typeComboBox;

    //!
    //! The graphics view for drawing the curves.
    //!
    CurveEditorGraphicsView *m_curveEditorGraphicsView;

    //!
    //! The layout for horizontal arranging the tree an drawing region.
    //!
    QHBoxLayout *m_hboxLayout;

    //!
    //! The tree widget visualizing the databases.
    //!
    QTreeWidget *m_dataTree;

	//!
	//! The mute icon for the data tree
	//!
	QIcon m_muteIcon;
};

} // end namespace Frapper

#endif