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
//! \file "WindowsDialog.cpp"
//! \brief Implementation file for WindowsDialog class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "WindowsDialog.h"
#include "WindowItem.h"


namespace Frapper {

//!
//! Constructor of the WindowsDialog class.
//!
//! \param windowListModel The model that contains the list of the application's windows.
//! \param parent A parent widget the created instance will be child of.
//!
WindowsDialog::WindowsDialog ( QStandardItemModel *windowListModel, QWidget *parent /* = 0 */ )
    : QDialog(parent)
{
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    // fill list of windows with window titles
    m_windowListModel = windowListModel;
    m_selectionModel = new QItemSelectionModel(m_windowListModel);
    windowListView->setModel(m_windowListModel);
    windowListView->setSelectionModel(m_selectionModel);

    // build connections
    connect(m_selectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentWindowChanged(const QModelIndex &, const QModelIndex &)));
    connect(windowListView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(activateSelectedWindow()));
    connect(activateWindowButton, SIGNAL(clicked()), this, SLOT(activateSelectedWindow()));
    connect(closeWindowButton, SIGNAL(clicked()), this, SLOT(closeSelectedWindow()));

    // check if given parent widget is a window
    if (parent) {
        Window *window = qobject_cast<Window *>(parent);
        if (window) {
            // find the window item that represents the parent window
            QStandardItem *rootItem = m_windowListModel->invisibleRootItem();
            WindowItem *parentWindowItem = 0;
            int row = 0;
            while (!parentWindowItem && row < rootItem->rowCount()) {
                WindowItem *windowItem = dynamic_cast<WindowItem *>(rootItem->child(row));
                if (windowItem && windowItem->getWindow() == window)
                    parentWindowItem = windowItem;
                else
                    ++row;
            }
            // if a window item could be found that represents the parent window, select it
            if (parentWindowItem)
                windowListView->setCurrentIndex(parentWindowItem->index());
        }
    }
}


//!
//! Destructor of the WindowsDialog class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
WindowsDialog::~WindowsDialog ()
{
    delete m_selectionModel;
}


///
/// Private Slots
///


//!
//! Enables the close button according to the current selection in the list of
//! windows.
//!
//! \param current The index of the currently selected item.
//! \param previous The index of the previously selected item.
//!
void WindowsDialog::currentWindowChanged ( const QModelIndex &current, const QModelIndex &previous )
{
    activateWindowButton->setEnabled(current.isValid());
    closeWindowButton->setEnabled(current.isValid() && current != m_windowListModel->invisibleRootItem()->child(0)->index());
}


//!
//! Activates the currently selected window.
//!
void WindowsDialog::activateSelectedWindow ()
{
    // close the dialog window
    accept();

    WindowItem *windowItem = dynamic_cast<WindowItem *>(m_windowListModel->itemFromIndex(windowListView->currentIndex()));
    if (windowItem)
        QApplication::setActiveWindow(windowItem->getWindow());
}


//!
//! Closes the currently selected window.
//!
void WindowsDialog::closeSelectedWindow ()
{
    WindowItem *windowItem = dynamic_cast<WindowItem *>(m_windowListModel->itemFromIndex(windowListView->currentIndex()));
    if (windowItem)
        windowItem->getWindow()->close();
}

} // end namespace Frapper