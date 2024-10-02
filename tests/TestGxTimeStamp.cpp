// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include <QObject>

#include "TestUtils.h"
#include <GeoDataCamera.h>
#include <GeoDataDocument.h>
#include <GeoDataPlacemark.h>
#include <GeoDataTimeStamp.h>
#include <MarbleDebug.h>

using namespace Marble;

class TestGxTimeStamp : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestGxTimeStamp::initTestCase()
{
    MarbleDebug::setEnabled(true);
}

void TestGxTimeStamp::simpleParseTest()
{
    QString const centerContent(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Document>"
        "<Placemark>"
        "<Camera>"
        "<gx:TimeStamp>"
        "<when>1987-06-05T04:03:02-01:00</when>"
        "</gx:TimeStamp>"
        "</Camera>"
        "</Placemark>"
        "</Document>"
        "</kml>");

    GeoDataDocument *dataDocument = parseKml(centerContent);
    QCOMPARE(dataDocument->placemarkList().size(), 1);
    GeoDataPlacemark *placemark = dataDocument->placemarkList().at(0);
    GeoDataAbstractView *view = placemark->abstractView();
    QVERIFY(view != nullptr);
    auto camera = dynamic_cast<GeoDataCamera *>(view);
    QVERIFY(camera != nullptr);
    QCOMPARE(camera->timeStamp().when().toUTC(), QDateTime::fromString("1987-06-05T04:03:02-01:00", Qt::ISODate).toUTC());

    delete dataDocument;
}

QTEST_MAIN(TestGxTimeStamp)

#include "TestGxTimeStamp.moc"
