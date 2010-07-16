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
#include "GeoDataPlacemark.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"

namespace Marble
{

class TestGeoData : public QObject
{
    Q_OBJECT
 private slots:
    void nodeTypeTest();
    void parentingTest();
    void normalizeLatTest_data();
    void normalizeLatTest();
    void normalizeLonTest_data();
    void normalizeLonTest();
    
    void latLonAltBoxIntersects_data();
    void latLonAltBoxIntersects();
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

void TestGeoData::parentingTest()
{
    GeoDataDocument *document = new GeoDataDocument;
    GeoDataFolder *folder = new GeoDataFolder;

    /// simple parenting test
    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setParent(document);
    QCOMPARE(placemark->parent(), document);

    /// simple append and child count test
    document->append(placemark);

    /// appending folder to document before feeding folder
    document->append(folder);
    QCOMPARE(document->size(), 2);

    GeoDataPlacemark *placemark2 = new GeoDataPlacemark;
    folder->append(placemark2);
    QCOMPARE(folder->size(), 1);


    /// retrieve child and check it matches placemark
    GeoDataPlacemark *placemarkPtr;
    QCOMPARE(document->child(0)->nodeType(), placemark->nodeType());
    placemarkPtr = static_cast<GeoDataPlacemark*>(document->child(0));
    QCOMPARE(placemarkPtr, placemark);

    /// check retrieved placemark matches intented child
    int position = document->childPosition(placemarkPtr);
    QCOMPARE(position, 0);

    /// retrieve child two and check it matches folder
    GeoDataFolder *folderPtr;
    QCOMPARE(document->child(1)->nodeType(), folder->nodeType());
    folderPtr = static_cast<GeoDataFolder*>(document->child(1));
    QCOMPARE(folderPtr, folder);

    /// check retrieved folder matches intended child
    position = document->childPosition(folderPtr);
    QCOMPARE(position, 1);

    /// retrieve child three and check it matches placemark
    QCOMPARE(folderPtr->size(), 1);
    placemarkPtr = static_cast<GeoDataPlacemark*>(folderPtr->child(0));
    QCOMPARE(placemarkPtr->nodeType(), placemark2->nodeType());
    QCOMPARE(placemarkPtr, placemark2);


    /// check retrieved placemark matches intended child
    QCOMPARE(folderPtr->childPosition(placemarkPtr), 0);
}

void TestGeoData::normalizeLatTest_data()
{
    QTest::addColumn<qreal>( "latRadian" );

    QTest::newRow( "north pole" ) << M_PI / 2;
    QTest::newRow( "south pole" ) << - M_PI / 2;
    QTest::newRow( "somewhere" ) << 1.0;
}

void TestGeoData::normalizeLatTest()
{
    QFETCH( qreal, latRadian );

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

void TestGeoData::normalizeLonTest_data()
{
    QTest::addColumn<qreal>( "lonRadian" );

    QTest::newRow( "half east" ) << M_PI / 2;
    QTest::newRow( "half west" ) << - M_PI / 2;
    QTest::newRow( "somewhere" ) << 1.0;
    QTest::newRow( "date line east" ) << M_PI;
    QTest::newRow( "date line west" ) << - M_PI;
}

void TestGeoData::normalizeLonTest()
{
    QFETCH( qreal, lonRadian );

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

void TestGeoData::latLonAltBoxIntersects_data()
{
    QTest::addColumn<qreal>( "box1north" );
    QTest::addColumn<qreal>( "box1south" );
    QTest::addColumn<qreal>( "box1west" );
    QTest::addColumn<qreal>( "box1east" );
    QTest::addColumn<qreal>( "box1minAltitude" );
    QTest::addColumn<qreal>( "box1maxAltitude" );
    QTest::addColumn<qreal>( "box2north" );
    QTest::addColumn<qreal>( "box2south" );
    QTest::addColumn<qreal>( "box2west" );
    QTest::addColumn<qreal>( "box2east" );
    QTest::addColumn<qreal>( "box2minAltitude" );
    QTest::addColumn<qreal>( "box2maxAltitude" );
    QTest::addColumn<bool>( "intersects" );
    
    QTest::newRow( "same" ) << 56.0 << 40.0 << 0.0 << 11.0 << 10.0 << 12.0
                            << 56.0 << 40.0 << 0.0 << 11.0 << 10.0 << 12.0
                            << true;
    QTest::newRow( "dateLineFalse" ) << 30.0 << -30.0 << -170.0 << 170.0 << 0.0 << 0.0
                                     << 30.0 << -30.0 << 171.0 << -171.0 << 0.0 << 0.0
                                     << false;
    QTest::newRow( "dateLineTrue" ) << 20.0 << 0.0 << -171.0 << 171.0 << 0.0 << 0.0
                                    << 30.0 << -30.0 << 170.0 << -170.0 << 0.0 << 0.0
                                    << true;
}

void TestGeoData::latLonAltBoxIntersects()
{
    QFETCH( qreal, box1north );
    QFETCH( qreal, box1south );
    QFETCH( qreal, box1west );
    QFETCH( qreal, box1east );
    QFETCH( qreal, box1minAltitude );
    QFETCH( qreal, box1maxAltitude );
    QFETCH( qreal, box2north );
    QFETCH( qreal, box2south );
    QFETCH( qreal, box2west );
    QFETCH( qreal, box2east );
    QFETCH( qreal, box2minAltitude );
    QFETCH( qreal, box2maxAltitude );
    QFETCH( bool, intersects );
    
    GeoDataLatLonAltBox box1;
    GeoDataLatLonAltBox box2;
    box1.setNorth( box1north, GeoDataCoordinates::Degree );
    box1.setSouth( box1south, GeoDataCoordinates::Degree );
    box1.setWest( box1west, GeoDataCoordinates::Degree );
    box1.setEast( box1east, GeoDataCoordinates::Degree );
    box1.setMinAltitude( box1minAltitude );
    box1.setMaxAltitude( box1maxAltitude );
    box2.setNorth( box2north, GeoDataCoordinates::Degree );
    box2.setSouth( box2south, GeoDataCoordinates::Degree );
    box2.setWest( box2west, GeoDataCoordinates::Degree );
    box2.setEast( box2east, GeoDataCoordinates::Degree );
    box2.setMinAltitude( box2minAltitude );
    box2.setMaxAltitude( box2maxAltitude );
    QCOMPARE( box1.intersects( box2 ), intersects );
}

}

QTEST_MAIN( Marble::TestGeoData )

#include "TestGeoData.moc"
