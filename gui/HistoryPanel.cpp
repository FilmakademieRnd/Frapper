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
//! \file "HistoryPanel.h"
//! \brief Implementation file for HistoryPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "HistoryPanel.h"
#include <QtCore/QTime>


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the HistoryPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
HistoryPanel::HistoryPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    Panel(parent, flags)
{
    setupUi(this);

    // TODO: add connections for history actions
    //...
}


//!
//! Destructor of the HistoryPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
HistoryPanel::~HistoryPanel ()
{
}


///
/// Public Functions
///


//!
//! Fills the given tool bars with actions for the history panel.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void HistoryPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
    mainToolBar->addAction(ui_undoAction);
    mainToolBar->addAction(ui_redoAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(ui_clearAction);
}


//!
//! Adds the given text to the history list.
//!
//! \param text The text to add to the history list.
//!
void HistoryPanel::addItem ( const QString &text )
{
    listWidget->addItem(new QListWidgetItem(QString("[%1] %2").arg(QTime::currentTime().toString()).arg(text)));
}

} // end namespace Frapper