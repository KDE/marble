//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Cruceru Calin-Cristian <crucerucalincristian@gmail.com>
// Copyright 2014      Sanjiban Bairagya      <sanjiban22393@gmail.com>
// Copyright 2014      Abhinav Gangwar        <abhgang@gmail.com>
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
#include <GeoDataIconStyle.h>
#include <GeoDataLabelStyle.h>
#include <GeoDataLineStyle.h>
#include <GeoDataListStyle.h>
#include <GeoDataPolyStyle.h>
#include <GeoDataStyle.h>
#include <GeoDataStyleMap.h>
#include <GeoDataTimeSpan.h>
#include <GeoDataTimeStamp.h>
#include <GeoDataViewVolume.h>
#include <GeoDataVec2.h>
#include <GeoDataScreenOverlay.h>
#include <GeoDataPhotoOverlay.h>
#include <GeoDataGroundOverlay.h>
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
    void iconStyleTest();
    void lineStyleTest();
    void listStyleTest();
    void polyStyleTest();
    void labelStyleTest();
    void styleTest();
    void styleMapTest();
    void timeSpanTest();
    void timeStampTest();
    void viewVolumeTest();
    void vecTest();
    void screenOverlayTest();
    void photoOverlayTest();
    void groundOverlayTest();
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


    GeoDataTimeStamp timeStampBegin, timeStampEnd;
    QDateTime date1( QDate(2014, 4, 7) );

    timeStampBegin.setWhen( date1 );
    timeStampEnd.setWhen( date1 );
    timeStampBegin.setResolution( GeoDataTimeStamp::YearResolution );
    timeStampEnd.setResolution( GeoDataTimeStamp::YearResolution );

    GeoDataTimeSpan timeSpan1, timeSpan2;
    timeSpan1.setBegin( timeStampBegin );
    timeSpan1.setEnd( timeStampEnd );
    timeSpan2.setBegin( timeStampBegin );
    timeSpan2.setEnd( timeStampEnd );

    GeoDataTimeStamp timeStamp1, timeStamp2;
    QDateTime date2( QDate(2014, 4, 8) );
    timeStamp1.setWhen( date2 );
    timeStamp2.setWhen( date2 );
    timeStamp1.setResolution( GeoDataTimeStamp::SecondResolution );
    timeStamp2.setResolution( GeoDataTimeStamp::SecondResolution );

    camera1.setTimeSpan( timeSpan1 );
    camera2.setTimeSpan( timeSpan2 );
    camera1.setTimeStamp( timeStamp1 );
    camera2.setTimeStamp( timeStamp2 );

    camera2.setAltitudeMode( Marble::Absolute );

    QCOMPARE( camera1, camera2 );
    QCOMPARE( camera2, camera2 );
    QVERIFY( camera1 == camera2 );

    camera1.setId("camera1");
    camera2.setId("camera2");
    QVERIFY( camera1 != camera2 );

    camera1.setId("camera2");
    QVERIFY( camera1 == camera2 );

    timeStamp1.setResolution( GeoDataTimeStamp::YearResolution );
    camera2.setTimeStamp( timeStamp1 );
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
    img1.fill( Qt::green );
    img2.fill( Qt::green );

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

void TestEquality::timeSpanTest()
{
    GeoDataTimeSpan timeSpan1, timeSpan2;
    GeoDataTimeStamp timeStampBegin, timeStampEnd;

    QDateTime date1( QDate(2014, 1, 10) ), date2( QDate(2014, 4, 21) );

    timeStampBegin.setWhen( date1 );
    timeStampEnd.setWhen( date2 );
    timeStampBegin.setResolution( GeoDataTimeStamp::DayResolution );
    timeStampEnd.setResolution( GeoDataTimeStamp::DayResolution );

    timeSpan1.setBegin( timeStampBegin );
    timeSpan1.setEnd( timeStampEnd );
    timeSpan2.setBegin( timeStampBegin );
    timeSpan2.setEnd( timeStampBegin );

    QCOMPARE( timeSpan1, timeSpan1 );
    QCOMPARE( timeSpan2, timeSpan2 );
    QCOMPARE( timeSpan1 == timeSpan2, false );
    QVERIFY( timeSpan1 != timeSpan2 );

    timeSpan2.setEnd( timeStampEnd );

    timeSpan1.setTargetId("timeSpan2");
    timeSpan2.setTargetId("timeSpan1");
    QVERIFY( timeSpan1 != timeSpan2 );

    timeSpan1.setTargetId("timeSpan1");
    QVERIFY( timeSpan1 == timeSpan2 );
}

void TestEquality::timeStampTest()
{
    GeoDataTimeStamp timeStamp1, timeStamp2;
    QDateTime date1( QDate(1994, 10, 4) ), date2( QDate(2070, 10, 4) );

    timeStamp1.setWhen( date1 );
    timeStamp2.setWhen( date2 );
    timeStamp1.setResolution( GeoDataTimeStamp::YearResolution );
    timeStamp2.setResolution( GeoDataTimeStamp::DayResolution );

    QCOMPARE( timeStamp1, timeStamp1 );
    QCOMPARE( timeStamp2, timeStamp2 );
    QCOMPARE( timeStamp1 == timeStamp2, false );
    QVERIFY( timeStamp1 != timeStamp2 );

    timeStamp2.setWhen( date1 );
    timeStamp1.setResolution( GeoDataTimeStamp::DayResolution );

    timeStamp1.setId("timeStamp1");
    timeStamp2.setId("timeStamp2");
    QVERIFY( timeStamp1 != timeStamp2 );

    timeStamp1.setId("timeStamp2");
    QVERIFY( timeStamp1 == timeStamp2 );
}

void TestEquality::iconStyleTest()
{
    GeoDataIconStyle iconStyle1, iconStyle2;
    QImage icon( 50, 50, QImage::Format_Mono );
    icon.fill( Qt::black );
    QPointF hotSpot = QPointF( 6.7, 4.6 );

    iconStyle1.setScale( 1.0 );
    iconStyle1.setIconPath( "path/to/icon" );
    iconStyle1.setIcon( icon );
    iconStyle1.setHotSpot( hotSpot, GeoDataHotSpot::Fraction, GeoDataHotSpot::Fraction );
    iconStyle1.setHeading( 0 );

    iconStyle2.setScale( 1.0 );
    iconStyle2.setIconPath( "path/to/icon" );
    iconStyle2.setIcon( icon );
    iconStyle2.setHotSpot( hotSpot, GeoDataHotSpot::Fraction, GeoDataHotSpot::Fraction );
    iconStyle2.setHeading( 0 );

    QCOMPARE( iconStyle1, iconStyle1 );
    QCOMPARE( iconStyle2, iconStyle2 );
    QCOMPARE( iconStyle1 != iconStyle2, false );
    QVERIFY( iconStyle1 == iconStyle2 );

    iconStyle2.setHeading( 1 );
    iconStyle2.setScale( 2.0 );

    QCOMPARE( iconStyle1, iconStyle1 );
    QCOMPARE( iconStyle2, iconStyle2 );
    QCOMPARE( iconStyle1 == iconStyle2, false );
    QVERIFY( iconStyle1 != iconStyle2 );

}

void TestEquality::lineStyleTest()
{
    GeoDataLineStyle lineStyle1, lineStyle2;
    QVector< qreal > pattern( 7, 7.8 );
    
    lineStyle1.setWidth( 1.3 );
    lineStyle1.setPhysicalWidth( 1.0 );
    lineStyle1.setCapStyle( Qt::RoundCap );
    lineStyle1.setPenStyle( Qt::SolidLine );
    lineStyle1.setBackground( false );
    lineStyle1.setDashPattern( pattern );

    lineStyle2.setWidth( 1.3 );
    lineStyle2.setPhysicalWidth( 1.0 );
    lineStyle2.setCapStyle( Qt::RoundCap );
    lineStyle2.setPenStyle( Qt::SolidLine );
    lineStyle2.setBackground( false );
    lineStyle2.setDashPattern( pattern );

    QCOMPARE( lineStyle1, lineStyle2 );
    QCOMPARE( lineStyle2, lineStyle2 );
    QCOMPARE( lineStyle1 != lineStyle2, false );
    QVERIFY( lineStyle1 == lineStyle2 );

    lineStyle2.setCapStyle( Qt::FlatCap );
    lineStyle2.setBackground( true );

    QCOMPARE( lineStyle1, lineStyle1 );
    QCOMPARE( lineStyle2, lineStyle2 );
    QCOMPARE( lineStyle1 == lineStyle2, false );
    QVERIFY( lineStyle1 != lineStyle2 );
}

void TestEquality::listStyleTest()
{
    GeoDataListStyle listStyle1, listStyle2;
    
    listStyle1.setListItemType( GeoDataListStyle::Check );
    listStyle1.setBackgroundColor( Qt::gray );
    for( int i = 0; i < 5; ++i ) {
        GeoDataItemIcon *icon = new GeoDataItemIcon;
        QImage img( 10 * ( i + 1 ), 10 * ( i + 1 ), QImage::Format_Mono );
        img.fill( Qt::black );
        icon->setIcon( img );
        icon->setIconPath( QString("path/to/icon") );
        icon->setState( GeoDataItemIcon::Open );
        listStyle1.append( icon );
    }

    listStyle2.setListItemType( GeoDataListStyle::Check );
    listStyle2.setBackgroundColor( Qt::gray );
    for( int i = 0; i < 5; ++i ) {
        GeoDataItemIcon *icon = new GeoDataItemIcon;
        QImage img( 10 * ( i + 1 ), 10 * ( i + 1 ), QImage::Format_Mono );
        img.fill( Qt::black );
        icon->setIcon( img );
        icon->setIconPath( QString("path/to/icon") );
        icon->setState( GeoDataItemIcon::Open );
        listStyle2.append( icon );
    }

    QCOMPARE( listStyle1, listStyle1 );
    QCOMPARE( listStyle2, listStyle2 );
    QCOMPARE( listStyle1 != listStyle2, false );
    QVERIFY( listStyle1 == listStyle2 );

    listStyle2.setBackgroundColor( Qt::green );
    listStyle2.setListItemType( GeoDataListStyle::RadioFolder );

    QCOMPARE( listStyle1, listStyle1 );
    QCOMPARE( listStyle2, listStyle2 );
    QCOMPARE( listStyle1 == listStyle2, false );
    QVERIFY( listStyle1 != listStyle2 );
}

void TestEquality::polyStyleTest()
{
    GeoDataPolyStyle polyStyle1, polyStyle2;

    polyStyle1.setColor( Qt::red );
    polyStyle1.setFill( false );
    polyStyle1.setOutline( false );
    polyStyle1.setBrushStyle( Qt::SolidPattern );

    polyStyle2.setColor( Qt::red );
    polyStyle2.setFill( false );
    polyStyle2.setOutline( false );
    polyStyle2.setBrushStyle( Qt::SolidPattern );

    QCOMPARE( polyStyle1, polyStyle1 );
    QCOMPARE( polyStyle2, polyStyle2 );
    QCOMPARE( polyStyle1 != polyStyle2, false );
    QVERIFY( polyStyle1 == polyStyle2 );

    polyStyle2.setOutline( true );
    polyStyle2.setBrushStyle( Qt::CrossPattern );

    QCOMPARE( polyStyle1, polyStyle1 );
    QCOMPARE( polyStyle2, polyStyle2 );
    QCOMPARE( polyStyle1 == polyStyle2, false );
    QVERIFY( polyStyle1 != polyStyle2 );
}

void TestEquality::labelStyleTest()
{
    GeoDataLabelStyle labelStyle1, labelStyle2;

    labelStyle1.setColor( Qt::blue );
    labelStyle1.setScale( 1.0 );
    labelStyle1.setAlignment( GeoDataLabelStyle::Center );
    labelStyle1.setFont( QFont( "Helvetica", 10 ) );

    labelStyle2.setColor( Qt::blue );
    labelStyle2.setScale( 1.0 );
    labelStyle2.setAlignment( GeoDataLabelStyle::Center );
    labelStyle2.setFont( QFont( "Helvetica", 10 ) );

    QCOMPARE( labelStyle1, labelStyle1 );
    QCOMPARE( labelStyle2, labelStyle2 );
    QCOMPARE( labelStyle1 != labelStyle2, false );
    QVERIFY( labelStyle1 == labelStyle2);

    labelStyle2.setAlignment( GeoDataLabelStyle::Corner );
    labelStyle2.setFont( QFont( "Helvetica [Cronyx]", 12 ) );

    QCOMPARE( labelStyle1, labelStyle1 );
    QCOMPARE( labelStyle2, labelStyle2 );
    QCOMPARE( labelStyle1 == labelStyle2, false);
    QVERIFY( labelStyle1 != labelStyle2 );
}

void TestEquality::styleTest()
{
    GeoDataStyle style1, style2;
    GeoDataIconStyle iconStyle;
    QImage icon( 50, 50, QImage::Format_Mono );
    icon.fill( Qt::black );
    QPointF hotSpot = QPointF( 7.6, 6.4 );

    iconStyle.setScale( 1.0 );
    iconStyle.setIconPath( "path/to/icon" );
    iconStyle.setIcon( icon );
    iconStyle.setHotSpot( hotSpot, GeoDataHotSpot::Fraction, GeoDataHotSpot::Fraction );
    iconStyle.setHeading( 0 );
    
    GeoDataLabelStyle labelStyle;
    labelStyle.setColor( Qt::blue );
    labelStyle.setScale( 1.0 );
    labelStyle.setAlignment( GeoDataLabelStyle::Center );
    labelStyle.setFont( QFont( "Helvetica", 10 ) );

    GeoDataLineStyle lineStyle;
    QVector< qreal > pattern( 5, 6.2 );
    lineStyle.setWidth( 1.2 );
    lineStyle.setPhysicalWidth( 1.0 );
    lineStyle.setCapStyle( Qt::RoundCap );
    lineStyle.setPenStyle( Qt::SolidLine );
    lineStyle.setBackground( false );
    lineStyle.setDashPattern( pattern );

    GeoDataPolyStyle polyStyle;
    polyStyle.setColor( Qt::red );
    polyStyle.setFill( false );
    polyStyle.setOutline( false );
    polyStyle.setBrushStyle( Qt::SolidPattern );

    GeoDataBalloonStyle balloon;
    balloon.setBackgroundColor(Qt::white);
    balloon.setTextColor(Qt::black);
    balloon.setText("SomeText");
    balloon.setDisplayMode(GeoDataBalloonStyle::Hide);

    GeoDataListStyle listStyle;    
    listStyle.setListItemType( GeoDataListStyle::Check );
    listStyle.setBackgroundColor( Qt::gray );
    for( int i = 0; i < 5; ++i ) {
        GeoDataItemIcon *icon = new GeoDataItemIcon;
        QImage img( 20 * ( i + 1 ), 20 * ( i + 1 ), QImage::Format_Mono );
        img.fill( Qt::black );
        icon->setIcon( img );
        icon->setIconPath( QString("path/to/icon") );
        icon->setState( GeoDataItemIcon::Open );
        listStyle.append( icon );
    }

    style1.setIconStyle( iconStyle );
    style1.setLineStyle( lineStyle );
    style1.setLabelStyle( labelStyle );
    style1.setPolyStyle( polyStyle );
    style1.setBalloonStyle( balloon );
    style1.setListStyle( listStyle );

    style2.setIconStyle( iconStyle );
    style2.setListStyle( listStyle );
    style2.setLabelStyle( labelStyle );
    style2.setPolyStyle( polyStyle );
    style2.setBalloonStyle( balloon );
    style2.setListStyle( listStyle );

    QCOMPARE( style1, style1 );
    QCOMPARE( style2, style2 );
    QCOMPARE( style1 != style2, false );
    QVERIFY( style1 == style2 );

    iconStyle.setScale( 2.0 );
    labelStyle.setAlignment( GeoDataLabelStyle::Corner );
    lineStyle.setBackground( true );
    polyStyle.setOutline( true );

    style2.setIconStyle( iconStyle );
    style2.setLabelStyle( labelStyle );
    style2.setLineStyle( lineStyle );
    style2.setPolyStyle( polyStyle );
    
    QCOMPARE( style1, style1 );
    QCOMPARE( style2, style2 );
    QCOMPARE( style1 == style2, false );
    QVERIFY( style1 != style2 );
}

void TestEquality::styleMapTest()
{
    GeoDataStyleMap styleMap1, styleMap2;
    styleMap1["germany"] = "gst1";
    styleMap1["germany"] = "gst2";
    styleMap1["germany"] = "gst3";
    styleMap1["poland"] = "pst1";
    styleMap1["poland"] = "pst2";
    styleMap1["poland"] = "pst3";
    styleMap1.setLastKey("poland");

    styleMap2["germany"] = "gst1";
    styleMap2["germany"] = "gst2";
    styleMap2["germany"] = "gst3";
    styleMap2["poland"] = "pst1";
    styleMap2["poland"] = "pst2";
    styleMap2["poland"] = "pst3";
    styleMap2.setLastKey("poland");

    QCOMPARE( styleMap1, styleMap1 );
    QCOMPARE( styleMap2, styleMap2 );
    QCOMPARE( styleMap1 != styleMap2, false );

    styleMap2.insert("Romania", "rst1");
    styleMap2.insert("Romania", "rst2");
    styleMap2.insert("Romania", "rst3");
    styleMap2.setLastKey("Romania");

    QCOMPARE( styleMap1, styleMap1 );
    QCOMPARE( styleMap2, styleMap2 );
    QCOMPARE( styleMap1 == styleMap2, false );
    QVERIFY( styleMap1 != styleMap2 );
}

void TestEquality::viewVolumeTest()
{
    GeoDataViewVolume volume1, volume2;

    volume1.setLeftFov( 5.6 );
    volume1.setRightFov( 6.5 );
    volume1.setTopFov( 2.3 );
    volume1.setBottomFov( 9.2 );
    volume1.setNear( 8.6 );

    volume2.setLeftFov( 5.6 );
    volume2.setRightFov( 6.5 );
    volume2.setTopFov( 2.3 );
    volume2.setBottomFov( 9.2 );
    volume2.setNear( 8.6 );

    QCOMPARE( volume1, volume1 );
    QCOMPARE( volume2, volume2 );
    QCOMPARE( volume1 != volume2, false );
    QVERIFY( volume1 == volume2 );

    volume2.setRightFov( 7.3 );

    QCOMPARE( volume1, volume1 );
    QCOMPARE( volume2, volume2 );
    QCOMPARE( volume1 == volume2, false );
    QVERIFY( volume1 != volume2 );
}

void TestEquality::vecTest()
{
    GeoDataVec2 vec1, vec2;

    vec1.setXunits( GeoDataVec2::Fraction );
    vec1.setYunits( GeoDataVec2::Pixels );

    vec2.setXunits( GeoDataVec2::Fraction );
    vec2.setYunits( GeoDataVec2::Pixels );

    QCOMPARE( vec1, vec1 );
    QCOMPARE( vec2, vec2 );
    QCOMPARE( vec1 != vec2, false );
    QVERIFY( vec1 == vec2 );

    vec2.setYunits( GeoDataVec2::Fraction );

    QCOMPARE( vec1, vec1 );
    QCOMPARE( vec2, vec2 );
    QCOMPARE( vec1 == vec2, false );
    QVERIFY( vec1 != vec2 );
}

void TestEquality::screenOverlayTest()
{
    GeoDataVec2 vec1, vec2, vec3, vec4;

    vec1.setXunits( GeoDataVec2::Fraction );
    vec1.setYunits( GeoDataVec2::Pixels );

    vec2.setXunits( GeoDataVec2::Fraction );
    vec2.setYunits( GeoDataVec2::InsetPixels );

    vec3.setXunits( GeoDataVec2::Pixels );
    vec3.setYunits( GeoDataVec2::Pixels );

    vec4.setXunits( GeoDataVec2::Fraction );
    vec4.setYunits( GeoDataVec2::Fraction );

    GeoDataScreenOverlay overlay1, overlay2;

    overlay1.setOverlayXY( vec1 );
    overlay1.setRotationXY( vec2 );
    overlay1.setScreenXY( vec3 );
    overlay1.setSize( vec4 );
    overlay1.setRotation( 3.4 );

    overlay2.setOverlayXY( vec1 );
    overlay2.setRotationXY( vec2 );
    overlay2.setScreenXY( vec3 );
    overlay2.setSize( vec4 );
    overlay2.setRotation( 3.4 );

    QCOMPARE( overlay1, overlay1 );
    QCOMPARE( overlay2, overlay2 );
    QCOMPARE( overlay1 != overlay2, false );
    QVERIFY( overlay1 == overlay2 );

    overlay2.setRotation( 7.3 );

    QCOMPARE( overlay1, overlay1 );
    QCOMPARE( overlay2, overlay2 );
    QCOMPARE( overlay1 == overlay2, false );
    QVERIFY( overlay1 != overlay2 );
}

void TestEquality::photoOverlayTest()
{
    GeoDataPhotoOverlay overlay1, overlay2;
    GeoDataViewVolume volume1, volume2;

    volume1.setLeftFov( 5.6 );
    volume1.setRightFov( 6.5 );
    volume1.setTopFov( 2.3 );
    volume1.setBottomFov( 9.2 );
    volume1.setNear( 8.6 );

    volume2.setLeftFov( 5.6 );
    volume2.setRightFov( 6.5 );
    volume2.setTopFov( 2.3 );
    volume2.setBottomFov( 9.2 );
    volume2.setNear( 8.6 );

    overlay1.setViewVolume( volume1 );
    overlay2.setViewVolume( volume2 );

    GeoDataImagePyramid pyramid1, pyramid2;

    pyramid1.setTileSize( 3 );
    pyramid1.setMaxWidth( 5 );
    pyramid1.setMaxHeight( 8 );
    pyramid1.setGridOrigin( GeoDataImagePyramid::UpperLeft );

    pyramid2.setTileSize( 3 );
    pyramid2.setMaxWidth( 5 );
    pyramid2.setMaxHeight( 8 );
    pyramid2.setGridOrigin( GeoDataImagePyramid::UpperLeft );

    overlay1.setImagePyramid( pyramid1 );
    overlay2.setImagePyramid( pyramid2 );

    GeoDataPoint point1, point2;
    GeoDataCoordinates coord1, coord2;

    coord1.set(100,100,100);
    coord2.set(100,100,100);
    point1.setCoordinates(coord1);
    point2.setCoordinates(coord2);
    overlay1.setPoint( point1 );
    overlay2.setPoint( point2 );

    overlay1.setShape( GeoDataPhotoOverlay::Cylinder );
    overlay2.setShape( GeoDataPhotoOverlay::Cylinder );

    overlay1.setRotation( 2.5 );
    overlay2.setRotation( 2.5 );

    QCOMPARE( overlay1, overlay1 );
    QCOMPARE( overlay2, overlay2 );
    QCOMPARE( overlay1 != overlay2, false );
    QVERIFY( overlay1 == overlay2 );

    overlay2.setRotation( 4.3 );

    QCOMPARE( overlay1, overlay1 );
    QCOMPARE( overlay2, overlay2 );
    QCOMPARE( overlay1 == overlay2, false );
    QVERIFY( overlay1 != overlay2 );
}

void TestEquality::groundOverlayTest()
{
    GeoDataGroundOverlay overlay1, overlay2;
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

    overlay1.setLatLonBox( latLonBox1 );
    overlay2.setLatLonBox( latLonBox2 );

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

    overlay1.setLatLonQuad( quad1 );
    overlay2.setLatLonQuad( quad2 );

    overlay1.setAltitude( 23.5 );
    overlay2.setAltitude( 23.5 );

    overlay1.setAltitudeMode( Marble::Absolute );
    overlay2.setAltitudeMode( Marble::Absolute );

    QCOMPARE( overlay1, overlay1 );
    QCOMPARE( overlay2, overlay2 );
    QCOMPARE( overlay1 != overlay2, false );
    QVERIFY( overlay1 == overlay2 );

    overlay2.setAltitude( 42.3 );

    QCOMPARE( overlay1, overlay1 );
    QCOMPARE( overlay2, overlay2 );
    QCOMPARE( overlay1 == overlay2, false );
    QVERIFY( overlay1 != overlay2 );
}

QTEST_MAIN( TestEquality )

#include "TestEquality.moc"
