//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include <GeoDataGroundOverlay.h>

using namespace Marble;

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

class TestGroundOverlay : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestGroundOverlay::initTestCase()
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

void TestGroundOverlay::simpleParseTest()
{
  QString const centerContent (
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Folder>"
            "<GroundOverlay>"
                "<altitude>0</altitude>"
                "<altitudeMode>absolute</altitudeMode>"
                "<LatLonBox>"
                    "<north>37.91904192681665</north>"
                    "<south>37.46543388598137</south>"
                    "<east>15.35832653742206</east>"
                    "<west>14.60128369746704</west>"
                    "<rotation>-0.1556640799496235</rotation>"
                 "</LatLonBox>"
             "</GroundOverlay>"
              "<GroundOverlay>"
                  "<altitude>233</altitude>"
                  "<drawOrder>2</drawOrder>"
                  "<LatLonBox>"
                      "<north>23.3765376</north>"
                      "<south>1.5743867869</south>"
                      "<east>33.78365874</east>"
                      "<west>94.352435642</west>"
                      "<rotation>6.346364378</rotation>"
                   "</LatLonBox>"
               "</GroundOverlay>"
        "</Folder>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent  );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->size(), 2 );
    GeoDataGroundOverlay *overlayFirst = dynamic_cast<GeoDataGroundOverlay*>( folder->child( 0 ) );
    GeoDataGroundOverlay *overlaySecond = dynamic_cast<GeoDataGroundOverlay*>( folder->child( 1 ) );
    QVERIFY( overlayFirst != 0 );
    QVERIFY( overlaySecond != 0 );

    QFUZZYCOMPARE( overlayFirst->altitude(), 0.0, 0.0001 );

    QFUZZYCOMPARE( overlayFirst->altitudeMode(), Absolute, 0.0001 );
    QCOMPARE( overlayFirst->drawOrder(), 0 );

    QFUZZYCOMPARE( overlayFirst->latLonBox().north(), 37.91904192681665 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlayFirst->latLonBox().south(), 37.46543388598137 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlayFirst->latLonBox().east(), 15.35832653742206 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlayFirst->latLonBox().west(), 14.60128369746704 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlayFirst->latLonBox().rotation(), -0.1556640799496235 * DEG2RAD, 0.0001 );

    QFUZZYCOMPARE( overlaySecond->altitude(), 233.0, 0.0001 );

    QCOMPARE( overlaySecond->altitudeMode(), ClampToGround );
    QCOMPARE( overlaySecond->drawOrder(), 2 );

    QFUZZYCOMPARE( overlaySecond->latLonBox().north(), 23.3765376 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlaySecond->latLonBox().south(), 1.5743867869 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlaySecond->latLonBox().east(), 33.78365874 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlaySecond->latLonBox().west(), 94.352435642 * DEG2RAD, 0.0001 );
    QFUZZYCOMPARE( overlaySecond->latLonBox().rotation(), 6.346364378 * DEG2RAD, 0.0001 );

    delete dataDocument;
}

QTEST_MAIN( TestGroundOverlay )

#include "TestGroundOverlay.moc"
