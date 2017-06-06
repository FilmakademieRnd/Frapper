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
//! \file "AnimationClipNode.h"
//! \brief Header file for MeshNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#ifndef ANIMATIONCLIPNODE_H
#define ANIMATIONCLIPNODE_H

#include "Node.h"
#include <QColor>
#include <QtCore/QVariant>
#include <QStandardItem>
#include <QtCore/QTimer>
#include <QtCore/QTime>

// OGRE
#include "Ogre.h"
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace AnimationClipNode {
using namespace Frapper;

//!
//! Data structure for animation curves.
//!
struct AnimCurve
{

public: // constructors and destructors

    AnimCurve()
    {
    }

    ~AnimCurve()
    {
        keys.clear();
    }

public: // data

	float length;
    QString name;
    QList<QPair<float, float>> keys;

};


//!
//! Class in the Borealis application representing nodes that can
//! contai OGRE entities with animation.
//!
class AnimationClipNode : public Node
{

    Q_OBJECT
    ADD_INSTANCE_COUNTER

public: // constructors and destructors

    //!
    //! Constructor of the AnimationClipNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    AnimationClipNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the AnimationClipNode class.
    //!
    ~AnimationClipNode ();

public: // functions

    ////!
    ////! Runs the internal code for this node.
    ////!
    ////! \return True if processing the node succeeded, otherwise False.
    ////!
    //bool process ( const QString& connectorName );

    //!
    //! Sets up time dependencies for node parameters.
    //!
    //! \param timeParameter The parameter representing the current scene time.
    //!
	virtual void setUpTimeDependencies ( NumberParameter *timeParameter, NumberParameter *rangeParameter );

public slots: //

    //!
    //! Loads the animation clip from the selected file.
    //!
    //! Is called when the animation clip file has been changed.
    //!
    void animationClipFileChanged ();
    
    //!
    //! Slot which is called when animation parameter needs to be updated.
    //!
    void processAnimationParameter();

    //!
    //! Slot which is called on interpolation mode change.
    //!
    void toggleInterpolationMode();

	//!
    //! Slot which is called Offset Animation option change.
    //!
    void toggleOffsetAnimation();

	//!
    //! Slot which is called external control option change.
    //!
	void toggelExternalControl();

	//!
	//! Slot which is called when Scale And Play is changed.
	//!
	void toggleScaleAndChange();


    //!
    //! On timer update.
    //!
    void updateTimer ();

    //!
    //! Trigger anim file reload.
    //!
    void triggerReload ();

	//!
	//! It adjusts the "length" of the clip according the the "scale" value. Then, the clip is played only once
	//!
	void scaleAndChange();


protected: // functions

    //!
    //! Reads all animation curves from the file with the given name.
    //!
    //! \param filename The name of the file with the animation curves to parse.
    //!
    void parseAnimationFile ( const QString &filename );

    //!
    //! Generates OGRE animation curves (Ogre::NumericAnimationTrack).
    //!
    //! \param clipName Name of the clip.
    //! \param animationLength Overall length of animation clip.
    //!
    void createAnimationCurves ( QString clipName, float clipLength );

    //!
    //! Loads animation clip.
    //!
    bool loadAnimationClip ();

    //!
    //! Removes dynamic parameters.
    //!
    bool removeParameters (); 

    //!
    //! Generates dynamic parameters.
    //!
    bool generateParameters();

private slots: //

    //!
    //! Increase frame count.
    //!
    void increaseFrameCount ();

    //!
    //! Reset frame count.
    //!
    void resetFrameCount ();

private: // data

	bool m_isExternallyControlled;
    bool m_isPlaying;
    bool m_stop;
	bool m_offsetAnimation;
    float m_progress;
	float m_defaultLength;
    int m_currentFrame;
    ParameterGroup *m_animationGroup;
    ParameterGroup *m_boneGroup;
    ParameterGroup *m_cameraGroup;
	Ogre::Animation* m_animation;
    QTimer *m_timer;
	QTime m_lastTimestamp;
    QStringList m_groupSuffixes;
    QStringList m_boneSuffixes;
    QStringList m_translateSuffixes;
    QStringList m_rotateSuffixes;
    QStringList m_scaleSuffixes;
    QHash<QString, QHash<QString, float> > m_parameterMap;
    QHash<QString, unsigned short> m_forwCurveMap;
    QHash<unsigned short, QString> m_backwCurveMap;
    QHash<QString, AnimCurve> m_animCurves;
};
} // namespace AnimationClipNode

#endif
