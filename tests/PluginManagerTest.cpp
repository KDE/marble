// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//


#include "MarbleDirs.h"
#include "PluginManager.h"

#include <QTest>

namespace Marble
{

class PluginManagerTest : public QObject
{
    Q_OBJECT
    private Q_SLOTS:
        void loadPlugins();
};

void PluginManagerTest::loadPlugins()
{
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    const int pluginNumber = MarbleDirs::pluginEntryList( "", QDir::Files ).size();

    PluginManager pm;
    const int renderPlugins = pm.renderPlugins().size();
    const int positionPlugins = pm.positionProviderPlugins().size();
    const int searchRunnerPlugins = pm.searchRunnerPlugins().size();
    const int reverseGeocodingRunnerPlugins = pm.reverseGeocodingRunnerPlugins().size();
    const int routingRunnerPlugins = pm.routingRunnerPlugins().size();
    const int parsingRunnerPlugins = pm.parsingRunnerPlugins().size();

    const int runnerPlugins = searchRunnerPlugins + reverseGeocodingRunnerPlugins + routingRunnerPlugins + parsingRunnerPlugins;

    QCOMPARE( renderPlugins + positionPlugins + runnerPlugins, pluginNumber );
}

}

QTEST_MAIN( Marble::PluginManagerTest )

#include "PluginManagerTest.moc"
