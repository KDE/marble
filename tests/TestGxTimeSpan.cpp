// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include <QObject>

#include "TestUtils.h"
#include <GeoDataCamera.h>
#include <GeoDataDocument.h>
#include <GeoDataLookAt.h>
#include <GeoDataPlacemark.h>
#include <GeoDataTimeSpan.h>
#include <GeoDataTimeStamp.h>
#include <MarbleDebug.h>

using namespace Marble;

class TestGxTimeSpan : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void simpleParseTest();
};

void TestGxTimeSpan::initTestCase()
{
    MarbleDebug::setEnabled(true);
}

void TestGxTimeSpan::simpleParseTest()
{
    QString const centerContent(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\""
        " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">"
        "<Document>"
        "<Placemark>"
        "<LookAt>"
        "<gx:TimeSpan>"
        "<begin>2010-05-28T02:02:09Z</begin>"
        "<end>2010-05-28T02:02:56Z</end>"
        "</gx:TimeSpan>"
        "</LookAt>"
        "</Placemark>"
        "</Document>"
        "</kml>");

    GeoDataDocument *dataDocument = parseKml(centerContent);
    QCOMPARE(dataDocument->placemarkList().size(), 1);
    GeoDataPlacemark *placemark = dataDocument->placemarkList().at(0);
    QVERIFY(placemark->lookAt() != nullptr);
    QCOMPARE(placemark->lookAt()->timeSpan().begin().when(), QDateTime::fromString("2010-05-28T02:02:09Z", Qt::ISODate));
    QCOMPARE(placemark->lookAt()->timeSpan().end().when(), QDateTime::fromString("2010-05-28T02:02:56Z", Qt::ISODate));

    delete dataDocument;
}

QTEST_MAIN(TestGxTimeSpan)

#include "TestGxTimeSpan.moc"
