//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin  <ps_ml@gmx.de>
//

#include <QtGui/QtGui>
#include <QtTest/QtTest>
#include <QtTest/QTestEvent>
#include "MarbleDirs.h"
#include "MarbleWidget.h"

namespace Marble
{

class TestMarbleWidget: public QObject
{
    Q_OBJECT

private slots:
    void testWidget();

};

void TestMarbleWidget::testWidget()
{
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );
    MarbleWidget widget;
    widget.setMapThemeId("earth/srtm/srtm.dgml");

    QTest::mouseMove( &widget );
}

}

QTEST_MAIN( Marble::TestMarbleWidget )

#include "MarbleWidgetTest.moc"
