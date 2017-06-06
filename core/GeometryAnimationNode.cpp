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
//! \file "GeometryAnimationNode.cpp"
//! \brief Implementation file for GeometryAnimationNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       26.11.2012 (last updated)
//!

#include "GeometryAnimationNode.h"

#define USE_ANIMATION_WEIGHTS

using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the GeometryAnimationNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
GeometryAnimationNode::GeometryAnimationNode ( const QString &name, ParameterGroup *parameterRoot) :
    GeometryNode(name, parameterRoot, "Geometry")
{	
	Log::info("DEBUG");

    // set affections and functions
    setChangeFunction("Use Spline Interpolation", SLOT(setInterpolationMode()));    

	m_autoPose = getBoolValue("autoPose");

	Parameter *animMultParameter = getParameter("mult");
	if (animMultParameter) {
		animMultParameter->setChangeFunction(SLOT(setAnimationMultiplayer()));
		m_animationMultiplayer = animMultParameter->getValue().toFloat();
	}
	
	// change functions for update flags
	setChangeFunction("autoPose", SLOT(setSwitches()));
    // create parameter groups for animation parameters
	m_animationGroup = new ParameterGroup("Animation Parameter");
	m_skeletalGroup = new ParameterGroup("Skeletal Animations");
    m_skeletalWeightGroup = new ParameterGroup("Skeletal Animation Weights");
    m_poseGroup = new ParameterGroup("Pose Animations");
    m_poseWeightGroup = new ParameterGroup("Pose Animation Weights");
	m_morphGroup = new ParameterGroup("Vertex Cache Animations");
	m_morphWeightGroup = new ParameterGroup("Vertex Cache Animation Weights");
    m_boneGroup = new ParameterGroup("Bone Parameter");
	m_boneAttachmentGroup = new ParameterGroup("Bone Attachment Parameter");
	parameterRoot->addParameter(m_animationGroup);
	m_animationGroup->addParameter(m_skeletalGroup);
    m_animationGroup->addParameter(m_skeletalWeightGroup);
    m_animationGroup->addParameter(m_poseGroup);
    m_animationGroup->addParameter(m_poseWeightGroup);
	m_animationGroup->addParameter(m_morphGroup);
	m_animationGroup->addParameter(m_morphWeightGroup);
    m_animationGroup->addParameter(m_boneGroup);
	m_animationGroup->addParameter(m_boneAttachmentGroup);

	// for visibility of the entity
	Parameter *visibleParameter = getParameter("Visible");
	if (visibleParameter) {
		parameterRoot->addParameter(visibleParameter);
		visibleParameter->setProcessingFunction(SLOT(setEntityVisibility()));
	}


	// This frame listener is needed to manually update the entity's
	// animations to get the right positions and orientations of
	// the bone attachments.
	OgreManager::getRoot()->addFrameListener(this);

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the GeometryAnimationNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
GeometryAnimationNode::~GeometryAnimationNode ()
{
	OgreManager::getRoot()->removeFrameListener(this);
	deleteBoneAttachmentSceneNodes();

	// detach all bone listeners
	DetachBoneListeners();

	emit viewNodeUpdated();
	DEC_INSTANCE_COUNTER
}


///
/// Public Functions
///

///
/// Public Slots
///


/////
///// Public Slots
/////

//!
//! Update an animation.
//!
void GeometryAnimationNode::updateAnimation ()
{
	if( !m_entity)
		return;

	Parameter *parameter = dynamic_cast<Parameter *>(sender());
	if( parameter )
	{
		updateAnimation( parameter );
	}
}


void GeometryAnimationNode::updateAnimation( Parameter * animationParameter )
{
	QString animationName = animationParameter->getName();

	float progress = animationParameter->getValue().toFloat();
	const QVariantList &animationParameterValueList = animationParameter->getValueList();
	for (int i = 1; i < animationParameterValueList.size(); ++i) {
		const QVariant &value = animationParameterValueList[i]; 
		progress += value.toFloat();
	}

	if (progress > 100.0f)
		progress = 100.0f;

	float weight = 1.0;
#ifdef USE_ANIMATION_WEIGHTS
	QString weightParamName = animationName + "_weight";
	if (m_weightNames.contains(weightParamName)) {        
		Parameter *weightParameter = m_skeletalWeightGroup->getParameter(weightParamName);
		weight = weightParameter->getValue().toFloat();
	}

	// NILS: TODO: THIS IS A HACK!
	else if (animationName.endsWith("_weight")) {
		animationName = animationName.remove("_weight");
		if (!animationName.startsWith("pose_") && m_animationNames.contains(animationName)) {
			Parameter *connectedAnimationParameter = m_skeletalGroup->getParameter(animationName);
			weight = progress;
			progress = connectedAnimationParameter->getValue().toFloat();
		}
	}
#endif

	progressAnimation(animationName, progress / 100.0, weight);	
	if (m_autoPose && m_poseNames.contains("pose_" + animationName))
			setValue("pose_" + animationName, progress, weight);	

	updateEyeRotation();

}

//!
//! Update bones.
//!
void GeometryAnimationNode::updateBone ()
{
	if(!m_entity)
		return;

	Parameter *parameter = dynamic_cast<Parameter *>(sender());
	if(parameter)
	{
		updateBone( parameter );		
	}
}


void GeometryAnimationNode::updateBone( Parameter* parameter )
{
	const QString &boneName = parameter->getName();
	const QVariantList& values = parameter->getValue().toList();

	Ogre::Vector3 sumRotations = Ogre::Vector3(0,0,0);
	Ogre::Vector3 sumTranslations = Ogre::Vector3(0,0,0);
	getOrientationAndTranslation(values, sumRotations, sumTranslations);

	// multiple connections: read other bone transformations from valueList
	const QVariantList &valuelist = parameter->getValueList();

	Ogre::Vector3 rotation = Ogre::Vector3(0,0,0);
	Ogre::Vector3 translation = Ogre::Vector3(0,0,0);

	foreach(const QVariant &value, valuelist) {
		getOrientationAndTranslation( value.toList(), rotation, translation);
		sumRotations += rotation;
		sumTranslations += translation;
	}

	if (sumTranslations.length() > 0.0f)
		transformBone(boneName, sumTranslations, sumRotations);
}

//!
//! Update bone attachments.
//!
void GeometryAnimationNode::updateBoneAttachment()
{
	if (!m_entity)
		return;

	GeometryParameter *parameter = dynamic_cast<GeometryParameter *>(sender());

	if (parameter)
	{
		// obtain the OGRE scene manager
		Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();

		QString name = parameter->getName();
		name = name.remove("att_");

		// proceed only if entity available
		if (!sceneManager->hasEntity(m_name.toStdString()))
			return;

		Ogre::Entity *entity = sceneManager->getEntity(m_name.toStdString());
		if (!entity || !entity->hasSkeleton())
			return;

		Ogre::Skeleton *skeleton = entity->getSkeleton();
		Ogre::Bone *bone = 0;
		if (skeleton->hasBone(name.toStdString()))
			bone = skeleton->getBone(name.toStdString());

		if (m_boneAttachmentMap.contains(name)) {
			BoneAttachment * boneAttachment = m_boneAttachmentMap[name];
			Ogre::SceneNode * sceneNode = boneAttachment->getSceneNode();
			m_sceneNode->removeChild(sceneNode);
			OgreTools::deepDeleteSceneNode(sceneNode, sceneManager, true);
			delete boneAttachment;
			m_boneAttachmentMap.remove(name);
		}

		foreach(QVariant parameterValue, parameter->getValueList())
		{
			if (parameterValue.canConvert<Ogre::SceneNode *>())
			{
				Ogre::SceneNode *inputSceneNode = parameterValue.value<Ogre::SceneNode *>();
				if (inputSceneNode) {
					Ogre::SceneNode *inputSceneNodeCopy = 0;
					OgreTools::deepCopySceneNode(inputSceneNode, inputSceneNodeCopy, QString("%1GeometrySceneNode").arg(m_name), sceneManager);
					if (inputSceneNodeCopy) {
						//Ogre::SceneNode * sceneNode = sceneManager->createSceneNode(name.toStdString());
						Ogre::SceneNode * sceneNode = sceneManager->createSceneNode(createUniqueName(name));	//scene node must have an unique name

						OgreContainer* ogreContainer = new OgreContainer(sceneNode);
						sceneNode->setUserAny(Ogre::Any(ogreContainer));
						BoneAttachment * boneAttachment = new BoneAttachment(bone, sceneNode);
						sceneNode->addChild(inputSceneNodeCopy);
						m_sceneNode->addChild(sceneNode);
						m_boneAttachmentMap[name] = boneAttachment;
					}
				}
			}
		}
		// needed in case the visibility flag is 0 -> to make children invisible
		if (getParameter("Visible"))
			if (getParameter("Visible")->getValue().toInt() == 0)
				makeObjectVisible(m_sceneNode, 0);
	}

	updateAllBoneAttachments();
}

//!
//! Update poses.
//!
void GeometryAnimationNode::updatePose ()
{
	if( !m_entity)
		return;

	Parameter *parameter = dynamic_cast<Parameter *>(sender());
	if( parameter)
	{
		updatePose(parameter);
	}
}


void GeometryAnimationNode::updatePose( Parameter * poseParameter )
{
	const QString &poseName = poseParameter->getName();
	float weight = 1.0;

#ifdef USE_ANIMATION_WEIGHTS
	QString weightParamName = poseName;
	weightParamName.remove("pose_");
	weightParamName = weightParamName + "_weight";
	if (m_weightNames.contains(weightParamName))
		weight = getDoubleValue(weightParamName, true);
#endif

	double progress = poseParameter->getValue().toDouble();

	if (progress > 100.0f)
		progress = 100.0f;
	progressAnimation(poseName, progress/100.0, weight);
}


///
/// Private Functions
///

///
/// Private Functions
///

//!
//! Initialize all animation states of this object (OGRE-specific).
//!
//! \return True if the animation states where successfully initialized, otherwise False.
//!
bool GeometryAnimationNode::initAnimationStates ()
{  
	Ogre::AnimationStateSet *animStateSet = m_entity->getAllAnimationStates();
	if (animStateSet) {
		Ogre::AnimationStateIterator stateIter = animStateSet->getAnimationStateIterator();
		while (stateIter.hasMoreElements()) {
			Ogre::AnimationState* state = stateIter.getNext();
			state->setLoop(false);
		}
		setInterpolationMode();
		return true;
	}
	else
		return false;
}





//!
//! Retrieve the names of all animation names, types and index numbers
//!
//! \return List with names, types and index numbers of animations.
//!
QStringList GeometryAnimationNode::getAnimationNamesAndTypes (Ogre::Entity *entity)
{
    QStringList animNamesAndTypes;
	Ogre::MeshPtr meshInstance = entity->getMesh();	
	Ogre::Mesh *pMesh= meshInstance.get();	
	if (!pMesh) {
		Log::error("Could not obtain OGRE mesh.", "GeometryAnimationNode::getAnimationNamesAndTypes ");
		return animNamesAndTypes;
	}
		
	// skeletal animation
	if (entity->hasSkeleton()) {		
		Ogre::SkeletonInstance *skeletonInstance = entity->getSkeleton();
		if (!skeletonInstance) {
			Log::error("Could not obtain OGRE skeleton.", "GeometryAnimationNode::getAnimationNamesAndTypes ");
			return animNamesAndTypes;
		}
		unsigned short numAnims = skeletonInstance->getNumAnimations();
		for (unsigned short i = 0; i < numAnims; ++i) {
			Ogre::Animation *animation = skeletonInstance->getAnimation(i);			
			animNamesAndTypes.append(animation->getName().c_str());			
			animNamesAndTypes.append("skeleton");
			animNamesAndTypes.append(QString::number(i));
		}
	}	
	// vertex animation		
	if (pMesh->hasVertexAnimation()){
		unsigned short numAnims = pMesh->getNumAnimations();
		for (unsigned short i = 0; i < numAnims; ++i) {
			Ogre::Animation *animation = pMesh->getAnimation(i);
			Ogre::Animation::VertexTrackIterator vtxTrackIter = animation->getVertexTrackIterator();			
			while( vtxTrackIter.hasMoreElements()) {
				Ogre::VertexAnimationTrack *vtxTrack = vtxTrackIter.getNext();
				// Pose Animation
				if (vtxTrack->getAnimationType() == 2 ) {					
					animNamesAndTypes.append(animation->getName().c_str());					
					animNamesAndTypes.append("pose");
					animNamesAndTypes.append(QString::number(i));
				}
				// Morph Animation
				if (vtxTrack->getAnimationType() == 1) {
					animNamesAndTypes.append(animation->getName().c_str());					
					animNamesAndTypes.append("morph");
					animNamesAndTypes.append(QString::number(i));
					while( vtxTrackIter.hasMoreElements())
						vtxTrackIter.getNext();	// each Submesh has its own Vertex Track, skip all following tracks
				}
		}
	}			
	}
    return animNamesAndTypes;
}

//!
//! Adds the given time value to the animation state with the given name in
//! order to progress the animation (OGRE-specific).
//!
//! \param aUnitName The name of the animation state to progress.
//! \param timeToAdd The time to add to the animation state with the given name.
//!
void GeometryAnimationNode::progressAnimation ( const QString &animationName, float timeToAdd, float weight /* = 1.0 */ )
{
	if (m_entity->hasAnimationState(animationName.toStdString())) {
		Ogre::AnimationState *animState = m_entity->getAnimationState(animationName.toStdString());
	
		if (animState) {
			const float animLength = animState->getLength();
			Ogre::Real pos = m_animationMultiplayer * timeToAdd * animLength;

			// update all copies created from the entity through the entity container
			if (m_entityContainer) {			
				m_entityContainer->updateAnimationState(animationName, pos, weight);
			}
		}
	}
}

//!
//! Transforms a given bone using euler angles.
//! 
//! \param name The name of the bone.
//! \param position Position of bone
//! \param orientation Orientation of bone as rotation around x-/y-/z-axis
//!
void GeometryAnimationNode::transformBone( const QString &name, const Ogre::Vector3 &position, const Ogre::Vector3 &orientation )
{
	if (m_entity->hasSkeleton()) 
	{
		const std::string &stdName = name.toStdString();
		Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
		if (skeletonInstance->hasBone(stdName)) 
		{
			Ogre::Bone *bone = skeletonInstance->getBone(stdName);

			bone->setManuallyControlled(true);
			bone->reset();
			bone->translate(position);

			bone->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian( orientation.x ));
			bone->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian( orientation.y));
			bone->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian( orientation.z ));

			m_entityContainer->updateCopies(name, position.x, position.y, position.z, 
				orientation.x, orientation.y, orientation.z);
		}
	}
}

//!
//! Transforms a given bone using a quaternion.
//! 
//! \param name The name of the bone.
//! \param position Position of the bone
//! \param orientation Orientation of bone as axis/angle rotation
//!
void GeometryAnimationNode::transformBone( const QString &name, const Ogre::Vector3& position, const Ogre::Quaternion &orientation )
{
	if (m_entity->hasSkeleton()) 
	{
		const std::string &stdName = name.toStdString();
		Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
		if (skeletonInstance->hasBone(stdName)) 
		{
			Ogre::Bone *bone = skeletonInstance->getBone(stdName);

			bone->setManuallyControlled(true);
			bone->reset();
			bone->translate( position);

			bone->rotate( orientation);

			Ogre::Matrix3 rot;
			orientation.ToRotationMatrix(rot);

			Ogre::Radian rx, ry, rz;
			rot.ToEulerAnglesXYZ(rx, ry, rz);

			float yaw = orientation.getYaw().valueDegrees();
			float pitch = orientation.getPitch().valueDegrees();
			float roll = orientation.getRoll().valueDegrees();

			// @TODO: update copies with quaternions!!!
			m_entityContainer->updateCopies(name, position.x, position.y, position.z, 
				rx.valueRadians(), ry.valueRadians(), rz.valueRadians());

		}
	}
}


//!
//! Returns the names of all bones in the skeleton for the mesh.
//!
//! \return The names of all bones in the skeleton for the mesh.
//!
QStringList GeometryAnimationNode::getBoneNames ()
{
    QStringList result;

    // obtain the OGRE scene manager
    Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    if (!sceneManager) {
        Log::error("Could not obtain OGRE scene manager.", "GeometryAnimationNode::getBoneNames");
        return result;
    }

    // proceed only if entity available
    if (!sceneManager->hasEntity(m_name.toStdString()))
        return result;
    Ogre::Entity *entity = sceneManager->getEntity(m_name.toStdString());
    if (!entity || !entity->hasSkeleton())
        return result;

    Ogre::Skeleton *skeleton = entity->getSkeleton();
    unsigned short numberOfBones = skeleton->getNumBones();
    for (unsigned short i = 0; i < numberOfBones; ++i) {
        Ogre::Bone *bone = skeleton->getBone(i);
        result.append(QString(bone->getName().c_str()));
    }
    return result;
}


//!
//! Initialize a bunch of bones.
//!
//! \param boneNames The list of names of bones to initialize.
//!
void GeometryAnimationNode::initializeBones ( QStringList boneNames )
{
    //// obtain the OGRE scene manager
    //Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
    //if (!sceneManager) {
    //    Log::error("Could not obtain OGRE scene manager.", "GeometryAnimationNode::initializeBones");
    //    return;
    //}

    //// proceed only if entity and entity skeleton available
    //Ogre::Entity *entity = sceneManager->getEntity(m_name.toStdString());
    //if (!entity || !entity->hasSkeleton())
    //    return;

    //Ogre::Skeleton *skeleton = entity->getSkeleton();
    //for (int i = 0; i < boneNames.size(); ++i) {
    //    if (skeleton->hasBone(boneNames[i].toStdString())) {
    //        Ogre::Bone *bone = skeleton->getBone(boneNames[i].toStdString());
    //        if (bone) {
    //            bone->setManuallyControlled(true);
    //            //bone->setBindingPose();
    //        }
    //    }
    //}
}


///
/// Private Slots
///

//!
//! Change function for the Geometry File parameter.
//!
void GeometryAnimationNode::geometryFileChanged ()
{
	// call change method of parent
	GeometryNode::geometryFileChanged();

	// check if geometry file was loaded successfully
	if (!m_entity)
		return;

	DetachBoneListeners();

	// GUI is optimised, empty groups are removed
    // need to build from scratch each time Geometry File changes
	m_animationGroup->destroyAllParameters();
	m_skeletalGroup = new ParameterGroup("Skeletal Animations");
    m_skeletalWeightGroup = new ParameterGroup("Skeletal Animation Weights");
    m_poseGroup = new ParameterGroup("Pose Animations");
    m_poseWeightGroup = new ParameterGroup("Pose Animation Weights");
	m_morphGroup = new ParameterGroup("Vertex Cache Animations");
	m_morphWeightGroup = new ParameterGroup("Vertex Cache Animation Weights");
    m_boneGroup = new ParameterGroup("Bone Parameter");
	m_boneAttachmentGroup = new ParameterGroup("Bone Attachment Parameter");
    m_animationGroup->addParameter(m_skeletalGroup);
    m_animationGroup->addParameter(m_poseGroup);
	m_animationGroup->addParameter(m_morphGroup);
	m_animationGroup->addParameter(m_boneGroup);
	m_animationGroup->addParameter(m_boneAttachmentGroup);
	m_animationGroup->addParameter(m_skeletalWeightGroup);    
    m_animationGroup->addParameter(m_poseWeightGroup);
	m_animationGroup->addParameter(m_morphWeightGroup);  
	
    // clear lists
    m_animationNames.clear();
    m_poseNames.clear();

	// Set blend mode, clean empty tracks
	if (m_entity->hasSkeleton()) 
	{
		// set cumulative blend mode instead of Ogre::ANIMBLEND_AVERAGE which is default
		Ogre::Skeleton *skeleton = m_entity->getSkeleton();
		skeleton->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

		// clean empty animation tracks
		if( getBoolValue("Clean empty Skeleton Animation tracks on load"))
		{
			skeleton->optimiseAllAnimations(false);
			CleanEmptyTracks(skeleton);
		}
	}

	// retrieve names, type and index    	    
	const QStringList &animationNamesTypes = GeometryAnimationNode::getAnimationNamesAndTypes(m_entity);	    
    for (int i = 0; i < animationNamesTypes.size(); i=i+3) {
        // create a new number parameter for the animation
        NumberParameter *animationParameter = new NumberParameter(animationNamesTypes[i], Parameter::T_Float, 0.0);
        animationParameter->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
        animationParameter->setMinValue(0.0);
        animationParameter->setMaxValue(100.0);
        animationParameter->setStepSize(1.0);
        animationParameter->setPinType(Parameter::PT_Input);
        animationParameter->setMultiplicity(Parameter::M_OneOrMore);
        //animationParameter->setSelfEvaluating(true);
		#ifdef USE_ANIMATION_WEIGHTS
			NumberParameter *weightParameter = new NumberParameter(animationNamesTypes[i] + "_weight", Parameter::T_Float, 1.0);
			weightParameter->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
			weightParameter->setMinValue(0.0);
			weightParameter->setMaxValue(1.0);
			weightParameter->setStepSize(0.01);
			weightParameter->setPinType(Parameter::PT_Input);
			weightParameter->setMultiplicity(Parameter::M_OneOrMore);
		#endif		
	
		// skeletal animation
		if (animationNamesTypes[i+1]=="skeleton"){
			m_skeletalGroup->addParameter(animationParameter);
			animationParameter->setProcessingFunction(SLOT(updateAnimation()));
			animationParameter->setChangeFunction(SIGNAL(triggerRedraw()));
			animationParameter->setSelfEvaluating(true);
			m_animationNames.append(animationNamesTypes[i]);
			#ifdef USE_ANIMATION_WEIGHTS
				m_skeletalWeightGroup->addParameter(weightParameter);
				weightParameter->setProcessingFunction(SLOT(updateAnimation()));
				weightParameter->setChangeFunction(SIGNAL(triggerRedraw()));
				weightParameter->setSelfEvaluating(true);
				m_weightNames.append(animationNamesTypes[i] + "_weight");
			#endif				
			}
		// pose animation
		else if (animationNamesTypes[i+1]=="pose"){
			m_poseGroup->addParameter(animationParameter);
			animationParameter->setProcessingFunction(SLOT(updatePose()));
			animationParameter->setChangeFunction(SIGNAL(triggerRedraw()));
			animationParameter->setSelfEvaluating(true);			
			m_animationNames.append(animationNamesTypes[i]);
			m_poseNames.append(animationNamesTypes[i]);
			#ifdef USE_ANIMATION_WEIGHTS
				m_poseWeightGroup->addParameter(weightParameter);
				weightParameter->setProcessingFunction(SLOT(updateAnimation()));
				weightParameter->setChangeFunction(SIGNAL(triggerRedraw()));
				weightParameter->setSelfEvaluating(true);
				m_weightNames.append(animationNamesTypes[i] + "_weight");
			#endif
		}
		// morph animation
		else if (animationNamesTypes[i+1]=="morph"){
			m_morphGroup->addParameter(animationParameter);
			animationParameter->setProcessingFunction(SLOT(updatePose()));
			animationParameter->setChangeFunction(SIGNAL(triggerRedraw()));
			animationParameter->setSelfEvaluating(true);
			m_animationNames.append(animationNamesTypes[i]);
			#ifdef USE_ANIMATION_WEIGHTS
				m_morphWeightGroup->addParameter(weightParameter);
				weightParameter->setProcessingFunction(SLOT(updateAnimation()));
				weightParameter->setChangeFunction(SIGNAL(triggerRedraw()));
				weightParameter->setSelfEvaluating(true);
				m_weightNames.append(animationNamesTypes[i] + "_weight");
			#endif
		}

		// throw warning if animation name is 'Animation', not possible due to limitation in ParameterGroup::setValue	
		if (animationNamesTypes[i]=="Animation Parameter")
			Log::warning("Animation name can't be 'Animation', please choose a different name", "GeometryAnimationNode:::geometryFileChanged");
    }

	// initialize the OGRE animation states
    initAnimationStates();	

	createBoneParams();
	deleteBoneAttachmentSceneNodes();
	createBoneAttachmentParams();

	// bind listeners to eye bones for shader updates if exist.
	if(m_entity->hasSkeleton() && !m_entity->isHardwareAnimationEnabled()) {
		Log::debug("HWS NOT ENABLED FOR " + QString::fromStdString(m_entity->getMesh()->getName()), "GeometryAnimationNode::geometryFileChanged");
		const Ogre::Skeleton *skeletonInstance = m_entity->getSkeleton();
		for(int i=0; i<m_entity->getNumSubEntities(); ++i) {
			const Ogre::SubEntity *subEntity = m_entity->getSubEntity(i);
			const QString subEntityMaterialName = subEntity->getMaterialName().c_str();

			if(subEntityMaterialName.contains("LeftEye")  || subEntityMaterialName.contains("Left_Eye_Mat") || subEntityMaterialName.contains("Gunnar_Left_Eye")) {
				const Ogre::MaterialPtr &subEntityMaterial = subEntity->getMaterial();
				const Ogre::GpuProgramParametersSharedPtr &vp = subEntityMaterial->getTechnique(0)->getPass(0)->getVertexProgramParameters();
				if(vp->_findNamedConstantDefinition("initPosition") &&
					vp->_findNamedConstantDefinition("initRotation")) {
					// temporal solution - all bones should be exported with the same name
					//const Ogre::String boneName = "joint_LeftEye";
					//if (skeletonInstance->hasBone(boneName)) {

					Ogre::String boneName = "";
					if(skeletonInstance->hasBone("Eye_Rig_v2:jnt_left_eyeball_"))
						boneName = "Eye_Rig_v2:jnt_left_eyeball_";
					else if(skeletonInstance->hasBone("joint_LeftEye"))
						boneName = "joint_LeftEye";
					else if(skeletonInstance->hasBone("LEyeJoint"))
						boneName = "LEyeJoint";

					if(!boneName.empty()) {
						BoneListener *boneListener = new BoneListener("initPosition", "initRotation", vp);
						Ogre::Bone *bone = skeletonInstance->getBone(boneName);
						bone->setListener(boneListener);
					}
				}
			}
			else if(subEntityMaterialName.contains("RightEye")  || subEntityMaterialName.contains("Right_Eye_Mat") || subEntityMaterialName.contains("Gunnar_Right_Eye")) {
				const Ogre::MaterialPtr &subEntityMaterial = subEntity->getMaterial();
				const Ogre::GpuProgramParametersSharedPtr &vp = subEntityMaterial->getTechnique(0)->getPass(0)->getVertexProgramParameters();
				if(vp->_findNamedConstantDefinition("initPosition") &&
					vp->_findNamedConstantDefinition("initRotation")) {
					// temporal solution - all bones should be exported with the same name
					//const Ogre::String boneName = "joint_RightEye";
					//if (skeletonInstance->hasBone(boneName)) {

					Ogre::String boneName = "REyeJoint";
					/*
					if (skeletonInstance->hasBone("Eye_Rig_v2:jnt_right_eyeball_"))
						boneName = "Eye_Rig_v2:jnt_right_eyeball_";
					else if (skeletonInstance->hasBone("joint_RightEye"))
						boneName = "joint_RightEye";
					else if (skeletonInstance->hasBone("REyeJoint"))
						boneName = "REyeJoint";
					*/

					if(!boneName.empty()) {
						BoneListener *boneListener = new BoneListener("initPosition", "initRotation", vp);
						Ogre::Bone *bone = skeletonInstance->getBone(boneName);
						bone->setListener(boneListener);
					}
				}
			}

		}
	}
	else
		Log::debug("HWS ENABLED FOR " + QString::fromStdString(m_entity->getMesh()->getName()), "GeometryAnimationNode::geometryFileChanged");

	// redraw node to display new parameters    
	forcePanelUpdate();
}


//!
//! Create bone parameters.
//! 
void GeometryAnimationNode::createBoneParams()
{
	// initialize the list of values for the bone number parameters
	QVariantList values = QVariantList() << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

	// iterate over the list of bones
	const QStringList &boneNames = getBoneNames();
	for (int i = 0; i < boneNames.size(); ++i) {
		if (!boneNames[i].contains("MarkerJoint_")) {
			// create a new number parameter for the bone
			NumberParameter *boneParameter = new NumberParameter(boneNames[i], Parameter::T_Float, 0.0);
			boneParameter->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
			boneParameter->setMinValue(0.0);
			boneParameter->setMaxValue(100.0);
			boneParameter->setStepSize(0.01);
			boneParameter->setPinType(Parameter::PT_Input);
			boneParameter->setSize(6);
			boneParameter->setSelfEvaluating(true);
			boneParameter->setMultiplicity(Parameter::M_OneOrMore);
			boneParameter->setValue(QVariant(values));
			m_boneGroup->addParameter(boneParameter);
			boneParameter->setProcessingFunction(SLOT(updateBone()));
			boneParameter->setChangeFunction(SIGNAL(triggerRedraw()));
		}
	}
}

//!
//! Create bone attachment parameters.
//! 
void GeometryAnimationNode::createBoneAttachmentParams()
{
	Parameter *geometryOutputParameter = getParameter(m_outputGeometryName);

	// iterate over the list of bones
	const QStringList &boneNames = getBoneNames();
	for (int i = 0; i < boneNames.size(); ++i) {
		if (!boneNames[i].contains("MarkerJoint_")) {
			// create a new number parameter for the bone
			GeometryParameter *boneParameter = new GeometryParameter("att_" + boneNames[i]);
			boneParameter->setPinType(Parameter::PT_Input);
			//boneParameter->setSelfEvaluating(true);
			//boneParameter->setDirty(true);
			m_boneAttachmentGroup->addParameter(boneParameter);
			boneParameter->setProcessingFunction(SLOT(updateBoneAttachment()));
			boneParameter->addAffectedParameter(geometryOutputParameter);
		}
	}
}

void GeometryAnimationNode::deleteBoneAttachmentSceneNodes()
{
	QMap<QString, BoneAttachment *>::const_iterator i = m_boneAttachmentMap.constBegin();
	while (i != m_boneAttachmentMap.constEnd()) {
		BoneAttachment * boneAttachment = i.value();
		Ogre::SceneNode * sceneNode = boneAttachment->getSceneNode();
		if (sceneNode) {
			Ogre::SceneManager *sceneManager = sceneNode->getCreator();
			if (sceneManager) {
				OgreTools::deepDeleteSceneNode(sceneNode, sceneManager, true);
			}
		}
		delete boneAttachment;
		++i;
	}
	m_boneAttachmentMap.clear();
}

//!
//! Set interpolation mode.
//!
void GeometryAnimationNode::setInterpolationMode ()
{
	if (m_entity) {
		Ogre::Animation::InterpolationMode interpolationMode = Ogre::Animation::IM_LINEAR;
		bool useSplineInterpolation = getBoolValue("Use Spline Interpolation");
		if (useSplineInterpolation)
			interpolationMode = Ogre::Animation::IM_SPLINE;    
		Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
		if (!skeletonInstance)
			return;
		Ogre::AnimationStateSet *animStateSet = m_entity->getAllAnimationStates();
		Ogre::AnimationStateIterator stateIter = animStateSet->getAnimationStateIterator();
		while (stateIter.hasMoreElements()) {
			Ogre::AnimationState* state = stateIter.getNext();
			Ogre::String animName = state->getAnimationName();
			if (skeletonInstance->hasAnimation(animName)) {
				Ogre::Animation *animation = skeletonInstance->getAnimation(animName);
				animation->setInterpolationMode(interpolationMode);
			}
		}
	}
}


//!
//! Sets the internal config switches
//!
void GeometryAnimationNode::setSwitches()
{
	Parameter *parameter = dynamic_cast<Parameter *>(sender());
   
	if (parameter) {
		const QString &name = parameter->getName();
		const bool value = parameter->getValue().toBool();

		if (name == "autoPose")
			m_autoPose = value;
	}
}


//!
//! Sets the animation multiplayer.
//!
void GeometryAnimationNode::setAnimationMultiplayer()
{
	Parameter *parameter = dynamic_cast<Parameter *>(sender());
   
	if (parameter) {
		m_animationMultiplayer = parameter->getValue().toFloat();
	}
}


//!
//! Checks the Ogre entity if it is animatable.
//! \return True if the entity is animated
//!
bool GeometryAnimationNode::isAnimated () const
{
	return m_entity->hasVertexAnimation() || m_entity->hasSkeleton();
}

//!
//! Cleans all empty (identity) tracks from the given ogre skeleton
//!
void GeometryAnimationNode::CleanEmptyTracks( Ogre::Skeleton * skeleton )
{
    // clean empty tracks
    for( int i = 0; i<skeleton->getNumAnimations(); i++)
    {
        Ogre::Animation *anim = skeleton->getAnimation(i);		
        Ogre::Animation::NodeTrackIterator nodeTrackIter = anim->getNodeTrackIterator();		

		// Iterate over node tracks and fill list of empty node track ids
		QList<unsigned short> emptyNodeTracks;
        while( nodeTrackIter.hasMoreElements())
        {
            Ogre::NodeAnimationTrack *nodeTrack = nodeTrackIter.getNext();				
            bool isEmpty = true;
			
			// Iterate over all key frames and check if ...
			for( int j=0; j<nodeTrack->getNumKeyFrames(); j++)
            {
                Ogre::TransformKeyFrame *trans_key = nodeTrack->getNodeKeyFrame(j);

				// Rotation is identity
				isEmpty &= trans_key->getRotation()  == Ogre::Quaternion::IDENTITY;
				// Scale is 1,1,1
				isEmpty &= trans_key->getScale()     == Ogre::Vector3(1,1,1);
				// Translation is 0,0,0
				isEmpty &= trans_key->getTranslate() == Ogre::Vector3(0,0,0);

				if( !isEmpty)
				{
					// break loop because animation is not empty
                    break;
				}
            }
			// if all transformations on all key frames are identical, delete this track
			if( isEmpty )
                emptyNodeTracks.push_back(nodeTrack->getHandle());
        }

        foreach( unsigned short trackHandle, emptyNodeTracks)
        {
            anim->destroyNodeTrack(trackHandle);
        }
    }
}

//!
//! Detaches all listeners from all bones.
//!
void GeometryAnimationNode::DetachBoneListeners()
{
	if (m_entity) {
		if (m_entity->hasSkeleton()) {
			Ogre::SkeletonInstance *skeleton = m_entity->getSkeleton();
			Ogre::Skeleton::BoneIterator iter = skeleton->getBoneIterator();
			while (iter.hasMoreElements()) {
				Ogre::Bone *bone = iter.getNext();
				Ogre::Node::Listener *boneListener = bone->getListener();
				if (boneListener) {
					delete bone->getListener();
					bone->setListener(0);
				}
			}
		}
	}	
}

//!
//! Update eyeRotation for ambient occlusion (Gaze)
//! 
void GeometryAnimationNode::updateEyeRotation()
{
	NumberParameter* Surp = dynamic_cast<NumberParameter *>(m_skeletalGroup->getParameter("Surp"));
	NumberParameter* gazeLeft = dynamic_cast<NumberParameter *>(m_skeletalGroup->getParameter("GazeHLeft"));
	NumberParameter* gazeRight = dynamic_cast<NumberParameter *>(m_skeletalGroup->getParameter("GazeHRight"));
	NumberParameter* gazeDown = dynamic_cast<NumberParameter *>(m_skeletalGroup->getParameter("GazeVDown"));
	NumberParameter* gazeUp = dynamic_cast<NumberParameter *>(m_skeletalGroup->getParameter("GazeVUp"));
	NumberParameter* rotation1 = dynamic_cast<NumberParameter *>(m_materialGroup->getParameter("gazeDirection@RightEye"));
	NumberParameter* rotation2 = dynamic_cast<NumberParameter *>(m_materialGroup->getParameter("gazeDirection@LeftEye"));

	if ( rotation1 && gazeLeft && gazeRight && rotation2 && gazeUp && gazeDown && Surp ) 
	{
		float xValue = 127.5;
		float yValue = 127.5;
		float zValue = 127.5;
		xValue += Surp->getValue().toFloat() * 0.05 * 127.5 / 100.0;
		xValue -= gazeUp->getValue().toFloat() * 0.09 * 127.5 / 100.0;		
		xValue += gazeDown->getValue().toFloat() * 0.15 * 127.5 / 100.0;
		yValue += gazeLeft->getValue().toFloat() * 0.565539 * 127.5 / 100.0;
		yValue -= gazeRight->getValue().toFloat() * 0.565539 * 127.5 / 100.0;
		QColor color = QColor(xValue, yValue, zValue);
		rotation1->setValue(color, true);
		rotation2->setValue(color, true);
		rotation1->propagateEvaluation();
		rotation2->propagateEvaluation();
	}
}

void GeometryAnimationNode::calculateBoneOcclusion()
{
	// Bone Occlusion
	static const float stretchSpeed = 0.5f;
	static const float moveSpeed = 0.2f;
	static const Ogre::String nameL = "MarkerJoint_LUL9";
	static const Ogre::String nameR = "MarkerJoint_RUL9";
	NumberParameter* occlusionStretchY = 0;
	NumberParameter* occlusionMoveY = 0;

	Ogre::SkeletonInstance *skeletonInstance = m_entity->getSkeleton();
	if (skeletonInstance->hasBone(nameL)) {
		if ( (occlusionStretchY = dynamic_cast<NumberParameter *>(m_parameterGroup->getParameter("occlusionStretchY@LeftEye"))) &&
			(occlusionMoveY = dynamic_cast<NumberParameter *>(m_parameterGroup->getParameter("occlusionMoveY@LeftEye"))) ) {
				Ogre::Bone *bone = skeletonInstance->getBone(nameL);
				const Ogre::Vector3 &MarkerJoint_LUL9 = bone->getPosition();

				const float stretchY = stretchSpeed * MarkerJoint_LUL9.y + (1.15 - 7.367 * stretchSpeed);
				occlusionStretchY->setValue(QVariant(stretchY), true);
				occlusionStretchY->propagateEvaluation();

				const float moveY = moveSpeed * MarkerJoint_LUL9.y - (7.367 * moveSpeed);
				occlusionMoveY->setValue(QVariant(moveY), true);
				occlusionMoveY->propagateEvaluation();
		}
	}

	if (skeletonInstance->hasBone(nameR)) {
		if ( (occlusionStretchY = dynamic_cast<NumberParameter *>(m_parameterGroup->getParameter("occlusionStretchY@RightEye"))) &&
			(occlusionMoveY = dynamic_cast<NumberParameter *>(m_parameterGroup->getParameter("occlusionMoveY@RightEye"))) ) {
				Ogre::Bone *bone = skeletonInstance->getBone(nameR);
				const Ogre::Vector3 &MarkerJoint_RUL9 = bone->getPosition();

				const float stretchY = stretchSpeed * MarkerJoint_RUL9.y + (1.10 - 7.367 * stretchSpeed);
				occlusionStretchY->setValue(QVariant(stretchY), true);
				occlusionStretchY->propagateEvaluation();

				const float moveY = moveSpeed * MarkerJoint_RUL9.y - (7.367 * moveSpeed);
				occlusionMoveY->setValue(QVariant(moveY), true);
				occlusionMoveY->propagateEvaluation();
		}
	}
}

void GeometryAnimationNode::getOrientationAndTranslation( const QVariantList &values, Ogre::Vector3 &rotation, Ogre::Vector3 &translation )
{
	if (values.size() == 6)
	{
		// Interpret values as: rotation around x-/y-/z-axis, position
		rotation = Ogre::Vector3(  values[0].toFloat(),  values[1].toFloat(),  values[2].toFloat() );
		translation = Ogre::Vector3(  values[3].toFloat(),  values[4].toFloat(),  values[5].toFloat() );
	}
	else if (values.size() == 7) 
	{
		// Interpret values as: angle, axis, position
		Ogre::Quaternion quaternion( values[0].toFloat(),values[1].toFloat(),values[2].toFloat(),values[3].toFloat());

		// convert quaternion to Euler angles here
		Ogre::Matrix3 rotationMatrix;
		quaternion.ToRotationMatrix(rotationMatrix);

		Ogre::Radian rx, ry, rz;
		rotationMatrix.ToEulerAnglesXYZ(rx, ry, rz);

		rotation = Ogre::Vector3( rx.valueRadians(), ry.valueRadians(), rz.valueRadians());
		translation = Ogre::Vector3 (values[4].toFloat(), values[5].toFloat(), values[6].toFloat());
	}
}

//!
//! Triggers before redraw.
//!
void GeometryAnimationNode::onPreRedraw()
{
	updateAllBoneAttachments();
}

//!
//! Ogre frame listener callback.
//!
bool GeometryAnimationNode::frameStarted(const Ogre::FrameEvent & event)
{
	//updateAllBoneAttachments();
	return true;
}

void GeometryAnimationNode::updateAllBoneAttachments()
{
	if (m_entity && m_boneAttachmentMap.size() > 0) {
		//Log::info("BEGIN: Animation Update");
		m_entity->_updateAnimation();
		//m_entityContainer->updateCopies();
		//m_entity->getParentNode()->_update(true, true);

		QMap<QString, BoneAttachment *>::const_iterator i = m_boneAttachmentMap.constBegin();
		while (i != m_boneAttachmentMap.constEnd()) {
			BoneAttachment * boneAttachment = i.value();
			if (boneAttachment) {
				Ogre::SceneNode* sceneNode = boneAttachment->getSceneNode();
				// Bone pointer seems to be invalid after skeleton is shared.
				// Sharing happens in OgreTools.cpp when node is copied. -> Instancing.
				//Ogre::Bone * bone = boneAttachment->getBone();
				//Ogre::Bone * bone = m_entity->getSkeleton()->getBone(sceneNode->getName());

				// scene node has an unique name. Here we extract the bone's name, contained in the scenenode's name
				QString sNodeName = QString::fromStdString(sceneNode->getName());
				int index = sNodeName.indexOf('_');
				sNodeName = sNodeName.remove(0, index + 1);
				Ogre::Bone * bone = m_entity->getSkeleton()->getBone(sNodeName.toStdString());

				const Ogre::Vector3 &position = bone->_getDerivedPosition();
				const Ogre::Quaternion &orientation = bone->_getDerivedOrientation();
				const Ogre::Any &ogreAny = sceneNode->getUserAny();
				OgreContainer * ogreContainer = ogreAny.get<OgreContainer *>();

				sceneNode->setPosition(position);
				sceneNode->setOrientation(orientation);
				ogreContainer->updateCopies();
			}
			++i;
		}
		//Log::info("END: Animation Update");
	}
}

//!
//! Sets the visibility of the entity
//!
void GeometryAnimationNode::setEntityVisibility()
{
	int visible = getParameter("Visible")->getValue().toInt();
	
	if(m_sceneNode)
		makeObjectVisible(m_sceneNode, visible);

	this->getParameter(m_outputGeometryName)->propagateDirty(true);
}

//!
//! To make movable objects not visible, even their children
//!
void GeometryAnimationNode::makeObjectVisible(Ogre::SceneNode *sceneNode, int visible)
{	
	if (sceneNode)
	{
		//check if there are more children
		Ogre::Node::ChildNodeIterator childIt = sceneNode->getChildIterator();
		while(childIt.hasMoreElements())
		{
			Ogre::SceneNode *childSceneNode1 = dynamic_cast<Ogre::SceneNode*>(childIt.getNext());
			makeObjectVisible(childSceneNode1, visible);
		}

		Ogre::SceneNode::ObjectIterator objIt = sceneNode->getAttachedObjectIterator();
		while(objIt.hasMoreElements())
		{
			Ogre::MovableObject *mo = dynamic_cast<Ogre::MovableObject*>(objIt.getNext());
			mo->setVisibilityFlags(visible);
		}
	}
	
}
