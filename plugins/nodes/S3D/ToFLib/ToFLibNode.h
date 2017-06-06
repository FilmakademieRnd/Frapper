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
//! \file "ToFLibNode.h"
//! \brief Header file for ToFLibNode class.
//!
//! \author		Michael Bussler <michael.bussler@filmakademie.de>
//! \author     Simon Spielmann <simon.spielmann@filmakademie.de>
//! \version	1.0
//! \date		26.03.2014 (last updated)
//!
//!
#ifndef ToFLibNODE_H
#define ToFLibNODE_H

#include "Log.h"
#include "OgreTools.h"
#include "ImageNode.h"

namespace oxo {
	struct OptionsBase;
}

namespace ToFLibNode {
using namespace Frapper;

class ToFLibRenderer;

//!
//! Class representing ToFLib
//!
class ToFLibNode : public ImageNode
{
    Q_OBJECT

public: /// constructors and destructors

    //!
    //! Constructor of the ToFLibNode class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    ToFLibNode ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the ToFLibNode class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual ~ToFLibNode ();
	
private slots:

	//!
	//! Updates the output image
	//!
	void processOutputImage();

	void initOptions();

	void parametersFromOption( oxo::OptionsBase* options, ParameterGroup* algoOptionsGroup );

private: // functions

	void setOptionsFromParameter( QString optionsGroupName, oxo::OptionsBase* optionsPtr );

	Frapper::ParameterGroup* getOrCreateParameterGroup(QString name);

private: //data

	ToFLibRenderer* m_tofRenderer;
};

} // namespace ToFLibNode

#endif
