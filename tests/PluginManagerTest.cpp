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

    int             pluginNumber = MarbleDirs::pluginEntryList( "", QDir::Files ).size();
    PluginManager  *pm = new PluginManager( 0 );
    QCOMPARE( pm->createRenderPlugins().size(), pluginNumber );
}

}

QTEST_MAIN( Marble::PluginManagerTest )

#include "PluginManagerTest.moc"
