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
//! \file "ParameterTypeIcon.cpp"
//! \brief Implementation file for ParameterTypeIcon class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    0.2
//! \date       27.11.2008 (last updated)
//!

#include "ParameterTypeIcon.h"
#include "Log.h"
#include <QApplication>
#include <QPainter>
#include <QBitmap>
#include <QStyleFactory>

namespace Frapper {

INIT_INSTANCE_COUNTER(ParameterTypeIcon)

///
/// Private Static Data
///


//!
//! Map for action icons by parameter type name.
//!
QMap<QString, QIcon> ParameterTypeIcon::s_iconCache;


///
/// Private Static Functions
///


//!
//! Returns the icon for the parameter type with the given name.
//! Creates the icon if it doesn't exist yet and stores it in the parameter
//! type icon cache.
//!
//! \param parameterTypeName The name of the parameter type to return the icon for.
//! \return The icon that represents the parameter type with the given name.
//!
QIcon ParameterTypeIcon::getIcon ( const QString &parameterTypeName )
{
    if (!s_iconCache.contains(parameterTypeName)) {
        // create the icon for the parameter type with the given name
        ParameterTypeIcon parameterTypeIcon (Parameter::getTypeColor(Parameter::getTypeByName(parameterTypeName)));
        s_iconCache[parameterTypeName] = QIcon(parameterTypeIcon.pixmap(16));
    }

    return s_iconCache[parameterTypeName];
}


///
/// Constructors and Destructors
///


//!
//! Constructor of the ParameterTypeIcon class.
//!
//! \param color The color for the parameter type icon.
//!
ParameterTypeIcon::ParameterTypeIcon ( const QColor &color )
{
    m_color = color;

    const QRect pinRect (0, 4, 15, 8);
    const QColor penColor (109, 109, 109);
    QPixmap normalPixmap (16, 16);
    QPainter painter;
    if (painter.begin(&normalPixmap)) {
        // get the background color from the application style's standard palette
        QColor backgroundColor = QApplication::style()->standardPalette().color(QPalette::Base);
        // fill the pixmap with the background color
        painter.fillRect(normalPixmap.rect(), QBrush(backgroundColor));
        // draw the pin rect into the pixmap
        painter.setPen(QPen(penColor));
        painter.setBrush(QBrush(m_color));
        painter.drawRoundRect(pinRect);
        // end painting the pixmap
        if (painter.end()) {
            // create a bitmap for masking the pixmap
            QBitmap maskBitmap (normalPixmap.size());
            if (painter.begin(&maskBitmap)) {
                // fill the bitmap with white (meaning transparent)
                painter.fillRect(maskBitmap.rect(), QBrush(QColor(Qt::white)));
                // draw the pin rect as black (meaning opaque)
                painter.setPen(QPen(QColor(Qt::black)));
                painter.setBrush(QBrush(QColor(Qt::black)));
                painter.drawRoundRect(pinRect);
                // end painting the bitmap and set it as mask for the pixmap
                if (painter.end())
                    normalPixmap.setMask(maskBitmap);
            }
        }
    }
    // add the pixmap to the icon
    addPixmap(normalPixmap);
}


//!
//! Destructor of the ParameterTypeIcon class.
//!
ParameterTypeIcon::~ParameterTypeIcon ()
{
}


///
/// Public Functions
///


//!
//! Uses the given painter to paint the icon with specified alignment,
//! required mode, and state into the given rectangle.
//!
//! \param painter The object to use for painting.
//! \param rect The rectangle in which to paint.
//! \param alignment The alignment to use for painting.
//! \param mode The mode for which to paint the icon.
//! \param state The state in which to paint the icon.
//!
void ParameterTypeIcon::paint ( QPainter *painter, const QRect &rect, Qt::Alignment alignment /* = Qt::AlignCenter */, Mode mode /* = Normal */, State state /* = Off */ ) const
{
    Log::debug("Painting an icon...");
    // paint the pin
    painter->setPen(QPen(QBrush(QColor(109, 109, 109)), 0.8));
    painter->setBrush(QBrush(m_color));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundRect(rect);

    //// check if the parameter the pin represents is a parameter group
    //if (m_abstractParameter->isGroup()) {
    //    painter->setPen(QPen(penColor));
    //    painter->setBrush(QBrush(QColor(0, 0, 0)));
    //    if (m_abstractParameter->getName().isEmpty()) {
    //        // draw two dots for parameter roots
    //        painter->drawEllipse(rect().center() - QPointF(2, 0), 1, 1);
    //        painter->drawEllipse(rect().center() + QPointF(2, 0), 1, 1);
    //    } else
    //        // draw one dot for other parameter groups
    //        painter->drawEllipse(rect().center(), 1, 1);
    //}
}

} // end namespace Frapper