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
//! \file "TextureGeometry.h"
//! \brief Implementation file for TextureGeometry class.
//!
//! \author     Amit Rojtblat <amit.rojtblat@filmakademie.de>
//! \version    1.0
//! \date       22.02.2014 (last updated)
//!

#include "TextureGeometryNodeAbstract.h"
#include "OgreManager.h"
#include "OgreTools.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTime>

namespace Frapper{

///
/// Constructors and Destructors
///


//!
//! Constructor of the TextureGeometry class.
//!
//! \param name The name to give the new mesh node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
TextureGeometryNodeAbstract::TextureGeometryNodeAbstract ( const QString &name, ParameterGroup *parameterRoot ) :
    ViewNode(name, parameterRoot),
	m_sceneManager(0),
	m_sceneNode(0),
	m_pointsPerPrimitive(16777216),
    m_dummyManualObject(0),
	m_pointCountOutParameter(0),
	m_pointCountParameter(0),
	m_oldPointCount(0){

	//init the empty texture
	initEmptyTexture();

	Ogre::Root *ogreRoot = OgreManager::getRoot();
	m_sceneManager = OgreManager::getSceneManager();
	if (ogreRoot&&m_sceneManager) {
		// create new scene node
		m_sceneNode = OgreManager::createSceneNode(m_name);
		createDummyManualObject();
    } else
            Log::error("could not create sceneManager!", "TextureGeometry");

	//add the max point count parameter
	m_enumMaxPointParameter = new EnumerationParameter("maxPointCount",1);
	m_enumMaxPointParameter->setPinType(Frapper::Parameter::PT_Input);
	m_enumMaxPointParameter->setNode(this);
	parameterRoot->addParameter(m_enumMaxPointParameter);
	QStringList values;
	values.append(QString::number(256));
	values.append(QString::number(512));
	values.append(QString::number(1024));
	values.append(QString::number(2048));
	values.append(QString::number(4096));
	QStringList literals;
	literals.append("65,536");
	literals.append("262,144");
	literals.append("1,048,576");
	literals.append("4,194,304");
	literals.append("16,777,216");
	m_enumMaxPointParameter->setValues(values);
	m_enumMaxPointParameter->setLiterals(literals);
	m_enumMaxPointParameter->setSelfEvaluating(true);
	m_enumMaxPointParameter->setProcessingFunction(SLOT(setMaxPointCountProcessingFunction()));

	//add the pointCount parameter
	m_pointCountParameter = new NumberParameter("pointCount",Frapper::Parameter::T_Int,0);
	m_pointCountParameter->setInputMethod(NumberParameter::IM_SliderPlusSpinBox);
	m_pointCountParameter->setPinType(Frapper::Parameter::PT_Input);
	m_pointCountParameter->setNode(this);
	m_pointCountParameter->setMaxValue(512*512);
	m_pointCountParameter->setMinValue(0);
	m_pointCountParameter->setSelfEvaluating(true);
	parameterRoot->addParameter(m_pointCountParameter);
	m_pointCountParameter->setProcessingFunction(SLOT(pointCountProcessingFunction()));

	//add the output max point count parameter
	m_maxPointCountOutParameter = new NumberParameter("maxPointCountOut",Frapper::Parameter::T_Int,512*512);
	m_maxPointCountOutParameter->setPinType(Frapper::Parameter::PT_Output);
	m_maxPointCountOutParameter->setNode(this);
	m_maxPointCountOutParameter->setMaxValue(4096*4096);
	m_maxPointCountOutParameter->setMinValue(0);
	parameterRoot->addParameter(m_maxPointCountOutParameter);
	m_maxPointCountOutParameter->addAffectingParameter(getParameter("maxPointCount"));
	m_maxPointCountOutParameter->setProcessingFunction(SLOT(setMaxOutPointCount()));

	//add the output point count parameter
	m_pointCountOutParameter = new NumberParameter("pointCountOut",Frapper::Parameter::T_Int,0);
	m_pointCountOutParameter->setPinType(Frapper::Parameter::PT_Output);
	m_pointCountOutParameter->setNode(this);
	m_pointCountOutParameter->setMaxValue(512*512);
	m_pointCountOutParameter->setMinValue(0);
	parameterRoot->addParameter(m_pointCountOutParameter);
	m_pointCountOutParameter->addAffectingParameter(getParameter("pointCount"));
	m_pointCountOutParameter->setProcessingFunction(SLOT(setOutPointCount()));
	

}


//!
//! Destructor of the TextureGeometry class.
//!
TextureGeometryNodeAbstract::~TextureGeometryNodeAbstract ()
{

	//destroy the dummy manual object
    destroyDummyManualObject();

	//destroy the empty texture
	if(!m_emptyTexture.isNull()){
		Ogre::TextureManager::getSingleton().remove("EmptyTexture");
	}

	// clear the list
	m_inputTextureList.clear();

    //emit viewNodeUpdated();

}

///
/// Public functions
///

//!
//! Add an input texture target
//!
Parameter* TextureGeometryNodeAbstract::addInputRenderTexture(const QString& parameterName){

	// create a new input texture parameter and assign it to this node
	// create an empty texture pointer to hold the address of this parameter
	Parameter* newInputParameter = Parameter::createImageParameter(parameterName,m_emptyTexture);
	newInputParameter->setNode(this);
	newInputParameter->setPinType(Frapper::Parameter::PT_Input);
	newInputParameter->setSelfEvaluating(true);
	newInputParameter->setProcessingFunction(SLOT(updateInput()));
	getParameterRoot()->addParameter(newInputParameter);
	return newInputParameter;
}

//!
//! set inpupt texture to affect the specified texture unit state of an output texture
//!
void TextureGeometryNodeAbstract::addTusToParameter(Parameter* p, Ogre::TextureUnitState* t){
	m_inputTextureList[p->getName()].append(t);
}

//!
//! create a copy of the material for this node instance
//! \param name the name of the material to copy
//!
Ogre::MaterialPtr TextureGeometryNodeAbstract::copyMaterial(Ogre::String name){

	Ogre::MaterialPtr& newCopy = Ogre::MaterialManager::getSingleton().getByName(name)->clone(m_name.toStdString()+"_"+name);
	m_materialsList.append(newCopy);
	return newCopy;
}

//!
//! Creates an image from the given texture.
//!
//! \param texturePointer The texture from which to copy the image data.
//! \return The image created from the given texture, or 0 if the given texture is invalid.
//!
void TextureGeometryNodeAbstract::createImageFromTexture( const Ogre::TexturePtr texturePointer, Ogre::Image& outputImage)
{
    if (texturePointer.isNull()) {
        Log::error("Could not create image from texture: Invalid texture given.", "ImageNode::createImageFromTexture");
        return;
    }

	// get the pixel format
	Ogre::PixelFormat outputPixelFormat = texturePointer->getFormat();

    // obtain dimensions of given texture
    size_t width = texturePointer->getWidth();
    size_t height = texturePointer->getHeight();

    // create an empty image with the requested resolution
    size_t memorySize = Ogre::PixelUtil::getMemorySize(width, height, 1, outputPixelFormat );
    Ogre::uchar *imageData = OGRE_ALLOC_T(Ogre::uchar, memorySize, Ogre::MEMCATEGORY_GENERAL);
    outputImage.loadDynamicImage(imageData, width, height, 1, outputPixelFormat, true);

    // copy the render texture's image data to the image
    texturePointer->getBuffer()->blitToMemory(outputImage.getPixelBox());
}

//!
//! set the material to be affected by count
//!
void TextureGeometryNodeAbstract::setAffectedBycount(Ogre::MaterialPtr material){

	// add the material to the list
	m_materialsAffectedByCount.append(material);
}

///
/// Protected Functions
///

///
/// public Slots
///

//!
//! set the pointcount value
//!
void TextureGeometryNodeAbstract::setPointCount(int i){

	getParameter("pointCount")->setValue(QVariant(i));
	setOutPointCount();

}

//!
//! Processing function of the input point count
//! sets the count int the fragment and hull shader if they exist and have the parameter
//!
void TextureGeometryNodeAbstract::pointCountProcessingFunction(){

	QList<Ogre::MaterialPtr>::iterator iter;

	int count = (dynamic_cast<Parameter *>(sender()))->getValue().toInt();

	//fix the primitive count
	int oldCount = m_oldPointCount;
	if(oldCount!=count){
		int oldStep = oldCount/m_pointsPerPrimitive;
		int newStep = count/m_pointsPerPrimitive;
		if(oldStep!=newStep){
			m_dummyManualObject->beginUpdate(0);
			//m_dummyManualObject->begin("DefaultShaderMaterial", Ogre::RenderOperation::OT_LINE_LIST);
			for(int i=0;i<newStep+1;i++){
				m_dummyManualObject->position(0,1,0);
				m_dummyManualObject->position(1,0,0);
			}
			m_dummyManualObject->end();
			m_dummyManualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		}
	}

	m_oldPointCount = count;

	for(	iter=m_materialsAffectedByCount.begin();
			iter!=m_materialsAffectedByCount.end();
			++iter){
	
		Ogre::MaterialPtr mat = *iter;
		Ogre::Pass* pass = (*iter)->getTechnique(0)->getPass(0);
#if (OGRE_VERSION >= 0x010A00)
		//check if hull shader exists and set it
		if(pass->hasTessellationHullProgram())
			if(pass->getTessellationHullProgramParameters()->_findNamedConstantDefinition("count"))
				pass->getTessellationHullProgramParameters()->setNamedConstant("count",count);
#else
		//check if hull shader exists and set it
		if(pass->hasTesselationHullProgram())
			if(pass->getTesselationHullProgramParameters()->_findNamedConstantDefinition("count"))
				pass->getTesselationHullProgramParameters()->setNamedConstant("count",count);
#endif

		//check if geometry shader exists and set it
		if(pass->hasGeometryProgram())
			if(pass->getGeometryProgramParameters()->_findNamedConstantDefinition("count"))
				pass->getGeometryProgramParameters()->setNamedConstant("count",count);


		//cheack for fragment shader
		if(pass->getFragmentProgramParameters()->_findNamedConstantDefinition("count"))
			pass->getFragmentProgramParameters()->setNamedConstant("count",count);

	}

	//triggerRedraw();

}

//!
//! Processing function of the input max point count
//! sets the resolution in the geometry if they exist and have the parameter
//!
void TextureGeometryNodeAbstract::setMaxPointCountProcessingFunction(){

	QList<Ogre::MaterialPtr>::iterator iter;

	int resolution = (dynamic_cast<EnumerationParameter *>(sender()))->getCurrentValue().toInt();

	for(	iter=m_materialsAffectedByCount.begin();
			iter!=m_materialsAffectedByCount.end();
			++iter){
	
		Ogre::MaterialPtr mat = *iter;
		Ogre::Pass* pass = (*iter)->getTechnique(0)->getPass(0);

#if (OGRE_VERSION >= 0x010A00)
	//check if hull shader exists and set it
	if(pass->hasTessellationHullProgram())
		if(pass->getTessellationHullProgramParameters()->_findNamedConstantDefinition("resolution"))
			pass->getTessellationHullProgramParameters()->setNamedConstant("resolution",resolution);
#else
	//check if hull shader exists and set it
	if(pass->hasTesselationHullProgram())
		if(pass->getTesselationHullProgramParameters()->_findNamedConstantDefinition("resolution"))
			pass->getTesselationHullProgramParameters()->setNamedConstant("resolution",resolution);
#endif
		//check if geometry shader exists and set it
		if(pass->hasGeometryProgram())
			if(pass->getGeometryProgramParameters()->_findNamedConstantDefinition("resolution"))
				pass->getGeometryProgramParameters()->setNamedConstant("resolution",resolution);


		//cheack for fragment shader
		if(pass->getFragmentProgramParameters()->_findNamedConstantDefinition("resolution"))
			pass->getFragmentProgramParameters()->setNamedConstant("resolution",resolution);

	}

}

///
/// Private Functions
///

//!
//! creates the dummy manual object
//!
//! \return True, if successful loading of ogre mesh
//! False, otherwise.
//!
bool TextureGeometryNodeAbstract::createDummyManualObject()
{

    if (!m_sceneNode) {
        Log::error(QString("Scene node for node \"%1\" could not be created.").arg(m_name), "TextureGeometry::createDummyManualObject");
        return false;
    }

    // create a new OGRE entity for the buffer
	m_dummyManualObject = m_sceneManager->createManualObject((m_name + "_Dummy").toStdString());
	m_dummyManualObject->begin("DefaultShaderMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	m_dummyManualObject->position(0,1,0);
	m_dummyManualObject->position(1,0,0);
	m_dummyManualObject->end();
	//m_dummyManualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
    m_sceneNode->attachObject(m_dummyManualObject);

    return true;
}


//!
//! Removes the OGRE entity containing the mesh geometry from the scene and
//! destroys it along with the OGRE scene node.
//!
void TextureGeometryNodeAbstract::destroyDummyManualObject ()
{
    if (m_dummyManualObject) {
        // remove the entity from the scene node it is attached to
        Ogre::SceneNode *parentSceneNode = m_dummyManualObject->getParentSceneNode();
        if (parentSceneNode)
            parentSceneNode->detachObject(m_dummyManualObject);

        // destroy the entity through its scene manager
        Ogre::SceneManager *sceneManager = OgreManager::getSceneManager();
        if (sceneManager) {
			sceneManager->destroyManualObject(m_dummyManualObject);
            m_dummyManualObject = 0;
        }
    }

    if (m_sceneNode) {
        // destroy the scene node through its scene manager
        Ogre::SceneManager *sceneManager = m_sceneNode->getCreator();
        if (sceneManager) {
            sceneManager->destroySceneNode(m_sceneNode);
            m_sceneNode = 0;
        }
    }
}

//!
//! Initialize the empty texture
//!
void TextureGeometryNodeAbstract::initEmptyTexture(){

	if(!Ogre::TextureManager::getSingleton().resourceExists("EmptyTexture")){
		m_emptyTexture = Ogre::TextureManager::getSingleton().createManual("EmptyTexture",
											Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
											Ogre::TEX_TYPE_2D, 
											2, 2,
											0,
											Ogre::PixelFormat::PF_R8G8B8A8,
											Ogre::TU_RENDERTARGET);
	} else 
		if(m_emptyTexture.isNull()) m_emptyTexture = Ogre::TextureManager::getSingleton().getByName("EmptyTexture");

}

///
/// Private Slots
///

//!
//! upadte inputs
//!
void TextureGeometryNodeAbstract::updateInput(){

	// get the parameter
	Parameter* param = dynamic_cast<Parameter *>(sender());

	// get the input pointer
	Ogre::TexturePtr& input = param->getValue().value<Ogre::TexturePtr>();

	// check if connected
	bool connected = param->isConnected();

	// loop through all affected tus
	QList<Ogre::TextureUnitState*>::iterator iter;
	for(iter=m_inputTextureList[param->getName()].begin();
		iter!=m_inputTextureList[param->getName()].end();
		++iter){

			Ogre::TextureUnitState* tus = *iter;

		// if the input is connected
		if(connected){
			// check if input is not null
			if(!input.isNull()){
				tus->setTexture(input);
			}
		} else {

			// set the poiner to point at the empty texture
			tus->setTexture(m_emptyTexture);
		}
	}

	triggerRedraw();
}

//!
//! upadte the output point count
//!
void TextureGeometryNodeAbstract::setOutPointCount(){

	m_pointCountOutParameter->setValue(getValue("pointCount"),true);

}

//!
//! upadte the output point count
//!
void TextureGeometryNodeAbstract::setMaxOutPointCount(){

	// get the parameter
	NumberParameter* param = dynamic_cast<NumberParameter *>(sender());
	
	int value = getEnumerationParameter("maxPointCount")->getCurrentValue().toInt();
	param->setValue((QVariant)value,false);

}

} // end namespace
