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
//! \file "DoubleSlider.cpp"
//! \brief Implementation file for DoubleSlider class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       23.06.2009 (last updated)
//!

#include "DoubleSlider.h"
#include <math.h>
#include <iostream>


namespace Frapper {

//void DoubleSlider::setRange(int min, int max)
//{
//}


DoubleSlider::DoubleSlider ( QWidget *parent ) :
    QSlider(parent),
    m_value(0.0)
{
    setSingleStep(1);
    setDoubleMinimum(0.0);
    setDoubleMaximum(100.0);
    setDoubleSingleStep(1.0);
    setValue(convertToInt(m_value));

    connect(this, SIGNAL(valueChanged(int)), SLOT(emitDoubleValueChanged(int)));
    connect(this, SIGNAL(sliderMoved(int)), SLOT(emitDoubleSliderMoved(int)));
}


DoubleSlider::~DoubleSlider ()
{
}


void DoubleSlider::setDoubleMinimum ( double min )
{
    m_min = min;
    setMinimum(0);
    setValue(convertToInt(m_value));
}


double DoubleSlider::doubleMinimum () const
{
    return m_min;
}


void DoubleSlider::setDoubleMaximum ( double max )
{
    m_max = max;
    setMaximum(round((m_max - m_min) / m_step));
    setValue(convertToInt(m_value));
}


double DoubleSlider::doubleMaximum () const
{
    return m_max;
}


void DoubleSlider::setDoubleSingleStep ( double step )
{
    m_step = step;
    setMaximum(round((m_max - m_min) / m_step));
    setValue(convertToInt(m_value));
}


double DoubleSlider::doubleSingleStep () const
{
    return m_step;
}


double DoubleSlider::doubleValue()
{
    return convertToDouble(value());
}


void DoubleSlider::setDoubleValue ( double value )
{
    if (m_value != value) {
        m_value = value;
        setValue(convertToInt(m_value));
        emit doubleValueChanged(m_value);
    }
}


void DoubleSlider::emitDoubleValueChanged ( int value )
{
    m_value = convertToDouble(value);
    emit doubleValueChanged(convertToDouble(value));
}


void DoubleSlider::emitDoubleSliderMoved ( int value )
{
    emit doubleSliderMoved(convertToDouble(value));
}


double DoubleSlider::convertToDouble ( int value )
{
    double doubleValue = m_min + value * m_step;
    if (doubleValue > m_max)
        doubleValue = m_max;
    else if (doubleValue < m_min)
        doubleValue = m_min;
    return doubleValue;
}


int DoubleSlider::convertToInt ( double value )
{
    int intValue = round((value - m_min) / m_step);
    if (intValue > maximum())
        intValue = maximum();
    else if (intValue < minimum())
        intValue = minimum();
    return intValue;
}


int DoubleSlider::round ( double value )
{
    int intValue;
    if (value - floor(value) >= 0.5)
        intValue = ceil(value);
    else
        intValue = floor(value);
    return intValue;
}

} // end namespace Frapper