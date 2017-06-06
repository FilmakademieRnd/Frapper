/*
-----------------------------------------------------------------------------
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
//! \file "LightWarpingNode.cpp"
//! \brief Interface file for LightWarpingNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.1
//! \date       08.12.2009 (last updated)
//!


#ifndef LIGHTWARPINGNODE_H
#define LIGHTWARPINGNODE_H

#include "CompositorNode.h"
#include "InstanceCounterMacros.h"

namespace LightWarpingNode {
using namespace Frapper;

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class LightWarpingNode : public CompositorNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the LightWarpingNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    LightWarpingNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the LightWarpingNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~LightWarpingNode ();

protected: // functions

    //!
    //! Callback when instance of this class is registered as Ogre::CompositorListener.
    //! 
    //! \param pass_id Id to identifiy current compositor pass.
    //! \param mat Material this pass is currently using.
    //!
    virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);

public:
	//!
    //! Redraw of ogre scene has been triggered.
    //!
	void redrawTriggered ();

private slots:

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

	//!
    //! Loads an XML reference data file.
    //!
    //! Is called when the value of the Reference Data File parameter has
    //! changed.
    //!
    //! \return True if the reference data was successfully loaded from file, otherwise False.
    //!
    bool loadLightDescriptionFile ();

public: // members

	Ogre::TexturePtr m_lightDirsTexture;
	Ogre::TexturePtr m_lightPowsTexture;

};

} // namespace LightWarpingNode 

#endif
