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
//! \file "PoemReaderNode.h"
//! \brief Header file for PoemReaderNode class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       29.04.2009 (last updated)
//!

#ifndef POEMREADERNODE_H
#define POEMREADERNODE_H

#include "Node.h"
#include "ImageNode.h"
#include "FrapperPrerequisites.h"
#include "BaseGraphicsView.h"

#include <qpropertyanimation.h>


#include <QPointF>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtGui>
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace PoemReaderNode {
using namespace Frapper;

//!
//! Class representing nodes for head animation logic in a 3D scene.
//!
class PoemReaderNode : public Node
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PoemReaderNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    PoemReaderNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the BorAnimationNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PoemReaderNode ();

public: // functions


	void createPoemsData();

	//!
    //! Render the words in the scene view.
    //!
	//! \param scene The scene where to render
    //! \param word The string to render.
	//! \param size Is the point size of the word according to its frequency of appearance
	//! \param posx Is the position in X that the word will occupy
	//! \param posy Is the position in Y that the word will occupy
	//! \param gWordHeight Contains the height of the word with the greates height in one line
    //!
	void renderWords(QGraphicsScene *scene, QString word, qreal size, int &posx, int &posy, int &gWordHeight);

	//!
	//! Checks if the word is suitable to be in the cloud. For instance, it is not a connector (and, but, with...) or indicates location (this, there, that) ... and so on
	//!
	bool wordAccepted(QString word);

	//!
	//! Removes the wild cards provided to Kinect to understand better the words: "I choose" and "use ... please"
	//!
	QString removeWildCards(QString word);

	//!
	//! This function allows to elicit an empathic response from the character
	//!
	QString replyFromCharacter(QString word, bool finalWord);


public slots:

	//!
    //! Loads the XML file.
    //!
	void loadXmlFile();

	//!
    //! Reloads the XML file.
    //!
	void reloadXmlFile();
	

private: //methods
   
	bool findWord(QString str);
	bool hasBeenSelected(QString word);
	void setPoemSelected(QString name, QString replyCharacter);
	void createPoemParameterGroup(QString poem, int poemId);
	void createGrammar(QString wordselected);
	

private slots: 

	void reduceWordsCloud();

	void updatePoemSelected();

	void triggerPoemLines();

	void updateFadingOutTimer();

	void updateFadingInTimer();

	void stopReciting();

signals:

	void emitGrammar(QString grammar);
	

private: // data

	QString			m_poemsIndexStr;
	QStringList		poems;
	QStringList		selectedWords;
	QTimer			*m_foutTimer;
	QTimer			*m_finTimer;
	QWidget			*parent;
	
	int m_numberWords;
	int poemLine;

	bool m_recitingPoem;

	ParameterGroup *parameterRoot; // pointer to this Parameter Root

protected: // data

	ParameterGroup *m_wordsGroup;	
	ParameterGroup *m_poemsGroup;
	ParameterGroup *m_poemLinesGroup;

	Parameter *groupParameter;
	Parameter *inputStringParameter;
	Parameter *inputTagParameter;
	Parameter *animStateParameter;	
	Parameter *m_poemSelected;
	Parameter *m_alphaParameter;
	Parameter *m_activationParameter;

	int m_numberWordsSelected;
	bool m_stopPoem;
	int m_minFrequency;
};

} // namespace PoemReaderNode 

#endif
