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
//! \file "ParameterTabPage.h"
//! \brief Header file for ParameterTabPage class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    0.81
//! \date       12.02.2009 (last updated)
//!

#ifndef PARAMETERTABPAGE_H
#define PARAMETERTABPAGE_H

#include "FrapperPrerequisites.h"
#include <QWidget>
#include "ParameterGroup.h"


namespace Frapper {

//!
//! Class representing a tab page containing widgets for editing parameters of
//! a node.
//!
class FRAPPER_GUI_EXPORT ParameterTabPage : public QWidget
{

public: // constructors and destructors

    //!
    //! Constructor of the ParameterTabPage class.
    //!
    //! \param parent The parent widget the created instance will be a child of.
    //! \param parameterGroup The group of parameters represented by the tab page.
    //!
    ParameterTabPage ( QWidget *parent, ParameterGroup *parameterGroup, bool advanced = false );

    //!
    //! Destructor of the ParameterTabPage class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~ParameterTabPage ();

public: // functions

    //!
    //! Returns the group of parameters the tab page represents.
    //!
    //! \return The group of parameters the tab page represents.
    //!
    ParameterGroup * getParameterGroup ();

    //!
    //! Returns whether the widgets for editing the parameters in the parameter
    //! group represented by the page have been created.
    //!
    //! \return True if the editing widgets have been created, otherwise False.
    //!
    bool isLoaded () const;

    //!
    //! Sets the flag that states whether widgets for editing the parameters in
    //! the parameter group have been created for the page to True.
    //!
    void setLoaded ();

	//!
    //! Returns whether the shoud be shown in advance tab or not.
    //!
    //! \return True if the editing widgets have been created, otherwise False.
    //!
    bool isAdvanced () const;

private: // data

    //!
    //! The group of parameters the tab page represents.
    //!
    ParameterGroup *m_parameterGroup;

    //!
    //! Flag that states whether widgets for editing the parameters in the
    //! parameter group have been created for the page.
    //!
    bool m_isLoaded;

	//!
    //! Flag that states whether the page should be shown in advanced
    //! parameter tab.
 	//!
	bool m_advanced;

};

} // end namespace Frapper

#endif
