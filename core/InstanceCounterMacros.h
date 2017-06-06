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
//! \file "instancecountermacros.h"
//! \brief Header file containing instance counter debugging macros.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \version    1.0
//! \date       07.04.2009 (last updated)
//!

#ifndef INSTANCE_COUNTER_MACROS_H
#define INSTANCE_COUNTER_MACROS_H

#include "Log.h"


//!
//! Macro definition that adds static member variables to a class to keep track
//! of the number of instances created from this class.
//!
//! Variables added to a class using this macro will have to be initialized in
//! the implementation file of the class using the INIT_INSTANCE_COUNTER macro.
//!
//! \see INIT_INSTANCE_COUNTER
//!
#define ADD_INSTANCE_COUNTER \
public:\
    static int s_numberOfInstances;\
    static int s_totalNumberOfInstances;\
private:


//!
//! Macro definition that initializes the static member variables added to a
//! class using the ADD_INSTANCE_COUNTER macro.
//!
//! To be added in the implementation file of the class.
//!
//! \param className The name of the class to which the static member variables have been added.
//! \see ADD_INSTANCE_COUNTER
//!
#define INIT_INSTANCE_COUNTER(className) \
    int className::s_numberOfInstances = 0;\
    int className::s_totalNumberOfInstances = 0;


//!
//! Macro definition that updates the static member variables added to a class
//! using the ADD_INSTANCE_COUNTER macro.
//!
//! Should be called when an instance of this class is created (e.g. by adding
//! it to constructors of the class).
//!
//! \see DEC_INSTANCE_COUNTER
//!
#define INC_INSTANCE_COUNTER \
    ++s_numberOfInstances;\
    ++s_totalNumberOfInstances;


//!
//! Macro definition that updates the static member variables added to a class
//! using the ADD_INSTANCE_COUNTER macro.
//!
//! Should be called when an instance of this class is destroyed (e.g. by
//! adding it to destructors of the class).
//!
//! \see INC_INSTANCE_COUNTER
//!
#define DEC_INSTANCE_COUNTER \
    --s_numberOfInstances;


//!
//! Macro definition that displays the statistics about the number of instances
//! created from a class to which instance counter member variables have been
//! added using the ADD_INSTANCE_COUNTER macro.
//!
//! \param className The name of the class to which the static member variables have been added.
//! \see ADD_INSTANCE_COUNTER
//!
#define SHOW_INSTANCE_COUNTER(className) \
    Log::debug(QString("%1 of %2 instances of class " #className " left").arg(className::s_numberOfInstances).arg(className::s_totalNumberOfInstances), "main");


#endif
