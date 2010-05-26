//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include <QtGui/QApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTranslator>

#include "QtMainWindow.h"

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleTest.h"

#ifdef STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif

#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

using namespace Marble;
 
int main(int argc, char *argv[])
{
    // The GraphicsSystem needs to be set before the instantiation of the
    // QApplication. Therefore we need to parse the current setting 
    // in this unusual place :-/
    QSettings * graphicsSettings = new QSettings("kde.org", "Marble Desktop Globe");
    QString graphicsString = graphicsSettings->value("View/graphicsSystem", "native").toString();
    delete graphicsSettings;
    QApplication::setGraphicsSystem( graphicsString );

    QApplication app(argc, argv);
    // Widget translation

    QString      lang = QLocale::system().name().section('_', 0, 0);
    QTranslator  translator;
    translator.load( "marble-" + lang, MarbleDirs::path(QString("lang") ) );
    app.installTranslator(&translator);

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
    int dataPathIndex=0;
    MarbleGlobal::Profiles profiles = MarbleGlobal::detectProfiles();

    for ( int i = 1; i < argc; ++i ) {
        if ( strcmp( argv[ i ], "--debug-info" ) == 0 )
        {
            MarbleDebug::enable = true;
        }
        else if ( strcmp( argv[ i ], "--marbledatapath" ) == 0 && i + 1 < argc )
        {
            dataPathIndex = i + 1;
            marbleDataPath = argv[ dataPathIndex ];
        }
        else if ( strcmp( argv[ i ], "--smallscreen" ) == 0 ) {
            profiles |= MarbleGlobal::SmallScreen;
        }
        else if ( strcmp( argv[ i ], "--nosmallscreen" ) == 0 ) {
            profiles &= ~MarbleGlobal::SmallScreen;
        }
        else if ( strcmp( argv[ i ], "--highresolution" ) == 0 ) {
            profiles |= MarbleGlobal::HighResolution;
        }
        else if ( strcmp( argv[ i ], "--nohighresolution" ) == 0 ) {
            profiles &= ~MarbleGlobal::HighResolution;
        }
    }
    MarbleGlobal::getInstance()->setProfiles( profiles );

    MainWindow *window = new MainWindow( marbleDataPath );
    window->setAttribute( Qt::WA_DeleteOnClose, true );

    MarbleTest *marbleTest = new MarbleTest( window->marbleWidget() );


//    window->marbleWidget()->rotateTo( 0, 0, -90 );
//    window->show();

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
        else if( strcmp( argv[ i ], "--fps" ) == 0 ) {
            window->marbleControl()->marbleWidget()->setShowFrameRate( true );
        }
        else if( strcmp( argv[ i ], "--enableCurrentLocation" ) ==0 )
        {
            window->marbleControl()->setCurrentLocationTabShown(true);
        }
        else if( strcmp( argv[ i ], "--enableFileView" ) ==0 )
        {
            window->marbleControl()->setFileViewTabShown(true);
        }
        else if (strcmp( argv[ i ],"--tile-id" ) ==0 ) 
        {
	    window->marbleControl()->marbleWidget()->setShowTileId(true);
        }
        else if ( QFile::exists( app.arguments().at( i ) ) 
                && i != dataPathIndex )
            ( window->marbleControl() )->addPlacemarkFile( argv[i] );
    }

    delete marbleTest;

    return app.exec();
}
