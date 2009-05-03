//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include <KApplication>
#include <KLocale>
#include <KAboutData>
#include <KCmdLineArgs>

#include <QtCore/QFile>
 
#include "ControlView.h"
#include "KdeMainWindow.h"

#include "MarbleTest.h"

#ifdef STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif

using namespace Marble;

int main (int argc, char *argv[])
{
    KAboutData aboutData( "marble", 0, 
                          ki18n( "Marble Desktop Globe" ),
                          MARBLE_VERSION_STRING.toLatin1(), 
                          ki18n( "A World Atlas." ),
                          KAboutData::License_LGPL, ki18n( "(c) 2007, 2008, 2009" ),
                          KLocalizedString(),
                          "http://edu.kde.org/marble" );

    // Active Development Team of Marble 0.7
    aboutData.addAuthor( ki18n( "Torsten Rahn" ),
                         ki18n( "Core Developer and Original Author" ),
                         "rahn@kde.org" );
    aboutData.addAuthor( ki18n( "Patrick Spendrin" ),
                         ki18n( "Core Developer: KML and Windows support" ),
                         "pspendrin@gmail.com" );
    aboutData.addAuthor( ki18n( "Jens-Michael Hoffmann" ),
                         ki18n( "OpenStreetMap Integration, OSM Namefinder, Download Management" ),
                         "jensmh@gmx.de" );
    aboutData.addAuthor( ki18n( "Henry de Valence" ),
                         ki18n( "Core Developer: Marble Runners, World-Clock Plasmoid" ),
                         "hdevalence@gmail.com" );
    aboutData.addAuthor( ki18n( "Bastian Holst" ),
                         ki18n( "Online Services support" ),
                         "bastianholst@gmx.de" );
    aboutData.addAuthor( ki18n( "Pino Toscano" ),
                         ki18n( "Network plugins" ),
                         "pino@kde.org" );
    aboutData.addAuthor( ki18n( "Harshit Jain" ),
                         ki18n( "Planet filter" ),
                         "sonu.itbhu@googlemail.com" );
    aboutData.addAuthor( ki18n( "Simon Edwards" ),
                         ki18n( "Marble Python Bindings" ),
                         "simon@simonzone.com" );
    aboutData.addAuthor( ki18n( "Dennis Nienhüser" ),
                         ki18n( "Navigator Float Item" ),
                         "earthwings@gentoo.org" );
    aboutData.addAuthor( ki18n( "Magnus Valle" ),
                         ki18n( "Historical Maps" ),
                         "" );
    aboutData.addAuthor( ki18n( "Médéric Boquien" ),
                         ki18n( "Astronomical Observatories" ),
                         "mboquien@free.fr" );
    aboutData.addAuthor( ki18n( "Eckhart Wörner" ),
                         ki18n( "Bugfixes" ),
                         "kde@ewsoftware.de" );

    // Google Summer of Code
    aboutData.addAuthor( ki18n( "Patrick Spendrin" ),
                         ki18n( "Google Summer of Code 2008 Project:"
                                " Vector Tiles for Marble" ),
                         "pspendrin@gmail.com" );
    aboutData.addAuthor( ki18n( "Shashank Singh" ),
                         ki18n( "Google Summer of Code 2008 Project:"
                                " Panoramio / Wikipedia -photo support for Marble" ),
                         "shashank.personal@gmail.com" );
    aboutData.addAuthor( ki18n( "Carlos Licea" ),
                         ki18n( "Google Summer of Code 2007 Project:"
                                " Equirectangular Projection (\"Flat Map\")" ),
                         "carlos.licea@kdemail.net" );
    aboutData.addAuthor( ki18n( "Andrew Manson" ),
                         ki18n( "Google Summer of Code 2007 Project:"
                                " GPS Support for Marble" ),
                         "g.real.ate@gmail.com" );
    aboutData.addAuthor( ki18n( "Murad Tagirov" ),
                         ki18n( "Google Summer of Code 2007 Project:"
                                " KML Support for Marble" ),
                         "tmurad@gmail.com" );

    // Developers
    aboutData.addAuthor( ki18n( "Inge Wallin" ),
                         ki18n( "Core Developer and Co-Maintainer" ),
                         "inge@lysator.liu.se" );
    aboutData.addAuthor( ki18n( "Simon Schmeisser" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Claudiu Covaci" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "David Roberts" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Nikolas Zimmermann" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Jan Becker" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Stefan Asserhäll" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Laurent Montel" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Prashanth Udupa" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Anne-Marie Mahfouf" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Josef Spillner" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Frerich Raabe" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Frederik Gladhorn" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Fredrik Höglund" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Albert Astals Cid" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Thomas Zander" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Joseph Wenninger" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Kris Thomsen" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Daniel Molkentin" ),
                         ki18n( "Development & Patches" ));
    aboutData.addAuthor( ki18n( "Tim Sutton" ),
                         ki18n( "Platforms & Distributions" ));
    aboutData.addAuthor( ki18n( "Christian Ehrlicher" ),
                         ki18n( "Platforms & Distributions" ));
    aboutData.addAuthor( ki18n( "Ralf Habacker" ),
                         ki18n( "Platforms & Distributions" ));
    aboutData.addAuthor( ki18n( "Steffen Joeris" ),
                         ki18n( "Platforms & Distributions" ));
    aboutData.addAuthor( ki18n( "Marcus Czeslinski" ),
                         ki18n( "Platforms & Distributions" ));
    aboutData.addAuthor( ki18n( "Marcus D. Hanwell" ),
                         ki18n( "Platforms & Distributions" ));
    aboutData.addAuthor( ki18n( "Chitlesh Goorah" ),
                         ki18n( "Platforms & Distributions" ));
    aboutData.addAuthor( ki18n( "Nuno Pinheiro" ),
                         ki18n( "Artwork" ));
    aboutData.addAuthor( ki18n( "Torsten Rahn" ),
                         ki18n( "Artwork" ));

    // Credits
    aboutData.addCredit( ki18n( "Luis Silva" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Stefan Jordan" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Robert Scott" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Lubos Petrovic" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Benoit Sigoure" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Martin Konold" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Matthias Welwarsky" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Rainer Endres" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Ralf Gesellensetter" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "Tim Alder" ),
                         ki18n( "Various Suggestions & Testing" ));
    aboutData.addCredit( ki18n( "John Layt" ),
                         ki18n( "Special thanks for providing an"
                                " important source of inspiration by creating"
                                " Marble's predecessor \"Kartographer\"." ));

    KCmdLineArgs::init( argc, argv, &aboutData );

    
    KCmdLineOptions  options;
    options.add( "timedemo", ki18n( "Make a time measurement to check performance" ) );
    options.add( "gpsdemo", ki18n( "Check speed of gps drawing" ) );
    options.add( "fps", ki18n( "Show frame rate" ) );
    options.add( "enableCurrentLocation", ki18n( "Enable tab to show the current location" ) );
    options.add( "enableFileView",
                 ki18n( "Enable tab to see gpxFileView") );
    options.add( "tile-id", ki18n( "Show tile id's" ) );
    KCmdLineArgs::addCmdLineOptions( options );
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KApplication app;
    KGlobal::locale()->insertCatalog("marble_qt");

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
    window->show();

    if ( args->isSet( "timedemo" ) ) {
        window->resize(900, 640);
        MarbleTest test( window->marbleWidget() );
        test.timeDemo();
        return 0;
    }
    
    if ( args->isSet( "gpsdemo" ) ) {
        window->resize( 900, 640 );
        MarbleTest test( window->marbleWidget() );
        test.gpsDemo();
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

    if (args->isSet( "tile-id" ) ) {
	window->marbleControl()->marbleWidget()->setShowTileId(true);
    }

    // Read the files that are given on the command line.
    for ( int i = 0; i < args->count(); ++i ) {

        // FIXME: Use openUrl( args->url(i)) instead?
        if ( QFile::exists( args->arg( i ) ) && i != dataPathIndex )
            window->marbleControl()->addPlacemarkFile( args->arg( i ) );
    }

    return app.exec();
}
