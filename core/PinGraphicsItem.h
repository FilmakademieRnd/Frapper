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
//! \file "PinGraphicsItem.h"
//! \brief Header file for PinGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef PINGRAPHICSITEM_H
#define PINGRAPHICSITEM_H

#include "BaseRectItem.h"
#include "Parameter.h"
#include "ParameterGroup.h"
#include "InstanceCounterMacros.h"

namespace Frapper {

//!
//! Class for graphical representation of node connector pins.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph PinGraphicsItem {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     PinGraphicsItem [label="PinGraphicsItem",fillcolor="grey75"];
//!     QObject -> PinGraphicsItem;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!     BaseRectItem -> PinGraphicsItem;
//!     BaseRectItem [label="BaseRectItem",URL="class_base_rect_item.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT PinGraphicsItem : public BaseRectItem
{

    Q_OBJECT
        ADD_INSTANCE_COUNTER

private: // nested enumerations

    //!
    //! Nested enumeration for the different types of connected states the pin
    //! represents.
    //! A pin can represent all parameters in a parameter group, just those
    //! that are connected, or just those that are not connected.
    //!
    enum ConnectedType {
        CT_All,
        CT_Connected,
        CT_NonConnected
    };

public: // constructors and destructors

    //!
    //! Constructor of the PinGraphicsItem class.
    //! Creates a pin graphics item representing the given parameter.
    //!
    //! \param parent The graphics item this item will be a child of.
    //! \param index The index of the pin in a list of pins.
    //! \param parameter The parameter this pin item represents.
    //!
    PinGraphicsItem ( QGraphicsItem *parent, int index, Parameter *parameter );

    //!
    //! Constructor of the PinGraphicsItem class.
    //! Creates a pin graphics item representing all parameters in the given
    //! parameter group.
    //!
    //! \param parent The graphics item this item will be a child of.
    //! \param index The index of the pin in a list of pins.
    //! \param parameterGroup The parameter group this pin item represents.
    //! \param pinType The type that states whether the item represents input or output parameters in the given group.
    //!
    PinGraphicsItem ( QGraphicsItem *parent, int index, ParameterGroup *parameterGroup, Parameter::PinType pinType );

    //!
    //! Constructor of the PinGraphicsItem class.
    //! Creates a pin graphics item representing only connected or unconnected
    //! parameters in the given parameter group.
    //!
    //! \param parent The graphics item this item will be a child of.
    //! \param index The index of the pin in a list of pins.
    //! \param parameterGroup The parameter group this pin item represents.
    //! \param pinType The type that states whether the item represents input or output parameters in the given group.
    //! \param connected Flag that states whether the item represents connected or unconnected parameters.
    //!
    PinGraphicsItem ( QGraphicsItem *parent, int index, ParameterGroup *parameterGroup, Parameter::PinType pinType, bool connected );

	void setItemTooltip( Parameter * parameter );

    //!
    //! Destructor of the PinGraphicsItem class.
    //!
    ~PinGraphicsItem ();

public: // functions

    //!
    //! Paints the graphics item into a graphics view.
    //!
    //! \param painter The object to use for painting.
    //! \param option Style options for painting the graphics item.
    //! \param widget The widget into which to paint the graphics item.
    //!
    virtual void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

    //!
    //! Returns the center point of the pin item.
    //!
    //! \return The center point of the pin item.
    //!
    QPointF center () const;

    //!
    //! Enables or disables the graphics item.
    //!
    //! \param enabled The new enabled state for the item.
    //!
    void setEnabled ( bool enabled );

    //!
    //! Enables the pin graphics item if it represents the given parameter type
    //! and pin type.
    //!
    //! \param parameterType The parameter type of pins to enable.
    //! \param pinType The pin type of pins to enable.
    //! \return True if the pin was enabled, otherwise False.
    //!
    bool setEnabled ( Parameter::Type parameterType, Parameter::PinType pinType );

    //!
    //! Returns the pin's type.
    //!
    //! \return The pin's type.
    //!
    Parameter::PinType getPinType () const;

    //!
    //! Returns whether the pin represents the given parameter.
    //!
    //! \param parameter The parameter to check.
    //! \return True if the pin represents the given parameter, otherwise False.
    //!
    bool represents ( Parameter *parameter ) const;

    //!
    //! Returns whether the pin represents parameters of the given type
    //!
    //! \param parameterType The parameter type to check.
    //! \return True if the pin represents parameters of the given type, otherwise False.
    //!
    bool represents ( Parameter::Type parameterType ) const;

signals: //

    //!
    //! Signal that is emitted when a parameter represented by the pin has been
    //! clicked.
    //! Is either emitted when a pin representing one parameter only itself has
    //! been clicked, or when an entry in the context menu of a pin representing
    //! a parameter group has been clicked.
    //!
    //! \param pinItem The pin item emitting the signal.
    //! \param parameter The parameter that was clicked.
    //!
    void parameterClicked ( PinGraphicsItem *pinItem, Parameter *parameter );

protected: // functions

    //!
    //! Event handler that reacts to mouse release events.
    //!
    //! \param event The object containing details about the event.
    //!
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent *event );

    //!
    //! Event handler that reacts to context menu events.
    //!
    //! \param event The object containing details about the event.
    //!
    void contextMenuEvent ( QGraphicsSceneContextMenuEvent *event );

private: // functions

    //!
    //! Fills the given menu with actions and sub menus for the parameters in the
    //! given parameter group.
    //!
    //! \param menu The menu to which the actions and sub menus will be added.
    //! \param parameterGroup The parameter group to create the context menu for.
    //!
    void fillMenu ( QMenu *menu, ParameterGroup *parameterGroup );

    private slots:
        //!
        //! Removes the parameter (m_abstractParameter). E.g. on deletion.
        //!
        void removeParameter ();

private: // data

    //!
    //! The parameter or parameter group the pin represents.
    //!
    AbstractParameter *m_abstractParameter;

    //!
    //! Represented parameter is group.
    //!
    bool m_isGroup;

    //!
    //! Represented parameter is root group.
    //!
    bool m_isRootGroup;

    //!
    //! The type of parameters in a parameter group the pin represents.
    //!
    Parameter::PinType m_pinType;

    //!
    //! The connected type of parameters in a parameter group the pin
    //! represents.
    //! A pin can represent all parameters in a parameter group, just those
    //! that are connected, or just those that are not connected.
    //!
    ConnectedType m_connectedType;

    //!
    //! The parameter type to filter in the context menu.
    //!
    Parameter::Type m_parameterTypeFilter;

};

} // end namespace Frapper

#endif
