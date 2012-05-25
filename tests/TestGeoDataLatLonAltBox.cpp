//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.

// Copyright 2009       Bastian Holst <bastianholst@gmx.de>
// Copyright 2010       Cezar Mocan <mocancezar@gmail.com>

#include <QtTest/QtTest>
#include <QtGui/QApplication>
#include "MarbleGlobal.h"
#include "MarbleWidget.h"
#include "AbstractFloatItem.h"
#include "GeoDataCoordinates.h"
#include "GeoDataFolder.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataPlacemark.h"

using namespace Marble;

class TestGeoDataLatLonAltBox : public QObject
{
    Q_OBJECT

private slots:
    void testConstruction_data();
    void testConstruction();
    void testAltitude_data();
    void testAltitude();
    void testContains();
    void testIntersects_data();
    void testIntersects();
    void testCenter_data();
    void testCenter();
    void testUnited_data();
    void testUnited();
    void testIdlCrossing();
    void testDefaultConstruction();

    void testFromLineString_data();
    void testFromLineString();
    void testToString_data();
    void testToString();
    void testPack_data();
    void testPack();
    void testContainerBox_data();
    void testContainerBox();

private: 
    qreal randomLon();
    qreal randomLat();
};

void TestGeoDataLatLonAltBox::testConstruction_data() 
{
    QTest::addColumn<qreal>("north");
    QTest::addColumn<qreal>("south");
    QTest::addColumn<qreal>("east");
    QTest::addColumn<qreal>("west");
    QTest::addColumn<qreal>("alt");
    QTest::addColumn<QString>("unit");
    
    QTest::newRow("deg") << 15.0 << 180.0 << 90.0 << 118.0 << 10.0 << "degree";
    QTest::newRow("rad") << 1.0 << 2.2 << 1.8 << 1.4 << 61.0 << "radian";
}

void TestGeoDataLatLonAltBox::testConstruction() 
{
    
    GeoDataLatLonAltBox box1;
    GeoDataLatLonAltBox box2(box1);
    QCOMPARE(box1, box2);

    QFETCH(qreal, north);
    QFETCH(qreal, south);
    QFETCH(qreal, east);
    QFETCH(qreal, west);
    QFETCH(qreal, alt);
    QFETCH(QString, unit);

    if (unit == "degree") {
        GeoDataLatLonBox* box3 = new GeoDataLatLonBox(north, south, east, west, GeoDataCoordinates::Degree);
        GeoDataLatLonAltBox* box4 = new GeoDataLatLonAltBox(*box3);
        QCOMPARE(box3->west(), box4->west());
        QCOMPARE(box3->east(), box4->east());
        QCOMPARE(box3->north(), box4->north());
        QCOMPARE(box3->south(), box4->south());
        QCOMPARE(box3->rotation(), box4->rotation());

        GeoDataCoordinates* box5 = new GeoDataCoordinates(east, north, alt, GeoDataCoordinates::Degree);
        GeoDataLatLonAltBox* box6 = new GeoDataLatLonAltBox(*box5);
        QCOMPARE(box5->longitude(), box6->east());
        QCOMPARE(box5->longitude(), box6->west());
        QCOMPARE(box5->latitude(), box6->north());
        QCOMPARE(box5->latitude(), box6->south());
        QCOMPARE(box5->altitude(), box6->minAltitude());
        QCOMPARE(box5->altitude(), box6->maxAltitude());
    }

    if (unit == "radian") {
        GeoDataLatLonBox* box3 = new GeoDataLatLonBox(north, south, east, west, GeoDataCoordinates::Radian);
        GeoDataLatLonAltBox* box4 = new GeoDataLatLonAltBox(*box3);
        QCOMPARE(box3->west(), box4->west());
        QCOMPARE(box3->east(), box4->east());
        QCOMPARE(box3->north(), box4->north());
        QCOMPARE(box3->south(), box4->south());
        QCOMPARE(box3->rotation(), box4->rotation());

        GeoDataCoordinates* box5 = new GeoDataCoordinates(east, north, alt, GeoDataCoordinates::Radian);
        GeoDataLatLonAltBox* box6 = new GeoDataLatLonAltBox(*box5);
        QCOMPARE(box5->longitude(), box6->east());
        QCOMPARE(box5->longitude(), box6->west());
        QCOMPARE(box5->latitude(), box6->north());
        QCOMPARE(box5->latitude(), box6->south());
        QCOMPARE(box5->altitude(), box6->minAltitude());
        QCOMPARE(box5->altitude(), box6->maxAltitude());
    }
}

void TestGeoDataLatLonAltBox::testAltitude_data() 
{
    QTest::addColumn<qreal>("alt");

    QTest::newRow("Altitude 1") << 27.2;
    QTest::newRow("Altitude 2") << 0.22;
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
    QTest::addColumn<qreal>( "box1north" );
    QTest::addColumn<qreal>( "box1south" );
    QTest::addColumn<qreal>( "box1west" );
    QTest::addColumn<qreal>( "box1east" );
    QTest::addColumn<qreal>( "box1minAltitude" );
    QTest::addColumn<qreal>( "box1maxAltitude" );
    QTest::addColumn<qreal>( "box2north" );
    QTest::addColumn<qreal>( "box2south" );
    QTest::addColumn<qreal>( "box2west" );
    QTest::addColumn<qreal>( "box2east" );
    QTest::addColumn<qreal>( "box2minAltitude" );
    QTest::addColumn<qreal>( "box2maxAltitude" );
    QTest::addColumn<bool>( "intersects" );

    QTest::newRow( "same" ) << 56.0 << 40.0 << 0.0 << 11.0 << 10.0 << 12.0
                            << 56.0 << 40.0 << 0.0 << 11.0 << 10.0 << 12.0
                            << true;
    QTest::newRow( "dateLineFalse" ) << 30.0 << -30.0 << -170.0 << 170.0 << 0.0 << 0.0
                                     << 30.0 << -30.0 << 171.0 << -171.0 << 0.0 << 0.0
                                     << false;
    QTest::newRow( "dateLineTrue" ) << 20.0 << 0.0 << -171.0 << 171.0 << 0.0 << 0.0
                                    << 30.0 << -30.0 << 170.0 << -170.0 << 0.0 << 0.0
                                    << true;
}

void TestGeoDataLatLonAltBox::testIntersects()
{
    QFETCH( qreal, box1north );
    QFETCH( qreal, box1south );
    QFETCH( qreal, box1west );
    QFETCH( qreal, box1east );
    QFETCH( qreal, box1minAltitude );
    QFETCH( qreal, box1maxAltitude );
    QFETCH( qreal, box2north );
    QFETCH( qreal, box2south );
    QFETCH( qreal, box2west );
    QFETCH( qreal, box2east );
    QFETCH( qreal, box2minAltitude );
    QFETCH( qreal, box2maxAltitude );
    QFETCH( bool, intersects );

    GeoDataLatLonAltBox box1;
    GeoDataLatLonAltBox box2;
    box1.setNorth( box1north, GeoDataCoordinates::Degree );
    box1.setSouth( box1south, GeoDataCoordinates::Degree );
    box1.setWest( box1west, GeoDataCoordinates::Degree );
    box1.setEast( box1east, GeoDataCoordinates::Degree );
    box1.setMinAltitude( box1minAltitude );
    box1.setMaxAltitude( box1maxAltitude );
    box2.setNorth( box2north, GeoDataCoordinates::Degree );
    box2.setSouth( box2south, GeoDataCoordinates::Degree );
    box2.setWest( box2west, GeoDataCoordinates::Degree );
    box2.setEast( box2east, GeoDataCoordinates::Degree );
    box2.setMinAltitude( box2minAltitude );
    box2.setMaxAltitude( box2maxAltitude );
    QCOMPARE( box1.intersects( box2 ), intersects );
}

void TestGeoDataLatLonAltBox::testCenter_data()
{
    QTest::addColumn<qreal>( "boxnorth" );
    QTest::addColumn<qreal>( "boxsouth" );
    QTest::addColumn<qreal>( "boxwest" );
    QTest::addColumn<qreal>( "boxeast" );
    QTest::addColumn<qreal>( "centerlat" );
    QTest::addColumn<qreal>( "centerlon" );

    QTest::newRow( "N-E" ) << 60.0 << 40.0 << 10.0 << 30.0
                          << 50.0 << 20.0;

    QTest::newRow( "N-GW" ) << 60.0 << 40.0 << -30.0 << 10.0
                          << 50.0 << -10.0;

    QTest::newRow( "N-W" ) << 60.0 << 40.0 << -30.0 << -10.0
                          << 50.0 << -20.0;

    QTest::newRow( "NS-W" ) << 30.0 << -30.0 << -30.0 << -10.0
                           << 0.0  << -20.0;

    QTest::newRow( "N-IDL" ) << 30.0 << -30.0 << 170.0 << -150.0
                            << 0.0  << -170.0;
}

void TestGeoDataLatLonAltBox::testCenter()
{
    QFETCH( qreal, boxnorth );
    QFETCH( qreal, boxsouth );
    QFETCH( qreal, boxwest );
    QFETCH( qreal, boxeast );
    QFETCH( qreal, centerlat );
    QFETCH( qreal, centerlon );

    GeoDataLatLonAltBox box;
    box.setNorth( boxnorth, GeoDataCoordinates::Degree );
    box.setSouth( boxsouth, GeoDataCoordinates::Degree );
    box.setWest( boxwest, GeoDataCoordinates::Degree );
    box.setEast( boxeast, GeoDataCoordinates::Degree );

    GeoDataCoordinates center = box.center();
    QCOMPARE( center.latitude(GeoDataCoordinates::Degree), centerlat );
    QCOMPARE( center.longitude(GeoDataCoordinates::Degree), centerlon );
}

void TestGeoDataLatLonAltBox::testUnited_data()
{
    QTest::addColumn<qreal>( "box1north" );
    QTest::addColumn<qreal>( "box1south" );
    QTest::addColumn<qreal>( "box1west" );
    QTest::addColumn<qreal>( "box1east" );
    QTest::addColumn<qreal>( "box2north" );
    QTest::addColumn<qreal>( "box2south" );
    QTest::addColumn<qreal>( "box2west" );
    QTest::addColumn<qreal>( "box2east" );
    QTest::addColumn<qreal>( "box3north" );
    QTest::addColumn<qreal>( "box3south" );
    QTest::addColumn<qreal>( "box3west" );
    QTest::addColumn<qreal>( "box3east" );

    QTest::newRow( "same" ) << 56.0 << 40.0 << 0.0 << 11.0
                            << 56.0 << 40.0 << 0.0 << 11.0
                            << 56.0 << 40.0 << 0.0 << 11.0;

    // 2 boxes in West, result stays west
    QTest::newRow( "bigWest" ) << 30.0 << -30.0 << -30.0 << -10.0   // -20
                               << 30.0 << -30.0 << -170.0 << -150.0 // -160
                               << 30.0 << -30.0 << -170.0 << -10.0;

    // 2 boxes each side of greenwich, result crosses greenwich
    QTest::newRow( "aroundGreenwich" ) << 30.0 << -30.0 << -30.0 << -10.0 // -20
                                 << 30.0 << -30.0 << 10.0 << 30.0         // -160
                                 << 30.0 << -30.0 << -30.0 << 30.0;

    // 2 boxes crossing greenwich, result crosses greenwich
    QTest::newRow( "aroundGreenwich" ) << 30.0 << -30.0 << -30.0 << 10.0  // -20
                                 << 30.0 << -30.0 << -10.0 << 30.0        // 20
                                 << 30.0 << -30.0 << -30.0 << 30.0;

    // 2 boxes each side of IDL, result crosses IDL as smaller box
    QTest::newRow( "aroundIDL" ) << 30.0 << -30.0 << -170.0 << -150.0     // -160
                                 << 30.0 << -30.0 << 150.0 << 170.0       // 160
                                 << 30.0 << -30.0 << 150.0 << -150.0;

    // reciprocical, so independent of side
    QTest::newRow( "aroundIDL2" ) << 30.0 << -30.0 << 150.0 << 170.0     // 160
                                  << 30.0 << -30.0 << -170.0 << -150.0   // -160
                                  << 30.0 << -30.0 << 150.0 << -150.0;

    // 1 box crossing IDL, the 2 centers are close together, result crosses IDL
    QTest::newRow( "crossingIDLclose" ) << 30.0 << -30.0 << -150.0 << -130.0  // -140
                                        << 30.0 << -30.0 << 170.0 << -150.0   // -170
                                        << 30.0 << -30.0 << 170.0 << -130.0;

    // reciprocical
    QTest::newRow( "crossingIDLclose2" ) << 30.0 << -30.0 << 170.0 << -160.0   // -175
                                         << 30.0 << -30.0 << -150.0 << -140.0  // -145
                                         << 30.0 << -30.0 << 170.0 << -140.0;

    // 1 box crossing IDL, the 2 centers are across IDL, result crosses IDL
    QTest::newRow( "crossingIDLfar" ) << 30.0 << -30.0 << -170.0 << -150.0    // -160
                                      << 30.0 << -30.0 << 150.0 << -170.0          // 170
                                      << 30.0 << -30.0 << 150.0 << -150.0;

    // reciprocical
    QTest::newRow( "crossingIDLfar2" ) << 30.0 << -30.0 << 150.0 << -170.0          // 170
                                       << 30.0 << -30.0 << -170.0 << -150.0    // -160
                                       << 30.0 << -30.0 << 150.0 << -150.0;

    // 2 box crossing IDL, the 2 centers are close together, result crosses IDL
    QTest::newRow( "crossingsIDLclose" ) << 30.0 << -30.0 << 160.0 << -140.0   // -170
                                        << 30.0 << -30.0 << 170.0 << -160.0   // -175
                                        << 30.0 << -30.0 << 160.0 << -140.0;

    // 2 box crossing IDL, the 2 centers are across IDL, result crosses IDL
    QTest::newRow( "crossingsIDLfar" ) << 30.0 << -30.0 << -170.0 << -150.0    // -160
                                      << 30.0 << -30.0 << 150.0 << -170.0     // 170
                                      << 30.0 << -30.0 << 150.0 << -150.0;

    QTest::newRow( "bug299959" ) << 90.0 << -90.0 << -180.0 << 180.0
                                 << 18.0 << -18.0 <<   20.0 <<  30.0
                                 << 90.0 << -90.0 << -180.0 << 180.0;
}

void TestGeoDataLatLonAltBox::testUnited()
{
    QFETCH( qreal, box1north );
    QFETCH( qreal, box1south );
    QFETCH( qreal, box1west );
    QFETCH( qreal, box1east );
    QFETCH( qreal, box2north );
    QFETCH( qreal, box2south );
    QFETCH( qreal, box2west );
    QFETCH( qreal, box2east );
    QFETCH( qreal, box3north );
    QFETCH( qreal, box3south );
    QFETCH( qreal, box3west );
    QFETCH( qreal, box3east );

    GeoDataLatLonAltBox box1;
    GeoDataLatLonAltBox box2;
    GeoDataLatLonAltBox box3;
    box1.setNorth( box1north, GeoDataCoordinates::Degree );
    box1.setSouth( box1south, GeoDataCoordinates::Degree );
    box1.setWest( box1west, GeoDataCoordinates::Degree );
    box1.setEast( box1east, GeoDataCoordinates::Degree );
    box2.setNorth( box2north, GeoDataCoordinates::Degree );
    box2.setSouth( box2south, GeoDataCoordinates::Degree );
    box2.setWest( box2west, GeoDataCoordinates::Degree );
    box2.setEast( box2east, GeoDataCoordinates::Degree );
    box3 = box1 | box2;
    QCOMPARE( box3.north( GeoDataCoordinates::Degree ), box3north );
    QCOMPARE( box3.south( GeoDataCoordinates::Degree ), box3south );
    QCOMPARE( box3.west( GeoDataCoordinates::Degree ), box3west );
    QCOMPARE( box3.east( GeoDataCoordinates::Degree ), box3east );
}

void TestGeoDataLatLonAltBox::testIdlCrossing()
{
    // Test case for bug 299527
    GeoDataLineString string;
    string << GeoDataCoordinates(    0, 0, 200, GeoDataCoordinates::Degree );
    string << GeoDataCoordinates(  180, 0, 200, GeoDataCoordinates::Degree );
    string << GeoDataCoordinates( -180, 0, 200, GeoDataCoordinates::Degree );
    string << GeoDataCoordinates(    0, 0, 200, GeoDataCoordinates::Degree );

    GeoDataLatLonAltBox box = string.latLonAltBox();
    QCOMPARE( box.west(), -M_PI );
    QCOMPARE( box.east(), M_PI );
    QCOMPARE( box.north(), 0.0 );
    QCOMPARE( box.south(), 0.0 );
}

void TestGeoDataLatLonAltBox::testDefaultConstruction()
{
    GeoDataLatLonBox const null;
    QVERIFY( null.isNull() );
    QVERIFY( null.isEmpty() );
    QVERIFY( (null|null).isNull() );
    QVERIFY( (null|null).isEmpty() );

    GeoDataLatLonBox const box( 0.5, 0.4, 0.3, 0.2 );

    GeoDataLatLonBox const leftUnited = null | box;
    QCOMPARE( leftUnited.west(), 0.2 );
    QCOMPARE( leftUnited.east(), 0.3 );
    QCOMPARE( leftUnited.north(), 0.5 );
    QCOMPARE( leftUnited.south(), 0.4 );

    GeoDataLatLonBox const rightUnited = box | null;
    QCOMPARE( rightUnited.west(), 0.2 );
    QCOMPARE( rightUnited.east(), 0.3 );
    QCOMPARE( rightUnited.north(), 0.5 );
    QCOMPARE( rightUnited.south(), 0.4 );

    QVERIFY( !null.intersects( null ) );
    QVERIFY( !null.intersects( box ) );
    QVERIFY( !box.intersects( null ) );
}

qreal TestGeoDataLatLonAltBox::randomLon() 
{
    qreal lon = qrand();

    while ( lon < -180 ) 
        lon += 360;    
    while ( lon > 180 ) 
        lon -= 360;

    return lon;
}

qreal TestGeoDataLatLonAltBox::randomLat()
{
    qreal lat = qrand();

    while ( lat < -90 ) 
        lat += 180;
    while ( lat > 90 ) 
        lat -= 180;
                                
    return lat;
}


void TestGeoDataLatLonAltBox::testFromLineString_data() 
{
    QTest::addColumn<qreal>("north");
    QTest::addColumn<qreal>("south");
    QTest::addColumn<qreal>("east");
    QTest::addColumn<qreal>("west");    

    QTest::newRow("Test 1") << randomLat() << randomLat() << randomLon() << randomLon();
    QTest::newRow("Test 2") << randomLat() << randomLat() << randomLon() << randomLon();
    QTest::newRow("Test 3") << randomLat() << randomLat() << randomLon() << randomLon();
}

void TestGeoDataLatLonAltBox::testFromLineString() {
    QFETCH(qreal, north);
    QFETCH(qreal, south);
    QFETCH(qreal, east);
    QFETCH(qreal, west);

    GeoDataLatLonAltBox box;
    if (north >= south) 
        box = GeoDataLatLonBox(north, south, east, west, GeoDataCoordinates::Degree);
    else
        box = GeoDataLatLonBox(south, north, east, west, GeoDataCoordinates::Degree);

    GeoDataLinearRing ring;

    // SouthWest
    ring << GeoDataCoordinates(box.west(), box.south());
    // SouthEast
    ring << GeoDataCoordinates(box.east(), box.south());
    // NorthEast
    ring << GeoDataCoordinates(box.east(), box.north());
    // NorthWest
    ring << GeoDataCoordinates(box.west(), box.north());

//    QCOMPARE(GeoDataLatLonAltBox::fromLineString(ring).toString(), GeoDataLatLonBox::fromLineString(ring).toString());
    QCOMPARE(GeoDataLatLonAltBox::fromLineString(ring).north(), GeoDataLatLonBox::fromLineString(ring).north());
    QCOMPARE(GeoDataLatLonAltBox::fromLineString(ring).south(), GeoDataLatLonBox::fromLineString(ring).south());
    QCOMPARE(GeoDataLatLonAltBox::fromLineString(ring).east(), GeoDataLatLonBox::fromLineString(ring).east());
    QCOMPARE(GeoDataLatLonAltBox::fromLineString(ring).west(), GeoDataLatLonBox::fromLineString(ring).west());
}

void TestGeoDataLatLonAltBox::testToString_data() {
    
}

void TestGeoDataLatLonAltBox::testToString() {
    
}

void TestGeoDataLatLonAltBox::testPack_data() {
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<qreal>("lat");
    QTest::addColumn<qreal>("alt");
    
    QTest::newRow("rad1") << 1.0 << 2.2 << 1.8;
    QTest::newRow("rad2") << 0.2 << 3.1 << 2.9;
}

void TestGeoDataLatLonAltBox::testPack() {
    QFETCH(qreal, lon);
    QFETCH(qreal, lat);
    QFETCH(qreal, alt);

    GeoDataCoordinates box1 = GeoDataCoordinates(lon, lat, alt, GeoDataCoordinates::Radian);
    GeoDataLatLonAltBox box2 = GeoDataLatLonAltBox(box1);
    GeoDataLatLonAltBox box3;

    QTemporaryFile file;
    if(file.open()) {
        QDataStream out(&file);
        box2.pack(out);
    }
    file.close();

    if(file.open()) {
        QDataStream in(&file);
        box3.unpack(in);
    }
    file.close();

/*    QCOMPARE(box2.north(), box3.north());
    QCOMPARE(box2.south(), box3.south());
    QCOMPARE(box2.east(), box3.east());
    QCOMPARE(box2.west(), box3.west());*/ 

    QCOMPARE(box2.maxAltitude(), box3.maxAltitude());
    QCOMPARE(box2.minAltitude(), box3.minAltitude());
    QCOMPARE(box2.altitudeMode(), box3.altitudeMode());
}

void TestGeoDataLatLonAltBox::testContainerBox_data() {
    QTest::addColumn<qreal>("lon1");
    QTest::addColumn<qreal>("lat1");
    QTest::addColumn<qreal>("lon2");
    QTest::addColumn<qreal>("lat2");
    QTest::addColumn<qreal>("lon3");
    QTest::addColumn<qreal>("lat3");

    QTest::newRow("rad1") << 2.4 << 0.1 << 1.0 << 1.2 << -1.8 << -0.7 ;
    QTest::newRow("rad2") << -1.3 << -0.1 << 0.2 << 1.1 << 2.9 << 0.9 ;
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

QTEST_MAIN(TestGeoDataLatLonAltBox)
#include "TestGeoDataLatLonAltBox.moc"

