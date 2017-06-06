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
//! \file "PythonNode.cpp"
//! \brief Implementation file for PythonNode class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    1.0
//! \date       18.05.2009 (last updated)
//!

#include "PythonNode.h"
#include <iostream>
#include <boost/python.hpp>
#include <QtCore/QDir>

#define str(s) #s

static QString log_buffer("");

void write_to_frapper_log( const char* text ) {

	QString msg(text);
	QStringList lines = msg.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);

	// write all lines except the last to frapper log, starting with the previously buffered output
	for( size_t i = 0; i< lines.size()-1; i++){		
		if( i==0) {
			Frapper::Log::info( log_buffer, "PythonNode::execute");
			log_buffer = "";
		}
		Frapper::Log::info( lines[i], "PythonNode::execute");
	}

	// store last line in buffer if fragment
	if( lines.size() > 0 && !msg.endsWith("\n")) {
		log_buffer += lines.last();
	} else {
		Frapper::Log::info( lines.last(), "PythonNode::execute");
	}
}

BOOST_PYTHON_MODULE(pythonnode)
{
	boost::python::def("write", write_to_frapper_log);
}

namespace PythonNode {
using namespace Frapper;

///
/// Constructors and Destructors
///


//!
//! Constructor of the PythonNode class.
//!
//! \param name The name for the new node.
//! \param parameterRoot A copy of the parameter tree specific for the type of the node.
//!
PythonNode::PythonNode ( const QString &name, ParameterGroup *parameterRoot ) :
    Node(name, parameterRoot)
{
	setCommandFunction("Execute Script", SLOT(runScript()));
}


//!
//! Destructor of the PythonNode class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
PythonNode::~PythonNode ()
{
}

void PythonNode::runScript()
{
	//StreamListener s(std::cout);
	
	Py_Initialize();
		
	QString script = getStringValue("Script");
	QString path = QDir::currentPath()+"/plugins/nodes";
	QString setpath = "os.environ['PATH'] = '" + path + "';os.environ['PATH']";

	PyRun_SimpleString( "import os");
	PyRun_SimpleString( setpath.toStdString().c_str());
	PyRun_SimpleString( "import pythonnode");	
	PyRun_SimpleString( "import sys");	
	PyRun_SimpleString( "sys.stdout = FrapperLog");	
	PyRun_SimpleString( "sys.stderr = FrapperLog");	
	PyRun_SimpleString( script.toStdString().c_str());
	Py_Finalize();
}

} // namespace PythonNode 
