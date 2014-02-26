//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Cruceru Calin-Cristian <crucerucalincristian@gmail.com>
//

#include <QObject>

#include <MarbleDebug.h>
#include <GeoDataAlias.h>
#include <GeoDataAccuracy.h>
#include <GeoDataSimpleArrayData.h>
#include <GeoDataData.h>
#include <GeoDataExtendedData.h>
#include "TestUtils.h"

using namespace Marble;

class TestEquality : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void aliasTest();
    void accuracyTest();
    void simpleArrayDataTest();
    void dataTest();
    void extendedDataTest();
};


void TestEquality::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestEquality::aliasTest()
{
    GeoDataAlias alias;
    alias.setSourceHref("test1");
    alias.setTargetHref("test1");

    GeoDataAlias alias2;
    alias2.setSourceHref("test1");
    alias2.setTargetHref("test1");

    QCOMPARE( alias, alias );
    QCOMPARE( alias2, alias2 );
    QCOMPARE( alias, alias2 );

    alias.setSourceHref("test2");
    alias2.setSourceHref("Test2");
    QCOMPARE( alias == alias2, false );
    QVERIFY( alias != alias2 );

    alias.setSourceHref("test3");
    alias.setTargetHref("test3");
    alias2.setSourceHref("test3");
    alias2.setTargetHref("test2");
    QCOMPARE( alias == alias2, false );

    alias2.setSourceHref("Test3");
    QVERIFY( alias != alias2 );

    alias.setSourceHref("Test3");
    alias.setTargetHref("test2");
    QCOMPARE( alias, alias2 );
}

void TestEquality::accuracyTest()
{
    GeoDataAccuracy accuracy1, accuracy2;

    accuracy1.level = GeoDataAccuracy::Country;
    accuracy1.vertical = 2;
    accuracy1.horizontal = 3;
    accuracy2.level = GeoDataAccuracy::Country;
    accuracy2.vertical = 2;
    accuracy2.horizontal = 3;
    QCOMPARE( accuracy1, accuracy1 );
    QCOMPARE( accuracy2, accuracy2 );
    QCOMPARE( accuracy1, accuracy2 );

    accuracy1.vertical = 1;
    QVERIFY( accuracy1 != accuracy2 );

    accuracy1.horizontal = 1;
    accuracy2.horizontal = 1;

    QCOMPARE( accuracy1 == accuracy2, false );
}

void TestEquality::simpleArrayDataTest()
{
    GeoDataSimpleArrayData simpleArray1, simpleArray2;

    simpleArray1.append("Marble");
    simpleArray1.append(2014);
    simpleArray1.append("Globe");

    simpleArray2.append("Globe");
    simpleArray2.append(2014);
    simpleArray2.append("Marble");

    QCOMPARE( simpleArray1, simpleArray1 );
    QCOMPARE( simpleArray2, simpleArray2 );
    QCOMPARE( simpleArray1 == simpleArray2, false );
    QVERIFY( simpleArray1 != simpleArray2 );
}

void TestEquality::dataTest()
{
    GeoDataData data1, data2;

    data1.setName("Something");
    data1.setValue(QVariant(23.56));
    data1.setDisplayName("Marble");

    data2.setName("Something");
    data2.setValue(QVariant(23.56));
    data2.setDisplayName("Marble");

    QCOMPARE( data1, data1 );
    QCOMPARE( data2, data2 );
    QCOMPARE( data1, data2 );
    QCOMPARE( data1 != data2, false );

    data1.setName("Marble");
    data1.setDisplayName("Something");

    QCOMPARE( data1 == data2, false );
    QVERIFY( data1 != data2 );
}

void TestEquality::extendedDataTest()
{
    GeoDataExtendedData extendedData1, extendedData2;
    GeoDataData data1, data2;

    data1.setName("Something");
    data1.setValue(QVariant(23.56));
    data1.setDisplayName("Marble");

    data2.setName("Marble");
    data2.setValue(QVariant(23.56));
    data2.setDisplayName("Globe");

    extendedData1.addValue(data1);
    extendedData2.addValue(data2);

    QCOMPARE( extendedData1, extendedData1 );
    QCOMPARE( extendedData2, extendedData2 );
    QCOMPARE( extendedData1 == extendedData2, false );
    QVERIFY( extendedData1 != extendedData2 );
}

QTEST_MAIN( TestEquality )

#include "TestEquality.moc"
