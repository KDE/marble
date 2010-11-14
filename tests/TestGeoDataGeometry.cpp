//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include "GeoDataPoint.h"

using namespace Marble;


class TestGeoDataGeometry : public QObject
{
    Q_OBJECT
private slots:
    void downcastPointTest_data();
    void downcastPointTest();
};

void TestGeoDataGeometry::downcastPointTest_data()
{
    QTest::addColumn<GeoDataPoint>("point");

    GeoDataPoint point1;
    point1.setLongitude( .5 );
    point1.setLatitude( .2 );
    point1.setAltitude( 100 );

    QTest::newRow("First") << point1;
}

void TestGeoDataGeometry::downcastPointTest()
{
    QFETCH(GeoDataPoint, point);

    QVERIFY( ! point.toString().isEmpty() );

    GeoDataGeometry tmpGeometry( point );
    GeoDataPoint newPoint( tmpGeometry );

    QCOMPARE( newPoint.toString() , point.toString());
}

QTEST_MAIN( TestGeoDataGeometry )
#include "TestGeoDataGeometry.moc"

