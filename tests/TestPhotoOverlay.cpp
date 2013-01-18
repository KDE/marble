//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include <QtCore/QObject>
#include <QtTest/QtTest>

#include <GeoDataParser.h>
#include <GeoDataDocument.h>
#include <MarbleDebug.h>
#include <GeoDataFolder.h>
#include <GeoDataPhotoOverlay.h>
#include "TestUtils.h"

using namespace Marble;

class TestPhotoOverlay : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};

void TestPhotoOverlay::initTestCase()
{
    MarbleDebug::enable = true;
}

void TestPhotoOverlay::simpleParseTest()
{
  QString const centerContent (
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Folder>"
        "  <PhotoOverlay>"
        "    <rotation>12.455</rotation>"
        "    <ViewVolume>"
        "        <near>1000</near>"
        "        <leftFov>-60</leftFov>"
        "        <rightFov>60</rightFov>"
        "        <bottomFov>-45</bottomFov>"
        "        <topFov>45</topFov>"
        "    </ViewVolume>"
        "    <ImagePyramid>"
        "        <tileSize>219</tileSize>"
        "        <maxWidth>22</maxWidth>"
        "        <maxHeight>36</maxHeight>"
        "        <gridOrigin>lowerLeft</gridOrigin>"
        "    </ImagePyramid>"
        "    <Point>"
        "        <coordinates>45.78665,0.6565</coordinates>"
        "    </Point>"
        "    <shape>sphere</shape>"
        "  </PhotoOverlay>"
        "</Folder>"
        "</kml>" );

    GeoDataDocument* dataDocument = parseKml( centerContent  );
    QCOMPARE( dataDocument->folderList().size(), 1 );
    GeoDataFolder *folder = dataDocument->folderList().at( 0 );
    QCOMPARE( folder->size(), 1 );
    GeoDataPhotoOverlay *overlay = dynamic_cast<GeoDataPhotoOverlay*>( folder->child( 0 ) );
    QVERIFY( overlay != 0 );

    QFUZZYCOMPARE( overlay->rotation(), 12.455, 0.0001 );

    QFUZZYCOMPARE( overlay->viewVolume().near(), 1000.0, 0.0001 );
    QFUZZYCOMPARE( overlay->viewVolume().leftFov(), -60.0, 0.0001 );
    QFUZZYCOMPARE( overlay->viewVolume().rightFov(), 60.0, 0.0001 );
    QFUZZYCOMPARE( overlay->viewVolume().bottomFov(), -45.0, 0.0001 );
    QFUZZYCOMPARE( overlay->viewVolume().topFov(), 45.0, 0.0001 );

    QFUZZYCOMPARE( overlay->imagePyramid().tileSize(), 219, 0.0001 );
    QFUZZYCOMPARE( overlay->imagePyramid().maxWidth(), 22, 0.0001 );
    QFUZZYCOMPARE( overlay->imagePyramid().maxHeight(), 36, 0.0001 );
    QFUZZYCOMPARE( overlay->imagePyramid().gridOrigin(), GeoDataImagePyramid::LowerLeft, 0.0001 );

    QFUZZYCOMPARE( overlay->point().coordinates().longitude( GeoDataCoordinates::Degree ), 45.78665, 0.0001 );
    QFUZZYCOMPARE( overlay->point().coordinates().latitude( GeoDataCoordinates::Degree ), 0.6565, 0.0001 );

    QFUZZYCOMPARE( overlay->shape(), GeoDataPhotoOverlay::Sphere, 0.0001 );

    delete dataDocument;
}

QTEST_MAIN( TestPhotoOverlay )

#include "TestPhotoOverlay.moc"
