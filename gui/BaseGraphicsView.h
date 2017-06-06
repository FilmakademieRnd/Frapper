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
//! \file "BaseGraphicsView.h"
//! \brief Header file for BaseGraphicsView class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       02.07.2009 (last updated)
//!

#ifndef BASEGRAPHICSVIEW_H
#define BASEGRAPHICSVIEW_H

#include "FrapperPrerequisites.h"
#include "NodeGraphicsItem.h"
#include <QGraphicsView.h>
#include <QKeyEvent>
#include <QMouseEvent>

#define MIN_SCALE 0.001
#define MAX_SCALE 1000.0

namespace Frapper {

//!
//! Class representing a graphics view for displaying and editing a network of
//! nodes.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph BaseGraphicsView {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     BaseGraphicsView [label="BaseGraphicsView",fillcolor="grey75"];
//!     QGraphicsView -> BaseGraphicsView;
//!     QGraphicsView [label="QGraphicsView",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qgraphicsview.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_GUI_EXPORT BaseGraphicsView : public QGraphicsView
{
    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the BaseGraphicsView class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //!
    BaseGraphicsView ( QWidget *parent = 0 );

    //!
    //! Destructor of the BaseGraphicsView class.
    //!
    virtual ~BaseGraphicsView ();


public slots: //

    //!
    //! Resets the network graphics view's matrix.
    //!
    void homeView ();

    //!
    //! Changes the viewing transformation so that all items are visible at maximum
    //! zoom level.
    //!
    void frameAll ();

    //!
    //! Changes the viewing transformation so that the selected items are visible
    //! at maximum zoom level.
    //!
    void frameSelected ();

    //!
    //! Toggles the visibility of scrollbars for the network graphics view.
    //!
    //! \param visible Flag to control whether to show or hide the scrollbars.
    //!
    void toggleScrollbars ( bool visible );

protected: // events

    //!
    //! Event handler for key press events.
    //!
    //! \param event The description of the key event.
    //!
    virtual void keyPressEvent ( QKeyEvent *event );

    //!
    //! Event handler for key release events.
    //!
    //! \param event The description of the key event.
    //!
    virtual void keyReleaseEvent ( QKeyEvent *event );

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
    //! Event handler for mouse release events.
    //!
    //! \param event The description of the mouse event.
    //!
    virtual void mouseReleaseEvent ( QMouseEvent *event );

    //!
    //! Event handler for mouse wheel events.
    //!
    //! \param event The description of the mouse wheel event.
    //!
    virtual void wheelEvent ( QWheelEvent *event );

protected: // functions

    //!
    //! Changes the scale of the graphics scene so that it appears bigger in
    //! the graphics view.
    //!
    virtual void zoomIn ();

    //!
    //! Changes the scale of the graphics scene so that it appears smaller in
    //! the graphics view.
    //!
    virtual void zoomOut ();

    //!
    //! Sets the scale of the viewing transformation matrix that is used for
    //! displaying the graphics scene.
    //!
    //! \param s The new scale for the viewing transformation matrix.
    //!
    void setScale ( qreal s );

    //!
    //! Sets the viewing transformation so that the given bounding rectangle is
    //! fully visible in the graphics view, and centers the bounding rectangle
    //! in the middle of the view.
    //!
    //! \param rect The rectangular region to frame in the graphics view.
    //!
    void frame ( const QRectF &rect );

protected: // data

    //!
    //! Flag that states whether the viewing mode is activated.
    //!
    bool m_viewMode;

    //!
    //! The last position of the mouse pointer over the viewport.
    //!
    QPoint m_lastPosition;

};

} // end namespace Frapper

#endif