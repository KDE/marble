//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
// Loosely based on MarblePhysics.cpp
//

/**
  * Animated jump from a source point to a destination point, recording
  * the result to a video.
  */

#include <marble/MarbleWidget.h>
#include <marble/GeoDataCoordinates.h>
#include <marble/GeoDataLineString.h>
#include <marble/RenderPlugin.h>
#include <marble/Quaternion.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QtCore/QTimeLine>
#include <QtCore/qmath.h>
#include <QtGui/QApplication>

#include <cstdio>

using namespace Marble;
using namespace cv;

namespace {
    // Some stuff you might want to change

    // The map theme in use
    QString const mapTheme = "earth/mapquest-open-aerial/mapquest-open-aerial.dgml";

    // Enabled plugins. Everything else will be disabled
    QStringList const features = QStringList() << "stars" << "atmosphere";

    // Camera starting point: Position and zoom level
    GeoDataCoordinates const source(   8.40314, 49.01302, 0.0, GeoDataCoordinates::Degree);
    double const sourceZoomLevel = 11;

    // Camera destination point: Position and zoom level
    GeoDataCoordinates const destination( -101.36631, 43.13718, 0.0, GeoDataCoordinates::Degree);
    double const destinationZoomLevel = 14;

    // Minimum zoom level (in the middle of the animation)
    double const jumpZoomLevel = 5.5;

    // Length of the video
    QTimeLine timeLine( 20 * 1000 );

    // Frames per second
    int const fps = 30;

    // Target video file name
    std::string const videoFile = "marble-animated-zoom.avi";

    // Video resolution
    Size frameSize( 1280, 720 );
}

void interpolate( MarbleWidget* widget, qreal value )
{
    GeoDataCoordinates coordinates;
    qreal lon, lat;
    Quaternion::slerp( source.quaternion(), destination.quaternion(), value ).getSpherical( lon, lat );
    coordinates.setLongitude( lon );
    coordinates.setLatitude( lat );
    widget->centerOn( coordinates );
    widget->setRadius( exp(jumpZoomLevel) + (value < 0.5 ? exp(sourceZoomLevel*(1.0-2*value)) : exp(destinationZoomLevel*(2*value-1.0))) );
}

void animatedFlight( MarbleWidget *mapWidget )
{
    mapWidget->resize( frameSize.width, frameSize.height );
    VideoWriter videoWriter( videoFile, CV_FOURCC('D','I','V','X'), fps, frameSize );
    Mat buffer;
    buffer.create(frameSize, CV_8UC3);
    timeLine.setCurveShape( QTimeLine::EaseInOutCurve );
    int const frameTime = qRound( 1000.0 / fps );
    for ( int i=1; i<=timeLine.duration(); i+=frameTime ) {
        printf("[%i%% done]\r", cvRound( (100.0*i)/timeLine.duration() ) );
        fflush(stdout);
        interpolate( mapWidget, timeLine.valueForTime( i ) );
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
    MarbleWidget *mapWidget = new MarbleWidget;
    mapWidget->setMapThemeId(mapTheme);
    foreach( RenderPlugin* plugin, mapWidget->renderPlugins() ) {
        if ( !features.contains( plugin->nameId() ) ) {
            plugin->setEnabled( false );
        }
    }

    animatedFlight( mapWidget );
    return 0;
}
