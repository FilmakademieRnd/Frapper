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
//! \file "Controller.h"
//! \brief Header file for Controller class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "FrapperPrerequisites.h"
#include "ViewPanel.h"
#include "SceneModel.h"

namespace Frapper {

//!
//! Class for setting up model/view connections.
//!
class FRAPPER_GUI_EXPORT Controller
{

public: // constructors and destructors

    //!
    //! Constructor of the Controller class.
    //!
    //! \param model The model to use for the views.
    //!
    Controller ( SceneModel *model );

public: // functions

    //!
    //! Registers the given view panel to display the registered model.
    //!
    //! \param viewPanel The view panel to display the registered model.
    //!
    virtual void registerViewPanel ( ViewPanel *viewPanel );

protected: // data

    //!
    //! A pointer to the scene model.
    //!
    SceneModel *m_sceneModel;

};

} // end namespace Frapper

#endif
