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
//! \file "AbstractParameter.h"
//! \brief Header file for AbstractParameter class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       08.04.2009 (last updated)
//!

#ifndef ABSTRACTPARAMETER_H
#define ABSTRACTPARAMETER_H

#include "FrapperPrerequisites.h"
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QString>


namespace Frapper {

//
// Forward Declarations
//
class Node;


//!
//! Abstract base class for parameters of a node.
//!
class FRAPPER_CORE_EXPORT AbstractParameter : public QObject
{

    Q_OBJECT

public: // type definitions

    //!
    //! Type definition for a map of parameters and parameter groups.
    //!
    typedef QHash<QString, AbstractParameter *> Map;

    //!
    //! Type definition for a list of parameters and parameter groups.
    //!
    typedef QList<AbstractParameter *> List;

public: // constructors and destructors

    //!
    //! Constructor of the AbstractParameter class.
    //!
    //! \param name The name of the parameter or parameter group.
    //!
    AbstractParameter ( const QString &name );

    //!
    //! Copy constructor of the AbstractParameter class.
    //!
    //! \param parameter The parameter to copy.
    //!
    AbstractParameter ( const AbstractParameter &parameter, Node* node = 0 );

    //!
    //! Destructor of the AbstractParameter class.
    //!
    virtual ~AbstractParameter ();

public: // functions

	//!
    //! Returns the name of the parameter or parameter group.
    //!
    //! \return The name of the parameter or parameter group.
    //!
	const QString &getName () const;

    //!
    //! Returns whether the controls for editing the parameter or parameter
    //! group in the UI should be enabled.
    //!
    //! \return True if the controls should be enabled, otherwise False.
    //!
	bool isEnabled () const;

	//!
    //! Sets name of the parameter.
    //!
    //! \param name The parameter name.
    //!
	virtual void setName ( const QString &name );

    //!
    //! Sets whether the controls for editing the parameter or parameter group
    //! in the UI should be enabled.
    //!
    //! \param enabled The value for the parameter's enabled flag.
    //!
    virtual void setEnabled ( const bool enabled, const bool propagate = true );

    //!
    //! Returns the node to which this parameter or parameter group belongs.
    //!
    //! \return The node to which the parameter or parameter group belongs.
    //!
	virtual Node * getNode () const;

    //!
    //! Sets the node to which this parameter or parameter group belongs.
    //!
    //! \param node The node to which the parameter or parameter group belongs.
    //!
	virtual void setNode ( Node *node );

    //!
    //! Creates an exact copy of the parameter.
    //!
    //! \return An exact copy of the parameter.
    //!
    virtual AbstractParameter * clone () = 0;

    //!
    //! Returns whether the parameter object derived from this class is a
    //! parameter group.
    //!
    //! \return True if the parameter object is a parameter group, otherwise False.
    //!
    virtual bool isGroup () const = 0;

	//!
	//! Returns whether the parameter object derived from this class is visible
	//!
	//! \return True if the parameter object is visible, otherwise False.
	//!
	virtual bool isVisible () const = 0;

    //!
    //! Resets the parameter to its default value or parameters contained in
    //! the group to their default values.
    //!
    virtual void reset () = 0;

signals:  // signals

    //!
    //! Signal that is emitted when the parameter is enabled or disabled
    //!
    void enabledChanged ();


protected: // data

    //!
    //! Flag that states whether the controls for editing the parameter or
    //! parameter group in the UI should be enabled.
    //!
    bool m_enabled;

    //!
    //! The name of the parent node that this parameter or parameter group
    //! belongs to.
    //!
    Node *m_node;

	//!
    //! The name of the parameter or parameter group.
    //!
    QString m_name;
};

} // end namespace Frapper

#endif
