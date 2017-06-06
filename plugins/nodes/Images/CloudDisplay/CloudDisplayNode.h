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
//! \file "CloudDisplayNode.h"
//! \brief Header file for CloudDisplayNode class.
//!
//! \author     Diana Arellano
//! \version    1.0
//! \date       18.09.2012 (last updated)
//!

#ifndef CLOUDDISPLAYNODE_H
#define CLOUDDISPLAYNODE_H

#define MIN(X,Y) ((X) < (Y) ?  (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ?  (X) : (Y))


#include "FrapperPrerequisites.h"
#include "ImageNode.h"
#include "TagCloudView.h"
#include "BaseGraphicsView.h"
#include "ParameterGroup.h"
#include "NumberParameter.h"
#include "TimelineGraphicsItem.h"
#include <QtGui>
#include <QPointF>

namespace CloudDisplayNode {
using namespace Frapper;


//!
//! Class representing nodes for head animation logic in a 3D scene.
//!
class CloudDisplayNode : public ImageNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the CloudDisplayNode class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    CloudDisplayNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the BorAnimationNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~CloudDisplayNode ();

public: // functions

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

	
public:
	TagCloudView *m_tagCloudView;

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
	bool obtainWord(QString word);
	void createWordsTable(QString word);
	void setPoemSelected(QString name);
	

private slots: //

	void displayCloud();
	void relocateCloud();
	void animateCloud();
	//void timerEvent();
	void activateTimer();


private: // data

	QTableWidget *m_wordsTable;
	
	int m_numberWords;
	int rowsInTable;
	int poemLine;
	
	QStringList poems;
	QStringList m_poemsindex;
	QString poemsIndexStr;

	QTimer *m_pTimer;
	
	bool m_animation;
	QWidget *parent;

	ParameterGroup *parameterRoot; // pointer to this Parameter Root

protected: // data

	ParameterGroup *m_wordsGroup;	
	ParameterGroup *m_poemsGroup;
	ParameterGroup *m_poemLinesGroup;

	Parameter *groupParameter;
	Parameter *outputImageParameter;
	Parameter *inputStringParameter;
	Parameter *inputTagParameter;
	Parameter *animStateParameter;	
	Parameter *m_poemSelected;
	Parameter *m_alphaParameter;
	//Parameter *m_numberWordsSelected;	

	Ogre::TexturePtr m_texture;
	Ogre::String uniqueRenderTextureName;

	int m_numberWordsSelected;
	int m_interval;
	
	qreal m_fontSize; 
	EnumerationParameter *m_fontsList;
	QString m_font;

};

} // namespace CloudDisplayNode 

#endif
