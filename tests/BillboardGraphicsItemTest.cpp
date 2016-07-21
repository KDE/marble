//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "BillboardGraphicsItem.h"

#include "FrameGraphicsItem.h"
#include "MarbleGraphicsGridLayout.h"
#include "ViewportParams.h"

#include <QImage>
#include <QPainter>
#include <QTest>

namespace Marble
{

class BillboardGraphicsItemTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructorValues();

    void paintEvent();
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
    QCOMPARE( item.positions(), QVector<QPointF>() );
    QCOMPARE( item.boundingRects(), QVector<QRectF>() );
}

void BillboardGraphicsItemTest::paintEvent()
{
    BillboardGraphicsItem item;
    item.setCoordinate( GeoDataCoordinates( 0, 0 ) );

    MarbleGraphicsGridLayout *topLayout = new MarbleGraphicsGridLayout( 1, 1 );
    item.setLayout( topLayout );

    FrameGraphicsItem frameItem( &item );
    frameItem.setSize( QSizeF( 11.2, 11.3 ) );
    topLayout->addItem( &frameItem, 0, 0 );

    QCOMPARE( item.positions(), QVector<QPointF>() );
    QCOMPARE( item.size(), QSizeF() );

    QImage paintDevice( 100, 100, QImage::Format_ARGB32_Premultiplied );
    QPainter painter( &paintDevice );

    const ViewportParams viewport( Mercator, 0, 0, 20, paintDevice.size() );

    item.paintEvent( &painter, &viewport );

    QCOMPARE( item.size(), frameItem.size() );
    QCOMPARE( item.positions().size(), 1 );
    QCOMPARE( item.positions()[0], QPointF( 44, 44 ) );
}

}

QTEST_MAIN( Marble::BillboardGraphicsItemTest )

#include "BillboardGraphicsItemTest.moc"
