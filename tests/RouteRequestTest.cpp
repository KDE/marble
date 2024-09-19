// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QTest>

#include "GeoDataPlacemark.h"
#include "routing/RouteRequest.h"

namespace Marble
{

class RouteRequestTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor();
    void reverse();
};

void RouteRequestTest::defaultConstructor()
{
    const RouteRequest request;

    QCOMPARE(request.size(), 0);
    QCOMPARE(request.source(), GeoDataCoordinates());
    QCOMPARE(request.destination(), GeoDataCoordinates());
    QCOMPARE(request.routingProfile(), RoutingProfile());
}

void RouteRequestTest::reverse()
{
    {
        RouteRequest request;
        request.reverse(); // should not crash
        QCOMPARE(request.size(), 0);
    }

    {
        RouteRequest request;
        request.append(GeoDataCoordinates(), "A");
        request.reverse();
        QCOMPARE(request.size(), 1);
        QCOMPARE(request[0].name(), QString("A"));
    }

    {
        RouteRequest request;
        request.append(GeoDataCoordinates(), "A");
        request.append(GeoDataCoordinates(), "B");
        request.reverse();
        QCOMPARE(request.size(), 2);
        QCOMPARE(request[0].name(), QString("B"));
        QCOMPARE(request[1].name(), QString("A"));
    }

    {
        RouteRequest request;
        request.append(GeoDataCoordinates(), "A");
        request.append(GeoDataCoordinates(), "B");
        request.append(GeoDataCoordinates(), "C");
        request.reverse();
        QCOMPARE(request.size(), 3);
        QCOMPARE(request[0].name(), QString("C"));
        QCOMPARE(request[1].name(), QString("B"));
        QCOMPARE(request[2].name(), QString("A"));
    }
}

}

QTEST_MAIN(Marble::RouteRequestTest)

#include "RouteRequestTest.moc"
