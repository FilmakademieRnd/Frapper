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
//! \file "ViewFlagGraphicsItem.h"
//! \brief Header file for ViewFlagGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    0.1
//! \date       16.02.2009 (last updated)
//!

#ifndef VIEWFLAGGRAPHICSITEM_H
#define VIEWFLAGGRAPHICSITEM_H

#include "FlagGraphicsItem.h"
#include "ViewNode.h"
#include <QAction>

namespace Frapper {

//!
//! Class for graphical representation of node view flags.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph ViewFlagGraphicsItem {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     ViewFlagGraphicsItem [label="ViewFlagGraphicsItem",fillcolor="grey75"];
//!     FlagGraphicsItem -> ViewFlagGraphicsItem;
//!     FlagGraphicsItem [label="FlagGraphicsItem",URL="class_flag_graphics_item.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT ViewFlagGraphicsItem : public FlagGraphicsItem
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the ViewFlagGraphicsItem class.
    //!
    //! \param parent The graphics item this item will be a child of.
    //! \param state The initial state of the flag.
    //! \param position The flag's relative position in its parent.
    //! \param stageIndex The index of the stage that the node that this flag belongs to is contained in.
    //!
    ViewFlagGraphicsItem ( QGraphicsItem *parent, bool state, const QPointF &position, unsigned int stageIndex );

    //!
    //! Destructor of the ViewFlagGraphicsItem class.
    //!
    ~ViewFlagGraphicsItem ();

public: // functions

    //!
    //! Paints the graphics item into a graphics view.
    //!
    //! \param painter The object to use for painting.
    //! \param option Style options for painting the graphics item.
    //! \param widget The widget into which to paint the graphics item.
    //!
    virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

protected: // events

    //!
    //! Event handler that reacts to context menu events.
    //!
    //! \param event The object containing details about the event.
    //!
    void contextMenuEvent ( QGraphicsSceneContextMenuEvent *event );

signals: //

    //!
    //! Signal that is emitted when the stage index has been changed.
    //!
    //! \param stageIndex The index of the new stage that the node should be contained in.
    //!
    void stageIndexSet ( unsigned int stageIndex );

    private slots: //

        //!
        //! Selects the stage to assign to the node corresponding to the triggered
        //! stage action.
        //!
        //! \param triggeredAction The action that was triggered.
        //!
        void selectStage ( QAction *triggeredAction );

        //!
        //! Activated when a new stage has been selected from outside.
        //!
        //! \param index The stage index.
        //!
        void setStageIndex ( unsigned int index, ViewNode *viewNode );

private: // data

    //!
    //! The index of the stage that the node that this flag belongs to is
    //! contained in.
    //!
    unsigned int m_stageIndex;

    //!
    //! The group that all the stage actions will be added to.
    //!
    QActionGroup *m_stageActionGroup;

};

} // end namespace Frapper

#endif
