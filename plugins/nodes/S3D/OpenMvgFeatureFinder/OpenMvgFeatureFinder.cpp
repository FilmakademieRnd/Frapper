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
//! \file "OpenMvgFeatureFinder.cpp"
//! \brief Camera Input
//!
//! \author		Jonas Trottnow <jtrottno@filmakademie.de>
//! \version	1.0
//! \date		28.04.2014 (last updated)
//!

#include "OpenMvgFeatureFinder.h"
#include "qmessagebox.h"

namespace OpenMvgFeatureFinder {
using namespace Frapper;

///
/// Public Constructors
///

//!
//! Constructor of the OpenMvgFeatureFinder class.
//!
//! \param name The name to give the new mesh node.
//!
OpenMvgFeatureFinder::OpenMvgFeatureFinder ( QString name, ParameterGroup *parameterRoot) :
    Node(name, parameterRoot)
{
	QDir dir; 
	QString appPath = dir.absolutePath();
	if(dir.exists("OpenMVG"))
	{
		dir.cd("OpenMVG");
		if(dir.entryList().contains("openMVG_main_computeMatches_OpenCV.exe"))
		{
			setValue("exe Location",dir.absolutePath(),false);
		}
	}
	setCommandFunction("Calculate!", SLOT(run()));
	process = new QProcess(this);
	QObject::connect(process,SIGNAL(readyReadStandardOutput()), this, SLOT(writeLog()));
}

//!
//! Destructor of the OpenMvgFeatureFinder class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
OpenMvgFeatureFinder::~OpenMvgFeatureFinder ()
{
}

///
/// Private Slots
///


void OpenMvgFeatureFinder::run()
{
	QFile FunctionExe(getValue("exe Location",true).toString()+"/openMVG_main_computeMatches_OpenCV.exe");
	if(!FunctionExe.exists()) 
	{
		setValue("exe Location","NOT FOUND",true);
		return;
	}
	QString workingPath = SceneModel::getWorkingDirectory();
	QDir workingDir(workingPath);
	if(!workingDir.exists(getValue("Image Location",true).toString()) || !QDir::isAbsolutePath(getValue("Image Location",true).toString()))
	{
		setValue("Image Location","NOT FOUND",true);
		return;
	}
	if(!workingDir.exists(getValue("Matches Output Path",true).toString()) || !QDir::isAbsolutePath(getValue("Matches Output Path",true).toString()))
	{
		setValue("Matches Output Path","NOT FOUND",true);
		return;
	}

	QString imageLocation = getValue("Image Location",true).toString();
	if(QDir::isRelativePath(imageLocation)) 
	{
		workingDir.cd(imageLocation);
		imageLocation = workingDir.absolutePath();
	}
	QDir imageDir(imageLocation);
	imageDir.setNameFilters(QStringList()<<"*.png"<<"*.jpg"<<"*.tif"<<"*.tiff");
	QStringList images = imageDir.entryList();
	if(images.size() < 1)
	{
		setValue("Image Location","EMPTY",false);
		return;
	}

	std::string imgPath = QString(imageLocation+"/"+images.at(0)).toLatin1().data();

	// obtain the OGRE resource group manager
	Ogre::ResourceGroupManager *resourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();
    if (!resourceGroupManager) {
        Log::error("The OGRE resource group manager could not be obtained.", "ImageFileNode::processOutputImage");
        return;
 }

	if (!resourceGroupManager->resourceExists(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, imgPath))
		resourceGroupManager->addResourceLocation(imageLocation.toLatin1().data(), "FileSystem");

	Ogre::Image *iImage;
	iImage = new Ogre::Image();
	iImage->load(imgPath, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	QString matchesLocation = getValue("Matches Output Path",true).toString();
	if(QDir::isRelativePath(matchesLocation)) 
	{
		workingDir.cd(matchesLocation);
		matchesLocation = workingDir.absolutePath();
	}

	int width = iImage->getWidth();
	int height = iImage->getHeight();

	QFile imageList(matchesLocation+"/"+"lists.txt");
	if (imageList.open(QIODevice::ReadWrite)) {
        QTextStream stream(&imageList);
		for(int i = 0; i < images.size(); i++)
		{
			stream << images.at(i)+";"+QString::number(width)+";"+QString::number(height)+";"+getValue("focal length (pixel)",true).toString()+";0;"+QString::number(width/2.0)+";0;"+getValue("focal length (pixel)",true).toString()+";"+QString::number(height/2.0)+";0;0;1" << endl;
		}
    }
	imageList.close();

	QString program = getValue("exe Location",true).toString()+"/openMVG_main_computeMatches_OpenCV.exe";
	QStringList arguments;
	arguments << "-g" << "e"
			  << "-r" << "0.8"
			  << "-oct" << QString::number(getValue("BRISK Octaves",true).toInt())
			  << "-b" << QString::number(getValue("BRISK Treshhold",true).toInt())
			  << "-i" << imageLocation
			  << "-o" << matchesLocation;

	QProgressDialog progress("Calculating Features & Matches...", "Abort", 0, 0, 0);

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

void OpenMvgFeatureFinder::writeLog()
{
	Log::info(process->readLine(),"OpenMVG Matcher");
}

} // namespace OpenMvgFeatureFinder
