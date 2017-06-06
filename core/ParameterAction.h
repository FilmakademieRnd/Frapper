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
//! \file "ParameterAction.h"
//! \brief Header file for ParameterAction class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef PARAMETERACTION_H
#define PARAMETERACTION_H

#include <QAction>
#include "Parameter.h"
#include "InstanceCounterMacros.h"

namespace Frapper {

//!
//! Class representing an action representing a parameter of a node.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph ParameterAction {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     ParameterAction [label="ParameterAction",fillcolor="grey75"];
//!     QAction -> ParameterAction;
//!     QAction [label="QAction",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qaction.html"];
//!     QObject -> QAction;
//!     QObject [label="QObject",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qobject.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_CORE_EXPORT ParameterAction : public QAction
{

    ADD_INSTANCE_COUNTER

private: // static data

    //!
    //! Map for action icons by parameter type name.
    //!
    static QMap<QString, QIcon> s_parameterTypeIcons;

private: // static functions

    //!
    //! Returns the icon for the parameter type with the given name.
    //! Creates the icon if it doesn't exist yet and stores it in the parameter
    //! type icon cache.
    //!
    //! \param parameterTypeName The name of the parameter type to return the icon for.
    //! \return The icon that represents the parameter type with the given name.
    //!
    static QIcon getIcon ( const QString &parameterTypeName );

public: // constructors and destructors

    //!
    //! Constructor of the ParameterAction class.
    //!
    //! \param parameter The parameter the action represents.
    //! \param parent The object that the action will be a child of.
    //!
    ParameterAction ( Parameter *parameter, QObject *parent = 0 );

    //!
    //! Destructor of the ParameterAction class.
    //!
    virtual ~ParameterAction ();

public: // functions

    //!
    //! Returns the parameter the action represents.
    //!
    Parameter * getParameter () const;

private: // data

    //!
    //! The parameter the action represents.
    //!
    Parameter *m_parameter;

};

} // end namespace Frapper

#endif
