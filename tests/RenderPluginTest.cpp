//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "MarbleModel.h"
#include "PluginManager.h"
#include "RenderPlugin.h"

#include <QMetaType>
#include <QTest>

Q_DECLARE_METATYPE( const Marble::RenderPlugin * )

namespace Marble
{

class RenderPluginTest : public QObject
{
    Q_OBJECT

 private Q_SLOTS:
    void newInstance_data();
    void newInstance();

    void initialize_data();
    void initialize();

    void setVisible_data();
    void setVisible();

    void setEnabled_data();
    void setEnabled();

    void setSettingsVisible_data();
    void setSettingsVisible();

    void setSettingsEnabled_data();
    void setSettingsEnabled();

    void setSettingsAllEqual_data();
    void setSettingsAllEqual();

    void restoreDefaultSettings_data();
    void restoreDefaultSettings();

 private:
    MarbleModel m_model;
};

void RenderPluginTest::newInstance_data()
{
    QTest::addColumn<const RenderPlugin *>( "factory" );

    foreach ( const RenderPlugin *factory, m_model.pluginManager()->renderPlugins() ) {
        QTest::newRow(factory->nameId().toLatin1().constData())
            << factory;
    }
}

void RenderPluginTest::newInstance()
{
    QFETCH( const RenderPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );

    const bool enabledByDefault = instance->enabled();
    const bool visibleByDefault = instance->visible();

    QVERIFY( instance->settings().contains( "enabled" ) );
    QVERIFY( instance->settings().contains( "visible" ) );
    QCOMPARE( instance->settings().value( "enabled", !enabledByDefault ).toBool(), enabledByDefault );
    QCOMPARE( instance->settings().value( "visible", !visibleByDefault ).toBool(), visibleByDefault );

    delete instance;
}

void RenderPluginTest::initialize_data()
{
    QTest::addColumn<const RenderPlugin *>( "factory" );

    foreach ( const RenderPlugin *plugin, m_model.pluginManager()->renderPlugins() ) {
        QTest::newRow(plugin->nameId().toLatin1().constData())
            << plugin;
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

void RenderPluginTest::setVisible_data()
{
    QTest::addColumn<const RenderPlugin *>( "factory" );

    foreach ( const RenderPlugin *factory, m_model.pluginManager()->renderPlugins() ) {
        QTest::newRow(factory->nameId().toLatin1().constData())
            << factory;
    }
}

void RenderPluginTest::setVisible()
{
    QFETCH( const RenderPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );

    const bool visibleByDefault = instance->visible();

    instance->setVisible( !visibleByDefault );

    QCOMPARE( instance->visible(), !visibleByDefault );
    QCOMPARE( instance->settings().value( "visible", visibleByDefault ).toBool(), !visibleByDefault );

    QHash<QString, QVariant> settings;
    settings.insert( "visible", !visibleByDefault );

    instance->setSettings( settings );
    instance->setVisible( visibleByDefault ); // should also overwrite setting "visible"

    QCOMPARE( instance->visible(), visibleByDefault );
    QCOMPARE( instance->settings().value( "visible", !visibleByDefault ).toBool(), visibleByDefault );

    delete instance;
}

void RenderPluginTest::setEnabled_data()
{
    QTest::addColumn<const RenderPlugin *>( "factory" );

    foreach ( const RenderPlugin *factory, m_model.pluginManager()->renderPlugins() ) {
        QTest::newRow(factory->nameId().toLatin1().constData())
            << factory;
    }
}

void RenderPluginTest::setEnabled()
{
    QFETCH( const RenderPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );

    const bool enabledByDefault = instance->enabled();

    instance->setEnabled( !enabledByDefault );

    QCOMPARE( instance->enabled(), !enabledByDefault );
    QCOMPARE( instance->settings().value( "enabled", enabledByDefault ).toBool(), !enabledByDefault );

    QHash<QString, QVariant> settings;
    settings.insert( "enabled", !enabledByDefault );

    instance->setSettings( settings );
    instance->setEnabled( enabledByDefault ); // should also overwrite setting "enabled"

    QCOMPARE( instance->enabled(), enabledByDefault );
    QCOMPARE( instance->settings().value( "enabled", !enabledByDefault ).toBool(), enabledByDefault );

    delete instance;
}

void RenderPluginTest::setSettingsVisible_data()
{
    QTest::addColumn<const RenderPlugin *>( "factory" );

    foreach ( const RenderPlugin *factory, m_model.pluginManager()->renderPlugins() ) {
        QTest::newRow(factory->nameId().toLatin1().constData())
            << factory;
    }
}

void RenderPluginTest::setSettingsVisible()
{
    QFETCH( const RenderPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );

    const bool visibleByDefault = instance->visible();

    QHash<QString, QVariant> settings = instance->settings();

    // visibile property should follow setting
    settings.insert( "visible", !visibleByDefault );
    instance->setSettings( settings );

    QCOMPARE( instance->settings().value( "visible", visibleByDefault ).toBool(), !visibleByDefault );
    QCOMPARE( instance->visible(), !visibleByDefault );

    settings.insert( "visible", visibleByDefault );
    instance->setSettings( settings );

    QCOMPARE( instance->settings().value( "visible", !visibleByDefault ).toBool(), visibleByDefault );
    QCOMPARE( instance->visible(), visibleByDefault );

    // restoreDefaultSettings() is triggered by the config dialog, but it shouldn't touch visible property
    instance->setVisible( !visibleByDefault );
    instance->restoreDefaultSettings();

    QCOMPARE( instance->visible(), !visibleByDefault );

    delete instance;
}

void RenderPluginTest::setSettingsEnabled_data()
{
    QTest::addColumn<const RenderPlugin *>( "factory" );

    foreach ( const RenderPlugin *factory, m_model.pluginManager()->renderPlugins() ) {
        QTest::newRow(factory->nameId().toLatin1().constData())
            << factory;
    }
}

void RenderPluginTest::setSettingsEnabled()
{
    QFETCH( const RenderPlugin *, factory );

    RenderPlugin *const instance = factory->newInstance( &m_model );

    const bool enabledByDefault = instance->enabled();

    QHash<QString, QVariant> settings = instance->settings();

    // enabled property should follow setting
    settings.insert( "enabled", !enabledByDefault );
    instance->setSettings( settings );

    QCOMPARE( instance->settings().value( "enabled", enabledByDefault ).toBool(), !enabledByDefault );
    QCOMPARE( instance->enabled(), !enabledByDefault );

    settings.insert( "enabled", enabledByDefault );
    instance->setSettings( settings );

    QCOMPARE( instance->settings().value( "enabled", !enabledByDefault ).toBool(), enabledByDefault );
    QCOMPARE( instance->enabled(), enabledByDefault );

    // restoreDefaultSettings() is triggered by the config dialog, but it shouldn't touch enabled property
    instance->setEnabled( !enabledByDefault );
    instance->restoreDefaultSettings();

    QCOMPARE( instance->enabled(), !enabledByDefault );

    delete instance;
}

void RenderPluginTest::setSettingsAllEqual_data()
{
    QTest::addColumn<QVariant>( "result" );
    QTest::addColumn<QVariant>( "expected" );

    foreach ( const RenderPlugin *plugin, m_model.pluginManager()->renderPlugins() ) {
        const RenderPlugin *const expected = plugin->newInstance( &m_model );

        RenderPlugin *const result = plugin->newInstance( &m_model );
        result->setSettings( expected->settings() );

        const QHash<QString, QVariant> settings = expected->settings();
        QHash<QString, QVariant>::const_iterator itpoint = settings.begin();
        QHash<QString, QVariant>::const_iterator const endpoint = settings.end();
        for (; itpoint != endpoint; ++itpoint ) {
            const QString testName = QString( "%1 %2" ).arg( plugin->nameId() ).arg( itpoint.key() );
            QTest::newRow(testName.toLatin1().constData())
                << result->settings().value(itpoint.key())
                << expected->settings().value(itpoint.key());
        }
    }
}

void RenderPluginTest::setSettingsAllEqual()
{
    QFETCH( QVariant, result );
    QFETCH( QVariant, expected );

    QCOMPARE( result, expected );
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
            QTest::newRow(testName.toLatin1().constData())
                << result->settings().value(itpoint.key())
                << expected->settings().value(itpoint.key());
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
