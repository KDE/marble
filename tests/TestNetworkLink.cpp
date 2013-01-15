//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataDocument.h>
#include <GeoDataFolder.h>
#include <GeoDataParser.h>
#include <MarbleDebug.h>
#include <GeoDataNetworkLink.h>

namespace QTest
{

bool qCompare(qreal val1, qreal val2, qreal epsilon, const char *actual, const char *expected, const char *file, int line)
{
    return ( qAbs( val1 - val2 ) < epsilon )
        ? compare_helper( true, "COMPARE()", file, line )
        : compare_helper( false, "Compared qreals are not the same", toString( val1 ), toString( val2 ), actual, expected, file, line );
}

}

#define QFUZZYCOMPARE(actual, expected, epsilon) \
do {\
    if (!QTest::qCompare(actual, expected, epsilon, #actual, #expected, __FILE__, __LINE__))\
        return;\
} while (0)

using namespace Marble;


class TestNetworkLink : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestNetworkLink::initTestCase()
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

