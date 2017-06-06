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
//! \file "OpenMvgFeatureFinder.h"
//! \brief Header file for OpenMvgFeatureFinder class.
//!
//! \author		Jonas Trottnow <jtrottno@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!
//!

#include "Log.h"
#include "Node.h"
#include "QProgressDialog"
#include "qstringlist.h"
#include "qvector.h"
#include "qdir.h"
#include "qtextstream.h"
#include "qfileinfo.h"
#include "SceneModel.h"
#include "qprocess.h"
#include "qcoreapplication.h"
#include "Log.h"

namespace OpenMvgFeatureFinder {
using namespace Frapper;

//!
//! Class representing OpenMvgFeatureFinder
//!
class OpenMvgFeatureFinder : public Node
{
    Q_OBJECT


public: /// constructors and destructors

    //!
    //! Constructor of the OpenMvgFeatureFinder class.
    //!
    //! \param name The name to give the new mesh node.
	//! \param parameterRoot The main Parameter Group of this Node
    //!
    OpenMvgFeatureFinder ( QString name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the OpenMvgFeatureFinder class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
	virtual ~OpenMvgFeatureFinder ();

	
private slots:

	//!
	//! Updates the output image
	//!
	void run();
	void writeLog();
	

private: //data
	QProcess *process;

};

} // namespace OpenMvgFeatureFinder
