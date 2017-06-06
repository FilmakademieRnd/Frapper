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
//! \file "CopyHandler.h"
//! \brief Header file for CopyHandler interface class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    0.11
//! \date       12.02.2009 (last updated)
//!

#ifndef COPYHANDLER_H
#define COPYHANDLER_H

namespace Frapper {

//!
//! Interface for classes to provide a custom copy function.
//!
class CopyHandler
{

public: // functions

    //!
    //! Copies the currently selected (or all) objects to the clipboard.
    //!
    virtual void copy () = 0;

};

} // end namespace Frapper

#endif
