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
//! \file "ParameterTypeIcon.h"
//! \brief Header file for ParameterTypeIcon class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef PARAMETERTYPEICON_H
#define PARAMETERTYPEICON_H

#include <QIcon>
#include "Parameter.h"
#include "InstanceCounterMacros.h"

namespace Frapper {

//!
//! Class representing an icon representing a parameter type.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph ParameterTypeIcon {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     ParameterTypeIcon [label="ParameterTypeIcon",fillcolor="grey75"];
//!     QIcon -> ParameterTypeIcon;
//!     QIcon [label="QIcon",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qicon.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT ParameterTypeIcon : public QIcon
{

    ADD_INSTANCE_COUNTER

public: // static functions

    //!
    //! Returns the icon for the parameter type with the given name.
    //! Creates the icon if it doesn't exist yet and stores it in the parameter
    //! type icon cache.
    //!
    //! \param parameterTypeName The name of the parameter type to return the icon for.
    //! \return The icon that represents the parameter type with the given name.
    //!
    static QIcon getIcon ( const QString &parameterTypeName );

private: // static data

    //!
    //! Map for action icons by parameter type name.
    //!
    static QMap<QString, QIcon> s_iconCache;

public: // constructors and destructors

    //!
    //! Constructor of the ParameterTypeIcon class.
    //!
    //! \param color The color for the parameter type icon.
    //!
    ParameterTypeIcon ( const QColor &color );

    //!
    //! Destructor of the ParameterTypeIcon class.
    //!
    virtual ~ParameterTypeIcon ();

public: // functions

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
    void paint ( QPainter *painter, const QRect &rect, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off ) const;

private: // data

    //!
    //! The color to use when painting the icon.
    //!
    QColor m_color;

};

} // end namespace Frapper

#endif
