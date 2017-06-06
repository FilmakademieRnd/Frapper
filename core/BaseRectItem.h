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
//! \file "BaseRectItem.h"
//! \brief Header file for BaseRectItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef BASERECTITEM_H
#define BASERECTITEM_H

#include "FrapperPrerequisites.h"
#include <QGraphicsRectItem>
#include <QPainter>

namespace Frapper {

//!
//! Abstract base class for the graphical representation of nodes, node flags
//! and node connector pins.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph BaseRectItem {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     BaseRectItem [label="BaseRectItem",fillcolor="grey75"];
//!     QGraphicsRectItem -> BaseRectItem;
//!     QGraphicsRectItem [label="QGraphicsRectItem",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qgraphicsrectItem.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT BaseRectItem : public QObject, public QGraphicsRectItem
{
	Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the BaseRectItem class.
    //!
    //! \param color The base color for the graphics item.
    //! \param actAsButton Flag that states whether the item should act as a button.
    //! \param parent The graphics item this item will be a child of.
    //!
    BaseRectItem ( const QColor &color, bool actAsButton = false, QGraphicsItem *parent = 0 );

	//!
	//! Constructor of the BaseRectItem class.
	//!
	//! \param color The base color for the graphics item.
	//! \param actAsButton Flag that states whether the item should act as a button.
	//! \param parent The graphics item this item will be a child of.
	//!
	BaseRectItem ( const int width, const int height, const QColor &color, bool actAsButton = false, QGraphicsItem *parent = 0 );

    //!
    //! Destructor of the BaseRectItem class.
    //!
    ~BaseRectItem ();

public: // functions

    //!
    //! Returns the bounding rectangle of the graphics item.
    //!
    //! \return The bounding rectangle of the graphics item.
    //!
    QRectF boundingRect () const;

    //!
    //! Paints the graphics item into a graphics view.
    //!
    //! \param painter The object to use for painting.
    //! \param option Style options for painting the graphics item.
    //! \param widget The widget into which to paint the graphics item.
    //!
    virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 ) = 0;

    //!
    //! Returns the item's base color.
    //!
    //! \return The item's base color.
    //!
    QColor getColor () const;

protected: // functions

    //!
    //! Event handler that reacts to hover enter events.
    //!
    //! \param event The object containing details about the event.
    //!
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );

    //!
    //! Event handler that reacts to hover leave events.
    //!
    //! \param event The object containing details about the event.
    //!
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );

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
    //! The item's base color.
    //!
    QColor m_color;

    //!
    //! Flag that states whether the item should act as a button.
    //!
    bool m_actAsButton;

    //!
    //! Flag that states whether the item is currently hovered by the mouse
    //! pointer.
    //!
    bool m_hovered;

    //!
    //! Flag that states whether the item is currently clicked by the mouse
    //! pointer (i.e. a mouse button is pressed).
    //!
    bool m_clicked;

};

} // end namespace Frapper

#endif
