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
//! \file "StereoViewNode.h"
//! \brief Header file for StereoViewNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.2
//! \date       17.12.2012 (last updated)
//!

#ifndef STEREOVIEWNODE_H
#define STEREOVIEWNODE_H

#include "CompositorNode.h"
#include "StereoViewWindow.h"
#include <QString>

namespace StereoViewNode {
	using namespace Frapper;

//!
//! Class implementing the StereoView Node
//!
class StereoViewNode : public CompositorNode
{
    Q_OBJECT

public: // Enumerations
	enum ViewMode {
		NVISION,
		INTERLACED
	};

public: // constructors and destructors

    //!
    //! Constructor of the StereoViewNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    StereoViewNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the StereoViewNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~StereoViewNode ();

protected: // callbacks

	//!
	//! Callback when instance of this class is registered as Ogre::CompositorListener.
	//! 
	//! \param pass_id Id to identifiy current compositor pass.
	//! \param mat Material this pass is currently using.
	//!
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);

private slots:

	
	//!
	//! Process the output image, which also updates the StereoView Window
	//!
	void processOutputImage();

	//!
	//! Open the Stereo Window which displays the given input images in a DX9 context to support NVidia NVision
	//!
	void openStereoWindow();

	void enableStereo();
	void swapEyes();
	void setFullscreenMode();

	void leftImageDirty();
	void rightImageDirty();

	void updateView();
	void ViewModeChanged();


private: // functions

	void setupTextures( Ogre::TexturePtr &leftTexture, Ogre::TexturePtr &rightTexture );
	void load3DVisionImage();
	void loadStereoTestImage();

private: //data

	//!
	//! The Stereo Window which displays the given input images in a DX9 context to support NVidia NVision
	//!
	StereoViewWindow *m_window;

	bool m_leftInputImageDirty;
	bool m_rightInputImageDirty;

	ViewMode m_viewMode;

	//! The test images to be shown on creation
	Ogre::TexturePtr m_testImageLeft;
	Ogre::TexturePtr m_testImageRight;

	//! The 3D Vision image to be shown in the viewport when 3D Vision is enabled
	Ogre::TexturePtr m_NV3DImage;


};

} // end namespace

#endif // STEREOVIEWNODE_H
