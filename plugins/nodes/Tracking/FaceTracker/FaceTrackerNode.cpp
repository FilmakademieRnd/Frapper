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
//! \file "FaceTrackerNode.cpp"
//! \brief Implementation file for FaceTrackerNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author		Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       04.06.2012 (last updated)
//!

#include "FaceTrackerNode.h"
#include <QtCore/QFile>
#include <QtCore/QDir>

namespace FaceTrackerNode {
using namespace Frapper;

const float PROBABILITY_INCREASE_PER_DETECTION = 0.2f;
const float PROBABILITY_DECREASE_PER_TIME_STEP = 0.1f;
const float THRESHOLD = 10.0f;
///
///   MISC FUNCTIONS
///


//!
//! Returns the normalized coordinates of the given CvPoint with respect to the dimensions of the given image.
//!
//! \param image    pointer to an image, whose dimensions should be used in calculating the normalized coordinates
//! \param p        the point to be normalized using the image's coordinates
//! \return The given coordinates positioned in the space of the given image.
//!
PointNDC toPointNDC (Mat *image, CvPoint p)
{
    assert(image);
    PointNDC result;
    result.x = (float) p.x / image->cols;
    result.y = (float) p.y / image->rows;
    return result;
}


//!
//! Returns the int coordinates of the given NDC with respect to the dimensions of the given image.
//!
//! \param image    pointer to an image, whose dimensions should be used in calculating the int coordinates
//! \param x        the normalized x coordinate to convert
//! \param y        the normalized y coordinate to convert
//! \return The given coordinates positioned in the space of the given image.
//!
CvPoint toCvPoint (Mat image, float x, float y)
{
    //assert(image);

    return cvPoint((int) (x * image.cols), (int) (y * image.rows));
}


//!
//! Returns the int coordinates of the given point's NDC with respect to the dimensions of the given image.
//!
//! \param image    pointer to an image, whose dimensions should be used in calculating the int coordinates
//! \param p        the normalized device coordinates of the point to convert
//! \return The coordinates of the given point in the space of the given image.
//!
CvPoint toCvPoint(Mat image, PointNDC p)
{
    return toCvPoint(image, p.x, p.y);
}


//!
//! Returns the int coordinates of the top left corner's NDC of the given rectangle with respect to the dimensions of the given image.
//!
//! \param image    pointer to an image, whose dimensions should be used in calculating the int coordinates
//! \param rect     the rectangle whose top left corner to convert
//! \return The coordinates of the given point in the space of the given image.
//!
CvPoint toCvPoint(Mat image, RectNDC rect)
{
    return toCvPoint(image, rect.x, rect.y);
}

///
///   DEFINES
///


//!
//! The name of the Haar classifier cascade file to use for detecting faces.
//!
#define HAAR_FILENAME "plugins/nodes/haarcascade_frontalface_alt.cascade"


///
/// Constructors and Destructors
///


//!
//! Constructor of the FaceTrackerNode class.
//!
//! \param name The name to give the new mesh node.
//!
FaceTrackerNode::FaceTrackerNode ( QString name, ParameterGroup *parameterRoot ) :
    opencvColorInput(name, parameterRoot)
{
    m_switchUserNow = false;
    m_justSwitched = 0;
	m_drawDebugInformation = false;
	m_lastNumberofAttractos = 0;
	
	Parameter *flipHorizontalParameter = getParameter("flipHorizontal");
	if(flipHorizontalParameter)
	{
		flipHorizontalParameter->setChangeFunction(SLOT(flipPicture()));
		m_flipHorizontal =  getValue("flipHorizontal").toBool();
	}

	Parameter *probabilityParameter = getParameter("probability");
	if(probabilityParameter)
	{
		probabilityParameter->setChangeFunction(SLOT(probability()));
		m_probability = getDoubleValue("probability");
	}
	
	Parameter *switchTimeParameter = getParameter("switchTime");
	if(switchTimeParameter)
	{
		switchTimeParameter->setChangeFunction(SLOT(switchTime()));
		m_switchTime = getValue("switchTime").toInt();
	}

	Parameter *switchTimeRandParameter = getParameter("switchTimeRand");
	if(switchTimeRandParameter)
	{
		switchTimeRandParameter->setChangeFunction(SLOT(switchTimeRand()));
		m_switchTimeRand = getValue("switchTimeRand").toInt();
	}

	Parameter *randomLookParameter = getParameter("randomLook");
	if(randomLookParameter)
	{
		randomLookParameter->setChangeFunction(SLOT(randomLook()));
		m_randomLook = getValue("randomLook").toBool();
	}

	Parameter *focusPointSpeedParameter = getParameter("focusPointSpeed");
	if(focusPointSpeedParameter)
	{
		focusPointSpeedParameter->setChangeFunction(SLOT(focusPointSpeed()));
		m_focusPointSpeed = getDoubleValue("focusPointSpeed");
	}

	Parameter *focusPointMoveLimitParameter = getParameter("focusPointMoveLimit");
	if(focusPointMoveLimitParameter)
	{
		focusPointMoveLimitParameter->setChangeFunction(SLOT(focusPointMoveLimit()));
		m_focusPointMoveLimit = getDoubleValue("focusPointMoveLimit");
	}

	Parameter *switchUserNowParameter = getParameter("switchUserNow");
	if(switchUserNowParameter)
	{
		switchUserNowParameter->setChangeFunction(SLOT(switchUserNow()));
		m_switchUserNow =  getValue("switchUserNow").toBool();
	}

	Parameter *minSizeParameter = getParameter("minSize");
	if(minSizeParameter)
	{
		minSizeParameter->setChangeFunction(SLOT(minSize()));
		m_minSize = getDoubleValue("minSize");
	}

	Parameter *maxSizeParameter = getParameter("maxSize");
	if(maxSizeParameter)
	{
		maxSizeParameter->setChangeFunction(SLOT(maxSize()));
		m_maxSize = getDoubleValue("maxSize");
	}


    m_focusPointTarget.x = 0.5;
    m_focusPointTarget.y = 0.5;
    
    m_focusPoint.x = 0.5;
    m_focusPoint.y = 0.5;
    
    // initialize data
	std::string cascadeFileName = HAAR_FILENAME;
	m_detectedObjects.clear();

	m_cascadeClassifier = new CascadeClassifier();
	// check if the cascade file exists
	if(!m_cascadeClassifier->load(cascadeFileName))
		Log::error("Could not load the cascade file.", "FaceTrackerNode::FaceTrackerNode");

	Parameter *drawDebugInformationParameter = getParameter("drawDebugInformation");
    if (drawDebugInformationParameter)
        drawDebugInformationParameter->setChangeFunction(SLOT(drawDebugInformation()));


}


//!
//! Destructor of the FaceTrackerNode class.
//!
FaceTrackerNode::~FaceTrackerNode ()
{  
    for (unsigned int i = 0; i < m_faces.size(); ++i)
        delete m_faces.at(i);
    m_faces.clear();
	delete m_cascadeClassifier;

}


///
/// Public Functions
///

///
/// Public Slots
///

/////
/////   PRIVATE SLOTS
/////

//!
//! Sets if user wants to flip the Image
//!
void FaceTrackerNode::flipPicture()
{
	m_flipHorizontal =  getValue("flipHorizontal").toBool();
}

//!
//! Sets the Probility value
//!
void FaceTrackerNode::probability()
{
	m_probability = getDoubleValue("probability");
}

//!
//! Sets the switchTime value
//!
void FaceTrackerNode::switchTime()
{
	m_switchTime = getValue("switchTime").toInt();
}

//!
//! Sets the m_switchTimeRand value
//!
void FaceTrackerNode::switchTimeRand()
{
	m_switchTimeRand = getValue("switchTimeRand").toInt();
}
    
//!
//! Sets the m_randomLook value
//!
void FaceTrackerNode::randomLook()
{
	m_randomLook = getValue("randomLook").toBool();
}

//!
//! Set m_focusPointSpeed
//!
void FaceTrackerNode::focusPointSpeed()
{
	m_focusPointSpeed = getValue("focusPointSpeed").toDouble();
}

//!
//! Set m_focusPointMoveLimit
//!
void FaceTrackerNode::focusPointMoveLimit()
{
	m_focusPointMoveLimit = getValue("focusPointMoveLimit").toDouble();
}

//!
//! Set m_switchUserNow
//!
void FaceTrackerNode::switchUserNow()
{
	m_switchUserNow = getValue("switchUserNow").toBool(); 
}

//!
//! Set the mimSize of a detected face
//!
void FaceTrackerNode::minSize()
{
	m_minSize = getDoubleValue("minSize");
}

//!
//! Set the MaxSize of a detected face
//!
void FaceTrackerNode::maxSize()
{
	m_maxSize = getDoubleValue("maxSize");
}

//!
//! Sets if user wants the Debug Information in the Picture-Drawn
//!
void FaceTrackerNode::drawDebugInformation ()
{
	m_drawDebugInformation = getValue("drawDebugInformation").toBool();
}


void FaceTrackerNode::processInputMatrix()
{
	if(m_imagePtr && m_matrixParameter->isConnected()){
		m_picture = m_imagePtr->clone();
		if(m_flipHorizontal)
			cv::flip(m_picture, m_picture, 1);
		detectFaces(m_picture);
		drawDetectedFaces(m_picture);
		if(isViewed()){
			// add debug information to image
			if(m_drawDebugInformation)
				drawDebugInfo(m_picture);
			updateVideoTexture();
		}
		decreaseProbabilities();
		// send a message with the current list of faces as attractors to the Behavior system's message cache
		attractorCount = (unsigned int) m_faces.size();
		//std::cout << "Faces: " << m_faces.size() << std::endl; 

			
		//if(abs(m_focusPoint.x - m_focusPointTarget.x) < 0.01 && abs(m_focusPoint.y - m_focusPointTarget.y) < 0.01) {
			m_attractors.clear();
			Attractor attractor;
			// use list of faces as attractors

			for (unsigned int i = 0; i < attractorCount; ++i) {
				//attractors[i].focusPoint = m_faces.at(i)->getPointOfAttraction();
				//attractors[i].size       = m_faces.at(i)->getRect().w / m_width;
				float prob = m_faces.at(i)->getProbability();   
				//std::cout << "Prob: " << prob << std::endl; 
				if(m_faces.at(i)->getProbability() > m_probability) {
					//std::cout << i <<"fx: " << m_faces.at(i)->getPointOfAttraction().x << "fy: " << m_faces.at(i)->getPointOfAttraction().y << std::endl;
					attractor.focusPoint = m_faces.at(i)->getPointOfAttraction();
					attractor.size       = m_faces.at(i)->getRect().w / m_width;
					m_attractors.push_back(attractor);
				}
			}
			processAttractors();
			animateFocusPoint();	
			
				if (m_attractors.size() == 0){
					m_lastNumberofAttractos = m_attractors.size();
					setValue("noFaceDetected", 1, true);
				}
				else{
					if( m_lastNumberofAttractos < m_attractors.size() && m_attractors.size() > 0)
					{
						m_lastNumberofAttractos = m_attractors.size();
						setValue("noFaceDetected", 0, true);
					}
					//if(abs(m_focusPoint.x - m_focusPointTarget.x) < 0.01)
					if( ceil(m_focusPoint.x * 100)/100 != ceil(m_focusPointTarget.x * 100)/100 )
					{
						m_focusPoint.x = m_focusPointTarget.x;
						setValue("faceX", m_focusPoint.x, true);
					}
					if(ceil(m_focusPoint.y * 100)/100 != ceil(m_focusPointTarget.y * 100)/100)
					{
						m_focusPoint.y = m_focusPointTarget.y;
						setValue("faceY", m_focusPoint.y, true);
					}
				}

			
		//}
		
	}
}

///
///   PRIVATE FUNCTIONS
///



//!
//! Animates the focus point according to the focus point target and whether or not a face is currently focused.
//!
void FaceTrackerNode::animateFocusPoint ()
{
    //m_mutex.lock();


		// look around randomly (if activated)
	if (m_randomLook && QTime::currentTime() > m_focusTimeLimit && getRandom() > 0.95f) {
			m_attractorIndex     = -1;
			m_focusPointTarget.x = getRandom();
			m_focusPointTarget.y = getRandom();
			m_focusTimeLimit     = QTime::currentTime(); 
			m_focusTimeLimit.addMSecs(m_switchTime + getRandom((unsigned int) m_switchTimeRand));
		}

		// animate focus point
		PointNDC delta;
		delta.x = (m_focusPointTarget.x - m_focusPoint.x);
		delta.y = (m_focusPointTarget.y - m_focusPoint.y);
		double length = sqrt(delta.x * delta.x + delta.y * delta.y);
		//if (length > focusPointMoveLimit) {
			//m_focusPoint.x += delta.x * focusPointSpeed;
			//m_focusPoint.y += delta.y * focusPointSpeed;
			

			
			//if(abs(m_focusPoint.y - m_focusPointTarget.y) < 0.01)
			
		//}
   // m_mutex.unlock();
    //if (abs(delta.x) > focusPointMoveLimit)
    //    m_focusPoint.x += delta.x;
    //if (abs(delta.y) > focusPointMoveLimit)
    //    m_focusPoint.y += delta.y;
}

//!
//! Handler for attractor list.
//! Updates the coordinates of the focus point depending on the list of attractors.
//!
void FaceTrackerNode::processAttractors ()
{
    unsigned int attractorCount = m_attractors.size();
    m_attractorIndexOld = m_attractorIndex;
   

    if (attractorCount == 1) {
        // select single attractor
        if ( QTime::currentTime() > m_focusTimeLimit)
          m_attractorIndex = 0;
    } 
    else {
        // check if current attractor has been focused long enough and if no main animation is currently running
        if (m_switchUserNow || QTime::currentTime() > m_focusTimeLimit) {
            m_switchUserNow = false;
            m_justSwitched = getIntValue("switchDelay");
            m_attractorIndex = attractorCount == 2
                ? !m_attractorIndex                // switch between 2 attractors
                : getRandom(attractorCount);       // select random attractor from the list
        }
    }
    // update the focus time limit if the attractor index has changed
    if (m_attractorIndex != m_attractorIndexOld) {
        m_focusTimeLimit = QTime::currentTime();
        //std::cout << "after: " << m_focusTimeLimit.toString().toStdString();
        m_focusTimeLimit = m_focusTimeLimit.addMSecs(m_switchTime + getRandom((unsigned int) m_switchTimeRand));
        //std::cout << "after: " << m_focusTimeLimit.toString().toStdString() << std::endl;
    }
    // check if a valid index into the list has been selected
    if (m_attractorIndex >= 0 && m_attractorIndex < (int) attractorCount) {
        // use the focus point of the currently selected attractor as the target point to look at
        //m_mutex.lock();
			m_focusPointTarget = m_attractors[m_attractorIndex].focusPoint;
        //m_mutex.unlock();
    }
    if (m_justSwitched > 0)
        m_justSwitched--;
}



//!
//! Detects all faces in the given image, and creates or updates face objects for each detected face.
//!
//! \param image    the image to detect faces in
//!
void FaceTrackerNode::detectFaces(Mat image)
{
    RectNDC detectionRect;
 
	float   scale = 2.0;

	Mat gray = image.clone();
	cvtColor(gray, gray, CV_BGR2GRAY, 1);
	cv::resize(gray, gray, cvSize( cvRound (m_width/scale), cvRound (m_height/scale)), CV_INTER_LINEAR);
	int imageWidth  = gray.cols;
    int imageHeight = gray.rows;
	cv::equalizeHist(gray, gray);
 
	m_detectedObjects.empty();

	m_cascadeClassifier->detectMultiScale(
		gray,						// Matrix of type CV_8U containing the image in which to detect objects.
		m_detectedObjects,			// Vector of rectangles such that each rectangle contains the detected object.
		1.1,						// Specifies how much the image size is reduced at each image scale.
		3,							// Speficifes how many neighbors should each candiate rectangle have to retain it.
		CV_HAAR_DO_CANNY_PRUNING,							// This parameter is not used for new cascade and have the same meaning for old cascade as in function cvHaarDetectObjects.
		Size(30,30));				// The minimum possible object size. Objects smaller than that are ignored

	unsigned int i;
	for(i=0; i < m_detectedObjects.size(); i++){
        //CvRect *r = (CvRect *) cvGetSeqElem(m_cascadeClassifier->s, i);
		cv::Rect r = m_detectedObjects.at(i);
        // get normalized device coordinates of rectangle
        detectionRect.x = (float) r.x      / imageWidth;
        detectionRect.y = (float) r.y      / imageHeight;
        detectionRect.w = (float) r.width  / imageWidth;
        detectionRect.h = (float) r.height / imageHeight;
        // iterate over all existing faces and check if detected object can be assigned to a face
        unsigned int n = 0;
        bool found = false;
        while (n < m_faces.size() && !found) {
			found = m_faces.at(n)->checkDetectionRect(detectionRect);
			if (!found)
                ++n;
        }
        // create new Face object if detection rect could not be associated with any existing face
    
		if (!found && detectionRect.w > m_minSize && detectionRect.w < m_maxSize)
				m_faces.push_back(new Face(detectionRect));	
    }
	m_detectedObjectsCount = i;
}


//!
//! Draws an overlay onto the image that shows the locations of detected faces in the scene.
//!
//! \param image    the image to show detected faces in
//!
void FaceTrackerNode::drawDetectedFaces(Mat image)
{
    for (unsigned int i = 0; i < m_faces.size(); ++i) {
        Face     *face          = m_faces.at(i);

        // retrieve point positions in normalized device coordinates
        RectNDC   rect          = face->getRect();
        PointNDC  poa           = face->getPointOfAttraction();

        // calculate point positions
        CvPoint   pTopLeft      = toCvPoint(image, rect);
        CvPoint   pBottomRight  = toCvPoint(image, rect.x + rect.w, rect.y + rect.h);
        CvPoint   pCenter       = toCvPoint(image, poa);

        // calculate color from probability value (0.0..0.5..1.0 => red..yellow..green)
        float     probab        = face->getProbability();
		Scalar& color = Scalar(0.0, MIN(255 * probab * 2, 255), MIN(255 * (1.5f - probab), 255), 255.0); //last Parameter is the Alphachannel

        // draw face bounding rect
		cv::rectangle(image, pTopLeft, pBottomRight, color, 3, 8, 0);
        // draw point of attraction
		cv::circle(image, pCenter, 5, color);
    }
}

//!
//! Draws some debug information into the given image.
//!
//! \param image    pointer to the image to draw the debug info in
//!
void FaceTrackerNode::drawDebugInfo(Mat image)
{
#ifndef __GNUC__
    char buffer[200];

	Scalar& colorBlack = Scalar(0.0, 0, 0, 255.0); //last Value is alpha channel
	Scalar& colorWhite = Scalar(255.0, 255.0, 255.0, 255.0); //last Value is alpha channel

	// draw the number of faces in the image
	switch (m_detectedObjectsCount) {
                        case 0:  sprintf_s(buffer, 200, "no faces detected"); break;
                        case 1:  sprintf_s(buffer, 200, "%d face detected",  m_detectedObjectsCount); break;
                        default: sprintf_s(buffer, 200, "%d faces detected", m_detectedObjectsCount);
	}
	cv::putText(image, buffer, cvPoint(11, 25), CV_FONT_HERSHEY_SIMPLEX, 0.7, colorBlack, 1);
	cv::putText(image, buffer, cvPoint(10, 20), CV_FONT_HERSHEY_SIMPLEX, 0.7, colorWhite, 1);

	switch (m_faces.size()) {
                        case 0:  sprintf_s(buffer, 200, "no faces in list"); break;
                        case 1:  sprintf_s(buffer, 200, "%d face in list",  m_faces.size()); break;
                        default: sprintf_s(buffer, 200, "%d faces in list", m_faces.size());
	}

	cv::putText(image, buffer, cvPoint(11, 43), CV_FONT_HERSHEY_SIMPLEX, 0.7, colorBlack, 1);
	cv::putText(image, buffer, cvPoint(10, 38), CV_FONT_HERSHEY_SIMPLEX, 0.7, colorWhite, 1);

#endif
}

//!
//! Decreases the probability values of the detected faces.
//! Is called on each time step (/frame).
//!
void FaceTrackerNode::decreaseProbabilities()
{
    // traverse all faces
    unsigned int i = 0;
    while (i < m_faces.size()) {
        // decrease probability of face
        m_faces.at(i)->decreaseProbability();
        // delete face if probability is (near) zero
        if (abs(m_faces.at(i)->getProbability()) < EPSILON) {
            delete m_faces.at(i);
            m_faces.erase(m_faces.begin() + i);
        } else
            ++i;
    }
}

} // namespace FaceTrackerNode 
