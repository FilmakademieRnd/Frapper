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
//! \file "WindowsDialog.h"
//! \brief Header file for WindowsDialog class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef WINDOWSDIALOG_H
#define WINDOWSDIALOG_H

#include "FrapperPrerequisites.h"
#include "ui_WindowsDialog.h"
#include <QDialog>
#include <QStandardItemModel>
#include <QItemSelectionModel>

namespace Frapper {


//!
//! Dialog for displaying the list of the application's windows.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph WindowsDialog {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     WindowsDialog [label="WindowsDialog",fillcolor="grey75"];
//!     QDialog -> WindowsDialog;
//!     QDialog [label="QDialog",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qdialog.html"];
//!     UiWindowsDialog -> WindowsDialog [color="darkgreen"];
//!     UiWindowsDialog [label="Ui::WindowsDialog",URL="class_ui_1_1_windows_dialog.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_GUI_EXPORT WindowsDialog : public QDialog, protected Ui::WindowsDialog
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the WindowsDialog class.
    //!
    //! \param windowListModel The model that contains the list of the application's windows.
    //! \param parent A parent widget the created instance will be child of.
    //!
    WindowsDialog ( QStandardItemModel *windowListModel, QWidget *parent = 0 );

    //!
    //! Destructor of the WindowsDialog class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~WindowsDialog ();

public slots: //

    //!
    //! Enables the close button according to the current selection in the list of
    //! windows.
    //!
    //! \param current The index of the currently selected item.
    //! \param previous The index of the previously selected item.
    //!
    void currentWindowChanged ( const QModelIndex &current, const QModelIndex &previous );

    //!
    //! Activates the currently selected window.
    //!
    void activateSelectedWindow ();

    //!
    //! Closes the currently selected window.
    //!
    void closeSelectedWindow ();

private: // data

    //!
    //! The model that contains the list of the application's windows.
    //!
    QStandardItemModel *m_windowListModel;

    //!
    //! The selection model that works on the list of the application's windows.
    //!
    QItemSelectionModel *m_selectionModel;

};

} // end namespace Frapper

#endif