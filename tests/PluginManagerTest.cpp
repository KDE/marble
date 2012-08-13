//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin  <ps_ml@gmx.de>
//

#include <QtTest/QtTest>

#include "MarbleDirs.h"
#include "PluginManager.h"

namespace Marble
{

class PluginManagerTest : public QObject
{
    Q_OBJECT
    private slots:
        void loadPlugins();
};

void PluginManagerTest::loadPlugins()
{
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );

    const int pluginNumber = MarbleDirs::pluginEntryList( "", QDir::Files ).size();

    PluginManager pm;
    const int renderPlugins = pm.renderPlugins().size();
    const int networkPlugins = pm.networkPlugins().size();
    const int positionPlugins = pm.positionProviderPlugins().size();
    const int searchRunnerPlugins = pm.searchRunnerPlugins().size();
    const int reverseGeocodingRunnerPlugins = pm.reverseGeocodingRunnerPlugins().size();
    const int routingRunnerPlugins = pm.routingRunnerPlugins().size();
    const int parsingRunnerPlugins = pm.parsingRunnerPlugins().size();

    const int runnerPlugins = searchRunnerPlugins + reverseGeocodingRunnerPlugins + routingRunnerPlugins + parsingRunnerPlugins;

    QCOMPARE( renderPlugins + networkPlugins + positionPlugins + runnerPlugins, pluginNumber );
}

}

QTEST_MAIN( Marble::PluginManagerTest )

#include "PluginManagerTest.moc"
