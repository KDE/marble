//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include <QObject>

#include <GeoDataDocument.h>
#include <GeoDataPlacemark.h>
#include <GeoDataOrientation.h>
#include <GeoDataScale.h>
#include <MarbleDebug.h>
#include <GeoDataModel.h>
#include <GeoDataLink.h>
#include <GeoDataLocation.h>

#include "TestUtils.h"

using namespace Marble ;
class TestModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};
void TestModel::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestModel::simpleParseTest()
{
  QString const centerContent (
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<kml xmlns=\"http://www.opengis.net/kml/2.2\">"
"<Placemark>"
  "<Model id=\"model_4\">"
    "<altitudeMode>relativeToGround</altitudeMode>"
    "<Location>"
      "<longitude>-105.27</longitude>"
      "<latitude>40.00</latitude>"
      "<altitude>23.4</altitude>"
    "</Location>"
    "<Orientation>"
      "<heading>1</heading>"
      "<tilt>2</tilt>"
      "<roll>3</roll>"
    "</Orientation>"
    "<Scale>"
      "<x>3</x>"
      "<y>4</y>"
      "<z>5</z>"
    "</Scale>"
    "<Link>"
      "<href>MackyBldg.kmz/files/CU Macky.dae</href>"
      "<refreshMode>onExpire</refreshMode>"
    "</Link>"
    "<ResourceMap id=\"resourcemap_for_model_4\">"
      "<Alias>"
        "<sourceHref>../files/CU-Macky-4sideturretnoCulling.jpg</sourceHref>"
        "<targetHref>../files/CU-Macky-4sideturretnoCulling.jpg</targetHref>"
      "</Alias>"
    "</ResourceMap>"
  "</Model>"
"</Placemark>"
"</kml>" );
    GeoDataDocument* dataDocument = parseKml( centerContent );

    QCOMPARE( dataDocument->placemarkList().size(), 1 );

    GeoDataPlacemark *placemark = dataDocument->placemarkList().at( 0 );

    GeoDataModel *model = dynamic_cast<GeoDataModel*>( placemark->geometry() );

    QVERIFY( model != 0 );

    QCOMPARE( model->altitudeMode(), RelativeToGround);

    QCOMPARE( model->location().altitude(), 23.4);
    QCOMPARE( model->location().latitude(GeoDataCoordinates::Degree), 40.00 );
    QCOMPARE( model->location().longitude(GeoDataCoordinates::Degree), -105.27 );

    QCOMPARE( model->orientation().heading(), 1.0);
    QCOMPARE( model->orientation().tilt(), 2.0);
    QCOMPARE( model->orientation().roll(), 3.0);
    QCOMPARE( model->scale().x(), 3.0);
    QCOMPARE( model->scale().y(), 4.0);
    QCOMPARE( model->scale().z(), 5.0);
    QCOMPARE( model->link().href(), QString("MackyBldg.kmz/files/CU Macky.dae"));
    QCOMPARE( model->link().refreshMode(), GeoDataLink::OnExpire );
    QCOMPARE( model->targetHref(), QString("../files/CU-Macky-4sideturretnoCulling.jpg"));
    QCOMPARE( model->sourceHref(), QString("../files/CU-Macky-4sideturretnoCulling.jpg"));

    delete dataDocument;
}
QTEST_MAIN( TestModel )

#include "TestModel.moc"
