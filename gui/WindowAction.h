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
//! \file "WindowAction.h"
//! \brief Header file for WindowAction class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef WINDOWACTION_H
#define WINDOWACTION_H

#include "FrapperPrerequisites.h"
#include <QAction>
#include "Window.h"


namespace Frapper {

//!
//! Class for actions that represent one of the application's windows.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph WindowAction {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     WindowAction [label="WindowAction",fillcolor="grey75"];
//!     QAction -> WindowAction;
//!     QAction [label="QAction",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qaction.html"];
//!     QObject -> QAction;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_GUI_EXPORT WindowAction : public QAction
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the WindowAction class.
    //!
    //! \param window The window that the action represents.
    //! \param index The index of the action in the window menu.
    //!
    WindowAction ( Window *window, int index );

public: // functions

    //!
    //! Returns the window that the action represents.
    //!
    //! \return The window that the action represents.
    //!
    Window * getWindow ();

    //!
    //! Sets the text of the action.
    //!
    //! \param text The new text for the action.
    //!
    void setText ( const QString &text );

public slots: //

    //!
    //! Slot that is called when the action is triggered.
    //! Activates the window that the action represents.
    //!
    void activateWindow ();

private: // data

    //!
    //! The window that the action represents.
    //!
    Window *m_window;

    //!
    //! The index of the action in the window menu.
    //!
    int m_index;

};

} // end namespace Frapper

#endif
