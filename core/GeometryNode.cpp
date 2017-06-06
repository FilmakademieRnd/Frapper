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
//! \file "GeometryNode.cpp"
//! \brief Implementation file for GeometryNode class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#include "GeometryNode.h"
#include "SceneNodeParameter.h"
#include "OgreManager.h"
#include <QFile>
#include "OgreTools.h"

namespace Frapper {

///
/// Constructors and Destructors
///


//!
//! Constructor of the GeometryNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputGeometryName The name of the geometry output parameter.
//!
GeometryNode::GeometryNode (	const QString &name, 
								ParameterGroup *parameterRoot, 
								const QString &outputGeometryName /* = "Geometry" */) :
	ViewNode(name, parameterRoot),
	m_outputGeometryName(outputGeometryName),
	m_sceneNode(0),
	m_entity(0),
	m_entityContainer(0),
	m_oldResourceGroupName("")
{
    addOutputParameter(new GeometryParameter(m_outputGeometryName));

	// set affections and functions
	addAffection("Geometry File", m_outputGeometryName);

	setCommandFunction("Geometry File", SLOT(geometryFileChanged())); // triggered by reload
	setChangeFunction("Geometry File", SLOT(geometryFileChanged())); // triggered by open mesh file
	setCommandFunction("useSaC", SLOT(geometryFileChanged()));

	m_materialGroup = new ParameterGroup("Material Parameter");
	m_objectIdGroup = new ParameterGroup("Object Id Parameter");    
	parameterRoot->addParameter(m_materialGroup);
	parameterRoot->addParameter(m_objectIdGroup);
}


//!
//! Destructor of the GeometryNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
GeometryNode::~GeometryNode ()
{
	destroyEntity();
	OgreTools::destroyResourceGroup(m_oldResourceGroupName);
}


///
/// Public Functions
///

//!
//! Returns the scene node that contains scene objects created or modified
//! by this node.
//!
//! \return The scene node containing objects created or modified by this node.
//!
Ogre::SceneNode * GeometryNode::getSceneNode ()
{
    Ogre::SceneNode *result = getSceneNodeValue(m_outputGeometryName, true);
    if (!result)
        Log::error("Could not obtain scene node.", "GeometryNode::getSceneNode");
    return result;
}

//!
//! Returns the radius of the bounding sphere surrounding this object.
//!
//! \return The radius of the bounding sphere surrounding this object.
//!
double GeometryNode::getBoundingSphereRadius () const
{
    return 0;
}


//!
//! Change function for the Geometry File parameter.
//!
void GeometryNode::geometryFileChanged ()
{
	// load new mesh and skeleton
	loadMesh();

	if (!m_entity)
		return;

	// remove existing parameters in case of reload
	m_objectIdGroup->destroyAllParameters();	

	// Create Object Id parameter for each sub entity
	int numOfSubentities = m_entity->getNumSubEntities();
	const Ogre::Mesh::SubMeshNameMap &nameMap = m_entity->getMesh()->getSubMeshNameMap();
	const QString combineIDs = getStringValue("Combine Object IDs for Namespace");
	int objectID = 0;
	bool firstMatchFound = false;
	
	for (Ogre::Mesh::SubMeshNameMap::const_iterator iter = nameMap.begin(); iter != nameMap.end(); iter++) {		
		// create new parameter for each sub entity			

		// subsequent matches first otherwise firstMatchFound will be true
		if (QString::fromStdString(iter->first).startsWith(combineIDs) && firstMatchFound && combineIDs.size()>0){
			Ogre::SubEntity *subEntity = m_entity->getSubEntity(iter->first);
			subEntity->setCustomParameter(0, Ogre::Vector4((float) (objectID), 0.0, 0.0, 0.0));			
		}
		// first match to combine object Ids
		if (QString::fromStdString(iter->first).startsWith(combineIDs) && !firstMatchFound && combineIDs.size()>0){
			NumberParameter *objectIdParameter = new NumberParameter(combineIDs+"*", Parameter::T_Int, 0);
			objectIdParameter->setInputMethod(NumberParameter::IM_SpinBox);
			objectIdParameter->setMinValue(0);
			objectIdParameter->setMaxValue(255);
			objectIdParameter->setStepSize(1);
			objectIdParameter->setValue(QVariant((int) iter->second));
			Ogre::SubEntity *subEntity = m_entity->getSubEntity(iter->first);
			subEntity->setCustomParameter(0, Ogre::Vector4((float) (iter->second + 1)/255.f, 0.0, 0.0, 0.0));
			m_objectIdGroup->addParameter(objectIdParameter);
			objectIdParameter->setChangeFunction(SLOT(updateObjectId()));
			Parameter *affectedParameter = getParameter(m_outputGeometryName);
			objectIdParameter->addAffectedParameter(affectedParameter);
			firstMatchFound = true;
			objectID = iter->second + 1;
		}		
		// all non matches
		if (!QString::fromStdString(iter->first).startsWith(combineIDs) || combineIDs.size()==0) {
			NumberParameter *objectIdParameter = new NumberParameter(QString::fromStdString(iter->first), Parameter::T_Int, 0);
			objectIdParameter->setInputMethod(NumberParameter::IM_SpinBox);
			objectIdParameter->setMinValue(0);
			objectIdParameter->setMaxValue(255);
			objectIdParameter->setStepSize(1);
			objectIdParameter->setValue(QVariant((int) iter->second));
			Ogre::SubEntity *subEntity = m_entity->getSubEntity(iter->first);
			subEntity->setCustomParameter(0, Ogre::Vector4((float) (iter->second + 1)/255.f, 0.0, 0.0, 0.0));
			m_objectIdGroup->addParameter(objectIdParameter);
			objectIdParameter->setChangeFunction(SLOT(updateObjectId()));
			Parameter *affectedParameter = getParameter(m_outputGeometryName);
			objectIdParameter->addAffectedParameter(affectedParameter);
		}
	

	}

	// initialize the material number parameters
	parseMaterialParameters();

	// redraw node to display new parameters    
	forcePanelUpdate();
}


//!
//! Loads animation mesh.
//!
//! \return True, if successful loading of ogre mesh
//!            False, otherwise.
//!
bool GeometryNode::loadMesh ()
{
	QString filename = getStringValue("Geometry File");
	if (filename == "") {
		Log::debug(QString("Geometry file has not been set yet. (\"%1\")").arg(m_name), "GeometryNode::loadMesh");
		return false;
	}

	// obtain the OGRE scene manager
	Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
	if (!sceneManager) {
		Log::error("Could not obtain OGRE scene manager.", "GeometryNode::loadMesh");
		return false;
	}

	// destroy an existing OGRE entity for the mesh
	destroyEntity();

	// create new scene node
	m_sceneNode = OgreManager::createSceneNode(m_name);
	if (!m_sceneNode) {
		Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "GeometryNode::loadMesh");
		return false;
	}

	setValue(m_outputGeometryName, m_sceneNode, true);

	// check if the file exists
	if (!QFile::exists(filename)) {
		Log::error(QString("Mesh file \"%1\" not found.").arg(filename), "GeometryNode::loadMesh");
		return false;
	}

	// split the absolute filename to path and base filename
	int lastSlashIndex = filename.lastIndexOf('/');
	QString path = "";
	if (lastSlashIndex > -1) {
		path = filename.mid(0, lastSlashIndex);
		filename = filename.mid(lastSlashIndex + 1);
	}
	if (!filename.endsWith(".mesh")) {
		Log::error("The geometry file has to be an OGRE mesh file.", "GeometryNode::loadMesh");
		return false;
	}

	// destroy old resource group and generate new one
	QString resourceGroupName = QString::fromStdString(createUniqueName("ResourceGroup_" + filename + "_AnimatableMesh"));
	OgreTools::destroyResourceGroup(m_oldResourceGroupName);
	m_oldResourceGroupName = resourceGroupName;
	OgreTools::createResourceGroup(resourceGroupName, path);

	// create a new OGRE entity for the mesh file
	m_entity = sceneManager->createEntity(m_name.toStdString(), filename.toStdString());
	if (m_entity) {
		int numOfSubentities = m_entity->getNumSubEntities();
		bool SaC = getBoolValue("useSaC");
		for (int i = 0; i < numOfSubentities; ++i) {
			// create a new number parameter for the bone
			Ogre::SubEntity *subEntity = m_entity->getSubEntity(i);
			subEntity->setCustomParameter(0, Ogre::Vector4(0.0, 0.0, 0.0, 0.0));

			const Ogre::String subMeshMaterialName = subEntity->getMaterialName();
			std::string::size_type loc = subMeshMaterialName.find("Face");
			if( loc != std::string::npos && SaC ) {
				writeDataToVertices(m_entity->getSubEntity(i)->getSubMesh());
			}
		}

		m_sceneNode->attachObject(m_entity);
	}

	// create a container for the entity
	m_entityContainer = new OgreContainer(m_entity);
	m_entity->setUserAny(Ogre::Any(m_entityContainer));

	updateStatistics();
	Log::info(QString("Mesh file \"%1\" loaded.").arg(filename), "GeometryNode::loadMesh");
	return true;
}

//!
//! Removes the OGRE entity containing the mesh geometry from the scene and
//! destroys it along with the OGRE scene node.
//!
void GeometryNode::destroyEntity ()
{
	if (m_entity) {
		// stop sharing skeleton
		if (m_entity->getMesh()->hasSkeleton() && m_entity->sharesSkeletonInstance()){
			m_entity->stopSharingSkeletonInstance();
			m_entity->detachAllObjectsFromBone();
		}

		// delete entity container
		if (m_entityContainer) {
			delete m_entityContainer;
			m_entityContainer = 0;
		}

		// remove the entity from the scene node it is attached to
		Ogre::SceneNode *parentSceneNode = m_entity->getParentSceneNode();
		if (parentSceneNode)
			parentSceneNode->detachObject(m_entity);

		// destroy the entity through its scene manager
		Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
		if (sceneManager) {
			sceneManager->destroyEntity(m_entity);
			m_entity = 0;
		}
	}

	if (m_sceneNode) {
		// destroy the scene node through its scene manager
		OgreTools::deepDeleteSceneNode(m_sceneNode, OgreManager::getSceneManager(), true);
		setValue(m_outputGeometryName, m_sceneNode, true);
	}
}

//!
//! Update object id.
//!
void GeometryNode::updateObjectId ()
{
	const QString combineIDs = getStringValue("Combine Object IDs for Namespace");
	Parameter* parameter = dynamic_cast<Parameter*>(sender());	
	int numOfSubentities = m_entity->getNumSubEntities();
	const Ogre::Mesh::SubMeshNameMap &nameMap = m_entity->getMesh()->getSubMeshNameMap();	
		
	if (parameter && m_entity) {
		
		const QString &name = parameter->getName();
		int value = parameter->getValue().toInt();
			// Is not combined Object ID parameter
			if (!name.startsWith(combineIDs) || combineIDs.size()==0) {
				try {
					Ogre::SubEntity * subEntity = m_entity->getSubEntity(name.toStdString());
					if( subEntity ) {
						const Ogre::Vector4 newValue = Ogre::Vector4((float) value/255.0f, 0.0, 0.0, 0.0);
						subEntity->setCustomParameter(0, newValue);
						m_entityContainer->updateCustomParameter(name, newValue);
					}
				}
				catch( Ogre::Exception& e) {
					Log::error( "Ogre::Subentity: " + QString::fromStdString( e.getDescription()), "GeometryNode::updateObjectId");
				}
			}
			// Is combined Object ID parameter
			else {
				// iterate over all sub entites and set value
				try {
					for (Ogre::Mesh::SubMeshNameMap::const_iterator iter = nameMap.begin(); iter != nameMap.end(); iter++) {	
						if (QString::fromStdString(iter->first).startsWith(combineIDs)) {							
							Ogre::SubEntity *subEntity = m_entity->getSubEntity(iter->first);						
							if( subEntity ) {
								const Ogre::Vector4 newValue = Ogre::Vector4((float) value/255.0f, 0.0, 0.0, 0.0);
								subEntity->setCustomParameter(0, newValue);
								m_entityContainer->updateCustomParameter(QString::fromStdString(iter->first), newValue);
							}
						}
					}					
				}
				catch( Ogre::Exception& e) {
					Log::error( "Ogre::Subentity: " + QString::fromStdString( e.getDescription()), "GeometryNode::updateObjectId");
				}
			}		
	}
}

//!
//! Texture Reload Function
//!
void GeometryNode::reloadTexture ()
{
	/*Parameter *parameter = dynamic_cast<Parameter *>(sender());
	NumberParameter *numberParameter = static_cast<NumberParameter *>(parameter);

	Ogre::TextureManager::ResourceMapIterator textureResourceMapIterator = Ogre::TextureManager::getSingleton().getResourceIterator();
    while (textureResourceMapIterator.hasMoreElements())
        {
			Ogre::TexturePtr texturePtr = textureResourceMapIterator.getNext().staticCast<Ogre::Material>();
            std::string textureName = texturePtr->getName();
            std::string groupName = texturePtr->getGroup();
            if (textureName.empty()) {
                continue;
            }                        
			if (numberParameter->getName().toStdString()=="Reload All Textures") {
				texturePtr->reload();					         
			}
			if (!textureName.compare(numberParameter->getName().toStdString())) {
				texturePtr->reload();
				break;
            }			
	}*/
	//triggerRedraw();
}

//!
//! Change function for the Material parameters.
//!
void GeometryNode::setMaterialParameter ()
{
    Parameter *parameter = dynamic_cast<Parameter *>(sender());
    if (!parameter)
        return;
    else {
        NumberParameter *numberParameter = static_cast<NumberParameter *>(parameter);
		const Ogre::MeshPtr meshPtr = m_entity->getMesh();
		Ogre::Mesh::SubMeshIterator subMeshIter = meshPtr->getSubMeshIterator();
        while (subMeshIter.hasMoreElements()) {
            Ogre::SubMesh* subMesh = subMeshIter.getNext();
            Ogre::String subMeshMaterialName = subMesh->getMaterialName();
			Ogre::MaterialPtr subMeshMaterial;
			QStringList parameter_Material = numberParameter->getName().split("@");					
			if (getBoolValue("Global Material Parameter Update")) {
				subMeshMaterial = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(subMeshMaterialName); }
			else {				
				subMeshMaterial = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(parameter_Material[1].toStdString()); }
            if (!subMeshMaterial.isNull()) {
                Ogre::Technique* tech = subMeshMaterial->getTechnique(0);
                if (!tech)
                    continue;
                Ogre::Technique::PassIterator passIter = tech->getPassIterator();
				while (passIter.hasMoreElements()) {
                    Ogre::Pass* pass = passIter.getNext();
                    if (pass->hasFragmentProgram()) {
                        Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
						if (fpParams->_findNamedConstantDefinition(parameter_Material[0].toStdString()) != 0)
							// float
							if (numberParameter->getType() == Parameter::T_Float) {
								fpParams->setNamedConstant(parameter_Material[0].toStdString(), (Ogre::Real) numberParameter->getValue().toDouble());}
							// color
							else if (numberParameter->getType() == Parameter::T_Color) {
								QColor color = getColorValue(numberParameter->getName());								
								Ogre::ColourValue oColor;
								oColor.r = color.redF();
								oColor.g = color.greenF();
								oColor.b = color.blueF();								
								oColor.a = color.alphaF();							
								fpParams->setNamedConstant(parameter_Material[0].toStdString(), oColor)	;			
							}				
                    }
                    if (pass->hasVertexProgram()) {
                        Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
						if (vpParams->_findNamedConstantDefinition(parameter_Material[0].toStdString()) != 0)
                            vpParams->setNamedConstant(parameter_Material[0].toStdString(), (Ogre::Real) numberParameter->getValue().toDouble());
                    }
					if (pass->hasGeometryProgram()) {
						Ogre::GpuProgramParametersSharedPtr gpParams = pass->getGeometryProgramParameters();
                        if (gpParams->_findNamedConstantDefinition(parameter_Material[0].toStdString()) != 0)
                            gpParams->setNamedConstant(parameter_Material[0].toStdString(), (Ogre::Real) numberParameter->getValue().toDouble());
                    }
				}
            }
        }
    }
}



//!
//! Parse the Material Parameters and add to Material Parameter Group.
//!
void GeometryNode::parseMaterialParameters ()
{
	// remove old material parameters
	m_materialGroup->destroyAllParameters();

	// initialize the material number parameters
	const Ogre::MeshPtr meshPtr = m_entity->getMesh();
	if (meshPtr.isNull())
		return;

	Ogre::Mesh::SubMeshIterator subMeshIter = meshPtr->getSubMeshIterator();
	while (subMeshIter.hasMoreElements()) {
		Ogre::SubMesh* subMesh = subMeshIter.getNext();
		Ogre::String subMeshMaterialName = subMesh->getMaterialName();
		if (m_materialGroup->containsGroup(subMeshMaterialName.c_str()))
			continue;
		ParameterGroup *subMeshGroup = new ParameterGroup(subMeshMaterialName.c_str());
		m_materialGroup->addParameter(subMeshGroup);
		Ogre::MaterialPtr subMeshMaterial = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(subMeshMaterialName);
		if (!subMeshMaterial.isNull()) {
			Ogre::Technique* tech = subMeshMaterial->getTechnique(0);
			if (!tech)
				continue;
			Ogre::Technique::PassIterator passIter = tech->getPassIterator();
			while (passIter.hasMoreElements()) {
				Ogre::Pass* pass = passIter.getNext();
				ParameterGroup *passGroup = new ParameterGroup("Pass: " + QString(pass->getName().c_str()));
				subMeshGroup->addParameter(passGroup);

				// reload textures
				Ogre::Pass::TextureUnitStateIterator textureUnitStateIterator = pass->getTextureUnitStateIterator();
				if (textureUnitStateIterator.hasMoreElements()) { 
					ParameterGroup *texGroup = new ParameterGroup("Reload Textures:");
					passGroup->addParameter(texGroup);				
					NumberParameter *numberPara = new NumberParameter(QString("Reload All Textures"), Parameter::T_Command, "__rla__" + QString(pass->getName().c_str()) );					 					
					texGroup->addParameter(numberPara);
					numberPara->setCommandFunction(SLOT(reloadTexture()));					
					while (textureUnitStateIterator.hasMoreElements())
					{
						Ogre::TextureUnitState* textureUnitState = textureUnitStateIterator.getNext();
						NumberParameter *numberPara = new NumberParameter(QString(textureUnitState->getTextureName().c_str()), Parameter::T_Command, "reload");					 
						texGroup->addParameter(numberPara);
						numberPara->setCommandFunction(SLOT(reloadTexture()));						 
					}
				}				

				// fragment shader
				if (pass->hasFragmentProgram()) {
					ParameterGroup *progGroup = new ParameterGroup("FP: " + QString(pass->getFragmentProgram()->getName().c_str()));
					passGroup->addParameter(progGroup);
					Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
					const Ogre::GpuNamedConstants &namedConstants = fpParams->getConstantDefinitions();
					const Ogre::GpuConstantDefinitionMap &fpParamMap = namedConstants.map;
					Ogre::GpuConstantDefinitionMap::const_iterator fpParamIter;
					for (fpParamIter = fpParamMap.begin(); fpParamIter != fpParamMap.end(); ++fpParamIter) {
						// ignore auto parameters
						if (fpParams->findAutoConstantEntry(fpParamIter->first))
							continue;
						// float
						if	(fpParamIter->second.isFloat() && 
							(fpParamIter->second.constType == Ogre::GCT_FLOAT1) &&
							(fpParamIter->first.find("[0]") == Ogre::String::npos)) {
								float value = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex);
								NumberParameter *numberPara = new NumberParameter(QString(fpParamIter->first.c_str())+"@"+QString(subMeshMaterialName.c_str()), Parameter::T_Float, value);
								numberPara->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
								value = pow(10.0f, (float) ceil(log10(abs(value+0.001))));
								if (value < 0.01)
									value = 1.0;
								numberPara->setMinValue(-value);
								numberPara->setMaxValue( value);
								numberPara->setStepSize(value/100.0);
								progGroup->addParameter(numberPara);
								numberPara->setPinType(Parameter::PT_Input);		// 4DEMO
								numberPara->setSelfEvaluating(true);
								numberPara->setProcessingFunction(SLOT(setMaterialParameter()));
								numberPara->setChangeFunction(SIGNAL(triggerRedraw()));
								numberPara->setDescription(subMeshMaterialName.c_str());
								// if eye redness parameter exist create inputs
								if (numberPara->getName().contains("scleraRedness")) {
									numberPara->setProcessingFunction(SLOT(setMaterialParameter()));
									numberPara->setChangeFunction(SIGNAL(triggerRedraw()));
									numberPara->setPinType(Parameter::PT_Input);
									numberPara->setSelfEvaluating(true);
								}
						}
						// float3						
						if	(fpParamIter->second.isFloat() && 								
							(fpParamIter->second.constType == Ogre::GCT_FLOAT3) &&
							(fpParamIter->first.find("[0]") == Ogre::String::npos)) {	
								float value[3];
								QColor color;
								color.setRedF(*fpParams->getFloatPointer(fpParamIter->second.physicalIndex));		
								color.setGreenF(*fpParams->getFloatPointer(fpParamIter->second.physicalIndex+1));		
								color.setBlueF(*fpParams->getFloatPointer(fpParamIter->second.physicalIndex+2));										
								value[0] = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex);		
								value[1] = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex+1);	
								value[2] = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex+2);	

								NumberParameter *numberPara = new NumberParameter(QString(fpParamIter->first.c_str())+"@"+QString(subMeshMaterialName.c_str()), Parameter::T_Color, color);
								progGroup->addParameter(numberPara);
								numberPara->setSelfEvaluating(true);
								numberPara->setPinType(Parameter::PT_Input);
								numberPara->setProcessingFunction(SLOT(setMaterialParameter()));
								numberPara->setChangeFunction(SIGNAL(triggerRedraw()));
						}
						// float4
						if	(fpParamIter->second.isFloat() && 								
							(fpParamIter->second.constType == Ogre::GCT_FLOAT4) &&
							(fpParamIter->first.find("[0]") == Ogre::String::npos)) {	
								float value[4];
								QColor color;
								color.setRedF(*fpParams->getFloatPointer(fpParamIter->second.physicalIndex));		
								color.setGreenF(*fpParams->getFloatPointer(fpParamIter->second.physicalIndex+1));		
								color.setBlueF(*fpParams->getFloatPointer(fpParamIter->second.physicalIndex+2));										
								color.setAlphaF(*fpParams->getFloatPointer(fpParamIter->second.physicalIndex+3));										
								value[0] = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex);		
								value[1] = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex+1);	
								value[2] = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex+2);	
								value[3] = *fpParams->getFloatPointer(fpParamIter->second.physicalIndex+3);	

								NumberParameter *numberPara = new NumberParameter(QString(fpParamIter->first.c_str())+"@"+QString(subMeshMaterialName.c_str()), Parameter::T_Color, color);				
								progGroup->addParameter(numberPara);
								numberPara->setSelfEvaluating(true);
								numberPara->setPinType(Parameter::PT_Input);
								numberPara->setProcessingFunction(SLOT(setMaterialParameter()));
								numberPara->setChangeFunction(SIGNAL(triggerRedraw()));
						}
					}
				}
				// vertex shader
				if (pass->hasVertexProgram()) {
					ParameterGroup *progGroup = new ParameterGroup("VP: " + QString(pass->getVertexProgram()->getName().c_str()));
					passGroup->addParameter(progGroup);
					Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
					const Ogre::GpuNamedConstants &namedConstants = vpParams->getConstantDefinitions();
					const Ogre::GpuConstantDefinitionMap &vpParamMap = namedConstants.map;
					Ogre::GpuConstantDefinitionMap::const_iterator vpParamIter;					
					for (vpParamIter = vpParamMap.begin(); vpParamIter != vpParamMap.end(); ++vpParamIter) {
						// ignore auto parameters
						if (vpParams->findAutoConstantEntry(vpParamIter->first))
							continue;
						if	(vpParamIter->second.isFloat() && 
							(vpParamIter->second.constType == Ogre::GCT_FLOAT1) &&
							(vpParamIter->first.find("[0]") == Ogre::String::npos)) {
								float value = *vpParams->getFloatPointer(vpParamIter->second.physicalIndex);
								NumberParameter *numberPara = new NumberParameter(QString(vpParamIter->first.c_str())+"@"+QString(subMeshMaterialName.c_str()), Parameter::T_Float, value);
								numberPara->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
								value = pow(10.0f, (float) ceil(log10(abs(value+0.001))));
								numberPara->setMinValue(-value);
								numberPara->setMaxValue( value);
								numberPara->setStepSize(value/100.0);
								progGroup->addParameter(numberPara);
								numberPara->setPinType(Parameter::PT_Input);		// 4DEMO
								numberPara->setSelfEvaluating(true);
								numberPara->setProcessingFunction(SLOT(setMaterialParameter()));
								numberPara->setChangeFunction(SIGNAL(triggerRedraw()));
								numberPara->setDescription(subMeshMaterialName.c_str());
						}
					}
				}
				// geometry shader
				if (pass->hasGeometryProgram()) {
					ParameterGroup *progGroup = new ParameterGroup("GP: " + QString(pass->getGeometryProgram()->getName().c_str()));
					passGroup->addParameter(progGroup);
					Ogre::GpuProgramParametersSharedPtr gpParams = pass->getGeometryProgramParameters();
					const Ogre::GpuNamedConstants &namedConstants = gpParams->getConstantDefinitions();
					const Ogre::GpuConstantDefinitionMap &gpParamMap = namedConstants.map;
					Ogre::GpuConstantDefinitionMap::const_iterator gpParamIter;							
					for (gpParamIter = gpParamMap.begin(); gpParamIter != gpParamMap.end(); ++gpParamIter) {
						// ignore auto parameters
						if (gpParams->findAutoConstantEntry(gpParamIter->first))
							continue;
						if	(gpParamIter->second.isFloat() && 
							(gpParamIter->second.constType == Ogre::GCT_FLOAT1) &&
							(gpParamIter->first.find("[0]") == Ogre::String::npos)) {
								float value = *gpParams->getFloatPointer(gpParamIter->second.physicalIndex);
								NumberParameter *numberPara = new NumberParameter(QString(gpParamIter->first.c_str())+"@"+QString(subMeshMaterialName.c_str()), Parameter::T_Float, value);
								numberPara->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
								value = pow(10.0f, (float) ceil(log10(abs(value+0.001))));
								numberPara->setMinValue(-value);
								numberPara->setMaxValue( value);
								numberPara->setStepSize(value/100.0);
								numberPara->setPinType(Parameter::PT_Input);		// 4DEMO
								progGroup->addParameter(numberPara);
								numberPara->setProcessingFunction(SLOT(setMaterialParameter()));
								numberPara->setChangeFunction(SIGNAL(triggerRedraw()));
								numberPara->setDescription(subMeshMaterialName.c_str());
						}
					}
				}
			}
		}
	}
}


///
/// Private Functions
///


//!
//! Attaches a locator geometry to the geometry node's scene node.
//!
void GeometryNode::createLocatorGeometry ()
{
    //Ogre::Root *ogreRoot = Ogre::Root::getSingletonPtr();
    //Ogre::SceneManager *sceneManager = 0;
    //if (ogreRoot)
    //    sceneManager = ogreRoot->getSceneManager(SCENE_MANAGER_NAME);
    //if (!sceneManager) {
    //    Log::error("The OGRE scene manager could not be obtained.", "NullNode::createGeometry");
    //    return;
    //}

    //if (!m_nullGeometrySceneNode) {
    //    Log::error("The null geometry's OGRE scene node could not be obtained.", "NullNode::createGeometry");
    //    return;
    //}

    //Ogre::ManualObject *xAxis = sceneManager->createManualObject(QString(m_name + "_xAxis").toStdString());
    //xAxis->begin("nullMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    //xAxis->position(-NullGeometryScale, 0, 0);
    //xAxis->position(NullGeometryScale, 0, 0);
    //xAxis->end();

    //Ogre::ManualObject *yAxis = sceneManager->createManualObject(QString(m_name + "_yAxis").toStdString());
    //yAxis->begin("nullMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    //yAxis->position(0, -NullGeometryScale, 0);
    //yAxis->position(0, NullGeometryScale, 0);
    //yAxis->end();

    //Ogre::ManualObject *zAxis = sceneManager->createManualObject(QString(m_name + "_zAxis").toStdString());
    //zAxis->begin("nullMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    //zAxis->position(0, 0, -NullGeometryScale);
    //zAxis->position(0, 0, NullGeometryScale);
    //zAxis->end();

    //m_nullGeometrySceneNode->attachObject(xAxis);
    //m_nullGeometrySceneNode->attachObject(yAxis);
    //m_nullGeometrySceneNode->attachObject(zAxis);
}

//!
//! Insert unique IDs in a texture coordinate for every vertex of the mesh that is
//! on the same spatial position (e.g. for wrinkle mapping)
//!
//! \param mesh Submesh that contains the vertices (typically the skin submesh)
//!	\return Pointer to the VertexBuffer that gets modified by this function
//!
Ogre::HardwareVertexBufferSharedPtr GeometryNode::writeDataToVertices(Ogre::SubMesh* mesh) 
{
	// get Buffers
	// In Buffer 0 are normally the POSITIONs and the NORMALS
	Ogre::HardwareVertexBufferSharedPtr vertexBuffer = mesh->vertexData->vertexBufferBinding->getBuffer(0); 
	Ogre::HardwareIndexBufferSharedPtr indexBuffer = mesh->indexData->indexBuffer;

	// some general information about the buffers
	size_t numberOfBuffers = mesh->vertexData->vertexBufferBinding->getBufferCount();
	unsigned short bufferID = short(numberOfBuffers);
	unsigned short coordNr = 1;

	// get some information about the vertices
	size_t vertexAmount = mesh->vertexData->vertexCount;
	size_t vertexSizeInByteOfBuffer0 = mesh->vertexData->vertexDeclaration->getVertexSize(0); // Vertex Size in Byte

	// get some attributes of the indexBuffer
	size_t numberOfIndexBufferEntries = indexBuffer->getNumIndexes();
	size_t sizePerIndex = indexBuffer->getIndexSize();
	size_t IndexBufferSizeInByte = indexBuffer->getSizeInBytes();

	// define two Elements for a new HardwareVertexBuffer
	mesh->vertexData->vertexDeclaration->addElement(bufferID, 0, Ogre::VET_FLOAT1, Ogre::VES_BINORMAL);

	// generate the new HardwareVertexBuffer
	Ogre::HardwareVertexBufferSharedPtr specialDataVertexBufferPtr = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		mesh->vertexData->vertexDeclaration->getVertexSize(bufferID),
		mesh->vertexData->vertexCount,
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	// bind the new Vertexbuffer
	mesh->vertexData->vertexBufferBinding->setBinding(bufferID, specialDataVertexBufferPtr);

	// lock the new vertex buffer for writing
	float* bufferPtrNew = static_cast<float*>(specialDataVertexBufferPtr->lock(Ogre::HardwareBuffer::HBL_DISCARD));

	// Locking the old vertexbuffer
	float* bufferPtrOld = static_cast<float*>(vertexBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

	// Copy data to new buffer
	int numberOfFloatsPerVertex = int(vertexSizeInByteOfBuffer0 / 4); // 4 = sizeof(float)

	QList<Ogre::Vector3> vertices;

	for(Ogre::uint i=0; i<vertexAmount; i++) 
	{

		// 3 floats for the positions (x,y,z)
		Ogre::Vector3 position;
		position.x = *bufferPtrOld++;
		position.y = *bufferPtrOld++;
		position.z = *bufferPtrOld;

		// generate the list with all vertex positions in it once
		if (vertices.length() == 0)
		{
			vertices.append( position );
			*bufferPtrNew++ = (float) 0;
		} 
		else 
		{
			for (int vi=0; vi<vertices.length(); vi++) 
			{
				// compare to previous vertices
				if ( vertices[vi] == position )
				{
					*bufferPtrNew++ = (float) vi+1;
					vi = vertices.length();
				} 
				// end of the road
				else if(vi == vertices.length()-1)
				{
					vertices.append(position);
					*bufferPtrNew++ = (float) vi+1;
					vi += 2;
				}
			}
		}
	}

	// clean up
	vertices.clear(); 
	vertexBuffer->unlock();
	specialDataVertexBufferPtr->unlock();

	// Reorganize bufferstructure
#if(OGRE_VERSION >= 0x010800)
	mesh->vertexData->reorganiseBuffers(mesh->vertexData->vertexDeclaration->getAutoOrganisedDeclaration(true, false, false));
#else
	mesh->vertexData->reorganiseBuffers(mesh->vertexData->vertexDeclaration->getAutoOrganisedDeclaration(true, false));
#endif
	return vertexBuffer;
}

//!
//! Retrieves the numbers of vertices and triangles from the mesh and stores
//! them in parameters of this node.
//!
void GeometryNode::updateStatistics ()
{
	unsigned int numVertices = 0;
	unsigned int numTriangles = 0;

	if (m_entity) {
		const Ogre::MeshPtr &mesh = m_entity->getMesh();
		if (!mesh.isNull()) {
			bool sharedAdded = false;
			unsigned int indexCount = 0;

			for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
				Ogre::SubMesh* subMesh = mesh->getSubMesh(i);
				if (subMesh->useSharedVertices) {
					if (!sharedAdded) {
						numVertices += (unsigned int) mesh->sharedVertexData->vertexCount;
						sharedAdded = true;
					}
				} else {
					numVertices += (unsigned int) subMesh->vertexData->vertexCount;
				}

				indexCount += (unsigned int) subMesh->indexData->indexCount;
			}

			numTriangles = indexCount / 3;
		}
	}

	blockSignals(true);
	setValue("Number of Vertices", QString("%L1").arg(numVertices), true);
	setValue("Number of Triangles", QString("%L1").arg(numTriangles), true);
	blockSignals(false);
}

} // end namespace Frapper

