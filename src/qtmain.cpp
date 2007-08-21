//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include <QtGui/QApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "QtMainWindow.h"

#include "MarbleTest.h"

#if STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif

#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // For non static builds on mac and win
    // we need to be sure we can find the qt image
    // plugins. In mac be sure to look in the
    // application bundle...
#ifdef Q_WS_WIN
    QApplication::addLibraryPath( QApplication::applicationDirPath() 
        + QDir::separator() + "plugins" );
#endif
#ifdef Q_OS_MACX
    qDebug("Adding qt image plugins to plugin search path...");
    CFURLRef myBundleRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef myMacPath = CFURLCopyFileSystemPath(myBundleRef, kCFURLPOSIXPathStyle);
    const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
    CFRelease(myBundleRef);
    CFRelease(myMacPath);
    QString myPath(mypPathPtr);
    // if we are not in a bundle assume that the app is built
    // as a non bundle app and that image plugins will be
    // in system Qt frameworks. If the app is a bundle
    // lets try to set the qt plugin search path...
    if (myPath.contains(".app"))
    {
      myPath += "/Contents/plugins";
      QApplication::addLibraryPath( myPath );
      qDebug( "Added %s to plugin search path", qPrintable( myPath ) );
    }
#endif

    QString marbleDataPath;
    uint dataPathIndex;

    for ( int i = 1; i < argc - 1; ++i ) {
        if ( strcmp( argv[ i ], "--marbleDataPath" ) == 0 )
        {
            dataPathIndex = i + 1;
            marbleDataPath = argv[ dataPathIndex ];
        }
    }

    MainWindow *window = new MainWindow( marbleDataPath );
    window->setAttribute( Qt::WA_DeleteOnClose, true );

    MarbleTest *marbleTest = new MarbleTest( window->marbleWidget() );


//    window->marbleWidget()->rotateTo( 0, 0, -90 );
    window->show();

    for ( int i = 1; i < argc; ++i ) {
        if ( strcmp( argv[ i ], "--timedemo" ) == 0 )
        {
            window->resize(900, 640);
            marbleTest->timeDemo();
            return 0;
        }
        else if( strcmp( argv[ i ], "--gpsdemo" ) == 0 ) {
            window->resize( 900, 640 );
            marbleTest->gpsDemo();
            return 0;
        }
        else if( strcmp( argv[ i ], "--enableCurrentLocation" ) ==0 )
        {
            window->marbleControl()->setCurrentLocationTabShown(true);
        }
        else if( strcmp( argv[ i ], "--enableFileView" ) ==0 )
        {
            window->marbleControl()->setFileViewTabShown(true);
        }
        else if ( QFile::exists( app.arguments().at( i ) ) 
                && i != dataPathIndex )
            ( window->marbleControl() )->addPlaceMarkFile( argv[i] );
    }

    delete marbleTest;

    return app.exec();
}
