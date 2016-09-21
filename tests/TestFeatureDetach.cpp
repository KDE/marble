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

#include "GeoDataContainer.h"
#include "GeoDataPoint.h"
#include "GeoDataPlacemark.h"
#include "GeoDataCamera.h"
#include "MarbleGlobal.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTour.h"
#include "TestUtils.h"


namespace Marble
{

class TestFeatureDetach : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    /**
     * FIXME: Doesn't work for the moment because calling detach() in
     * GeoDataFeature::set/abstractView() doesn't help because the object
     * isn't deep-copied in the private class.
     *
     * @brief testFeature shows that getting the abstractView() of a copied
     * feature and modifying it doesn't modify the original one.
     */
    void testFeature();

    /**
     * @brief testContainer shows that getting some child and modifying it,
     * doesn't modify the child at the same position in the original container.
     */
    void testContainer();

    /**
     * @brief testPlacemark shows that getting the geometry() and modifying it
     * doesn't modify the geometry of the original placemark.
     */
    void testPlacemark();

    /**
     * @brief testTour shows that modifying the playlist of a copied tour doesn't
     * modify the playlist of the original one.
     */
    void testTour();

    /**
     * @brief testGeometryParentInPlacemark shows that copying a placemark correctly
     * keeps the geometries of both the copied one and the original one pointing to its
     * parent. Before the changes made in GeoDataPlacemark (calling setParent() after
     * each detach() call and not calling anymore in the
     * GeoDataPlacemark( const GeoDataGeometry &other ) constructor), after the operation
     * highlighted in this test, the geometry of the first one (the original one) ended
     * pointing (its parent) to the second placemark and the second one had its geometry's
     * parent a null pointer.
     */
    void testGeometryParentInPlacemark();

};

void TestFeatureDetach::testFeature()
{
    GeoDataFeature feat1;
    GeoDataCamera *view1 = new GeoDataCamera();
    view1->setAltitudeMode(Absolute);
    feat1.setAbstractView(view1);

    GeoDataFeature feat2 = feat1;
    feat2.abstractView()->setAltitudeMode(ClampToSeaFloor);
    // FIXME: See above (method description).
    // QVERIFY(feat1.abstractView()->altitudeMode() == Absolute);
}

void TestFeatureDetach::testContainer()
{
    GeoDataContainer cont1;
    GeoDataFeature *feat1 = new GeoDataFeature();
    feat1->setName("Feat1");
    cont1.insert(0, feat1);

    GeoDataContainer cont2 = cont1;
    cont2.child(0)->setName("Feat2");
    QCOMPARE(cont1.child(0)->name(), QLatin1String("Feat1"));

    const GeoDataContainer cont3 = cont1;
    QCOMPARE(cont3.child(0)->name(), QLatin1String("Feat1"));
}

void TestFeatureDetach::testPlacemark()
{
    GeoDataCoordinates coords1(30, 30, 0, GeoDataCoordinates::Degree);
    GeoDataPlacemark place1;
    place1.setCoordinate(coords1);

    GeoDataPlacemark place2 = place1;

    GeoDataCoordinates coords2(60, 60, 0, GeoDataCoordinates::Degree);
    GeoDataPoint *point = static_cast<GeoDataPoint*>( place2.geometry() );
    point->setCoordinates(coords2);
    QVERIFY(place1.coordinate() == coords1);

    const GeoDataPlacemark place3 = place1;
    QVERIFY(place3.coordinate() == coords1);
}

void TestFeatureDetach::testTour()
{
    GeoDataPlaylist *newPlaylist = new GeoDataPlaylist;
    newPlaylist->setId("Playlist1");
    GeoDataTour tour1;
    tour1.setPlaylist(newPlaylist);

    GeoDataTour tour2 = tour1;
    tour2.playlist()->setId("Playlist2");
    QCOMPARE(tour1.playlist()->id(), QLatin1String("Playlist1"));

    const GeoDataTour tour3 = tour1;
    QCOMPARE(tour3.playlist()->id(), QLatin1String("Playlist1"));
}

void TestFeatureDetach::testGeometryParentInPlacemark()
{
    GeoDataPlacemark place1;
    QVERIFY(place1.geometry()->parent() == &place1);

    GeoDataPlacemark place2 = place1;

    // With the changes (regarding setParent() multiple calls after each
    // detach() call) the only moment when some invariant is broken is right now,
    // after the copy constructor has been called and no other method (which calls
    // detach()) hasn't. This is because the geometry is not immediately copied,
    // so the geometry of place2 has as parent place1. This is immediately solved
    // when calling geometry() below.
    QVERIFY(place2.geometry()->parent() == &place2);
    QVERIFY(place1.geometry()->parent() == &place1);
}

}

QTEST_MAIN( Marble::TestFeatureDetach )

#include "TestFeatureDetach.moc"
