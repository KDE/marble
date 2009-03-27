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
#include <QtCore/QTime>

// Local dir
#include "MarbleDirs.h"
#include "RenderPlugin.h"
#include "NetworkPlugin.h"
#include "AbstractFloatItem.h"

namespace Marble
{

class PluginManagerPrivate
{
 public:
    QList<RenderPlugin *> m_renderPlugins;
    QList<NetworkPlugin *> m_networkPlugins;
};

PluginManager::PluginManager( QObject *parent )
    : QObject(parent),
      d( new PluginManagerPrivate() )
{
    // For testing:
    QTime t;
    t.start();
    loadPlugins();
    qDebug("Time elapsed: %d ms", t.elapsed());
}

PluginManager::~PluginManager()
{
    qDeleteAll( d->m_renderPlugins );
    d->m_renderPlugins.clear();
    delete d;
}

QList<AbstractFloatItem *> PluginManager::floatItems() const
{
    QList<AbstractFloatItem *> floatItemList;

    QList<RenderPlugin *>::const_iterator i = d->m_renderPlugins.constBegin();
    for (; i != d->m_renderPlugins.constEnd(); ++i) {
        AbstractFloatItem *floatItem = qobject_cast<AbstractFloatItem *>(*i);
        if ( floatItem ) {
            floatItemList.append( floatItem );
        }
    }

    return floatItemList;
}

QList<RenderPlugin *> PluginManager::renderPlugins() const
{
    return d->m_renderPlugins;
}

QList<NetworkPlugin *> PluginManager::networkPlugins() const
{
    return d->m_networkPlugins;
}

void PluginManager::loadPlugins()
{
    qDebug() << "Starting to load Plugins.";
    QStringList pluginFileNameList;

    pluginFileNameList = MarbleDirs::pluginEntryList( "", QDir::Files );

    MarbleDirs::debug();

    qDeleteAll( d->m_renderPlugins );
    d->m_renderPlugins.clear();

    qDeleteAll( d->m_networkPlugins );
    d->m_networkPlugins.clear();

    foreach( const QString &fileName, pluginFileNameList ) {
        qDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QPluginLoader loader( MarbleDirs::pluginPath( fileName ) );

        QObject * obj = loader.instance();

        RenderPlugin * renderPlugin = 0;
        NetworkPlugin * networkPlugin = 0;
        if ( obj ) {
            if ( obj->inherits( "Marble::RenderPlugin" ) ) {
                qDebug() << "render plugin found" << MarbleDirs::pluginPath( fileName );
                renderPlugin = qobject_cast<RenderPlugin *>( obj )->pluginInstance();
                d->m_renderPlugins.append( renderPlugin );
            }
            else if ( obj->inherits( "Marble::NetworkPlugin" ) ) {
                qDebug() << "network plugin found" << MarbleDirs::pluginPath( fileName );
                networkPlugin = qobject_cast<NetworkPlugin *>( obj );
                d->m_networkPlugins.append( networkPlugin );
            }
        }

        if( !renderPlugin && !networkPlugin ) {
            qDebug() << "Plugin Failure: " << fileName << " is not a valid Marble Plugin:";
            qDebug() << loader.errorString();
        }
    }
}

}

#include "PluginManager.moc"
