//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Patrick Spendrin  <ps_ml@gmx.de>
//

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtCore/QPointF>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "MarbleDebug.h"

#include "MarbleDirs.h"
#include "GeoDataParser.h"

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
        void copyLatLonAltBox();

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
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    coordString << QString(" 13° 42' 38,5\"E,  51° 01' 24,6\"N" );
    coordString << QString(" 14° 42' 38,5\"E,  52° 01' 24,6\"N" );
    coordString << QString(" 15° 42' 38,5\"E,  53° 01' 24,6\"N" );

    coord1.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    coord1.setDetail(2);
    testCoordinate(coord1, 123.4, 2, coordString[0]);
    
    coord2.set(14.7107,52.0235, 133.4, GeoDataCoordinates::Degree);
    coord2.setDetail(3);
    testCoordinate(coord2, 133.4, 3, coordString[1]);

    coord3.set(15.7107,53.0235, 143.4, GeoDataCoordinates::Degree);
    coord3.setDetail(4);
    testCoordinate(coord3, 143.4, 4, coordString[2]);
    
    point1.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    point1.setDetail(2);
    testCoordinate(point1, 123.4, 2, coordString[0]);
    
    point2.set(14.7107,52.0235, 133.4, GeoDataCoordinates::Degree);
    point2.setDetail(3);
    testCoordinate(point2, 133.4, 3, coordString[1]);

    point3.set(15.7107,53.0235, 143.4, GeoDataCoordinates::Degree);
    point3.setDetail(4);
    testCoordinate(point3, 143.4, 4, coordString[2]);
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

    point.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    point.setDetail(2);
    point.setExtrude( true );

    // make sure that the coordinate contains the right values
    testCoordinate(point, 123.4, 2, coordString[0]);
    QCOMPARE(point.extrude(), true);

    GeoDataPoint other = point;
    
    // make sure that the coordinate contains the right values
    testCoordinate(other, 123.4, 2, coordString[0]);
    QCOMPARE(other.extrude(), true);
    
    QVERIFY(point == other);
    
    point.setDetail(17);
    point.setExtrude(false);
    QCOMPARE(other.detail(), 2);
    QCOMPARE(point.detail(), 17);
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
    GeoDataMultiGeometry multiGeometry;
    GeoDataLinearRing *linearRing1 = new GeoDataLinearRing;
    GeoDataLinearRing *linearRing2 = new GeoDataLinearRing;
    GeoDataLinearRing *linearRing3 = new GeoDataLinearRing;
    GeoDataLinearRing *linearRing4 = new GeoDataLinearRing;
    
    linearRing1->append(coord1); linearRing1->append(coord2); linearRing1->append(coord3);
    linearRing2->append(coord3); linearRing2->append(coord2); linearRing2->append(coord1);
    linearRing3->append(coord1); linearRing3->append(coord2); linearRing3->append(coord3);
    linearRing3->append(coord3); linearRing3->append(coord2); linearRing3->append(coord1);
    linearRing4->append(coord3); linearRing4->append(coord2); linearRing4->append(coord1);
    linearRing4->append(coord1); linearRing4->append(coord2); linearRing4->append(coord3);
    
    GeoDataPolygon *polygon = new GeoDataPolygon;
    polygon->appendInnerBoundary(*linearRing1);
    polygon->appendInnerBoundary(*linearRing2);
    polygon->appendInnerBoundary(*linearRing3);
    polygon->setOuterBoundary(*linearRing4);
    polygon->setTessellate(true);
    
    multiGeometry.append(polygon);
    multiGeometry.append(linearRing1);
    multiGeometry.append(linearRing2);
    multiGeometry.append(linearRing3);
    multiGeometry.append(linearRing4);

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
    

    delete linearRing1;
    delete linearRing2;
    delete linearRing3;
    delete linearRing4;
    delete polygon;
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
    GeoDataPlacemark *pl1 = new GeoDataPlacemark;
    GeoDataPlacemark *pl2 = new GeoDataPlacemark;
    GeoDataPlacemark *pl3 = new GeoDataPlacemark;

    pl1->setCoordinate(point1);
    pl2->setCoordinate(point2);
    pl3->setCoordinate(point3);

    GeoDataFolder *folder = new GeoDataFolder;
    folder->append(pl1);
    folder->append(pl2);
    folder->append(pl3);

    GeoDataDocument document;
    document.append(pl3);
    document.append(folder);
    document.append(pl1);

    GeoDataDocument other = document;

    QCOMPARE(document.size(), 3);
    QCOMPARE(other.size(), 3);

    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(0))->coordinate(), 143.4, 4, coordString[2]);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(2))->coordinate(), 123.4, 2, coordString[0]);

    GeoDataFolder *otherFolder = static_cast<GeoDataFolder*>(other.child(1));
    testCoordinate(static_cast<GeoDataPlacemark*>(otherFolder->child(0))->coordinate(), 123.4, 2, coordString[0]);
    testCoordinate(static_cast<GeoDataPlacemark*>(otherFolder->child(1))->coordinate(), 133.4, 3, coordString[1]);
    testCoordinate(static_cast<GeoDataPlacemark*>(otherFolder->child(2))->coordinate(), 143.4, 4, coordString[2]);

    other.append(pl1);

    QCOMPARE(document.size(), 3);
    QCOMPARE(other.size(), 4);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(3))->coordinate(), 123.4, 2, coordString[0]);
}

void TestGeoDataCopy::copyFolder()
{
    GeoDataPlacemark pl1, pl2, pl3;
    pl1.setCoordinate(point1);
    pl2.setCoordinate(point2);
    pl3.setCoordinate(point3);

    GeoDataFolder folder;
    folder.append(&pl1);
    folder.append(&pl2);
    folder.append(&pl3);

    QCOMPARE(folder.size(), 3);

    GeoDataFolder other = folder;
    QCOMPARE(other.size(), 3);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(0))->coordinate(), 123.4, 2, coordString[0]);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(1))->coordinate(), 133.4, 3, coordString[1]);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(2))->coordinate(), 143.4, 4, coordString[2]);

    other.append(&pl1);

    QCOMPARE(folder.size(), 3);
    QCOMPARE(other.size(), 4);
    testCoordinate(static_cast<GeoDataPlacemark*>(other.child(3))->coordinate(), 123.4, 2, coordString[0]);
}

void TestGeoDataCopy::copyPlacemark()
{
    GeoDataPoint point;

    point.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    point.setDetail(2);
    point.setExtrude( true );

    // make sure that the coordinate contains the right values
    testCoordinate(point, 123.4, 2, coordString[0]);
    QCOMPARE(point.extrude(), true);

    GeoDataPlacemark placemark;
    placemark.setName("Patrick Spendrin");
    placemark.setGeometry(&point);
    placemark.setCoordinate(point);
    placemark.setArea(12345678.0);
    placemark.setPopulation(123456789);
    placemark.setId(281012);

    testCoordinate(placemark.coordinate(), 123.4, 2, coordString[0]);
//    TODO Fix ref
//    testCoordinate(static_cast<GeoDataPoint*>(placemark.geometry()), 123.4, 2, coordString[0]);
    QCOMPARE(placemark.area(), 12345678.0);
    QCOMPARE(placemark.population(), (qint64)123456789);
    QCOMPARE(placemark.id(), 281012);
    QCOMPARE(placemark.name(), QString::fromLatin1("Patrick Spendrin"));

    GeoDataPlacemark other = placemark;
    
    testCoordinate(other.coordinate(), 123.4, 2, coordString[0]);
//    TODO Fix ref
//    testCoordinate(static_cast<GeoDataPoint*>(other.geometry()), 123.4, 2, coordString[0]);
    QCOMPARE(other.area(), 12345678.0);
    QCOMPARE(other.population(), (qint64)123456789);
    QCOMPARE(other.id(), 281012);
    QCOMPARE(other.name(), QString::fromLatin1("Patrick Spendrin"));

    other.setPopulation(987654321);

    testCoordinate(other.coordinate(), 123.4, 2, coordString[0]);
//    TODO Fix ref
//    testCoordinate(static_cast<GeoDataPoint*>(other.geometry()), 123.4, 2, coordString[0]);
    QCOMPARE(other.area(), 12345678.0);
    QCOMPARE(other.population(), (qint64)987654321);
    QCOMPARE(placemark.population(), (qint64)123456789);
    QCOMPARE(placemark.name(), QString::fromLatin1("Patrick Spendrin"));
    QCOMPARE(other.name(), QString::fromLatin1("Patrick Spendrin"));
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

void TestGeoDataCopy::copyLatLonAltBox()
{
    GeoDataLatLonAltBox llabox(GeoDataLatLonBox(30.1, 12.2, 110.0, 44.9, GeoDataCoordinates::Degree));
    QCOMPARE(llabox.north(GeoDataCoordinates::Degree), 30.1);
    QCOMPARE(llabox.south(GeoDataCoordinates::Degree), 12.2);
    QCOMPARE(llabox.east(GeoDataCoordinates::Degree), 110.0);
    QCOMPARE(llabox.west(GeoDataCoordinates::Degree), 44.9);

    GeoDataLatLonAltBox other = llabox;

    QCOMPARE(other.north(GeoDataCoordinates::Degree), 30.1);
    QCOMPARE(other.south(GeoDataCoordinates::Degree), 12.2);
    QCOMPARE(other.east(GeoDataCoordinates::Degree), 110.0);
    QCOMPARE(other.west(GeoDataCoordinates::Degree), 44.9);
    
    llabox.setNorth(0.1);
    other.setSouth(1.4);
    
    QCOMPARE(llabox.north(), 0.1);
    QCOMPARE(llabox.south(GeoDataCoordinates::Degree), 12.2);
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
    QFont testFont(QFont("Sans Serif").family(), 12, 10, false);
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
    QMap<QString,QString> testMap;
    styleMap["germany"] = "gst1";
    styleMap["germany"] = "gst2";
    styleMap["germany"] = "gst3";
    styleMap["poland"] = "pst1";
    styleMap["poland"] = "pst2";
    styleMap["poland"] = "pst3";
    testMap["germany"] = "gst1";
    testMap["germany"] = "gst2";
    testMap["germany"] = "gst3";
    testMap["poland"] = "pst1";
    testMap["poland"] = "pst2";
    testMap["poland"] = "pst3";
    styleMap.setLastKey("poland");

    QVERIFY(styleMap == testMap);
    QVERIFY(styleMap.lastKey() == QString("poland"));
}

}

QTEST_MAIN( Marble::TestGeoDataCopy )

#include "TestGeoDataCopy.moc"

