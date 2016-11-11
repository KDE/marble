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
#include <GeoDataPlacemark.h>
#include <GeoDataTimeStamp.h>
#include <GeoDataCamera.h>
#include <MarbleDebug.h>
#include "TestUtils.h"

using namespace Marble;

class TestGxTimeStamp : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestGxTimeStamp::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestGxTimeStamp::simpleParseTest()
{
  QString const centerContent (
              "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
              "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
              " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
              "<Document>"
                  "<Placemark>"
                    "<Camera>"
                      "<gx:TimeStamp>"
                        "<when>1987-06-05T04:03:02-01:00</when>"
                      "</gx:TimeStamp>"
                    "</Camera>"
                  "</Placemark>"
              "</Document>"
              "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent );
    QCOMPARE( dataDocument->placemarkList().size(), 1 );
    GeoDataPlacemark *placemark = dataDocument->placemarkList().at( 0 );
    GeoDataAbstractView* view = placemark->abstractView();
    QVERIFY( view != 0 );
    GeoDataCamera* camera = dynamic_cast<GeoDataCamera*>( view );
    QVERIFY( camera != 0 );
    QCOMPARE( camera->timeStamp().when().toUTC(), QDateTime::fromString( "1987-06-05T04:03:02-01:00", Qt::ISODate).toUTC() );

    delete dataDocument;
}

QTEST_MAIN( TestGxTimeStamp )

#include "TestGxTimeStamp.moc"

