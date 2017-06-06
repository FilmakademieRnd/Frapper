//!
//! \file "main.cpp"
//! \brief Main implementation file for aQtree application.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "Application.h"
#include "Log.h"
#include <QtCore/QFile>

#ifdef _DEBUG
    #include "instancecountermacros.h"
    #include "Panel.h"
    #include "NodeType.h"
    #include "PinGraphicsItem.h"
    #include "NodeGraphicsItem.h"
    #include "OgreContainer.h"
#endif

#ifdef HIDE_CONSOLE
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

//!
//! The application's entry point.
//!
//! \param argc     the number of parameters passed to the program
//! \param argv     the list of parameters passed to the program
//!
int main ( int argc, char **argv )
{
    // initialize the log message handler
    Log::initialize(true);

    // set application strings
    QString organizationName ("Filmakademie Baden-Württemberg");
#ifndef _DEBUG
    QString applicationName ("Frapper Demo");
#else
    QString applicationName ("Frapper Demo - DEBUG");
#endif

    QString aboutText (
        "<div align=\"center\">"
        "<a href=\"http://frapper.sourceforge.net\"></a><br>"
        "<b>Version 1.0 Pre-Alpha</b><br>"
        "<br>"
        "Copyright &copy; 2009 Filmakademie Baden-Wuerttemberg. All rights reserved."
    );


    // create, run and destroy the application
	int result = 0;
try{
    Application *app = new Application(organizationName, applicationName, argc, argv, aboutText);
    result = app->exec();
	delete app;
	app = 0;
} catch ( std::exception& e) {
	std::cout << "An unhandled exception was thrown:" << std::endl;
	std::cout << e.what() << std::endl;
}

#ifdef _DEBUG
    SHOW_INSTANCE_COUNTER(Panel)
    SHOW_INSTANCE_COUNTER(PinGraphicsItem)
    SHOW_INSTANCE_COUNTER(NodeGraphicsItem)
	SHOW_INSTANCE_COUNTER(Parameter)
    SHOW_INSTANCE_COUNTER(ParameterGroup)
    SHOW_INSTANCE_COUNTER(Connection)
    SHOW_INSTANCE_COUNTER(Node)
    SHOW_INSTANCE_COUNTER(NodeType)
    SHOW_INSTANCE_COUNTER(OgreContainer)
#endif

    // free all resources from the log message handler
    Log::finalize();

    return result;
}
