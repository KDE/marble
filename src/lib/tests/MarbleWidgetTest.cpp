//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin  <ps_ml@gmx.de>
//

#include <QtGui/QtGui>
#include <QtTest/QtTest>
#include "MarbleWidget.h"

class TestMarbleWidget: public QObject
{
    Q_OBJECT

private slots:
    void testWidget();

};

void TestMarbleWidget::testWidget()
{
    MarbleWidget widget;
    widget.setMapThemeId("earth/srtm/srtm.dgml");

    QTest::mouseMove( &widget );
}

QTEST_MAIN( TestMarbleWidget )

#include "MarbleWidgetTest.moc"
