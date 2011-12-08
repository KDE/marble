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

using namespace Marble;


class TestGeoDataTrack : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
    void removeBeforeTest();
    void removeAfterTest();
    void extendedDataParseTest();
    void withoutTimeTest();
};

void TestGeoDataTrack::initTestCase()
{
    MarbleDebug::enable = true;
}

GeoDataDocument *parseKml(const QString &content)
{
    GeoDataParser parser( GeoData_KML );

    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        qFatal( "Could not parse data!" );
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    return static_cast<GeoDataDocument*>( document );
}

    //"Simple Example" from kmlreference
    QString simpleExampleContent(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<kml xmlns=\"http://www.opengis.net/kml/2.2\""
" xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
"<Folder>"
"  <Placemark>"
"    <gx:Track>"
"      <when>2010-05-28T02:02:09Z</when>"
"      <when>2010-05-28T02:02:35Z</when>"
"      <when>2010-05-28T02:02:44Z</when>"
"      <when>2010-05-28T02:02:53Z</when>"
"      <when>2010-05-28T02:02:54Z</when>"
"      <when>2010-05-28T02:02:55Z</when>"
"      <when>2010-05-28T02:02:56Z</when>"
"      <gx:coord>-122.207881 37.371915 156.000000</gx:coord>"
"      <gx:coord>-122.205712 37.373288 152.000000</gx:coord>"
"      <gx:coord>-122.204678 37.373939 147.000000</gx:coord>"
"      <gx:coord>-122.203572 37.374630 142.199997</gx:coord>"
"      <gx:coord>-122.203451 37.374706 141.800003</gx:coord>"
"      <gx:coord>-122.203329 37.374780 141.199997</gx:coord>"
"      <gx:coord>-122.203207 37.374857 140.199997</gx:coord>"
"    </gx:Track>"
"  </Placemark>"
"</Folder>"
"</kml>" );

void TestGeoDataTrack::simpleParseTest()
{
    GeoDataDocument* dataDocument = parseKml( simpleExampleContent );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->placemarkList().size(), 1 );
    GeoDataPlacemark* placemark = folder->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataTrackId );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( placemark->geometry() );
    QCOMPARE( track->size(), 7 );
    {
        QDateTime when = track->whenList().at( 0 );
        QCOMPARE( when, QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 9 ), Qt::UTC ) );
    }
    {
        GeoDataCoordinates coord = track->coordinatesList().at( 0 );
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

    delete dataDocument;
}

void TestGeoDataTrack::removeBeforeTest()
{
    GeoDataDocument* dataDocument = parseKml( simpleExampleContent );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->placemarkList().size(), 1 );
    GeoDataPlacemark* placemark = folder->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataTrackId );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( placemark->geometry() );
    QCOMPARE( track->size(), 7 );
    track->removeBefore( QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 54 ), Qt::UTC ) );
    QCOMPARE( track->size(), 3 );
    {
        QDateTime when = track->whenList().at( 0 );
        QCOMPARE( when, QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 54 ), Qt::UTC ) );
    }
    {
        QDateTime when = track->whenList().at( 2 );
        QCOMPARE( when, QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 56 ), Qt::UTC ) );
    }
    {
        GeoDataCoordinates coord = track->coordinatesList().at( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), -122.203451 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 37.374706 );
        QCOMPARE( coord.altitude(), 141.800003 );
    }

    delete dataDocument;
}

void TestGeoDataTrack::removeAfterTest()
{
    GeoDataDocument* dataDocument = parseKml( simpleExampleContent );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->placemarkList().size(), 1 );
    GeoDataPlacemark* placemark = folder->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataTrackId );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( placemark->geometry() );
    QCOMPARE( track->size(), 7 );
    track->removeAfter( QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 54 ), Qt::UTC ) );
    QCOMPARE( track->size(), 5 );
    {
        QDateTime when = track->whenList().at( 0 );
        QCOMPARE( when, QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 9 ), Qt::UTC ) );
    }
    {
        QDateTime when = track->whenList().at( 4 );
        QCOMPARE( when, QDateTime( QDate( 2010, 5, 28 ), QTime( 2, 2, 54 ), Qt::UTC ) );
    }
    {
        GeoDataCoordinates coord = track->coordinatesList().at( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), -122.207881 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 37.371915 );
        QCOMPARE( coord.altitude(), 156.000000 );
    }

    delete dataDocument;
}

void TestGeoDataTrack::extendedDataParseTest()
{
    //"Example of Track with Extended Data" from kmlreference
    QString content(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
"  <Document>"
"    <name>GPS device</name>"
"    <Snippet>Created Wed Jun 2 15:33:39 2010</Snippet>"
"    <Schema id=\"schema\">"
"      <gx:SimpleArrayField name=\"heartrate\" type=\"int\">"
"        <displayName>Heart Rate</displayName>"
"      </gx:SimpleArrayField>"
"      <gx:SimpleArrayField name=\"cadence\" type=\"int\">"
"        <displayName>Cadence</displayName>"
"      </gx:SimpleArrayField>"
"      <gx:SimpleArrayField name=\"power\" type=\"float\">"
"        <displayName>Power</displayName>"
"      </gx:SimpleArrayField>"
"    </Schema>"
"    <Folder>"
"      <name>Tracks</name>"
"      <Placemark>"
"        <name>2010-05-28T01:16:35.000Z</name>"
"        <styleUrl>#multiTrack</styleUrl>"
"        <gx:Track>"
"          <when>2010-05-28T02:02:09Z</when>"
"          <when>2010-05-28T02:02:35Z</when>"
"          <when>2010-05-28T02:02:44Z</when>"
"          <when>2010-05-28T02:02:53Z</when>"
"          <when>2010-05-28T02:02:54Z</when>"
"          <when>2010-05-28T02:02:55Z</when>"
"          <when>2010-05-28T02:02:56Z</when>"
"          <gx:coord>-122.207881 37.371915 156.000000</gx:coord>"
"          <gx:coord>-122.205712 37.373288 152.000000</gx:coord>"
"          <gx:coord>-122.204678 37.373939 147.000000</gx:coord>"
"          <gx:coord>-122.203572 37.374630 142.199997</gx:coord>"
"          <gx:coord>-122.203451 37.374706 141.800003</gx:coord>"
"          <gx:coord>-122.203329 37.374780 141.199997</gx:coord>"
"          <gx:coord>-122.203207 37.374857 140.199997</gx:coord>"
"          <ExtendedData>"
"            <SchemaData schemaUrl=\"#schema\">"
"              <gx:SimpleArrayData name=\"cadence\">"
"                <gx:value>86</gx:value>"
"                <gx:value>103</gx:value>"
"                <gx:value>108</gx:value>"
"                <gx:value>113</gx:value>"
"                <gx:value>113</gx:value>"
"                <gx:value>113</gx:value>"
"                <gx:value>113</gx:value>"
"              </gx:SimpleArrayData>"
"              <gx:SimpleArrayData name=\"heartrate\">"
"                <gx:value>181</gx:value>"
"                <gx:value>177</gx:value>"
"                <gx:value>175</gx:value>"
"                <gx:value>173</gx:value>"
"                <gx:value>173</gx:value>"
"                <gx:value>173</gx:value>"
"                <gx:value>173</gx:value>"
"              </gx:SimpleArrayData>"
"              <gx:SimpleArrayData name=\"power\">"
"                <gx:value>327.0</gx:value>"
"                <gx:value>177.0</gx:value>"
"                <gx:value>179.0</gx:value>"
"                <gx:value>162.0</gx:value>"
"                <gx:value>166.0</gx:value>"
"                <gx:value>177.0</gx:value>"
"                <gx:value>183.0</gx:value>"
"              </gx:SimpleArrayData>"
"            </SchemaData>"
"          </ExtendedData>"
"        </gx:Track>"
"      </Placemark>"
"    </Folder>"
"  </Document>"
"</kml>" );

    GeoDataDocument* dataDocument = parseKml( content );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->placemarkList().size(), 1 );
    GeoDataPlacemark* placemark = folder->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataTrackId );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( placemark->geometry() );
    QCOMPARE( track->size(), 7 );

    {
        GeoDataSimpleArrayData cadence = track->extendedData().simpleArrayData( "cadence" );
        QCOMPARE( cadence.size(), 7 );
        QCOMPARE( cadence.valueAt( 0 ), QVariant( "86" ) );
        QCOMPARE( cadence.valueAt( 6 ), QVariant( "113" ) );
    }
    {
        GeoDataSimpleArrayData hr = track->extendedData().simpleArrayData( "heartrate" );
        QCOMPARE( hr.size(), 7 );
        QCOMPARE( hr.valueAt( 0 ), QVariant( "181" ) );
        QCOMPARE( hr.valueAt( 6 ), QVariant( "173" ) );
    }
    {
        GeoDataSimpleArrayData power = track->extendedData().simpleArrayData( "power" );
        QCOMPARE( power.size(), 7 );
        QCOMPARE( power.valueAt( 0 ), QVariant( "327.0" ) );
        QCOMPARE( power.valueAt( 6 ), QVariant( "183.0" ) );
    }

    delete dataDocument;
}

void TestGeoDataTrack::withoutTimeTest()
{
    //"Simple Example" from kmlreference; when elements emptied
    QString content(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<kml xmlns=\"http://www.opengis.net/kml/2.2\""
" xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
"<Folder>"
"  <Placemark>"
"    <gx:Track>"
"      <when></when>"
"      <when></when>"
"      <when></when>"
"      <when></when>"
"      <when></when>"
"      <when></when>"
"      <when></when>"
"      <gx:coord>-122.207881 37.371915 156.000000</gx:coord>"
"      <gx:coord>-122.205712 37.373288 152.000000</gx:coord>"
"      <gx:coord>-122.204678 37.373939 147.000000</gx:coord>"
"      <gx:coord>-122.203572 37.374630 142.199997</gx:coord>"
"      <gx:coord>-122.203451 37.374706 141.800003</gx:coord>"
"      <gx:coord>-122.203329 37.374780 141.199997</gx:coord>"
"      <gx:coord>-122.203207 37.374857 140.199997</gx:coord>"
"    </gx:Track>"
"  </Placemark>"
"</Folder>"
"</kml>" );

    GeoDataDocument* dataDocument = parseKml( content );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->placemarkList().size(), 1 );
    GeoDataPlacemark* placemark = folder->placemarkList().at( 0 );
    QCOMPARE( placemark->geometry()->geometryId(), GeoDataTrackId );
    GeoDataTrack* track = static_cast<GeoDataTrack*>( placemark->geometry() );
    QCOMPARE( track->size(), 7 );
    {
        GeoDataCoordinates coord = track->coordinatesList().at( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), -122.207881 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 37.371915 );
        QCOMPARE( coord.altitude(), 156.000000 );
    }

    {
    GeoDataLineString *lineString = track->lineString();
        QCOMPARE( lineString->size(), 7 );
        GeoDataCoordinates coord = lineString->at( 0 );
        QCOMPARE( coord.longitude( GeoDataCoordinates::Degree ), -122.207881 );
        QCOMPARE( coord.latitude( GeoDataCoordinates::Degree ), 37.371915 );
        QCOMPARE( coord.altitude(), 156.000000 );
    }

    delete dataDocument;
}

QTEST_MAIN( TestGeoDataTrack )

#include "TestGeoDataTrack.moc"

