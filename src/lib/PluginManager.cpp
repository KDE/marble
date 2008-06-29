//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


// Own
#include "PluginManager.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QPluginLoader>

// Local dir
#include "MarbleDirs.h"
#include "MarbleAbstractLayer.h"
#include "MarbleAbstractFloatItem.h"

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
    // For testing:
    loadPlugins();
}

PluginManager::~PluginManager()
{
}

QList<MarbleAbstractFloatItem *> PluginManager::floatItems() const
{
    QList<MarbleAbstractFloatItem *> floatItemList;

    QList<MarbleAbstractLayer *>::const_iterator i;
    for (i = m_layerPlugins.begin(); i != m_layerPlugins.end(); ++i)
    {
        MarbleAbstractFloatItem *floatItem = qobject_cast<MarbleAbstractFloatItem *>(*i);
        if ( floatItem )
        {
            floatItemList.append( floatItem );
        }
    }

    return floatItemList;
}

QList<MarbleAbstractLayer *> PluginManager::layerPlugins() const
{
    return m_layerPlugins;
}

void PluginManager::loadPlugins()
{
    qDebug() << "Starting to load Plugins.";
    QStringList pluginFileNameList;

    pluginFileNameList = MarbleDirs::pluginEntryList( "", QDir::Files );

    MarbleDirs::debug();

    qDeleteAll( m_layerPlugins );
    m_layerPlugins.clear();

    foreach( QString fileName, pluginFileNameList ) {
        qDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QPluginLoader loader( MarbleDirs::pluginPath( fileName ) );

        QObject *obj = loader.instance();
        MarbleAbstractLayer * layerPlugin = qobject_cast<MarbleAbstractLayer *>(obj);

        if( layerPlugin ) {
            m_layerPlugins.append( layerPlugin );
        }
        else
        {
            qDebug() << "Plugin Failure: " << fileName << " is not a valid Marble Plugin:";
            qDebug() << loader.errorString();
        }
    }
}

#include "PluginManager.moc"
