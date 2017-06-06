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
//! \file "AnimatableMeshDECTNode.h"
//! \brief Header file for AnimatableMeshDECTNode class.
//!
//! \author     Volker Helzle <volker.helzle@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.1
//! \date       26.11.2012 (last updated)
//!

#ifndef ANIMATABLEMESHDECTNODE_H
#define ANIMATABLEMESHDECTNODE_H

#include "GeometryAnimationNode.h"
#include "OgreTagPoint.h"
#include "OgreColourValue.h"
#include <vector>
#include "ObjectTextDisplay.h"

#include "RTShaderSystem/OgreRTShaderSystem.h"
#include "RTShaderSystem/OgreShaderRenderState.h"
#include "RTShaderSystem/OgreShaderGenerator.h"
#include "RTShaderSystem/OgreShaderExNormalMapLighting.h"



namespace AnimatableMeshDECTNode {
using namespace Frapper;

class ViewportListener : public Ogre::Viewport::Listener
{
public: 

	ViewportListener() {};
	~ViewportListener() {};

	void viewportDimensionsChanged (Ogre::Viewport *viewport);
};

//!
//! Class representing nodes that can contain OGRE entities with animation.
//!
class AnimatableMeshDECTNode : public GeometryAnimationNode
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructor and destructor

    //!
    //! Constructor of the AnimatableMeshDECTNode class.
    //!
    //! \param name The name to give the new mesh node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AnimatableMeshDECTNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AnimatableMeshDECTNode class.
    //!
    ~AnimatableMeshDECTNode ();

	
protected:
	//!
	//! Creates and makes visible the overlays for AOIs.
	//!
	void createAOIOverlays();

	//!
	//! Updates the position of the overlays and displays them on the viewport. 
	//!
	void modifyAOIOverlays();

	//!
	//! Destroys the overlays for AOIs.
	//!
	void destroyAOIOverlays();	

	//!
	//! Sets the AOI coordinates in the parameter panel.
	//!
	void setAOICoordinate(QPair<int,int> coord, QString bonesListName,  QString nameBone, QString position);

	//!
	//!  Sets the AOI coordinates back to 0 (zero).
	//!
	void resetAOICoordinates();

	//!
	//! Transforms the AOI coordinates to screen pixels using the camera of the RenderNode.
	//!
	QPair<int, int> getBonePixelCoords( Ogre::Bone *bone, QString nameBone, int separator );
	
	//!
	//! Loads the RTShader resources, to create the materials of the manual objects.
	//!
	void loadRTShaderResources();

	//!
	//! Creates the MaterialPtr to be used by the AOI manual objects. 
	//!
	void createAOIOverlayMaterial();

private slots:

	//!
	//! Prepares the environment for the creation of AOIs 
	//!
	void initializeAOIs();

	//!
	//! Creates the data structure that contains the AOIs (structures) with their corresponding parameters
	//! Modifies the overlays, if existent. 
	//!
	void createAOIs();

	//!
	//! Destroys the vector containing the AOIs' coordinates, and the overlays if existing.
	//!
	void destroyAOIs();

	//!
	//! Sets the overlays to be visible or invisible correspondingly.
	//!
	void setOverlays();

	//!
	//! Sets the overlays to be visible or invisible according to the param "visible"
	//!
	void setOverlays(bool visible);

private:

	//!
	//! Viewport width 
	//!
	int		mViewWidth;	

	//!
	//! Viewport height 
	//!
	int		mViewHeight;	

	//!
	//! Camera width
	//!
	int		mCamWidth;	

	//!
	//! Camera height
	//!
	int		mCamHeight;	

	//!
	//! To know if the AOIs were created already
	//!
	bool	mInitiated;
	
	//!
	//! The OGRE entity to be displayed.
	//!
	Ogre::Entity		*mEntity;
	
	//!
	//! The scene manager handling all OGRE objects.
	//!
	Ogre::SceneManager	*mSceneMan;

	//!
	//! The camera in the scene.
	//!
	Ogre::Camera		*mCamera;

	//!
	//! The skeleton of the OGRE entity.
	//!
	Ogre::Skeleton *mSkeleton;

	//!
	//! The OGRE Run Time Shader System, used to generate shaders on the fly based on object material properties, scene setup and other user definitions.
	//!
	Ogre::RTShader::ShaderGenerator *mShaderGenerator;

	//!
	//! The name of the material for the overlays (AOIs).
	//!
	Ogre::String moMaterial;

	//!
	//! Each of the bones groups related to AOIs.
	//!
	QList<AbstractParameter*> mBonesParamsList;

	//!
	//! Coordinates of the positions of each bone in the skeleton.
	//!
	std::vector<QList<Ogre::Vector3>*>	mBonesPositions;

	//!
	//! Manual Objects containing the overlays of AOIs.
	//!
	std::vector<Ogre::ManualObject*>	mAOIOverlays;

	//!
	//! Bones from the XML description file.
	//!
	ParameterGroup	*mBonesParams;

	//!
	//! Viewport listener.
	//!
	ViewportListener *m_viewportListener;

};

} // namespace AnimatableMeshDECTNode
#endif
