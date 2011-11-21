//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>

#include "MapViewWidget.h"

#define addRow() QTest::newRow( QString("line %1").arg( __LINE__ ).toAscii().data() )

namespace Marble
{

class MapViewWidgetTest : public QObject
{
    Q_OBJECT

 private slots:
    void initTestCase();

    void setMapThemeId();

    void setProjection();
};

void MapViewWidgetTest::initTestCase()
{
    qRegisterMetaType<Projection>( "Projection" );
}

void MapViewWidgetTest::setMapThemeId()
{
    MapViewWidget widget;
    QSignalSpy spy( &widget, SIGNAL( mapThemeIdChanged( const QString & ) ) );

    widget.setMapThemeId( "" );

    QCOMPARE( spy.count(), 0 );

    widget.setMapThemeId( "foo/bar/bar.dgml" );
}

void MapViewWidgetTest::setProjection()
{
    MapViewWidget widget;
    QSignalSpy spy( &widget, SIGNAL( projectionChanged( Projection ) ) );

    widget.setProjection( Spherical );

    QCOMPARE( spy.count(), 0 );

    widget.setProjection( Mercator );
}

}

QTEST_MAIN( Marble::MapViewWidgetTest )

#include "MapViewWidgetTest.moc"
