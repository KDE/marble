//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.

// Copyright 2009       Bastian Holst <bastianholst@gmx.de>
// Copyright 2010       Cezar Mocan <mocancezar@gmail.com>
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "GeoDataCoordinates.h"
#include "GeoDataFolder.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataPlacemark.h"

#include <QTest>
#include <QBuffer>

using namespace Marble;

class TestGeoDataLatLonAltBox : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultConstruction();
    void testCopyConstruction_data();
    void testCopyConstruction();
    void testConstructionFromLatLonBox_data();
    void testConstructionFromLatLonBox();
    void testAssignment_data();
    void testAssignment();
    void testContstructionFromCoordinates_data();
    void testContstructionFromCoordinates();
    void testAltitude_data();
    void testAltitude();
    void testSetNorthRadian_data();
    void testSetNorthRadian();
    void testSetSouthRadian_data();
    void testSetSouthRadian();
    void testContains();
    void testIntersects_data();
    void testIntersects();
    void testCrossesDateline_data();
    void testCrossesDateline();
    void testCenter_data();
    void testCenter();
    void testUnited_data();
    void testUnited();

    void testFromLineString_data();
    void testFromLineString();
    void testToString_data();
    void testToString();
    void testPack_data();
    void testPack();
    void testContainerBox_data();
    void testContainerBox();
    void testScale_data();
    void testScale();
};

void TestGeoDataLatLonAltBox::testDefaultConstruction()
{
    GeoDataLatLonBox const latLonBox;

    QCOMPARE( latLonBox.north(), 0.0 );
    QCOMPARE( latLonBox.south(), 0.0 );
    QCOMPARE( latLonBox.east(), 0.0 );
    QCOMPARE( latLonBox.west(), 0.0 );
    QCOMPARE( latLonBox.rotation(), 0.0 );
    QCOMPARE( latLonBox.width(), 0.0 );
    QCOMPARE( latLonBox.height(), 0.0 );
    QVERIFY( !latLonBox.crossesDateLine() );
    QCOMPARE( latLonBox.center(), GeoDataCoordinates( 0, 0 ) );
    QVERIFY( latLonBox.isNull() );
    QVERIFY( latLonBox.isEmpty() );

    QVERIFY( (latLonBox|latLonBox).isNull() );
    QVERIFY( (latLonBox|latLonBox).isEmpty() );
    QVERIFY( !latLonBox.intersects( latLonBox ) );


    GeoDataLatLonAltBox const latLonAltBox;

    QCOMPARE( latLonAltBox.north(), 0.0 );
    QCOMPARE( latLonAltBox.south(), 0.0 );
    QCOMPARE( latLonAltBox.east(), 0.0 );
    QCOMPARE( latLonAltBox.west(), 0.0 );
    QCOMPARE( latLonAltBox.rotation(), 0.0 );
    QCOMPARE( latLonAltBox.width(), 0.0 );
    QCOMPARE( latLonAltBox.height(), 0.0 );
    QVERIFY( !latLonAltBox.crossesDateLine() );
    QCOMPARE( latLonAltBox.center(), GeoDataCoordinates( 0, 0, 0 ) );
    QVERIFY( latLonAltBox.isNull() );
    QVERIFY( latLonAltBox.isEmpty() );
    QCOMPARE( latLonAltBox.minAltitude(), 0.0 );
    QCOMPARE( latLonAltBox.maxAltitude(), 0.0 );
    QCOMPARE( latLonAltBox.altitudeMode(), ClampToGround );

    QVERIFY( (latLonAltBox|latLonAltBox).isNull() );
    QVERIFY( (latLonAltBox|latLonAltBox).isEmpty() );
    QVERIFY( !latLonAltBox.intersects( latLonAltBox ) );
}

void TestGeoDataLatLonAltBox::testCopyConstruction_data()
{
    QTest::addColumn<GeoDataLatLonAltBox>("expected");

    QTest::newRow("null") << GeoDataLatLonAltBox();
}

void TestGeoDataLatLonAltBox::testCopyConstruction()
{
    QFETCH(GeoDataLatLonAltBox, expected);

    GeoDataLatLonAltBox const result(expected);

    QCOMPARE(result, expected);
}

void TestGeoDataLatLonAltBox::testConstructionFromLatLonBox_data()
{
    QTest::addColumn<GeoDataLatLonBox>("latLonBox");
    QTest::addColumn<qreal>("minAltitude");
    QTest::addColumn<qreal>("maxAltitude");

    QTest::newRow("deg") << GeoDataLatLonBox(15.0, 180.0, 90.0, 118.0, GeoDataCoordinates::Degree) << qreal(143.0) << qreal(356.0);
    QTest::newRow("rad") << GeoDataLatLonBox(1.0, 2.2, 1.8, 1.4, GeoDataCoordinates::Radian) << qreal(112.0) << qreal(120.0);
}

void TestGeoDataLatLonAltBox::testConstructionFromLatLonBox()
{
    QFETCH(GeoDataLatLonBox, latLonBox);
    QFETCH(qreal, minAltitude);
    QFETCH(qreal, maxAltitude);

    GeoDataLatLonAltBox const box(latLonBox, minAltitude, maxAltitude);

    QCOMPARE(box.west(), latLonBox.west());
    QCOMPARE(box.east(), latLonBox.east());
    QCOMPARE(box.north(), latLonBox.north());
    QCOMPARE(box.south(), latLonBox.south());
    QCOMPARE(box.rotation(), latLonBox.rotation());
    QCOMPARE(box.minAltitude(), minAltitude);
    QCOMPARE(box.maxAltitude(), maxAltitude);
}

void TestGeoDataLatLonAltBox::testAssignment_data()
{
    QTest::addColumn<GeoDataLatLonAltBox>("expected");

    QTest::newRow("deg") << GeoDataLatLonAltBox(GeoDataLatLonBox( 15.0, 180.0, 90.0, 118.0, GeoDataCoordinates::Degree), 143.0, 356.0);
    QTest::newRow("rad") << GeoDataLatLonAltBox(GeoDataLatLonBox( 1.0, 2.2, 1.8, 1.4, GeoDataCoordinates::Radian ), 112.0, 120.0);
}

void TestGeoDataLatLonAltBox::testAssignment()
{
    QFETCH(GeoDataLatLonAltBox, expected);

    GeoDataLatLonAltBox other = expected;

    QCOMPARE( expected, other );
}

void TestGeoDataLatLonAltBox::testContstructionFromCoordinates_data()
{
    QTest::addColumn<GeoDataCoordinates>("coordinates");

    QTest::newRow("deg") << GeoDataCoordinates(90.0, 15.0, 10.0, GeoDataCoordinates::Degree);
    QTest::newRow("rad") << GeoDataCoordinates(1.8, 1.0, 61.0, GeoDataCoordinates::Radian);
}

void TestGeoDataLatLonAltBox::testContstructionFromCoordinates()
{
    QFETCH(GeoDataCoordinates, coordinates);

    GeoDataLatLonAltBox const box(coordinates);

    QCOMPARE(box.east(), coordinates.longitude());
    QCOMPARE(box.west(), coordinates.longitude());
    QCOMPARE(box.north(), coordinates.latitude());
    QCOMPARE(box.south(), coordinates.latitude());
    QCOMPARE(box.minAltitude(), coordinates.altitude());
    QCOMPARE(box.maxAltitude(), coordinates.altitude());
}

void TestGeoDataLatLonAltBox::testAltitude_data() 
{
    QTest::addColumn<qreal>("alt");

    QTest::newRow("Altitude 1") << qreal(27.2);
    QTest::newRow("Altitude 2") << qreal(0.22);
}

void TestGeoDataLatLonAltBox::testAltitude() 
{
    QFETCH(qreal, alt);

    GeoDataLatLonAltBox box;
    box.setMinAltitude(alt);
    QCOMPARE(box.minAltitude(), alt);

    box.setMaxAltitude(alt);
    QCOMPARE(box.maxAltitude(), alt);
}

void TestGeoDataLatLonAltBox::testSetNorthRadian_data()
{
    QTest::addColumn<GeoDataLatLonAltBox>("box");
    QTest::addColumn<qreal>("north");

    QTest::newRow("deg") << GeoDataLatLonAltBox(GeoDataLatLonBox( 15.0, 180.0, 90.0, 118.0, GeoDataCoordinates::Degree), 143.0, 356.0) << qreal(0.1);
    QTest::newRow("rad") << GeoDataLatLonAltBox(GeoDataLatLonBox( 1.0, 2.2, 1.8, 1.4, GeoDataCoordinates::Radian ), 112.0, 120.0) << qreal(0.1);
}

void TestGeoDataLatLonAltBox::testSetNorthRadian()
{
    QFETCH(GeoDataLatLonAltBox, box);
    QFETCH(qreal, north);

    box.setNorth( north );

    QCOMPARE( box.north(), north );
}

void TestGeoDataLatLonAltBox::testSetSouthRadian_data()
{
    QTest::addColumn<GeoDataLatLonAltBox>("box");
    QTest::addColumn<qreal>("south");

    QTest::newRow("deg") << GeoDataLatLonAltBox(GeoDataLatLonBox( 15.0, 180.0, 90.0, 118.0, GeoDataCoordinates::Degree), 143.0, 356.0) << qreal(1.4);
    QTest::newRow("rad") << GeoDataLatLonAltBox(GeoDataLatLonBox( 1.0, 2.2, 1.8, 1.4, GeoDataCoordinates::Radian ), 112.0, 120.0) << qreal(1.4);
}

void TestGeoDataLatLonAltBox::testSetSouthRadian()
{
    QFETCH(GeoDataLatLonAltBox, box);
    QFETCH(qreal, south);

    box.setSouth( south );

    QCOMPARE( box.south(), south );
}

void TestGeoDataLatLonAltBox::testContains()
{
    GeoDataLatLonAltBox const largeBox = GeoDataLatLonAltBox::fromLineString( GeoDataLineString()
            << GeoDataCoordinates( -20.0, +10.0, 15.0, GeoDataCoordinates::Degree )
            << GeoDataCoordinates( +20.0, -10.0, 25.0, GeoDataCoordinates::Degree ) );
    GeoDataLatLonAltBox const smallBox = GeoDataLatLonAltBox::fromLineString( GeoDataLineString()
            << GeoDataCoordinates( -2.0, +1.0, 18.0, GeoDataCoordinates::Degree )
            << GeoDataCoordinates( +2.0, -1.0, 22.0, GeoDataCoordinates::Degree ) );

    QVERIFY( largeBox.contains( GeoDataCoordinates( 5.0, 5.0, 20.0, GeoDataCoordinates::Degree ) ) );
    QVERIFY( largeBox.contains( smallBox ) );
    QVERIFY( largeBox.contains( largeBox ) );
    QVERIFY( !smallBox.contains( largeBox ) );
    QVERIFY(  smallBox.contains( GeoDataCoordinates(    0.0,   0.0, 20.0, GeoDataCoordinates::Degree ) ) );
    QVERIFY( !largeBox.contains( GeoDataCoordinates(   5.0,   5.0, 30.0, GeoDataCoordinates::Degree ) ) );
    QVERIFY( !largeBox.contains( GeoDataCoordinates(   5.0,   5.0, 10.0, GeoDataCoordinates::Degree ) ) );
    QVERIFY( !largeBox.contains( GeoDataCoordinates(  35.0,   5.0, 20.0, GeoDataCoordinates::Degree ) ) );
    QVERIFY( !largeBox.contains( GeoDataCoordinates( -35.0,   5.0, 20.0, GeoDataCoordinates::Degree ) ) );
    QVERIFY( !largeBox.contains( GeoDataCoordinates(   5.0,  35.0, 20.0, GeoDataCoordinates::Degree ) ) );
    QVERIFY( !largeBox.contains( GeoDataCoordinates(   5.0, -35.0, 20.0, GeoDataCoordinates::Degree ) ) );
}

void TestGeoDataLatLonAltBox::testIntersects_data()
{
    QTest::addColumn<GeoDataLatLonBox>( "latLonBox1" );
    QTest::addColumn<qreal>( "box1minAltitude" );
    QTest::addColumn<qreal>( "box1maxAltitude" );
    QTest::addColumn<GeoDataLatLonBox>( "latLonBox2" );
    QTest::addColumn<qreal>( "box2minAltitude" );
    QTest::addColumn<qreal>( "box2maxAltitude" );
    QTest::addColumn<bool>( "intersects" );

    QTest::newRow( "empty1" ) << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 ) << qreal(0.0) << qreal(0.0)
                              << GeoDataLatLonBox() << qreal(0.0) << qreal(0.0)
                              << false;
    QTest::newRow( "empty2" ) << GeoDataLatLonBox() << qreal(0.0) << qreal(0.0)
                              << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 ) << qreal(0.0) << qreal(0.0)
                              << false;
    QTest::newRow( "same" ) << GeoDataLatLonBox( 56.0, 40.0, 11.0, 0.0, GeoDataCoordinates::Degree ) << qreal(10.0) << qreal(12.0)
                            << GeoDataLatLonBox( 56.0, 40.0, 11.0, 0.0, GeoDataCoordinates::Degree ) << qreal(10.0) << qreal(12.0)
                            << true;
    QTest::newRow( "dateLineFalse" ) << GeoDataLatLonBox( 30.0, -30.0,  170.0, -170.0, GeoDataCoordinates::Degree ) << qreal(0.0) << qreal(0.0)
                                     << GeoDataLatLonBox( 30.0, -30.0, -171.0,  171.0, GeoDataCoordinates::Degree ) << qreal(0.0) << qreal(0.0)
                                     << false;
    QTest::newRow( "dateLineTrue" ) << GeoDataLatLonBox( 20.0,   0.0,  171.0, -171.0, GeoDataCoordinates::Degree ) << qreal(0.0) << qreal(0.0)
                                    << GeoDataLatLonBox( 30.0, -30.0, -170.0,  170.0, GeoDataCoordinates::Degree ) << qreal(0.0) << qreal(0.0)
                                    << true;
}

void TestGeoDataLatLonAltBox::testIntersects()
{
    QFETCH( GeoDataLatLonBox, latLonBox1 );
    QFETCH( qreal, box1minAltitude );
    QFETCH( qreal, box1maxAltitude );
    QFETCH( GeoDataLatLonBox, latLonBox2 );
    QFETCH( qreal, box2minAltitude );
    QFETCH( qreal, box2maxAltitude );
    QFETCH( bool, intersects );

    const GeoDataLatLonAltBox box1( latLonBox1, box1minAltitude, box1maxAltitude );
    const GeoDataLatLonAltBox box2( latLonBox2, box2minAltitude, box2maxAltitude );

    QCOMPARE( box1.intersects( box2 ), intersects );
}

void TestGeoDataLatLonAltBox::testCrossesDateline_data()
{
    QTest::addColumn<GeoDataLatLonBox>("box");
    QTest::addColumn<bool>("expected");

    QTest::newRow("all") << GeoDataLatLonBox(90, -90, 179.999, -180, GeoDataCoordinates::Degree) << false;

    QTest::newRow("left")  << GeoDataLatLonBox(90, -90,   0, -180, GeoDataCoordinates::Degree) << false;
    QTest::newRow("front") << GeoDataLatLonBox(90, -90,  90,  -90, GeoDataCoordinates::Degree) << false;
    QTest::newRow("right") << GeoDataLatLonBox(90, -90, 180,    0, GeoDataCoordinates::Degree) << false;
    QTest::newRow("back")  << GeoDataLatLonBox(90, -90, -90,   90, GeoDataCoordinates::Degree) << true;
}

void TestGeoDataLatLonAltBox::testCrossesDateline()
{
    QFETCH(GeoDataLatLonBox, box);
    QFETCH(bool, expected);

    bool const result = box.crossesDateLine();

    QCOMPARE(result, expected);
}

void TestGeoDataLatLonAltBox::testCenter_data()
{
    QTest::addColumn<GeoDataLatLonBox>( "box" );
    QTest::addColumn<GeoDataCoordinates>( "center" );

    QTest::newRow( "N-E" ) << GeoDataLatLonBox( 60.0, 40.0, 30.0, 10.0, GeoDataCoordinates::Degree )
                           << GeoDataCoordinates( 20.0, 50.0, 0, GeoDataCoordinates::Degree );

    QTest::newRow( "N-GW" ) << GeoDataLatLonBox( 60.0, 40.0, 10.0, -30.0, GeoDataCoordinates::Degree )
                            << GeoDataCoordinates( -10.0, 50.0, 0, GeoDataCoordinates::Degree );

    QTest::newRow( "N-W" ) << GeoDataLatLonBox( 60.0, 40.0, -10.0, -30.0, GeoDataCoordinates::Degree )
                           << GeoDataCoordinates( -20.0, 50.0, 0, GeoDataCoordinates::Degree );

    QTest::newRow( "NS-W" ) << GeoDataLatLonBox( 30.0, -30.0, -10.0, -30.0, GeoDataCoordinates::Degree )
                            << GeoDataCoordinates( -20.0, 0.0, 0, GeoDataCoordinates::Degree );

    QTest::newRow( "N-IDL" ) << GeoDataLatLonBox( 30.0, -30.0, -150.0, 170.0, GeoDataCoordinates::Degree )
                             << GeoDataCoordinates( -170.0, 0.0, 0, GeoDataCoordinates::Degree );
}

void TestGeoDataLatLonAltBox::testCenter()
{
    QFETCH( GeoDataLatLonBox, box );
    QFETCH( GeoDataCoordinates, center );

    QCOMPARE( box.center().latitude(), center.latitude() );
    QCOMPARE( box.center().longitude(), center.longitude() );
}

void TestGeoDataLatLonAltBox::testUnited_data()
{
    QTest::addColumn<GeoDataLatLonBox>( "box1" );
    QTest::addColumn<GeoDataLatLonBox>( "box2" );
    QTest::addColumn<GeoDataLatLonBox>( "expected" );

    QTest::newRow( "emptyRight" ) << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 )
                                  << GeoDataLatLonBox()
                                  << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 );

    QTest::newRow( "emptyLeft" ) << GeoDataLatLonBox()
                                 << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 )
                                 << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 );

    QTest::newRow( "same" ) << GeoDataLatLonBox( 56.0, 40.0, 11.0, 0.0, GeoDataCoordinates::Degree )
                            << GeoDataLatLonBox( 56.0, 40.0, 11.0, 0.0, GeoDataCoordinates::Degree )
                            << GeoDataLatLonBox( 56.0, 40.0, 11.0, 0.0, GeoDataCoordinates::Degree );

    // 2 boxes in West, result stays west
    QTest::newRow( "bigWest" ) << GeoDataLatLonBox( 30.0, -30.0,  -10.0,  -30.0, GeoDataCoordinates::Degree )
                               << GeoDataLatLonBox( 30.0, -30.0, -150.0, -170.0, GeoDataCoordinates::Degree )
                               << GeoDataLatLonBox( 30.0, -30.0,  -10.0, -170.0, GeoDataCoordinates::Degree );

    // 2 boxes each side of greenwich, result crosses greenwich
    QTest::newRow( "aroundGreenwich" ) << GeoDataLatLonBox( 30.0, -30.0, -10.0, -30.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0,  30.0,  10.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0,  30.0, -30.0, GeoDataCoordinates::Degree );

    // 2 boxes crossing greenwich, result crosses greenwich
    QTest::newRow( "aroundGreenwich" ) << GeoDataLatLonBox( 30.0, -30.0, 10.0, -30.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0, 30.0, -10.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0, 30.0, -30.0, GeoDataCoordinates::Degree );

    // 2 boxes each side of IDL, result crosses IDL as smaller box
    QTest::newRow( "aroundIDL" ) << GeoDataLatLonBox( 30.0, -30.0, -150.0, -170.0, GeoDataCoordinates::Degree )
                                 << GeoDataLatLonBox( 30.0, -30.0,  170.0,  150.0, GeoDataCoordinates::Degree )
                                 << GeoDataLatLonBox( 30.0, -30.0, -150.0,  150.0, GeoDataCoordinates::Degree );

    // reciprocical, so independent of side
    QTest::newRow( "aroundIDL2" ) << GeoDataLatLonBox( 30.0, -30.0,  170.0,  150.0, GeoDataCoordinates::Degree )
                                  << GeoDataLatLonBox( 30.0, -30.0, -150.0, -170.0, GeoDataCoordinates::Degree )
                                  << GeoDataLatLonBox( 30.0, -30.0, -150.0,  150.0, GeoDataCoordinates::Degree );

    // 1 box crossing IDL, the 2 centers are close together, result crosses IDL
    QTest::newRow( "crossingIDLclose" ) << GeoDataLatLonBox( 30.0, -30.0, -130.0, -150.0, GeoDataCoordinates::Degree )
                                        << GeoDataLatLonBox( 30.0, -30.0, -150.0,  170.0, GeoDataCoordinates::Degree )
                                        << GeoDataLatLonBox( 30.0, -30.0, -130.0,  170.0, GeoDataCoordinates::Degree );

    // reciprocical
    QTest::newRow( "crossingIDLclose2" ) << GeoDataLatLonBox( 30.0, -30.0, -160.0,  170.0, GeoDataCoordinates::Degree )
                                         << GeoDataLatLonBox( 30.0, -30.0, -140.0, -150.0, GeoDataCoordinates::Degree )
                                         << GeoDataLatLonBox( 30.0, -30.0, -140.0,  170.0, GeoDataCoordinates::Degree );

    // 1 box crossing IDL, the 2 centers are across IDL, result crosses IDL
    QTest::newRow( "crossingIDLfar" ) << GeoDataLatLonBox( 30.0, -30.0, -150.0, -170.0, GeoDataCoordinates::Degree )
                                      << GeoDataLatLonBox( 30.0, -30.0, -170.0,  150.0, GeoDataCoordinates::Degree )
                                      << GeoDataLatLonBox( 30.0, -30.0, -150.0,  150.0, GeoDataCoordinates::Degree );

    // reciprocical
    QTest::newRow( "crossingIDLfar2" ) << GeoDataLatLonBox( 30.0, -30.0, -170.0,  150.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0, -150.0, -170.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0, -150.0,  150.0, GeoDataCoordinates::Degree );

    // 2 box crossing IDL, the 2 centers are close together, result crosses IDL
    QTest::newRow( "crossingsIDLclose" ) << GeoDataLatLonBox( 30.0, -30.0, -140.0, 160.0, GeoDataCoordinates::Degree )
                                         << GeoDataLatLonBox( 30.0, -30.0, -160.0, 170.0, GeoDataCoordinates::Degree )
                                         << GeoDataLatLonBox( 30.0, -30.0, -140.0, 160.0, GeoDataCoordinates::Degree );

    // 2 box crossing IDL, the 2 centers are across IDL, result crosses IDL
    QTest::newRow( "crossingsIDLfar" ) << GeoDataLatLonBox( 30.0, -30.0, -150.0, -170.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0, -170.0,  150.0, GeoDataCoordinates::Degree )
                                       << GeoDataLatLonBox( 30.0, -30.0, -150.0,  150.0, GeoDataCoordinates::Degree );

    QTest::newRow( "bug299959" ) << GeoDataLatLonBox( 90.0, -90.0, 180.0, -180.0, GeoDataCoordinates::Degree )
                                 << GeoDataLatLonBox( 18.0, -18.0,  30.0,   20.0, GeoDataCoordinates::Degree )
                                 << GeoDataLatLonBox( 90.0, -90.0, 180.0, -180.0, GeoDataCoordinates::Degree );
}

void TestGeoDataLatLonAltBox::testUnited()
{
    QFETCH( GeoDataLatLonBox, box1 );
    QFETCH( GeoDataLatLonBox, box2 );
    QFETCH( GeoDataLatLonBox, expected );

    GeoDataLatLonBox const result = box1 | box2;

    QCOMPARE( result.north( GeoDataCoordinates::Degree ), expected.north( GeoDataCoordinates::Degree ) );
    QCOMPARE( result.south( GeoDataCoordinates::Degree ), expected.south( GeoDataCoordinates::Degree ) );
    QCOMPARE( result.west( GeoDataCoordinates::Degree ), expected.west( GeoDataCoordinates::Degree ) );
    QCOMPARE( result.east( GeoDataCoordinates::Degree ), expected.east( GeoDataCoordinates::Degree ) );
}

void TestGeoDataLatLonAltBox::testFromLineString_data() 
{
    QTest::addColumn<GeoDataLineString>("string");
    QTest::addColumn<GeoDataLatLonBox>("expected");

    QTest::newRow("empty") << GeoDataLineString() << GeoDataLatLonBox(0, 0, 0, 0);

    QTest::newRow("bug 299527")
            << (GeoDataLineString()
                 << GeoDataCoordinates(   0, 0, 200, GeoDataCoordinates::Degree)
                 << GeoDataCoordinates( 180, 0, 200, GeoDataCoordinates::Degree)
                 << GeoDataCoordinates(-180, 0, 200, GeoDataCoordinates::Degree)
                 << GeoDataCoordinates(   0, 0, 200, GeoDataCoordinates::Degree))
            << GeoDataLatLonBox(90.0, 0.0, 180, -180, GeoDataCoordinates::Degree);

    QTest::newRow("around south pole")
            << (GeoDataLineString()
                 << GeoDataCoordinates(   0, -10, 200, GeoDataCoordinates::Degree)
                 << GeoDataCoordinates( 180, -10, 200, GeoDataCoordinates::Degree)
                 << GeoDataCoordinates(-180, -10, 200, GeoDataCoordinates::Degree)
                 << GeoDataCoordinates(   0, -10, 200, GeoDataCoordinates::Degree))
            << GeoDataLatLonBox(-10.0, -90.0, 180, -180, GeoDataCoordinates::Degree);
}

void TestGeoDataLatLonAltBox::testFromLineString() {
    QFETCH(GeoDataLineString, string);
    QFETCH(GeoDataLatLonBox, expected);

    GeoDataLatLonAltBox const result = GeoDataLatLonAltBox::fromLineString(string);

    QCOMPARE(result.north(), expected.north());
    QCOMPARE(result.south(), expected.south());
    QCOMPARE(result.east(), expected.east());
    QCOMPARE(result.west(), expected.west());
}

void TestGeoDataLatLonAltBox::testToString_data() {
    
}

void TestGeoDataLatLonAltBox::testToString() {
    
}

void TestGeoDataLatLonAltBox::testPack_data() {
    QTest::addColumn<GeoDataCoordinates>("coordinates");

    QTest::newRow("rad1") << GeoDataCoordinates( 1.0, 2.2, 1.8, GeoDataCoordinates::Radian );
    QTest::newRow("rad2") << GeoDataCoordinates( 0.2, 3.1, 2.9, GeoDataCoordinates::Radian );
}

void TestGeoDataLatLonAltBox::testPack() {
    QFETCH(GeoDataCoordinates, coordinates);

    GeoDataLatLonAltBox const original = GeoDataLatLonAltBox(coordinates);

    QBuffer buffer;
    bool const isOpenForWriting = buffer.open(QBuffer::WriteOnly);

    QVERIFY(isOpenForWriting);

    QDataStream out(&buffer);
    original.pack(out);
    buffer.close();

    bool const isOpenForReading = buffer.open(QBuffer::ReadOnly);

    QVERIFY(isOpenForReading);

    QDataStream in(&buffer);

    GeoDataLatLonAltBox unpacked;
    unpacked.unpack(in);

    buffer.close();

#if 0
    QCOMPARE(unpacked.north(), original.north());
    QCOMPARE(unpacked.south(), original.south());
    QCOMPARE(unpacked.east(), original.east());
    QCOMPARE(unpacked.west(), original.west());
#endif

    QCOMPARE(unpacked.maxAltitude(), original.maxAltitude());
    QCOMPARE(unpacked.minAltitude(), original.minAltitude());
    QCOMPARE(unpacked.altitudeMode(), original.altitudeMode());
}

void TestGeoDataLatLonAltBox::testContainerBox_data() {
    QTest::addColumn<qreal>("lon1");
    QTest::addColumn<qreal>("lat1");
    QTest::addColumn<qreal>("lon2");
    QTest::addColumn<qreal>("lat2");
    QTest::addColumn<qreal>("lon3");
    QTest::addColumn<qreal>("lat3");

    QTest::newRow("rad1") << qreal(2.4) << qreal(0.1) << qreal(1.0) << qreal(1.2) << qreal(-1.8) << qreal(-0.7) ;
    QTest::newRow("rad2") << qreal(-1.3) << qreal(-0.1) << qreal(0.2) << qreal(1.1) << qreal(2.9) << qreal(0.9) ;
}

void TestGeoDataLatLonAltBox::testContainerBox() {
    QFETCH(qreal, lon1);
    QFETCH(qreal, lat1);
    QFETCH(qreal, lon2);
    QFETCH(qreal, lat2);
    QFETCH(qreal, lon3);
    QFETCH(qreal, lat3);

    GeoDataPlacemark p1, p2, p3;
    p1.setCoordinate(lon1, lat1, GeoDataCoordinates::Degree);
    p2.setCoordinate(lon2, lat2, GeoDataCoordinates::Degree);
    p3.setCoordinate(lon3, lat3, GeoDataCoordinates::Degree);
    GeoDataFolder f1, f2;
    f1.append(new GeoDataPlacemark(p1));
    f2.append(new GeoDataPlacemark(p2));
    f2.append(new GeoDataPlacemark(p3));
    f1.append(new GeoDataFolder(f2));
    GeoDataLatLonAltBox box = f1.latLonAltBox();

    QCOMPARE(box.north(), qMax(qMax(lat1, lat2), lat3));
    QCOMPARE(box.east(), qMax(qMax(lon1, lon2), lon3));
    QCOMPARE(box.south(), qMin(qMin(lat1, lat2), lat3));
    QCOMPARE(box.west(), qMin(qMin(lon1, lon2), lon3));
}

void TestGeoDataLatLonAltBox::testScale_data()
{
    QTest::addColumn<GeoDataLatLonBox>( "box" );
    QTest::addColumn<qreal>( "verticalScale" );
    QTest::addColumn<qreal>( "horizontalScale" );
    QTest::addColumn<GeoDataLatLonBox>( "expected" );

    QTest::newRow( "void" ) << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 )
                            << qreal(1.0)
                            << qreal(1.0)
                            << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 );
    QTest::newRow( "simple vertical" ) << GeoDataLatLonBox( 0.5, 0.4, 0.3, 0.2 )
                                       << qreal(1.5)
                                       << qreal(1.0)
                                       << GeoDataLatLonBox( 0.525, 0.375, 0.3, 0.2 );
    QTest::newRow( "simple horizontal" ) << GeoDataLatLonBox( 0.7, 0.6, 0.3, 0.2 )
                                         << qreal(1.0)
                                         << qreal(2.0)
                                         << GeoDataLatLonBox( 0.7, 0.6, 0.35, 0.15 );
    QTest::newRow( "crosses dateline" )  << GeoDataLatLonBox( 20.0,   0.0, -170.0, 170.0, GeoDataCoordinates::Degree )
                                         << qreal(2.0)
                                         << qreal(2.0)
                                         << GeoDataLatLonBox( 30.0, -10.0, -160.0, 160.0, GeoDataCoordinates::Degree );
}

void TestGeoDataLatLonAltBox::testScale()
{
    QFETCH(GeoDataLatLonBox, box);
    QFETCH(qreal, verticalScale);
    QFETCH(qreal, horizontalScale);
    QFETCH(GeoDataLatLonBox, expected);
    GeoDataLatLonBox const scaled = box.scaled(verticalScale, horizontalScale);
    QCOMPARE(scaled.west(), expected.west());
    QCOMPARE(scaled.north(), expected.north());
    QCOMPARE(scaled.south(), expected.south());
    QCOMPARE(scaled.east(), expected.east());
    QCOMPARE(scaled.rotation(), expected.rotation());
    QCOMPARE(scaled.center(), expected.center());
}

QTEST_MAIN(TestGeoDataLatLonAltBox)
#include "TestGeoDataLatLonAltBox.moc"

