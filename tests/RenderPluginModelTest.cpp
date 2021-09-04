//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "RenderPluginModel.h"
#include "MarbleMap.h"
#include "TestUtils.h"

namespace Marble
{

class RenderPluginModelTest : public QObject
{
    Q_OBJECT

 public:
    RenderPluginModelTest() :
        m_map( nullptr )
    {}

 private Q_SLOTS:
    void initTestCase() { m_map = new MarbleMap(); }
    void cleanupTestCase() { delete m_map; }

    void construct();
    void setRenderPlugins();

 private:
    MarbleMap *m_map;
};

void RenderPluginModelTest::construct()
{
    const RenderPluginModel model;

    QCOMPARE( model.rowCount(), 0 );
}

void RenderPluginModelTest::setRenderPlugins()
{
    RenderPluginModel model;
    model.setRenderPlugins( m_map->renderPlugins() );

    QCOMPARE( model.rowCount(), m_map->renderPlugins().count() );
}

}

QTEST_MAIN( Marble::RenderPluginModelTest )

#include "RenderPluginModelTest.moc"
