//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Niko Sams <niko.sams@gmail.com>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include "GeoDataPoint.h"
#include "GeoDataLinearRing.h"
#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <GeoDataPlacemark.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include <GeoDataTrack.h>
#include <GeoDataExtendedData.h>
#include <GeoDataSimpleArrayData.h>
#include "GpxParser.h"

using namespace Marble;


class TestTrack : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
    void withoutTimeTest();

};

void TestTrack::initTestCase()
{
    MarbleDebug::enable = true;
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
        QDateTime when = track->whenAt( 0 );
        QCOMPARE( when, QDateTime( QDate( 2011, 6, 24 ), QTime( 10, 33, 40 ), Qt::UTC ) );
    }
    {
        GeoDataCoordinates coord = track->coordinatesAt( 0 );
        qDebug() << QString("%0").arg(coord.longitude( GeoDataCoordinates::Degree ), 0, 'g', 15);
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
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->placemarkList().size(), 1 );
    GeoDataPlacemark* placemark = folder->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataTrackId );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( placemark->geometry() );
    QCOMPARE( track->size(), 7 );
    {
        QDateTime when = track->whenAt( 0 );
        QCOMPARE( when, QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 9 ), Qt::UTC ) );
    }
    {
        GeoDataCoordinates coord = track->coordinatesAt( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), -122.207881 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 37.371915 );
        QCOMPARE( coord.altitude(), 156.000000 );
    }
    {
        GeoDataCoordinates coord = track->coordinatesAt( QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 9 ), Qt::UTC ) );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), -122.207881 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 37.371915 );
        QCOMPARE( coord.altitude(), 156.000000 );
    }

    delete document;
}

QTEST_MAIN( TestTrack )

#include "TestTrack.moc"

