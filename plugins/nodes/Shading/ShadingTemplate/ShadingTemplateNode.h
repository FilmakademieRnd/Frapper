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
//! \file "ShadingTemplateNode.h"
//! \brief Header file for ShadingTemplateNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       02.11.2011 (last updated)
//!

#ifndef SHADINGTEMPLATENODE_H
#define SHADINGTEMPLATENODE_H

#include "CompositorNode.h"
#include "InstanceCounterMacros.h"

namespace ShadingTemplateNode {
using namespace Frapper;

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class ShadingTemplateNode : public CompositorNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the ShadingTemplateNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    ShadingTemplateNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ShadingTemplateNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ShadingTemplateNode ();

protected slots:
	
	////!
	////! Reload resources.
	////!
	void reload ();

protected: // functions

    //!
    //! Callback when instance of this class is registered as Ogre::ShadingListener.
    //! 
    //! \param pass_id Id to identifiy current shading pass.
    //! \param mat Material this pass is currently using.
    //!
    virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);

private slots:

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

private: // members
    
    NumberParameter* m_resX;
    NumberParameter* m_resY;
};

} // namespace ShadingTemplateNode
#endif
