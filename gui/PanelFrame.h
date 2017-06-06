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
//! \file "PanelFrame.h"
//! \brief Header file for PanelFrame class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#ifndef PANELFRAME_H
#define PANELFRAME_H

#include "FrapperPrerequisites.h"
#include "Panel.h"
#include "ViewPanel.h"
#include "ui_PanelFrame.h"
#include <QFrame>
#include <QToolBar>
#include <QLabel>


namespace Frapper {

//!
//! Class for panel widgets to contain view and other widgets.
//!
//! When referencing the Panel class in this class the namespaceFrapper:: has to be
//! used in order to differentiate betweenFrapper::Panel and
//! <a href="http://doc.qtsoftware.com/4.5/qframe.html#Shape-enum" target="_blank">QFrame::Shape::Panel</a>.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph PanelFrame {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     PanelFrame [label="PanelFrame",fillcolor="grey75"];
//!     QFrame -> PanelFrame;
//!     QFrame [label="QFrame",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qframe.html"];
//!     QWidget -> QFrame;
//!     QWidget [label="QWidget",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qwidget.html"];
//!     QObject -> QWidget;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!     QPaintDevice -> QWidget;
//!     QPaintDevice [label="QPaintDevice",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qpaintdevice.html"];
//!     UiPanelFrame -> PanelFrame [color="darkgreen"];
//!     UiPanelFrame [label="Ui::PanelFrame",URL="class_ui_1_1_panel_frame.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_GUI_EXPORT PanelFrame : public QFrame, protected Ui::PanelFrame
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PanelFrame class.
    //!
    //! \param panelType The type of panel to create.
    //! \param parent A widget the created instance will be child of.
    //! \param flags Extra widget options.
    //! \param showToolBars Flag to control whether to display tool bars in the panel frame.
    //!
    PanelFrame (Frapper::Panel::Type panelType, QWidget *parent = 0, Qt::WindowFlags flags = 0, bool showToolBars = true );

    //!
    //! Destructor of the PanelFrame class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PanelFrame ();

public: // functions

    //!
    //! Returns the the panel.
    //!
    //! \return The panel.
    //!
   Frapper::Panel * getPanel ();

    //!
    //! Returns the type of the panel.
    //!
    //! \return The type of the panel.
    //!
    Panel::Type getPanelType ();

    //!
    //! Toggles the display of tool bars in the panel frame.
    //!
    //! \param visible Flag to control whether to display tool bars in the panel frame.
    //!
    void toggleToolBars ( bool visible );

signals: //

    //!
    //! Signal that is emitted when the type of the panel has changed.
    //!
    //! \param panelTypeName The name of the new panel type.
    //!
    void panelTypeChanged ( const QString &panelTypeName );

    //!
    //! Signal that is emitted when the given panel frame should be duplicated.
    //!
    //! \param panelFrame The panel frame to duplicate.
    //!
    void duplicateRequested ( PanelFrame *panelFrame );

    //!
    //! Signal that is emitted when the given panel should be extracted to a
    //! new window.
    //!
    //! \param panelFrame The panel frame to extract to a new window.
    //!
    void extractRequested ( PanelFrame *panelFrame );

    //!
    //! Signal that is emitted when the given panel should be closed.
    //!
    //! \param panelFrame The panel frame to close.
    //!
    void closeRequested ( PanelFrame *panelFrame );

    //!
    //! Signal that is emitted when a ViewPanel-derived widget has been created
    //! for the panel frame.
    //!
    //! \param viewPanel The ViewPanel-derived widget that has been created for the panel frame.
    //!
    void viewPanelCreated ( ViewPanel *viewPanel );

protected: // event handlers

    //!
    //! Event handler that is called when the widget receives keyboard focus.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void focusInEvent ( QFocusEvent *event );

    //!
    //! Event handler that is called when the widget is resized.
    //!
    //! \param event The object containing details about the event.
    //!
    virtual void resizeEvent ( QResizeEvent *event );

private: // functions

    //!
    //! Sets the panel type of the panel frame to the given value.
    //!
    //! \param panelType The new panel type for the panel frame.
    //!
    void setPanelType (Frapper::Panel::Type panelType );

    //!
    //! Splits the panel frame according to the given orientation.
    //!
    //! \param orientation The orientation in which to split the panel frame.
    //!
    void split ( Qt::Orientation orientation );

    //!
    //! Maximizes the given widget inside its parent widget.
    //!
    //! The widget can be a panel frame or a splitter widget.
    //!
    //! \param widget The widget to maximize.
    //!
    void maximizeWidget ( QWidget *widget );

    //!
    //! Minimizes the given widget inside its parent widget.
    //!
    //! The widget can be a panel frame or a splitter widget.
    //!
    //! \param widget The widget to minimize.
    //!
    void minimizeWidget ( QWidget *widget );

private slots: //

    //!
    //! Fills the panel frame with a panel according to the given panel type
    //! index.
    //!
    //! \param index The new panel type index.
    //!
    void on_ui_panelTypeComboBox_currentIndexChanged ( int index );

    //!
    //! Splits the panel frame horizontally.
    //!
    //! Convenience function that calls split(Qt::Horizontal).
    //!
    void on_ui_splitHorizontallyAction_triggered ();

    //!
    //! Splits the panel frame vertically.
    //!
    //! Convenience function that calls split(Qt::Vertical).
    //!
    void on_ui_splitVerticallyAction_triggered ();

    //!
    //! Maximizes the panel frame.
    //!
    void on_ui_maximizeAction_triggered ();

    //!
    //! Maximizes the panel frame in its row.
    //!
    void on_ui_maximizeHorizontallyAction_triggered ();

    //!
    //! Maximizes the panel frame in its column.
    //!
    void on_ui_maximizeVerticallyAction_triggered ();

    //!
    //! Minimizes the panel frame.
    //!
    void on_ui_minimizeAction_triggered ();

    //!
    //! Duplicates the panel frame by creating a new window.
    //!
    //! Does not duplicate the panel itself, but notifies connected objects
    //! that the panel should be duplicated.
    //!
    void on_ui_duplicateAction_triggered ();

    //!
    //! Extracts the panel frame to a new window.
    //!
    //! Does not extract the panel frame itself, but notifies connected objects
    //! that the panel frame should be extracted.
    //!
    void on_ui_extractAction_triggered ();

    //!
    //! Closes the panel frame.
    //!
    //! Does not close the panel frame itself, but notifies connected objects
    //! that the panel frame should be closed.
    //!
    void on_ui_closeAction_triggered ();

private: // data

    //!
    //! The panel frame's main tool bar.
    //!
    QToolBar *m_mainToolBar;

    //!
    //! The panel frame's panel tool bar.
    //!
    QToolBar *m_panelToolBar;

    //!
    //! The type of the panel.
    //!
   Frapper::Panel::Type m_panelType;

    //!
    //! The panel that will be contained in the panel frame's main layout.
    //!
   Frapper::Panel *m_panel;

    //!
    //! The label that will becontained in the panel frame's main layout.
    //!
    QLabel *m_label;

};

} // end namespace Frapper

#endif
