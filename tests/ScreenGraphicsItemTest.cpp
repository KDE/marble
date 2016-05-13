//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "ScreenGraphicsItem.h"
#include "GeoGraphicsItem.h"

#include <QTest>

namespace Marble
{

class ScreenGraphicsItemTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructorValues();

    void constructorValuesScreenParent();

    void setLayout();
};

void ScreenGraphicsItemTest::defaultConstructorValues()
{
    const ScreenGraphicsItem item;

    QCOMPARE( item.layout(), static_cast<AbstractMarbleGraphicsLayout *>( 0 ) );
    QCOMPARE( item.cacheMode(), ScreenGraphicsItem::NoCache );
    QCOMPARE( item.visible(), true );
    QCOMPARE( item.size(), QSizeF() );
    QCOMPARE( item.contentSize(), QSizeF() );

    QCOMPARE( item.position(), QPointF( 0, 0 ) );
    QCOMPARE( item.positivePosition(), QPointF( 0, 0 ) );
    QCOMPARE( item.absolutePositions().size(), 1 );
    QCOMPARE( item.absolutePositions()[0], QPointF( 0, 0 ) );

    // invariants
    QCOMPARE( item.contentRect(), QRectF( item.position(), item.contentSize() ) );
}

void ScreenGraphicsItemTest::constructorValuesScreenParent()
{
    ScreenGraphicsItem screenParent;

    QVERIFY( screenParent.absolutePositions().size() == 1 );
    QVERIFY( screenParent.absolutePositions()[0] == QPointF( 0, 0 ) );

    const ScreenGraphicsItem item( &screenParent );

    QCOMPARE( item.layout(), static_cast<AbstractMarbleGraphicsLayout *>( 0 ) );
    QCOMPARE( item.cacheMode(), ScreenGraphicsItem::NoCache );
    QCOMPARE( item.visible(), true );
    QCOMPARE( item.size(), QSizeF() );
    QCOMPARE( item.contentSize(), QSizeF() );

    QCOMPARE( item.position(), QPointF( 0, 0 ) );
    QCOMPARE( item.positivePosition(), QPointF( 0, 0 ) );

    // invariants
    QCOMPARE( item.contentRect(), QRectF( item.position(), item.contentSize() ) );

    QCOMPARE( item.absolutePositions().size(), screenParent.absolutePositions().size() );
    QCOMPARE( item.absolutePositions(), screenParent.absolutePositions() );
}

void ScreenGraphicsItemTest::setLayout()
{
    ScreenGraphicsItem screenParent;

    QVERIFY( screenParent.absolutePositions().size() == 1 );
    QVERIFY( screenParent.absolutePositions()[0] == QPointF( 0, 0 ) );

    const ScreenGraphicsItem item( &screenParent );

    QCOMPARE( item.layout(), static_cast<AbstractMarbleGraphicsLayout *>( 0 ) );
    QCOMPARE( item.cacheMode(), ScreenGraphicsItem::NoCache );
    QCOMPARE( item.visible(), true );
    QCOMPARE( item.size(), QSizeF() );
    QCOMPARE( item.contentSize(), QSizeF() );

    QCOMPARE( item.position(), QPointF( 0, 0 ) );
    QCOMPARE( item.positivePosition(), QPointF( 0, 0 ) );

    // invariants
    QCOMPARE( item.contentRect(), QRectF( item.position(), item.contentSize() ) );

    QCOMPARE( item.absolutePositions().size(), screenParent.absolutePositions().size() );
    QCOMPARE( item.absolutePositions(), screenParent.absolutePositions() );
}

}

QTEST_MAIN( Marble::ScreenGraphicsItemTest )

#include "ScreenGraphicsItemTest.moc"
