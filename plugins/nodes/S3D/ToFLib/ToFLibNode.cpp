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
//! \file "ToFLibNode.cpp"
//! \brief Camera Input
//!
//! \author		Michael Bussler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version	1.0
//! \date		26.03.2014 (last updated)
//!

#include <oxofyuzit.hxx>
#include "ToFLibNode.h"

#define WITH_OCV 1

#if WITH_OCV
#include <cv.h>
#include <highgui.h>
#endif
#include "common\options.hxx"

namespace ToFLibNode {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the ToFLibNode class.
//!
//! \param name The name to give the new mesh node.
//!
ToFLibNode::ToFLibNode ( QString name, ParameterGroup *parameterRoot) :
	ImageNode(name, parameterRoot),
	m_tofRenderer(NULL)
{
	
	addAffection("Input Map Left",  m_outputImageName);
	addAffection("Input Map Right", m_outputImageName);
	addAffection("Input Map ToF",   m_outputImageName);

	setProcessingFunction(m_outputImageName, SLOT(processOutputImage()));

	setCommandFunction("ToF Sensor Fusion > Render!", SLOT(processOutputImage()));

	initOptions();
}

//!
//! Destructor of the ToFLibNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ToFLibNode::~ToFLibNode ()
{
	//delete m_tofRenderer;
}

///
/// Private Slots
///


void ToFLibNode::processOutputImage()
{
	Ogre::TexturePtr inputMapL = getTextureValue("Input Map Left", true);
	Ogre::TexturePtr inputMapR = getTextureValue("Input Map Right", true);
	Ogre::TexturePtr inputMapT = getTextureValue("Input Map ToF", true);

	Frapper::Log::debug("Validating Input Images...", "ToFLibNode::processOutputImage");

	if( inputMapL.isNull() || inputMapT.isNull() ) {
		setValue(m_outputImageName, Ogre::TexturePtr());
		Frapper::Log::error("Invalid Input Image(s)", "ToFLibNode::processOutputImage");
		return;
	}

	bool useRight = true;
	if( inputMapR.isNull() ) {
		Frapper::Log::debug("Using left input image only", "ToFLibNode::processOutputImage");
		useRight = false;
	}

	Frapper::Log::debug("Loading Data...", "ToFLibNode::processOutputImage");

	Ogre::Image inputL;
	Ogre::Image inputR;
	Ogre::Image inputT;

	createImageFromTexture(inputMapL, inputL, Ogre::PF_R8G8B8 ); // conversion required -> slow!!
	createImageFromTexture(inputMapT, inputT, Ogre::PF_FLOAT32_RGB ); // But toflib can't handle 4 chan data

	if( useRight )	{
		createImageFromTexture(inputMapR, inputR, Ogre::PF_R8G8B8 ); // you want PF_A8R8G8B8 or PF_X8R8G8B8
	}

	// store current (Ogre) context to restore later on
	HGLRC ogreOglContext = wglGetCurrentContext();
	HDC   ogreDC = wglGetCurrentDC();

	// ref to data from algorithm
	int width = 0, height = 0;
	vigra::MultiArray<3, float> output;
	
	// create pipe from std::cout to frapper log
	StreamBuffer sb;
	std::streambuf* old_buf = std::cout.rdbuf();

	try{

		Frapper::Log::debug("Initializing Algorithm...", "ToFLibNode::processOutputImage");

		float fov = getFloatValue("Input Image Options > FOV");

		oxo::FusionCalibrationData calib_data;
		calib_data.left.setShape(	3,	inputMapL->getWidth(),	inputMapL->getHeight()).setFOV(fov);
		if( useRight )
			calib_data.right.setShape(	3,	inputMapR->getWidth(),	inputMapR->getHeight()).setFOV(fov);
		else
			calib_data.right.setShape(	3, 0, 0).setFOV(fov);
		calib_data.tof.setShape(	3,	inputMapT->getWidth(),	inputMapT->getHeight()).setFOV(fov);

		float baseline_lt = getFloatValue("Input Image Options > Baseline Left-ToF");
		float baseline_lr = getFloatValue("Input Image Options > Baseline Left-Right");

		calib_data.left_tof.setBaseline( baseline_lt );
		calib_data.left_right.setBaseline( baseline_lr );

		int algoIndex = getEnumerationParameter("ToF Sensor Fusion > Algorithm")->getCurrentIndex();
		QString algoName = getEnumerationParameter("ToF Sensor Fusion > Algorithm")->getCurrentLiteral();

		std::cout.rdbuf(&sb);

		// select algorithm
		oxo::FusionAlgorithm<>* algorithm = 0;
		if( algoIndex == 0) { // Just Reproject

			oxo::JustReproject<>::OptionsT opts;
			setOptionsFromParameter("Just Reproject Options", &(opts.base) );
			std::stringstream sout; 
			sout << opts.depth_cutoff();
			Frapper::Log::info(QString::fromStdString(sout.str()));
			opts.set_init_in_constructor(true);
			algorithm = new oxo::JustReproject<>( calib_data, opts);
		
		} else if( algoIndex == 1) { // Block Matching

			if( !useRight ){
				throw std::exception( QString("Block Matching Algorithm requires Stereo Images!").toStdString().c_str() );
			}

			oxo::BlockMatchingFusion<>::OptionsT opts;
			setOptionsFromParameter("Block Matching Options", &(opts.base) );
			algorithm = new oxo::BlockMatchingFusion<>( calib_data, opts);

		} else if( algoIndex == 2) { // Full Model

			if( !useRight ){
				throw std::exception( QString("Full Model Algorithm requires Stereo Images!").toStdString().c_str() );
			}

			oxo::FullModel<>::OptionsT opts;
			setOptionsFromParameter("Full Model Options", &(opts.base));
			algorithm = new oxo::FullModel<>( calib_data, opts);
			//throw std::exception( QString("Full Model Algorithm not implemented!").toStdString().c_str() );

		} else if( algoIndex == 3) { // TV Fusion

				if( !useRight ){
					throw std::exception( QString("TV Fusion Algorithm requires Stereo Images!").toStdString().c_str() );
				}
				oxo::TVFusion<>::OptionsT opts;
				setOptionsFromParameter("TV Fusion Options", &(opts.base));
				algorithm = new oxo::TVFusion<>( calib_data, opts);
		}

		width  = algorithm->getOutputShape()[1];
		height = algorithm->getOutputShape()[2];
		output = vigra::MultiArray<3, float>( vigra::TinyVectorView<int, 3>(algorithm->getOutputShape()));

		const unsigned char* leftdata  = (const unsigned char*) inputL.getPixelBox().data;
		const unsigned char* rightdata = useRight ? (const unsigned char*) inputR.getPixelBox().data : 0;
		float* tofdata = (float*) inputT.getPixelBox().data;

		// fire computation
		Frapper::Log::debug("Executing "+algoName+"...", "ToFLibNode::processOutputImage");
		algorithm->compute(leftdata, rightdata, tofdata, output.data());

		delete algorithm;

	} catch (std::exception e) {

		std::cout.rdbuf(old_buf);

		Frapper::Log::error( e.what(), "ToFLibNode::processOutputImage" );

		// restore Ogre context
		wglMakeCurrent( ogreDC, ogreOglContext);

		setOutputImage(Ogre::TexturePtr());
		emit viewNodeUpdated();

		return;
	}

	std::cout.rdbuf(old_buf);


	// restore Ogre context
	wglMakeCurrent( ogreDC, ogreOglContext);

	// create output image texture
	Ogre::TexturePtr outputTexture = getTextureValue(m_outputImageName);
	if (!outputTexture.isNull()) {
		Ogre::TextureManager::getSingletonPtr()->remove(outputTexture->getHandle());
		outputTexture.setNull();
	}

	Ogre::String textureName = QString("%1OutputTexture").arg(m_name).toStdString();
	outputTexture = 
		Ogre::TextureManager::getSingletonPtr()->createManual( textureName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		Ogre::TEX_TYPE_2D, width, height, 1, Ogre::PF_FLOAT32_R, Ogre::TU_DEFAULT);

	OgreTools::HardwareBufferLocker hbl( outputTexture->getBuffer(0,0));
	
	// mem size in bytes
	size_t memorySize = Ogre::PixelUtil::getMemorySize( width, height, 1, Ogre::PF_FLOAT32_R );

	const Ogre::PixelBox pb = hbl.getCurrentLock();
	
	// copy result
	vigra::MultiArray<2, float> result = output.bindInner(0);
	float* src = result.data();
	float* dst = ((float*)pb.data);
	
	size_t memSizeFloat = memorySize / sizeof(float);
	for( int i=0; i<memSizeFloat; i++) 
	{
		dst[i] = (src[i] / 7.0f); // normalize, should not be done here, uses memcpy instead!
	}
	//memcpy( dst, src, memorySize);

	setOutputImage(outputTexture);

	emit viewNodeUpdated();
}

void ToFLibNode::initOptions()
{
	ParameterGroup* algoOptionsGroup = 0;

	// get algorithm options
	oxo::OptionsBase* options = 0;

	// Just Reproject
	options = &(new oxo::JustReproject<>::OptionsT())->base;
	algoOptionsGroup = getOrCreateParameterGroup("Just Reproject Options");
	parametersFromOption(options, algoOptionsGroup);

	// Block Matching
	options = &(new oxo::BlockMatchingFusion<>::OptionsT())->base;
	algoOptionsGroup = getOrCreateParameterGroup("Block Matching Options");
	parametersFromOption(options, algoOptionsGroup);

	// Full Model
	options = &(new oxo::FullModel<>::OptionsT())->base;
	algoOptionsGroup = getOrCreateParameterGroup("Full Model Options");
	parametersFromOption(options, algoOptionsGroup);
	
	// TV Fusion
	options = &(new oxo::TVFusion<>::OptionsT())->base;
	algoOptionsGroup = getOrCreateParameterGroup("TV Fusion Options");
	parametersFromOption(options, algoOptionsGroup);

	forcePanelUpdate();
}

void ToFLibNode::setOptionsFromParameter( QString optionsGroupName, oxo::OptionsBase* optionsPtr )
{
	ParameterGroup* optionsGroup = getParameterGroup("ToF Sensor Fusion")->getParameterGroup(optionsGroupName);
	if( optionsGroup ){
		foreach ( AbstractParameter* optionParameterBase, optionsGroup->getParameterList()) {
			Parameter* optionParameter = dynamic_cast<Parameter*>(optionParameterBase);
			if( optionParameter ) {
				QString parameterName = optionParameter->getName();
				Parameter::Type parameterType = optionParameter->getType();
				QVariant parameterValue = optionParameter->getValue();
				std::string internalT = optionsPtr->parameters[optionsPtr->name2idx[parameterName.toStdString()]].type;
				if( parameterType == Parameter::T_Int ){
					std::stringstream sout;
					sout << "INT param : " << parameterName.toStdString() << " "  << parameterValue.toInt();
					Frapper::Log::info(QString::fromStdString(sout.str()));
					optionsPtr->set<int>( parameterName.toStdString(), parameterValue.toInt());

				} else if( parameterType == Parameter::T_Float && internalT ==  oxo::Parameter::convertName("float")){
					std::stringstream sout;
					sout << "Float param : " << parameterName.toStdString() << " "  << parameterValue.toFloat();
					Frapper::Log::info(QString::fromStdString(sout.str()));
					optionsPtr->set<float>( parameterName.toStdString(), parameterValue.toFloat());
				} else if( parameterType == Parameter::T_Float && internalT == oxo::Parameter::convertName("double")){
					std::stringstream sout;
					sout << "Double param : " << parameterName.toStdString() << " "  << parameterValue.toFloat();
					Frapper::Log::info(QString::fromStdString(sout.str()));
					optionsPtr->set<double>( parameterName.toStdString(), parameterValue.toDouble());
				} else if( parameterType == Parameter::T_Bool ){
					optionsPtr->set<bool>( parameterName.toStdString(), parameterValue.toBool());

				} else if ( parameterType == Parameter::T_String ) {
					optionsPtr->set<std::string>( parameterName.toStdString(), parameterValue.toString().toStdString());

				// to be continued...
				//} else if ( parameterType == Parameter::T_... ){
				//	optionsPtr->set<...>( parameterName.toStdString(), parameterValue);

				} else {
					Frapper::Log::warning("Option "+parameterName+" has an unknown type!", "ToFLibNode::setOptionsFromParameter");
				}
			}
		}
	}
}

void ToFLibNode::parametersFromOption( oxo::OptionsBase* options, ParameterGroup* algoOptionsGroup )
{
	if( options ){

		oxo::OptionsBase::iterator iter;
		for( iter = options->begin(); iter != options->end(); ++iter)
		{
			QString optionName = QString::fromStdString(iter->name);
			QString optionType = QString::fromStdString(iter->type);
			QString defaultValueStr = QString::fromStdString(iter->default_value);

			Frapper::Log::debug( "Name: "+optionName+", Type: "+optionType+", Default Value: "+defaultValueStr, "ToFLibNode::algorithmChanged");

			Parameter* optionParameter = 0;

			if( iter->type == oxo::Parameter::convertName("int")) {
				optionParameter = new NumberParameter( optionName, Parameter::T_Int, QVariant::fromValue<int>(defaultValueStr.toInt()));
				static_cast<NumberParameter *>(optionParameter)->setMinValue( INT_MIN);
				static_cast<NumberParameter *>(optionParameter)->setMaxValue( INT_MAX);

			} else if( iter->type == oxo::Parameter::convertName("float")) {
				optionParameter = new NumberParameter( optionName, Parameter::T_Float, QVariant::fromValue<float>(defaultValueStr.toFloat()));
				static_cast<NumberParameter *>(optionParameter)->setMinValue( FLT_MIN);
				static_cast<NumberParameter *>(optionParameter)->setMaxValue( FLT_MAX);

			} else if( iter->type == oxo::Parameter::convertName("double")) {
				optionParameter = new NumberParameter( optionName, Parameter::T_Float, QVariant::fromValue<double>(defaultValueStr.toFloat()));
				static_cast<NumberParameter *>(optionParameter)->setMinValue( DBL_MIN);
				static_cast<NumberParameter *>(optionParameter)->setMaxValue( DBL_MAX);

			} else if( iter->type == oxo::Parameter::convertName("bool")) {
				bool val = defaultValueStr.compare("1") || defaultValueStr.compare("true", Qt::CaseInsensitive);
				optionParameter = new Parameter( optionName, Parameter::T_Bool, QVariant::fromValue<bool>(val) );

			} else if( iter->type == oxo::Parameter::convertName("string")) {
				optionParameter = new Parameter( optionName, Parameter::T_String, QVariant::fromValue<QString>(defaultValueStr) );

				// to be continued...
				//} else if( iter->type == oxo::Parameter::convertName("...")) {
				//	optionParameter = new Parameter( optionName, Parameter::T_...);

			} else {
				Frapper::Log::warning("Unknown type of option "+optionName+": "+optionType);
			}

			if( optionParameter){

				QString parameterDescription = QString::fromStdString(iter->description);
				if( !parameterDescription.isEmpty() ) {
					optionParameter->setDescription( parameterDescription);
				}
				algoOptionsGroup->addParameter(optionParameter);
			}
		}
	}
}

Frapper::ParameterGroup* ToFLibNode::getOrCreateParameterGroup( QString name )
{
	ParameterGroup* tofGroup = getParameterGroup("ToF Sensor Fusion");
	ParameterGroup* algoOptionsGroup = tofGroup->getParameterGroup(name);

	if( !algoOptionsGroup ) {
		algoOptionsGroup = new Frapper::ParameterGroup(name);
		tofGroup->addParameter( algoOptionsGroup, true);
	}

	return algoOptionsGroup;
}



} // namespace ToFLibNode
