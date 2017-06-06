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
//! \file "MotionDataNode.h"
//! \brief Header file for MotionDataNode class.
//!
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       18.05.2009 (last updated)
//!

#ifndef MOTIONDATANODE_H
#define MOTIONDATANODE_H

#include "FrapperPrerequisites.h"
#include "CurveEditorDataNode.h"

namespace Frapper {

//!
//! Base class for nodes that can load motion data.
//!
class FRAPPER_CORE_EXPORT MotionDataNode : public CurveEditorDataNode
{

    Q_OBJECT

public:

    //!
    //! Constructor of the MotionDataNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    MotionDataNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the MotionDataNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~MotionDataNode ();

public: // functions

    //!
    //! Returns the maximum number of parameters in the motion data set.
    //!
    //! \return The maximum number of parameters in the motion data set.
    //!
    inline unsigned int getMaxNumberParameters () const;

    //!
    //! Returns the maximum number of keys in databse
    //!
    //! \return The maximum number of keys.
    //!
    unsigned int getMaxNumberKeys () const;

protected: // functions

    //!
    //! Clears all data from the motion data database.
    //!
    void clearDatabase ();

    //!
    //! Disables paramters in Database based on the DisabledParameters enumeration.
    //!
    void disableParameters ();

    //!
    //! Updates a list containing the disabled parameter names 
    //!
    void updateDisabledParameters ();

    //!
    //! Check list containing the disabled parameters and add or remove the given
    //!
    inline void updateDisabledList (const AbstractParameter *parameter, QStringList &list);


protected: // data

    //!
    //! The maximum number of parameters contained in the motion data set.
    //!
    unsigned int m_maxNumberParameters;

    //!
    //! The maximum number of keys contained in the motion data set.
    //!
    unsigned int m_maxNumberKeys;

    //!
    //! The parameter group containing the motion data and datastructure informations.
    //!
    ParameterGroup *m_dataGroup;

    //!
    //! The parameter group containing the motion data.
    //!
    ParameterGroup *m_databaseGroup;

    //!
    //! The parameter group containing informations about datastructure.
    //!
    ParameterGroup *m_infoGroup;

};

} // end namespace Frapper

#endif
