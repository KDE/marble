// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTypes.h"
#include "MarbleDebug.h"
#include <QDebug>
#include <QTest>

namespace Marble
{

class TestGeoData : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void nodeTypeTest();
    void parentingTest();
    void testCast();
};

/// test the nodeType function through various construction tests
void TestGeoData::nodeTypeTest()
{
    /// basic testing of nodeType
    auto folder = new GeoDataFolder;
    const char *folderType = GeoDataTypes::GeoDataFolderType;
    QCOMPARE(folder->nodeType(), folderType);

    /// testing the nodeType of an object appended to a container
    GeoDataDocument document;
    document.append(folder);
    GeoDataFeature &featureRef = document.last();
    QVERIFY(geodata_cast<GeoDataFolder>(&featureRef));
}

void TestGeoData::parentingTest()
{
    auto document = new GeoDataDocument;
    auto folder = new GeoDataFolder;

    /// simple parenting test
    auto placemark = new GeoDataPlacemark;
    placemark->setParent(document);
    QCOMPARE(placemark->parent(), document);

    /// simple append and child count test
    document->append(placemark);

    /// appending folder to document before feeding folder
    document->append(folder);
    QCOMPARE(document->size(), 2);

    auto placemark2 = new GeoDataPlacemark;
    folder->append(placemark2);
    QCOMPARE(folder->size(), 1);

    /// retrieve child and check it matches placemark
    GeoDataPlacemark *placemarkPtr;
    QVERIFY(geodata_cast<GeoDataPlacemark>(document->child(0)));
    placemarkPtr = static_cast<GeoDataPlacemark *>(document->child(0));
    QCOMPARE(placemarkPtr, placemark);

    /// check retrieved placemark matches intented child
    int position = document->childPosition(placemarkPtr);
    QCOMPARE(position, 0);

    /// retrieve child two and check it matches folder
    GeoDataFolder *folderPtr;
    QVERIFY(geodata_cast<GeoDataFolder>(document->child(1)));
    folderPtr = static_cast<GeoDataFolder *>(document->child(1));
    QCOMPARE(folderPtr, folder);

    /// check retrieved folder matches intended child
    position = document->childPosition(folderPtr);
    QCOMPARE(position, 1);

    /// retrieve child three and check it matches placemark
    QCOMPARE(folderPtr->size(), 1);
    placemarkPtr = static_cast<GeoDataPlacemark *>(folderPtr->child(0));
    QCOMPARE(placemarkPtr->nodeType(), placemark2->nodeType());
    QCOMPARE(placemarkPtr, placemark2);

    /// check retrieved placemark matches intended child
    QCOMPARE(folderPtr->childPosition(placemarkPtr), 0);

    /// Set a style
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath("myicon.png");
    GeoDataStyle::Ptr style(new GeoDataStyle);
    style->setId("mystyle");
    style->setIconStyle(iconStyle);
    GeoDataObject *noParent = nullptr;
    QCOMPARE(style->parent(), noParent);
    QCOMPARE(iconStyle.parent(), noParent);
    document->setStyle(style);
    QCOMPARE(style->parent(), document); // Parent should be assigned now
    QCOMPARE(style->iconStyle().parent(), style.data());
    QCOMPARE(iconStyle.parent(), noParent); // setIconStyle copies
    QCOMPARE(placemark->style()->parent(), noParent);
    placemark->setStyle(style);
    QCOMPARE(placemark->style()->parent(), placemark); // Parent should be assigned now

    /// Set a style map
    auto styleMap = new GeoDataStyleMap;
    styleMap->setId("mystylemap");
    styleMap->insert("normal", "#mystyle");
    styleMap->insert("highlight", "#mystyle");
    document->addStyle(style);
    document->setStyleMap(styleMap);
    QCOMPARE(placemark2->style()->parent(), noParent);
    placemark2->setStyleUrl("#mystyle");
    QCOMPARE(placemark2->style()->parent(), document); // Parent is document, not placemark2
    QCOMPARE(iconStyle.iconPath(), QString("myicon.png"));
    QCOMPARE(placemark2->style()->iconStyle().iconPath(), QString("myicon.png"));
}

void TestGeoData::testCast()
{
    GeoDataLineString obj;
    GeoDataObject *base = &obj;
    QCOMPARE(geodata_cast<GeoDataObject>(base), nullptr);
    QCOMPARE(geodata_cast<GeoDataGeometry>(base), nullptr);
    QCOMPARE(geodata_cast<GeoDataLineString>(base), &obj);
    QCOMPARE(geodata_cast<GeoDataLinearRing>(base), nullptr);
    QCOMPARE(geodata_cast<GeoDataPlacemark>(base), nullptr);

    const GeoDataObject *cbase = &obj;
    QCOMPARE(geodata_cast<GeoDataObject>(cbase), nullptr);
    QCOMPARE(geodata_cast<GeoDataGeometry>(cbase), nullptr);
    QCOMPARE(geodata_cast<GeoDataLineString>(cbase), &obj);
    QCOMPARE(geodata_cast<GeoDataLinearRing>(cbase), nullptr);
    QCOMPARE(geodata_cast<GeoDataPlacemark>(cbase), nullptr);

    QCOMPARE(geodata_cast<GeoDataPlacemark>(static_cast<GeoDataObject *>(nullptr)), nullptr);
}

}

QTEST_MAIN(Marble::TestGeoData)

#include "TestGeoData.moc"
