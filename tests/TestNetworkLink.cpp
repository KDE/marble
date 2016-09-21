//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include <QObject>

#include "TestUtils.h"
#include <GeoDataDocument.h>
#include <GeoDataFolder.h>
#include <MarbleDebug.h>
#include <GeoDataNetworkLink.h>
#include <GeoDataLink.h>

using namespace Marble;


class TestNetworkLink : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestNetworkLink::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestNetworkLink::simpleParseTest()
{
    QString const centerContent (
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
    " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
    "<Folder>"
    " <NetworkLink>"
    "  <refreshVisibility>1</refreshVisibility> <!-- boolean -->"
    "  <flyToView>1</flyToView>                 <!-- boolean -->"
    "  <Link>"
    "   <href>http://files.kde.org/marble/examples/kml/</href>"
    "   <refreshMode>onChange</refreshMode>"
    "   <refreshInterval>2.1</refreshInterval>"
    "   <viewRefreshMode>never</viewRefreshMode>"
    "   <viewRefreshTime>4.2</viewRefreshTime>"
    "   <viewBoundScale>1.5</viewBoundScale>"
    "   <viewFormat>BBOX=[bboxWest],[bboxSouth],[bboxEast],[bboxNorth]</viewFormat>"
    "   <httpQuery>SiteType=sw,gw,sp&amp;SiteCode=all&amp;Format=ge</httpQuery>"
    "  </Link>"
    " </NetworkLink>"
    "</Folder>"
    "</kml>");

    GeoDataDocument* dataDocument = parseKml( centerContent  );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->size(), 1 );
    GeoDataNetworkLink *networkLink = dynamic_cast<GeoDataNetworkLink*>( folder->child( 0 ) );
    QVERIFY( networkLink != 0 );

    QCOMPARE( networkLink->refreshVisibility(), true );
    QCOMPARE( networkLink->flyToView(), true );
    QCOMPARE( networkLink->link().href(), QString("http://files.kde.org/marble/examples/kml/") );
    QCOMPARE( networkLink->link().refreshMode(), GeoDataLink::OnChange );
    QFUZZYCOMPARE( networkLink->link().refreshInterval(), 2.1, 0.0001 );
    QCOMPARE( networkLink->link().viewRefreshMode(), GeoDataLink::Never);
    QFUZZYCOMPARE( networkLink->link().viewRefreshTime(), 4.2, 0.0001 );
    QFUZZYCOMPARE( networkLink->link().viewBoundScale(), 1.5, 0.0001 );
    QCOMPARE( networkLink->link().viewFormat(), QString("BBOX=[bboxWest],[bboxSouth],[bboxEast],[bboxNorth]") );
    QCOMPARE( networkLink->link().httpQuery(), QString("SiteType=sw,gw,sp&SiteCode=all&Format=ge"));

}

QTEST_MAIN( TestNetworkLink )

#include "TestNetworkLink.moc"

