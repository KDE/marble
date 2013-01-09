//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Thibaut Gridel <tgridel@free.fr>
//

#include <QtTest/QtTest>
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleMap.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"

namespace Marble
{

class ProjectionTest : public QObject
{
    Q_OBJECT

 private slots:
    void constructorDefaultValues();

    void drawLineString_data();
    void drawLineString();

    void setInvalidRadius();
};

void ProjectionTest::constructorDefaultValues()
{
    const ViewportParams viewport;

    QCOMPARE( viewport.projection(), Spherical );
    QCOMPARE( viewport.size(), QSize( 100, 100 ) );
    QCOMPARE( viewport.width(), 100 );
    QCOMPARE( viewport.height(), 100 );
    QCOMPARE( viewport.centerLongitude(), 0. );
    QCOMPARE( viewport.centerLatitude(), 0. );
    QCOMPARE( viewport.polarity(), 1 );
    QCOMPARE( viewport.radius(), 2000 );
    QCOMPARE( viewport.mapCoversViewport(), true );
    QCOMPARE( viewport.focusPoint(), GeoDataCoordinates( 0., 0., 0. ) );

    // invariants:
    QVERIFY( viewport.radius() > 0 ); // avoids divisions by zero
    QVERIFY( viewport.viewLatLonAltBox() == viewport.latLonAltBox( QRect( 0, 0, 100, 100 ) ) );
    // FIXME QCOMPARE( viewport.viewLatLonAltBox().center().longitude(), viewport.centerLongitude() );
    // FIXME QCOMPARE( viewport.viewLatLonAltBox().center().latitude(), viewport.centerLatitude() );
}

void ProjectionTest::drawLineString_data()
{
    QTest::addColumn<Marble::Projection>( "projection" );
    QTest::addColumn<Marble::TessellationFlags>( "tessellation" );
    QTest::addColumn<GeoDataLineString>( "line" );
    QTest::addColumn<int>( "size" );

    GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;

    GeoDataLineString longitudeLine;
    longitudeLine << GeoDataCoordinates(185, 5, 0, deg )
                  << GeoDataCoordinates(185, 15, 0, deg );

    GeoDataLineString diagonalLine;
    diagonalLine << GeoDataCoordinates(-185, 5, 0, deg )
                 << GeoDataCoordinates(185, 15, 0, deg );

    GeoDataLineString latitudeLine;
    latitudeLine << GeoDataCoordinates(-185, 5, 0, deg )
                 << GeoDataCoordinates(185, 5, 0, deg );

    Projection projection = Mercator;

    TessellationFlags flags = NoTessellation;
    QTest::newRow("Mercator NoTesselation Longitude")
            << projection << flags << longitudeLine << 2;

    QTest::newRow("Mercator NoTesselation Diagonal IDL")
            << projection << flags << diagonalLine << 2;

    QTest::newRow("Mercator NoTesselation Latitude IDL")
            << projection << flags << latitudeLine << 2;

    flags = Tessellate;
    QTest::newRow("Mercator Tesselate Longitude")
            << projection << flags << longitudeLine << 2;

    QTest::newRow("Mercator Tesselate Diagonal IDL")
            << projection << flags << diagonalLine << 4;

    QTest::newRow("Mercator Tesselate Latitude IDL")
            << projection << flags << latitudeLine << 4;

    flags = Tessellate | RespectLatitudeCircle;
    QTest::newRow("Mercator LatitudeCircle Longitude")
            << projection << flags << longitudeLine << 2;

    QTest::newRow("Mercator LatitudeCircle Diagonal IDL")
            << projection << flags << diagonalLine << 4;

    QTest::newRow("Mercator LatitudeCircle Latitude IDL")
            << projection << flags << latitudeLine << 2;


    projection = Spherical;

    flags = NoTessellation;
    QTest::newRow("Spherical NoTesselation Longitude")
            << projection << flags << longitudeLine << 1;

    QTest::newRow("Spherical NoTesselation Diagonal IDL")
            << projection << flags << diagonalLine << 1;

    QTest::newRow("Spherical NoTesselation Latitude IDL")
            << projection << flags << latitudeLine << 1;

    flags = Tessellate;
    QTest::newRow("Spherical Tesselate Longitude")
            << projection << flags << longitudeLine << 1;

    QTest::newRow("Spherical Tesselate Diagonal IDL")
            << projection << flags << diagonalLine << 1;

    QTest::newRow("Spherical Tesselate Latitude IDL")
            << projection << flags << latitudeLine << 1;

    flags = Tessellate | RespectLatitudeCircle;
    QTest::newRow("Spherical LatitudeCircle Longitude")
            << projection << flags << longitudeLine << 1;

    QTest::newRow("Spherical LatitudeCircle Diagonal IDL")
            << projection << flags << diagonalLine << 1;

    QTest::newRow("Spherical LatitudeCircle Latitude IDL")
            << projection << flags << latitudeLine << 1;

}

void ProjectionTest::drawLineString()
{
    QFETCH( Marble::Projection, projection );
    QFETCH( Marble::TessellationFlags, tessellation );
    QFETCH( GeoDataLineString, line );
    QFETCH( int, size );

    ViewportParams viewport;
    viewport.setProjection( projection );
    viewport.setRadius( 360 / 4 ); // for easy mapping of lon <-> x
    viewport.centerOn(185 * DEG2RAD, 0);

    line.setTessellationFlags( tessellation );
    QVector<QPolygonF*> polys;
    viewport.screenCoordinates(line, polys);

    foreach (QPolygonF* poly, polys) {
        // at least 2 points in one poly
        QVERIFY( poly->size() > 1 );
        QPointF oldCoord = poly->first();
        poly->pop_front();

        foreach(const QPointF &coord, *poly) {
            // no 2 same points
            QVERIFY( (coord-oldCoord) != QPointF() );

            // no 2 consecutive points should be more than 90° apart
            QVERIFY( (coord-oldCoord).manhattanLength() < viewport.radius() );
            oldCoord = coord;
        }
    }

    // check the provided number of polys
    QCOMPARE( polys.size(), size );
}

void ProjectionTest::setInvalidRadius()
{
    ViewportParams viewport;

    // QVERIFY( viewport.radius() > 0 ); already verified above

    const int radius = viewport.radius();
    viewport.setRadius( 0 );

    QCOMPARE( viewport.radius(), radius );
}

}

Q_DECLARE_METATYPE( Marble::Projection )
Q_DECLARE_METATYPE( Marble::TessellationFlag )
Q_DECLARE_METATYPE( Marble::TessellationFlags )
QTEST_MAIN( Marble::ProjectionTest )

#include "ProjectionTest.moc"
