//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Dennis Nienhüser <nienhueser@kde.org>
//

/**
  * Does an animated camera flight using a given route loaded from
  * a .kml file and writes an .avi (DIVX) video of it.
  */

#include <marble/MarbleWidget.h>
#include <marble/MarbleMath.h>
#include <marble/GeoDataCoordinates.h>
#include <marble/GeoDataLineString.h>
#include <marble/RenderPlugin.h>
#include <marble/MarbleModel.h>
#include <marble/Route.h>
#include <marble/RoutingManager.h>
#include <marble/RoutingModel.h>
#include <marble/TourPlayback.h>
#include <marble/GeoDataTour.h>
#include <marble/GeoDataPlaylist.h>
#include <marble/GeoDataFlyTo.h>
#include <marble/GeoDataLookAt.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QApplication>
#include <QThread>
#include <QDebug>

#include <cstdio>

using namespace Marble;
using namespace cv;

class Waiter: private QThread { public: using QThread::msleep; };

namespace {
// Some stuff you might want to change

// The map theme in use
QString const mapTheme = "earth/openstreetmap/openstreetmap.dgml";

// Enabled plugins. Everything else will be disabled
QStringList const features = QStringList() << "stars" << "atmosphere";

// Frames per second
int const fps = 30;

// Target video file name
std::string const videoFile = "marble-tour-preview.avi";

// Video resolution
Size frameSize( 1280, 720 );

// Camera velocity in km/h
double const velocity = 200.0;
}

GeoDataTour* createTour( const Route &route )
{
    GeoDataTour* tour = new GeoDataTour;
    tour->setPlaylist( new GeoDataPlaylist );
    GeoDataLineString path = route.path();
    if ( path.size() < 1 ) {
        return tour;
    }

    // Extract tour points at about all 500 meters
    GeoDataCoordinates last = path.at( 0 );
    for ( int i=1; i<path.size(); ++i ) {
        GeoDataCoordinates coordinates = path.at( i );
        double const distance = EARTH_RADIUS * distanceSphere( last, coordinates );
        if ( i > 1 && distance < 500 ) {
            // Ignore waypoints that are quite close
            continue;
        }
        last = coordinates;

        // Create a point in the tour from the given route point
        GeoDataLookAt* lookat = new GeoDataLookAt;
        coordinates.setAltitude( 800 );
        lookat->setCoordinates( coordinates );
        lookat->setRange( 800 );
        GeoDataFlyTo* flyto = new GeoDataFlyTo;
        double const duration = qBound( 0.2, distance / velocity / 3.6, 10.0 );
        flyto->setDuration( duration );
        flyto->setView( lookat );
        flyto->setFlyToMode( GeoDataFlyTo::Smooth );
        tour->playlist()->addPrimitive( flyto );
    }

    return tour;
}

void animatedFlight( MarbleWidget *mapWidget, GeoDataTour* tour )
{
    mapWidget->resize( frameSize.width, frameSize.height );
    TourPlayback* playback = new TourPlayback;
    playback->setMarbleWidget( mapWidget );
    playback->setTour( tour );
    QObject::connect( playback, SIGNAL(centerOn(GeoDataCoordinates)),
                      mapWidget, SLOT(centerOn(GeoDataCoordinates)) );

    double const shift = 1.0 / fps;
    double const duration = playback->duration();

    VideoWriter videoWriter( videoFile, CV_FOURCC('D','I','V','X'), fps, frameSize );
    Mat buffer;
    buffer.create(frameSize, CV_8UC3);
    for ( double position = 0.0; position <= duration; position += shift ) {
        printf("[%i%% done]\r", cvRound( (100.0*position)/duration ) );
        fflush(stdout);

        playback->seek( position );
        QImage screenshot = QPixmap::grabWidget( mapWidget ).toImage().convertToFormat( QImage::Format_RGB888 );
        Mat converter( frameSize, CV_8UC3 );
        converter.data = screenshot.bits();
        cvtColor( converter, buffer, CV_RGB2BGR );
        videoWriter.write( buffer );
    }

    for ( int i=0; i<fps; ++i ) {
        videoWriter.write( buffer ); // one second stand-still at end
    }
    printf("Wrote %s\n", videoFile.c_str());
}

int main(int argc, char** argv)
{
    QApplication app(argc,argv);
    if (app.arguments().size() < 2) {
        qDebug() << "Usage: " << app.applicationName() << " /path/to/route.kml";
        qDebug() << "You can create a suitable route.kml file with Marble.";
        return 0;
    }

    MarbleWidget *mapWidget = new MarbleWidget;
    mapWidget->setMapThemeId(mapTheme);
    foreach( RenderPlugin* plugin, mapWidget->renderPlugins() ) {
        if ( !features.contains( plugin->nameId() ) ) {
            plugin->setEnabled( false );
        }
    }

    mapWidget->model()->routingManager()->loadRoute(argv[1]);
    Route const route = mapWidget->model()->routingManager()->routingModel()->route();
    if ( route.size() == 0 ) {
        qDebug() << "Failed to open route " << argv[1];
        return 1;
    }
    GeoDataCoordinates start = route.path().at( 0 );
    start.setLongitude( start.longitude() + 1e-6 );
    mapWidget->centerOn( start );
    mapWidget->setDistance( 0.8 );
    animatedFlight( mapWidget, createTour( route ) );
    return 0;
}
