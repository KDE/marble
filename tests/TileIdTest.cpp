//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>


#include <QtCore/QMetaType>
#include <QtTest/QtTest>
#include "TileId.h"
#include "GeoDataCoordinates.h"

namespace QTest
{

}

#define addRow() QTest::newRow( QString("line %1").arg( __LINE__ ).toAscii().data() )

namespace Marble
{

class TileIdTest : public QObject
{
    Q_OBJECT

 private slots:
    void testFromCoordinates_data();
    void testFromCoordinates();

};


void TileIdTest::testFromCoordinates_data()
{
    QTest::addColumn<qreal>( "lon" );
    QTest::addColumn<qreal>( "lat" );
    QTest::addColumn<int>( "zoom" );
    QTest::addColumn<int>( "x" );
    QTest::addColumn<int>( "y" );

    for ( int zoom = 1; zoom < 19; ++zoom) {
        int last = (2 << (zoom-1)) -1;
        int quarter = qRound(last/4.0);
        int mid = qRound(last/2.0);
        int three = mid + quarter;

        addRow() << -180.0 << 0.0 << zoom << 0 << mid;
        addRow() <<  -90.0 << 0.0 << zoom << quarter << mid;
        addRow() <<    0.0 << 0.0 << zoom << mid << mid;
        addRow() <<   90.0 << 0.0 << zoom << three << mid;
        addRow() <<  180.0 << 0.0 << zoom << last << mid;

        addRow() << -180.0 << -89.9999 << zoom << 0 << last;
        addRow() <<  -90.0 << -89.9999 << zoom << quarter << last;
        addRow() <<    0.0 << -89.9999 << zoom << mid << last;
        addRow() <<   90.0 << -89.9999 << zoom << three << last;
        addRow() <<  180.0 << -89.9999 << zoom << last << last;

        addRow() << -180.0 << 89.9999 << zoom << 0 << 0;
        addRow() <<  -90.0 << 89.9999 << zoom << quarter << 0;
        addRow() <<    0.0 << 89.9999 << zoom << mid << 0;
        addRow() <<   90.0 << 89.9999 << zoom << three << 0;
        addRow() <<  180.0 << 89.9999 << zoom << last << 0;

        addRow() << -180.0 << -179.9999 << zoom << 0 << mid;
        addRow() <<  -90.0 << -179.9999 << zoom << quarter << mid;
        addRow() <<    0.0 << -179.9999 << zoom << mid << mid;
        addRow() <<   90.0 << -179.9999 << zoom << three << mid;
        addRow() <<  180.0 << -179.9999 << zoom << last << mid;

        addRow() << -180.0 << 179.9999 << zoom << 0 << mid-1;
        addRow() <<  -90.0 << 179.9999 << zoom << quarter << mid-1;
        addRow() <<    0.0 << 179.9999 << zoom << mid << mid-1;
        addRow() <<   90.0 << 179.9999 << zoom << three << mid-1;
        addRow() <<  180.0 << 179.9999 << zoom << last << mid-1;
    }
}

void TileIdTest::testFromCoordinates()
{
    QFETCH( qreal, lon );
    QFETCH( qreal, lat );
    QFETCH( int, zoom);
    QFETCH( int, x);
    QFETCH( int, y);

    const TileId tile = TileId::fromCoordinates(GeoDataCoordinates( lon , lat, GeoDataCoordinates::Degree), zoom );
    qDebug() << "zoom " << zoom;

    QCOMPARE( tile.x(), x );
    QCOMPARE( tile.y(), y );
}

}

QTEST_MAIN( Marble::TileIdTest )

#include "TileIdTest.moc"
