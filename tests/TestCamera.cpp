// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QObject>

#include "TestUtils.h"
#include <GeoDataCamera.h>
#include <GeoDataDocument.h>
#include <MarbleDebug.h>

using namespace Marble;

class TestCamera : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestCamera::initTestCase()
{
    MarbleDebug::setEnabled(true);
}

void TestCamera::simpleParseTest()
{
    QString const kmlContent(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Document>"
        "   <Camera id=\"myCam\">"
        "     <longitude>1</longitude>"
        "     <latitude>2</latitude>"
        "     <altitude>3</altitude>"
        "     <heading>4</heading>"
        "     <tilt>5</tilt>"
        "     <roll>6</roll>"
        "     <altitudeMode>relativeToGround</altitudeMode>"
        "   </Camera>"
        "</Document>"
        "</kml>");

    GeoDataDocument *dataDocument = parseKml(kmlContent);
    auto camera = dynamic_cast<GeoDataCamera *>(dataDocument->abstractView());

    QVERIFY(camera != nullptr);

    GeoDataCoordinates::Unit const degree = GeoDataCoordinates::Degree;
    QCOMPARE(camera->longitude(degree), 1.0);
    QCOMPARE(camera->latitude(degree), 2.0);
    QCOMPARE(camera->altitude(), 3.0);
    QCOMPARE(camera->heading(), 4.0);
    QCOMPARE(camera->tilt(), 5.0);
    QCOMPARE(camera->roll(), 6.0);
    QCOMPARE(camera->altitudeMode(), RelativeToGround);

    delete dataDocument;
}

QTEST_MAIN(TestCamera)

#include "TestCamera.moc"
