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
//! \file "DoubleSlider.h"
//! \brief Header file for DoubleSlider class.
//!
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       27.03.2009 (last updated)
//!

#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include "FrapperPrerequisites.h"
#include <QSlider>


namespace Frapper {

//!
//! Class representing a slider for setting double values.
//!
//! <div align="center"><b>Inheritance Diagram</b></div>
//!
//! \dot
//!   digraph DoubleSlider {
//!     node [fontname="FreeSans",fontsize="10",shape="box",height=0.2,width=0.4,fontname="FreeSans",color="black",style="filled",fillcolor="white"];
//!     edge [fontname="FreeSans",fontsize="10",labelfontname="FreeSans",labelfontsize="10",dir=back,style="solid",color="midnightblue"];
//!
//!     DoubleSlider [label="DoubleSlider",fillcolor="grey75"];
//!     QSlider -> DoubleSlider;
//!     QSlider [label="QSlider",fontcolor="white",fillcolor="#66b036",URL="http://doc.qtsoftware.com/4.5/qslider.html"];
//!   }
//! \enddot
//! <div><center>[<a href="graph_legend.html">legend</a>]</center></div>
//!
class FRAPPER_GUI_EXPORT DoubleSlider : public QSlider
{
    Q_OBJECT

public:
    DoubleSlider(QWidget *parent=0);
    ~DoubleSlider();

    void setDoubleMinimum(double min);
    double doubleMinimum() const;

    void setDoubleMaximum(double mox);
    double doubleMaximum() const;

    //void setRange(double min, double max);

    void setDoubleSingleStep(double step);
    double doubleSingleStep() const;

    double doubleValue();

public slots:
    void setDoubleValue(double value);
    void emitDoubleValueChanged(int v);
    void emitDoubleSliderMoved(int v);

signals:
    void doubleValueChanged(double value);
    void doubleSliderMoved(double position);

    //void rangeChanged(double min, double max);

    //void actionTriggered(double action);

protected: // methods

    double convertToDouble(int value);
    int convertToInt(double value);
    int round(double value);

protected: // variables

    double m_value;
    double m_max;
    double m_min;
    double m_step;

};

} // end namespace Frapper

#endif
