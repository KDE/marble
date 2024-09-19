// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "FrameGraphicsItem.h"

#include <QBrush>
#include <QTest>

namespace Marble
{

class FrameGraphicsItemTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void constructorDefaultValues();
};

void FrameGraphicsItemTest::constructorDefaultValues()
{
    const FrameGraphicsItem item;

    QCOMPARE(item.position(), QPointF(0, 0));
    QCOMPARE(item.frame(), FrameGraphicsItem::NoFrame);
    QCOMPARE(item.margin(), 0.);
    QCOMPARE(item.marginTop(), 0.);
    QCOMPARE(item.marginBottom(), 0.);
    QCOMPARE(item.marginLeft(), 0.);
    QCOMPARE(item.marginRight(), 0.);
    QCOMPARE(item.borderWidth(), 1.);
    QCOMPARE(item.padding(), 0.);
    QCOMPARE(item.contentSize(), QSizeF(0, 0));
    QCOMPARE(item.contentRect(), QRectF(QPointF(0, 0), QSizeF(0, 0)));
    QCOMPARE(item.paintedRect(), QRectF(QPointF(0, 0), QSizeF(1, 1)));
    QCOMPARE(item.borderBrush(), QBrush(Qt::black));
    QCOMPARE(item.borderStyle(), Qt::SolidLine);
    QCOMPARE(item.background(), QBrush(QColor(192, 192, 192, 192)));
}

}

QTEST_MAIN(Marble::FrameGraphicsItemTest)

#include "FrameGraphicsItemTest.moc"
