//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest/QtTest>

#include "FrameGraphicsItem.h"

namespace Marble
{

class FrameGraphicsItemTest : public QObject
{
    Q_OBJECT

 private slots:
    void constructorDefaultValues();
};

void FrameGraphicsItemTest::constructorDefaultValues()
{
    const FrameGraphicsItem item;

    QCOMPARE( item.position(), QPointF( 0, 0 ) );
    QCOMPARE( item.frame(), FrameGraphicsItem::NoFrame );
    QCOMPARE( item.margin(), 0. );
    QCOMPARE( item.marginTop(), 0. );
    QCOMPARE( item.marginBottom(), 0. );
    QCOMPARE( item.marginLeft(), 0. );
    QCOMPARE( item.marginRight(), 0. );
    QCOMPARE( item.borderWidth(), 1. );
    QCOMPARE( item.padding(), 0. );
    QCOMPARE( item.contentSize(), QSizeF( 0, 0 ) );
    QCOMPARE( item.contentRect(), QRectF( QPointF( 0, 0 ), QSizeF( 0, 0 ) ) );
    QCOMPARE( item.paintedRect(), QRectF( QPointF( 0, 0 ), QSizeF( 1, 1 ) ) );
    QCOMPARE( item.borderBrush(), QBrush( Qt::black ) );
    QCOMPARE( item.borderStyle(), Qt::SolidLine );
    QCOMPARE( item.background(), QBrush( QColor( 192, 192, 192, 192 ) ) );
}

}

QTEST_MAIN( Marble::FrameGraphicsItemTest )

#include "FrameGraphicsItemTest.moc"
