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
//! \file "TagCloudView.h"
//! \brief Header file for TagCloudView class.
//!
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \version    1.0
//! \date       28.07.2009 (last updated)
//!

#ifndef TAGCLOUDVIEW_H
#define TAGCLOUDVIEW_H

#define MIN(X,Y) ((X) < (Y) ?  (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ?  (X) : (Y))

#include "FrapperPrerequisites.h"
#include "BaseGraphicsView.h"
#include "ParameterGroup.h"
#include "NumberParameter.h"
#include "TimelineGraphicsItem.h"
#include "ImageNode.h"
#include <QtGui>
#include <QPointF>

namespace PoemReaderNode {
using namespace Frapper;

//!
//! Class representing a graphics view for displaying and editing animated
//! number parameters.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph CurveEditorGraphicsView {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     CurveEditorGraphicsView [label="CurveEditorGraphicsView",fillcolor="grey75"];
//!     QGraphicsView -> CurveEditorGraphicsView;
//!     QGraphicsView [label="QGraphicsView",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qgraphicsview.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!

class KeyGraphicsItem;

class TagCloudView : public BaseGraphicsView
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the TagCloudView class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    TagCloudView ( QWidget *parent = 0 );

    //!
    //! Destructor of the TagCloudView class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TagCloudView ();

public: // functions
 
	//!
	//! Sets the image in the selected ImageNode.
	//!
	//!
	void setImage(Node* selectedNode);

	//!
	//! Gets the image of the TagCloudView scene
	//!
	//!
	QImage* getImage();

	void createImage();

	void RenderCloud(QTableWidget *wordsTable, int rowsInTable);

public slots: //

	//!
    //! Resets the network graphics view's matrix.
    //!
    void homeView ();


signals:
	//!
	//! Signal that is emitted when a drag event is emited
	//!
	void drag();

protected: // event handlers

    //!
    //! The overwritten the event handler for resize events.
    //! Adds scene resizing and redrawing.
    //!
    //! \param event The description of the key event.
    //!
    virtual void resizeEvent ( QResizeEvent *event );

    //!
	//! The overwritten event "fills" the QImage with the content of the Qt scene 
	//!
	//virtual void paintEvent(QPaintEvent *event);

	
	
protected: // functions
	

private: // functions
	

private: // data

    
	//!
    //! The flag for showing only enabled parameters.
    //!
    bool m_showEnabledCurves;

	//!
    //! Scaling factor of the scene
	qreal m_scaleFactor;

	//!
    //! The image that contains the rendered scene.
    //!
	QImage *m_image;

	int m_fade;
    QPixmap m_originalPicForFade;

	int biggestSize;
	int smallestSize;
	int dDeg;
	bool useArea;
	double dRadius;
};

} // namespace PoemReaderNode 

#endif