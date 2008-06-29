//
// This file is part of the Marble Desktop Globe.
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

class PluginManagerTest : public QObject {
    Q_OBJECT
    private slots:
        void loadPlugins();
};

void PluginManagerTest::loadPlugins() {
    int pluginNumber = MarbleDirs::pluginEntryList( "", QDir::Files ).size();
    PluginManager *pm = new PluginManager();
    QCOMPARE( pm->layerPlugins().size(), pluginNumber );
}

QTEST_MAIN( PluginManagerTest )

#include "PluginManagerTest.moc"
