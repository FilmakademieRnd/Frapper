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
//! \file "DocumentationPanel.cpp"
//! \brief Implementation file for DocumentationPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#include "DocumentationPanel.h"
#include <QStyle>
#include <QtCore/QFileInfo>

#define DOCUMENTATION_MAIN_PAGE "index.html"


namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the DocumentationPanel class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
DocumentationPanel::DocumentationPanel ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
ViewPanel(ViewPanel::T_Documentation, parent, flags)
{
    setupUi(this);

    connect(ui_backwardAction, SIGNAL(triggered()), ui_textBrowser, SLOT(backward()));
    connect(ui_forwardAction, SIGNAL(triggered()), ui_textBrowser, SLOT(forward()));
    connect(ui_homeAction, SIGNAL(triggered()), ui_textBrowser, SLOT(home()));

    connect(ui_textBrowser, SIGNAL(backwardAvailable(bool)), ui_backwardAction, SLOT(setEnabled(bool)));
    connect(ui_textBrowser, SIGNAL(forwardAvailable(bool)), ui_forwardAction, SLOT(setEnabled(bool)));

	QString sheet = QString::fromLatin1("a { text-decoration: underline; color: #ffa02f }");
	ui_textBrowser->document()->setDefaultStyleSheet(sheet);

    ui_textBrowser->setSource(QUrl::fromLocalFile(QFileInfo(DOCUMENTATION_MAIN_PAGE).absoluteFilePath()));
    ui_textBrowser->setOpenExternalLinks(true);

}


//!
//! Destructor of the DocumentationPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
DocumentationPanel::~DocumentationPanel ()
{
}


///
/// Public Functions
///


//!
//! Fills the given tool bars with actions for the documentation panel.
//!
//! \param mainToolBar The main tool bar to fill with actions.
//! \param panelToolBar The panel tool bar to fill with actions.
//!
void DocumentationPanel::fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar )
{
    mainToolBar->addAction(ui_backwardAction);
    mainToolBar->addAction(ui_forwardAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(ui_homeAction);
}

///
/// Private Slots
///

//!
//! Sets the document to be displayed in the panel.
//!
//! \param filename Path of the document.
//!
void DocumentationPanel::setDocument ( const QString &filename )
{
    QString htmlFilename = filename;
    htmlFilename.replace(".dae", ".html");
    if (QFile::exists(htmlFilename))
        ui_textBrowser->setSource(QUrl::fromLocalFile(QFileInfo(htmlFilename).absoluteFilePath()));
    else
        ui_textBrowser->setSource(QUrl::fromLocalFile(QFileInfo(DOCUMENTATION_MAIN_PAGE).absoluteFilePath()));
}

///
/// Protected Event Handlers
///


//!
//! Event handler that is called when the widget receives keyboard focus.
//!
//! \param event The object containing details about the event.
//!
void DocumentationPanel::focusInEvent ( QFocusEvent *event )
{
    ui_textBrowser->setFocus();

    QFrame::focusInEvent(event);
}

} // end namespace Frapper