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
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

class TestGeoData : public QObject
{
    Q_OBJECT
 private slots:
    void nodeTypeTest();
    void normalizeLatTest();
    void normalizeLonTest();
};

/// test the nodeType function through various construction tests
void TestGeoData::nodeTypeTest()
{
    /// basic testing of nodeType
    GeoDataFolder folder;
    QString folderType(GeoDataTypes::GeoDataFolderType );
    QCOMPARE( folder.nodeType(), folderType );

    /// testing the nodeType of an object appended to a container
    GeoDataDocument document;
    document.append( folder );
    GeoDataFeature &featureRef = document.last();
    QCOMPARE( featureRef.nodeType(), folderType );
    QCOMPARE( static_cast<GeoDataObject*>(&featureRef)->nodeType(), folderType );

    /// testing the nodeType of an object assigned to a super type object
    GeoDataFeature featureAssign;
    featureAssign = folder;
    QCOMPARE( featureAssign.nodeType(), folderType);

    /// testing the nodeType of an object copied into a super type object
    GeoDataFeature featureCopy( folder );
    QCOMPARE( featureCopy.nodeType(), folderType );

    /// testing the nodeType of an object assigned to a non shared data object
    GeoDataObject objectAssign;
    objectAssign = folder;
    QCOMPARE( objectAssign.nodeType(), folderType );

    /// testing the nodeType of an object copied to a non shared data object
    GeoDataObject objectCopy(folder);
    QCOMPARE( objectCopy.nodeType(), folderType );
}


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
