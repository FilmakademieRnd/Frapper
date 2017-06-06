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
//! \file "PinGraphicsItem.cpp"
//! \brief Implementation file for PinGraphicsItem class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Simon Spielmann <sspielma@filmakademie.de>
//! \version    0.4
//! \date       21.11.2013 (last updated)
//!

#include "PinGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "ParameterAction.h"

namespace Frapper {

INIT_INSTANCE_COUNTER(PinGraphicsItem)

///
/// Global Constants with File Scope
///


//!
//! Background color for pins representing parameter groups.
//!
static const QColor PIN_GROUP_COLOR = QColor(200, 200, 200);

//!
//! Horizontal spacing on the left next to the pins in a node graphics item.
//!
static const qreal PIN_OFFSET_LEFT = 4;

//!
//! Horizontal spacing on the right next to the pins in a node graphics item.
//!
static const qreal PIN_OFFSET_RIGHT = 12;

//!
//! Vertical spacing above the first pin in a node graphics item.
//!
static const qreal PIN_OFFSET_TOP = 40;

//!
//! Vertical spacing between two pins.
//!
static const qreal PIN_VERTICAL_SPACING = 12;

//!
//! The pin's width.
//!
static const qreal PIN_WIDTH = 15;

//!
//! The pin's height.
//!
static const qreal PIN_HEIGHT = 8;


///
/// Constructors and Destructors
///


//!
//! Constructor of the PinGraphicsItem class.
//! Creates a pin graphics item representing the given parameter.
//!
//! \param parent The graphics item this item will be a child of.
//! \param index The index of the pin in a list of pins.
//! \param parameter The parameter this pin item represents.
//!
PinGraphicsItem::PinGraphicsItem ( QGraphicsItem *parent, int index, Parameter *parameter )
: BaseRectItem(Parameter::getTypeColor(parameter->getType()), true, parent)
{
    m_abstractParameter = parameter;
    m_isGroup = m_abstractParameter->isGroup();
    m_isRootGroup = m_abstractParameter->getName().isEmpty();
    connect(parameter, SIGNAL(destroyed()), SLOT(removeParameter()));
    m_pinType = parameter->getPinType();
    m_parameterTypeFilter = Parameter::T_Unknown;

    // set the item's position and size
    setRect(QRectF(0, 0, PIN_WIDTH, PIN_HEIGHT));
    if (m_pinType == Parameter::PT_Input)
        setPos(PIN_OFFSET_LEFT, PIN_OFFSET_TOP + index * PIN_VERTICAL_SPACING);
    else
        setPos(parent->boundingRect().width() - PIN_OFFSET_RIGHT - rect().width(), PIN_OFFSET_TOP + index * PIN_VERTICAL_SPACING);

	setItemTooltip(parameter);


    //setCacheMode(DeviceCoordinateCache);

    INC_INSTANCE_COUNTER
}


//!
//! Constructor of the PinGraphicsItem class.
//! Creates a pin graphics item representing the parameters in the given
//! parameter group.
//!
//! \param parent The graphics item this item will be a child of.
//! \param index The index of the pin in a list of pins.
//! \param parameterGroup The parameter group this pin item represents.
//! \param pinType The type that states whether the item represents input or output parameters in the given group.
//! \param connected Flag that states whether the item represents connected parameters only.
//!
PinGraphicsItem::PinGraphicsItem ( QGraphicsItem *parent, int index, ParameterGroup *parameterGroup, Parameter::PinType pinType )
: BaseRectItem(PIN_GROUP_COLOR, true, parent)
{
    m_abstractParameter = parameterGroup;
    m_isGroup = m_abstractParameter->isGroup();
    m_isRootGroup = m_abstractParameter->getName().isEmpty();
    m_pinType = pinType;
    m_connectedType = CT_All;
    m_parameterTypeFilter = Parameter::T_Unknown;

    // set the item's position and size
    setRect(QRectF(0, 0, PIN_WIDTH, PIN_HEIGHT));
    if (m_pinType == Parameter::PT_Input)
        setPos(PIN_OFFSET_LEFT, PIN_OFFSET_TOP + index * PIN_VERTICAL_SPACING);
    else
        setPos(parent->boundingRect().width() - PIN_OFFSET_RIGHT - rect().width(), PIN_OFFSET_TOP + index * PIN_VERTICAL_SPACING);

    // set the item's tool tip
    QString pinInfo;
    if (parameterGroup->getName() == "")
        if (m_pinType == Parameter::PT_Input)
            pinInfo = "Input parameters";
        else
            pinInfo = "Ouput parameters";
    else
        pinInfo = parameterGroup->getName();

	setToolTip(pinInfo);

    INC_INSTANCE_COUNTER
}


//!
//! Constructor of the PinGraphicsItem class.
//! Creates a pin graphics item representing the parameters in the given
//! parameter group.
//!
//! \param parent The graphics item this item will be a child of.
//! \param index The index of the pin in a list of pins.
//! \param parameterGroup The parameter group this pin item represents.
//! \param pinType The type that states whether the item represents input or output parameters in the given group.
//! \param connected Flag that states whether the item represents connected parameters only.
//!
PinGraphicsItem::PinGraphicsItem ( QGraphicsItem *parent, int index, ParameterGroup *parameterGroup, Parameter::PinType pinType, bool connected )
: BaseRectItem(PIN_GROUP_COLOR, true, parent)
{
    m_abstractParameter = parameterGroup;
    m_isGroup = m_abstractParameter->isGroup();
    m_isRootGroup = m_abstractParameter->getName().isEmpty();
    m_pinType = pinType;
    m_connectedType = connected ? CT_Connected : CT_NonConnected;
    m_parameterTypeFilter = Parameter::T_Unknown;

    // set the item's position and size
    setRect(QRectF(0, 0, PIN_WIDTH, PIN_HEIGHT));
    if (m_pinType == Parameter::PT_Input)
        setPos(PIN_OFFSET_LEFT, PIN_OFFSET_TOP + index * PIN_VERTICAL_SPACING);
    else
        setPos(parent->boundingRect().width() - PIN_OFFSET_RIGHT - rect().width(), PIN_OFFSET_TOP + index * PIN_VERTICAL_SPACING);

    // set the item's tool tip
    QString pinInfo;
    if (parameterGroup->getName() == "")
        if (m_pinType == Parameter::PT_Input)
            pinInfo = "Input parameters";
        else
            pinInfo = "Output parameters";
    else
        pinInfo = parameterGroup->getName();
    setToolTip(pinInfo);

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the PinGraphicsItem class.
//!
PinGraphicsItem::~PinGraphicsItem ()
{
    DEC_INSTANCE_COUNTER
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
void PinGraphicsItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
    const QColor penColor (109, 109, 109);

    // set the brush
    QBrush brush;
    if (isEnabled())
        if (m_hovered && m_clicked)
            brush = QBrush(m_color.darker(115));
        else if (m_hovered || m_clicked)
            brush = QBrush(m_color.lighter(115));
        else
            brush = QBrush(m_color);
    else
        brush = Qt::NoBrush;

    // paint the pin
    painter->setPen(QPen(QBrush(penColor), 1));
    painter->setBrush(brush);
    painter->drawRoundRect(rect());

    // check if the parameter the pin represents is a parameter group
    //if (m_abstractParameter && m_abstractParameter->isGroup()) {
    //    painter->setPen(QPen(penColor));
    //    painter->setBrush(QBrush(QColor(Qt::black)));
    //    painter->setRenderHint(QPainter::Antialiasing);
    //    if (m_abstractParameter->getName().isEmpty()) {
    //        // draw two dots for parameter roots
    //        painter->drawEllipse(rect().center() + QPointF(-2, 0.5), 1, 1);
    //        painter->drawEllipse(rect().center() + QPointF(2, 0.5), 1, 1);
    //    } else
    //        // draw one dot for other parameter groups
    //        painter->drawEllipse(rect().center(), 1, 1);
    //}

    if (m_isGroup) {
        painter->setPen(QPen(penColor));
        painter->setBrush(QBrush(QColor(Qt::black)));
        painter->setRenderHint(QPainter::Antialiasing);
        if (m_isRootGroup) {
            // draw two dots for parameter roots
            painter->drawEllipse(rect().center() + QPointF(-2, 0.5), 1, 1);
            painter->drawEllipse(rect().center() + QPointF(2, 0.5), 1, 1);
        } else
            // draw one dot for other parameter groups
            painter->drawEllipse(rect().center(), 1, 1);
    }
}


//!
//! Returns the center point of the pin item.
//!
//! \return The center point of the pin item.
//!
QPointF PinGraphicsItem::center () const
{
    return pos() + rect().center();
}


//!
//! Enables or disables the graphics item.
//!
//! \param enabled The new enabled state for the item.
//!
void PinGraphicsItem::setEnabled ( bool enabled )
{
    BaseRectItem::setEnabled(enabled);

    if (enabled)
        // reset the parameter type filter for the context menu
        m_parameterTypeFilter = Parameter::T_Unknown;
}


//!
//! Enables the pin graphics item if it represents the given parameter type
//! and pin type.
//!
//! \param parameterType The parameter type of pins to enable.
//! \param pinType The pin type of pins to enable.
//! \return True if the pin was enabled, otherwise False.
//!
bool PinGraphicsItem::setEnabled ( Parameter::Type parameterType, Parameter::PinType pinType )
{
    bool enabled = represents(parameterType) && m_pinType == pinType;
    setEnabled(enabled);
    // set the parameter type filter for the context menu
    m_parameterTypeFilter = parameterType;
    return enabled;
}


//!
//! Returns the pin's type.
//!
//! \return The pin's type.
//!
Parameter::PinType PinGraphicsItem::getPinType () const
{
    return m_pinType;
}


//!
//! Returns whether the pin represents the given parameter.
//!
//! \param parameter The parameter to check.
//! \return True if the pin represents the given parameter, otherwise False.
//!
bool PinGraphicsItem::represents ( Parameter *parameter ) const
{
    if (m_abstractParameter->isGroup())
        return dynamic_cast<ParameterGroup *>(m_abstractParameter)->contains(parameter);
    else {
        return dynamic_cast<Parameter *>(m_abstractParameter) == parameter;
	}
}


//!
//! Returns whether the pin represents parameters of the given type
//!
//! \param parameterType The parameter type to check.
//! \return True if the pin represents parameters of the given type, otherwise False.
//!
bool PinGraphicsItem::represents ( Parameter::Type parameterType ) const
{
    if (m_abstractParameter->isGroup())
        return dynamic_cast<ParameterGroup *>(m_abstractParameter)->contains(parameterType);
    else
        return dynamic_cast<Parameter *>(m_abstractParameter)->getType() == parameterType;
}


///
/// Protected Functions
///


//!
//! Event handler that reacts to mouse release events.
//!
//! \param event The object containing details about the event.
//!
void PinGraphicsItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
    if (event->button() == Qt::LeftButton)
        if (m_abstractParameter)
        {
            if (m_abstractParameter->isGroup()) {
                // create context menu for parameter selection
                QMenu menu;
                fillMenu(&menu, dynamic_cast<ParameterGroup *>(m_abstractParameter));

                // show the menu
                QAction *selectedAction = menu.exec(event->screenPos());
                // notify connected objects that the parameter represented by the
                // menu action has been clicked
                ParameterAction *parameterAction = dynamic_cast<ParameterAction *>(selectedAction);
                if (parameterAction)
                    emit parameterClicked(this, parameterAction->getParameter());
            } else {
                // notify connected objects that the parameter represented by the
                // pin has been clicked
                emit parameterClicked(this, dynamic_cast<Parameter *>(m_abstractParameter));
            }
        } else {
            Log::debug("m_abstractParameter is not given (NULL)!", "PinGraphicsItem::mouseReleaseEvent");
        }
    else
        BaseRectItem::mouseReleaseEvent(event);
}


//!
//! Event handler that reacts to context menu events.
//!
//! \param event The object containing details about the event.
//!
void PinGraphicsItem::contextMenuEvent ( QGraphicsSceneContextMenuEvent *event )
{
}


///
/// Private Functions
///


//!
//! Fills the given menu with actions and sub menus for the parameters in the
//! given parameter group.
//!
//! \param menu The menu to which the actions and sub menus will be added.
//! \param parameterGroup The parameter group to create the context menu for.
//!
void PinGraphicsItem::fillMenu ( QMenu *menu, ParameterGroup *parameterGroup )
{
    // iterate over the list of parameters in the group
    const AbstractParameter::List& parameterList = parameterGroup->getParameterList();
    for (int i = 0; i < parameterList.size(); ++i) {
        AbstractParameter *abstractParameter = parameterList.at(i);
        if (abstractParameter->isGroup()) {
            ParameterGroup *parameterGroup = dynamic_cast<ParameterGroup *>(abstractParameter);
            if (parameterGroup->contains(m_pinType)) {
                // create a sub menu for the parameter group and recursively
                // fill it with actions and sub menus (if applicable)
                QMenu *subMenu = new QMenu(parameterGroup->getName(), menu);
                fillMenu(subMenu, parameterGroup);
                menu->addMenu(subMenu);
            }
        } else {
            Parameter *parameter = dynamic_cast<Parameter *>(abstractParameter);
            if (parameter->getPinType() == m_pinType) {
                // create an action representing the parameter in the menu
                ParameterAction *parameterAction = new ParameterAction(parameter, menu);
                parameterAction->setEnabled(m_parameterTypeFilter == Parameter::T_Unknown || parameter->getType() == m_parameterTypeFilter);
                menu->addActions(QList<QAction *>() << parameterAction);
            }
        }
    }
}

//!
//! Removes the parameter (m_abstractParameter). E.g. on deletion.
//!
void PinGraphicsItem::removeParameter ()
{
    m_abstractParameter = 0;
}

void PinGraphicsItem::setItemTooltip( Parameter * parameter )
{
	// set the item's tool tip
	QString pinInfo = parameter->getName();

	if( parameter->getType() == Parameter::T_Generic ) {
		pinInfo += "\nType: " + static_cast<GenericParameter*>(parameter)->getTypeInfo();
	} else {
		pinInfo += "\nType: " + Parameter::getTypeName(parameter->getType());
		if (parameter->getMultiplicity() == Parameter::M_OneOrMore)
			pinInfo += " List";
	}
	if (!parameter->getDescription().isEmpty())
		pinInfo += "\nDescription: " + parameter->getDescription();
	
	setToolTip(pinInfo);
}

} // end namespace Frapper