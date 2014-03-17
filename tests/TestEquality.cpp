//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Cruceru Calin-Cristian <crucerucalincristian@gmail.com>
// Copyright 2014      Sanjiban Bairagya      <sanjiban22393@gmail.com>
//

#include <QObject>

#include <MarbleDebug.h>
#include <GeoDataAlias.h>
#include <GeoDataAccuracy.h>
#include <GeoDataSimpleArrayData.h>
#include <GeoDataData.h>
#include <GeoDataExtendedData.h>
#include <GeoDataBalloonStyle.h>
#include <GeoDataCamera.h>
#include <GeoDataColorStyle.h>
#include <GeoDataFlyTo.h>
#include <GeoDataHotSpot.h>
#include <GeoDataImagePyramid.h>
#include <GeoDataItemIcon.h>
#include <GeoDataLink.h>
#include <GeoDataAnimatedUpdate.h>
#include <GeoDataSoundCue.h>
#include <GeoDataTourControl.h>
#include <GeoDataWait.h>
#include <GeoDataTour.h>
#include <GeoDataPoint.h>
#include <GeoDataLinearRing.h>
#include <GeoDataLineString.h>
#include <GeoDataPolygon.h>
#include <GeoDataLatLonQuad.h>
#include <GeoDataLatLonAltBox.h>
#include <GeoDataLatLonBox.h>
#include <GeoDataLod.h>
#include <GeoDataRegion.h>
#include "TestUtils.h"

using namespace Marble;

class TestEquality : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void aliasTest();
    void accuracyTest();
    void simpleArrayDataTest();
    void dataTest();
    void extendedDataTest();
    void balloonTest();
    void colorTest();
    void cameraTest();
    void flyToTest();
    void hotSpotTest();
    void imagePyramidTest();
    void itemIconTest();
    void linkTest();
    void tourTest();
    void pointTest();
    void linearRingTest();
    void lineStringTest();
    void polygonTest();
    void latLonQuadTest();
    void latLonBoxTest();
    void latLonAltBoxTest();
    void lodTest();
    void regionTest();
};


void TestEquality::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestEquality::aliasTest()
{
    GeoDataAlias alias;
    alias.setSourceHref("test1");
    alias.setTargetHref("test1");

    GeoDataAlias alias2;
    alias2.setSourceHref("test1");
    alias2.setTargetHref("test1");

    QCOMPARE( alias, alias );
    QCOMPARE( alias2, alias2 );
    QCOMPARE( alias, alias2 );

    alias.setSourceHref("test2");
    alias2.setSourceHref("Test2");
    QCOMPARE( alias == alias2, false );
    QVERIFY( alias != alias2 );

    alias.setSourceHref("test3");
    alias.setTargetHref("test3");
    alias2.setSourceHref("test3");
    alias2.setTargetHref("test2");
    QCOMPARE( alias == alias2, false );

    alias2.setSourceHref("Test3");
    QVERIFY( alias != alias2 );

    alias.setSourceHref("Test3");
    alias.setTargetHref("test2");
    QCOMPARE( alias, alias2 );
}

void TestEquality::accuracyTest()
{
    GeoDataAccuracy accuracy1, accuracy2;

    accuracy1.level = GeoDataAccuracy::Country;
    accuracy1.vertical = 2;
    accuracy1.horizontal = 3;
    accuracy2.level = GeoDataAccuracy::Country;
    accuracy2.vertical = 2;
    accuracy2.horizontal = 3;
    QCOMPARE( accuracy1, accuracy1 );
    QCOMPARE( accuracy2, accuracy2 );
    QCOMPARE( accuracy1, accuracy2 );

    accuracy1.vertical = 1;
    QVERIFY( accuracy1 != accuracy2 );

    accuracy1.horizontal = 1;
    accuracy2.horizontal = 1;

    QCOMPARE( accuracy1 == accuracy2, false );
}

void TestEquality::simpleArrayDataTest()
{
    GeoDataSimpleArrayData simpleArray1, simpleArray2;

    simpleArray1.append("Marble");
    simpleArray1.append(2014);
    simpleArray1.append("Globe");

    simpleArray2.append("Globe");
    simpleArray2.append(2014);
    simpleArray2.append("Marble");

    QCOMPARE( simpleArray1, simpleArray1 );
    QCOMPARE( simpleArray2, simpleArray2 );
    QCOMPARE( simpleArray1 == simpleArray2, false );
    QVERIFY( simpleArray1 != simpleArray2 );
}

void TestEquality::dataTest()
{
    GeoDataData data1, data2;

    data1.setName("Something");
    data1.setValue(QVariant(23.56));
    data1.setDisplayName("Marble");

    data2.setName("Something");
    data2.setValue(QVariant(23.56));
    data2.setDisplayName("Marble");

    QCOMPARE( data1, data1 );
    QCOMPARE( data2, data2 );
    QCOMPARE( data1, data2 );
    QCOMPARE( data1 != data2, false );

    data1.setName("Marble");
    data1.setDisplayName("Something");

    QCOMPARE( data1 == data2, false );
    QVERIFY( data1 != data2 );
}

void TestEquality::extendedDataTest()
{
    GeoDataExtendedData extendedData1, extendedData2;
    GeoDataData data1, data2;

    data1.setName("Something");
    data1.setValue(QVariant(23.56));
    data1.setDisplayName("Marble");

    data2.setName("Marble");
    data2.setValue(QVariant(23.56));
    data2.setDisplayName("Globe");

    extendedData1.addValue(data1);
    extendedData2.addValue(data2);

    QCOMPARE( extendedData1, extendedData1 );
    QCOMPARE( extendedData2, extendedData2 );
    QCOMPARE( extendedData1 == extendedData2, false );
    QVERIFY( extendedData1 != extendedData2 );
}

void TestEquality::balloonTest()
{
    GeoDataBalloonStyle balloon1, balloon2;

    balloon1.setBackgroundColor(Qt::white);
    balloon1.setTextColor(Qt::black);
    balloon1.setText("Text1");
    balloon1.setDisplayMode(GeoDataBalloonStyle::Hide);

    balloon2.setBackgroundColor(Qt::white);
    balloon2.setTextColor(Qt::black);
    balloon2.setText("Text1");
    balloon2.setDisplayMode(GeoDataBalloonStyle::Hide);

    QCOMPARE( balloon1, balloon1 );
    QCOMPARE( balloon2, balloon2 );
    QCOMPARE( balloon1, balloon2 );

    balloon2.setDisplayMode(GeoDataBalloonStyle::Default);
    QCOMPARE( balloon1 != balloon2, true );

    balloon1.setDisplayMode(GeoDataBalloonStyle::Default);
    balloon1.setText("Text2");
    QCOMPARE( balloon1 == balloon2, false);

    QVERIFY( balloon1 != balloon2 );
}

void TestEquality::colorTest()
{
    GeoDataColorStyle color1, color2;

    color1.setColor(Qt::white);
    color2.setColor(Qt::white);

    QCOMPARE( color1, color1 );
    QCOMPARE( color2, color2 );
    QCOMPARE( color1, color2 );

    color1.setId("foo");
    QVERIFY( color1 != color2 );
    color2.setId("foo");
    QCOMPARE( color1, color2 );

    color1.setColorMode(GeoDataColorStyle::Random);
    QCOMPARE( color1, color1 );
    QCOMPARE( color2, color2 );
    // Different color mode, so not equal
    QVERIFY( color1 != color2 );

    color2.setColorMode(GeoDataColorStyle::Random);
    QCOMPARE( color1, color1 );
    QCOMPARE( color2, color2 );
    // Paint colors are now likely different, but we still consider them equal
    QCOMPARE( color1, color2 );

    color2.setColor( Qt::yellow );
    QCOMPARE( color1, color1 );
    QCOMPARE( color2, color2 );
    QVERIFY( color1 != color2 );
}

void TestEquality::cameraTest()
{
    GeoDataCamera camera1, camera2;

    camera1.setAltitudeMode(Marble::Absolute);
    camera1.setAltitude(100);
    camera1.setLatitude(100);
    camera1.setLongitude(100);
    camera1.setHeading(200);
    camera1.setRoll(300);
    camera1.setTilt(400);

    camera2.setAltitudeMode(Marble::ClampToGround);
    camera2.setAltitude(100);
    camera2.setLatitude(100);
    camera2.setLongitude(100);
    camera2.setHeading(200);
    camera2.setRoll(300);
    camera2.setTilt(400);

    QCOMPARE( camera1, camera1 );
    QCOMPARE( camera2, camera2 );
    QCOMPARE( camera1 == camera2, false );
    QVERIFY( camera1 != camera2 );
}

void TestEquality::flyToTest()
{
    GeoDataFlyTo flyTo1, flyTo2;

    flyTo1.setDuration(7.6);
    flyTo1.setFlyToMode(GeoDataFlyTo::Bounce);

    flyTo2.setDuration( 5.8 );
    flyTo1.setFlyToMode( GeoDataFlyTo::Smooth );

    QCOMPARE( flyTo1, flyTo1 );
    QCOMPARE( flyTo2, flyTo2 );
    QCOMPARE( flyTo1 == flyTo2, false );
    QVERIFY( flyTo1 != flyTo2 );
}

void TestEquality::hotSpotTest()
{
    QPointF point1, point2;
    point1.setX( 5 );
    point1.setY( 4 );
    point2.setX( 6 );
    point2.setY( 2 );

    GeoDataHotSpot hotSpot1, hotSpot2;
    hotSpot1.setHotSpot( point1, GeoDataHotSpot::Fraction, GeoDataHotSpot::Fraction );
    hotSpot2.setHotSpot( point2, GeoDataHotSpot::Pixels, GeoDataHotSpot::Pixels );

    QCOMPARE( &hotSpot1, &hotSpot1 );
    QCOMPARE( &hotSpot2, &hotSpot2 );
    QCOMPARE( hotSpot1 == hotSpot2, false );
    QVERIFY( hotSpot1 != hotSpot2 );
}

void TestEquality::imagePyramidTest()
{
    GeoDataImagePyramid pyramid1, pyramid2;
    pyramid1.setGridOrigin( GeoDataImagePyramid::LowerLeft );
    pyramid1.setMaxHeight( 40 );
    pyramid1.setMaxWidth( 30 );
    pyramid1.setTileSize( 20 );

    pyramid2.setGridOrigin( GeoDataImagePyramid::UpperLeft );
    pyramid2.setMaxHeight( 50 );
    pyramid2.setMaxWidth( 80 );
    pyramid2.setTileSize( 30 );

    QCOMPARE( &pyramid1, &pyramid1 );
    QCOMPARE( &pyramid2, &pyramid2 );
    QCOMPARE( pyramid1 == pyramid2, false );
    QVERIFY( pyramid1 != pyramid2 );
}

void TestEquality::itemIconTest()
{
    GeoDataItemIcon icon1, icon2;
    QImage img1( 10, 20, QImage::Format_Mono );
    QImage img2( 30, 50, QImage::Format_Mono );

    icon1.setIcon( img1 );
    icon1.setIconPath( "some/path" );
    icon1.setState( GeoDataItemIcon::Open );

    icon2.setIcon( img2 );
    icon2.setIconPath( "some/other/path" );
    icon2.setState( GeoDataItemIcon::Closed );

    QCOMPARE( &icon1, &icon1 );
    QCOMPARE( &icon2, &icon2 );
    QCOMPARE( &icon1 == &icon2, false );
    QVERIFY( &icon1 != &icon2 );

}

void TestEquality::linkTest()
{
    GeoDataLink link1, link2;

    link1.setHref("some/example/href.something");
    link1.setRefreshInterval( 23 );
    link1.setRefreshMode( GeoDataLink::OnChange );
    link1.setViewBoundScale( 50 );
    link1.setViewRefreshTime( 30 );

    link2.setHref("some/other/example/href.something");
    link1.setRefreshInterval( 32 );
    link1.setRefreshMode(GeoDataLink::OnChange);
    link1.setViewBoundScale( 60 );
    link1.setViewRefreshTime( 40 );

    QCOMPARE( &link1, &link1 );
    QCOMPARE( &link2, &link2 );
    QCOMPARE( &link1 == &link2, false );
    QVERIFY( &link1 != &link2 );
}

void TestEquality::tourTest()
{
    GeoDataTour tour1;
    GeoDataTour tour2;
    GeoDataPlaylist* playlist1 = new GeoDataPlaylist;
    GeoDataPlaylist* playlist2 = new GeoDataPlaylist;

    GeoDataAnimatedUpdate* animatedUpdate1 = new GeoDataAnimatedUpdate;
    GeoDataAnimatedUpdate* animatedUpdate2 = new GeoDataAnimatedUpdate;
    GeoDataSoundCue* cue1 = new GeoDataSoundCue;
    GeoDataSoundCue* cue2 = new GeoDataSoundCue;
    GeoDataTourControl* control1 = new GeoDataTourControl;
    GeoDataTourControl* control2 = new GeoDataTourControl;
    GeoDataWait* wait1 = new GeoDataWait;
    GeoDataWait* wait2 = new GeoDataWait;
    GeoDataFlyTo* flyTo1 = new GeoDataFlyTo;
    GeoDataFlyTo* flyTo2 = new GeoDataFlyTo;

    playlist1->addPrimitive( animatedUpdate1 );
    playlist1->addPrimitive( cue1 );
    playlist1->addPrimitive( control1 );
    playlist1->addPrimitive( wait1 );
    playlist1->addPrimitive( flyTo1 );

    playlist2->addPrimitive( animatedUpdate2 );
    playlist2->addPrimitive( cue2 );
    playlist2->addPrimitive( control2 );
    playlist2->addPrimitive( wait2 );
    playlist2->addPrimitive( flyTo2 );

    tour1.setPlaylist( playlist1 );
    tour2.setPlaylist( playlist2 );

    QCOMPARE( tour1, tour1 );
    QCOMPARE( tour2, tour2 );
    QCOMPARE( tour1 != tour2, false );
    QVERIFY( tour1 == tour2 );

    tour1.playlist()->swapPrimitives( 1, 3 );

    QCOMPARE( tour1, tour1 );
    QCOMPARE( tour2, tour2 );
    QCOMPARE( tour1 == tour2, false );
    QVERIFY( tour1 != tour2 );

    tour1.playlist()->swapPrimitives( 1, 3 );
    QVERIFY( tour1 == tour2 );

    GeoDataSoundCue* cue3 = new GeoDataSoundCue;
    tour1.playlist()->addPrimitive( cue3 );

    QCOMPARE( tour1, tour1 );
    QCOMPARE( tour2, tour2 );
    QCOMPARE( tour1 == tour2, false );
    QVERIFY( tour1 != tour2 );
}

void TestEquality::pointTest()
{
    GeoDataPoint point1, point2;
    GeoDataCoordinates coord1, coord2;

    coord1.set(100,100,100);
    coord2.set(200,200,200);
    point1.setCoordinates(coord1);
    point2.setCoordinates(coord2);

    QVERIFY( point1 != point2 );
    QCOMPARE( point1 != point2, true );

    coord1.set(100,150);
    coord2.set(100,150);
    point1.setCoordinates(coord1);
    point2.setCoordinates(coord2);

    QVERIFY( point1 == point2 );
    QCOMPARE( point1, point2 );
    QCOMPARE( point1 == point2, true );

    QVERIFY( point1 == point1 );
    QCOMPARE( point1, point2 );
}

void TestEquality::linearRingTest()
{
    GeoDataLinearRing linearRing1, linearRing2;
    GeoDataCoordinates coord1, coord2, coord3, coord4, coord5, coord6;

    coord1.set(123,100);
    coord2.set(124,101);
    coord3.set(123,100);
    coord4.set(123,100);
    coord5.set(124,101);
    coord6.set(123,100);

    linearRing1.append(coord1);
    linearRing1.append(coord2);
    linearRing1.append(coord3);
    linearRing2.append(coord4);
    linearRing2.append(coord5);
    linearRing2.append(coord6);
    linearRing1.setTessellate(false);
    linearRing2.setTessellate(false);

    QVERIFY( linearRing1 == linearRing2 );
    QCOMPARE( linearRing1 != linearRing2, false );

    linearRing2.clear();
    linearRing2.append(coord1);
    linearRing2.append(coord3);
    linearRing2.append(coord4);

    QVERIFY( linearRing1 != linearRing2 );
    QCOMPARE( linearRing1 == linearRing2, false );
}

void TestEquality::lineStringTest()
{
    GeoDataLineString lineString1, lineString2;
    GeoDataCoordinates coord1, coord2, coord3, coord4, coord5, coord6;

    coord1.set(100,100,100);
    coord2.set(-100,-100,-100);
    coord3.set(50,50,50);
    coord4.set(150,150,150);
    coord5.set(-150,-150,-150);
    coord6.set(-50,-50,-50);

    lineString1.append(coord1);
    lineString1.append(coord3);
    lineString1.append(coord4);
    lineString2.append(coord2);
    lineString2.append(coord5);
    lineString2.append(coord6);
    lineString1.setTessellate(false);
    lineString2.setTessellate(false);

    QVERIFY( lineString1 != lineString2 );
    QCOMPARE( lineString1 == lineString2, false );

    lineString2.clear();
    lineString2.append(coord1);
    lineString2.append(coord3);
    lineString2.append(coord4);

    QVERIFY( lineString1 == lineString2 );
    QCOMPARE( lineString1 != lineString2, false );
}

void TestEquality::polygonTest()
{
    GeoDataPolygon polygon1, polygon2;

    GeoDataLinearRing outerBoundary1, outerBoundary2;
    GeoDataCoordinates coord1, coord2, coord3, coord4, coord5, coord6;

    coord1.set(623,600);
    coord2.set(624,601);
    coord3.set(623,600);
    coord4.set(623,600);
    coord5.set(624,601);
    coord6.set(623,600);

    outerBoundary1.append(coord1);
    outerBoundary1.append(coord2);
    outerBoundary1.append(coord3);
    outerBoundary2.append(coord4);
    outerBoundary2.append(coord5);
    outerBoundary2.append(coord6);
    outerBoundary1.setTessellate(true);
    outerBoundary2.setTessellate(true);
    polygon1.setOuterBoundary(outerBoundary1);
    polygon2.setOuterBoundary(outerBoundary2);


    GeoDataLinearRing innerBoundary11, innerBoundary12, innerBoundary21, innerBoundary22;
    GeoDataCoordinates coord111, coord112, coord113, coord121, coord122, coord123;
    GeoDataCoordinates coord211, coord212, coord213, coord221, coord222, coord223;

    coord111.set(100,100);
    coord112.set(200,200);
    coord113.set(100,100);
    coord121.set(400,400);
    coord122.set(500,500);
    coord123.set(400,400);
    coord211.set(100,100);
    coord212.set(200,200);
    coord213.set(100,100);
    coord221.set(400,400);
    coord222.set(500,500);
    coord223.set(400,400);

    innerBoundary11.append(coord111);
    innerBoundary11.append(coord112);
    innerBoundary11.append(coord113);
    innerBoundary12.append(coord121);
    innerBoundary12.append(coord122);
    innerBoundary12.append(coord123);
    innerBoundary21.append(coord211);
    innerBoundary21.append(coord212);
    innerBoundary21.append(coord213);
    innerBoundary22.append(coord221);
    innerBoundary22.append(coord222);
    innerBoundary22.append(coord223);
    innerBoundary11.setTessellate(true);
    innerBoundary12.setTessellate(true);
    innerBoundary21.setTessellate(true);
    innerBoundary22.setTessellate(true);

    polygon1.appendInnerBoundary(innerBoundary11);
    polygon1.appendInnerBoundary(innerBoundary12);
    polygon2.appendInnerBoundary(innerBoundary21);
    polygon2.appendInnerBoundary(innerBoundary22);
    polygon1.setTessellate(true);
    polygon2.setTessellate(true);

    QVERIFY( polygon1 == polygon2 );
    QCOMPARE( polygon1 != polygon2, false );

    polygon1.setTessellate(false);
    QVERIFY( polygon1 != polygon2 );

    polygon1.setTessellate(true);
    polygon1.appendInnerBoundary(innerBoundary11);
    QVERIFY( polygon1 != polygon2 );

    QCOMPARE( polygon1 == polygon2, false );
}

void TestEquality::latLonQuadTest()
{
    GeoDataLatLonQuad quad1, quad2;
    quad1.setBottomLeftLatitude( 55, GeoDataCoordinates::Degree );
    quad1.setBottomLeftLongitude( 60, GeoDataCoordinates::Degree );
    quad1.setBottomRightLatitude( 45, GeoDataCoordinates::Degree );
    quad1.setBottomRightLongitude( 50, GeoDataCoordinates::Degree );

    quad1.setTopLeftLatitude( 55, GeoDataCoordinates::Degree );
    quad1.setTopLeftLongitude( 60, GeoDataCoordinates::Degree );
    quad1.setTopRightLatitude( 45, GeoDataCoordinates::Degree );
    quad1.setTopRightLongitude( 50, GeoDataCoordinates::Degree );

    quad2.setBottomLeftLatitude( 55, GeoDataCoordinates::Degree );
    quad2.setBottomLeftLongitude( 60, GeoDataCoordinates::Degree );
    quad2.setBottomRightLatitude( 45, GeoDataCoordinates::Degree );
    quad2.setBottomRightLongitude( 50, GeoDataCoordinates::Degree );

    quad2.setTopLeftLatitude( 55, GeoDataCoordinates::Degree );
    quad2.setTopLeftLongitude( 60, GeoDataCoordinates::Degree );
    quad2.setTopRightLatitude( 45, GeoDataCoordinates::Degree );
    quad2.setTopRightLongitude( 50, GeoDataCoordinates::Degree );

    QCOMPARE( quad1, quad1 );
    QCOMPARE( quad2, quad2 );
    QCOMPARE( quad1 != quad2, false );
    QVERIFY( quad1 == quad2 );

    quad1.setTopLeftLatitude( 65, GeoDataCoordinates::Degree );

    QCOMPARE( quad1, quad1 );
    QCOMPARE( quad2, quad2 );
    QCOMPARE( quad1 == quad2, false );
    QVERIFY( quad1 != quad2 );
}

void TestEquality::latLonBoxTest()
{
    GeoDataLatLonBox latLonBox1, latLonBox2;

    latLonBox1.setEast( 40 );
    latLonBox1.setWest( 50 );
    latLonBox1.setNorth( 60 );
    latLonBox1.setSouth( 70 );
    latLonBox1.setRotation( 20 );
    latLonBox1.setBoundaries( 70, 80, 50, 60 );

    latLonBox2.setEast( 40 );
    latLonBox2.setWest( 50 );
    latLonBox2.setNorth( 60 );
    latLonBox2.setSouth( 70 );
    latLonBox2.setRotation( 20 );
    latLonBox2.setBoundaries( 70, 80, 50, 60 );

    QCOMPARE( latLonBox1, latLonBox1 );
    QCOMPARE( latLonBox2, latLonBox2 );
    QCOMPARE( latLonBox1 != latLonBox2, false );
    QVERIFY( latLonBox1 == latLonBox2 );

    latLonBox2.setWest( 55 );

    QCOMPARE( latLonBox1, latLonBox1 );
    QCOMPARE( latLonBox2, latLonBox2 );
    QCOMPARE( latLonBox1 == latLonBox2, false );
    QVERIFY( latLonBox1 != latLonBox2 );
}

void TestEquality::latLonAltBoxTest()
{
    GeoDataLatLonAltBox latLonAltBox1, latLonAltBox2;

    latLonAltBox1.setEast( 40 );
    latLonAltBox1.setWest( 50 );
    latLonAltBox1.setNorth( 60 );
    latLonAltBox1.setSouth( 70 );
    latLonAltBox1.setRotation( 20 );
    latLonAltBox1.setBoundaries( 70, 80, 50, 60 );
    latLonAltBox1.setMaxAltitude( 100 );
    latLonAltBox1.setMinAltitude( 20 );
    latLonAltBox1.setAltitudeMode( Marble::Absolute );

    latLonAltBox2.setEast( 40 );
    latLonAltBox2.setWest( 50 );
    latLonAltBox2.setNorth( 60 );
    latLonAltBox2.setSouth( 70 );
    latLonAltBox2.setRotation( 20 );
    latLonAltBox2.setBoundaries( 70, 80, 50, 60 );
    latLonAltBox2.setMaxAltitude( 100 );
    latLonAltBox2.setMinAltitude( 20 );
    latLonAltBox2.setAltitudeMode( Marble::Absolute );

    QCOMPARE( latLonAltBox1, latLonAltBox1 );
    QCOMPARE( latLonAltBox2, latLonAltBox2 );
    QCOMPARE( latLonAltBox1 != latLonAltBox2, false );
    QVERIFY( latLonAltBox1 == latLonAltBox2 );

    latLonAltBox2.setEast( 30 );

    QCOMPARE( latLonAltBox1, latLonAltBox1 );
    QCOMPARE( latLonAltBox2, latLonAltBox2 );
    QCOMPARE( latLonAltBox1 == latLonAltBox2, false );
    QVERIFY( latLonAltBox1 != latLonAltBox2 );
}

void TestEquality::lodTest()
{
    GeoDataLod lod1, lod2;
    lod1.setMaxFadeExtent( 20 );
    lod1.setMinFadeExtent( 10 );
    lod1.setMaxLodPixels( 30 );
    lod1.setMinLodPixels( 5 );

    lod2.setMaxFadeExtent( 20 );
    lod2.setMinFadeExtent( 10 );
    lod2.setMaxLodPixels( 30 );
    lod2.setMinLodPixels( 5 );

    QCOMPARE( lod1, lod1 );
    QCOMPARE( lod2, lod2 );
    QCOMPARE( lod1 != lod2, false );
    QVERIFY( lod1 == lod2 );

    lod2.setMaxFadeExtent( 30 );

    QCOMPARE( lod1, lod1 );
    QCOMPARE( lod2, lod2 );
    QCOMPARE( lod1 == lod2, false );
    QVERIFY( lod1 != lod2 );
}

void TestEquality::regionTest()
{
    GeoDataRegion region1;
    GeoDataRegion region2;

    GeoDataLatLonAltBox latLonAltBox1;
    GeoDataLatLonAltBox latLonAltBox2;

    latLonAltBox1.setEast( 40 );
    latLonAltBox1.setWest( 50 );
    latLonAltBox1.setNorth( 60 );
    latLonAltBox1.setSouth( 70 );
    latLonAltBox1.setRotation( 20 );
    latLonAltBox1.setBoundaries( 70, 80, 50, 60 );
    latLonAltBox1.setMaxAltitude( 100 );
    latLonAltBox1.setMinAltitude( 20 );
    latLonAltBox1.setAltitudeMode( Marble::Absolute );

    latLonAltBox2.setEast( 40 );
    latLonAltBox2.setWest( 50 );
    latLonAltBox2.setNorth( 60 );
    latLonAltBox2.setSouth( 70 );
    latLonAltBox2.setRotation( 20 );
    latLonAltBox2.setBoundaries( 70, 80, 50, 60 );
    latLonAltBox2.setMaxAltitude( 100 );
    latLonAltBox2.setMinAltitude( 20 );
    latLonAltBox2.setAltitudeMode( Marble::Absolute );

    region1.setLatLonAltBox( latLonAltBox1 );
    region2.setLatLonAltBox( latLonAltBox2 );

    GeoDataLod lod1;
    GeoDataLod lod2;
    lod1.setMaxFadeExtent( 20 );
    lod1.setMinFadeExtent( 10 );
    lod1.setMaxLodPixels( 30 );
    lod1.setMinLodPixels( 5 );

    lod2.setMaxFadeExtent( 20 );
    lod2.setMinFadeExtent( 10 );
    lod2.setMaxLodPixels( 30 );
    lod2.setMinLodPixels( 5 );

    region1.setLod( lod1 );
    region2.setLod( lod2 );

    QCOMPARE( region1, region1 );
    QCOMPARE( region2, region2 );
    QCOMPARE( region1 != region2, false );
    QVERIFY( region1 == region2 );

    region2.lod().setMaxFadeExtent( 30 );

    QCOMPARE( region1, region1 );
    QCOMPARE( region2, region2 );
    QCOMPARE( region1 == region2, false );
    QVERIFY( region1 != region2 );
}

QTEST_MAIN( TestEquality )

#include "TestEquality.moc"
