// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "MarbleModel.h"
#include "PluginManager.h"
#include "AbstractDataPlugin.h"

#include <QMetaType>
#include <QTest>

Q_DECLARE_METATYPE( const Marble::AbstractDataPlugin * )

namespace Marble
{

class AbstractDataPluginTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initialize_data();
    void initialize();

 private:
    MarbleModel m_model;
};

void AbstractDataPluginTest::initialize_data()
{
    QTest::addColumn<const AbstractDataPlugin *>( "factory" );

    const auto plugins = m_model.pluginManager()->renderPlugins();
    for (const RenderPlugin *plugin: plugins) {
        const AbstractDataPlugin *const dataPlugin = qobject_cast<const AbstractDataPlugin *>( plugin );
        if ( !dataPlugin )
            continue;

        QTest::newRow(plugin->nameId().toLatin1().constData()) << dataPlugin;
    }
}

void AbstractDataPluginTest::initialize()
{
    QFETCH( const AbstractDataPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );
    AbstractDataPlugin *const dataInstance = qobject_cast<AbstractDataPlugin *>( instance );

    QVERIFY( dataInstance != nullptr );
    QVERIFY( dataInstance->model() == nullptr );

    dataInstance->initialize();

    QVERIFY( dataInstance->model() != nullptr );
}

}

QTEST_MAIN( Marble::AbstractDataPluginTest )

#include "AbstractDataPluginTest.moc"
