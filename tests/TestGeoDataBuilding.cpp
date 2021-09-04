// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include <QTest>
#include "TestUtils.h"

#include "GeoDataBuilding.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataLinearRing.h"

namespace Marble {

class TestGeoDataBuilding : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor();
    void testHeightExtraction();
};

void TestGeoDataBuilding::defaultConstructor() {
    GeoDataBuilding building;

    QCOMPARE(building.height(), 0.0);
    QCOMPARE(building.minLevel(), 0);
    QCOMPARE(building.maxLevel(), 0);

    building.setHeight(24.5);
    building.setMinLevel(-2);
    building.setMaxLevel(10);

    QCOMPARE(building.height(), 24.5);
    QCOMPARE(building.minLevel(), -2);
    QCOMPARE(building.maxLevel(), 10);

    QVERIFY(building.nonExistentLevels().isEmpty());

    QVector<int> nonExistentLevels;
    nonExistentLevels << 4 << 13;
    building.setNonExistentLevels(nonExistentLevels);

    QVERIFY(!building.nonExistentLevels().isEmpty());

    QVERIFY(building.multiGeometry()->size() == 0);

    building.multiGeometry()->append(new GeoDataLinearRing);

    QVERIFY(building.multiGeometry()->size() > 0);

    GeoDataBuilding building2(building);

    QCOMPARE(building2.height(), 24.5);
    QCOMPARE(building2.minLevel(), -2);
    QCOMPARE(building2.maxLevel(), 10);
    QVERIFY(!building2.nonExistentLevels().isEmpty());
    QVERIFY(building2.multiGeometry()->size() > 0);
}

void TestGeoDataBuilding::testHeightExtraction()
{
    QString const meters1 = "12 m";
    QString const meters2 = "12.8 meters";
    QString const meters3 = "12.56 meter";
    QString const meters4 = "14.44 metres";
    QString const meters5 = "23.43 metre";

    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(meters1), 12.0, 0.0001);
    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(meters2), 12.8, 0.0001);
    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(meters3), 12.56, 0.0001);
    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(meters4), 14.44, 0.0001);
    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(meters5), 23.43, 0.0001);

    QString const feet1 = "55'4\""; // 664 inches
    QString const feet2 = "60.56 feet"; // 726.72 inches
    QString const feet3 = "300\'"; // 3600 inches

    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(feet1), 16.8656, 0.0001);
    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(feet2), 18.4587, 0.0001);
    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(feet3), 91.44, 0.0001);

    QString const unitless1 = "0.8"; // default in meters
    QString const unitless2 = "12"; // default in meters

    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(unitless1), 0.8, 0.0001);
    QFUZZYCOMPARE(GeoDataBuilding::parseBuildingHeight(unitless2), 12.0, 0.0001);
}

}

QTEST_MAIN(Marble::TestGeoDataBuilding)

#include "TestGeoDataBuilding.moc"
