//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#include <QObject>

#include "GeoDataCoordinates.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLatLonAltBox.h"

namespace Marble
{

class GeoDataLatLonAltBoxTest : public QObject
{
    Q_OBJECT

 private Q_SLOTS:
    void fromLineStringTest();

 private:
    qreal randomLon();
    qreal randomLat();
};


void GeoDataLatLonAltBoxTest::fromLineStringTest()
{
    qreal west = randomLon();
    qreal east = randomLon();
    qreal lat1 = randomLat();
    qreal lat2 = randomLat();
    GeoDataLatLonAltBox sourceBox;

    if ( lat1 >= lat2 ) {
        sourceBox = GeoDataLatLonBox( lat1, lat2, east, west, GeoDataCoordinates::Degree );
    }
    else {
        sourceBox = GeoDataLatLonBox( lat2, lat1, east, west, GeoDataCoordinates::Degree );
    }

    GeoDataLinearRing ring;
    // SouthWest
    ring << GeoDataCoordinates( sourceBox.west(), sourceBox.south() );
    // SouthEast
    ring << GeoDataCoordinates( sourceBox.east(), sourceBox.south() );
    // NorthEast
    ring << GeoDataCoordinates( sourceBox.east(), sourceBox.north() );
    // NorthWest
    ring << GeoDataCoordinates( sourceBox.west(), sourceBox.north() );

    QCOMPARE( GeoDataLatLonAltBox::fromLineString( ring ).toString(), sourceBox.toString() );

}

qreal GeoDataLatLonAltBoxTest::randomLon()
{
    qreal lon = qrand();
    while ( lon < -180 ) {
        lon += 360;
    }
    while ( lon > 180 ) {
        lon -= 360;
    }

    return lon;
}

qreal GeoDataLatLonAltBoxTest::randomLat()
{
    qreal lat = qrand();
    while ( lat < -90 ) {
        lat += 180;
    }
    while ( lat > 90 ) {
        lat -= 180;
    }

    return lat;
}

} // namespace Marble

QTEST_MAIN( Marble::GeoDataLatLonAltBoxTest )

#include "GeoDataLatLonAltBoxTest.moc"
