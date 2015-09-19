//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Patrick Spendrin  <ps_ml@gmx.de>
//

#include "MarbleDebug.h"
#include "MarbleDirs.h"

// Feature:
#include "GeoDataFolder.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"

// Geometry:
#include "GeoDataPolygon.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"

// StyleSelector:
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyleMap.h"

// misc:
#include "GeoDataHotSpot.h"
#include "GeoDataLatLonBox.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataCoordinates.h"

#include <QCoreApplication>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QTest>

namespace Marble
{

class TestGeoDataCopy : public QObject
{
    Q_OBJECT
    private:
        void testCoordinate( GeoDataCoordinates coord, qreal alt, int detail, QString coordtest );
    private slots:
        void initTestCase();
        // misc.:
        void copyCoordinates();
        void copyHotSpot();
        void copyLatLonBox();

        // GeoDataGeometry:
        void copyLineString();
        void copyLinearRing();
        void copyPoint();
        void copyPolygon();
        void copyMultiGeometry();

        // GeoDataFeature:
        void copyDocument();
        void copyFolder();
        void copyPlacemark();

        // StyleSelector:
        void copyStyle();
        void copyIconStyle();
        void copyLabelStyle();
        void copyLineStyle();
        void copyPolyStyle();
        void copyStyleMap();
    private:
        QStringList coordString;
        GeoDataCoordinates coord1;
        GeoDataCoordinates coord2;
        GeoDataCoordinates coord3;
        GeoDataPoint point1;
        GeoDataPoint point2;
        GeoDataPoint point3;
};

void TestGeoDataCopy::testCoordinate( GeoDataCoordinates coord, qreal alt, int detail, QString coordtest )
{
    QCOMPARE(coord.altitude(), alt);
    QCOMPARE(coord.detail(), detail);
    QCOMPARE(coord.toString(), coordtest);
}

void TestGeoDataCopy::initTestCase()
{
    QLocale::setDefault(QLocale::German);
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    coordString << QString::fromUtf8(" 13\u00B0 42' 38,5\"E,  51\u00B0 01' 24,6\"N" );
    coordString << QString::fromUtf8(" 14\u00B0 42' 38,5\"E,  52\u00B0 01' 24,6\"N" );
    coordString << QString::fromUtf8(" 15\u00B0 42' 38,5\"E,  53\u00B0 01' 24,6\"N" );

    coord1.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    coord1.setDetail(2);
    testCoordinate(coord1, 123.4, 2, coordString[0]);

    coord2 = GeoDataCoordinates(14.7107, 52.0235, 133.4, GeoDataCoordinates::Degree, 3);
    testCoordinate(coord2, 133.4, 3, coordString[1]);

    coord3.set(15.7107,53.0235, 143.4, GeoDataCoordinates::Degree);
    coord3.setDetail(4);
    testCoordinate(coord3, 143.4, 4, coordString[2]);
    
    point1.setCoordinates( GeoDataCoordinates(13.7107, 51.0235, 123.4, GeoDataCoordinates::Degree, 2) );
    testCoordinate(point1.coordinates(), 123.4, 2, coordString[0]);
    
    point2.setCoordinates( GeoDataCoordinates(14.7107, 52.0235, 133.4, GeoDataCoordinates::Degree, 3) );
    testCoordinate(point2.coordinates(), 133.4, 3, coordString[1]);

    point3.setCoordinates( GeoDataCoordinates(15.7107, 53.0235, 143.4, GeoDataCoordinates::Degree, 4) );
    testCoordinate(point3.coordinates(), 143.4, 4, coordString[2]);
}

void TestGeoDataCopy::copyCoordinates()
{
    GeoDataCoordinates other = coord1;
    
    // make sure that the coordinate contains the right values
    testCoordinate(other, 123.4, 2, coordString[0]);
    
    QVERIFY(coord1 == other);
}

void TestGeoDataCopy::copyPoint()
{
    GeoDataPoint point;

    point.setCoordinates( GeoDataCoordinates(13.7107, 51.0235, 123.4, GeoDataCoordinates::Degree, 2) );
    point.setExtrude( true );

    // make sure that the coordinate contains the right values
    testCoordinate(point.coordinates(), 123.4, 2, coordString[0]);
    QCOMPARE(point.extrude(), true);

    GeoDataPoint other = point;
    
    // make sure that the coordinate contains the right values
    testCoordinate(other.coordinates(), 123.4, 2, coordString[0]);
    QCOMPARE(other.extrude(), true);
    
    QVERIFY(point.coordinates() == other.coordinates());
    
    point = GeoDataPoint( GeoDataCoordinates(13.7107, 51.0235, 123.4, GeoDataCoordinates::Degree, 17) );
    point.setExtrude(false);
    QCOMPARE(other.coordinates().detail(), 2);
    QCOMPARE(point.coordinates().detail(), 17);
    QCOMPARE(other.extrude(), true);
    QCOMPARE(point.extrude(), false);
}

void TestGeoDataCopy::copyLineString()
{
    GeoDataLineString lineString;
    lineString.setTessellate(true);
    

    lineString.append(coord1);
    lineString.append(coord2);
    lineString.append(coord3);
    QVERIFY(lineString.size() == 3);

    GeoDataLineString other = lineString;
    QVERIFY(other.size() == 3);

    testCoordinate(lineString.at(0), 123.4, 2, coordString[0]);
    testCoordinate(other.at(2), 143.4, 4, coordString[2]);

    QVERIFY(other.at(2) == coord3);
    QVERIFY(other.tessellate());
}

void TestGeoDataCopy::copyLinearRing()
{
    GeoDataLinearRing linearRing;

    linearRing.setTessellate(true);


    linearRing.append(coord1);
    linearRing.append(coord2);
    linearRing.append(coord3);
    QVERIFY(linearRing.size() == 3);

    GeoDataLinearRing other = linearRing;
    QVERIFY(other.size() == 3);

    testCoordinate(linearRing.at(0), 123.4, 2, coordString[0]);
    testCoordinate(other.at(2), 143.4, 4, coordString[2]);

    QVERIFY(other.at(2) == coord3);
    QVERIFY(other.tessellate());
}

void TestGeoDataCopy::copyPolygon()
{
    GeoDataLinearRing linearRing1;
    GeoDataLinearRing linearRing2;
    GeoDataLinearRing linearRing3;
    GeoDataLinearRing linearRing4;
    
    linearRing1.append(coord1); linearRing1.append(coord2); linearRing1.append(coord3);
    linearRing2.append(coord3); linearRing2.append(coord2); linearRing2.append(coord1);
    linearRing3.append(coord1); linearRing3.append(coord2); linearRing3.append(coord3);
    linearRing3.append(coord3); linearRing3.append(coord2); linearRing3.append(coord1);
    linearRing4.append(coord3); linearRing4.append(coord2); linearRing4.append(coord1);
    linearRing4.append(coord1); linearRing4.append(coord2); linearRing4.append(coord3);
    
    GeoDataPolygon polygon;
    polygon.appendInnerBoundary(linearRing1);
    polygon.appendInnerBoundary(linearRing2);
    polygon.appendInnerBoundary(linearRing3);
    polygon.setOuterBoundary(linearRing4);
    polygon.setTessellate(true);
    
    QCOMPARE(polygon.innerBoundaries().size(), 3);
    
    GeoDataPolygon other = polygon;
    QCOMPARE(other.innerBoundaries().size(), 3);
    QVERIFY(other.innerBoundaries()[0][0] == coord1);
    QVERIFY(other.innerBoundaries()[0][1] == coord2);
    QVERIFY(other.innerBoundaries()[0][2] == coord3);
    QVERIFY(other.innerBoundaries()[1][0] == coord3);
    QVERIFY(other.innerBoundaries()[1][1] == coord2);
    QVERIFY(other.innerBoundaries()[1][2] == coord1);
    QVERIFY(other.innerBoundaries()[2][0] == coord1);
    QVERIFY(other.innerBoundaries()[2][1] == coord2);
    QVERIFY(other.innerBoundaries()[2][2] == coord3);
    QVERIFY(other.innerBoundaries()[2][3] == coord3);
    QVERIFY(other.innerBoundaries()[2][4] == coord2);
    QVERIFY(other.innerBoundaries()[2][5] == coord1);
    
    QCOMPARE(other.outerBoundary().size(), 6);

    QVERIFY(other.outerBoundary()[0] == coord3);
    QVERIFY(other.outerBoundary()[1] == coord2);
    QVERIFY(other.outerBoundary()[2] == coord1);
    QVERIFY(other.outerBoundary()[3] == coord1);
    QVERIFY(other.outerBoundary()[4] == coord2);
    QVERIFY(other.outerBoundary()[5] == coord3);
    
    QVERIFY(other.tessellate());
}

void TestGeoDataCopy::copyMultiGeometry()
{
    GeoDataLinearRing linearRing1;
    GeoDataLinearRing linearRing2;
    GeoDataLinearRing linearRing3;
    GeoDataLinearRing linearRing4;
    
    linearRing1.append(coord1); linearRing1.append(coord2); linearRing1.append(coord3);
    linearRing2.append(coord3); linearRing2.append(coord2); linearRing2.append(coord1);
    linearRing3.append(coord1); linearRing3.append(coord2); linearRing3.append(coord3);
    linearRing3.append(coord3); linearRing3.append(coord2); linearRing3.append(coord1);
    linearRing4.append(coord3); linearRing4.append(coord2); linearRing4.append(coord1);
    linearRing4.append(coord1); linearRing4.append(coord2); linearRing4.append(coord3);
    
    GeoDataPolygon *polygon = new GeoDataPolygon;
    polygon->appendInnerBoundary(linearRing1);
    polygon->appendInnerBoundary(linearRing2);
    polygon->appendInnerBoundary(linearRing3);
    polygon->setOuterBoundary(linearRing4);
    polygon->setTessellate(true);
    
    GeoDataMultiGeometry multiGeometry;
    multiGeometry.append(polygon);
    multiGeometry.append(new GeoDataLinearRing(linearRing1));
    multiGeometry.append(new GeoDataLinearRing(linearRing2));
    multiGeometry.append(new GeoDataLinearRing(linearRing3));
    multiGeometry.append(new GeoDataLinearRing(linearRing4));

    GeoDataMultiGeometry other = multiGeometry;

    QCOMPARE(other.size(), 5);
    QCOMPARE(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries().size(), 3);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[0][0] == coord1);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[0][1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[0][2] == coord3);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[1][0] == coord3);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[1][1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[1][2] == coord1);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[2][0] == coord1);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[2][1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[2][2] == coord3);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[2][3] == coord3);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[2][4] == coord2);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->innerBoundaries()[2][5] == coord1);
    
    QCOMPARE(static_cast<GeoDataPolygon*>(other.child(0))->outerBoundary().size(), 6);

    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->outerBoundary()[0] == coord3);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->outerBoundary()[1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->outerBoundary()[2] == coord1);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->outerBoundary()[3] == coord1);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->outerBoundary()[4] == coord2);
    QVERIFY(static_cast<GeoDataPolygon*>(other.child(0))->outerBoundary()[5] == coord3);
    
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(1))->at(0) == coord1);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(1))->at(1) == coord2);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(1))->at(2) == coord3);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(2))->at(0) == coord3);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(2))->at(1) == coord2);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(2))->at(2) == coord1);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(3))->at(0) == coord1);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(3))->at(1) == coord2);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(3))->at(2) == coord3);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(3))->at(3) == coord3);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(3))->at(4) == coord2);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(3))->at(5) == coord1);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(4))->at(0) == coord3);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(4))->at(1) == coord2);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(4))->at(2) == coord1);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(4))->at(3) == coord1);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(4))->at(4) == coord2);
    QVERIFY(static_cast<GeoDataLinearRing*>(other.child(4))->at(5) == coord3);

}

void TestGeoDataCopy::copyDocument()
{
    QWARN("add more document specific data");
    GeoDataPlacemark pl1;
    GeoDataPlacemark pl2;
    GeoDataPlacemark pl3;

    pl1.setCoordinate(point1.coordinates());
    pl2.setCoordinate(point2.coordinates());
    pl3.setCoordinate(point3.coordinates());

    GeoDataFolder *folder = new GeoDataFolder;
    folder->append(new GeoDataPlacemark(pl1));
    folder->append(new GeoDataPlacemark(pl2));
    folder->append(new GeoDataPlacemark(pl3));

    GeoDataDocument document;
    document.append(new GeoDataPlacemark(pl3));
    document.append(folder);
    document.append(new GeoDataPlacemark(pl1));

    GeoDataDocument other = document;

    QCOMPARE(document.size(), 3);
    QCOMPARE(other.size(), 3);

    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(0))->coordinate(), 143.4, 4, coordString[2]);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(2))->coordinate(), 123.4, 2, coordString[0]);

    GeoDataFolder *otherFolder = static_cast<GeoDataFolder*>(other.child(1));
    testCoordinate(static_cast<GeoDataPlacemark*>(otherFolder->child(0))->coordinate(), 123.4, 2, coordString[0]);
    testCoordinate(static_cast<GeoDataPlacemark*>(otherFolder->child(1))->coordinate(), 133.4, 3, coordString[1]);
    testCoordinate(static_cast<GeoDataPlacemark*>(otherFolder->child(2))->coordinate(), 143.4, 4, coordString[2]);

    other.append(new GeoDataPlacemark(pl1));

    QCOMPARE(document.size(), 3);
    QCOMPARE(other.size(), 4);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(3))->coordinate(), 123.4, 2, coordString[0]);
}

void TestGeoDataCopy::copyFolder()
{
    GeoDataPlacemark pl1, pl2, pl3;
    pl1.setCoordinate(point1.coordinates());
    pl2.setCoordinate(point2.coordinates());
    pl3.setCoordinate(point3.coordinates());

    GeoDataFolder folder;
    folder.append(new GeoDataPlacemark(pl1));
    folder.append(new GeoDataPlacemark(pl2));
    folder.append(new GeoDataPlacemark(pl3));

    QCOMPARE(folder.size(), 3);
    QCOMPARE(folder.child(0)->parent(), &folder);
    QCOMPARE(folder.child(1)->parent(), &folder);
    QCOMPARE(folder.child(2)->parent(), &folder);

    GeoDataFolder other = folder;
    QCOMPARE(other.size(), 3);
    QEXPECT_FAIL("", "FIXME", Continue);
    QCOMPARE(other.child(0)->parent(), &other);
    QEXPECT_FAIL("", "FIXME", Continue);
    QCOMPARE(other.child(1)->parent(), &other);
    QEXPECT_FAIL("", "FIXME", Continue);
    QCOMPARE(other.child(2)->parent(), &other);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(0))->coordinate(), 123.4, 2, coordString[0]);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(1))->coordinate(), 133.4, 3, coordString[1]);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(2))->coordinate(), 143.4, 4, coordString[2]);

    other.append(new GeoDataPlacemark(pl1));

    QCOMPARE(folder.size(), 3);
    QCOMPARE(other.size(), 4);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(3))->coordinate(), 123.4, 2, coordString[0]);
}

void TestGeoDataCopy::copyPlacemark()
{
    GeoDataPoint *point = new GeoDataPoint( GeoDataCoordinates(13.7107, 51.0235, 123.4, GeoDataCoordinates::Degree, 2) );
    point->setExtrude( true );

    // make sure that the coordinate contains the right values
    testCoordinate(point->coordinates(), 123.4, 2, coordString[0]);
    QCOMPARE(point->extrude(), true);

    GeoDataFolder folder;
    GeoDataPlacemark placemark;
    placemark.setName("Patrick Spendrin");
    placemark.setGeometry(point);
    placemark.setArea(12345678.0);
    placemark.setPopulation(123456789);
    placemark.setId("281012");
    placemark.setParent(&folder);

    testCoordinate(placemark.coordinate(), 123.4, 2, coordString[0]);
    testCoordinate(static_cast<GeoDataPoint*>(placemark.geometry())->coordinates(), 123.4, 2, coordString[0]);
    QCOMPARE(placemark.area(), 12345678.0);
    QCOMPARE(placemark.population(), (qint64)123456789);
    QCOMPARE(placemark.id(), QString("281012"));
    QCOMPARE(placemark.name(), QString::fromLatin1("Patrick Spendrin"));
    QCOMPARE(placemark.geometry()->parent(), &placemark);
    QCOMPARE(placemark.parent(), &folder);

    {
        GeoDataPlacemark other(placemark);

        QCOMPARE(other.id(), QString());
        QCOMPARE(other.parent(), static_cast<GeoDataObject *>(0));
        testCoordinate(other.coordinate(), 123.4, 2, coordString[0]);
        testCoordinate(static_cast<GeoDataPoint*>(other.geometry())->coordinates(), 123.4, 2, coordString[0]);
        QCOMPARE(other.area(), 12345678.0);
        QCOMPARE(other.population(), (qint64)123456789);
        QCOMPARE(other.name(), QString::fromLatin1("Patrick Spendrin"));
        QCOMPARE(other.geometry()->parent(), &other);

        other.setPopulation(987654321);

        testCoordinate(other.coordinate(), 123.4, 2, coordString[0]);
        testCoordinate(static_cast<GeoDataPoint*>(other.geometry())->coordinates(), 123.4, 2, coordString[0]);
        QCOMPARE(other.area(), 12345678.0);
        QCOMPARE(other.population(), (qint64)987654321);
        QCOMPARE(placemark.population(), (qint64)123456789);
        QCOMPARE(placemark.name(), QString::fromLatin1("Patrick Spendrin"));
        QCOMPARE(other.name(), QString::fromLatin1("Patrick Spendrin"));
    }

    {
        GeoDataPlacemark other;

        QCOMPARE(other.parent(), static_cast<GeoDataObject *>(0)); // add a check before assignment to avoid compiler optimizing to copy c'tor

        other = placemark;

        QCOMPARE(other.id(), QString());
        QCOMPARE(other.parent(), static_cast<GeoDataObject *>(0));
        testCoordinate(other.coordinate(), 123.4, 2, coordString[0]);
        testCoordinate(static_cast<GeoDataPoint*>(other.geometry())->coordinates(), 123.4, 2, coordString[0]);
        QCOMPARE(other.area(), 12345678.0);
        QCOMPARE(other.population(), (qint64)123456789);
        QCOMPARE(other.name(), QString::fromLatin1("Patrick Spendrin"));
        QCOMPARE(other.geometry()->parent(), &other);

        other.setPopulation(987654321);

        testCoordinate(other.coordinate(), 123.4, 2, coordString[0]);
        testCoordinate(static_cast<GeoDataPoint*>(other.geometry())->coordinates(), 123.4, 2, coordString[0]);
        QCOMPARE(other.area(), 12345678.0);
        QCOMPARE(other.population(), (qint64)987654321);
        QCOMPARE(placemark.population(), (qint64)123456789);
        QCOMPARE(placemark.name(), QString::fromLatin1("Patrick Spendrin"));
        QCOMPARE(other.name(), QString::fromLatin1("Patrick Spendrin"));
    }

    {
        GeoDataFolder otherFolder;
        GeoDataPlacemark other;
        other.setParent(&otherFolder);

        QCOMPARE(other.parent(), &otherFolder);

        other = placemark;

        QCOMPARE(other.parent(), &otherFolder);
    }

    {
        const GeoDataPlacemark other(placemark);
        const GeoDataPlacemark other2(other);

        QCOMPARE(placemark.geometry()->parent(), &placemark);
        QEXPECT_FAIL("", "geometry needs to be detach()ed", Continue);
        QCOMPARE(other.geometry()->parent(), &other);
        QEXPECT_FAIL("", "geometry needs to be detach()ed", Continue);
        QCOMPARE(other2.geometry()->parent(), &other2);
    }

    {
        GeoDataPlacemark other;
        GeoDataPlacemark other2;

        QCOMPARE(placemark.geometry()->parent(), &placemark);
        QCOMPARE(other.geometry()->parent(), &other);
        QCOMPARE(other2.geometry()->parent(), &other2);

        other = placemark;
        other2 = other;

        QCOMPARE(placemark.geometry()->parent(), &placemark);
        QCOMPARE(other.geometry()->parent(), &other);
        QCOMPARE(other2.geometry()->parent(), &other2);
    }
}

void TestGeoDataCopy::copyHotSpot()
{
    QPointF point(0.25, 0.75);
    

    GeoDataHotSpot first(point);
    GeoDataHotSpot::Units xunits;
    GeoDataHotSpot::Units yunits;
    GeoDataHotSpot second = first;

    QVERIFY(first.hotSpot(xunits, yunits) == QPointF(0.25, 0.75));
    QVERIFY(second.hotSpot(xunits, yunits) == QPointF(0.25, 0.75));

    first.setHotSpot(QPointF(0.3333333, 0.666666));
    
    QVERIFY(first.hotSpot(xunits, yunits) == QPointF(0.3333333, 0.666666));
    QVERIFY(second.hotSpot(xunits, yunits) == QPointF(0.25, 0.75));
}

void TestGeoDataCopy::copyLatLonBox()
{
    // north south east west
    GeoDataLatLonBox llbox(30.1, 12.2, 110.0, 44.9, GeoDataCoordinates::Degree);
    QCOMPARE(llbox.north(GeoDataCoordinates::Degree), 30.1);
    QCOMPARE(llbox.south(GeoDataCoordinates::Degree), 12.2);
    QCOMPARE(llbox.east(GeoDataCoordinates::Degree), 110.0);
    QCOMPARE(llbox.west(GeoDataCoordinates::Degree), 44.9);

    GeoDataLatLonBox other = llbox;

    QCOMPARE(other.north(GeoDataCoordinates::Degree), 30.1);
    QCOMPARE(other.south(GeoDataCoordinates::Degree), 12.2);
    QCOMPARE(other.east(GeoDataCoordinates::Degree), 110.0);
    QCOMPARE(other.west(GeoDataCoordinates::Degree), 44.9);
    
    llbox.setNorth(0.1);
    other.setSouth(1.4);
    
    QCOMPARE(llbox.north(), 0.1);
    QCOMPARE(llbox.south(GeoDataCoordinates::Degree), 12.2);
    QCOMPARE(other.north(GeoDataCoordinates::Degree), 30.1);
    QCOMPARE(other.south(), 1.4);
}

void TestGeoDataCopy::copyStyle()
{
    GeoDataLineStyle line(Qt::green);
    line.setWidth(2.0);

    GeoDataStyle style;

    style.setLineStyle( line );
    
    QCOMPARE(style.lineStyle().width(), (float)2.0);
    QVERIFY(style.lineStyle().color() == Qt::green);
}

void TestGeoDataCopy::copyIconStyle()
{
    // hotspottesting is not implemented as I am not sure how it should work
    GeoDataIconStyle icon;
    icon.setScale(2.0);

    QCOMPARE(icon.scale(), (float)2.0);

    GeoDataIconStyle other = icon;

    QCOMPARE(other.scale(), (float)2.0);

    icon.setScale(5.0);

    QCOMPARE(icon.scale(), (float)5.0);
    QCOMPARE(other.scale(), (float)2.0);
}

void TestGeoDataCopy::copyLabelStyle()
{
    QFont testFont(QFont(QStringLiteral("Sans Serif")).family(), 12, 10, false);
    GeoDataLabelStyle label(testFont, Qt::red);
    label.setScale(2.0);

    QCOMPARE(label.scale(), (float)2.0);
    QVERIFY(label.color() == Qt::red);
    QVERIFY(label.font() == testFont);

    GeoDataLabelStyle other = label;

    QCOMPARE(other.scale(), (float)2.0);
    QVERIFY(other.color() == Qt::red);
    QVERIFY(other.font() == testFont);

    other.setColor(Qt::darkRed);
    label.setScale(5.0);

    QCOMPARE(label.scale(), (float)5.0);
    QCOMPARE(other.scale(), (float)2.0);
    QVERIFY(label.color() == Qt::red);
    QVERIFY(other.color() == Qt::darkRed);
}

void TestGeoDataCopy::copyLineStyle()
{
    GeoDataLineStyle line(Qt::green);
    line.setWidth(2.0);

    QCOMPARE(line.width(), (float)2.0);
    QVERIFY(line.color() == Qt::green);

    GeoDataLineStyle other = line;

    QCOMPARE(other.width(), (float)2.0);
    QVERIFY(other.color() == Qt::green);

    other.setColor(Qt::darkGreen);
    line.setWidth(5.0);

    QCOMPARE(line.width(), (float)5.0);
    QCOMPARE(other.width(), (float)2.0);
    QVERIFY(line.color() == Qt::green);
    QVERIFY(other.color() == Qt::darkGreen);
}

void TestGeoDataCopy::copyPolyStyle()
{
    GeoDataPolyStyle poly(Qt::blue);
    poly.setFill(true);
    poly.setOutline(false);

    QCOMPARE(poly.fill(), true);
    QCOMPARE(poly.outline(), false);
    QVERIFY(poly.color() == Qt::blue);

    GeoDataPolyStyle other = poly;

    QCOMPARE(other.fill(), true);
    QCOMPARE(other.outline(), false);
    QVERIFY(other.color() == Qt::blue);

    other.setOutline(true);
    poly.setColor( Qt::cyan );

    QCOMPARE(poly.outline(), false);
    QCOMPARE(other.outline(), true);
    QVERIFY(poly.color() == Qt::cyan);
    QVERIFY(other.color() == Qt::blue);
}

void TestGeoDataCopy::copyStyleMap()
{
    GeoDataStyleMap styleMap;
    styleMap["germany"] = "gst1";
    styleMap["germany"] = "gst2";
    styleMap["germany"] = "gst3";
    styleMap["poland"] = "pst1";
    styleMap["poland"] = "pst2";
    styleMap["poland"] = "pst3";
    styleMap.setLastKey("poland");

    QVERIFY( styleMap.lastKey() == QString("poland") );

    GeoDataStyleMap testMap = styleMap;

    QVERIFY( styleMap == testMap );

    testMap.insert("Romania", "rst1");
    testMap.insert("Romania", "rst2");
    testMap.insert("Romania", "rst3");
    testMap.setLastKey("Romania");

    QVERIFY( testMap.lastKey() == QString("Romania") );
    QVERIFY( styleMap.lastKey() == QString("poland") );
}

}

QTEST_MAIN( Marble::TestGeoDataCopy )

#include "TestGeoDataCopy.moc"

