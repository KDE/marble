//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin  <ps_ml@gmx.de>
//

#include <QtGui>
#include <QTestEvent>
#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "TestUtils.h"

#include "qtest_widgets.h"
#include "qtestmouse.h"

namespace Marble
{

class MarbleWidgetTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase(){}// will be called after the last testfunction was executed.
    void init(){}// will be called before each testfunction is executed.
    void cleanup(){}// will be called after every testfunction.

    void mouseMove();

    void setMapTheme_data();
    void setMapTheme();

    void switchMapThemes();

    void paintEvent_data();
    void paintEvent();

    void runMultipleWidgets();
};

void MarbleWidgetTest::initTestCase()
{
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );
}

void MarbleWidgetTest::mouseMove()
{
    MarbleWidget widget;
    widget.setMapThemeId("earth/srtm/srtm.dgml");

    QTest::mouseMove( &widget );

    QThreadPool::globalInstance()->waitForDone();  // wait for all runners to terminate
}

void MarbleWidgetTest::setMapTheme_data()
{
    QTest::addColumn<QString>( "mapThemeId" );

    addRow() << "earth/plain/plain.dgml";
    addRow() << "earth/srtm/srtm.dgml";
    addRow() << "earth/openstreetmap/openstreetmap.dgml";
}

void MarbleWidgetTest::setMapTheme()
{
    QFETCH( QString, mapThemeId );

    MarbleWidget widget;

    widget.setMapThemeId( mapThemeId );

    QCOMPARE( widget.mapThemeId(), mapThemeId );

    QThreadPool::globalInstance()->waitForDone();  // wait for all runners to terminate
}

void MarbleWidgetTest::switchMapThemes()
{
    MarbleWidget widget;

    widget.setMapThemeId( "earth/plain/plain.dgml" );
    QCOMPARE( widget.mapThemeId(), QString( "earth/plain/plain.dgml" ) );

    widget.setMapThemeId( "earth/srtm/srtm.dgml" );
    QCOMPARE( widget.mapThemeId(), QString( "earth/srtm/srtm.dgml" ) );

    widget.setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    QCOMPARE( widget.mapThemeId(), QString( "earth/openstreetmap/openstreetmap.dgml" ) );

    widget.setMapThemeId( "earth/plain/plain.dgml" );
    QCOMPARE( widget.mapThemeId(), QString( "earth/plain/plain.dgml" ) );

    QThreadPool::globalInstance()->waitForDone();  // wait for all runners to terminate
}

void MarbleWidgetTest::paintEvent_data()
{
    QTest::addColumn<QString>( "mapThemeId" );

    addRow() << "earth/plain/plain.dgml";
    addRow() << "earth/srtm/srtm.dgml";
    addRow() << "earth/openstreetmap/openstreetmap.dgml";
}

void MarbleWidgetTest::paintEvent()
{
    QFETCH( QString, mapThemeId );

    MarbleWidget widget;

    widget.setMapThemeId( mapThemeId );
    widget.resize( 200, 200 );

    QCOMPARE( widget.mapThemeId(), mapThemeId );

    widget.repaint();

    QThreadPool::globalInstance()->waitForDone();  // wait for all runners to terminate
}

void MarbleWidgetTest::runMultipleWidgets() {
    MarbleWidget widget1;
    MarbleWidget widget2;

    QCOMPARE(widget1.mapThemeId(), widget2.mapThemeId());
    QThreadPool::globalInstance()->waitForDone();
}

}

QTEST_MAIN( Marble::MarbleWidgetTest )

#include "MarbleWidgetTest.moc"
