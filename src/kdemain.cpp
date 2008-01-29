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
 
#include "ControlView.h"
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
                          ki18n( "Marble Desktop Globe" ),
                          "0.5.1 -- KDE 4.0 Branch", 
                          ki18n( "A World Atlas." ),
                          KAboutData::License_LGPL, ki18n( "(c) 2007" ),
                          KLocalizedString(),
                          "http://edu.kde.org/marble" );
    aboutData.addAuthor( ki18n( "Torsten Rahn" ),
                         ki18n( "Original Author, Developer and Maintainer" ),
                         "rahn@kde.org" );
    aboutData.addAuthor( ki18n( "Inge Wallin" ),
                         ki18n( "Co-maintainer and Architect" ),
                         "inge@lysator.liu.se" );

    KCmdLineArgs::init( argc, argv, &aboutData );

    
    KCmdLineOptions  options;
    options.add( "timedemo", ki18n( "Make a time measurement to check performance" ) );
    options.add( "gpsdemo", ki18n( "Check speed of gps drawing" ) );
    options.add( "fps", ki18n( "Show frame rate" ) );
    options.add( "enableCurrentLocation", ki18n( "Enable tab to show the current location" ) );
    options.add( "enableFileView", 
                 ki18n( "Enable tab to see gpxFileView") );
    KCmdLineArgs::addCmdLineOptions( options );
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KApplication app;

    QString  marbleDataPath;
    int      dataPathIndex = 0;

    for ( int i = 1; i < argc - 1; ++i ) {
        if ( strcmp( argv[ i ], "--marbleDataPath" ) == 0 ) {
            dataPathIndex = i + 1;
            marbleDataPath = argv[ dataPathIndex ];
        }
    }

    MainWindow *window = new MainWindow();
    window->setAttribute( Qt::WA_DeleteOnClose, true );

    MarbleTest *marbleTest = new MarbleTest( window->marbleWidget() );

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
    
    if ( args->isSet( "fps" ) ) {
        window->marbleControl()->marbleWidget()->setShowFrameRate( true );
    }

    if ( args->isSet( "enableCurrentLocation" ) ) {
        window->marbleControl()->setCurrentLocationTabShown(true);
    }
    
    if ( args->isSet( "enableFileView" ) ) {
        window->marbleControl()->setFileViewTabShown(true);
    }

    // Read the files that are given on the command line.
    for ( int i = 0; i < args->count(); ++i ) {

        // FIXME: Use openUrl( args->url(i)) instead?
        if ( QFile::exists( args->arg( i ) ) && i != dataPathIndex )
            window->marbleControl()->addPlaceMarkFile( args->arg( i ) );
    }

    delete marbleTest;

    return app.exec();
}
