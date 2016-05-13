//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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
        m_map( 0 )
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
