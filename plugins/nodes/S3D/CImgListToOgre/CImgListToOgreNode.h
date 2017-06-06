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
//! \file "CImgListToOgreNode.cpp"
//! \brief Header file for CImgListToOgreNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!

#ifndef CIMGLISTTOOGRENODE_H
#define CIMGLISTTOOGRENODE_H

#include "ImageNode.h"
#include "GenericParameter.h"
#include <charon-utils/CImg.h>
#include <charon-core/ParameteredObject.h>
#include <../CharonInterface/CharonInterface.h>



namespace CImgListToOgreNode {
	using namespace Frapper;

template <typename T>
class CImgListToOgrePO : public TemplatedParameteredObject<T> {

public:
	CImgListToOgrePO(const std::string& name = "");

private:
	InputSlot < cimg_library::CImgList<T> > image_in;
};

//!
//! Base class for all render nodes.
//!
class CImgListToOgreNode : public Frapper::ImageNode
{

    Q_OBJECT

public: // constructor and destructor

    //!
    //! Constructor of the CImgListToOgreNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	CImgListToOgreNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the CImgListToOgreNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CImgListToOgreNode ();

public:  //functions
	
	virtual void setUpTimeDependencies( NumberParameter *timeParameter, NumberParameter *rangeParameter );

private slots: 

	//!
	//! Connects Frapper in/out parameter with Charon in/out slots.
	//!
	void connectSlots ();

	//!
	//! Disonnects Frapper in/out parameter with Charon in/out slots.
	//!
	void disconnectSlots (int connenctionID);

	//!
    //! Initiates the Charon internal processing chain.
    //!
    void processOutputImage ();

	//!
    //! Changes the Charon plugin template type.
    //!
	void changeTemplateType ();

	//!
	//! Updates the local frame number and triggers blit
	//!
	void updateFrame(int frame);

	void setUpdateOption();
	void initOutputTexture();

	//!
	//! delete a connection
	//!
	void deleteConnection( Frapper::Connection *connection );

private: // data

	Ogre::String m_renderImageName;

	// Interface to Charon functions
	CharonInterface* m_charonInterface;

	Frapper::GenericParameter* m_inputParameter;

	Ogre::TexturePtr m_defaultTexture;

	int m_currentFrame;
	int m_selectedType;
};

} // end namespace

#endif

