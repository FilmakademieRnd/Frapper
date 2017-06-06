/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2009 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \file "universalGraph.h"
//! \brief Header file for universalGraph class.
//!
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       24.06.2009 (last updated)
//!

#ifndef UNIVERSALGRAPH_H
#define UNIVERSALGRAPH_H

#include "FrapperPrerequisites.h"
#include "WidgetPlugin.h"

#include <QPoint>
#include <QMouseEvent>
#include <QColor>
#include <QPixmap>
#include <QImage>

using namespace Frapper;

//!
//! Class representing a view for displaying and editing parameters of nodes.
//!
class universalGraph : public WidgetPlugin
{
Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the universalGraph class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param flags Extra widget options.
    //!
    universalGraph ( QWidget *parent , ParameterPlugin * parameter);

    //!
    //! Destructor of the universalGraph class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~universalGraph ();







private: // functions

	//!
	//! mouse moves
	//!
	void mouseMoveEvent( QMouseEvent* event );

	//!
	//! mouse pressed
	//!
	void mousePressEvent( QMouseEvent* event );

	//! 
	//! Transfers the QPoint coordinats of the widget into a -1/1 system
	//!
	//! \param p The QPoint of the current position
	//! \return a list of the coordinats as floats within QVariants
	//!
	QList<QVariant> getFloatCoordinates(QPoint p);

	//!
	//! Transfers the float coordinates into QPoint coordinates
	//!
	//! \param point List of QVariants of float values
	//! \return Coordinats of the point as QPoint
	//!
	QPoint universalGraph::getPointCoordinates(QList<QVariant> point);

	//!
	//! Draws the background image
	//!
	void drawBackgroundImage();

	//!
	//! Returns the color of a point with a certain weight
	//!
	//! \param p Index of the point
	//! \param w Weight of that point
	//! \return the final color
	//!
	QColor WeightedListColor(int p, float w);

	//! 
	//! Returns the index of a color according to the index of a point
	//!
	//! \param p Index of the point
	//! \return Index of the color
	//!
	int ListPositionToColorIndex(int p);

	//!
	//! Calculates the weights of the points
	//!
	//! \param position List of float-values as the coordinates of the current point
	//! \param dimensions Number of dimensions
	//! \return A List of weights
	//!
	QList<float> interpolation_algorithm_1(QPoint position);


private: // data

	//!
	//! old file
	//!
	QString m_oldFile;

	//!
	//! colors of elements
	//!
	QList<QColor> m_colors;

	//!
	//! background image
	//!
	QImage bgIMG;

	//! 
	//! List of all Points
	//!
	QList<QPoint> m_points;

	//!
	//! List of all expression names
	//!
	QStringList m_expressionNames;

	//!
	//! current cursor position
	//!
	QPoint m_cursor;




private slots: // functions

	//!
	//! Draws the Widget
	//!
	//! \param QPaintEvent 
	//!
	void paintEvent(QPaintEvent *);

};


#endif
