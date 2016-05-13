//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MercatorProjection.h"
#include "ViewportParams.h"
#include "TestUtils.h"

namespace Marble
{

class MercatorProjectionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void screenCoordinatesValidLat_data();
    void screenCoordinatesValidLat();

    void screenCoordinatesOfCenter_data();
    void screenCoordinatesOfCenter();

    void setInvalidRadius();
};

void MercatorProjectionTest::screenCoordinatesValidLat_data()
{
    ViewportParams mercator;
    mercator.setProjection( Mercator );

    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );
    QTest::addColumn<bool>( "validLat" );

    addRow() << qreal(0.0) << qreal(0.0) << true;

    addRow() << qreal(-180.0) << qreal(0.0) << true;
    addRow() <<  qreal(180.0) << qreal(0.0) << true;

    addRow() << qreal(0.0) << mercator.currentProjection()->minValidLat() * RAD2DEG << true;
    addRow() << qreal(0.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG << true;

    addRow() << qreal(0.0) << mercator.currentProjection()->minValidLat() * RAD2DEG - qreal(0.0001) << false;
    addRow() << qreal(0.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG + qreal(0.0001) << false;

    addRow() << qreal(-180.0) << mercator.currentProjection()->minValidLat() * RAD2DEG << true;
    addRow() <<  qreal(180.0) << mercator.currentProjection()->minValidLat() * RAD2DEG << true;

    addRow() << qreal(-180.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG << true;
    addRow() <<  qreal(180.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG << true;

    addRow() << qreal(-180.0) << mercator.currentProjection()->minValidLat() * RAD2DEG - qreal(0.0001) << false;
    addRow() <<  qreal(180.0) << mercator.currentProjection()->minValidLat() * RAD2DEG - qreal(0.0001) << false;

    addRow() << qreal(-180.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG + qreal(0.0001) << false;
    addRow() <<  qreal(180.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG + qreal(0.0001) << false;
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

    QVERIFY( viewport.currentProjection()->repeatableX() );

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
    ViewportParams mercator;
    mercator.setProjection( Mercator );

    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << qreal(0.0) << qreal(0.0);

    addRow() << qreal(-180.0) << qreal(0.0);
    addRow() <<  qreal(180.0) << qreal(0.0);

    addRow() << qreal(-360.0) << qreal(0.0);
    addRow() <<  qreal(360.0) << qreal(0.0);

    addRow() << qreal(-540.0) << qreal(0.0);
    addRow() <<  qreal(540.0) << qreal(0.0);

    addRow() << qreal(0.0) << mercator.currentProjection()->minValidLat() * RAD2DEG;
    addRow() << qreal(0.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG;

    addRow() << qreal(-180.0) << mercator.currentProjection()->minValidLat() * RAD2DEG;
    addRow() << qreal(-180.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG;

    addRow() <<  qreal(180.0) << mercator.currentProjection()->minValidLat() * RAD2DEG;
    addRow() <<  qreal(180.0) << mercator.currentProjection()->maxValidLat() * RAD2DEG;

    // FIXME: the following tests should succeed
#if 0
    addRow() << qreal(-541.0) << qreal(0.0);
    addRow() <<  qreal(541.0) << qreal(0.0);

    addRow() << qreal(-1000000.0) << qreal(0.0);
    addRow() <<  qreal(1000000.0) << qreal(0.0);
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

    QVERIFY( viewport.currentProjection()->repeatableX() );

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

void MercatorProjectionTest::setInvalidRadius()
{
    ViewportParams viewport;
    viewport.setProjection( Mercator );
    viewport.setRadius( 0 );
    qreal lon, lat;
    viewport.geoCoordinates( 23, 42, lon, lat );
}

}

QTEST_MAIN( Marble::MercatorProjectionTest )

#include "MercatorProjectionTest.moc"
