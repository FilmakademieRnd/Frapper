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
//! \file "Connection.cpp"
//! \brief Implementation file for Connection class.
//! \see Node
//!
//! This class implements a connection between parameters of nodes.
//! A connection can be established between an output parameter of a source
//! node and an input parameter of a target node.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       17.05.2009 (last updated)
//!

#include "Connection.h"
#include "Parameter.h"
#include "Node.h"
#include "Log.h"

namespace Frapper {

INIT_INSTANCE_COUNTER(Connection)

///
/// Private Static Data
///


//!
//! The ID to give to the Connection that is created next.
//!
Connection::ID Connection::s_currentId = 0;


///
/// Public Constructors
///


//!
//! Constructor of the Connection class.
//!
//! \param sourceParameter The parameter of the source node that this connection is connected to.
//! \param targetParameter The parameter of the target node that this connection is connected to.
//!
Connection::Connection ( Parameter *sourceParameter, Parameter *targetParameter ) :
m_id(s_currentId),
m_sourceParameter(sourceParameter),
m_targetParameter(targetParameter)
{
    updateName();

    ++s_currentId;

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the Connection class.
//!
Connection::~Connection ()
{
    if (m_sourceParameter)
        m_sourceParameter->removeConnection(m_id);

    if (m_targetParameter)
        m_targetParameter->removeConnection(m_id);

    // notify connected objects that the connection is being destroyed
    //emit destroyed(this);

    DEC_INSTANCE_COUNTER
}

///
/// Public Functions
///


//!
//! Returns the connection's ID.
//!
//! \return The connection's ID.
//!
Connection::ID Connection::getId () const
{
    return m_id;
}


//!
//! Returns whether this connection is connected to an output parameter.
//!
//! \return True if this connection is connected to an output parameter, otherwise False.
//!
bool Connection::hasSource () const
{
    return m_sourceParameter != 0;
}


//!
//! Returns the source parameter that this connection is connected to.
//!
//! \return The source parameter that this connection is connected to.
//!
Parameter * Connection::getSourceParameter () const
{
    return m_sourceParameter;
}


//!
//! Sets the source parameter that this connection should be connected to.
//!
//! \param sourceParameter The source parameter that this connection should be connected to.
//!
void Connection::setSourceParameter ( Parameter *sourceParameter )
{
    m_sourceParameter = sourceParameter;
    updateName();
}


//!
//! Returns whether this connection is connected to an input parameter of a
//! target node.
//!
//! \return True if this connection is connected to an input parameter, otherwise False.
//!
bool Connection::hasTarget () const
{
    return m_targetParameter != 0;
}


//!
//! Returns the target parameter that this connection is connected to.
//!
//! \return The target parameter that this connection is connected to.
//!
Parameter * Connection::getTargetParameter () const
{
    return m_targetParameter;
}


//!
//! Sets the target parameter that this connection should be connected to.
//!
//! \param targetParameter The target parameter that this connection should be connected to.
//!
void Connection::setTargetParameter ( Parameter *targetParameter )
{
    m_targetParameter = targetParameter;
    updateName();
}


///
/// Private Functions
///


//!
//! Updates the name of the connection according to its source and target
//! parameters.
//!
void Connection::updateName ()
{
    QString name = "Connection ";
    if (m_sourceParameter && m_sourceParameter->getNode())
        name += QString("%1.%2").arg(m_sourceParameter->getNode()->getName(), m_sourceParameter->getName());
    name += " -> ";
    if (m_targetParameter && m_targetParameter->getNode())
        name += QString("%1.%2").arg(m_targetParameter->getNode()->getName(), m_targetParameter->getName());
    setObjectName(name);
}

} // end namespace Frapper