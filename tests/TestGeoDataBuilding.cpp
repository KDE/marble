//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include <QTest>
#include <QVector>

#include "GeoDataBuilding.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataLinearRing.h"

namespace Marble {

class TestGeoDataBuilding : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor();
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

}

QTEST_MAIN(Marble::TestGeoDataBuilding)

#include "TestGeoDataBuilding.moc"
