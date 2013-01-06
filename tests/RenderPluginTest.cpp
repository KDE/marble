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
#include "RenderPlugin.h"

Q_DECLARE_METATYPE( const Marble::RenderPlugin * )

namespace Marble
{

class RenderPluginTest : public QObject
{
    Q_OBJECT

 private slots:
    void initialize_data();
    void initialize();

    void restoreDefaultSettings_data();
    void restoreDefaultSettings();

 private:
    MarbleModel m_model;
};

void RenderPluginTest::initialize_data()
{
    QTest::addColumn<const RenderPlugin *>( "factory" );

    foreach ( const RenderPlugin *plugin, m_model.pluginManager()->renderPlugins() ) {
        QTest::newRow( plugin->nameId().toAscii() ) << plugin;
    }
}

void RenderPluginTest::initialize()
{
    QFETCH( const RenderPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );

    instance->initialize();

    // prevent infinite loops
    QVERIFY( instance->isInitialized() );
}

void RenderPluginTest::restoreDefaultSettings_data()
{
    QTest::addColumn<QVariant>( "result" );
    QTest::addColumn<QVariant>( "expected" );

    foreach ( const RenderPlugin *plugin, m_model.pluginManager()->renderPlugins() ) {
        RenderPlugin *const result = plugin->newInstance( &m_model );
        result->initialize();

        RenderPlugin *const expected = plugin->newInstance( &m_model );
        expected->initialize();
        expected->restoreDefaultSettings();

        QHash<QString, QVariant> const & settings = expected->settings();
        QHash<QString, QVariant>::const_iterator itpoint = settings.begin();
        QHash<QString, QVariant>::const_iterator const endpoint = settings.end();
        for (; itpoint != endpoint; ++itpoint ) {
            const QString testName = QString( "%1 %2" ).arg( plugin->nameId() ).arg( itpoint.key() );
            QTest::newRow( testName.toAscii() ) << result->settings().value( itpoint.key() ) << expected->settings().value( itpoint.key() );
        }
    }
}

void RenderPluginTest::restoreDefaultSettings()
{
    QFETCH( QVariant, result );
    QFETCH( QVariant, expected );

    QCOMPARE( result, expected );
}

}

QTEST_MAIN( Marble::RenderPluginTest )

#include "RenderPluginTest.moc"
