//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest/QtTest>

#include "BillboardGraphicsItem.h"

namespace Marble
{

class BillboardGraphicsItemTest : public QObject
{
    Q_OBJECT

 private slots:
    void defaultConstructorValues();
};

void BillboardGraphicsItemTest::defaultConstructorValues()
{
    const BillboardGraphicsItem item;

    // MarbleGraphicsItem
    QCOMPARE( item.layout(), static_cast<AbstractMarbleGraphicsLayout *>( 0 ) );
    QCOMPARE( item.cacheMode(), BillboardGraphicsItem::NoCache );
    QCOMPARE( item.visible(), true );
    QCOMPARE( item.size(), QSizeF() );
    QCOMPARE( item.contentSize(), QSizeF() );

    // BillboardGraphicsItem
    QCOMPARE( item.coordinate(), GeoDataCoordinates() );
    QCOMPARE( item.alignment(), Qt::AlignHCenter | Qt::AlignVCenter );
    QCOMPARE( item.positions(), QList<QPointF>() );
    QCOMPARE( item.boundingRects(), QList<QRectF>() );
}

}

QTEST_MAIN( Marble::BillboardGraphicsItemTest )

#include "BillboardGraphicsItemTest.moc"
