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
//! \file "TridelityNode.h"
//! \brief Header file for TridelityNode class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       16.10.2009 (last updated)
//!

#ifndef TRIDELITYNODE_H
#define TRIDELITYNODE_H

#include "CompositorNode.h"
#include "InstanceCounterMacros.h"

namespace TridelityNode {
using namespace Frapper;

//!
//! Class implementing Screen Space Ambient Occlusion operations.
//!
class TridelityNode : public CompositorNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the TridelityNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TridelityNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the TridelityNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TridelityNode ();

protected: // functions

    //!
    //! Callback when instance of this class is registered as Ogre::CompositorListener.
    //! 
    //! \param pass_id Id to identifiy current compositor pass.
    //! \param mat Material this pass is currently using.
    //!
    virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);

    void SetupTextures();

    Ogre::TexturePtr CreateLookupTexture();

public:

private slots:


    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

    //!
    //! Redraw of ogre scene has been triggered.
    //!
    void redrawTriggered ();

private: // members

    //!
    //! The pixel format of the output texture.
    //!
    Ogre::CompositorInstance *m_compositors[2];

    Ogre::PixelFormat m_pixelFormat;
    Ogre::TexturePtr m_lookupTexture;
    NumberParameter* m_resX;
    NumberParameter* m_resY;

    static const int lookupX;
    static const int lookupY;
    static const int lookupML[5][15];
    static const int lookupMV[5][15];

    int m_screenType;

};

} // namespace TridelityNode
#endif
