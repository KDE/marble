//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest/QtTest>
#include "MercatorProjection.h"
#include "ViewportParams.h"

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

#define addRow() QTest::newRow( QString("line %1").arg( __LINE__ ).toAscii().data() )

namespace Marble
{

class MercatorProjectionTest : public QObject
{
    Q_OBJECT

 private slots:
    void screenCoordinatesValidLat_data();
    void screenCoordinatesValidLat();

    void screenCoordinatesOfCenter_data();
    void screenCoordinatesOfCenter();
};

void MercatorProjectionTest::screenCoordinatesValidLat_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );
    QTest::addColumn<bool>( "validLat" );

    addRow() << 0.0 << 0.0 << true;

    addRow() << -180.0 << 0.0 << true;
    addRow() <<  180.0 << 0.0 << true;

    addRow() << 0.0 << MercatorProjection().minValidLat() * RAD2DEG << true;
    addRow() << 0.0 << MercatorProjection().maxValidLat() * RAD2DEG << true;

    addRow() << 0.0 << MercatorProjection().minValidLat() * RAD2DEG - 0.0001 << false;
    addRow() << 0.0 << MercatorProjection().maxValidLat() * RAD2DEG + 0.0001 << false;

    addRow() << -180.0 << MercatorProjection().minValidLat() * RAD2DEG << true;
    addRow() <<  180.0 << MercatorProjection().minValidLat() * RAD2DEG << true;

    addRow() << -180.0 << MercatorProjection().maxValidLat() * RAD2DEG << true;
    addRow() <<  180.0 << MercatorProjection().maxValidLat() * RAD2DEG << true;

    addRow() << -180.0 << MercatorProjection().minValidLat() * RAD2DEG - 0.0001 << false;
    addRow() <<  180.0 << MercatorProjection().minValidLat() * RAD2DEG - 0.0001 << false;

    addRow() << -180.0 << MercatorProjection().maxValidLat() * RAD2DEG + 0.0001 << false;
    addRow() <<  180.0 << MercatorProjection().maxValidLat() * RAD2DEG + 0.0001 << false;
}

void MercatorProjectionTest::screenCoordinatesValidLat()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );
    QFETCH( bool, validLat );

    const GeoDataCoordinates coordinates( lon, lat, 0, GeoDataCoordinates::Degree );

    ViewportParams viewport;
    viewport.setProjection( Mercator );
    viewport.setRadius( 360 / 4 ); // for easy mapping of lon <-> x
    viewport.centerOn( 0.0, 0.0 );
    viewport.setSize( QSize( 360, 361 ) ); // TODO: check why height == 360 doesn't hold

    {
        qreal x;
        qreal y;

        const bool retval = viewport.screenCoordinates( lon * DEG2RAD, lat * DEG2RAD, x, y );

        QVERIFY( retval == validLat );
    }

    {
        qreal x;
        qreal y;
        bool globeHidesPoint = true;

        const bool retval = viewport.screenCoordinates( coordinates, x, y, globeHidesPoint );

        QVERIFY( retval == validLat );
        QVERIFY( !globeHidesPoint );
    }

    QVERIFY( viewport.currentProjection()->repeatX() );

    {
        qreal x[2];
        qreal y;
        int pointRepeatNum = 1000;
        bool globeHidesPoint = true;

        const bool retval = viewport.screenCoordinates( coordinates, x, y, pointRepeatNum, QSizeF( 0, 0 ), globeHidesPoint );

        QVERIFY( retval == validLat );
        QCOMPARE( pointRepeatNum, 1 );
        QVERIFY( !globeHidesPoint );
    }
}

void MercatorProjectionTest::screenCoordinatesOfCenter_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << 0.0 << 0.0;

    addRow() << -180.0 << 0.0;
    addRow() <<  180.0 << 0.0;

    addRow() << -360.0 << 0.0;
    addRow() <<  360.0 << 0.0;

    addRow() << -540.0 << 0.0;
    addRow() <<  540.0 << 0.0;

    addRow() << 0.0 << MercatorProjection().minValidLat() * RAD2DEG;
    addRow() << 0.0 << MercatorProjection().maxValidLat() * RAD2DEG;

    addRow() << -180.0 << MercatorProjection().minValidLat() * RAD2DEG;
    addRow() << -180.0 << MercatorProjection().maxValidLat() * RAD2DEG;

    addRow() <<  180.0 << MercatorProjection().minValidLat() * RAD2DEG;
    addRow() <<  180.0 << MercatorProjection().maxValidLat() * RAD2DEG;

    // FIXME: the following tests should succeed
#if 0
    addRow() << -541.0 << 0.0;
    addRow() <<  541.0 << 0.0;

    addRow() << -1000000.0 << 0.0;
    addRow() <<  1000000.0 << 0.0;
#endif
}

void MercatorProjectionTest::screenCoordinatesOfCenter()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    const GeoDataCoordinates coordinates( lon, lat, 0, GeoDataCoordinates::Degree );

    ViewportParams viewport;
    viewport.setProjection( Mercator );
    viewport.setRadius( 360 / 4 ); // for easy mapping of lon <-> x
    viewport.setSize( QSize( 2, 2 ) );
    viewport.centerOn( lon * DEG2RAD, lat * DEG2RAD );

    {
        qreal x;
        qreal y;

        const bool retval = viewport.screenCoordinates( lon * DEG2RAD, lat * DEG2RAD, x, y );

        QVERIFY( retval ); // FIXME: this should fail for lon < -180 || 180 < lon
        QCOMPARE( x, lon - viewport.centerLongitude() * RAD2DEG + 1.0 );
        QCOMPARE( y, 1.0 );
    }

    {
        qreal x;
        qreal y;
        bool globeHidesPoint = true;

        const bool retval = viewport.screenCoordinates( coordinates, x, y, globeHidesPoint );

        QVERIFY( retval ); // FIXME: this should fail for lon < -180 || 180 < lon
        QVERIFY( !globeHidesPoint );
        QCOMPARE( x, lon - viewport.centerLongitude() * RAD2DEG + 1.0 );
        QCOMPARE( y, 1.0 );
    }

    QVERIFY( viewport.currentProjection()->repeatX() );

    {
        qreal x[2];
        qreal y;
        int pointRepeatNum = 1000;
        bool globeHidesPoint = true;

        const bool retval = viewport.screenCoordinates( coordinates, x, y, pointRepeatNum, QSizeF( 0, 0 ), globeHidesPoint );

        QVERIFY( retval );
        QCOMPARE( pointRepeatNum, 1 );
        QVERIFY( !globeHidesPoint );
        QCOMPARE( x[0], 1.0 );
        QCOMPARE( y, 1.0 );
    }
}

}

QTEST_MAIN( Marble::MercatorProjectionTest )

#include "MercatorProjectionTest.moc"
