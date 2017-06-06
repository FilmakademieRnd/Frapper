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
//! \file "ViewFlagGraphicsItem.cpp"
//! \brief Implementation file for ViewFlagGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    0.1
//! \date       16.02.2009 (last updated)
//!

#include "ViewFlagGraphicsItem.h"
#include "Log.h"
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the ViewFlagGraphicsItem class.
//!
//! \param parent The graphics item this item will be a child of.
//! \param state The initial state of the flag.
//! \param position The flag's relative position in its parent.
//! \param stageIndex The index of the stage that the node that this flag represents is contained in.
//!
ViewFlagGraphicsItem::ViewFlagGraphicsItem ( QGraphicsItem *parent, bool state, const QPointF &position, unsigned int stageIndex ) :
    FlagGraphicsItem(parent, "View", state, QColor(0, 173, 240), position),
    m_stageIndex(stageIndex),
    m_stageActionGroup(0)
{
    // create an action group for the stage actions
    m_stageActionGroup = new QActionGroup(this);
    for (unsigned int stageIndex = 1; stageIndex <= NUMBER_OF_STAGES; ++stageIndex) {
        QString text;
        if (stageIndex < 10)
            text = QString("Stage &%1").arg(stageIndex);
        else if (stageIndex == 10)
            text = "Stage 1&0";
        else
            text = QString("Stage %1").arg(stageIndex);
        QIcon icon (QString(":/stage%1Icon").arg(stageIndex));
        QAction *stageAction = new QAction(icon, text, m_stageActionGroup);
        stageAction->setCheckable(true);
        if (stageIndex == m_stageIndex)
            stageAction->setChecked(true);
    }
    connect(m_stageActionGroup, SIGNAL(triggered(QAction *)), SLOT(selectStage(QAction *)));
}


//!
//! Destructor of the ViewFlagGraphicsItem class.
//!
ViewFlagGraphicsItem::~ViewFlagGraphicsItem ()
{
}


///
/// Public Functions
///


//!
//! Paints the graphics item into a graphics view.
//!
//! \param painter The object to use for painting.
//! \param option Style options for painting the graphics item.
//! \param widget The widget into which to paint the graphics item.
//!
void ViewFlagGraphicsItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
    FlagGraphicsItem::paint(painter, option, widget);

    QColor penColor (Qt::black);
    if (m_state)
        penColor = Qt::white;

    painter->setPen(QPen(penColor));
    painter->drawText(rect(), Qt::AlignCenter, QString("%1").arg(m_stageIndex));
}


///
/// Protected Events
///


//!
//! Event handler that reacts to context menu events.
//!
//! \param event The object containing details about the event.
//!
void ViewFlagGraphicsItem::contextMenuEvent ( QGraphicsSceneContextMenuEvent *event )
{
    // create and show the context menu
    QMenu menu;
    menu.addActions(m_stageActionGroup->actions());
    menu.exec(event->screenPos());
}


///
/// Private Slots
///


//!
//! Selects the stage to assign to the node corresponding to the triggered
//! stage action.
//!
//! \param triggeredAction The action that was triggered.
//!
void ViewFlagGraphicsItem::selectStage ( QAction *triggeredAction )
{
    int actionIndex = m_stageActionGroup->actions().indexOf(triggeredAction);
    if (actionIndex >= 0) {
        unsigned int stageIndex = actionIndex + 1;
        if (stageIndex != m_stageIndex) {
            m_stageIndex = stageIndex;

            // notify connected objects that the stage index has been set
            emit stageIndexSet(m_stageIndex);

            update();
        }
    } else
        Log::error("The triggered action could not be found in the list of actions contained in the stage action group.", "ViewFlagGraphicsItem::selectStage");
}

//!
//! Activated when a new stage has been selected from outside.
//!
//! \param index The stage index.
//!
void ViewFlagGraphicsItem::setStageIndex ( unsigned int index, ViewNode *viewNode )
{
    if (index >= 0) {
        if (index != m_stageIndex) {
            m_stageIndex = index;
            QList<QAction *> actions = m_stageActionGroup->actions();
            for (int i = 0; i < actions.size(); ++i) {
                QAction *action = actions[i]; 
                if (i+1 == index)
                    action->setChecked(true);
                else
                    action->setChecked(false);
            }
            
            // notify connected objects that the stage index has been set
            emit stageIndexSet(m_stageIndex);

            update();
        }
    } else
        Log::error("The index does not exist.", "ViewFlagGraphicsItem::setStageIndex");
}

} // end namespace Frapper