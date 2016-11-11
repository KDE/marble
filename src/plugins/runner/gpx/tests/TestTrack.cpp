//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Niko Sams <niko.sams@gmail.com>
//

#include <QObject>
#include <QtTest>

#include "GeoDataPoint.h"
#include "GeoDataLinearRing.h"
#include "GeoDataMultiGeometry.h"
#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <GeoDataPlacemark.h>
#include <MarbleDebug.h>
#include <GeoDataTrack.h>
#include <GeoDataExtendedData.h>
#include <GeoDataSimpleArrayData.h>
#include "GpxParser.h"

using namespace Marble;


class TestTrack : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
    void withoutTimeTest();
    void partialTimeTest();
    void extendedDataHeartRateTest();

};

void TestTrack::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestTrack::simpleParseTest()
{
    //example track recorded using a Garmin Vista HCx and downloaded using gpsbabel
    QString content(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<gpx"
"  version=\"1.0\""
"  creator=\"GPSBabel - http://www.gpsbabel.org\""
"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
"  xmlns=\"http://www.topografix.com/GPX/1/0\""
"  xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">"
"<time>2011-07-03T14:19:57Z</time>"
"<bounds minlat=\"47.231073193\" minlon=\"12.549449634\" maxlat=\"48.502999926\" maxlon=\"14.302069964\"/>"
"<trk>"
"  <name>test track</name>"
"<number>1</number>"
"<trkseg>"
"<trkpt lat=\"47.231477033\" lon=\"12.560534449\">"
"  <ele>1130.647705</ele>"
"  <time>2011-06-24T10:33:40Z</time>"
"</trkpt>"
"<trkpt lat=\"47.231486840\" lon=\"12.560604354\">"
"  <ele>1127.763672</ele>"
"  <time>2011-06-24T10:33:55Z</time>"
"</trkpt>"
"<trkpt lat=\"47.231497569\" lon=\"12.560612401\">"
"  <ele>1121.995850</ele>"
"  <time>2011-06-24T10:34:00Z</time>"
"</trkpt>"
"</trkseg>"
"</trk>"
"</gpx>"
);

    GpxParser parser;

    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    GeoDataPlacemark* placemark = dataDocument->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataMultiGeometryId );
    GeoDataMultiGeometry* multiGeo = static_cast<GeoDataMultiGeometry*>( placemark->geometry() );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( &multiGeo->at( 0 ) );
    QCOMPARE( track->size(), 3 );
    {
        QDateTime when = track->whenList().at( 0 );
        QCOMPARE( when, QDateTime( QDate( 2011, 6, 24 ), QTime( 10, 33, 40 ), Qt::UTC ) );
    }
    {
        GeoDataCoordinates coord = track->coordinatesAt( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }
    {
        GeoDataCoordinates coord = track->coordinatesAt( QDateTime( QDate( 2011, 6, 24 ), QTime( 10, 33, 40 ), Qt::UTC ) );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }
    {
        const GeoDataLineString* lineString = track->lineString();
        QCOMPARE( lineString->size(), 3 );
        GeoDataCoordinates coord = lineString->at( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }

    delete document;
}

void TestTrack::withoutTimeTest()
{
    //example track recorded using a Garmin Vista HCx and downloaded using gpsbabel; time removed
    QString content(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<gpx"
"  version=\"1.0\""
"  creator=\"GPSBabel - http://www.gpsbabel.org\""
"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
"  xmlns=\"http://www.topografix.com/GPX/1/0\""
"  xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">"
"<time>2011-07-03T14:19:57Z</time>"
"<bounds minlat=\"47.231073193\" minlon=\"12.549449634\" maxlat=\"48.502999926\" maxlon=\"14.302069964\"/>"
"<trk>"
"  <name>test track</name>"
"<number>1</number>"
"<trkseg>"
"<trkpt lat=\"47.231477033\" lon=\"12.560534449\">"
"  <ele>1130.647705</ele>"
"</trkpt>"
"<trkpt lat=\"47.231486840\" lon=\"12.560604354\">"
"  <ele>1127.763672</ele>"
"</trkpt>"
"<trkpt lat=\"47.231497569\" lon=\"12.560612401\">"
"  <ele>1121.995850</ele>"
"</trkpt>"
"</trkseg>"
"</trk>"
"</gpx>"
);

    GpxParser parser;

    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    GeoDataPlacemark* placemark = dataDocument->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataMultiGeometryId );
    GeoDataMultiGeometry* multiGeo = static_cast<GeoDataMultiGeometry*>( placemark->geometry() );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( &multiGeo->at( 0 ) );
    QCOMPARE( track->size(), 3 );
    {
        GeoDataCoordinates coord = track->coordinatesAt( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }
    {
        const GeoDataLineString* lineString = track->lineString();
        QCOMPARE( lineString->size(), 3 );
        GeoDataCoordinates coord = lineString->at( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }


    delete document;
}

void TestTrack::partialTimeTest()
{
    //example track recorded using a Garmin Vista HCx and downloaded using gpsbabel
    //one time information removed
    QString content(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<gpx"
"  version=\"1.0\""
"  creator=\"GPSBabel - http://www.gpsbabel.org\""
"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
"  xmlns=\"http://www.topografix.com/GPX/1/0\""
"  xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">"
"<time>2011-07-03T14:19:57Z</time>"
"<bounds minlat=\"47.231073193\" minlon=\"12.549449634\" maxlat=\"48.502999926\" maxlon=\"14.302069964\"/>"
"<trk>"
"  <name>test track</name>"
"<number>1</number>"
"<trkseg>"
"<trkpt lat=\"47.231477033\" lon=\"12.560534449\">"
"  <ele>1130.647705</ele>"
"  <time>2011-06-24T10:33:40Z</time>"
"</trkpt>"
"<trkpt lat=\"47.231486840\" lon=\"12.560604354\">"
"  <ele>1127.763672</ele>"
"</trkpt>"
"<trkpt lat=\"47.231497569\" lon=\"12.560612401\">"
"  <ele>1121.995850</ele>"
"  <time>2011-06-24T10:34:00Z</time>"
"</trkpt>"
"</trkseg>"
"</trk>"
"</gpx>"
);

    GpxParser parser;

    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    GeoDataPlacemark* placemark = dataDocument->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataMultiGeometryId );
    GeoDataMultiGeometry* multiGeo = static_cast<GeoDataMultiGeometry*>( placemark->geometry() );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( &multiGeo->at( 0 ) );
    QCOMPARE( track->whenList().size(), 3 );
    {
        QDateTime when = track->whenList().at( 0 );
        QCOMPARE( when, QDateTime( QDate( 2011, 6, 24 ), QTime( 10, 33, 40 ), Qt::UTC ) );
    }
    {
        QDateTime when = track->whenList().at( 1 );
        QCOMPARE( when, QDateTime() );
    }
    {
        QDateTime when = track->whenList().at( 2 );
        QCOMPARE( when, QDateTime( QDate( 2011, 6, 24 ), QTime( 10, 34, 00 ), Qt::UTC ) );
    }
    QCOMPARE( track->size(), 3 );
    {
        GeoDataCoordinates coord = track->coordinatesAt( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }
    {
        GeoDataCoordinates coord = track->coordinatesAt( QDateTime( QDate( 2011, 6, 24 ), QTime( 10, 33, 40 ), Qt::UTC ) );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }
    {
        GeoDataCoordinates coord = track->coordinatesAt( QDateTime( QDate( 2011, 6, 24 ), QTime( 10, 34, 00 ), Qt::UTC ) );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560612401 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231497569 );
        QCOMPARE( coord.altitude(), 1121.995850 );
    }
    {
        const GeoDataLineString* lineString = track->lineString();
        QCOMPARE( lineString->size(), 3 );
        GeoDataCoordinates coord = lineString->at( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), 12.560534449 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 47.231477033 );
        QCOMPARE( coord.altitude(), 1130.647705 );
    }

    delete document;
}

void TestTrack::extendedDataHeartRateTest()
{
    //example track recorded using a Garmin Oregon 450 and downloading using it's USB mass storare mode
    QString content(
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
"<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" "
"    xmlns:gpxx=\"http://www.garmin.com/xmlschemas/WaypointExtension/v1\" "
"    xmlns:gpxtrx=\"http://www.garmin.com/xmlschemas/GpxExtensions/v3\" "
"    xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\" "
"    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
"    creator=\"Oregon 450\" "
"    version=\"1.1\" "
"    xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 "
                         "http://www.topografix.com/GPX/1/1/gpx.xsd "
                         "http://www.garmin.com/xmlschemas/WaypointExtension/v1 "
                         "http://www8.garmin.com/xmlschemas/WaypointExtensionv1.xsd "
                         "http://www.garmin.com/xmlschemas/TrackPointExtension/v1 "
                         "http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd\" "
"  >"
"  <metadata>"
"    <link href=\"http://www.garmin.com\">"
"      <text>Garmin International</text>"
"    </link>"
"    <time>2011-10-29T15:29:19Z</time>"
"  </metadata>"
"  <trk>"
"    <name>29-OKT-11 17:29:17</name>"
"    <extensions>"
"      <gpxtrx:TrackExtension>"
"        <gpxtrx:DisplayColor>Black</gpxtrx:DisplayColor>"
"      </gpxtrx:TrackExtension>"
"    </extensions>"
"    <trkseg>"
"      <trkpt lat=\"47.951347\" lon=\"13.228035\">"
"        <ele>571.16</ele>"
"        <time>2011-10-29T08:35:31Z</time>"
"        <extensions>"
"          <gpxtpx:TrackPointExtension>"
"            <gpxtpx:hr>108</gpxtpx:hr>"
"          </gpxtpx:TrackPointExtension>"
"        </extensions>"
"      </trkpt>"
"      <trkpt lat=\"47.951348\" lon=\"13.228035\">"
"        <ele>573.56</ele>"
"        <time>2011-10-29T08:35:37Z</time>"
"        <extensions>"
"          <gpxtpx:TrackPointExtension>"
"            <gpxtpx:hr>109</gpxtpx:hr>"
"          </gpxtpx:TrackPointExtension>"
"        </extensions>"
"      </trkpt>"
"      <trkpt lat=\"47.951349\" lon=\"13.228036\">"
"        <ele>572.12</ele>"
"        <time>2011-10-29T08:35:43Z</time>"
"        <extensions>"
"          <gpxtpx:TrackPointExtension>"
"            <gpxtpx:hr>110</gpxtpx:hr>"
"          </gpxtpx:TrackPointExtension>"
"        </extensions>"
"      </trkpt>"
"    </trkseg>"
"  </trk>"
"</gpx>"
);

    GpxParser parser;

    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        QFAIL( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    QVERIFY( document );
    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    GeoDataPlacemark* placemark = dataDocument->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataMultiGeometryId );
    GeoDataMultiGeometry* multiGeo = static_cast<GeoDataMultiGeometry*>( placemark->geometry() );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( &multiGeo->at( 0 ) );
    QCOMPARE( track->size(), 3 );

    {
        GeoDataSimpleArrayData* hr = track->extendedData().simpleArrayData( "heartrate" );
        QCOMPARE( hr->size(), 3 );
        QCOMPARE( hr->valueAt( 0 ), QVariant( "108" ) );
        QCOMPARE( hr->valueAt( 2 ), QVariant( "110" ) );
    }

    delete document;
}

QTEST_MAIN( TestTrack )

#include "TestTrack.moc"

