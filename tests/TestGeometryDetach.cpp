//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014   Calin Cruceru <calin@rosedu.org>
//

#include <QObject>

#include "GeoDataMultiTrack.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataTrack.h"
#include "GeoDataMultiGeometry.h"
#include "TestUtils.h"


namespace Marble
{

class TestGeometryDetach : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    /**
     * @brief testMultiGeometry shows that modifying a child at a given position
     * in a copied multi geometry doesn't modify the child at the same position
     * in the original one.
     */
    void testMultiGeometry();

    /**
     * @see above.
     */
    void testMultiTrack();

    /**
     * @brief testPoint shows that modifying the coordinates (using setCoordinate()
     * method) of a copied point doesn't modify the coordinates of the original one.
     */
    void testPoint();

    /**
     * @brief testPolygon shows that modifying the outerBoundary() of a copied
     * polygon doesn't modify the outer boundary of the original one. The same
     * applies for innerBoundaries().
     */
    void testPolygon();

private:
    GeoDataCoordinates m_coords1;
    GeoDataCoordinates m_coords2;
};

void TestGeometryDetach::initTestCase()
{
    m_coords1 = GeoDataCoordinates(30, 30, 0, GeoDataCoordinates::Degree);
    m_coords2 = GeoDataCoordinates(60, 60, 0, GeoDataCoordinates::Degree);
}

void TestGeometryDetach::testMultiGeometry()
{
    GeoDataPoint *point = new GeoDataPoint;
    point->setCoordinates(m_coords1);
    GeoDataMultiGeometry multiGeom1;
    multiGeom1.append(point);

    GeoDataMultiGeometry multiGeom2 = multiGeom1;
    static_cast<GeoDataPoint*>(multiGeom2.child(0))->setCoordinates(m_coords2);
    QVERIFY(static_cast<GeoDataPoint*>(multiGeom1.child(0))->coordinates() == m_coords1);

    const GeoDataMultiGeometry multiGeom3 = multiGeom1;
    QVERIFY(static_cast<const GeoDataPoint*>(multiGeom3.child(0))->coordinates() == m_coords1);
}

void TestGeometryDetach::testMultiTrack()
{
    GeoDataTrack *track = new GeoDataTrack();
    track->setAltitudeMode(Absolute);
    GeoDataMultiTrack multiTrack1;
    multiTrack1.append(track);

    GeoDataMultiTrack multiTrack2 = multiTrack1;
    multiTrack2.child(0)->setAltitudeMode(RelativeToSeaFloor);
    QVERIFY(multiTrack1.child(0)->altitudeMode() == Absolute);

    const GeoDataMultiTrack multiTrack3 = multiTrack1;
    QVERIFY(multiTrack3.child(0)->altitudeMode() == Absolute);
}

void TestGeometryDetach::testPoint()
{
    GeoDataPoint point1;
    point1.setCoordinates(m_coords1);

    GeoDataPoint point2 = point1;
    point2.setCoordinates(m_coords2);
    QVERIFY(point1.coordinates() == m_coords1);

    const GeoDataPoint point3 = point1;
    QVERIFY(point3.coordinates() == m_coords1);
}

void TestGeometryDetach::testPolygon()
{
    GeoDataPolygon poly1;
    poly1.outerBoundary().append(m_coords1);
    poly1.appendInnerBoundary(GeoDataLinearRing());

    GeoDataPolygon poly2 = poly1;
    poly2.outerBoundary().append(m_coords2);
    poly2.innerBoundaries().clear();

    QVERIFY(poly2.outerBoundary().size() == 2);
    QVERIFY(poly2.innerBoundaries().size() == 0);
    QVERIFY(poly1.outerBoundary().size() == 1);
    QVERIFY(poly1.innerBoundaries().size() == 1);

    const GeoDataPolygon poly3 = poly1;
    QVERIFY(poly3.outerBoundary().size() == 1);
    QVERIFY(poly3.innerBoundaries().size() == 1);
}

}

QTEST_MAIN( Marble::TestGeometryDetach )

#include "TestGeometryDetach.moc"
