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
//! \file "NodeBackDropGraphicsItem.cpp"
//! \brief Implementation file for NodeBackDropGraphicsItem class.
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       06.03.2014 (last updated)
//!

#include "NodeBackDropGraphicsItem.h"

namespace Frapper {


NodeBackDropGraphicsItem::BackDropCorner::BackDropCorner(QPointF position, NodeBackDropGraphicsItem *parent) :
QGraphicsRectItem(-12, -12, 12, 12, parent),
m_parent(parent)
{
	setPos(position);
	setFlag(ItemIsMovable);
}

bool NodeBackDropGraphicsItem::BackDropCorner::isInResizeMode()
{
	return m_resizeMode;
}

void NodeBackDropGraphicsItem::BackDropCorner::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
	qreal scaleItem = scale(); // <- thats the current scale factor of this item
	qreal scaleView = painter->transform().m11(); // <- thats the scale factor inherited from the view
	painter->scale( scaleItem / scaleView, scaleItem / scaleView ); // apply the inverse of the views scale to the items scale for revealing the original items scale
	painter->setBrush( QColor(230, 140, 34, 255) );
	painter->drawRoundedRect(rect(), 2, 2);
}

void NodeBackDropGraphicsItem::BackDropCorner::prepareChange()
{
	prepareGeometryChange();
}

void NodeBackDropGraphicsItem::BackDropCorner::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
	m_resizeMode = false;
}

void NodeBackDropGraphicsItem::BackDropCorner::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	m_resizeMode = true;
}

void NodeBackDropGraphicsItem::BackDropCorner::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	if(!m_parent->getLock()) {
		m_parent->redraw();
		const QPointF off = event->scenePos() - event->lastScenePos();
		const qreal oldX = m_parent->rect().x();
		const qreal oldY = m_parent->rect().y();
		const qreal oldW = m_parent->rect().width();
		const qreal oldH = m_parent->rect().height();

		qreal newW = oldW + off.x();
		qreal newH = oldH + off.y();

		if (newW < 60.f)
			newW = 60.f;

		if (newH < 60.f)
			newH = 60.f;

		m_parent->setRect(oldX, oldY, newW, newH);
		m_parent->setWidth(newW);
		m_parent->setHeight(newH);
		const qreal cornerW = oldX + m_parent->rect().width();
		const qreal cornerH = oldY + m_parent->rect().height();
		setPos(QPointF(cornerW, cornerH));
	}
}

///
/// Constructors and Destructors
///

//!
//! Constructor of the NodeGroupGraphicsItem class.
//!
//! \param node The node that the graphics item represents.
//! \param position The initial position for the graphics item.
//! \param color The color in which to draw the graphics item.
//!
NodeBackDropGraphicsItem::NodeBackDropGraphicsItem ( const QString &name, Node *node, const QPointF position, const qreal width, const qreal height) :
	BaseRectItem(width, height, QColor(48, 48, 48)),
	m_node(node),
	m_name(name),
	m_corner(new BackDropCorner(QPointF(width, height), this)),
	m_textItem(new QGraphicsTextItem(this)),
	m_headTextSize(12),
	m_descriptionTextSize(8),
	m_headText(""),
	m_descriptionText(""),
	m_descriptionColor(255,255,255),
	m_backgroundColor(0,0,128)
{
	m_textItem->setPos(10, 60);
	m_corner->setVisible(false);

#if QT_VERSION >= 0x040600
	// Since Qt 4.6 geometry changes are disabled by default. Introduction of new flag.
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);

	setPos(position);

	connect(m_node, SIGNAL(selectedChanged(bool)),	SLOT(setSelected(bool)));
	connect(m_node, SIGNAL(nodeChanged()),			SLOT(redraw()));

	setZValue(-100.0);
}

//!
//! Destructor of the NodeGroupGraphicsItem class.
//!
NodeBackDropGraphicsItem::~NodeBackDropGraphicsItem ()
{
}

void NodeBackDropGraphicsItem::setSelected(bool selection)
{
	QGraphicsItem::setSelected(selection);
}

void NodeBackDropGraphicsItem::redraw()
{
	m_headText = m_node->getStringValue("Headline");
	m_headTextSize = m_node->getIntValue("Headline Size");
	if(m_headTextSize < 1)
		m_headTextSize = 1;
	
	m_descriptionText = m_node->getStringValue("Description");
	m_descriptionTextSize = m_node->getIntValue("Description Size");
	if(m_descriptionTextSize < 1)
		m_descriptionTextSize = 1;
	
	m_descriptionColor = m_node->getColorValue("Text Color");
	m_backgroundColor = m_node->getColorValue("Background Color");

	prepareGeometryChange();

	QFont font;
	font.setFamily(font.defaultFamily());
	font.setPointSize(m_descriptionTextSize);
	m_textItem->setDefaultTextColor(m_descriptionColor);
	m_textItem->setHtml(m_descriptionText);
	m_textItem->setTextWidth(rect().width() * 0.75);
	m_textItem->setFont(font);
	m_textItem->setPos(5, (m_headTextSize+40));

	setToolTip(m_descriptionText);

	m_corner->prepareChange();
	update();
	m_corner->update();
}

void NodeBackDropGraphicsItem::setWidth( qreal width )
{
	m_node->setValue("Width", width);
}

void NodeBackDropGraphicsItem::setHeight( qreal height )
{
	m_node->setValue("Height", height);
}


QVariant NodeBackDropGraphicsItem::itemChange ( GraphicsItemChange change, const QVariant & value )
{
	if (change == QGraphicsItem::ItemSelectedChange)
	{
		if (value == true)
			m_corner->setVisible(true);
		else
			m_corner->setVisible(false);
	}
	return QGraphicsItem::itemChange(change, value);
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
void NodeBackDropGraphicsItem::paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{	
    static const int CornerRadius = 10;

    // draw selection border
    if (isSelected()) {
        painter->setPen(QPen(m_color, 8));
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRoundedRect(rect(), CornerRadius, CornerRadius);
    }

    // set up the node's background gradient
    QLinearGradient linearGradient (rect().left(), 0, rect().right(), 0);
    linearGradient.setColorAt(0.7, m_backgroundColor);
    linearGradient.setColorAt(1, m_color);

    // draw the node's shape
    QBrush gradientBrush (linearGradient);
    if (isEnabled())
        painter->setPen(QPen(Qt::black, 1));
    else
        painter->setPen(QPen(Qt::gray, 1));
    painter->setBrush(gradientBrush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(rect(), CornerRadius, CornerRadius);

	// draw the node's headline
	QFont font;
	font.setPointSize(m_headTextSize);
	painter->setFont(font);
	painter->setPen(QPen(m_descriptionColor, 1));
	painter->setClipping(true);
	painter->setClipRect(rect().adjusted(2, 2, -2, -2));
	painter->setRenderHint(QPainter::TextAntialiasing);
	painter->drawText(QPointF(rect().left() + 8, rect().top() + (m_headTextSize+25)), m_headText);
	painter->setClipping(false);
}

//!
//! Returns the selected node.
//!
//! \param i The position of the node in list.
//! 
//! \return The node that the graphics item represents.
//!
Node * NodeBackDropGraphicsItem::getNode () const
{
    return m_node;
}

//!
//! Returns whether the group item contains the node with
//! the given name or not.
//!
//! \return True the group item contains the node with
//! the given name, false otherwise.
//!
const bool NodeBackDropGraphicsItem::containsNode(const QString &nodeName) const
{
	if (m_node->getName() == nodeName)
			return true;
	return false;
}

//!
//! Returns the name of the item.
//!
//! \return The Name of the item.
//!
const QString NodeBackDropGraphicsItem::getName () const
{
	return m_name;
}

void NodeBackDropGraphicsItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	bool lock = m_node->getBoolValue("Lock Changes");
	if(!lock) {
		BaseRectItem::mouseMoveEvent(event);
	}
}

bool NodeBackDropGraphicsItem::getLock()
{
	return m_node->getBoolValue("Lock Changes");
}

///
/// Private Functions
///

//!
//! Internal function to determine if an parameter of the node group is internal.
//!
//! \param parameter The Parameter to check.
//! \return The estimated result.
//!
bool NodeBackDropGraphicsItem::isInternal(AbstractParameter *inParameter) const
{
	bool returnvalue = false;
	Parameter *connectedParameter = 0;
	if (inParameter->isGroup()) {
		ParameterGroup *parameterGroup = static_cast<ParameterGroup *>(inParameter);
		const AbstractParameter::List& parameterList = parameterGroup->getParameterList();
		foreach (AbstractParameter *curPara, parameterList)
			returnvalue = returnvalue || isInternal(curPara);
	}
	else {
		Parameter *parameter = static_cast<Parameter *>(inParameter);
		if (parameter->isConnected()) {
			const Connection::Map &connectionMap = parameter->getConnectionMap();
			foreach (const Connection *connection, connectionMap) {
				connectedParameter = connection->getTargetParameter();
				if (connectedParameter)
					returnvalue = returnvalue || m_node == (connectedParameter->getNode());
				connectedParameter = connection->getSourceParameter();
				if (connectedParameter)
					returnvalue = returnvalue && m_node == (connectedParameter->getNode());
			}
		}
	}
	return returnvalue;
}

} // end namespace Frapper