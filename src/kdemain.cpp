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
    KAboutData aboutData( "marble", 
        "Marble Desktop Globe", "0.3.5", I18N_NOOP("A World Atlas."),
        KAboutData::License_LGPL, I18N_NOOP("(c) 2007") );
    aboutData.addAuthor("Torsten Rahn", "Original author and maintainer", "rahn@kde.org");
    aboutData.addAuthor("Inge Wallin", "co-maintainer", "inge@lysator.liu.se");
    aboutData.setHomepage("http://edu.kde.org/marble");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication app;

    MainWindow *window = new MainWindow();
    MarbleTest *marbleTest = new MarbleTest( window->marbleWidget() );

    window->resize(680, 640);
    window->show();

    // This should be reimplemented properly using KDE classes
    for ( int i = 1; i < argc; ++i ) {
        if ( strcmp( argv[ i ], "--timedemo" ) == 0 )
        {
            marbleTest->timeDemo();
            return 0;
        }
        else if ( QFile::exists( app.arguments().at( i ) ) )
            ( window->marbleControl() )->addPlaceMarkFile( argv[i] );
    }

    delete marbleTest;

    return app.exec();
}
