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

#include "MarbleModel.h"
#include "PluginManager.h"
#include "AbstractDataPlugin.h"

Q_DECLARE_METATYPE( const Marble::AbstractDataPlugin * )

namespace Marble
{

class AbstractDataPluginTest : public QObject
{
    Q_OBJECT

 private slots:
    void initialize_data();
    void initialize();

 private:
    MarbleModel m_model;
};

void AbstractDataPluginTest::initialize_data()
{
    QTest::addColumn<const AbstractDataPlugin *>( "factory" );

    foreach ( const RenderPlugin *plugin, m_model.pluginManager()->renderPlugins() ) {
        const AbstractDataPlugin *const dataPlugin = qobject_cast<const AbstractDataPlugin *>( plugin );
        if ( !dataPlugin )
            continue;

        QTest::newRow( plugin->nameId().toAscii() ) << dataPlugin;
    }
}

void AbstractDataPluginTest::initialize()
{
    QFETCH( const AbstractDataPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );
    AbstractDataPlugin *const dataInstance = qobject_cast<AbstractDataPlugin *>( instance );

    QVERIFY( dataInstance != 0 );
    QVERIFY( dataInstance->model() == 0 );

    dataInstance->initialize();

    QVERIFY( dataInstance->model() != 0 );
}

}

QTEST_MAIN( Marble::AbstractDataPluginTest )

#include "AbstractDataPluginTest.moc"
