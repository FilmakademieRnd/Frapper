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
//! \brief Implementation file for MaskEditNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.1
//! \date       16.06.2011 (last updated)
//!


// Frapper
#include "MaskEditNode.h"
#include "OgreTools.h"
#include "GenericParameter.h"

#include "../CImgListTools/CImgListTools.hpp"
#include "../CharonInterface/CharonInterface.hpp"

namespace MaskEditNode {

Q_DECLARE_METATYPE(Slot *);

using namespace cimg_library;

///
/// Constructors and Destructors
///

template <typename T>
MaskEditNodeParameteredObject<T>::MaskEditNodeParameteredObject(const std::string& name) :
	TemplatedParameteredObject<T>( "MaskEditNode", name,  "Show and Edit an image mask from a CImgList"),
	images_in( true, false) // optional=true, multi=false
{
	this->_addInputSlot(  images_in, "Images In",  "The input images", "CImgList<T>");
	this->_addOutputSlot( masks_out, "Masks Out",  "The output masks", "CImgList<T>");
}

//!
//! Constructor of the MaskEditNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
MaskEditNode::MaskEditNode ( const QString &name, Frapper::ParameterGroup *parameterRoot ) :
	PainterPanel::PainterPanelNode( name, parameterRoot ),
	m_imageParameter(0),
	m_selectedType(0),
	m_maskChanged(false),
	m_currentFrame(-1),
	m_sceneChanged(true) // init with true to allow immediate rendering of sequence
{	

	m_charonInterface = new CharonInterface(name);
	connect( m_charonInterface, SIGNAL(requestDeleteConnection(Frapper::Connection*)), this, SLOT(deleteConnection(Frapper::Connection*)));

	// template type T of this node
	Frapper::EnumerationParameter *templateTypeParameter = getEnumerationParameter("Template Type");
	assert( templateTypeParameter);
	templateTypeParameter->setChangeFunction( SLOT(changeTemplateType()) );
	QString templateInfoStr = "CImgList<" + templateTypeParameter->getCurrentLiteral() + ">";

	// the output image
	m_imageParameter = getParameter( m_outputImageName );
	m_imageParameter->setProcessingFunction( SLOT(processOutputImage()));


	// Images In Parameter
	Frapper::GenericParameter* imagesInParameter  = getGenericParameter( "Images In" );
	assert( imagesInParameter );
	imagesInParameter->setTypeInfo( templateInfoStr );
	imagesInParameter->setOnConnectFunction( SLOT(connectSlots()) );
	imagesInParameter->setOnDisconnectFunction( SLOT(disconnectSlots(int)) );
	imagesInParameter->addAffectedParameter( m_imageParameter );

	// Masks out
	Frapper::GenericParameter* masksOutParameter  = getGenericParameter( "Masks Out" );
	assert( masksOutParameter );
	masksOutParameter->setTypeInfo( templateInfoStr );
	masksOutParameter->setProcessingFunction( SLOT(renderSequence()) );

	
	// Mask in	
	Frapper::Parameter* maskInParameter = getParameter("Mask In");
	assert( maskInParameter );
	maskInParameter->setOnConnectFunction( SLOT( maskInParameterConnected() ));
	maskInParameter->addAffectedParameter( m_imageParameter );

	// Edge in
	Frapper::Parameter* edgeInParameter = getParameter("Edge In");
	assert( edgeInParameter );
	//edgeInParameter->setOnConnectFunction( SLOT( updateOutput() ));
	edgeInParameter->addAffectedParameter( m_imageParameter );


	// Need this extra parameter as m_timeParameter is initialized after the c'tor
	// The change function is called on scene load
	setChangeFunction("Update on frame change",   SLOT( updateOnFrameChanged()));
	setChangeFunction("Use LUT",                  SLOT( useLUTChanged()));
	setChangeFunction("Background Darken Factor", SLOT( darkenBackgroundChanged() ));

	setChangeFunction("Edge Layer > Layer Color", SLOT( updateEdgeLayer() ));

	// Initialize default textures
	initTextures();

	// Initialize template type for CImg
	changeTemplateType();

	setCommandFunction("Render Frame",	  SLOT(renderFrame()));
	setCommandFunction("Render Sequence", SLOT(renderSequence()));

	setCommandFunction("Clear Frame",	 SLOT(clearFrame()));
	setCommandFunction("Clear Sequence", SLOT(clearSequence()));

	// clear painter panel items group unless we can store and load items properly
	getParameterGroup("Painter Panel Items")->destroyAllParameters();
}


//!
//! Destructor of the MaskEditNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
MaskEditNode::~MaskEditNode ()
{
}


// public functions

Ogre::TexturePtr MaskEditNode::getBackground()
{
	int frame = m_timeParameter->getValue().toInt();
	if( m_currentFrame != frame) 
	{
		// get background image from input images list
		imageFromList( m_inputImage , "Images In", frame );
	}
	return m_inputImage;
}

Ogre::TexturePtr MaskEditNode::getMask()
{
	int frame = m_timeParameter->getValue().toInt();
	if( m_currentFrame != frame) {

		// get mask from input mask image parameter
		m_inputMask = getTextureValue("Mask In", true);
	}
	return m_inputMask;
}

Frapper::ParameterGroup* MaskEditNode::getItemParameterGroup()
{
	return getParameterGroup("Painter Panel Items");
}

void MaskEditNode::onSceneChanged()
{
	m_sceneChanged = true;
}

Frapper::Node* MaskEditNode::getNode()
{
	return this;
}

//!
//! Update the output image and mask, if current frame has changed
//! Also write back the mask to the output list if it was altered
//!
void MaskEditNode::processOutputImage()
{
	int frame = m_timeParameter->getValue().toInt();
	
	if( m_currentFrame != frame )
	{
		// update background (-> getBackground())
		// update mask (-> getMask())
		// update scene items according to frame number
		updateScene( frame );

		// get mask from input mask image parameter
		updateEdgeLayer();

		m_currentFrame = frame;
	}
}

void MaskEditNode::renderFrame()
{
	if( !m_inputImage.isNull() ) {
		renderSceneToImage( m_outputMask, m_inputImage->getWidth(), m_inputImage->getHeight() );
	} else if( !m_inputMask.isNull()) {
		renderSceneToImage( m_outputMask, m_inputMask->getWidth(), m_inputMask->getHeight() );
	} else {
		renderSceneToImage( m_outputMask );
	}

	imageToMasksList( m_outputMask );
	setOutputImage( m_outputMask );
}

void MaskEditNode::renderSequence()
{
	if( !m_sceneChanged)
		return;

	// prevent update via timeline parameter while rendering...
	setTimeparameterAffection(false);

	// prevent loops
	getGenericParameter( "Masks Out" )->setProcessingFunction("");

	int currentFrame = m_currentFrame; // store for later
	m_currentFrame = -1; // force refresh of current frame

	int inFrame  = m_rangeParameter->getMinValue().toInt();
	int outFrame = m_rangeParameter->getMaxValue().toInt();
	int nFrames  = outFrame-inFrame+1; //getImageListSize("Images In");

	QProgressDialog pd( "Rendering Sequence...", "Abort Rendering", 0, nFrames );
	pd.setWindowModality( Qt::WindowModal);
	pd.show();

	for( int i=0; i < nFrames; i++ )
	{
		pd.setValue( i );
		if( pd.wasCanceled())
			break;

		// set current frame for all nodes via sceneModel
		m_timeParameter->setValue( QVariant( inFrame + i), true);
		emit frameChanged( inFrame + i );

		processOutputImage();
		renderFrame();
	}

	// set current frame for all nodes via sceneModel
	m_timeParameter->setValue( QVariant( currentFrame), true);
	emit frameChanged(currentFrame);

	processOutputImage();

	// reset timeline affection
	setTimeparameterAffection(getBoolValue("Update on frame change"));

	getGenericParameter( "Masks Out" )->setProcessingFunction(SLOT(renderSequence()));

	m_sceneChanged = false;
}

void MaskEditNode::inputImagesChanged()
{
	m_currentFrame = -1; // force update of output image
}

void MaskEditNode::imageFromList( Ogre::TexturePtr& image, const QString& slotName, unsigned int frameIndex )
{
	Frapper::Parameter* slotInParameter = getParameter(slotName);

	if ( !slotInParameter) {
		Frapper::Log::error( QString("Input Slot Parameter %1 not found!").arg(slotName), QString("MaskEditNode::imageFromList( slotName: %1)").arg(slotName));
	} else if ( image.isNull() ) {
		Frapper::Log::error( "Image is null!", QString("MaskEditNode::imageFromList( slotName: %1)").arg(slotName));
	} else try {

		Slot* slotIn = slotInParameter->getValue(true).value<Slot*>();
		int startFrame = m_timeParameter->getMinValue().toInt();

		if( slotInParameter->isConnected() && !m_charonInterface->executed() ){
			m_charonInterface->processWorkflow();
		}

		switch (m_selectedType) 
		{
			case 0:
			default : 
				CImgListTools::imageFromSlot<double>(image, slotIn, frameIndex-startFrame); //use startFrame as offset to access charon image list
				break;
			case 1: 
				CImgListTools::imageFromSlot<float>(image, slotIn, frameIndex-startFrame); //use startFrame as offset to access charon image list
				break;
			case 2: 
				CImgListTools::imageFromSlot<int>(image, slotIn, frameIndex-startFrame); //use startFrame as offset to access charon image list
				break;
		}
	} 
	catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), QString("MaskEditNode::imageFromList( slotName: %1)").arg(slotName));
		m_defaultImage->copyToTexture( image);
	}
}


void MaskEditNode::imageToMasksList( Ogre::TexturePtr& image )
{
	Frapper::Parameter* imagesInParameter = getParameter("Images In");
	Frapper::Parameter* masksOutParameter = getParameter("Masks Out");
	
	if ( !imagesInParameter || !masksOutParameter) {
		Frapper::Log::error( QString("Slot(s) not found!"), "MaskEditNode::imageToMasksList" );
	} else if ( image.isNull() ) {
		Frapper::Log::error( "Image is null!", "MaskEditNode::imageToMasksList" );
	} else try {

		// trigger update of image lists
		imagesInParameter->getValue(true);

		Slot* imagesIn = m_charonInterface->getSlot("Images In");
		Slot* masksOut = m_charonInterface->getSlot("Masks Out");

		int startFrame = m_timeParameter->getMinValue().toInt();

		switch (m_selectedType) {
			default : 
			case 0:
				CImgListTools::prepareImageList<double>(imagesIn, masksOut);
				CImgListTools::imageToSlot<double>( image, masksOut, m_currentFrame-startFrame );
				break;
			case 1: 
				CImgListTools::prepareImageList<float>(imagesIn, masksOut);
				CImgListTools::imageToSlot<float>( image, masksOut, m_currentFrame-startFrame );
				break;
			case 2: 
				CImgListTools::prepareImageList<int>(imagesIn, masksOut);
				CImgListTools::imageToSlot<int>( image, masksOut, m_currentFrame-startFrame );
				break;
		}
	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), "MaskEditNode::imageToMasksList");
		m_defaultImage->copyToTexture( image);
	}

}

unsigned int MaskEditNode::mapFrameToImage()
{
	// m_currentFrame should be in range [0..images.size()]
	// therefore we need a remapping here from frapper frames to [0..]
	int startFrame = m_timeParameter->getMinValue().toInt();
	int mappedFrame = m_currentFrame - startFrame;
	
	// make sure to return a positive value
	return mappedFrame < 0 ? 0 : mappedFrame;
}

//!
//! Changes the template type of this node.
//!
void MaskEditNode::changeTemplateType ()
{
	m_charonInterface->setPO(NULL);

	Frapper::EnumerationParameter *enumParameter = getEnumerationParameter("Template Type");
	assert( enumParameter);

	const QString typeString = enumParameter->getCurrentLiteral();
	m_selectedType = enumParameter->getCurrentIndex();

	// reset slot parameter type
	Frapper::GenericParameter* imagesInParameter = getGenericParameter("Images In");
	Frapper::GenericParameter* masksOutParameter = getGenericParameter("Masks Out");
	assert( imagesInParameter && masksOutParameter );
	imagesInParameter->setTypeInfo("NO TYPE");
	masksOutParameter->setTypeInfo("NO TYPE");

	// recreate parametered object with new template type
	const std::string objectName = "MaskEdit";
	ParameteredObject* po = 0;
	switch (m_selectedType) {
		case 0: 
		default :
			po = new MaskEditNodeParameteredObject<double>(objectName);
			break;
		case 1: 
			po = new MaskEditNodeParameteredObject<float>(objectName);
			break;
		case 2: 
			po = new MaskEditNodeParameteredObject<int>(objectName);
			break;
	}

	try {

		m_charonInterface->setPO(po);

		Slot *imagesInSlot = po->getSlot("Images In");
		Slot *masksOutSlot = po->getSlot("Masks Out");

		assert( imagesInSlot && masksOutSlot );

		imagesInSlot->prepare();
		masksOutSlot->prepare();

		// update slot parameter with new slot and type info
		imagesInParameter->setValue(QVariant::fromValue<Slot *>(imagesInSlot), false);
		masksOutParameter->setValue(QVariant::fromValue<Slot *>(masksOutSlot), false);

		imagesInParameter->setTypeInfo("CImgList<" + typeString + ">");
		masksOutParameter->setTypeInfo("CImgList<" + typeString + ">");

	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), QString("MaskEditNode::changeTemplateType"));
	}

	emit nodeChanged();
	forcePanelUpdate();
}

void MaskEditNode::initTextures()
{

	Ogre::Image defaultTextureImage;
	defaultTextureImage.load("DefaultRenderImage.png", "General");

	// create default texture
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();

	if( m_defaultImage.isNull() )
	{
		m_defaultImage = textureManager.createManual(
			createUniqueName("MaskEdit_DefaultImage"), 
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			Ogre::TEX_TYPE_2D,
			1024, 768, 0, 
			Ogre::PF_X8R8G8B8,
			Ogre::TU_DEFAULT );	
		m_defaultImage->loadImage(defaultTextureImage);
	}

	if( m_inputImage.isNull() ) {
		m_inputImage = textureManager.createManual(
			createUniqueName("MaskEdit_InputImage"), 
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			Ogre::TEX_TYPE_2D,
			1024, 768, 0, 
			Ogre::PF_X8R8G8B8,
			Ogre::TU_DEFAULT );	
		m_inputImage->loadImage(defaultTextureImage);
	}

	if( m_outputMask.isNull() ) {
		m_outputMask = textureManager.createManual(
			createUniqueName("MaskEdit_MaskImage"), 
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			Ogre::TEX_TYPE_2D,
			1024, 768, 0, 
			Ogre::PF_X8R8G8B8,
			Ogre::TU_DEFAULT );	
		m_outputMask->loadImage(defaultTextureImage);
	}

	// set initial parameter value
	setValue( m_outputImageName, m_outputMask );
}

void MaskEditNode::updateOnFrameChanged()
{
	setTimeparameterAffection( getBoolValue("Update on frame change"));
}


void MaskEditNode::useLUTChanged()
{
	setUseLUT( getBoolValue("Use LUT"));
}

void MaskEditNode::darkenBackgroundChanged()
{
	setBGDarkenFactor( getFloatValue("Background Darken Factor"));
}

void MaskEditNode::updateEdgeLayer()
{
	m_inputEdge = getTextureValue("Edge In", true);

	if( !m_inputEdge.isNull() ) {
		QColor layerColor = getColorValue( "Edge Layer > Layer Color" );
		setLayer( 0, m_inputEdge, layerColor );
	} else {
		setLayer( 0, Ogre::TexturePtr(), Qt::black );
	}
}

//!
//! Connects Frapper in/out parameter with Charon in/out slots.
//!

void MaskEditNode::connectSlots()
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	m_charonInterface->connectSlots(inParameter);
}

//!
//! Disonnects Frapper in/out parameter with Charon in/out slots.
//!
void MaskEditNode::disconnectSlots( int connectionID )
{
	Frapper::GenericParameter *inParameter = dynamic_cast<Frapper::GenericParameter *>(sender());
	m_charonInterface->disconnectSlots(inParameter,connectionID);
}

size_t MaskEditNode::getImageListSize( QString slotName )
{
	try {
	
		Slot* imageListSlot = m_charonInterface->getSlot( slotName.toStdString() );
		switch (m_selectedType) {
			case 0: 
			default :
				return CImgListTools::getListSize<double>(imageListSlot);
				break;
			case 1: 
				return CImgListTools::getListSize<float>(imageListSlot);
				break;
			case 2: 
				return CImgListTools::getListSize<int>(imageListSlot);
				break;
		}

	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), "MaskEditNode::getImageListSize");
	}	

	return 0;
}

void MaskEditNode::updateOutput()
{
	m_currentFrame = -1;
	processOutputImage();
}

void MaskEditNode::clearSequence()
{
	try {
		Slot* imageListSlot = m_charonInterface->getSlot( "Masks Out" );
		switch (m_selectedType) {
			case 0: 
			default :
				CImgListTools::clearImageList<double>(imageListSlot);
				break;
			case 1: 
				CImgListTools::clearImageList<float>(imageListSlot);
				break;
			case 2: 
				CImgListTools::clearImageList<int>(imageListSlot);
				break;
		}
	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), "MaskEditNode::clearSequence");
	}

	m_sceneChanged = true;
}

void MaskEditNode::clearFrame()
{
	try {
		Slot* imageListSlot = m_charonInterface->getSlot( "Masks Out" );
		switch (m_selectedType) {
		case 0: 
		default :
			CImgListTools::clearImage<double>(imageListSlot, m_currentFrame-1);
			break;
		case 1: 
			CImgListTools::clearImage<float>(imageListSlot, m_currentFrame-1);
			break;
		case 2: 
			CImgListTools::clearImage<int>(imageListSlot, m_currentFrame-1);
			break;
		}
	} catch (std::exception& e) {
		Frapper::Log::error( QString(e.what()), "MaskEditNode::clearSequence");
	}
}

void MaskEditNode::setUpTimeDependencies( Frapper::NumberParameter *timeParameter, Frapper::NumberParameter *rangeParameter )
{
	Node::setUpTimeDependencies( timeParameter, rangeParameter );
	updateOnFrameChanged();
}

void MaskEditNode::setTimeparameterAffection( bool enabled )
{
	if( enabled)
	{
		// set affection of image to time parameter to handle update on frame change
		m_timeParameter->addAffectedParameter( m_imageParameter );
		m_timeParameter->addAffectedParameter( getParameter("Masks Out"));		

	}
	else
	{
		m_timeParameter->removeAffectedParameter( m_imageParameter );
		m_timeParameter->removeAffectedParameter( getParameter("Masks Out"));		
	}
}

void MaskEditNode::maskInParameterConnected()
{
	Frapper::Parameter* imagesInParameter = getParameter("Images In");
	Frapper::Parameter* masksOutParameter = getParameter("Masks Out");

	if ( imagesInParameter && masksOutParameter) try 
	{
		Slot* imagesIn = m_charonInterface->getSlot("Images In");
		Slot* masksOut = m_charonInterface->getSlot("Masks Out");

		switch (m_selectedType) {
		default : 
		case 0:
			CImgListTools::prepareImageList<double>(imagesIn, masksOut);
			break;
		case 1: 
			CImgListTools::prepareImageList<float>(imagesIn, masksOut);
			break;
		case 2: 
			CImgListTools::prepareImageList<int>(imagesIn, masksOut);
			break;
		}
	} catch (std::exception& e) {}

	//updateOutput();
}

void MaskEditNode::deleteConnection( Frapper::Connection *connection )
{
	Frapper::Node::deleteConnection(connection);
}


} // end namespace
