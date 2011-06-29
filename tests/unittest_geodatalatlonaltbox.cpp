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
#include "global.h"
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
    void testConstruction_data(); //i'll test the 3 types of constructor: from GeoDataLatLonAltBox, from GeoDataLatLonBox, from GeoDataCoordinates
    void testConstruction();
    void testAltitude_data();
    void testAltitude();
    void testContains_data(); //again 3 types of constructor: GeoDataLatLonAltBox, GeoDataPoint, GeoDataCoordinates
    void testContains();  
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


void TestGeoDataLatLonAltBox::testContains_data() {
    
}

void TestGeoDataLatLonAltBox::testContains() {
    
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
#include "unittest_geodatalatlonaltbox.moc"

