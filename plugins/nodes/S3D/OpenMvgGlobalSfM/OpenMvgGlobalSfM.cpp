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
//! \file "OpenMvgGlobalSfM.cpp"
//! \brief Camera Input
//!
//! \author		Jonas Trottnow <jtrottno@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!

#include "OpenMvgGlobalSfM.h"
#include "qmessagebox.h"

namespace OpenMvgGlobalSfM {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the OpenMvgGlobalSfM class.
//!
//! \param name The name to give the new mesh node.
//!
OpenMvgGlobalSfM::OpenMvgGlobalSfM ( QString name, ParameterGroup *parameterRoot) :
    Node(name, parameterRoot)
{
	QDir dir; 
	QString appPath = dir.absolutePath();
	if(dir.exists("OpenMVG"))
	{
		dir.cd("OpenMVG");
		if(dir.entryList().contains("openMVG_main_computeMatches_OpenCV.exe"))
		{
			setValue("exe Location",dir.absolutePath(),true);
		}
	}
	setCommandFunction("Calculate!", SLOT(run()));

	process = new QProcess(this);
	QObject::connect(process,SIGNAL(readyReadStandardOutput()), this, SLOT(writeLog()));
}

//!
//! Destructor of the OpenMvgGlobalSfM class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
OpenMvgGlobalSfM::~OpenMvgGlobalSfM ()
{
}

///
/// Private Slots
///


void OpenMvgGlobalSfM::run()
{
	QFile FunctionExe(getValue("exe Location",true).toString()+"/openMVG_main_GlobalSfM.exe");
	if(!FunctionExe.exists()) 
	{
		setValue("exe Location","NOT FOUND",true);
		return;
	}
	QString workingPath = SceneModel::getWorkingDirectory();
	QDir workingDir(workingPath);
	if(!workingDir.exists(getValue("Output Path",true).toString()) || !QDir::isAbsolutePath(getValue("Output Path",true).toString()))
	{
		setValue("Output Path","NOT FOUND",true);
		return;
	}
	if(!workingDir.exists(getValue("Image Location",true).toString()) || !QDir::isAbsolutePath(getValue("Image Location",true).toString()))
	{
		setValue("Image Location","NOT FOUND",true);
		return;
	}
	if(!workingDir.exists(getValue("Matches Path",true).toString()) || !QDir::isAbsolutePath(getValue("Matches Path",true).toString()))
	{
		setValue("Matches Path","NOT FOUND",true);
		return;
	}
	QString program = getValue("exe Location",true).toString()+"/openMVG_main_GlobalSfM.exe";

	QString outputLocation = getValue("Output Path",true).toString();
	if(QDir::isRelativePath(outputLocation)) 
	{
		workingDir.cd(outputLocation);
		outputLocation = workingDir.absolutePath();
	}

	QString imagesLocation = getValue("Image Location",true).toString();
	if(QDir::isRelativePath(imagesLocation)) 
	{
		workingDir.cd(imagesLocation);
		imagesLocation = workingDir.absolutePath();
	}

	QString matchesLocation = getValue("Matches Path",true).toString();
	if(QDir::isRelativePath(matchesLocation)) 
	{
		workingDir.cd(matchesLocation);
		matchesLocation = workingDir.absolutePath();
	}


	QStringList arguments;
	arguments << "-o" << outputLocation
			  << "-i" << imagesLocation
			  << "-m" << matchesLocation
			  << "-c" << "1";

	QProgressDialog progress("Calculating globalSfM...", "Abort", 0, 0, 0);
	
	QTime time;
	time.start();

	process->start(program,arguments);
	while( process->state() != 0 ) 
	{
		QCoreApplication::processEvents();
		if( time.elapsed() > 1 ) 
		{
			if (progress.wasCanceled())
			{
				process->kill();
			}
			progress.show();
		}
	}

}// run

void OpenMvgGlobalSfM::writeLog()
{
	Log::info(process->readLine(),"OpenMVG globalSfM");
}


} // namespace OpenMvgGlobalSfM
