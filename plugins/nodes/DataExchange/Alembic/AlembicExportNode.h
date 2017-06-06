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
//! \file "AlembicExportNode.h"
//! \brief Header file for AlembicExportNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       11.04.2013 (last updated)
//!

#ifndef RESULTDATANODE_H
#define RESULTDATANODE_H

#include "InstanceCounterMacros.h"
#include "Node.h"

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

// Alembic

namespace AbcG = Alembic::AbcGeom;
using namespace AbcG;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;

namespace AlembicExportNode {
	using namespace Frapper;

//!
//! Class representing nodes that can save solver reference data.
//!
class AlembicExportNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the AlembicExportNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AlembicExportNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AlembicExportNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~AlembicExportNode ();

private: // functions

    //!
    //! Creates the COLLADA header with asset information.
    //!
    void buildHeader ();

	//!
    //! Creates the COLLADA header with asset information.
    //!
    //! \param list The source list.
    //! \param vector The destination vector.
    //!
	template <class T>
    void copyListToVector (const QVector<float> &list, std::vector<T> &vector);

	void createAlembicFile(const AbstractParameter::List& buffers, const QString &path, const unsigned int nbrFrames, const unsigned int fps);

private slots: //

    //!
    //! Saves the result data as COLLADA-formatted file.
    //!
    //! \return True if the data was successfully written to file, false otherwise.
    //!
    void saveSolveResultFile ();

private: // data

    //!
    //! The parameter group containing the solving results
    //!
    QList<ParameterGroup *> m_solvingResults;
};

} // end namespace

#endif
