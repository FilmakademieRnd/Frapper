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
//! \file "KeyboardNode.h"
//! \brief Header file for KeyboardNode class.
//!
//! \author     Michael Bussler <michael.bussler@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       19.05.2009 (last updated)
//!

#ifndef KEYBOARDNODE_H
#define KEYBOARDNODE_H

#include "Node.h"
#include "Parameter.h"


namespace KeyboardNode {
using namespace Frapper;

//!
//! Class representing nodes that can access Wii controllers.
//!
class KeyboardNode : public Node //, public QThread
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the KeyboardNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    KeyboardNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the KeyboardNode class.
    //!
    ~KeyboardNode ();

protected: // functions

	bool eventFilter(QObject *obj, QEvent *event);

private slots: // slots
	
	void setOptions();

private: // functions

	bool handleKeyPressEvent( int keyId );
    bool handleKeyReleaseEvent( int keyId );
	
	void initParameterMap();

private: // data

	QMap<int, Parameter*> m_parameterMap;
	bool m_eatKeys;
};

} // namespace KeyboardNode

#endif
