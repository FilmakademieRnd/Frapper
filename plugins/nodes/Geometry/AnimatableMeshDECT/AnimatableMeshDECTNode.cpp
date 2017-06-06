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
//! \brief Implementation file for AnimatableMeshDECTNode class.
//!
//! \author     Diana Arellano <diana.arellano@filmakademie.de>
//! \version    1.1
//! \date       07.08.2015 (last updated)
//!

#include "AnimatableMeshDECTNode.h"

namespace AnimatableMeshDECTNode {
using namespace Frapper;


INIT_INSTANCE_COUNTER(AnimatableMeshDECTNode)

Ogre::Viewport *currentViewport;

void ViewportListener::viewportDimensionsChanged(Ogre::Viewport *viewport)
{
	//std::cout << "DDD viewportDimensionsChanged " <<  std::endl;
}

///
/// Constructors and Destructor
///

//!
//! Constructor of the AnimatableMeshDECTNode class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
AnimatableMeshDECTNode::AnimatableMeshDECTNode ( const QString &name, ParameterGroup *parameterRoot ) :
	GeometryAnimationNode(name, parameterRoot),
	mInitiated(0),
	mCamera(NULL)
{
	// get the bones from the XML
	mBonesParams = getParameterGroup("Bones for AOIs");

	// get the Ogre Scene Manager
	mSceneMan = OgreManager::getSceneManager();	

	//add affection 
	getParameter("AOIs")->addAffectingParameter(getParameter("Visible"));
	
	// load the RTShader resources
	loadRTShaderResources();

	// create material for the AOI Overlays
	createAOIOverlayMaterial();

	setProcessingFunction("AOIs", SLOT(initializeAOIs()));
	setChangeFunction("Show AOIs", SLOT(setOverlays()));

	// listener for viewport 
	m_viewportListener = new ViewportListener();	
	Ogre::Viewport *v = mSceneMan->getCurrentViewport();
	v->addListener(m_viewportListener);
}

//!
//! Destructor of the AnimatableMeshDECTNode class.
//!
AnimatableMeshDECTNode::~AnimatableMeshDECTNode ()
{
}

//!
//! Prepares the environment for the creation of AOIs 
//!
void AnimatableMeshDECTNode::initializeAOIs()
{
	bool isVisible = getValue("Visible").toBool(); 
	bool isActive  = getValue("AOIs").toBool();

	// first check visibility of the mesh
	if (!isVisible) {
		setValue("AOIs", false);
		isActive = 0;		
	}

	if (isActive) 
	{
		if (!mSceneMan->hasCamera("cameraCamera")) 		
			return; 	

		if (!mBonesParams)
			return;

		if (mInitiated == 0) {		// attach processing function to the Animation Parameters
			ParameterGroup *param = getParameterGroup("Animation Parameter");
			if (param)
			{
				ParameterGroup *skeletalAnims = param->getParameterGroup("Skeletal Animations");
				if (skeletalAnims)
				{					
					AbstractParameter::List params= skeletalAnims->getParameterList();
					foreach(AbstractParameter *p, params)
					{
						Parameter *anim = dynamic_cast<Parameter*>(p);
						connect(anim, SIGNAL(valueChanged()), this, SLOT(createAOIs()));
					}					
				}
			}
			mInitiated = 1;
		}
		
		mEntity = mSceneMan->getEntity(m_name.toStdString());
		mSkeleton = mEntity->getSkeleton();

		// Get each of the bones groups related to AOIs
		mBonesParamsList = mBonesParams->getParameterList();

		mCamera = mSceneMan->getCamera("cameraCamera");		

		// Creating the AOIs
		Ogre::Viewport *viewport = mSceneMan->getCurrentViewport();
		Ogre::RenderTarget *rt = viewport->getTarget();
		mViewWidth =  rt->getWidth();
		mViewHeight = rt->getHeight();

		OgreContainer *cameraContainer = Ogre::any_cast<OgreContainer *>(((Ogre::MovableObject *) mCamera)->getUserAny());
		mCamWidth = cameraContainer->getCameraWidth();
		mCamHeight = cameraContainer->getCameraHeight();

		createAOIs();
	}
	else 	// destroy AOIs
	{
		destroyAOIs();
		resetAOICoordinates();
		// Destroy AOIs overlays
		if (getValue("Show AOIs").toBool()) 
			destroyAOIOverlays();
	}

	// propagate changes to other nodes
	this->getParameter(m_outputGeometryName)->propagateDirty(true);
}

//!
//! Creates the data structure that contains the AOIs (structures) with their corresponding parameters
//! Modifies the overlays, if existent. 
//!
void AnimatableMeshDECTNode::createAOIs()
{
	if (mCamera == NULL) {
		Log::info("A camera must exist", "AnimatableMeshDECTNode::createAOIs()");
		return;
	}

	if (!getParameter("Visible")->getValue().toBool()) {
		//Log::info("The character must be visible", "AnimatableMeshDECTNode::createAOIs()");
		return;
	}
	else {
		if (getParameter("AOIs")->getValue().toBool() == 0) {
			//Log::info("AOIs parameter is not activated", "AnimatableMeshDECTNode::createAOIs()");
			return;
		}
	}

	// first destroy AOIs if existent
	destroyAOIs();

	mSkeleton = mEntity->getSkeleton();

	foreach(AbstractParameter* bonesGroup, mBonesParamsList)
	{
		ParameterGroup *bonesParameterGroup = dynamic_cast<ParameterGroup*>(bonesGroup);

		if(bonesParameterGroup) 
		{
			QList<AbstractParameter*> bonesList = bonesParameterGroup->getParameterList();

			QString bonesListName = bonesParameterGroup->getName();

			QList<Ogre::Vector3> *positionsbonesAOI = new QList<Ogre::Vector3>();

			foreach(AbstractParameter* b, bonesList)
			{
				Parameter *boneParam = dynamic_cast<Parameter*>(b);	//this is the bone 
				QString nameBone = boneParam->getName();
				Ogre::Bone *bone;
				
				int separator = nameBone.indexOf('-');
				if (separator != -1)
				{
					QString position = nameBone.right(nameBone.size() - separator - 1);
					QString nBone = nameBone.left(separator);

					if (!mSkeleton->hasBone(nBone.toStdString()))
						continue;

					bone = mSkeleton->getBone(nBone.toStdString());

					//processing for the "Forehead" of the character  -- no direct mapping between bones and AOI region
					if(bonesListName.compare("Forehead") == 0)
					{						
						if (nBone.contains("RUFH"))
							bone->_setDerivedPosition(Ogre::Vector3(QVariant(bone->_getDerivedPosition().x - 1.0).toReal(), QVariant(bone->_getDerivedPosition().y + 1.0).toReal(), bone->_getDerivedPosition().z));
						
						if (nBone.contains("LUFH"))						
							bone->_setDerivedPosition(Ogre::Vector3(QVariant(bone->_getDerivedPosition().x + 1.0).toReal(), QVariant(bone->_getDerivedPosition().y + 1.0).toReal(), QVariant(bone->_getDerivedPosition().z).toReal()));
						
						if (nBone.contains("RUBR"))						
							bone->_setDerivedPosition(Ogre::Vector3(QVariant(bone->_getDerivedPosition().x - 1.5).toReal(), bone->_getDerivedPosition().y, bone->_getDerivedPosition().z));
						
						if (nBone.contains("LUBR"))						
							bone->_setDerivedPosition(Ogre::Vector3(QVariant(bone->_getDerivedPosition().x + 1.5).toReal(), bone->_getDerivedPosition().y, bone->_getDerivedPosition().z));
					}


					// Transforms the AOI coordinates to screen pixels.
					QPair<int, int> coord = getBonePixelCoords(bone, nBone, separator);	

					if (coord.first == -9999)
						continue;

					setAOICoordinate(coord, bonesListName, nameBone, position);	//need the nameBone which contains the position of the bone (upper, left, right, lower)
				}
		
				positionsbonesAOI->append(bone->_getDerivedPosition());
				bone->reset();		// to reset to the original derived position in case it changed
			}
			mBonesPositions.push_back(positionsbonesAOI);
		}
	}

	// Modify the AOI overlay in case it is activated.
	if (getValue("Show AOIs").toBool())
		modifyAOIOverlays();	
}

//!
//! Destroys the vector containing the AOIs' coordinates, and the overlays if existing.
//!
void AnimatableMeshDECTNode::destroyAOIs()
{
	if(mSceneMan)
	{
		try {
			for (int i=0; i< mBonesPositions.size(); i++)
				delete mBonesPositions.at(i);
				//mBonesPositions.pop_back();
			mBonesPositions.clear();			
		}
		catch(Ogre::Exception e) {
			std::cout << e.getDescription() << std::endl;
		}
	}
}

//!
//! Loads the RTShader resources, to create the materials of the manual objects.
//!
void AnimatableMeshDECTNode::loadRTShaderResources()
{
	if (Ogre::RTShader::ShaderGenerator::initialize())
	{
		Ogre::String resourceGroupName = "RTShader";

		Ogre::ResourceGroupManager &resourceGroupMgr = Ogre::ResourceGroupManager::getSingleton();			 
		Ogre::FileInfoListPtr fileInfoVector = resourceGroupMgr.listResourceFileInfo(resourceGroupName, true);
		Ogre::ResourceGroupManager::LocationList resLocationList = resourceGroupMgr.getResourceLocationList(resourceGroupName);
		Ogre::String shaderCoreLibsPath;

		if (!resLocationList.empty())
		{				
			Ogre::ResourceGroupManager::LocationList::iterator it = resLocationList.begin();
			Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationList.end();
			bool coreLibsFound = false;

			//get path from resources
			for (;it != itEnd; it++)
			{
				if ((*it)->archive->getName().find(resourceGroupName) != Ogre::String::npos)
				{
					shaderCoreLibsPath = (*it)->archive->getName() + "/";
					coreLibsFound = true;
					break;
				}
			}			
		}
		
		Ogre::String shaderCachePath = shaderCoreLibsPath + "cache/";
		Ogre::String shaderMaterialsPath = shaderCoreLibsPath + "materials/";

		const QString renderSystemName = OgreManager::getRenderSystemName();
		if (renderSystemName != "UNKNOWN")
		{
			// compile Set Shader
			Ogre::String profileFlag;
			Ogre::HighLevelGpuProgramPtr arnFragmentProgram;

			if (renderSystemName.contains("3+")) 
				shaderCoreLibsPath = shaderCoreLibsPath + "GLSL/";
			else
				shaderCoreLibsPath = shaderCoreLibsPath + "Cg/";
		}
		
		// Add the shader libs resource location.
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(shaderCoreLibsPath, "FileSystem");
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(shaderMaterialsPath, "FileSystem");
		
		// Grab the shader generator pointer.
		mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

		// Set shader cache path.
		mShaderGenerator->setShaderCachePath(shaderCachePath);	

		// Set the scene manager.
		mShaderGenerator->addSceneManager(OgreManager::getSceneManager());
		
		// Add a specialized sub-render (per-pixel lighting) state to the default scheme render state
		Ogre::RTShader::RenderState* pMainRenderState = mShaderGenerator->createOrRetrieveRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
		pMainRenderState->reset();

		pMainRenderState->addTemplateSubRenderState(mShaderGenerator->createSubRenderState(Ogre::RTShader::PerPixelLighting::Type));	
	}
}

//!
//! Transforms the AOI coordinates to screen pixels using the camera of the RenderNode.
//!
QPair<int, int> AnimatableMeshDECTNode::getBonePixelCoords( Ogre::Bone *bone, QString nameBone, int separator )
{
	QPair<int, int> coord = QPair<int,int>(-9999, -9999);

	//To get pixel positions for eye tracker - works very accurate with the image in the RenderNode
	Ogre::Matrix4 pM = mCamera->getProjectionMatrix();
	Ogre::Matrix4 vM = mCamera->getViewMatrix();

	Ogre::Vector3 ScreenPosBone = pM * vM * bone->_getDerivedPosition();

	float sx = (ScreenPosBone.x + 1) / 2;
	float sy = (ScreenPosBone.y - 1) / (-2);
	float pixelXBone = sx * mViewWidth;
	float pixelYBone = sy * mViewHeight;
	
	int scaledPixelXBone = (mCamWidth * pixelXBone) / mViewWidth;
	scaledPixelXBone = scaledPixelXBone + ( (mViewWidth/2) - (mCamWidth/2) );
	int scaledPixelYBone = (mCamHeight * pixelYBone) / mViewHeight;
	scaledPixelYBone = scaledPixelYBone + ( (mViewHeight/2) - (mCamHeight/2) );

	coord.first	 = scaledPixelXBone;
	coord.second = scaledPixelYBone;
	
	return coord;
}

//!
//! Sets the AOI coordinates in the parameter panel.
//!
void AnimatableMeshDECTNode::setAOICoordinate(QPair<int,int> coord, QString bonesListName, QString nameBone, QString position)
{	
	QString paramName = "AOI coords > " + bonesListName;

	ParameterGroup *AOICoordsGroup = getParameterGroup("AOI coords")->getParameterGroup(bonesListName);
	if (AOICoordsGroup)
	{
		QList<AbstractParameter*> AOICoordsList = AOICoordsGroup->getParameterList();
		foreach(AbstractParameter *c, AOICoordsList)
		{
			int coordValue = 0;
			QString coordName = dynamic_cast<Parameter*>(c)->getName();
			coordName = paramName + " > " + coordName;

			if (coordName.contains(position))
			{
				if ( (position.compare("upper") == 0) || (position.compare("lower") == 0) )
					coordValue = coord.second;
				else if ( (position.compare("left") == 0) || (position.compare("right") == 0) )
					coordValue = coord.first;
	
				setValue(coordName, coordValue);
				getParameter(coordName)->propagateDirty();				

				// connection
				Parameter *targetParam = getParameter(coordName)->getConnectedParameter();
				if (targetParam)
					targetParam->setValue(coordValue);

				return;
			}
		}
	}
}

//!
//!  Sets the AOI coordinates back to 0 (zero).
//!
void AnimatableMeshDECTNode::resetAOICoordinates()
{
	ParameterGroup *aoigroup = getParameterGroup("AOI coords");
	QList<AbstractParameter *> aoilist = aoigroup->getParameterList();

	foreach(AbstractParameter *a, aoilist)
	{
		ParameterGroup *agroup = dynamic_cast<ParameterGroup*>(a);
		QList<AbstractParameter*> alist = agroup->getParameterList();

		foreach(AbstractParameter *ap, alist)
		{
			Parameter *aoiparam = dynamic_cast<Parameter*>(ap);
			aoiparam->setValue(QVariant(0.0), true);			
		}
	}
}

//!
//! Updates the position of the overlays and displays them on the viewport. 
//!
void AnimatableMeshDECTNode::modifyAOIOverlays()
{
	for (int i=0; i<mAOIOverlays.size(); i++)
	{
		if (mBonesPositions.size() > 0) 
		{
			QList<Ogre::Vector3> *positionsbones = mBonesPositions.at(i);

			if(positionsbones->size() < 4)
				continue;

			Ogre::ManualObject *mObj = mAOIOverlays.at(i);
			mObj->beginUpdate(0);

			for (int j=0; j<positionsbones->size(); j++)
				mObj->position(positionsbones->at(j));

			mObj->position(positionsbones->at(0));
			mObj->end();
		}
	}

	this->getParameter(m_outputGeometryName)->propagateDirty(true);
}

//!
//! Sets the overlays to be visible or invisible correspondingly.
//!
void AnimatableMeshDECTNode::setOverlays()
{
	if (getValue("AOIs").toBool() == false) {
		Log::info("No AOIs have been defined", "AnimatableMeshDECTNode::setOverlays()");
		setValue("Show AOIs", false);
		return;
	}

	if (getValue("Show AOIs").toBool())
		createAOIOverlays();
	else
		destroyAOIOverlays();

	this->getParameter(m_outputGeometryName)->propagateDirty(true);
}

//!
//! Sets the overlays to be visible or invisible according to the param "visible"
//!
void AnimatableMeshDECTNode::setOverlays(bool visible)
{
	if (visible) {
		createAOIOverlays();
		setValue("Show AOIs", true);
	}
	else {
		destroyAOIOverlays();
		setValue("Show AOIs", false);
	}

	this->getParameter(m_outputGeometryName)->propagateDirty(true);
}

//!
//! Creates the MaterialPtr to be used by the AOI manual objects. 
//!
void AnimatableMeshDECTNode::createAOIOverlayMaterial()
{
	moMaterial = createUniqueName( "Material" );

	Ogre::MaterialPtr mObjMaterial = Ogre::MaterialManager::getSingleton().create(moMaterial, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	Ogre::Pass* p = mObjMaterial->getTechnique(0)->getPass(0);
	p->setPolygonModeOverrideable(false);
	p->setVertexColourTracking(Ogre::TVC_AMBIENT);
	p->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	p->setDepthWriteEnabled(false);
	p->setDepthCheckEnabled(false);
	p->setDiffuse(Ogre::ColourValue::Green);

	// Create RTShader based technique from the default technique of the given material.
	mShaderGenerator->createShaderBasedTechnique(moMaterial, Ogre::MaterialManager::DEFAULT_SCHEME_NAME, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	// Apply the RTShader generated based techniques.
	mSceneMan->getCurrentViewport()->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
}

//!
//! Creates and makes visible the overlays for AOIs.
//!
void AnimatableMeshDECTNode::createAOIOverlays()
{
	int nrElements = mBonesPositions.size();

	for (int i=0; i<nrElements; i++)
	{
		QList<Ogre::Vector3> *positionsbones = mBonesPositions.at(i);

		Ogre::String moName = createUniqueName( "manual" + QVariant(i).toString() );

		Ogre::ManualObject *mObj = mSceneMan->createManualObject(moName);
		Ogre::SceneNode *mObjNode; 
		mObjNode = mEntity->getParentSceneNode();

		mObj->begin(moMaterial, Ogre::RenderOperation::OT_LINE_STRIP);

		for(int j=0; j<positionsbones->size(); j++)
			mObj->position(positionsbones->at(j));

		mObj->position(positionsbones->at(0));
		mObj->end();

		mObjNode->attachObject(mObj);
		mAOIOverlays.push_back(mObj);
	} 
}

//!
//! Destroys the overlays for AOIs.
//!
void AnimatableMeshDECTNode::destroyAOIOverlays()
{
	// AOIs Overlays
	Ogre::SceneManager *sceneMan = OgreManager::getSceneManager();
	if (sceneMan)
	{
		try {
			int mObjs = mAOIOverlays.size();

			if (mObjs == 0)
				return;

			for(int i=0; i<mObjs; i++)
			{
				Ogre::ManualObject *mObj = mAOIOverlays.at(i) ;

				bool exists = sceneMan->hasManualObject(mObj->getName());
				if (exists)
					sceneMan->destroyManualObject(mObj->getName());
			}
			while(mAOIOverlays.size() != 0)
				mAOIOverlays.pop_back();	

			if (getValue("Show AOIs").toBool())
				setValue("Show AOIs", false);
		}
		catch(Ogre::Exception e) {
			std::cout << e.getDescription() << std::endl;
		}
	}
}

} // namespace AnimatableMeshDECTNode
