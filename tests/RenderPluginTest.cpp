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

namespace Marble
{

class RenderPluginTest : public QObject
{
    Q_OBJECT

 private slots:
    void restoreDefaultSettings_data();
    void restoreDefaultSettings();

 private:
    MarbleModel m_model;
};

void RenderPluginTest::restoreDefaultSettings_data()
{
    QTest::addColumn<QVariant>( "result" );
    QTest::addColumn<QVariant>( "expected" );

    foreach ( const RenderPlugin *plugin, m_model.pluginManager()->renderPlugins() ) {
        RenderPlugin *const instance1 = plugin->newInstance( &m_model );
        instance1->initialize();

        RenderPlugin *const instance2 = plugin->newInstance( &m_model );
        instance2->initialize();

        instance2->restoreDefaultSettings();

        foreach ( const QString &key, instance1->settings().keys() ) {
            const QString testName = QString( "%1 %2" ).arg( plugin->nameId() ).arg( key );
            QTest::newRow( testName.toAscii() ) << instance1->settings().value( key ) << instance2->settings().value( key );
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
