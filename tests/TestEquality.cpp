//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Cruceru Calin-Cristian <crucerucalincristian@gmail.com>
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

    camera1.setAltitudeMode(Marble::ClampToGround);
    camera1.setAltitude(100);
    camera1.setLatitude(100);
    camera1.setLongitude(100);
    camera1.setHeading(200);
    camera1.setRoll(300);
    camera1.setTilt(400);

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


QTEST_MAIN( TestEquality )

#include "TestEquality.moc"
