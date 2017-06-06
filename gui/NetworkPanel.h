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
//! \file "NetworkPanel.h"
//! \brief Header file for NetworkPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#ifndef NETWORKPANEL_H
#define NETWORKPANEL_H

#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "NetworkGraphicsView.h"
#include "ui_NetworkPanel.h"


namespace Frapper {

//!
//! Class for a panel that contains a network editor widget using Qt's
//! graphics-view architecture.
//!
class FRAPPER_GUI_EXPORT NetworkPanel : public ViewPanel, protected Ui::NetworkPanel
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the NetworkPanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    NetworkPanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the NetworkPanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~NetworkPanel ();

public: // functions

    //!
    //! Fills the given tool bars in a panel frame with actions for the panel.
    //!
    //! \param mainToolBar The main tool bar to fill with actions.
    //! \param panelToolBar The panel tool bar to fill with actions.
    //!
    virtual void fillToolBars ( QToolBar *mainToolBar, QToolBar *panelToolBar );

    //!
    //! Returns the graphics view that is used to display the model's graphics
    //! scene.
    //!
    //! \return The graphics view that is used to display the model's graphics scene.
    //!
    NetworkGraphicsView * getNetworkGraphicsView ();

protected slots: // events

    //!
    //! Handles key press events for the widget.
    //!
    //! \param event The description of the key event.
    //!
    void keyPressEvent ( QKeyEvent *event );

private: // data

    //!
    //! The graphics view widget to use for displaying the nodes.
    //!
    NetworkGraphicsView *m_networkGraphicsView;

};

} // end namespace Frapper

#endif
