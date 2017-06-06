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
//! \file "Panel.h"
//! \brief Header file for Panel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       28.04.2009 (last updated)
//!

#ifndef PANEL_H
#define PANEL_H

#include "FrapperPrerequisites.h"
#include <QFrame>
#include <QToolBar>
#include "InstanceCounterMacros.h"
#include "NodeModel.h"
#include "SceneModel.h"


namespace Frapper {

//!
//! Base class for all panels to be contained in panel frames.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph Panel {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     Panel [label="Panel",fillcolor="grey75"];
//!     QFrame -> Panel;
//!     QFrame [label="QFrame",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qframe.html"];
//!     QWidget -> QFrame;
//!     QWidget [label="QWidget",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qwidget.html"];
//!     QObject -> QWidget;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!     QPaintDevice -> QWidget;
//!     QPaintDevice [label="QPaintDevice",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qpaintdevice.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_GUI_EXPORT Panel : public QFrame
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // nested enumerations

    //!
    //! Nested enumeration of panel types.
    //!
    enum Type {
        T_Uninitialized = -1, //!< A value indicating that no panel type has been set yet.
        T_Viewport, //!< Value for panel frames containing a ViewportPanel.
        T_NetworkEditor, //!< Value for panel frames containing a NetworkPanel.
        T_HierarchyEditor, //!< Value for panel frames containing a HierarchyPanel.
        T_ParameterEditor, //!< Value for panel frames containing a ParameterPanel.
        T_CurveEditor, //!< Value for panel frames containing a CurveEditorPanel.
        T_Timeline, //!< Value for panel frames containing a TimelinePanel.
		T_History, //!< Value for panel frames containing a HistoryPanel.
        T_Log, //!< Value for panel frames containing a LogPanel.
        T_Documentation, //!< Value for panel frames containing a DocumentationPanel.
        T_NumTypes //!< The number of panel types available.
    };

public: // static functions

    //!
    //! Returns the name of the given panel type.
    //!
    //! \param panelType The panel type for which to return the name.
    //! \return The name of the given panel type.
    //!
    static QString getTypeName ( Type panelType );

    //!
    //! Returns the icon name for the given panel type.
    //!
    //! \param panelType The panel type for which to return the icon name.
    //! \return The name of the icon for the given panel type, or an empty string if no icon for the panel type is available.
    //!
    static QString getTypeIconName ( Type panelType );

public: // constructors and destructors

    //!
    //! Constructor of the Panel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    Panel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the Panel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~Panel ();

public: // functions

    //!
    //! Fills the given tool bars in a panel frame with actions for the panel.
    //!
    //! \param mainToolBar The main tool bar to fill with actions.
    //! \param panelToolBar The panel tool bar to fill with actions.
    //!
    virtual void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

    //!
    //! Gets the panel parameters. For load and save functionality.
    //! 
    QMap<QString, QString> getPanelParameters ();

    //!
    //! Sets the panel parameters. For load and save functionality.
    //! 
    virtual void setPanelParameters ( const QMap<QString, QString> &parameterMap );

	//!
	//! Connects the panel with the scene.
	//!
	virtual void registerControl(NodeModel *nodeModel, SceneModel *sceneModel);

protected: // variables

    //!
    //! Map with default values. For load and save functionality.
    //!
    QMap<QString, QString> m_parameterMap;

};

} // end namespace Frapper

#endif
