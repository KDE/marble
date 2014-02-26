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
#include "TestUtils.h"

using namespace Marble;

class TestEquality : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void simpleParseTest();
};


void TestEquality::initTestCase()
{
    MarbleDebug::setEnabled( true );
}

void TestEquality::simpleParseTest()
{
    GeoDataAlias test1;
    test1.setSourceHref("test1");
    test1.setTargetHref("test1");

    GeoDataAlias test1_other;
    test1_other.setSourceHref("test1");
    test1_other.setTargetHref("test1");

    /* Tests for GeoDataAlias */

    // Test 1
    bool rez = test1 == test1_other;
    QCOMPARE( rez, true );

    // Test 2
    test1.setSourceHref("test2");
    test1_other.setSourceHref("Test2");

    rez = test1 == test1_other;
    QCOMPARE( rez, false );

    // Test 3
    rez = test1 != test1_other;
    QCOMPARE( rez, true );

    // Test 4
    test1.setSourceHref("test3");
    test1.setTargetHref("test3");
    test1_other.setSourceHref("test3");
    test1_other.setTargetHref("test2");

    rez = test1 == test1_other;
    QCOMPARE( rez, false );

    // Test 5
    test1_other.setSourceHref("Test3");

    rez = test1 != test1_other;
    QCOMPARE( rez, true );

    // Test 6
    test1.setSourceHref("Test3");
    test1.setTargetHref("test2");

    rez = test1 == test1_other;
    QCOMPARE( rez, true );


    /* Tests for GeoDataAccuracy */
    GeoDataAccuracy test2, test2_other;

    // Test1
    test2.level = GeoDataAccuracy::Country;
    test2.vertical = 2;
    test2.horizontal = 3;
    test2_other.level = GeoDataAccuracy::Country;
    test2_other.vertical = 2;
    test2_other.horizontal = 3;

    rez = test2 == test2_other;
    QCOMPARE( rez, true );

    // Test2
    test2.vertical = 1;

    rez = test2 != test2_other;
    QCOMPARE( rez, true );

    // Test 3
    test2.horizontal = 1;
    test2_other.horizontal = 1;

    rez = test2 == test2_other;
    QCOMPARE( rez, false );
}

QTEST_MAIN( TestEquality )

#include "TestEquality.moc"
