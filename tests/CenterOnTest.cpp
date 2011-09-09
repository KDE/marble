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
#include "MarbleMap.h"

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

class CenterOnTest : public QObject
{
    Q_OBJECT

 private slots:
    void testSphericalEquirectangular_data();
    void testSphericalEquirectangular();

    void testSphericalEquirectangularSingularities_data();
    void testSphericalEquirectangularSingularities();

    void testMercator_data();
    void testMercator();

    void testMercatorMinLat_data();
    void testMercatorMinLat();

    void testMercatorMaxLat_data();
    void testMercatorMaxLat();
};

void CenterOnTest::testSphericalEquirectangular_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << 0.0 << 0.0;

    addRow() << -180.0 << 0.0;
    addRow() <<  -90.0 << 0.0;
    addRow() <<   90.0 << 0.0;
    addRow() <<  180.0 << 0.0;

    addRow() << -180.0 << -89.0;
    addRow() << -180.0 <<  89.0;
    addRow() <<  -90.0 << -89.0;
    addRow() <<  -90.0 <<  89.0;
    addRow() <<    0.0 << -89.0;
    addRow() <<    0.0 <<  89.0;
    addRow() <<   90.0 << -89.0;
    addRow() <<   90.0 <<  89.0;
    addRow() <<  180.0 << -89.0;
    addRow() <<  180.0 <<  89.0;
}

void CenterOnTest::testSphericalEquirectangular()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    MarbleMap sphericalMap;
    sphericalMap.setProjection( Spherical );

    sphericalMap.centerOn( lon, lat );
    QFUZZYCOMPARE( sphericalMap.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( sphericalMap.centerLatitude(), lat, 0.0001 );

    MarbleMap equirectangularMap;
    equirectangularMap.setProjection( Equirectangular );

    equirectangularMap.centerOn( lon, lat );
    QFUZZYCOMPARE( equirectangularMap.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( equirectangularMap.centerLatitude(), lat, 0.0001 );
}

void CenterOnTest::testSphericalEquirectangularSingularities_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << -180.0 << -90.0;
    addRow() << -180.0 <<  90.0;
    addRow() <<  -90.0 << -90.0;
    addRow() <<  -90.0 <<  90.0;
    addRow() <<    0.0 << -90.0;
    addRow() <<    0.0 <<  90.0;
    addRow() <<   90.0 << -90.0;
    addRow() <<   90.0 <<  90.0;
    addRow() <<  180.0 << -90.0;
    addRow() <<  180.0 <<  90.0;
}

void CenterOnTest::testSphericalEquirectangularSingularities()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    MarbleMap sphericalMap;
    sphericalMap.setProjection( Spherical );

    sphericalMap.centerOn( lon, lat );
    QFUZZYCOMPARE( sphericalMap.centerLatitude(), lat, 0.0001 );

    MarbleMap equirectangularMap;
    equirectangularMap.setProjection( Equirectangular );

    equirectangularMap.centerOn( lon, lat );
    QFUZZYCOMPARE( equirectangularMap.centerLatitude(), lat, 0.0001 );
}

void CenterOnTest::testMercator_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << 0.0 << 0.0;

    addRow() << -180.0 << 0.0;
    addRow() <<  -90.0 << 0.0;
    addRow() <<   90.0 << 0.0;
    addRow() <<  180.0 << 0.0;

    addRow() << -180.0 << -85.0511;
    addRow() <<  -90.0 << -85.0511;
    addRow() <<    0.0 << -85.0511;
    addRow() <<   90.0 << -85.0511;
    addRow() <<  180.0 << -85.0511;

    addRow() << -180.0 << 85.0511;
    addRow() <<  -90.0 << 85.0511;
    addRow() <<    0.0 << 85.0511;
    addRow() <<   90.0 << 85.0511;
    addRow() <<  180.0 << 85.0511;
}

void CenterOnTest::testMercator()
{
    MarbleMap map;

    map.setProjection( Mercator );

    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    map.centerOn( lon, lat );

    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), lat, 0.0001 );
}

void CenterOnTest::testMercatorMinLat_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << -180.0 << -87.0;
    addRow() <<  -90.0 << -87.0;
    addRow() <<    0.0 << -87.0;
    addRow() <<   90.0 << -87.0;
    addRow() <<  180.0 << -87.0;

    addRow() << -180.0 << -90.0;
    addRow() <<  -90.0 << -90.0;
    addRow() <<    0.0 << -90.0;
    addRow() <<   90.0 << -90.0;
    addRow() <<  180.0 << -90.0;
}

void CenterOnTest::testMercatorMinLat()
{
    MarbleMap map;

    map.setProjection( Mercator );

    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    map.centerOn( lon, lat );

    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), -85.0511, 0.0001 ); // clip to minLat
}

void CenterOnTest::testMercatorMaxLat_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << -180.0 << 87.0;
    addRow() <<  -90.0 << 87.0;
    addRow() <<    0.0 << 87.0;
    addRow() <<   90.0 << 87.0;
    addRow() <<  180.0 << 87.0;

    addRow() << -180.0 << 90.0;
    addRow() <<  -90.0 << 90.0;
    addRow() <<    0.0 << 90.0;
    addRow() <<   90.0 << 90.0;
    addRow() <<  180.0 << 90.0;
}

void CenterOnTest::testMercatorMaxLat()
{
    MarbleMap map;

    map.setProjection( Mercator );

    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    map.centerOn( lon, lat );

    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), 85.0511, 0.0001 ); // clip to maxLat
}

}

QTEST_MAIN( Marble::CenterOnTest )

#include "CenterOnTest.moc"
