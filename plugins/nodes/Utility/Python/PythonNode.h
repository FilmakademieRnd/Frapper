/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "PythonNode.h"
//! \brief Header file for PythonNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       18.05.2009 (last updated)
//!

#ifndef PYTHONNODE_H
#define PYTHONNODE_H

#include "Node.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

// Python API
#ifdef _DEBUG
/* We don't want to debug Python and most distribution packages come without python debug library
 * Be sure to include python.h _after_ including standart headers like <stdio.h>, <string.h>, <errno.h>, 
 * <limits.h>, <assert.h> and <stdlib.h>, otherwise you will get strange linker errors. */
	#undef _DEBUG
		#include <Python.h> 
	#define _DEBUG
#else
	#include <Python.h>
#endif

namespace PythonNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class PythonNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the PythonNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    PythonNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the PythonNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PythonNode ();

protected slots:

	void runScript();

};

} // namespace PythonNode 

#endif
