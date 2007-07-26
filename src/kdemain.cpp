//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include <KApplication>
#include <KLocale>
#include <KAboutData>
#include <KCmdLineArgs>

#include <QtCore/QFile>
 
#include "KdeMainWindow.h"

#include "MarbleTest.h"

#if STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif
 
int main (int argc, char *argv[])
{
    KAboutData aboutData( "marble", 0, 
        ki18n("Marble Desktop Globe"), "0.3.8", ki18n("A World Atlas."),
        KAboutData::License_LGPL, ki18n("(c) 2007") );
    aboutData.addAuthor(ki18n("Torsten Rahn"), ki18n("Original author and maintainer"), "rahn@kde.org");
    aboutData.addAuthor(ki18n("Inge Wallin"), ki18n("co-maintainer"), "inge@lysator.liu.se");
    aboutData.setHomepage("http://edu.kde.org/marble");

    KCmdLineArgs::init( argc, argv, &aboutData );

    
    KCmdLineOptions  options;
    options.add( "timedemo", ki18n( "Make a time measurement to check performance" ) );
    options.add( "gpsdemo", ki18n( "Check speed of gps drawing" ) );
    options.add( "enableCurrentLocation", ki18n("foo bar"));
    KCmdLineArgs::addCmdLineOptions( options );
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KApplication app;

    MainWindow *window = new MainWindow();
    MarbleTest *marbleTest = new MarbleTest( window->marbleWidget() );

    window->resize(680, 640);
    window->show();

    if ( args->isSet( "timedemo" ) ) {
        window->resize(900, 640);
        marbleTest->timeDemo();
        return 0;
    }
    
    if ( args->isSet( "gpsdemo" ) ) {
        window->resize( 900, 640 );
        marbleTest->gpsDemo();
        return 0;
    }
    
    if ( args->isSet( "enableCurrentLocation" ) ) {
        window->marbleControl()->setCurrentLocationTabShown(true);
    }

    // Read the files that are given on the command line.
    // FIXME: What should the '1' below really be?
    // Command line arguments, i.e. files to open
    for (int i = 0; i < args->count(); i++) {

        // FIXME: Use openUrl( args->url(i)) instead?
        if ( QFile::exists( args->arg( i ) ) )
            ( window->marbleControl() )->addPlaceMarkFile( args->arg( i ) );
    }

    delete marbleTest;

    return app.exec();
}
