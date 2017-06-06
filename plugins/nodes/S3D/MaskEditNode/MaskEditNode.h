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
//! \file "MaskEditNode.cpp"
//! \brief Header file for MaskEditNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!

#ifndef MASKEDITNODE_H
#define MASKEDITNODE_H

#include "ImageNode.h"
#include "GenericParameter.h"
#include "PainterPanelNode.h"

#include <charon-core/ParameteredObject.h>
#include <charon-utils/CImg.h>

#include <../CharonInterface/CharonInterface.h>

namespace MaskEditNode {

template <typename T>
class MaskEditNodeParameteredObject : public TemplatedParameteredObject<T> {

public:
	MaskEditNodeParameteredObject(const std::string& name = "");

private:
	InputSlot  < cimg_library::CImgList<T> > images_in;
	OutputSlot < cimg_library::CImgList<T> > masks_out;
};

//!
//! Base class for all render nodes.
//!
class MaskEditNode : public PainterPanel::PainterPanelNode
{

    Q_OBJECT

public: // constructor and destructor

    //!
    //! Constructor of the MaskEditNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
	MaskEditNode ( const QString &name, Frapper::ParameterGroup *parameterRoot );

	//!
    //! Destructor of the MaskEditNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~MaskEditNode ();

	//!
	//! PainterPanelNode methods
	//!
	virtual Ogre::TexturePtr getBackground();
	virtual Ogre::TexturePtr getMask();
	virtual Frapper::ParameterGroup* getItemParameterGroup();
	virtual Frapper::Node* getNode();

	virtual void onSceneChanged();
	virtual void setUpTimeDependencies( Frapper::NumberParameter *timeParameter, Frapper::NumberParameter *rangeParameter );

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
    //! Changes the template type of this node.
    //!
	void changeTemplateType ();

	//!
	//! Updates the output image
	//!
	void processOutputImage();

	//!
	//! Slot which is called when the input image list changes
	//!
	void inputImagesChanged();

	//!
	//! Slot to set options specific for layers
	//!
	void updateEdgeLayer();

	//!
	//! Render current frame
	//!
	void renderFrame();

	//!
	//! Render a sequence out output masks
	//!
	void renderSequence();

	//!
	//! Render current frame
	//!
	void clearFrame();

	//!
	//! Render a sequence out output masks
	//!
	void clearSequence();

	//!
	//!
	//!
	void updateOutput();

	//!
	//!
	//!
	void maskInParameterConnected();

	//!
	//! delete a connection
	//!
	void deleteConnection( Frapper::Connection *connection );

public slots:

	//!
	//! Slot to set options specific for this node
	//!
	void updateOnFrameChanged();

	//!
	//! Slot to set options specific for this node
	//!
	void useLUTChanged();

	//!
	//! Slot to set options specific for this node
	//!
	void darkenBackgroundChanged();


protected: // functions

	//!
	//! Load image given by parameter name from position in input list given by the current frame number
	//!
	//! \param imageName The name of the image parameter to load the image to
	//! \param slotName The name of the slot to load the image from
	//!
	void imageFromList ( Ogre::TexturePtr& image, const QString& slotName, unsigned int frameIndex );

	//!
	//! Write an image from given parameter name to position in output list given by current frame number
	//!
	//! \param imageName The name of the image parameter to read the image from
	//! \param slotName The name of the slot to write the image to
	//!
	void imageToMasksList( Ogre::TexturePtr& image );


	//!
	//! Maps current selected frame to the correspondent image index in the given image list of size nImages
	//! according to m_currentFrame - m_timeParameter->getMinValue().toInt();
	//!
	//! \param nImages The number of images in the image list
	//!
	unsigned int mapFrameToImage();

private: // functions
	
	//!
	//! Init default output textures
	//! 
	void initTextures();

	size_t getImageListSize( QString slotName );
	
	void processWorkflow();

	void setTimeparameterAffection(bool enabled);

private: // data

	// The charon parametered object, handles slots and processing
	CharonInterface* m_charonInterface;

	// image and mask parameter
	Frapper::Parameter* m_imageParameter;

	Ogre::TexturePtr m_defaultImage;
	Ogre::TexturePtr m_inputImage;
	Ogre::TexturePtr m_inputEdge;
	Ogre::TexturePtr m_inputMask;
	Ogre::TexturePtr m_outputMask;

	int m_currentFrame;
	int m_selectedType;
	bool m_maskChanged;

	bool m_sceneChanged;
};



} // end namespace

#endif

