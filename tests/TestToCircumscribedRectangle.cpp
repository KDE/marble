
//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Adrian Draghici <draghici.adrian.b@gmail.com>
//


#include "MarbleGlobal.h"
#include "GeoDataLatLonBox.h"
#include "TestUtils.h"

namespace Marble
{

class TestToCircumscribedRectangle : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testToCircumscribedRectangle_data();
    void testToCircumscribedRectangle();
};

void TestToCircumscribedRectangle::testToCircumscribedRectangle_data()
{
    QTest::addColumn<qreal>( "north" );
    QTest::addColumn<qreal>( "south" );
    QTest::addColumn<qreal>( "east" );
    QTest::addColumn<qreal>( "west" );
    QTest::addColumn<qreal>( "rotation" );

    QTest::addColumn<qreal>( "expectedNorth" );
    QTest::addColumn<qreal>( "expectedSouth" );
    QTest::addColumn<qreal>( "expectedEast" );
    QTest::addColumn<qreal>( "expectedWest" );

    addRow() << 5.0 << -5.0 << 5.0 << -5.0 << 45.0 << 7.07107 << -7.07107 << 7.07107 << -7.07107;
    addRow() << 50.0 << 30.0 << 170.0 << 160.0 << 90.0 << 45.0 << 35.0 << 175.0 << 155.0;
    addRow() << 50.0 << 30.0 << 40.0 << 10.0 << 10.0 << 52.4528 << 27.5472 << 41.5086 << 8.4914;
    addRow() << 50.0 << 30.0 << -170.0 << 170.0 << 90.0 << 50.0 << 30.0 << -170.0 << 170.0;
    addRow() << 50.0 << 30.0 << 20.0 << -10.0 << 90.0 << 55.0 << 25.0 << 15.0 << -5.0;
}

void TestToCircumscribedRectangle::testToCircumscribedRectangle()
{
    QFETCH( qreal, north );
    QFETCH( qreal, south );
    QFETCH( qreal, east );
    QFETCH( qreal, west );
    QFETCH( qreal, rotation );

    QFETCH( qreal, expectedNorth );
    QFETCH( qreal, expectedSouth );
    QFETCH( qreal, expectedEast );
    QFETCH( qreal, expectedWest );

    GeoDataLatLonBox box = GeoDataLatLonBox( north, south, east, west, GeoDataCoordinates::Degree );
    box.setRotation( rotation, GeoDataCoordinates::Degree );

    GeoDataLatLonBox circumscribedRectangle = box.toCircumscribedRectangle();

    QFUZZYCOMPARE( circumscribedRectangle.north()  * RAD2DEG, expectedNorth, 0.00001 );
    QFUZZYCOMPARE( circumscribedRectangle.south() * RAD2DEG, expectedSouth, 0.00001  );
    QFUZZYCOMPARE( circumscribedRectangle.east() * RAD2DEG, expectedEast, 0.00001  );
    QFUZZYCOMPARE( circumscribedRectangle.west() * RAD2DEG, expectedWest, 0.00001  );
}

}

QTEST_MAIN( Marble::TestToCircumscribedRectangle )

#include "TestToCircumscribedRectangle.moc"
