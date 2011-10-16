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
#include "MarbleModel.h"

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

class MarbleMapTest : public QObject
{
    Q_OBJECT

 private slots:
    void centerOnSpherical_data();
    void centerOnSpherical();

    void centerOnEquirectangular_data();
    void centerOnEquirectangular();

    void centerOnMercator_data();
    void centerOnMercator();

    void centerOnSphericalEquirectangularSingularities_data();
    void centerOnSphericalEquirectangularSingularities();

    void centerOnMercatorMinLat_data();
    void centerOnMercatorMinLat();

    void centerOnMercatorMaxLat_data();
    void centerOnMercatorMaxLat();

 private:
    MarbleModel m_model;
};

void MarbleMapTest::centerOnSpherical_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << 0.0 << 0.0;

    addRow() << -180.0 << 0.0;
    addRow() <<  -90.0 << 0.0;
    addRow() <<   90.0 << 0.0;
    addRow() <<  180.0 << 0.0;

    addRow() << -180.0 << 89.0;
    addRow() <<  -90.0 << 89.0;
    addRow() <<    0.0 << 89.0;
    addRow() <<   90.0 << 89.0;
    addRow() <<  180.0 << 89.0;

    addRow() << -180.0 << -89.0;
    addRow() <<  -90.0 << -89.0;
    addRow() <<    0.0 << -89.0;
    addRow() <<   90.0 << -89.0;
    addRow() <<  180.0 << -89.0;

    addRow() << -180.0 << 180.0;
    addRow() <<  -90.0 << 180.0;
    addRow() <<    0.0 << 180.0;
    addRow() <<   90.0 << 180.0;
    addRow() <<  180.0 << 180.0;

    addRow() << -180.0 << -180.0;
    addRow() <<  -90.0 << -180.0;
    addRow() <<    0.0 << -180.0;
    addRow() <<   90.0 << -180.0;
    addRow() <<  180.0 << -180.0;
}

void MarbleMapTest::centerOnSpherical()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    MarbleMap map( &m_model );
    map.setProjection( Spherical );

    map.centerOn( lon, lat );
    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), lat, 0.0001 );
}

void MarbleMapTest::centerOnEquirectangular_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << 0.0 << 0.0;

    addRow() << -180.0 << 0.0;
    addRow() <<  -90.0 << 0.0;
    addRow() <<   90.0 << 0.0;
    addRow() <<  180.0 << 0.0;

    addRow() << -180.0 << 89.0;
    addRow() <<  -90.0 << 89.0;
    addRow() <<    0.0 << 89.0;
    addRow() <<   90.0 << 89.0;
    addRow() <<  180.0 << 89.0;

    addRow() << -180.0 << -89.0;
    addRow() <<  -90.0 << -89.0;
    addRow() <<    0.0 << -89.0;
    addRow() <<   90.0 << -89.0;
    addRow() <<  180.0 << -89.0;
}

void MarbleMapTest::centerOnEquirectangular()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    MarbleMap map( &m_model );
    map.setProjection( Equirectangular );

    map.centerOn( lon, lat );
    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), lat, 0.0001 );
}

void MarbleMapTest::centerOnMercator_data()
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

void MarbleMapTest::centerOnMercator()
{
    MarbleMap map( &m_model );

    map.setProjection( Mercator );

    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    map.centerOn( lon, lat );

    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), lat, 0.0001 );
}

void MarbleMapTest::centerOnSphericalEquirectangularSingularities_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );

    addRow() << -180.0 << 90.0;
    addRow() <<  -90.0 << 90.0;
    addRow() <<    0.0 << 90.0;
    addRow() <<   90.0 << 90.0;
    addRow() <<  180.0 << 90.0;

    addRow() << -180.0 << -90.0;
    addRow() <<  -90.0 << -90.0;
    addRow() <<    0.0 << -90.0;
    addRow() <<   90.0 << -90.0;
    addRow() <<  180.0 << -90.0;
}

void MarbleMapTest::centerOnSphericalEquirectangularSingularities()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    MarbleMap sphericalMap( &m_model );
    sphericalMap.setProjection( Spherical );

    sphericalMap.centerOn( lon, lat );
    QFUZZYCOMPARE( sphericalMap.centerLatitude(), lat, 0.0001 );

    MarbleMap equirectangularMap( &m_model );
    equirectangularMap.setProjection( Equirectangular );

    equirectangularMap.centerOn( lon, lat );
    QFUZZYCOMPARE( equirectangularMap.centerLatitude(), lat, 0.0001 );
}

void MarbleMapTest::centerOnMercatorMinLat_data()
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

void MarbleMapTest::centerOnMercatorMinLat()
{
    MarbleMap map( &m_model );

    map.setProjection( Mercator );

    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    map.centerOn( lon, lat );

    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), -85.0511, 0.0001 ); // clip to minLat
}

void MarbleMapTest::centerOnMercatorMaxLat_data()
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

void MarbleMapTest::centerOnMercatorMaxLat()
{
    MarbleMap map( &m_model );

    map.setProjection( Mercator );

    QFETCH( qreal, lon );
    QFETCH( qreal, lat );

    map.centerOn( lon, lat );

    QFUZZYCOMPARE( map.centerLongitude(), lon, 0.0001 );
    QFUZZYCOMPARE( map.centerLatitude(), 85.0511, 0.0001 ); // clip to maxLat
}

}

QTEST_MAIN( Marble::MarbleMapTest )

#include "MarbleMapTest.moc"
