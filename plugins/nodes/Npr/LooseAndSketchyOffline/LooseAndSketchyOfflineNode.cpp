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
//! \file "LooseAndSketchyOfflineNode.cpp"
//! \brief Implementation file for LooseAndSketchyOfflineNode class.
//!
//! \author     Thomas Luft <thomas.luft@web.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!


#include "LooseAndSketchyOfflineNode.h"
#include "ImageNode.h"
#include "Parameter.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include <random>

namespace LooseAndSketchyOfflineNode {
	using namespace Frapper;

///
/// Constructors and Destructors
///

//!
//! Constructor of the LooseAndSketchyOfflineNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//! \param outputImageName The name of the geometry output parameter.
//!
LooseAndSketchyOfflineNode::LooseAndSketchyOfflineNode ( const QString &name, ParameterGroup *parameterRoot ) :
    RenderNode(name, parameterRoot),
    m_gradientMap(0)
{
    // Create a material using the texture
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("LooseAndSketchyOfflineMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    material->setAmbient(0.0, 0.0, 0.0);

    Parameter *outputParameter = getParameter(m_outputImageName);
    if (outputParameter) {
        // set up parameter affections
        outputParameter->addAffectingParameter(getParameter("Input Map"));

        // set the processing function for the output image parameter
        outputParameter->setProcessingFunction(SLOT(processOutputImage()));
    } else
        Log::error("Could not obtain output image parameter.", "LooseAndSketchyOfflineNode::LooseAndSketchyOfflineNode");

}

//!
//! Destructor of the LooseAndSketchyOfflineNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
LooseAndSketchyOfflineNode::~LooseAndSketchyOfflineNode ()
{
}


///
/// Private Slots
///

//!
//! Processes the node's input data to generate the node's output image.
//!
void LooseAndSketchyOfflineNode::processOutputImage ()
{
    // obtain input image
    Ogre::TexturePtr inputTexture = getTextureValue("Input Map");
    if (inputTexture.isNull() || inputTexture->getFormat() != Ogre::PF_FLOAT32_RGBA)
    {
        Log::warning("No input image connected.", "LooseAndSketchyOfflineNode::processOutputImage");
        return;
    }

    if (!m_renderTexture.isNull()) {
        size_t width = inputTexture->getWidth();
        size_t height = inputTexture->getHeight();
		resizeRenderTexture(width, height, Ogre::PF_A8R8G8B8);
		m_viewport->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
    }

    progressLooseAndSketchyOffline();
}

///
/// Private Methods
///

//!
//! Generate one loose and sketchy step.
//!
void LooseAndSketchyOfflineNode::progressLooseAndSketchyOffline()
{
    OgreTools::deepDeleteSceneNode(m_sceneManager->getRootSceneNode(), m_sceneManager);
    Ogre::TexturePtr inputTexture = getTextureValue("Input Map");
    if (inputTexture.isNull() || inputTexture->getFormat() != Ogre::PF_FLOAT32_RGBA)
    {
        Log::warning("No input image connected.", "LooseAndSketchyOfflineNode::progressLooseAndSketchyOffline");
        return;
    }
    //lock input map as gradient map buffer
    Ogre::HardwarePixelBufferSharedPtr inputPixelBuffer = inputTexture->getBuffer();
    const Ogre::PixelBox &inputPixelBox = inputPixelBuffer->lock(Ogre::Image::Box(0, 0, m_renderWidth, m_renderHeight), Ogre::HardwareBuffer::HBL_READ_ONLY);
    m_gradientMap = static_cast<float *>(inputPixelBox.data);

    //compute strokes
    computeLooseAndSketchyOffline();
    //Log::info(QString::number((unsigned int) m_strokes.size()) + "# strokes");

    //unlock input map
    inputPixelBuffer->unlock();

    //render strokes
    renderLooseAndSketchyOffline();

    //render and set output
    m_renderTexture->getBuffer()->getRenderTarget()->update();
    setValue("Image", m_renderTexture); 
}

//!
//! Compute one loose and sketchy step.
//!
void LooseAndSketchyOfflineNode::computeLooseAndSketchyOffline()
{
    //settings that cannot be adjusted so far
    double minStrokeLength            = 0.05;
    double minGradient                = 0.2;
    double strokeContourCurvature    = 0.85;                    //curvature measure
    //double strokeContourCurvature    = 0.1;
    double trackingStep                = 2.0;                    //tracking step in pixel
    int lineLength                    = 40;                    //line length in iterations (times pixel, times 2)
    unsigned int lineCount            = 2000;                    //line count
    unsigned int particleCount        = 10000; 

    particleCount = getUnsignedIntValue("Particle Count");
    lineCount = getUnsignedIntValue("Line Count");
    lineLength = getUnsignedIntValue("Line Length");
    minStrokeLength = getDoubleValue("Min Stroke Length");
    minGradient = getDoubleValue("Min Gradient");
    strokeContourCurvature = getDoubleValue("Stroke Contour Curvature");
    trackingStep = getDoubleValue("Tracking Step");
    
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0.f, 1.f);

	float minGradientSq = minGradient * minGradient;
	
    //delete strokes
    m_strokes.erase(m_strokes.begin(), m_strokes.end());

    for (int i = 0; i < particleCount; ++i)
    {
        //create new stroke
        Stroke lineStroke;

        //create particle
        Vector2d particlePos = Vector2d(distribution(generator), distribution(generator));
        Vector2d particlePos0 = particlePos;

        //gradient field lookup
        Vector2d gradient = getGradientMap(particlePos);
        //float intensity = getGradientMapIntensity(particlePos);
        //Vector2d prevGradient;

        //stroke direction (within gradient map space)
        Vector2d strokeDir = Vector2d(-gradient.y / (double)m_renderWidth, gradient.x / (double)m_renderHeight);

        //begin tracking
		if (gradient.lengthSq() > minGradientSq)
        //if (true)
        {
            //store first particle
            lineStroke.vertices.push_back( Vector2d(particlePos.x * 2.0 - 1.0, particlePos.y * 2.0 - 1.0, 1.0) );
            lineStroke.e0 = lineStroke.vertices.front();

            //forward tracking
            for (int t = 0; t < lineLength; ++t)
            {
                //update particle position
				strokeDir = strokeContourCurvature * strokeDir + (1.0 - strokeContourCurvature) * Vector2d(-gradient.y / (double)m_renderWidth, gradient.x / (double)m_renderHeight);
                particlePos += strokeDir * trackingStep;

                //store particle
                lineStroke.vertices.push_back( Vector2d(particlePos.x * 2.0 - 1.0, particlePos.y * 2.0 - 1.0, 1.0) );

                //lookup gradient
                //prevGradient = gradient;
                gradient = getGradientMap(particlePos);
                //intensity = getGradientMapIntensity(particlePos);


                //realign gradient if inverse
                //if (Vector2d::dotProduct(prevGradient, gradient) < 0.0)
                //    gradient = -gradient;

                //end if gradient is approx. zero
				if (gradient.lengthSq() < minGradientSq * 0.5)
                    break;
            }

            //restore initial particle
            particlePos = particlePos0;
            gradient = -getGradientMap(particlePos);
            //intensity = getGradientMapIntensity(particlePos);
            strokeDir = -Vector2d(-gradient.y / (double)m_renderWidth, gradient.x / (double)m_renderHeight);

            //backward tracking
            for (int t = 0; t < lineLength; ++t)
            {
                //update particle position
                strokeDir = strokeContourCurvature * strokeDir + (1.0 - strokeContourCurvature) * Vector2d(-gradient.y / (double)m_renderWidth, gradient.x / (double)m_renderHeight);
                particlePos += strokeDir * trackingStep;

                //store particle
                lineStroke.vertices.insert(lineStroke.vertices.begin(), Vector2d(particlePos.x * 2.0 - 1.0, particlePos.y * 2.0 - 1.0, 1.0) );

                //lookup gradient
                //prevGradient = gradient;
                gradient = -getGradientMap(particlePos);
                //intensity = getGradientMapIntensity(particlePos);

                //realign gradient if inverse
                //if (Vector2d::dotProduct(prevGradient, gradient) < 0.0)
                //gradient = -gradient;

                //end if gradient is approx. zero
				if (gradient.lengthSq() < minGradientSq * 0.5)
                    break;
            }

            //add line stroke
            lineStroke.computeLength();

            //keep min length and restrict strokes to mid area (say a circle)
            //if ((lineStroke.length >= minStrokeLength) && (lineStroke.e0.length() < 0.95))
            if (lineStroke.length >= minStrokeLength)
                addStroke(lineStroke);

        } //end tracking

        if (m_strokes.size() > lineCount) break;
    }


    //sort by length
    //std::sort(m_strokes.begin(), m_strokes.end(), sortByLength);
}

//
// Add a new stroke.
//
// \param stroke The stroke which should be added.
//
void LooseAndSketchyOfflineNode::addStroke(Stroke& stroke)
{
	////settings that cannot be adjusted so far
	double strokeMinDistanceSq = 0.005 * 0.005;

	//keep certain density
	for (std::vector<Stroke>::iterator s = m_strokes.begin(); s != m_strokes.end(); ++s)
	{
		for (UINT i = 0; i < s->vertices.size(); i += 1)
		{
			if (Vector2d::getLengthSq(stroke.e0 - s->vertices[i]) < strokeMinDistanceSq)
			{
				return;
			}
		}
	}

    double lineAbstractionSq = 0.000025;

    //simplify stroke
    Stroke abstractStroke;
    abstractStroke.vertices.reserve(stroke.vertices.size());


    //abstraction process
    abstractStroke.vertices.push_back(stroke.vertices.front());

    for (std::vector<Vector2d>::const_iterator v = stroke.vertices.begin() + 1; v != stroke.vertices.end() - 1; ++v)
    {
		if (Vector2d::getLengthSq(*v - abstractStroke.vertices.back()) > lineAbstractionSq)
		{
			abstractStroke.vertices.push_back(*v);
		}
    }

    abstractStroke.vertices.push_back(stroke.vertices.back());

    if (stroke.vertices.size() < 4) return;

    abstractStroke.e0 = stroke.e0;

    //fade in and out
    abstractStroke.vertices.front().w = 0.0;
    abstractStroke.vertices.back().w = 0.0;

    //length
    //abstractStroke.computeLength();


    //add stroke
    m_strokes.push_back(abstractStroke);
}


//!
//! Render the stroke geometry.
//!
void LooseAndSketchyOfflineNode::renderLooseAndSketchyOffline()
{
    Ogre::SceneNode *sceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();

	Ogre::ManualObject *strokeObject = m_sceneManager->createManualObject(std::string("strokes"));
	strokeObject->setUseIdentityProjection(true);
	strokeObject->setUseIdentityView(true);
    for (std::vector<Stroke>::const_iterator stroke = m_strokes.begin(); stroke != m_strokes.end(); ++stroke)
    {
        strokeObject->begin("LooseAndSketchyOfflineMaterial", Ogre::RenderOperation::OT_LINE_STRIP);

        for (std::vector<Vector2d>::const_iterator v = stroke->vertices.begin(); v != stroke->vertices.end(); ++v)
        {
            strokeObject->position(v->x, -v->y, 0.0);
        }

        strokeObject->end();
    }

	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	strokeObject->setBoundingBox(aabInf);

	// Render just before overlays
	strokeObject->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
	sceneNode->attachObject(strokeObject);
}

//!
//! Redraw of ogre scene has been triggered.
//!
void LooseAndSketchyOfflineNode::redrawTriggered ()
{
    progressLooseAndSketchyOffline();
}

//!
//! Private Helper Methods
//!

//!
//! Converts 2D texture position in 1D array index.
//!
//! \param Position which should be converted.
//! \return Index in gradient map array.
//!
unsigned int LooseAndSketchyOfflineNode::getGradientMapIndex(const Vector2d& pos)
{
    int w = (int)(pos.x * m_renderWidth);
    int h = (int)(pos.y * m_renderHeight);

    w = std::max<int>(0, std::min<int>(m_renderWidth-1, w));
    h = std::max<int>(0, std::min<int>(m_renderHeight-1, h));

    return h * m_renderWidth + w;
}

//!
//! Returns value of gradient map at position pos.
//!
//! \param Position in gradient map.
//! \return Value of gradient map at position pos.
//!
Vector2d LooseAndSketchyOfflineNode::getGradientMap(const Vector2d& pos)
{
    unsigned int tmpLookupIndex = getGradientMapIndex(pos);

    float gradientX = (float) m_gradientMap[tmpLookupIndex * 4 + 0];
    float gradientY = (float) m_gradientMap[tmpLookupIndex * 4 + 1];
    Vector2d gradient(gradientX, gradientY);
    return gradient;
}


//float getGradientMapIntensity(const Vector2d& pos)
//{
//    unsigned int tmpLookupIndex = getGradientMapIndex(pos);
//
//    return m_gradientMap[tmpLookupIndex * 4 + 2] / 255.0;
//}

//!
//! Returns a random double value.
//!
//! \return Value of gradient map at position pos.
//!
//double LooseAndSketchyOfflineNode::getRandom()
//{
//    return rand()/(double)RAND_MAX;
//}

//!
//! Returns whether length of stroke s0 is greater than the
//! length of s1.
//!
//! \return True if length(s0) > length(s1).
//!
bool LooseAndSketchyOfflineNode::sortByLength(Stroke s0, Stroke s1)
{
    return s0.length > s1.length;
}

} // end namespace
