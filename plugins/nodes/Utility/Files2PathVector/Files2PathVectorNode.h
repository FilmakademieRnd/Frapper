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
//! \file "Files2PathVectorNode.h"
//! \brief Header file for TimerNode class.
//!
//! \author     Jonas Trottnow <jtrottno@filmakademie.de>
//! \version    1.0
//! \date       21.05.2014 (last updated)
//!

#ifndef FILES2PATHVECTORNODE_H
#define FILES2PATHVECTORNODE_H

#include <QFileDialog>
#include "Node.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace Files2PathVectorNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class Files2PathVectorNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

private:


public: // constructors and destructors

    //!
    //! Constructor of the Files2PathVectorNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    Files2PathVectorNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the Files2PathVectorNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~Files2PathVectorNode ();

protected: // events

private slots:

    //! 
    //! Slot for the timer (de-)activation
    //! 
    void readFilePathes();


private: //funtions

private: // data
	QStringList *pathList;
};

} // namespace Files2PathVector

#endif
