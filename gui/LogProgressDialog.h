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
//! \file "LogProgressDialog.cpp"
//!
//! \brief Implementation file for LogProgressDialog class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//!
//! \version    1.0
//! \date       03.09.2013 (last updated)
//!


#ifndef LogProgressDialog_H
#define LogProgressDialog_H

#include "FrapperPrerequisites.h"
#include "ui_LogProgressDialog.h"
#include "CopyHandler.h"
#include <QDialog>

namespace Frapper {

//!
//! Class representing a panel for displaying log messages.
//!
class FRAPPER_GUI_EXPORT LogProgressDialog : public QDialog, public CopyHandler, protected Ui::LogProgressDialog
{

public: // constructors and destructors

    //!
    //! Constructor of the LogProgressDialog class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    LogProgressDialog ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the LogProgressDialog class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~LogProgressDialog ();

public: // functions

    //!
    //! Copies the currently selected (or all) log messages to the clipboard.
    //!
    virtual void copy ();

	QProgressBar* getProgressBar() { return progressBar; }

	QTableView* getTableView() { return tableView; }

protected: // events

private slots: //
	
private: // functions

private: // data

};

} // end namespace Frapper

#endif
