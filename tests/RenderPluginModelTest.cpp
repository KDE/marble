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

#include "RenderPluginModel.h"
#include "MarbleWidget.h"

#define addRow() QTest::newRow( QString("line %1").arg( __LINE__ ).toAscii().data() )

namespace Marble
{

class RenderPluginModelTest : public QObject
{
    Q_OBJECT

 public:
    RenderPluginModelTest() :
        m_widget( 0 )
    {}

 private slots:
    void initTestCase() { m_widget = new MarbleWidget(); }
    void cleanupTestCase() { delete m_widget; }

    void construct();

 private:
    MarbleWidget *m_widget;
};

void RenderPluginModelTest::construct()
{
    const RenderPluginModel model( m_widget );

    QCOMPARE( model.rowCount(), m_widget->renderPlugins().count() );
}

}

QTEST_MAIN( Marble::RenderPluginModelTest )

#include "RenderPluginModelTest.moc"
