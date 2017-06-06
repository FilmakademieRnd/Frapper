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
//! \file "universalGraph.cpp"
//! \brief Implementation file for universalGraphclass.
//!
//! \author     Sebastian Bach <sebastian.bach@filmakademie.de>
//! \version    1.0
//! \date       07.12.2009 (last updated)
//!

#include "universalGraph.h"

#include "Log.h"

#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <QFile>


///
/// Constructors and Destructors
///

//!
//! Constructor of the universalGraphclass.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
universalGraph::universalGraph( QWidget *parent /* = 0 */, ParameterPlugin * parameter) : WidgetPlugin (parent, parameter)
{
	setFixedSize(500,500);	
	setPalette(QPalette(QColor(250, 250, 200)));
    setAutoFillBackground(true);

	bgIMG = QImage(500,500,QImage::Format_RGB16);
	bgIMG.fill(uint(700));

	setParameterTriggersRedraw(true);

	if(checkFlag(QString("points"))){
		QList<QVariant> data = getFlag(QString("points")).toList();
		for(int i = 0; i < data.length(); ++i)
			m_points.append(data.at(i).toPoint());
	}

	m_colors.append(QColor(255,0,0));
	m_colors.append(QColor(0,255,0));
	m_colors.append(QColor(0,0,255));
	m_colors.append(QColor(255,255,0));
	m_colors.append(QColor(255,0,255));
	m_colors.append(QColor(0,255,255));
}


//!
//! Destructor of the NetworkPanel class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
universalGraph::~universalGraph()
{

}

///
/// Public Funcitons
///

/// 
/// Pulic Slots
///

void universalGraph::paintEvent(QPaintEvent *)
{
	Q_SLOT

	// Prepearing painter

	// if "reset", delete all bins

	if(checkFlag(QString("reset")))
		if(getFlag(QString("reset")).toBool()){
			m_points.clear();
			m_expressionNames.clear();
			setFlag(QString("reset"), QVariant(false));
		}

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	QPen pen;


	// draws the background image

	p.drawImage(0,0,bgIMG);

	// draws information about the mode

	if(checkFlag(QString("mode")))
		if(getFlag(QString("mode")).toString() == QString("add")){
			pen.setColor(QColor(255,0,0));
			pen.setStyle(Qt::SolidLine);
			pen.setWidth(1);
			p.setPen(pen);
			p.drawText(10,30,QString("Click to add new expression"));
		}

	// draws a coordinate system

	pen.setColor(QColor(255,255,255));
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	p.setPen(pen);

	p.drawLine(0,250,500,250);
	p.drawLine(250,0,250,500);
	
	// draws the points

	pen.setColor(QColor(0,0,0));
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	p.setPen(pen);

	for(int i = 0; i < m_points.length(); ++i)
	{
		

		//QLabel * label =  new QLabel(this);
		//label->setGeometry(m_points.at(i).x(), m_points.at(i).y(), 100, 25);
		//label->setText( m_expressionNames.at(i) );
		//label->setFont(QFont("Times", 10, QFont::Bold));
		//label->setAutoFillBackground(true);
		//label->setPalette(QPalette(QColor(200, 200, 200)));
		//label->show();

		QBrush brush;
		brush.setColor(QColor(200,200,200));
		brush.setStyle(Qt::SolidPattern);
		p.setBrush(brush);

		p.drawRect(m_points.at(i).x(), m_points.at(i).y(), 100, 25);
		p.drawText(m_points.at(i).x()+ 10 , m_points.at(i).y()+20, m_expressionNames.at(i));

		p.drawEllipse(m_points.at(i).x() - 5, m_points.at(i).y() - 5, 10 ,10);
	}

	// draws cursor

	pen.setColor(QColor(0,255,0));
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(2);
	p.setPen(pen);

	QPoint cursor;
	cursor = getPointCoordinates(getParameterValue().toList());

	p.drawLine(cursor.x()-5, cursor.y(), cursor.x() + 5, cursor.y());
	p.drawLine(cursor.x(), cursor.y()-5, cursor.x(), cursor.y() +5);


	//QString mode = getFlag(QString("mode")).toString();
	//p.drawText(10,20,mode);
}


void universalGraph::mouseMoveEvent( QMouseEvent* event )
{
	QPoint point(event->pos().x(), event->pos().y());
	m_cursor = point;
	updateParameterValue(QVariant(getFloatCoordinates(point)));	
}

void universalGraph::mousePressEvent( QMouseEvent* event )
{
	QPoint point(event->pos().x(), event->pos().y());

	if(getFlag(QString("mode")).toString() == QString("add")){
		m_points.append(point);

		QList<QPoint> pointsFlags;

		if(checkFlag(QString("points"))){
			QList<QVariant> data = getFlag(QString("points")).toList();
			for(int i = 0; i < data.length(); ++i)
				pointsFlags.append(data.at(i).toPoint());
		}			

		pointsFlags.append(point);


		QString expressionName;

		if(checkFlag(QString("expression")))
			expressionName = getFlag(QString("expression")).toString();

		m_expressionNames.append(expressionName);

		QList<QVariant> output;


		for(int i = 0; i < pointsFlags.length(); ++i)
			output.append(QVariant(pointsFlags.at(i)));

		setFlag(QString("points"), QVariant(output));
		//drawBackgroundImage();
	}

	m_cursor = point;
	updateParameterValue(QVariant(getFloatCoordinates(point)));
}

//! 
//! Transfers the QPoint coordinats of the widget into a -1/1 system
//!
//! \param p The QPoint of the current position
//! \return a list of the coordinats as floats
//!
QList<QVariant> universalGraph::getFloatCoordinates(QPoint p)
{
	QList<QVariant> values;

	float x = ((1.0/250.0)*float(p.x())) -1.0;
	float y = ((-1.0/250.0)*float(p.y())) +1.0;

	values.append(QVariant(x));
	values.append(QVariant(y));

	return values;
}

//!
//! Transfers the float coordinates into QPoint coordinates
//!
//! \param point List of QVariants of float values
//! \return Coordinats of the point as QPoint
//!
QPoint universalGraph::getPointCoordinates(QList<QVariant> point)
{
	QPoint coordinates;

	if(point.length() < 2)
		return coordinates;

	if(point.at(0).type() != QVariant::Double || point.at(1).type() != QVariant::Double)
		return coordinates;

	int x = 250*(point.at(0).toDouble()) + 250;
	int y = -250*(point.at(1).toDouble()) + 250;

	coordinates = QPoint(x,y);

	return coordinates;
}



//!
//! Draws the background image
//!
void universalGraph::drawBackgroundImage()
{
	int zaehler = 0;

	// prepeare painter

	QPainter p(&bgIMG);

	QPen pen;
	pen.setColor(QColor(155,155,155));
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(2);

	QBrush brush;
	brush.setColor(QColor(255,255,255));
	brush.setStyle(Qt::SolidPattern);

	p.setBrush(brush);
	p.setPen(pen);

	// fill picture

	for(int spalte = 0; spalte < 500; spalte++){
			for(int zeile = 0; zeile < 500; zeile++){
				QList<float> abstaende = interpolation_algorithm_1(QPoint(spalte, zeile));

				int r = 0;
				int g = 0;
				int b = 0;

				for(int i = 0; i < m_points.length(); i++)
				{
					QColor c = WeightedListColor(i, abstaende.at(i));
					r += c.red();
					g += c.green();
					b += c.blue();
				}

				if(r  > 255)
					r = 255;

				if(g  > 255)
					g = 255;

				if(b  > 255)
					b = 255;

				if(r < 0)
					r = 0;

				if(g < 0)
					g = 0;

				if(b < 0)
					b = 0;

					pen.setColor(QColor(r,g,b,255));
					p.setPen(pen);
					p.drawPoint(spalte, zeile);
					
				zaehler++;

				if(zaehler > 250000)
					break;
			}
	}
}

//!
//! Returns the color of a point with a certain weight
//!
//! \param p Index of the point
//! \param w Weight of that point
//! \return the final color
//!
QColor universalGraph::WeightedListColor(int p, float w)
{
	int c = ListPositionToColorIndex(p);
	QColor color_base = m_colors.at(c);

	int r = float(color_base.red()) * w;
	int g = float(color_base.green()) * w;
	int b = float(color_base.blue()) * w;

	QColor output_color = QColor(r,g,b);
	return output_color;
}

//! 
//! Returns the index of a color according to the index of a point
//!
//! \param p Index of the point
//! \return Index of the color
//!
int universalGraph::ListPositionToColorIndex(int p)
{
	int color;
	int max = m_colors.length();
	color = p % max;
	return color;
}

//!
//! Calculates the weights of the points
//!
//! \param position List of float-values as the coordinates of the current point
//! \param dimensions Number of dimensions
//! \return A List of weights
//!
QList<float> universalGraph::interpolation_algorithm_1(QPoint position)
{
	QList<float> distances_inverted;

	float theSumOfAllFears = 0.0;	// The sum of all distances

	for(int i = 0; i < m_points.length(); ++i){
		//Calculate the distance to the point for each dimension

		float SquareSum = 0.0;

		SquareSum = pow(double(m_points.at(i).x()-position.x()), 2) + pow(double(m_points.at(i).y()-position.y()),2); 

		// Calculate the scalar distance 
		float distance_scalar = sqrt(SquareSum);

		// Inverses the distance (1/(x+1))
		float muliplication = distance_scalar * 100;
		float addition = muliplication + 1;
		float division = 1 / addition;

		distances_inverted.append(division);

		theSumOfAllFears += distances_inverted.at(i);
	} // End for

	// Normalize the sum to 1

	QList<float> distances_normalized;
	float factor = 1 / theSumOfAllFears;

	for(int i = 0; i < distances_inverted.length(); ++i){
		float final_value = distances_inverted.at(i) * factor;
		distances_normalized.append(final_value);
		///std::cout<<"errechneter Wert: "<<final_value<<std::endl;
	}

	return distances_normalized;
}


