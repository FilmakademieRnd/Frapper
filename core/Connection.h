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
//! \file "Connection.h"
//! \brief Header file for Connection class.
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include "FrapperPrerequisites.h"
#include <QtCore/QMap>
#include "InstanceCounterMacros.h"

namespace Frapper {

//!
//! Forward declaration for Parameter class.
//!
class Parameter;

//!
//! Forward declaration for Node class.
//!
class Node;


//!
//! Class representing connections between nodes (or rather between parameters
//! of nodes).
//!
class FRAPPER_CORE_EXPORT Connection : public QObject
{

    Q_OBJECT
        ADD_INSTANCE_COUNTER

public: // type definitions

    //!
    //! Type definition for a connection's identifier.
    //!
    typedef unsigned int ID;

    //!
    //! Type definition for a map of connections with connection ID's as keys.
    //!
    typedef QMap<ID, Connection *> Map;

private: // static data

    //!
    //! The ID to give to the connection that is created next.
    //!
    static ID s_currentId;

public: // constructors and destructors

    //!
    //! Constructor of the Connection class.
    //!
    //! \param sourceParameter The parameter of the source node that this connection is connected to.
    //! \param targetParameter The parameter of the target node that this connection is connected to.
    //!
    Connection ( Parameter *sourceParameter, Parameter *targetParameter );

    //!
    //! Destructor of the Connection class.
    //!
    ~Connection ();

public: // functions

    //!
    //! Returns the connection's ID.
    //!
    //! \return The connection's ID.
    //!
    ID getId () const;

    //!
    //! Returns whether this connection is connected to an output parameter of
    //! a source node.
    //!
    //! \return True if this connection is connected to an output parameter, otherwise False.
    //!
    bool hasSource () const;

    //!
    //! Returns the source parameter that this connection is connected to.
    //!
    //! \return The source parameter that this connection is connected to.
    //!
    Parameter * getSourceParameter () const;

    //!
    //! Sets the source parameter that this connection should be connected to.
    //!
    //! \param sourceParameter The source parameter that this connection should be connected to.
    //!
    void setSourceParameter ( Parameter *sourceParameter );

    //!
    //! Returns whether this connection is connected to an input parameter of a
    //! target node.
    //!
    //! \return True if this connection is connected to an input parameter, otherwise False.
    //!
    bool hasTarget () const;

    //!
    //! Returns the target parameter that this connection is connected to.
    //!
    //! \return The target parameter that this connection is connected to.
    //!
    Parameter * getTargetParameter () const;

    //!
    //! Sets the target parameter that this connection should be connected to.
    //!
    //! \param targetParameter The target parameter that this connection should be connected to.
    //!
    void setTargetParameter ( Parameter *targetParameter );

signals: //

    //!
    //! Signal that is emitted when the connection is being destroyed.
    //!
    void destroyed (Connection *connection);

private: // functions

    //!
    //! Updates the name of the connection according to its source and target
    //! parameters.
    //!
    void updateName ();

private: // attributes

    //!
    //! The ID of the connection.
    //!
    ID m_id;

    //!
    //! The parameter of the source node that this connection is connected to.
    //!
    Parameter *m_sourceParameter;

    //!
    //! The parameter of the target node that this connection is connected to.
    //!
    Parameter *m_targetParameter;

};

} // end namespace Frapper

#endif
