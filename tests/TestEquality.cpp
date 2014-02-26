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
    void aliasTest();
    void accuracyTest();
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
    QCOMPARE( alias != alias2, true );

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
    QCOMPARE( accuracy1, accuracy2 );

    accuracy1.vertical = 1;
    QCOMPARE( accuracy1 != accuracy2, true );

    accuracy1.horizontal = 1;
    accuracy2.horizontal = 1;

    QCOMPARE( accuracy1 == accuracy2, false );
}

QTEST_MAIN( TestEquality )

#include "TestEquality.moc"
