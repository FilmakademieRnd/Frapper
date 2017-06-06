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
//! \file "Files2PathVectorNode.cpp"
//! \brief Implementation file for Files2PathVectorNode class.
//!
//! \author     Jonas Trottnow <jtrottno@filmakademie.de>
//! \version    1.0
//! \date       16.12.2011 (last updated)
//!

#include "Files2PathVectorNode.h"

namespace Files2PathVectorNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the Files2PathVectorNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
Files2PathVectorNode::Files2PathVectorNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	// Connect Open Button to File Dialog
	setCommandFunction("Load image Sequence", SLOT(readFilePathes()));
}


//!
//! Destructor of the Files2PathVectorTimerNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
Files2PathVectorNode::~Files2PathVectorNode ()
{
}

void Files2PathVectorNode::readFilePathes()
{
	pathList = new QStringList(0);
	*pathList = QFileDialog::getOpenFileNames(
                         0,
                         "Select files to open",
                         "/home",
                         "Images (*.png *.tif *.jpg *.exr)");
	if(pathList->size() != 0)
	{
		setValue("First Image of Sequence", pathList->at(0));
		QString pathListString = "";
		for(int i = 0; i < pathList->size(); i++)
		{
			pathListString = pathListString+pathList->at(i)+";";
		}
		setValue("PathVector", pathListString);
	}
}

} // namespace Files2PathVector
