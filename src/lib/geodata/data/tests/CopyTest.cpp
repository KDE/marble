//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Patrick Spendrin  <ps_ml@gmx.de>
//

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include "MarbleDirs.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

class CopyTest : public QObject {
    Q_OBJECT
    private slots:
        void initTestCase();
        void copyCoordinates();
        void copyLineString();
        void copyLinearRing();
        void copyPoint();
        void copyPolygon();
        void copyMultiGeometry();
        void copyDocument();
        void copyFolder();
        void copyPlacemark();
    private:
        QStringList coordString;
        GeoDataCoordinates coord1;
        GeoDataCoordinates coord2;
        GeoDataCoordinates coord3;
};

void CopyTest::initTestCase() {
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    coordString << QString(" 13° 42' 38\"E,  51° 01' 24\"N" );
    coordString << QString(" 14° 42' 38\"E,  52° 01' 24\"N" );
    coordString << QString(" 15° 42' 38\"E,  53° 01' 24\"N" );

    coord1.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    coord1.setDetail(2);
    QCOMPARE(coord1.altitude(), 123.4);
    QCOMPARE(coord1.detail(), 2);
    QCOMPARE(coord1.toString(), coordString[0]);
    
    coord2.set(14.7107,52.0235, 133.4, GeoDataCoordinates::Degree);
    coord2.setDetail(3);
    QCOMPARE(coord2.altitude(), 133.4);
    QCOMPARE(coord2.detail(), 3);
    QCOMPARE(coord2.toString(), coordString[1]);

    coord3.set(15.7107,53.0235, 143.4, GeoDataCoordinates::Degree);
    coord3.setDetail(4);
    QCOMPARE(coord3.altitude(), 143.4);
    QCOMPARE(coord3.detail(), 4);
    QCOMPARE(coord3.toString(), coordString[2]);

}

void CopyTest::copyCoordinates() {
    GeoDataCoordinates other = coord1;
    
    // make sure that the coordinate contains the right values
    QCOMPARE(other.altitude(), 123.4);
    QCOMPARE(other.detail(), 2);
    QCOMPARE(other.toString(), coordString[0]);
    
    QVERIFY(coord1 == other);
}

void CopyTest::copyPoint() {
    GeoDataPoint point;

    point.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    point.setDetail(2);

    // make sure that the coordinate contains the right values
    QCOMPARE(point.altitude(), 123.4);
    QCOMPARE(point.detail(), 2);
    QCOMPARE(point.toString(), coordString[0]);

    GeoDataPoint other = point;
    
    // make sure that the coordinate contains the right values
    QCOMPARE(other.altitude(), 123.4);
    QCOMPARE(other.detail(), 2);
    QCOMPARE(other.toString(), coordString[0]);
    
    QVERIFY(point == other);
}

void CopyTest::copyLineString() {
    GeoDataLineString lineString;
    lineString.setTessellate(true);
    

    lineString.append(coord1);
    lineString.append(coord2);
    lineString.append(coord3);
    QVERIFY(lineString.size() == 3);

    GeoDataLineString other = lineString;
    QVERIFY(other.size() == 3);

    QCOMPARE(lineString.at(0).altitude(), 123.4);
    QCOMPARE(lineString.at(0).detail(), 2);
    QCOMPARE(lineString.at(0).toString(), coordString[0]);
    
    QCOMPARE(other.at(2).altitude(), 143.4);
    QCOMPARE(other.at(2).detail(), 4);
    QCOMPARE(other.at(2).toString(), coordString[2]);
    
    QVERIFY(other.at(2) == coord3);
    QVERIFY(other.tessellate());
}

void CopyTest::copyLinearRing() {
    GeoDataLinearRing linearRing;

    linearRing.setTessellate(true);


    linearRing.append(coord1);
    linearRing.append(coord2);
    linearRing.append(coord3);
    QVERIFY(linearRing.size() == 3);

    GeoDataLinearRing other = linearRing;
    QVERIFY(other.size() == 3);

    QCOMPARE(linearRing.at(0).altitude(), 123.4);
    QCOMPARE(linearRing.at(0).detail(), 2);
    QCOMPARE(linearRing.at(0).toString(), coordString[0]);
    
    QCOMPARE(other.at(2).altitude(), 143.4);
    QCOMPARE(other.at(2).detail(), 4);
    QCOMPARE(other.at(2).toString(), coordString[2]);
    
    QVERIFY(other.at(2) == coord3);
    QVERIFY(other.tessellate());
}

void CopyTest::copyPolygon() {
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

void CopyTest::copyMultiGeometry() {
    GeoDataMultiGeometry multiGeometry;
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
    
    multiGeometry.append(polygon);
    multiGeometry.append(linearRing1);
    multiGeometry.append(linearRing2);
    multiGeometry.append(linearRing3);
    multiGeometry.append(linearRing4);

    GeoDataMultiGeometry other = multiGeometry;

    QCOMPARE(other.size(), 5);
/*    QCOMPARE(reinterpret_cast<GeoDataPolygon>(other[0]).innerBoundaries().size(), 3);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[0][0] == coord1);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[0][1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[0][2] == coord3);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[1][0] == coord3);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[1][1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[1][2] == coord1);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[2][0] == coord1);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[2][1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[2][2] == coord3);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[2][3] == coord3);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[2][4] == coord2);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).innerBoundaries()[2][5] == coord1);
    
    QCOMPARE(static_cast<GeoDataPolygon>(other[0]).outerBoundary().size(), 6);

    QVERIFY(static_cast<GeoDataPolygon>(other[0]).outerBoundary()[0] == coord3);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).outerBoundary()[1] == coord2);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).outerBoundary()[2] == coord1);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).outerBoundary()[3] == coord1);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).outerBoundary()[4] == coord2);
    QVERIFY(static_cast<GeoDataPolygon>(other[0]).outerBoundary()[5] == coord3);
    
    QVERIFY(other[1][0] == coord1);
    QVERIFY(other[1][1] == coord2);
    QVERIFY(other[1][2] == coord3);
    QVERIFY(other[2][0] == coord3);
    QVERIFY(other[2][1] == coord2);
    QVERIFY(other[2][2] == coord1);
    QVERIFY(other[3][0] == coord1);
    QVERIFY(other[3][1] == coord2);
    QVERIFY(other[3][2] == coord3);
    QVERIFY(other[3][3] == coord3);
    QVERIFY(other[3][4] == coord2);
    QVERIFY(other[3][5] == coord1);
    QVERIFY(other[4][0] == coord3);
    QVERIFY(other[4][1] == coord2);
    QVERIFY(other[4][2] == coord1);
    QVERIFY(other[4][3] == coord1);
    QVERIFY(other[4][4] == coord2);
    QVERIFY(other[4][5] == coord3);*/
    QFAIL("Test not implemented");
}

void CopyTest::copyDocument() {
    QFAIL("Test not implemented");
}

void CopyTest::copyFolder() {
    QFAIL("Test not implemented");
}

void CopyTest::copyPlacemark() {
    QFAIL("Test not implemented");
}

}

QTEST_MAIN( Marble::CopyTest )

#include "CopyTest.moc"
