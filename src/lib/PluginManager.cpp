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
#include "MarbleRenderPlugin.h"
#include "MarbleAbstractFloatItem.h"

namespace Marble
{

class PluginManagerPrivate
{
 public:
    PluginManagerPrivate()
    {
    }

    QList<MarbleRenderPlugin *> m_renderPlugins;
};

PluginManager::PluginManager( QObject *parent )
    : QObject(parent),
      d( new PluginManagerPrivate() )
{
    // For testing:
    loadPlugins();
}

PluginManager::~PluginManager()
{
//    qDeleteAll( d->m_renderPlugins );
//    d->m_renderPlugins.clear();
    delete d;
}

QList<MarbleAbstractFloatItem *> PluginManager::floatItems() const
{
    QList<MarbleAbstractFloatItem *> floatItemList;

    QList<MarbleRenderPlugin *>::const_iterator i;
    for (i = d->m_renderPlugins.constBegin(); i != d->m_renderPlugins.constEnd(); ++i)
    {
        MarbleAbstractFloatItem *floatItem = qobject_cast<MarbleAbstractFloatItem *>(*i);
        if ( floatItem )
        {
            floatItemList.append( floatItem );
        }
    }

    return floatItemList;
}

QList<MarbleRenderPlugin *> PluginManager::renderPlugins() const
{
    return d->m_renderPlugins;
}

void PluginManager::loadPlugins()
{
    qDebug() << "Starting to load Plugins.";
    QStringList pluginFileNameList;

    pluginFileNameList = MarbleDirs::pluginEntryList( "", QDir::Files );

    MarbleDirs::debug();

    qDeleteAll( d->m_renderPlugins );
    d->m_renderPlugins.clear();

    foreach( const QString &fileName, pluginFileNameList ) {
        qDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QPluginLoader loader( MarbleDirs::pluginPath( fileName ) );

        QObject *obj = loader.instance();
        MarbleRenderPlugin * layerPlugin = qobject_cast<MarbleRenderPlugin *>(obj);

        if( layerPlugin ) {
            d->m_renderPlugins.append( layerPlugin );
        }
        else {
            qDebug() << "Plugin Failure: " << fileName << " is not a valid Marble Plugin:";
            qDebug() << loader.errorString();
        }
    }
}

}

#include "PluginManager.moc"
