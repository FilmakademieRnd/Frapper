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
//! \file "BackDropNode.cpp"
//! \brief Implementation file for BackDropNode class.
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version    1.0
//! \date       06.03.2014 (last updated)

#include "BackDropNode.h"

namespace Frapper {

///
/// Constructors and Destructors
///

//!
//! Constructor of the BackDropNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
BackDropNode::BackDropNode ( const QString &name, ParameterGroup *parameterRoot, const QPointF position /* = 0.0 */, const qreal width /* = 200 */, const qreal height /* = 200 */, const QColor backgroundColor /* = QColor(48,48,48) */, const QColor textColor /* = QColor::white */, const QString text /* = "" */, const int textSize /* = 11 */, const QString head /* = "" */, const int headSize /* = 22 */, const bool lockChange /* = false */ ) :
    Node(name, parameterRoot)
{	
	NumberParameter *widthParameter = new NumberParameter("Width", Parameter::T_UnsignedInt, 0.0);
	widthParameter->setDefaultValue(0.0);
	widthParameter->setVisible(false);
	widthParameter->setValue(width);

	NumberParameter *heightParameter = new NumberParameter("Height", Parameter::T_UnsignedInt, 0.0);
	heightParameter->setDefaultValue(0.0);
	heightParameter->setVisible(false);
	heightParameter->setValue(height);

	Parameter *lockParameter = new Parameter("Lock Changes", Parameter::T_Bool, false);
	lockParameter->setDefaultValue(false);
	lockParameter->setValue(lockChange);

	Parameter *backgroundColorParameter = new Parameter("Background Color", Parameter::T_Color, QColor(0, 0, 0));
	backgroundColorParameter->setDefaultValue(QColor(0, 0, 0));
	backgroundColorParameter->setValue(backgroundColor);

	Parameter *textColorParameter = new Parameter("Text Color", Parameter::T_Color, QColor(0, 0, 0));
	textColorParameter->setDefaultValue(QColor(0, 0, 0));
	textColorParameter->setValue(textColor);
	
	Parameter *headParameter = new Parameter("Headline", Parameter::T_String, "...");
	headParameter->setDefaultValue("...");
	headParameter->setValue(head);

	Parameter *textParameter = new Parameter("Description", Parameter::T_String, "...");
	textParameter->setDefaultValue("...");
	textParameter->setValue(text);

	NumberParameter *headSizeParameter = new NumberParameter("Headline Size", Parameter::T_Int, 1);
	headSizeParameter->setDefaultValue(1);
	headSizeParameter->setMaxValue(250);
	headSizeParameter->setMinValue(1);
	headSizeParameter->setValue(headSize);

	NumberParameter *textSizeParameter = new NumberParameter("Description Size", Parameter::T_Int, 1);
	textSizeParameter->setDefaultValue(1);
	textSizeParameter->setMaxValue(250);
	textSizeParameter->setMinValue(1);
	textSizeParameter->setValue(textSize);
	
	ParameterGroup* root = getParameterRoot();

	root->addParameter(widthParameter);
	root->addParameter(heightParameter);
	root->addParameter(lockParameter);
	root->addParameter(headParameter);
	root->addParameter(textParameter);
	root->addParameter(headSizeParameter);
	root->addParameter(textSizeParameter);
	root->addParameter(backgroundColorParameter);
	root->addParameter(textColorParameter);

	setChangeFunction("Lock Changes", SLOT(lockParameter()));
	setChangeFunction("Background Color", SLOT(drawGraphics()));
	setChangeFunction("Text Color", SLOT(drawGraphics()));
	setChangeFunction("Headline", SLOT(drawGraphics()));
	setChangeFunction("Description", SLOT(drawGraphics()));
	setChangeFunction("Headline Size", SLOT(drawGraphics()));
	setChangeFunction("Description Size", SLOT(drawGraphics()));
}


void BackDropNode::lockParameter()
{
	const bool lock = this->getBoolValue("Lock Changes");
	getParameter("Headline")->setReadOnly(lock);
	getParameter("Description")->setReadOnly(lock);
	getParameter("Headline Size")->setReadOnly(lock);
	getParameter("Description Size")->setReadOnly(lock);
	getParameter("Background Color")->setReadOnly(lock);
	getParameter("Text Color")->setReadOnly(lock);

	forcePanelUpdate();
} 

void BackDropNode::drawGraphics()
{
	emit graphicDrawSignal();
}

//!
//! Destructor of the BackDropNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
BackDropNode::~BackDropNode ()
{
}

} // namespace Frapper 
