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
//! \file "Index2IndexNode.h"
//! \brief Header file for Index2IndexNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    0.1
//! \date       09.08.2013 (last updated)
//!

#ifndef INDEX2INDEXNODE_H
#define INDEX2INDEXNODE_H

#include "Node.h"
#include <QtCore/QTimer>

namespace Index2IndexNode {
using namespace Frapper;

//!
//! Class for a node that is used for debugging purposes.
//!
class Index2IndexNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the Index2IndexNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    Index2IndexNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the Index2IndexNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~Index2IndexNode ();

private slots: //

	//!
    //! Creates the input parameters for the node.
    //!
	void setupParameters();


	//!
    //! Sets the parameter values.
    //!
	void setParameters();

	//!
	//! add another output slot
	//!
	void addOutput();

	//!
	//! delete an output slot
	//!
	void deleteOutput();

	//!
	//! specifies a threshold above which the mapping will be performed
	//!
	void setThreshold();

	//!
	//! set the "label output" and the "index output" to empty and 0, so they can be re-written with the same value
	//!
	void cleanOutputs();


signals:
	void emitOutput(float viseme);

private:  // data
	
	ParameterGroup	*m_inputValues;
	ParameterGroup *parameterRoot; // pointer to this Parameter Root

	Parameter *m_threshold;
	Parameter *m_outIndex;
	Parameter *m_outName ;

	QList <int> m_Outputs; // number of outputs
};

} // namespace Index2IndexNode 

#endif
