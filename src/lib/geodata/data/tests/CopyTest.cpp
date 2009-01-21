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
#include <QtCore/QDebug>

#include "MarbleDirs.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

class CopyTest : public QObject {
    Q_OBJECT
    private slots:
        void initTestCase();
        void copyCoordinates();
        void copyLineString();
    private:
        QString content;
};

void CopyTest::initTestCase() {
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    content = QString( 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<kml xmlns=\"http://earth.google.com/kml/2.1\">\n"
"  <Document>\n"
"    <Placemark>\n"
"      <name>LinearRingTest.kml</name>\n"
"      <Polygon>\n"
"        <outerBoundaryIs>\n"
"          <LinearRing>\n"
"            <coordinates>\n"
"              -122.365662,37.826988,0\n"
"              -122.365202,37.826302,0\n"
"              -122.364581,37.82655,0\n"
"              -122.365038,37.827237,0\n"
"              -122.365662,37.826988,0\n"
"            </coordinates>\n"
"          </LinearRing>\n"
"        </outerBoundaryIs>\n"
"      </Polygon>\n"
"    </Placemark>\n"
"  </Document>\n"
"</kml>" );
}

void CopyTest::copyCoordinates() {
    GeoDataCoordinates coord;
    QString coordString(" 13° 42' 38\"E,  51° 01' 24\"N" );

    coord.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    coord.setDetail(2);

    // make sure that the coordinate contains the right values
    QCOMPARE(coord.altitude(), 123.4);
    QCOMPARE(coord.detail(), 2);
    QCOMPARE(coord.toString(), coordString);

    GeoDataCoordinates other = coord;
    
    // make sure that the coordinate contains the right values
    QCOMPARE(other.altitude(), 123.4);
    QCOMPARE(other.detail(), 2);
    QCOMPARE(other.toString(), coordString);
    
    QVERIFY(coord == other);
}

void CopyTest::copyLineString() {
    GeoDataLineString lineString;
    GeoDataCoordinates coord;
    
    QString coordString1(" 13° 42' 38\"E,  51° 01' 24\"N" );
    QString coordString2(" 14° 42' 38\"E,  52° 01' 24\"N" );
    QString coordString3(" 15° 42' 38\"E,  53° 01' 24\"N" );
    
    
    lineString.setTessellate(true);
    

    coord.set(13.7107,51.0235, 123.4, GeoDataCoordinates::Degree);
    coord.setDetail(2);
    QCOMPARE(coord.altitude(), 123.4);
    QCOMPARE(coord.detail(), 2);
    QCOMPARE(coord.toString(), coordString1);
    lineString.append(coord);
    QVERIFY(lineString.size() == 1);
    
    coord.set(14.7107,52.0235, 133.4, GeoDataCoordinates::Degree);
    coord.setDetail(3);
    QCOMPARE(coord.altitude(), 133.4);
    QCOMPARE(coord.detail(), 3);
    QCOMPARE(coord.toString(), coordString2);
    lineString.append(coord);
    QVERIFY(lineString.size() == 2);

    coord.set(15.7107,53.0235, 143.4, GeoDataCoordinates::Degree);
    coord.setDetail(4);
    QCOMPARE(coord.altitude(), 143.4);
    QCOMPARE(coord.detail(), 4);
    QCOMPARE(coord.toString(), coordString3);
    lineString.append(coord);
    QVERIFY(lineString.size() == 3);

    GeoDataLineString other = lineString;
}
}

QTEST_MAIN( Marble::CopyTest )

#include "CopyTest.moc"
