//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Dennis Nienhüser <nienhueser@kde.org>
//

#include <QObject>

#include <GeoDataDocument.h>
#include <GeoDataPlacemark.h>
#include <GeoDataTimeStamp.h>
#include <GeoDataCamera.h>
#include <MarbleDebug.h>
#include "TestUtils.h"

using namespace Marble;

class TestTimeStamp : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestTimeStamp::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestTimeStamp::simpleParseTest()
{
  QString const centerContent (
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
      " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
      "<Document>"
          "<Placemark><TimeStamp><when>1988</when></TimeStamp></Placemark>"
          "<Placemark><TimeStamp><when>1989-04</when></TimeStamp></Placemark>"
          "<Placemark><TimeStamp><when>1990-02-03</when></TimeStamp></Placemark>"
          "<Placemark><TimeStamp><when>1997-07-16T07:30:15Z</when></TimeStamp></Placemark>"
          "<Placemark><TimeStamp><when>1997-07-16T10:30:15+03:00</when></TimeStamp></Placemark>"
      "</Document>"
      "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent );
    QCOMPARE( dataDocument->placemarkList().size(), 5 );
    GeoDataPlacemark* placemark1 = dataDocument->placemarkList().at( 0 );
    QCOMPARE( placemark1->timeStamp().when().date().year(), 1988 );
    QCOMPARE( placemark1->timeStamp().resolution(), GeoDataTimeStamp::YearResolution );
    GeoDataPlacemark* placemark2 = dataDocument->placemarkList().at( 1 );
    QCOMPARE( placemark2->timeStamp().when().date().year(), 1989 );
    QCOMPARE( placemark2->timeStamp().when().date().month(), 4 );
    QCOMPARE( placemark2->timeStamp().resolution(), GeoDataTimeStamp::MonthResolution );
    GeoDataPlacemark* placemark3 = dataDocument->placemarkList().at( 2 );
    QCOMPARE( placemark3->timeStamp().when().date().year(), 1990 );
    QCOMPARE( placemark3->timeStamp().when().date().month(), 2 );
    QCOMPARE( placemark3->timeStamp().when().date().day(), 3 );
    QCOMPARE( placemark3->timeStamp().resolution(), GeoDataTimeStamp::DayResolution );
    GeoDataPlacemark* placemark4 = dataDocument->placemarkList().at( 3 );
    QCOMPARE( placemark4->timeStamp().when(), QDateTime::fromString( "1997-07-16T07:30:15Z", Qt::ISODate ) );
    QCOMPARE( placemark4->timeStamp().resolution(), GeoDataTimeStamp::SecondResolution );
    GeoDataPlacemark* placemark5 = dataDocument->placemarkList().at( 4 );
    QCOMPARE( placemark5->timeStamp().when(), QDateTime::fromString( "1997-07-16T10:30:15+03:00", Qt::ISODate ) );
    QCOMPARE( placemark5->timeStamp().resolution(), GeoDataTimeStamp::SecondResolution );

    delete dataDocument;
}

QTEST_MAIN( TestTimeStamp )

#include "TestTimeStamp.moc"

