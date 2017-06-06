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
//! \file "BackDropNode.h"
//! \brief Header file for BackDropNode class.
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       06.03.2014 (last updated)

#ifndef BACKDROPNODE_H
#define BACKDROPNODE_H

#include "Node.h"

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace Frapper {

//!
//! Class for a node that is used for debugging purposes.
//!
class FRAPPER_CORE_EXPORT BackDropNode : public Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the BackDropNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    BackDropNode ( const QString &name, ParameterGroup *parameterRoot, const QPointF position = QPointF(0.f, 0.f), const qreal width = 200, const qreal height = 200, const QColor backgroundColor = QColor(48,48,48), const QColor textColor = QColor(255,255,255), const QString text = "", const int textSize = 11, const QString head = "", const int headSize = 22, const bool lockChange = false );

    //!
    //! Destructor of the BackDropNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~BackDropNode ();

signals:

	void graphicDrawSignal();

public slots:

	void drawGraphics();

	void lockParameter();
};

} // namespace Frapper 

#endif // BACKDROPNODE_H
