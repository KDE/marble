//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoDataPoint.h"
#include "GeoDataLinearRing.h"

#include <QObject>
#include <QTest>

using namespace Marble;


class TestGeoDataGeometry : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void downcastPointTest_data();
    void downcastPointTest();
    void deleteAndDetachTest1();
    void deleteAndDetachTest2();
    void deleteAndDetachTest3();
};

void TestGeoDataGeometry::downcastPointTest_data()
{
    QTest::addColumn<GeoDataPoint>("point");

    GeoDataPoint point1;
    point1.setCoordinates( GeoDataCoordinates(.5, .2, 100) );
    QTest::newRow("First") << point1;
}

void TestGeoDataGeometry::downcastPointTest()
{
    QFETCH(GeoDataPoint, point);

    QVERIFY( ! point.coordinates().toString().isEmpty() );

    GeoDataCoordinates tmp( point.coordinates() );
    GeoDataPoint newPoint( tmp );

    QCOMPARE( newPoint.coordinates().toString(), point.coordinates().toString() );
}

/**
 * Test passes if the program does not crash
 */
void TestGeoDataGeometry::deleteAndDetachTest1()
{
    GeoDataLineString line1;
    line1 << GeoDataCoordinates();
    line1.toRangeCorrected();
    GeoDataLineString line2 = line1;
    line2 << GeoDataCoordinates();
}

/**
 * Test passes if the program does not crash
 */
void TestGeoDataGeometry::deleteAndDetachTest2()
{
    GeoDataLineString line1;
    line1 << GeoDataCoordinates();
    GeoDataLineString line2 = line1;
    line1.toRangeCorrected();
    line2 << GeoDataCoordinates();
}

/**
 * Test passes if the program does not crash
 */
void TestGeoDataGeometry::deleteAndDetachTest3()
{
    GeoDataLineString line1;
    line1 << GeoDataCoordinates();
    GeoDataLineString line2 = line1;
    line2.toRangeCorrected();
    line2 << GeoDataCoordinates();
}

QTEST_MAIN( TestGeoDataGeometry )
#include "TestGeoDataGeometry.moc"

