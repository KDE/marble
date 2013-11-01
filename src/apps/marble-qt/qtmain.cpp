//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QSettings>
#include <QTranslator>
#include <QProcessEnvironment>

#include "QtMainWindow.h"

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleTest.h"
#include "MarbleLocale.h"

#ifdef STATIC_BUILD
 #include <QtPlugin>
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
#if QT_VERSION < 0x050000
    // The GraphicsSystem needs to be set before the instantiation of the
    // QApplication. Therefore we need to parse the current setting 
    // in this unusual place :-/
    QSettings graphicsSettings("KDE", "Marble Virtual Globe"); // keep the parameters here
    QString const graphicsString = graphicsSettings.value("View/graphicsSystem", "raster").toString();
    QApplication::setGraphicsSystem( graphicsString );
#endif

    QApplication app(argc, argv);
    app.setApplicationName( "Marble Virtual Globe" );
    app.setOrganizationName( "KDE" );
    app.setOrganizationDomain( "kde.org" );
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
    QApplication::instance()->setAttribute(Qt::AA_DontShowIconsInMenus);
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
    QString mapThemeId;
    QString coordinatesString;
    QString distanceString;
    MarbleGlobal::Profiles profiles = MarbleGlobal::detectProfiles();

    QStringList args = QApplication::arguments();

    if ( args.contains( "-h" ) || args.contains( "--help" ) ) {
        qWarning() << "Usage: marble [options] [files]";
        qWarning();
        qWarning() << "[files] can be zero, one or more .kml and/or .gpx files to load and show.";
        qWarning();
        qWarning() << "general options:";
        qWarning() << "  --marbledatapath=<path> .... Overwrite the compile-time path to map themes and other data";
        qWarning() << "  --latlon=<coordinates> ..... Show map at given lat lon coordinates";
        qWarning() << "  --distance=<value> ......... Set the distance of the observer to the globe (in km)";
        qWarning() << "  --map=<id> ................. Use map id (e.g. \"earth/openstreetmap/openstreetmap.dgml\")";
        qWarning();
        qWarning() << "debug options:";
        qWarning() << "  --debug-info ............... write (more) debugging information to the console";
        qWarning() << "  --fps ...................... Show the paint performance (paint rate) in the top left corner";
        qWarning() << "  --runtimeTrace.............. Show the time spent and other debug info of each layer";
        qWarning() << "  --tile-id................... Write the identifier of texture tiles on top of them";
        qWarning() << "  --timedemo ................. Measure the paint performance while moving the map and quit";
        qWarning();
        qWarning() << "profile options (note that marble should automatically detect which profile to use. Override that with the options below):";
        qWarning() << "  --highresolution ........... Enforce the profile for devices with high resolution (e.g. desktop computers)";
        qWarning() << "  --nohighresolution ......... Deactivate the profile for devices with high resolution (e.g. desktop computers)";

        return 0;
    }

    for ( int i = 1; i < args.count(); ++i ) {
        const QString arg = args.at(i);

        if ( arg == QLatin1String( "--debug-info" ) )
        {
            MarbleDebug::setEnabled( true );
        }
        else if ( arg.startsWith( QLatin1String( "--marbledatapath=" ), Qt::CaseInsensitive ) )
        {
            marbleDataPath = args.at(i).mid(17);
        }
        else if ( arg.compare( QLatin1String( "--marbledatapath" ), Qt::CaseInsensitive ) == 0 ) {
            dataPathIndex = i + 1;
            marbleDataPath = args.value( dataPathIndex );
            ++i;
        }
        else if ( arg == QLatin1String( "--highresolution" ) ) {
            profiles |= MarbleGlobal::HighResolution;
        }
        else if ( arg == QLatin1String( "--nohighresolution" ) ) {
            profiles &= ~MarbleGlobal::HighResolution;
        }
        else if ( arg.startsWith( QLatin1String( "--latlon=" ), Qt::CaseInsensitive ) )
        {
            coordinatesString = arg.mid(9);
        }
        else if ( arg.compare( QLatin1String( "--latlon" ), Qt::CaseInsensitive ) == 0 ) {
            ++i;
            // TODO: misses an error check if there is a value at all
            // and error reporting to user (problem also exists with marbledatapath)
            coordinatesString = args.value( i );
        }
        else if ( arg.startsWith( QLatin1String( "--distance=" ), Qt::CaseInsensitive ) )
        {
            distanceString = arg.mid(11);
        }
        else if ( arg.compare( QLatin1String( "--distance" ), Qt::CaseInsensitive ) == 0 ) {
            ++i;
            // TODO: misses an error check if there is a value at all
            // and error reporting to user (problem also exists with marbledatapath)
            distanceString = args.value( i );
        }
        else if ( arg.startsWith( QLatin1String( "--map=" ), Qt::CaseInsensitive ) )
        {
            mapThemeId = arg.mid(6);
        }
        else if ( arg.compare( QLatin1String( "--map" ), Qt::CaseInsensitive ) == 0 ) {
            ++i;
            // TODO: misses an error check if there is a value at all
            // and error reporting to user (problem also exists with marbledatapath)
            mapThemeId = args.value( i );
        }
    }
    MarbleGlobal::getInstance()->setProfiles( profiles );

    QLocale::MeasurementSystem const measurement = QLocale::system().measurementSystem();
    MarbleGlobal::getInstance()->locale()->setMeasurementSystem( measurement );

    QVariantMap cmdLineSettings;
    if ( !mapThemeId.isEmpty() ) {
        cmdLineSettings.insert( QLatin1String("mapTheme"), QVariant(mapThemeId) );
    }

    if ( !coordinatesString.isEmpty() ) {
        bool success = false;
        const GeoDataCoordinates coordinates = GeoDataCoordinates::fromString(coordinatesString, success);
        if ( success ) {
            QVariantList lonLat;
            lonLat << QVariant( coordinates.longitude(GeoDataCoordinates::Degree) )
                   << QVariant( coordinates.latitude(GeoDataCoordinates::Degree) );
            cmdLineSettings.insert( QLatin1String("lonlat"), QVariant(lonLat) );
        }
    }
    if ( !distanceString.isEmpty() ) {
        bool success = false;
        const qreal distance = distanceString.toDouble(&success);
        if ( success ) {
            cmdLineSettings.insert( QLatin1String("distance"), QVariant(distance) );
        }
    }

    MainWindow *window = new MainWindow( marbleDataPath, cmdLineSettings );
    window->setAttribute( Qt::WA_DeleteOnClose, true );

//    window->marbleWidget()->rotateTo( 0, 0, -90 );
//    window->show();

    for ( int i = 1; i < args.count(); ++i ) {
        const QString arg = args.at(i);
        if ( arg == "--timedemo" )
        {
            window->resize(900, 640);
            MarbleTest marbleTest( window->marbleWidget() );
            marbleTest.timeDemo();
            return 0;
        }
        else if( arg == "--fps" ) {
            window->marbleControl()->marbleWidget()->setShowFrameRate( true );
        }
        else if ( arg == "--tile-id" )
        {
	    window->marbleControl()->marbleWidget()->setShowTileId(true);
        }
        else if( arg == "--runtimeTrace" ) {
            window->marbleControl()->marbleWidget()->setShowRuntimeTrace( true );
        }
        else if ( i != dataPathIndex && QFile::exists( arg ) )
            window->addGeoDataFile( arg );
    }

    return app.exec();
}
