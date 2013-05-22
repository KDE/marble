//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Dennis Nienhüser <earthwings@gentoo.org>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <GeoDataPlacemark.h>
#include <GeoDataTimeSpan.h>
#include <GeoDataCamera.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include "TestUtils.h"

using namespace Marble;

class TestTimeStamp : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestTimeStamp::initTestCase()
{
    MarbleDebug::enable = true;
}

void TestTimeStamp::simpleParseTest()
{
  QString const centerContent (
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
      " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
      "<Document>"
          "<Placemark><TimeSpan><begin>1988</begin><end>1999</end></TimeSpan></Placemark>"
          "<Placemark><TimeSpan><begin>1989-04</begin><end>1999-05</end></TimeSpan></Placemark>"
          "<Placemark><TimeSpan><begin>1990-02-03</begin><end>1999-04-08</end></TimeSpan></Placemark>"
          "<Placemark><TimeSpan><begin>1997-07-16T07:30:15Z</begin><end>1997-08-17T02:35:12Z</end></TimeSpan></Placemark>"
          "<Placemark><TimeSpan><begin>1997-07-16T10:30:15+03:00</begin><end>1998-07-16T10:30:15+03:00</end></TimeSpan></Placemark>"
      "</Document>"
      "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent );
    QCOMPARE( dataDocument->placemarkList().size(), 5 );
    GeoDataPlacemark* placemark1 = dataDocument->placemarkList().at( 0 );
    QCOMPARE( placemark1->timeSpan().begin().date().year(), 1988 );
    QCOMPARE( placemark1->timeSpan().end().date().year(), 1999 );
    GeoDataPlacemark* placemark2 = dataDocument->placemarkList().at( 1 );
    QCOMPARE( placemark2->timeSpan().begin().date().year(), 1989 );
    QCOMPARE( placemark2->timeSpan().begin().date().month(), 4 );
    QCOMPARE( placemark2->timeSpan().end().date().year(), 1999 );
    QCOMPARE( placemark2->timeSpan().end().date().month(), 5 );
    GeoDataPlacemark* placemark3 = dataDocument->placemarkList().at( 2 );
    QCOMPARE( placemark3->timeSpan().begin().date().year(), 1990 );
    QCOMPARE( placemark3->timeSpan().begin().date().month(), 2 );
    QCOMPARE( placemark3->timeSpan().begin().date().day(), 3 );
    QCOMPARE( placemark3->timeSpan().end().date().year(), 1999 );
    QCOMPARE( placemark3->timeSpan().end().date().month(), 4 );
    QCOMPARE( placemark3->timeSpan().end().date().day(), 8 );
    GeoDataPlacemark* placemark4 = dataDocument->placemarkList().at( 3 );
    QCOMPARE( placemark4->timeSpan().begin(), QDateTime::fromString( "1997-07-16T07:30:15Z", Qt::ISODate ) );
    QCOMPARE( placemark4->timeSpan().end(), QDateTime::fromString( "1997-08-17T02:35:12Z", Qt::ISODate ) );
    GeoDataPlacemark* placemark5 = dataDocument->placemarkList().at( 4 );
    QCOMPARE( placemark5->timeSpan().begin(), QDateTime::fromString( "1997-07-16T10:30:15+03:00", Qt::ISODate ) );
    QCOMPARE( placemark5->timeSpan().end(), QDateTime::fromString( "1998-07-16T10:30:15+03:00", Qt::ISODate ) );

    delete dataDocument;
}

QTEST_MAIN( TestTimeStamp )

#include "TestTimeSpan.moc"

