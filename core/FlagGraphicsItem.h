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
//! \file "FlagGraphicsItem.h"
//! \brief Header file for FlagGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef FLAGGRAPHICSITEM_H
#define FLAGGRAPHICSITEM_H

#include "FrapperPrerequisites.h"
#include "BaseRectItem.h"

namespace Frapper {

//!
//! Class for graphical representation of node flags.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph FlagGraphicsItem {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     FlagGraphicsItem [label="FlagGraphicsItem",fillcolor="grey75"];
//!     QObject -> FlagGraphicsItem;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!     BaseRectItem -> FlagGraphicsItem;
//!     BaseRectItem [label="BaseRectItem",URL="class_base_rect_item.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT FlagGraphicsItem : public BaseRectItem
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the FlagGraphicsItem class.
    //!
    //! \param parent The graphics item this item will be a child of.
    //! \param name The flag's name.
    //! \param state The initial state of the flag.
    //! \param color The flag's base color.
    //! \param position The flag's relative position in its parent.
    //!
    FlagGraphicsItem ( QGraphicsItem *parent, const QString &name, bool state, const QColor &color, const QPointF &position );

    //!
    //! Destructor of the FlagGraphicsItem class.
    //!
    ~FlagGraphicsItem ();

public: // functions

    //!
    //! Paints the graphics item into a graphics view.
    //!
    //! \param painter The object to use for painting.
    //! \param option Style options for painting the graphics item.
    //! \param widget The widget into which to paint the graphics item.
    //!
    virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

    public slots: //

        //!
        //! Sets the state of the flag to the given value.
        //!
        //! \param checked The new state for the flag.
        //!
        void setChecked ( bool checked );

signals: //

        //!
        //! Signal that is emitted when the flag has been toggled.
        //!
        //! \param checked The new state of the flag.
        //!
        void toggled ( bool checked );

protected: // functions

    //!
    //! Event handler that reacts to mouse press events.
    //!
    //! \param event The object containing details about the event.
    //!
    void mousePressEvent ( QGraphicsSceneMouseEvent *event );

    //!
    //! Event handler that reacts to mouse release events.
    //!
    //! \param event The object containing details about the event.
    //!
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent *event );

protected: // data

    //!
    //! The state of the flag.
    //!
    bool m_state;

};

} // end namespace Frapper

#endif
