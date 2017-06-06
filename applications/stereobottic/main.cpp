//!
//! \file "main.cpp"
//! \brief Main implementation file for aQtree application.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.05.2009 (last updated)
//!

#include "StereoBottic.h"
#include "Log.h"
#include <QFile>

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

    QString applicationName ("StereoBottic");

    QString aboutText (
        "<div align=\"center\">"
        "<a href=\"http://frapper.sourceforge.net\"></a><br/>"
        "<b>Version 1.1a</b><br/>"
        "<br/>"
        "Copyright &copy; 2009 Filmakademie Baden-Wuerttemberg. All rights reserved."
    );

    char* filename = "scenes/StereoBottic/StereoBottic.dae";
    if( QFile::exists(filename))
    {
        applicationName = "StereoBottic";
        argc = 2;
        argv[1] = filename;
    }

    StereoBottic *sb = new StereoBottic(organizationName, applicationName, argc, argv, aboutText);

    int result = sb->exec();
    delete sb;

    // free all resources from the log message handler
    Log::finalize();

    return result;
}
