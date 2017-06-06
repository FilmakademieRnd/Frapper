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
//! \file "NetworkGraphicsView.h"
//! \brief Header file for NetworkGraphicsView class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       03.05.2009 (last updated)
//!

#ifndef NETWORKGRAPHICSVIEW_H
#define NETWORKGRAPHICSVIEW_H

#include "FrapperPrerequisites.h"
#include "BaseGraphicsView.h"


namespace Frapper {

//!
//! Class representing a graphics view for displaying and editing a network of
//! nodes.
//!
class FRAPPER_GUI_EXPORT NetworkGraphicsView : public BaseGraphicsView
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the NetworkGraphicsView class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //!
    NetworkGraphicsView ( QWidget *parent = 0 );

    //!
    //! Destructor of the NetworkGraphicsView class.
    //!
    ~NetworkGraphicsView ();

public: // functions

    //!
    //! Sets the scene to display in the graphics view.
    //!
    //! \param scene The scene to display in the graphics view.
    //!
    void setScene ( QGraphicsScene *scene );

public slots: //

    //!
    //! Sets the pin display mode for selected or all node items to show all
    //! pins.
    //!
    void displayAllPins ();

    //!
    //! Sets the pin display mode for selected or all node items to show only
    //! connected pins.
    //!
    void displayConnectedPins ();

    //!
    //! Sets the pin display mode for selected or all node items to show all
    //! pins as one collapsed pin.
    //!
    void displayPinsCollapsed ();

signals: //

    //!
    //! Signal to notify connected objects that a context menu has been
    //! requested at the given scene position.
    //!
    //! \param scenePosition The position in scene coordinates at which the context menu has been requested.
    //!
    void contextMenuRequested ( const QPointF &scenePosition );

	//!
	//! Signal to notify panel to delete the current selected node
	//!
	void deleteSelectedNode();

protected: // events

	//!
    //! Event handler for key press events.
    //!
    //! \param event The description of the key event.
    //!
    virtual void keyPressEvent ( QKeyEvent *event );

	//!
    //! Event handler for mouse press events.
    //!
    //! \param event The description of the mouse event.
    //!
    virtual void mousePressEvent ( QMouseEvent *event );

	//!
    //! Event handler for mouse move events.
    //!
    //! \param event The description of the mouse event.
    //!
    virtual void mouseMoveEvent ( QMouseEvent *event );

    //!
    //! Event handler for context menu events.
    //!
    //! \param event The object that contains details about the event.
    //!
    virtual void contextMenuEvent ( QContextMenuEvent *event );

private: // functions

    //!
    //! Sets the pin display mode for selected or all node items to the given
    //! mode.
    //!
    //! \param pinDisplayMode The pin display mode to set for the node items.
    //!
    void setPinDisplayMode ( NodeGraphicsItem::PinDisplayMode pinDisplayMode );

    //!
    //! Cancels the creation of a new connection between pins of nodes.
    //!
    void cancelCreatingConnection ();
};

} // end namespace Frapper

#endif