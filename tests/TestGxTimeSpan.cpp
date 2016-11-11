//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Mayank Madan <maddiemadan@gmail.com>
//

#include <QObject>

#include <GeoDataDocument.h>
#include <GeoDataTimeSpan.h>
#include <GeoDataTimeStamp.h>
#include <MarbleDebug.h>
#include "TestUtils.h"
#include <GeoDataLookAt.h>
#include <GeoDataCamera.h>
#include <GeoDataPlacemark.h>

using namespace Marble;

class TestGxTimeSpan : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestGxTimeSpan::initTestCase()
{
    MarbleDebug::setEnabled( true );
}


void TestGxTimeSpan::simpleParseTest()
{
  QString const centerContent (
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
      " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
      "<Document>"
      "<Placemark>"
       "<LookAt>"
        "<gx:TimeSpan>"
          "<begin>2010-05-28T02:02:09Z</begin>"
          "<end>2010-05-28T02:02:56Z</end>"
        "</gx:TimeSpan>"
      "</LookAt>"
      "</Placemark>"
      "</Document>"
      "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent );
    QCOMPARE( dataDocument->placemarkList().size(), 1 );
    GeoDataPlacemark *placemark = dataDocument->placemarkList().at( 0 );
    QVERIFY( placemark->lookAt() != 0 );
    QCOMPARE( placemark->lookAt()->timeSpan().begin().when(), QDateTime::fromString( "2010-05-28T02:02:09Z", Qt::ISODate) );
    QCOMPARE( placemark->lookAt()->timeSpan().end().when(), QDateTime::fromString( "2010-05-28T02:02:56Z", Qt::ISODate) );

    delete dataDocument;
}

QTEST_MAIN( TestGxTimeSpan )

#include "TestGxTimeSpan.moc"
