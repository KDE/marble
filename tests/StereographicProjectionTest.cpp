//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "StereographicProjection.h"
#include "ViewportParams.h"
#include "TestUtils.h"
#include <QDebug>

namespace Marble
{

class StereographicProjectionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void screenCoordinatesOfCenter_data();
    void screenCoordinatesOfCenter();
};

void StereographicProjectionTest::screenCoordinatesOfCenter_data()
{
    ViewportParams stereographic;
    stereographic.setProjection( Stereographic );

    QTest::addColumn<QPoint>( "screenCoordinates" );
    QTest::addColumn<GeoDataCoordinates>( "expected" );

    addRow() << QPoint( 5, 15 ) << GeoDataCoordinates( -45, 72.1397, 0, GeoDataCoordinates::Degree );
    addRow() << QPoint( 15, 5 ) << GeoDataCoordinates( 135, 72.1397, 0, GeoDataCoordinates::Degree );
}

void StereographicProjectionTest::screenCoordinatesOfCenter()
{
    QFETCH( QPoint, screenCoordinates );
    QFETCH( GeoDataCoordinates, expected );

    ViewportParams viewport;
    viewport.setProjection( Stereographic );
    viewport.setRadius( 180 / 4 ); // for easy mapping of lon <-> x
    viewport.setSize( QSize( 20, 20 ) );
    viewport.centerOn( 0 * DEG2RAD, 90 * DEG2RAD );

    {
        qreal lon, lat;
        const bool retval = viewport.geoCoordinates( screenCoordinates.x(), screenCoordinates.y(), lon, lat, GeoDataCoordinates::Degree );

        QVERIFY( retval ); // we want valid coordinates
        QFUZZYCOMPARE( lon, expected.longitude( GeoDataCoordinates::Degree ), 0.0001  );
        QFUZZYCOMPARE( lat, expected.latitude( GeoDataCoordinates::Degree ), 0.0001  );
    }
}

}

QTEST_MAIN( Marble::StereographicProjectionTest )

#include "StereographicProjectionTest.moc"
