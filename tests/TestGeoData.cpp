//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#include <QtTest/QTest>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include "GeoDataCoordinates.h"

namespace Marble
{

class TestGeoData : public QObject
{
    Q_OBJECT
 private slots:
    void normalizeLatTest();
    void normalizeLonTest();
};

void TestGeoData::normalizeLatTest()
{
    QList<qreal> list;
    list.append( M_PI / 2 );
    list.append( -M_PI / 2 );
    list.append( 1 );

    foreach( qreal latRadian, list ) {
        qreal latDegree = RAD2DEG * latRadian;
        for ( int i = 1; i < 10; ++i ) {
            if ( ( i % 2 ) == 0 ) {
                QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI, GeoDataCoordinates::Radian ), latRadian );
                QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI ), latRadian );
                QCOMPARE( GeoDataCoordinates::normalizeLat( latDegree + i * 180, GeoDataCoordinates::Degree ), latDegree );
            }
            else {
                QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI, GeoDataCoordinates::Radian ), -latRadian );
                QCOMPARE( GeoDataCoordinates::normalizeLat( latRadian + i * M_PI ), -latRadian );
                QCOMPARE( GeoDataCoordinates::normalizeLat( latDegree + i * 180, GeoDataCoordinates::Degree ), -latDegree );
            }
        }
    }
}

void TestGeoData::normalizeLonTest()
{
    QList<qreal> list;
    list.append( M_PI / 2 );
    list.append( -M_PI / 2 );
    list.append( 1 );
    list.append( M_PI );
    list.append( -M_PI );

    foreach( qreal lonRadian, list ) {
        qreal lonDegree = RAD2DEG * lonRadian;
        for ( int i = 1; i < 10; ++i ) {
            if ( lonRadian == M_PI || lonRadian == -M_PI ) {
                int lonRadianLarge = qRound( lonRadian * 1000 );
                int lonDegreeLarge = qRound( lonDegree * 1000 );
                if ( qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI ) * 1000 ) != lonRadianLarge
                     && qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI ) * 1000 ) != -lonRadianLarge )
                {
                    QFAIL( "Error at M_PI/-M_PI" );
                }
                if ( qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI, GeoDataCoordinates::Radian ) * 1000 ) != lonRadianLarge
                     && qRound( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI, GeoDataCoordinates::Radian ) * 1000 ) != -lonRadianLarge )
                {
                    QFAIL( "Error at M_PI/-M_PI" );
                }
                if ( qRound( GeoDataCoordinates::normalizeLon( lonDegree + i * 360, GeoDataCoordinates::Degree ) * 1000 ) != lonDegreeLarge
                     && qRound( GeoDataCoordinates::normalizeLon( lonDegree + i * 360, GeoDataCoordinates::Degree ) * 1000 ) != -lonDegreeLarge )
                {
                    QFAIL( "Error at M_PI/-M_PI" );
                }
            }
            else {
                QCOMPARE( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI, GeoDataCoordinates::Radian ), lonRadian );
                QCOMPARE( GeoDataCoordinates::normalizeLon( lonRadian + i * 2 * M_PI ), lonRadian );
                QCOMPARE( GeoDataCoordinates::normalizeLon( lonDegree + i * 360, GeoDataCoordinates::Degree ), lonDegree );
            }
        }
    }
}

}

QTEST_MAIN( Marble::TestGeoData )

#include "TestGeoData.moc"
