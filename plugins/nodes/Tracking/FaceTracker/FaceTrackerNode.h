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
//! \file "FaceTrackerNodePlugin.h"
//! \brief Header file for FaceTrackerNodePlugin class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \autor		Martin Schwarz <martin.schwarz@filmakademie.de>
//! \version    1.0
//! \date       04.06.2012 (last updated)
//!

#ifndef FACETRACKERNODE_H
#define FACETRACKERNODE_H

#include "opencvColorInput.h"
#include "ParameterGroup.h"
#include <QtCore/QVariant>
#include <QtCore/QTime>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QTimer>

#include <vector>

#include "Face.h"
#include "Helper.h"

namespace FaceTrackerNode {
using namespace Frapper;
using namespace cv;

//!
//! Class in the Borealis application representing nodes that can
//! contai OGRE entities with animation.
//!
class FaceTrackerNode : public opencvColorInput  //, public QThread
{

	Q_OBJECT

public: /// constructors and destrucor

    //!
    //! Constructor of the FaceTrackerNode class.
    //!
    //! \param name The name to give the new mesh node.
    //!
	FaceTrackerNode ( QString name, ParameterGroup *parameterRoot );

public: /// destructors

    //!
    //! Destructor of the FaceTrackerNode class.
    //!
    ~FaceTrackerNode ();

public: /// methods
    	
    //virtual void run();

public slots:

private slots:

	//!
	//! Sets if user wants to flip the Image
	//!
	void flipPicture();

	//!
	//! Sets the Probility value
	//!
	void probability();

	//!
	//! Sets the switchTime value
	//!
	void switchTime();

	//!
	//! Sets the m_switchTimeRand value
	//!
	void switchTimeRand();
    
	//!
	//! Sets the m_randomLook value
	//!
	void randomLook();

	//!
	//! Set m_focusPointSpeed
	//!
	void focusPointSpeed();

	//!
	//! Set m_focusPointMoveLimit
	//!
	void focusPointMoveLimit();

	//!
	//! Set m_switchUserNow
	//!
	void switchUserNow();

	//!
	//! Set the mimSize of a detected face
	//!
	void minSize();

	//!
	//! Set the MaxSize of a detected face
	//!
	void maxSize();

	//!
	//! Sets if user wants the Debug Information in the Picture-Drawn
	//!
	void drawDebugInformation();

	//!
	//! SLOT that is used on the matrixParameter
	//!
	virtual void processInputMatrix();

private: /// methods
    //!
    //! Detects all faces in the given image, and creates or updates face objects for each detected face.
    //!
    //! \param image    the image to detect faces in
    //!
	void detectFaces(cv::Mat image);

    //!
    //! Draws an overlay onto the image that shows the locations of detected faces in the scene.
    //!
    //! \param image    the image to show detected faces in
    //!
	void drawDetectedFaces(cv::Mat image);

    //!
    //! Draws some debug information into the given image.
    //!
    //! \param image    pointer to the image to draw the debug info in
    //!
	void drawDebugInfo(cv::Mat image);

    //!
    //! Decreases the probability values of the detected faces.
    //! Is called on each time step (/frame).
    //!
    void decreaseProbabilities();

	//!
    //! Handler for attractor list.
    //! Updates the coordinates of the focus point depending on the list of attractors.
    //!
    void processAttractors();

    //!
    //! Animates the focus point according to the focus point target and whether or not a face is currently focused.
    //!
    void animateFocusPoint();



private: /// attributes
    
    //!
    //! A mutex.
    //!    
    QMutex m_mutex;

    //!
    //! A Timer.
    //!    
    QTimer m_timer;

    //!
    //! The current timestamp.
    //!
    QTime m_timestamp;
    
    //!
    //! Timestamp from previous step.
    //!
    QTime m_lastTimestamp;

    //!
    //! How many objects are currently detected?
    //!
    int m_detectedObjectsCount;
    
    //!
    //! List of users which attract the agent.
    //!
    std::vector<Face *> m_faces;

    //!
    //! List of users which attract the agent.
    //!
    std::vector<Attractor> m_attractors;

    //!
    //! Index of currently atrracted user.
    //!
    int m_attractorIndex;

    //!
    //! Old index of attracted user.
    //!
    int m_attractorIndexOld;
    
    //!
    //! Has there been a user switching recently?
    //!
    int m_justSwitched;

    //!
    //! ???????
    //!
    QTime m_focusTimeLimit;

    //!
    //! ???????
    //!
    QTime m_smallFocusTimeLimit;

	//!
    //! OpenCV rect storage.
    //!
	vector<cv::Rect> m_detectedObjects;

	//!
    //! OpenCV haar classifier cascade.
    //!
	cv::CascadeClassifier *m_cascadeClassifier;

    //!
    //! Configuration parameter group.
    //!
    ParameterGroup *m_configGroup;

    //!
    //! Output parameter group
    //!
    ParameterGroup *m_outputGroup;

    //!
    //! Focus point.
    //!
    PointNDC m_focusPoint;
    
    //!
    //! Focus point target.
    //!
    PointNDC m_focusPointTarget;

    //!
    //! Is used to set a delay for the thread.
    //!
	unsigned int delay;

    //!
    //! Counts the attractors
    //!
	unsigned int attractorCount;

	//!
    //! Sets the probability for a face - value is from the gui
    //!
	double m_probability;

	//!
	//! Is true if DebugInformation is clicked
	//!
	bool m_drawDebugInformation;

	//!
	//! Flip Picture horizontal if true
	//!
	bool m_flipHorizontal;

	//!
	//! Value at what time the user should be switched
	//!
	int m_switchTime;

	//!
	//! Value for the TimeRandomize
	//!
	int m_switchTimeRand;

	//!
	//! The Value for the randomLook
	//!
	int m_randomLook;

	//!
	//! The Value for the Point tspeed of the focus
	//!
	double m_focusPointSpeed;

	//!
	//! The Limit of the focus
	//!
	double m_focusPointMoveLimit;

	//!
	//! Shall the user be switched now?
	//!
	bool m_switchUserNow;

	//!
	//! the minimum Size of a detected face
	//!
	double m_minSize;

	//!
	//! the maximum Size of a detected face
	//!
	double m_maxSize;

	//!
	//! The last number of attractors
	//!
	int m_lastNumberofAttractos;
	
};

} // namespace FaceTrackerNode 

#endif
