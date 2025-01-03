// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"

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
    point1.setCoordinates(GeoDataCoordinates(.5, .2, 100));
    QTest::newRow("First") << point1;
}

void TestGeoDataGeometry::downcastPointTest()
{
    QFETCH(GeoDataPoint, point);

    QVERIFY(!point.coordinates().toString().isEmpty());

    GeoDataCoordinates tmp(point.coordinates());
    GeoDataPoint newPoint(tmp);

    QCOMPARE(newPoint.coordinates().toString(), point.coordinates().toString());
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

QTEST_MAIN(TestGeoDataGeometry)
#include "TestGeoDataGeometry.moc"
