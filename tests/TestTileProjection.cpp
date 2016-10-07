/*
    Copyright 2016 Friedrich W. H. Kossebau  <kossebau@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "TestUtils.h"

#include <GeoSceneEquirectTileProjection.h>
#include <GeoSceneMercatorTileProjection.h>
#include <GeoDataLatLonBox.h>
#include <TileId.h>


namespace Marble
{

class TileProjectionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testTypeEquirect();
    void testTypeMercator();

    void testLevelZeroColumnsRowsEquirect();
    void testLevelZeroColumnsRowsMercator();

    void testTileIndexesEquirect_data();
    void testTileIndexesEquirect();
    void testTileIndexesMercator_data();
    void testTileIndexesMercator();

    void testGeoCoordinatesEquirect_data();
    void testGeoCoordinatesEquirect();
    void testGeoCoordinatesMercator_data();
    void testGeoCoordinatesMercator();

private:
    void testLevelZeroColumnsRows(GeoSceneAbstractTileProjection& projection);
};


void TileProjectionTest::testLevelZeroColumnsRows(GeoSceneAbstractTileProjection& projection)
{
    // test default
    QCOMPARE(projection.levelZeroColumns(), 1);
    QCOMPARE(projection.levelZeroRows(), 1);

    // test setting a different value
    const int levelZeroColumns = 4;
    const int levelZeroRows = 6;

    projection.setLevelZeroColumns(levelZeroColumns);
    projection.setLevelZeroRows(levelZeroRows);

    QCOMPARE(projection.levelZeroColumns(), levelZeroColumns);
    QCOMPARE(projection.levelZeroRows(), levelZeroRows);
}

void TileProjectionTest::testLevelZeroColumnsRowsEquirect()
{
    GeoSceneEquirectTileProjection projection;
    testLevelZeroColumnsRows(projection);
}

void TileProjectionTest::testLevelZeroColumnsRowsMercator()
{
    GeoSceneMercatorTileProjection projection;
    testLevelZeroColumnsRows(projection);
}

void TileProjectionTest::testTypeEquirect()
{
    GeoSceneEquirectTileProjection projection;
    QCOMPARE(projection.type(), GeoSceneAbstractTileProjection::Equirectangular);
}

void TileProjectionTest::testTypeMercator()
{
    GeoSceneMercatorTileProjection projection;
    QCOMPARE(projection.type(), GeoSceneAbstractTileProjection::Mercator);
}


void TileProjectionTest::testTileIndexesEquirect_data()
{
    QTest::addColumn<qreal>("westLon");
    QTest::addColumn<qreal>("northLat");
    QTest::addColumn<qreal>("eastLon");
    QTest::addColumn<qreal>("southLat");
    QTest::addColumn<int>("zoomLevel");
    QTest::addColumn<int>("expectedTileXWest");
    QTest::addColumn<int>("expectedTileYNorth");
    QTest::addColumn<int>("expectedTileXEast");
    QTest::addColumn<int>("expectedTileYSouth");

    // zoomlevel zero: 1 tile
    // bounds matching the tile map
    addRow() << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 0
             << 0 << 0 << 0 << 0;
    // bounds inside the 1 tile
    addRow() << qreal(-M_PI*0.5) << qreal(+M_PI * 0.25)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 0
             << 0 << 0 << 0 << 0;
    // bounds west and north on tile map borders, with normal border values
    addRow() << qreal(-M_PI)     << qreal(+M_PI * 0.5)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 0
             << 0 << 0 << 0 << 0;
    // bounds west and north on tile map borders, with border values from other map border sides
    addRow() << qreal(+M_PI)     << qreal(-M_PI * 0.5)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 0
             << 0 << 0 << 0 << 0;

    // zoomlevel 1: 2 tiles per dimension
    // bounds matching the tile map
    addRow() << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 1
             << 0 << 0 << 1 << 1;
    // bounds inside the 4 tiles
    addRow() << qreal(-M_PI*0.5) << qreal(+M_PI * 0.25)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 1
             << 0 << 0 << 1 << 1;
    // bounds matching the most north-west tile, with normal border values
    addRow() << qreal(-M_PI)     << qreal(+M_PI * 0.5)
             << qreal(0)         << qreal(0)
             << 1
             << 0 << 0 << 0 << 0;
    // bounds matching the most north-west tile, with border values from other map border sides
    addRow() << qreal(+M_PI)     << qreal(-M_PI * 0.5)
             << qreal(0)         << qreal(0)
             << 1
             << 0 << 0 << 0 << 0;
    // bounds matching the most south-east tile, with normal border values
    addRow() << qreal(0) << qreal(0)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 1
             << 1 << 1 << 1 << 1;
    // bounds matching the most south-east tile, with border values from other map border sides
    addRow() << qreal(0) << qreal(0)
             << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << 1
             << 1 << 1 << 1 << 1;

    // zoomlevel 9: 2^8==512 tiles per dimension
    // bounds matching the tile map
    addRow() << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 9
             << 0 << 0 << 511 << 511;
    // bounds inside the outer tiles
    addRow() << qreal(-M_PI*(511/512.0)) << qreal(+M_PI * 0.5 * (511/512.0))
             << qreal(+M_PI*(511/512.0)) << qreal(-M_PI * 0.5 * (511/512.0))
             << 9
             << 0 << 0 << 511 << 511;
    // bounds matching the most north-west tile, with normal border values
    addRow() << qreal(-M_PI)             << qreal(+M_PI * 0.5)
             << qreal(-M_PI*(255/256.0)) << qreal(+M_PI * 0.5 *(255/256.0))
             << 9
             << 0 << 0 << 0 << 0;
    // bounds matching the most north-west tile, with border values from other map border sides
    addRow() << qreal(+M_PI)             << qreal(-M_PI * 0.5)
             << qreal(-M_PI*(255/256.0)) << qreal(+M_PI * 0.5 *(255/256.0))
             << 9
             << 0 << 0 << 0 << 0;
    // bounds matching the most south-east tile, with normal border values
    addRow() << qreal(+M_PI*(255/256.0)) << qreal(-M_PI * 0.5 *(255/256.0))
             << qreal(+M_PI)             << qreal(-M_PI * 0.5)
             << 9
             << 511 << 511 << 511 << 511;
    // bounds matching the most south-east tile, with border values from other map border sides
    addRow() << qreal(+M_PI*(255/256.0)) << qreal(-M_PI * 0.5 *(255/256.0))
             << qreal(-M_PI)             << qreal(+M_PI * 0.5)
             << 9
             << 511 << 511 << 511 << 511;
}


void TileProjectionTest::testTileIndexesEquirect()
{
    QFETCH(qreal, westLon);
    QFETCH(qreal, northLat);
    QFETCH(qreal, eastLon);
    QFETCH(qreal, southLat);
    QFETCH(int, zoomLevel);
    QFETCH(int, expectedTileXWest);
    QFETCH(int, expectedTileYNorth);
    QFETCH(int, expectedTileXEast);
    QFETCH(int, expectedTileYSouth);

    GeoDataLatLonBox latLonBox(northLat, southLat, eastLon, westLon);

    GeoSceneEquirectTileProjection projection;

    int tileXWest;
    int tileYNorth;
    int tileXEast;
    int tileYSouth;

    projection.tileIndexes(latLonBox, zoomLevel, tileXWest, tileYNorth, tileXEast, tileYSouth);

    QCOMPARE(tileXWest, expectedTileXWest);
    QCOMPARE(tileYNorth, expectedTileYNorth);
    QCOMPARE(tileXEast, expectedTileXEast);
    QCOMPARE(tileYSouth, expectedTileYSouth);
}


void TileProjectionTest::testTileIndexesMercator_data()
{
    QTest::addColumn<qreal>("westLon");
    QTest::addColumn<qreal>("northLat");
    QTest::addColumn<qreal>("eastLon");
    QTest::addColumn<qreal>("southLat");
    QTest::addColumn<int>("zoomLevel");
    QTest::addColumn<int>("expectedTileXWest");
    QTest::addColumn<int>("expectedTileYNorth");
    QTest::addColumn<int>("expectedTileXEast");
    QTest::addColumn<int>("expectedTileYSouth");

    // zoomlevel zero: 1 tile
    // bounds matching the tile map up to 90 degree latitude
    addRow() << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 0
             << 0 << 0 << 0 << 0;
    // bounds matching the tile map with 85 degree latitude limit
    addRow() << qreal(-M_PI) << qreal(85.0 * DEG2RAD)
             << qreal(+M_PI) << qreal(-85.0 * DEG2RAD)
             << 0
             << 0 << 0 << 0 << 0;
    // bounds inside the 1 tile
    addRow() << qreal(-M_PI*0.5) << qreal(+M_PI * 0.25)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 0
             << 0 << 0 << 0 << 0;
    // bounds west and north on tile map borders, with normal border values
    addRow() << qreal(-M_PI)     << qreal(+M_PI * 0.5)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 0
             << 0 << 0 << 0 << 0;
    // bounds west and north on tile map borders, with border values from other map border sides
    addRow() << qreal(+M_PI)     << qreal(-M_PI * 0.5)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 0
             << 0 << 0 << 0 << 0;

    // zoomlevel 1: 2 tiles per dimension
    // bounds matching the tile map up to 90 degree latitude
    addRow() << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 1
             << 0 << 0 << 1 << 1;
    // bounds matching the tile map with 85 degree latitude limit
    addRow() << qreal(-M_PI) << qreal(85.0 * DEG2RAD)
             << qreal(+M_PI) << qreal(-85.0 * DEG2RAD)
             << 1
             << 0 << 0 << 1 << 1;
    // bounds inside the 4 tiles
    addRow() << qreal(-M_PI*0.5) << qreal(+M_PI * 0.25)
             << qreal(+M_PI*0.5) << qreal(-M_PI * 0.25)
             << 1
             << 0 << 0 << 1 << 1;
    // bounds matching the most north-west tile, with normal border values
    addRow() << qreal(-M_PI)     << qreal(+M_PI * 0.5)
             << qreal(0)         << qreal(0)
             << 1
             << 0 << 0 << 0 << 0;
    // bounds matching the most north-west tile, with border values from other map border sides
    addRow() << qreal(+M_PI)     << qreal(-M_PI * 0.5)
             << qreal(0)         << qreal(0)
             << 1
             << 0 << 0 << 0 << 0;
    // bounds matching the most south-east tile, with normal border values
    addRow() << qreal(0) << qreal(0)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 1
             << 1 << 1 << 1 << 1;
    // bounds matching the most south-east tile, with border values from other map border sides
    addRow() << qreal(0) << qreal(0)
             << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << 1
             << 1 << 1 << 1 << 1;

    // zoomlevel 9: 2^8==512 tiles per dimension
    // GeoSceneMercatorTileProjection bounds latitude value at +/- 85.0 degree (so not at 85.05113),
    // which results in some tiles missed at the outer sides.
    // bounds matching the tile map up to 90 degree latitude
    addRow() << qreal(-M_PI) << qreal(+M_PI * 0.5)
             << qreal(+M_PI) << qreal(-M_PI * 0.5)
             << 9
             << 0 << 5 << 511 << 506;
    // bounds matching the tile map with 85 degree latitude limit
    addRow() << qreal(-M_PI) << qreal(85.0 * DEG2RAD)
             << qreal(+M_PI) << qreal(-85.0 * DEG2RAD)
             << 9
             << 0 << 5 << 511 << 506;
    // bounds inside the outer tiles
    addRow() << qreal(-M_PI*(511/512.0)) << qreal(+M_PI * 0.5 * (511/512.0))
             << qreal(+M_PI*(511/512.0)) << qreal(-M_PI * 0.5 * (511/512.0))
             << 9
             << 0 << 5 << 511 << 506;
    // bounds matching the most north-west tile, with normal border values
    addRow() << qreal(-M_PI)             << qreal(+M_PI * 0.5)
             << qreal(-M_PI*(255/256.0)) << qreal(+M_PI * 0.5 *(255/256.0))
             << 9
             << 0 << 5 << 0 << 5;
    // bounds matching the most north-west tile, with border values from other map border sides
    addRow() << qreal(+M_PI)             << qreal(-M_PI * 0.5)
             << qreal(-M_PI*(255/256.0)) << qreal(+M_PI * 0.5 *(255/256.0))
             << 9
             << 0 << 5 << 0 << 5;
    // bounds matching the most south-east tile, with normal border values
    addRow() << qreal(+M_PI*(255/256.0)) << qreal(-M_PI * 0.5 *(255/256.0))
             << qreal(+M_PI)             << qreal(-M_PI * 0.5)
             << 9
             << 511 << 506 << 511 << 506;
    // bounds matching the most south-east tile, with border values from other map border sides
    addRow() << qreal(+M_PI*(255/256.0)) << qreal(-M_PI * 0.5 *(255/256.0))
             << qreal(-M_PI)             << qreal(+M_PI * 0.5)
             << 9
             << 511 << 506 << 511 << 506;
}


void TileProjectionTest::testTileIndexesMercator()
{
    QFETCH(qreal, westLon);
    QFETCH(qreal, northLat);
    QFETCH(qreal, eastLon);
    QFETCH(qreal, southLat);
    QFETCH(int, zoomLevel);
    QFETCH(int, expectedTileXWest);
    QFETCH(int, expectedTileYNorth);
    QFETCH(int, expectedTileXEast);
    QFETCH(int, expectedTileYSouth);

    GeoDataLatLonBox latLonBox(northLat, southLat, eastLon, westLon);

    GeoSceneMercatorTileProjection projection;

    int tileXWest;
    int tileYNorth;
    int tileXEast;
    int tileYSouth;

    projection.tileIndexes(latLonBox, zoomLevel, tileXWest, tileYNorth, tileXEast, tileYSouth);

    QCOMPARE(tileXWest, expectedTileXWest);
    QCOMPARE(tileYNorth, expectedTileYNorth);
    QCOMPARE(tileXEast, expectedTileXEast);
    QCOMPARE(tileYSouth, expectedTileYSouth);
}


void TileProjectionTest::testGeoCoordinatesEquirect_data()
{
    QTest::addColumn<int>("tileX");
    QTest::addColumn<int>("tileY");
    QTest::addColumn<int>("zoomLevel");
    QTest::addColumn<qreal>("expectedWesternTileEdgeLon");
    QTest::addColumn<qreal>("expectedNorthernTileEdgeLat");
    QTest::addColumn<qreal>("expectedEasternTileEdgeLon");
    QTest::addColumn<qreal>("expectedSouthernTileEdgeLat");

    // zoomlevel zero: 1 tile
    addRow() << 0 << 0 << 0 << qreal(-M_PI) << qreal(+M_PI * 0.5)
                            << qreal(+M_PI) << qreal(-M_PI * 0.5);

    // zoomlevel 1: 2 tiles per dimension
    addRow() << 0 << 0 << 1 << qreal(-M_PI) << qreal(+M_PI * 0.5)
                            << qreal(0)     << qreal(0);
    addRow() << 0 << 1 << 1 << qreal(-M_PI) << qreal(0)
                            << qreal(0)     << qreal(-M_PI * 0.5);
    addRow() << 1 << 0 << 1 << qreal(0)     << qreal(+M_PI * 0.5)
                            << qreal(+M_PI) << qreal(0);
    addRow() << 1 << 1 << 1 << qreal(0)     << qreal(0)
                            << qreal(+M_PI) << qreal(-M_PI * 0.5);

    // zoomlevel 9: 2^8==512 tiles per dimension
    addRow() <<   0 <<   0 << 9 << qreal(-M_PI)               << qreal(+M_PI * 0.5)
                                << qreal(-M_PI * (255/256.0)) << qreal(+M_PI * 0.5 * (255/256.0));
    addRow() <<   0 << 256 << 9 << qreal(-M_PI)               << qreal(0)
                                << qreal(-M_PI * (255/256.0)) << qreal(-M_PI * 0.5 * (1/256.0));
    addRow() << 256 <<   0 << 9 << qreal(0)                   << qreal(+M_PI * 0.5)
                                << qreal(M_PI * (1/256.0))    << qreal(+M_PI * 0.5 * (255/256.0));
    addRow() << 511 << 511 << 9 << qreal(M_PI * (255/256.0))  << qreal(-M_PI * 0.5 * (255/256.0))
                                << qreal(+M_PI)               << qreal(-M_PI * 0.5);
}


void TileProjectionTest::testGeoCoordinatesEquirect()
{
    QFETCH(int, tileX);
    QFETCH(int, tileY);
    QFETCH(int, zoomLevel);
    QFETCH(qreal, expectedWesternTileEdgeLon);
    QFETCH(qreal, expectedNorthernTileEdgeLat);
    QFETCH(qreal, expectedEasternTileEdgeLon);
    QFETCH(qreal, expectedSouthernTileEdgeLat);

    GeoSceneEquirectTileProjection projection;

    qreal westernTileEdgeLon;
    qreal northernTileEdgeLat;

    // method variant with zoomLevel, tileX, tileY
    projection.geoCoordinates(zoomLevel, tileX, tileY, westernTileEdgeLon, northernTileEdgeLat);

    QCOMPARE(westernTileEdgeLon, expectedWesternTileEdgeLon);
    QCOMPARE(northernTileEdgeLat, expectedNorthernTileEdgeLat);

    // method variants with GeoDataLatLonBox
    GeoDataLatLonBox latLonBox;

    projection.geoCoordinates(zoomLevel, tileX, tileY, latLonBox);

    QCOMPARE(latLonBox.west(), expectedWesternTileEdgeLon);
    QCOMPARE(latLonBox.north(), expectedNorthernTileEdgeLat);
    QCOMPARE(latLonBox.east(), expectedEasternTileEdgeLon);
    QCOMPARE(latLonBox.south(), expectedSouthernTileEdgeLat);

    TileId tileId(QStringLiteral("testmap"), zoomLevel, tileX, tileY);
    GeoDataLatLonBox latLonBox2;

    projection.geoCoordinates(tileId, latLonBox2);

    QCOMPARE(latLonBox2.west(), expectedWesternTileEdgeLon);
    QCOMPARE(latLonBox2.north(), expectedNorthernTileEdgeLat);
    QCOMPARE(latLonBox2.east(), expectedEasternTileEdgeLon);
    QCOMPARE(latLonBox2.south(), expectedSouthernTileEdgeLat);
}

void TileProjectionTest::testGeoCoordinatesMercator_data()
{
    QTest::addColumn<int>("tileX");
    QTest::addColumn<int>("tileY");
    QTest::addColumn<int>("zoomLevel");
    QTest::addColumn<qreal>("expectedWesternTileEdgeLon");
    QTest::addColumn<qreal>("expectedNorthernTileEdgeLat");
    QTest::addColumn<qreal>("expectedEasternTileEdgeLon");
    QTest::addColumn<qreal>("expectedSouthernTileEdgeLat");

    const qreal absMaxLat = DEG2RAD * 85.05113;

    // zoomlevel zero: 1 tile
    addRow() << 0 << 0 << 0 << qreal(-M_PI) << qreal(+absMaxLat)
                            << qreal(+M_PI) << qreal(-absMaxLat);

    // zoomlevel 1: 2 tiles per dimension
    addRow() << 0 << 0 << 1 << qreal(-M_PI) << qreal(+absMaxLat)
                            << qreal(0)     << qreal(0);
    addRow() << 0 << 1 << 1 << qreal(-M_PI) << qreal(0)
                            << qreal(0)     << qreal(-absMaxLat);
    addRow() << 1 << 0 << 1 << qreal(0)     << qreal(+absMaxLat)
                            << qreal(+M_PI) << qreal(0);
    addRow() << 1 << 1 << 1 << qreal(0)     << qreal(0)
                            << qreal(+M_PI) << qreal(-absMaxLat);

    // zoomlevel 9: 2^8==512 tiles per dimension
    addRow() <<   0 <<   0 << 9 << qreal(-M_PI)               << qreal(+absMaxLat)
                                << qreal(-M_PI * (255/256.0)) << qreal(+1.48336);
    addRow() <<   0 << 256 << 9 << qreal(-M_PI)               << qreal(0)
                                << qreal(-M_PI * (255/256.0)) << qreal(-0.0122715);
    addRow() << 256 <<   0 << 9 << qreal(0)                   << qreal(+absMaxLat)
                                << qreal(M_PI * (1/256.0))    << qreal(+1.48336);
    addRow() << 511 << 511 << 9 << qreal(M_PI * (255/256.0))  << qreal(-1.48336)
                                << qreal(+M_PI)               << qreal(-absMaxLat);
}


void TileProjectionTest::testGeoCoordinatesMercator()
{
    QFETCH(int, tileX);
    QFETCH(int, tileY);
    QFETCH(int, zoomLevel);
    QFETCH(qreal, expectedWesternTileEdgeLon);
    QFETCH(qreal, expectedNorthernTileEdgeLat);
    QFETCH(qreal, expectedEasternTileEdgeLon);
    QFETCH(qreal, expectedSouthernTileEdgeLat);

    GeoSceneMercatorTileProjection projection;

    qreal westernTileEdgeLon;
    qreal northernTileEdgeLat;

    // method variant with zoomLevel, tileX, tileY
    projection.geoCoordinates(zoomLevel, tileX, tileY, westernTileEdgeLon, northernTileEdgeLat);

    QCOMPARE(westernTileEdgeLon, expectedWesternTileEdgeLon);
    QFUZZYCOMPARE(northernTileEdgeLat, expectedNorthernTileEdgeLat, 0.00001);

    // method variants with GeoDataLatLonBox
    GeoDataLatLonBox latLonBox;

    projection.geoCoordinates(zoomLevel, tileX, tileY, latLonBox);

    QCOMPARE(latLonBox.west(), expectedWesternTileEdgeLon);
    QFUZZYCOMPARE(latLonBox.north(), expectedNorthernTileEdgeLat, 0.00001);
    QCOMPARE(latLonBox.east(), expectedEasternTileEdgeLon);
    QFUZZYCOMPARE(latLonBox.south(), expectedSouthernTileEdgeLat, 0.00001);

    TileId tileId(QStringLiteral("testmap"), zoomLevel, tileX, tileY);
    GeoDataLatLonBox latLonBox2;

    projection.geoCoordinates(tileId, latLonBox2);

    QCOMPARE(latLonBox2.west(), expectedWesternTileEdgeLon);
    QFUZZYCOMPARE(latLonBox2.north(), expectedNorthernTileEdgeLat, 0.00001);
    QCOMPARE(latLonBox2.east(), expectedEasternTileEdgeLon);
    QFUZZYCOMPARE(latLonBox2.south(), expectedSouthernTileEdgeLat, 0.00001);
}

} // namespace Marble

QTEST_MAIN(Marble::TileProjectionTest)

#include "TestTileProjection.moc"
