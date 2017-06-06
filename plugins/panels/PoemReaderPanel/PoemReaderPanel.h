/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "PoemReaderPanel.h"
//! \brief Header file for PoemReaderPanel class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       24.06.2009 (last updated)
//!

#ifndef PoemReaderPanel_H
#define PoemReaderPanel_H

#define MIN(X,Y) ((X) < (Y) ?  (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ?  (X) : (Y))

#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "Node.h"
#include "Parameter.h"
#include "NumberParameter.h"
#include "FilenameParameter.h"
#include "EnumerationParameter.h"
#include "ParameterPlugin.h"
//#include "TagCloudView.h"



#include <math.h>
#include <QtWidgets>

namespace Frapper {

//!
//! Class representing a view for displaying and editing parameters of nodes.
//!
class PoemReaderPanel : public ViewPanel

{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the PoemReaderPanel class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    PoemReaderPanel ( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

    //!
    //! Destructor of the PoemReaderPanel class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~PoemReaderPanel ();


public slots: //

	//!
	//! Reduces the wordcloud given a word associated to a poem
	//!
	void sendPoem(int buttonId);
	//!
	//! To visualize the cloud in OGRE
	//!
	void visualizeCloud();

	void clicked(int buttonId);
	

public: // functions


	//!
	//! Connects the panel with the scene.
	//!
	//! \param *nodeModel NodeModel of the scene
	//! \param *sceneModel SceneModel of the scene
	//!
	virtual void registerControl(NodeModel *nodeModel, SceneModel *sceneModel);

	//!
	//! Adds a item to the list
	//!
	//! \param parameter Parameter to be added to a list
	//! \param groupElement true if element of a parameter group
	//! \return the created QListWidgetItem
	//!
	QListWidgetItem* addItem(Parameter *parameter, bool groupElement);

	//!
	//! Fills the table with with the words parameters of the PoemReaderNode
	//!
	//! \param node Is the selected node of the scene
	void createWordsTable(Node *selectedNode);

	// Added for word cloud generation


	
	bool findWord(QString word, int &listIndex);
	bool obtainWord(QString word);

	void loadXmlFile();

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
	
	void clearLayout(QLayout* layout, bool deleteWidgets);

	
private slots: //

	//!
	//! Is called a node is selected
	//!
	//! \param selecedNode the selected wich was selected
	//!
	void updateSelectedNode(Node * selectedNode);

	//!
	//! Updates the panel if the node model changes
	//!
	void update();

	//!
	//! Updates the scene and the panel if a node is selected in the panel
	//!
	void nodeSelected(QListWidgetItem * listItem); 

	
private: // data


	//!
	//! Node model
	//!
	NodeModel *m_nodeModel;

	//!
	//! selected node
	//!
	Node *m_node;

	//!
	//! Scene model
	//!
	SceneModel *m_sceneModel;

	//!
	//! List Widget to show all nodes in the scene
	//!
	QListWidget *m_ListNodes;

	//!
	//! Line Edit to introduce the word to render
	//!
	QLineEdit *m_lineEdit;

	QTableWidget *m_wordsTable;

	QStringList m_poemsindex;

	int rowsInTable;
	QStringList poems;
	bool done;
	
	QGraphicsScene *scene;
	QGraphicsView *view;
	QHBoxLayout *m_hboxLayout;
	QVBoxLayout *m_leftvboxLayout;
	QVBoxLayout *m_rightvboxLayout;
	QPushButton *m_buttonSelector;
	QPushButton *buttonCloudGenerator;
	QTabWidget *m_tabWidget;
	QSignalMapper* m_signalMapper;

	//TagCloudView *m_tagCloudView;
};



} // end namespace Frapper

#endif
