//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Dennis Nienhüser <earthwings@gentoo.org>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <GeoDataTour.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include <GeoDataCamera.h>

using namespace Marble;

class TestCamera : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestCamera::initTestCase()
{
    MarbleDebug::enable = true;
}

GeoDataDocument *parseKml(const QString &content)
{
    GeoDataParser parser( GeoData_KML );

    QByteArray array( content.toUtf8() );
    QBuffer buffer( &array );
    buffer.open( QIODevice::ReadOnly );
    //qDebug() << "Buffer content:" << endl << buffer.buffer();
    if ( !parser.read( &buffer ) ) {
        qFatal( "Could not parse data!" );
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    return static_cast<GeoDataDocument*>( document );
}

void TestCamera::simpleParseTest()
{
  QString const kmlContent (
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Document>"
        "   <Camera id=\"myCam\">"
        "     <longitude>1</longitude>"
        "     <latitude>2</latitude>"
        "     <altitude>3</altitude>"
        "     <heading>4</heading>"
        "     <tilt>5</tilt>"
        "     <roll>6</roll>"
        "     <altitudeMode>relativeToGround</altitudeMode>"
        "   </Camera>"
        "</Document>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( kmlContent );
    GeoDataCamera *camera = dynamic_cast<GeoDataCamera*>( dataDocument->abstractView() );

    QVERIFY( camera != 0);

    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    QCOMPARE( camera->longitude( degree ), 1.0 );
    QCOMPARE( camera->latitude( degree ), 2.0 );
    QCOMPARE( camera->altitude(), 3.0 );
    QCOMPARE( camera->heading(), 4.0 );
    QCOMPARE( camera->tilt(), 5.0 );
    QCOMPARE( camera->roll(), 6.0 );
    QCOMPARE( camera->altitudeMode(), RelativeToGround );

    delete dataDocument;
}

QTEST_MAIN( TestCamera )

#include "TestCamera.moc"

