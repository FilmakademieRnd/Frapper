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
//! \file "DocumentationPanel.h"
//! \brief Header file for DocumentationPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#ifndef DOCUMENTATIONPANEL_H
#define DOCUMENTATIONPANEL_H

#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "ui_DocumentationPanel.h"


namespace Frapper {

//!
//! Class representing a panel for displaying the application's documentation.
//!
class FRAPPER_GUI_EXPORT DocumentationPanel : public ViewPanel, protected Ui::DocumentationPanel
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the DocumentationPanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    DocumentationPanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the DocumentationPanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~DocumentationPanel ();

public: // functions

    //!
    //! Fills the given tool bars with actions for the documentation panel.
    //!
    //! \param mainToolBar The main tool bar to fill with actions.
    //! \param panelToolBar The panel tool bar to fill with actions.
    //!
    virtual void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

public slots:
    
    //!
    //! Sets the document to be displayed in the panel.
    //!
    //! \param filename Path of the document.
    //!
    void setDocument ( const QString &filename );

protected: // event handlers

    //!
    //! Event handler that is called when the widget receives keyboard focus.
    //!
    //! \param event The object containing details about the event.
    //!
    void focusInEvent ( QFocusEvent *event );

};

} // end namespace Frapper

#endif
