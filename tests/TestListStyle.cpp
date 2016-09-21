//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include <QObject>

#include <GeoDataDocument.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include <GeoDataPlacemark.h>
#include <GeoDataStyle.h>
#include <GeoDataListStyle.h>
#include <GeoDataItemIcon.h>
#include "TestUtils.h"

using namespace Marble;


class TestListStyle : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestListStyle::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestListStyle::simpleParseTest()
{
  QString const content (
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Document>"
        "  <name>The one and only BalloonStyle test case</name>"
        "  <Style id=\"my-list-style\">"
        "    <ListStyle>"
        "      <listItemType>checkOffOnly</listItemType>"
        "      <bgColor>aa112233</bgColor>"
        "      <ItemIcon>"
        "        <state>open error</state>"
        "        <href>https://developers.google.com/kml/documentation/images/itemicons.jpg</href>"
        "      </ItemIcon>"
        "      <ItemIcon>"
        "        <state>closed</state>"
        "        <href>https://developers.google.com/kml/documentation/images/itemicons1.jpg</href>"
        "      </ItemIcon>"
        "    </ListStyle>"
        "  </Style>"
        "  <Folder>"
        "  <Placemark>"
        "    <name>The first placemark</name>"
        "    <styleUrl>#my-list-style</styleUrl>"
        "    <Point><coordinates>80.0,30.0</coordinates></Point>"
        "  </Placemark>"
        "  </Folder>"
        "</Document>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( content  );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->size(), 1 );
    GeoDataPlacemark *placemark1 = dynamic_cast<GeoDataPlacemark*>( folder->child( 0 ) );
    QVERIFY( placemark1 != 0 );

    QCOMPARE( placemark1->name(), QString( "The first placemark" ) );
    QCOMPARE( placemark1->style()->listStyle().listItemType(), GeoDataListStyle::CheckOffOnly );
    QCOMPARE( placemark1->style()->listStyle().backgroundColor().red(), 51 );
    QCOMPARE( placemark1->style()->listStyle().itemIconList().at(0)->state(), GeoDataItemIcon::Open | GeoDataItemIcon::Error );
    QCOMPARE( placemark1->style()->listStyle().itemIconList().at(0)->iconPath(), QString( "https://developers.google.com/kml/documentation/images/itemicons.jpg" ) );
    QCOMPARE( placemark1->style()->listStyle().itemIconList().at(1)->state(), GeoDataItemIcon::Closed );
    QCOMPARE( placemark1->style()->listStyle().itemIconList().at(1)->iconPath(), QString( "https://developers.google.com/kml/documentation/images/itemicons1.jpg" ) );

    delete dataDocument;
}

QTEST_MAIN( TestListStyle )

#include "TestListStyle.moc"

