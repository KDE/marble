//
// This file is part of the Marble Virtual Globe.
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
#include "MarbleDebug.h"

namespace Marble
{

class TestGeoData : public QObject
{
    Q_OBJECT
 private slots:
    void nodeTypeTest();
    void parentingTest();
};

/// test the nodeType function through various construction tests
void TestGeoData::nodeTypeTest()
{
    /// basic testing of nodeType
    GeoDataFolder *folder = new GeoDataFolder;
    const char* folderType = GeoDataTypes::GeoDataFolderType;
    QCOMPARE( folder->nodeType(), folderType );

    /// testing the nodeType of an object appended to a container
    GeoDataDocument document;
    document.append( folder );
    GeoDataFeature &featureRef = document.last();
    QCOMPARE( featureRef.nodeType(), folderType );
    QCOMPARE( static_cast<GeoDataObject*>(&featureRef)->nodeType(), folderType );
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

}

QTEST_MAIN( Marble::TestGeoData )

#include "TestGeoData.moc"
