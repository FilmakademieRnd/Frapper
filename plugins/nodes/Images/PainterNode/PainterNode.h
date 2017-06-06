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
//! \file "PainterNode.h"
//! \brief Header file for PainterNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       03.04.2009 (last updated)
//!

#ifndef PAINTERNODE_H
#define PAINTERNODE_H

#include "InstanceCounterMacros.h"
#include "PainterPanelNode.h"

namespace PainterNode {
using namespace Frapper;

//!
//! Class representing nodes that load image data from file.
//!
class PainterNode : public PainterPanel::PainterPanelNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PainterNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    PainterNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the PainterNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    ~PainterNode ();

public:

	//!
	//! Get the background image of the node
	//!
	Ogre::TexturePtr getBackground();

	//!
	//! Get the mask image of the node
	//!
	Ogre::TexturePtr getMask();

	//!
	//! Get the node parameter group that stores values of painter panel items
	//!
	Frapper::ParameterGroup* getItemParameterGroup();

	//!
	//! Get the Node that uses this interface, e.g. for Signal/Slot processing
	//!
	Frapper::Node* getNode();

	//!
	//!
	//!
	void onSceneChanged();

	//!
	//!
	//!
	virtual void setUpTimeDependencies( NumberParameter *timeParameter, NumberParameter *rangeParameter );


private slots: //

    //!
    //! Processes the node's input data to generate the node's output image.
    //!
    void processOutputImage ();

	//!
	//!
	//!
	void resetOutputImage();

	//!
	//!
	//!
	void setOptions();

private: // functions

	//!
	//!
	//!
	Ogre::TexturePtr m_outputImage;

	//!
	//! The parameter where painted items are stored
	//!
	Frapper::ParameterGroup* m_ppItemsParameterGroup;

};

} // namespace PainterNode

#endif
