//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Thibaut Gridel <tgridel@free.fr>
// Copyright 2012,2013  Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "TestUtils.h"

#include "ViewportParams.h"

#include "AbstractProjection.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"

Q_DECLARE_METATYPE( Marble::GeoDataLinearRing )
Q_DECLARE_METATYPE( Marble::Projection )
Q_DECLARE_METATYPE( Marble::TessellationFlag )
Q_DECLARE_METATYPE( Marble::TessellationFlags )

namespace Marble
{

class ViewportParamsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void constructorDefaultValues();

    void constructorValues_data();
    void constructorValues();

    void screenCoordinates_GeoDataLineString_data();
    void screenCoordinates_GeoDataLineString();

    void screenCoordinates_GeoDataLineString2();

    void screenCoordinates_GeoDataLinearRing();

    void geoDataLinearRing_data();
    void geoDataLinearRing();

    void setInvalidRadius();

    void setFocusPoint();
};

void ViewportParamsTest::constructorDefaultValues()
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

void ViewportParamsTest::constructorValues_data()
{
    QTest::addColumn<Marble::Projection>( "projection" );
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );
    QTest::addColumn<int>( "radius" );
    QTest::addColumn<QSize>( "size" );

    ViewportParams viewport;

    viewport.setProjection( Spherical );
    const AbstractProjection *const spherical = viewport.currentProjection();

    viewport.setProjection( Mercator);
    const AbstractProjection *const mercator = viewport.currentProjection();

    viewport.setProjection( Equirectangular );
    const AbstractProjection *const equirectangular = viewport.currentProjection();

    addRow() << Spherical << qreal(0) << qreal(0) << 2000 << QSize( 100, 100 );
    addRow() << Mercator << qreal(0) << qreal(0) << 2000 << QSize( 100, 100 );
    addRow() << Equirectangular << qreal(0) << qreal(0) << 2000 << QSize( 100, 100 );

    addRow() << Spherical << qreal(205 * DEG2RAD) << spherical->maxValidLat() + qreal(1.0) << 2000 << QSize( 100, 100 );
    addRow() << Mercator << qreal(205 * DEG2RAD) << mercator->maxValidLat() + qreal(1.0) << 2000 << QSize( 100, 100 );
    addRow() << Equirectangular << qreal(205 * DEG2RAD) << equirectangular->maxValidLat() + qreal(1.0) << 2000 << QSize( 100, 100 );
}

void ViewportParamsTest::constructorValues()
{
    QFETCH( Projection, projection );
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );
    QFETCH( int, radius );
    QFETCH( QSize, size );

    const ViewportParams byConstructor( projection, lon, lat, radius, size );

    ViewportParams bySetters;
    bySetters.setProjection( projection );
    bySetters.centerOn( lon, lat );
    bySetters.setRadius( radius );
    bySetters.setSize( size );

    QCOMPARE( byConstructor.projection(), bySetters.projection() );
    QCOMPARE( byConstructor.currentProjection(), bySetters.currentProjection() );
    QCOMPARE( byConstructor.centerLongitude(), bySetters.centerLongitude() );
    QCOMPARE( byConstructor.centerLatitude(), bySetters.centerLatitude() );
    QCOMPARE( byConstructor.planetAxis(), bySetters.planetAxis() );
    QCOMPARE( byConstructor.angularResolution(), bySetters.angularResolution() );
    QCOMPARE( byConstructor.radius(), bySetters.radius() );
    QCOMPARE( byConstructor.size(), bySetters.size() );
}

void ViewportParamsTest::screenCoordinates_GeoDataLineString_data()
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
            << projection << flags << diagonalLine << 2;

    QTest::newRow("Mercator Tesselate Latitude IDL")
            << projection << flags << latitudeLine << 2;

    flags = Tessellate | RespectLatitudeCircle;
    QTest::newRow("Mercator LatitudeCircle Longitude")
            << projection << flags << longitudeLine << 2;

    QTest::newRow("Mercator LatitudeCircle Diagonal IDL")
            << projection << flags << diagonalLine << 2;

    QTest::newRow("Mercator LatitudeCircle Latitude IDL")
            << projection << flags << latitudeLine << 2;

    projection = Equirectangular;

    flags = NoTessellation;
    QTest::newRow("Equirect NoTesselation Longitude")
            << projection << flags << longitudeLine << 2;

    QTest::newRow("Equirect NoTesselation Diagonal IDL")
            << projection << flags << diagonalLine << 2;

    QTest::newRow("Equirect NoTesselation Latitude IDL")
            << projection << flags << latitudeLine << 2;

    flags = Tessellate;
    QTest::newRow("Equirect Tesselate Longitude")
            << projection << flags << longitudeLine << 2;

    QTest::newRow("Equirect Tesselate Diagonal IDL")
            << projection << flags << diagonalLine << 2;

    QTest::newRow("Equirect Tesselate Latitude IDL")
            << projection << flags << latitudeLine << 2;

    flags = Tessellate | RespectLatitudeCircle;
    QTest::newRow("Equirect LatitudeCircle Longitude")
            << projection << flags << longitudeLine << 2;

    QTest::newRow("Equirect LatitudeCircle Diagonal IDL")
            << projection << flags << diagonalLine << 2;

    QTest::newRow("Equirect LatitudeCircle Latitude IDL")
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

void ViewportParamsTest::screenCoordinates_GeoDataLineString()
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

void ViewportParamsTest::screenCoordinates_GeoDataLineString2()
{
    const ViewportParams viewport( Spherical, 90 * DEG2RAD, 38 * DEG2RAD, 256, QSize( 1165, 833 ) );

    const GeoDataCoordinates coordinates( -90, 23.44, 0.0, GeoDataCoordinates::Degree );
    qreal x, y;
    bool globeHidesPoint;
    viewport.screenCoordinates( coordinates, x, y, globeHidesPoint );

    QCOMPARE( globeHidesPoint, true );

    GeoDataLineString line( Tessellate | RespectLatitudeCircle );
    line << GeoDataCoordinates( -180, 23.4400, 0.0, GeoDataCoordinates::Degree );
    line << GeoDataCoordinates( 0, 23.4400, 0.0, GeoDataCoordinates::Degree );

    QVector<QPolygonF*> polys;
    viewport.screenCoordinates( line, polys );

    QCOMPARE( polys.size(), 2 );
}

void ViewportParamsTest::screenCoordinates_GeoDataLinearRing()
{
    // Creates a Rectangle on the eastern southern hemisphere
    // with the planet rotated so that only the western half
    // of the rectangle is visible. As a result only a single
    // screen polygon should be rendered.

    const ViewportParams viewport( Spherical, -15 * DEG2RAD, 0 * DEG2RAD, 350, QSize( 1000, 750 ) );

    GeoDataLinearRing line( Tessellate );
    GeoDataCoordinates coord1 ( 30, -10, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates coord2 ( 30, -45, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates coord3 ( 100, -45, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates coord4 ( 100, -10, 0.0, GeoDataCoordinates::Degree );

    qreal x, y;
    bool globeHidesPoint;
    viewport.screenCoordinates( coord1, x, y, globeHidesPoint );
    QCOMPARE( globeHidesPoint, false );
    viewport.screenCoordinates( coord2, x, y, globeHidesPoint );
    QCOMPARE( globeHidesPoint, false );
    viewport.screenCoordinates( coord3, x, y, globeHidesPoint );
    QCOMPARE( globeHidesPoint, true );
    viewport.screenCoordinates( coord4, x, y, globeHidesPoint );
    QCOMPARE( globeHidesPoint, true );

    line << coord1 << coord2 << coord3 << coord4;

    QVector<QPolygonF*> polys;
    viewport.screenCoordinates( line, polys );

    QCOMPARE( polys.size(), 1 );
}

void ViewportParamsTest::geoDataLinearRing_data()
{
    QTest::addColumn<Marble::Projection>( "projection" );
    QTest::addColumn<Marble::TessellationFlags>( "tessellation" );
    QTest::addColumn<GeoDataLinearRing>( "ring" );
    QTest::addColumn<int>( "size" );

    GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;

    GeoDataLinearRing normalRing;
    normalRing << GeoDataCoordinates(175, 5, 0, deg )
               << GeoDataCoordinates(175, 15, 0, deg )
               << GeoDataCoordinates(170, 15, 0, deg );

    GeoDataLinearRing acrossIDLRing;
    acrossIDLRing << GeoDataCoordinates(-175, 5, 0, deg )
                  << GeoDataCoordinates(175, 5, 0, deg )
                  << GeoDataCoordinates(175, 15, 0, deg );

    GeoDataLinearRing aroundSPoleRing;
    aroundSPoleRing << GeoDataCoordinates(-175, -65, 0, deg )
                 << GeoDataCoordinates(-55, -70, 0, deg )
                    << GeoDataCoordinates(65, -75, 0, deg );

    Projection projection = Mercator;

    TessellationFlags flags = NoTessellation;
    QTest::newRow("Mercator NoTesselation normalRing")
            << projection << flags << normalRing << 2;

    QTest::newRow("Mercator NoTesselation acrossIDLRing")
            << projection << flags << acrossIDLRing << 2;

#ifdef BUG_357540_IS_FIXED
    QTest::newRow("Mercator NoTesselation aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 2;
#endif

    flags = Tessellate;
    QTest::newRow("Mercator Tesselate normalRing")
            << projection << flags << normalRing << 2;

    QTest::newRow("Mercator Tesselate acrossIDLRing")
            << projection << flags << acrossIDLRing << 2;

#ifdef BUG_357540_IS_FIXED
    QTest::newRow("Mercator Tesselate aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 2;
#endif

    flags = Tessellate | RespectLatitudeCircle;
    QTest::newRow("Mercator LatitudeCircle normalRing")
            << projection << flags << normalRing << 2;

    QTest::newRow("Mercator LatitudeCircle acrossIDLRing")
            << projection << flags << acrossIDLRing << 2;

#ifdef BUG_357540_IS_FIXED
    QTest::newRow("Mercator LatitudeCircle aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 2;
#endif

    projection = Equirectangular;

    flags = NoTessellation;
    QTest::newRow("Equirect NoTesselation normalRing")
            << projection << flags << normalRing << 2;

    QTest::newRow("Equirect NoTesselation acrossIDLRing")
            << projection << flags << acrossIDLRing << 2;

#ifdef BUG_357540_IS_FIXED
    QTest::newRow("Equirect NoTesselation aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 2;
#endif

    flags = Tessellate;
    QTest::newRow("Equirect Tesselate normalRing")
            << projection << flags << normalRing << 2;

    QTest::newRow("Equirect Tesselate acrossIDLRing")
            << projection << flags << acrossIDLRing << 2;

#ifdef BUG_357540_IS_FIXED
    QTest::newRow("Equirect Tesselate aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 2;
#endif

    flags = Tessellate | RespectLatitudeCircle;
    QTest::newRow("Equirect LatitudeCircle normalRing")
            << projection << flags << normalRing << 2;

    QTest::newRow("Equirect LatitudeCircle acrossIDLRing")
            << projection << flags << acrossIDLRing << 2;

#ifdef BUG_357540_IS_FIXED
    QTest::newRow("Equirect LatitudeCircle aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 2;
#endif

    projection = Spherical;

    flags = NoTessellation;
    QTest::newRow("Spherical NoTesselation normalRing")
            << projection << flags << normalRing << 1;

    QTest::newRow("Spherical NoTesselation acrossIDLRing")
            << projection << flags << acrossIDLRing << 1;

    QTest::newRow("Spherical NoTesselation aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 1;

    flags = Tessellate;
    QTest::newRow("Spherical Tesselate normalRing")
            << projection << flags << normalRing << 1;

    QTest::newRow("Spherical Tesselate acrossIDLRing")
            << projection << flags << acrossIDLRing << 1;

/*    QTest::newRow("Spherical Tesselate aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 1;*/

    flags = Tessellate | RespectLatitudeCircle;
    QTest::newRow("Spherical LatitudeCircle normalRing")
            << projection << flags << normalRing << 1;

    QTest::newRow("Spherical LatitudeCircle acrossIDLRing")
            << projection << flags << acrossIDLRing << 1;

/*    QTest::newRow("Spherical LatitudeCircle aroundSPoleRing")
            << projection << flags << aroundSPoleRing << 1;*/

}

void ViewportParamsTest::geoDataLinearRing()
{
    QFETCH( Marble::Projection, projection );
    QFETCH( Marble::TessellationFlags, tessellation );
    QFETCH( GeoDataLinearRing, ring );
    QFETCH( int, size );

    ViewportParams viewport;
    viewport.setProjection( projection );
    viewport.setRadius( 360 / 4 ); // for easy mapping of lon <-> x
    viewport.centerOn(175 * DEG2RAD, 0);

    ring.setTessellationFlags( tessellation );
    QVector<QPolygonF*> polys;
    viewport.screenCoordinates(ring, polys);

    foreach (QPolygonF* poly, polys) {
        // at least 3 points in one poly
        QVERIFY( poly->size() > 2 );
        QPointF oldCoord = poly->first();
        // polygon comes back to same point
        QVERIFY( poly->isClosed() );
        poly->pop_front();

        foreach(const QPointF &coord, *poly) {
            // no 2 same points
            QVERIFY( (coord-oldCoord) != QPointF() );

            // no 2 consecutive points should be more than 90° apart
//            QVERIFY( (coord-oldCoord).manhattanLength() < viewport.radius() );
            oldCoord = coord;
        }
    }

    // check the provided number of polys
    QCOMPARE( polys.size(), size );
}

void ViewportParamsTest::setInvalidRadius()
{
    ViewportParams viewport;

    // QVERIFY( viewport.radius() > 0 ); already verified above

    const int radius = viewport.radius();
    viewport.setRadius( 0 );

    QCOMPARE( viewport.radius(), radius );
}

void ViewportParamsTest::setFocusPoint()
{
    const GeoDataCoordinates focusPoint1( 10, 13, 0, GeoDataCoordinates::Degree );
    const GeoDataCoordinates focusPoint2( 14.3, 20.5, 0, GeoDataCoordinates::Degree );

    ViewportParams viewport;

    const GeoDataCoordinates center = viewport.focusPoint();

    QVERIFY( center != focusPoint1 );
    QVERIFY( center != focusPoint2 );

    viewport.setFocusPoint( focusPoint1 );
    QCOMPARE( viewport.focusPoint(), focusPoint1 );

    viewport.resetFocusPoint();
    QCOMPARE( viewport.focusPoint(), center );

    viewport.setFocusPoint( focusPoint2 );
    QCOMPARE( viewport.focusPoint(), focusPoint2 );

    viewport.setFocusPoint( focusPoint1 );
    QCOMPARE( viewport.focusPoint(), focusPoint1 );

    viewport.resetFocusPoint();
    QCOMPARE( viewport.focusPoint(), center );
}

}

QTEST_MAIN( Marble::ViewportParamsTest )

#include "ViewportParamsTest.moc"
