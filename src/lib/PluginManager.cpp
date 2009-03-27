//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
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
    QList<RenderPlugin *> m_renderPluginTemplates;
    QList<NetworkPlugin *> m_networkPluginTemplates;
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
    qDeleteAll( d->m_renderPluginTemplates );
    qDeleteAll( d->m_networkPluginTemplates );
    delete d;
}

QList<AbstractFloatItem *> PluginManager::createFloatItems() const
{
    QList<AbstractFloatItem *> floatItemList;

    QList<RenderPlugin *>::const_iterator i = d->m_renderPluginTemplates.constBegin();
    QList<RenderPlugin *>::const_iterator const end = d->m_renderPluginTemplates.constEnd();
    for (; i != end; ++i) {
        AbstractFloatItem * const floatItem = qobject_cast<AbstractFloatItem *>(*i);
        if ( floatItem ) {
            floatItemList.append( qobject_cast<AbstractFloatItem *>( floatItem->
                                                                     pluginInstance() ));
        }
    }

    return floatItemList;
}

QList<RenderPlugin *> PluginManager::createRenderPlugins() const
{
    QList<RenderPlugin *> result;
    QList<RenderPlugin *>::const_iterator i = d->m_renderPluginTemplates.constBegin();
    QList<RenderPlugin *>::const_iterator const end = d->m_renderPluginTemplates.constEnd();
    for (; i != end; ++i) {
        result.append( (*i)->pluginInstance() );
    }
    return result;
}

QList<NetworkPlugin *> PluginManager::createNetworkPlugins() const
{
    QList<NetworkPlugin *> result;
    QList<NetworkPlugin *>::const_iterator pos = d->m_networkPluginTemplates.constBegin();
    QList<NetworkPlugin *>::const_iterator const end = d->m_networkPluginTemplates.constEnd();
    for (; pos != end; ++pos ) {
        result.append( (*pos)->create() );
    }
    return result;
}

void PluginManager::loadPlugins()
{
    qDebug() << "Starting to load Plugins.";

    QStringList pluginFileNameList = MarbleDirs::pluginEntryList( "", QDir::Files );

    MarbleDirs::debug();

    qDeleteAll( d->m_renderPluginTemplates );
    d->m_renderPluginTemplates.clear();

    qDeleteAll( d->m_networkPluginTemplates );
    d->m_networkPluginTemplates.clear();

    foreach( const QString &fileName, pluginFileNameList ) {
        // qDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QPluginLoader loader( MarbleDirs::pluginPath( fileName ) );

        QObject * obj = loader.instance();

        RenderPlugin * renderPlugin = 0;
        NetworkPlugin * networkPlugin = 0;
        if ( obj ) {
            if ( obj->inherits( "Marble::RenderPlugin" ) ) {
                qDebug() << "render plugin found" << MarbleDirs::pluginPath( fileName );
                renderPlugin = qobject_cast<RenderPlugin *>( obj );
                d->m_renderPluginTemplates.append( renderPlugin );
            }
            else if ( obj->inherits( "Marble::NetworkPlugin" ) ) {
                qDebug() << "network plugin found" << MarbleDirs::pluginPath( fileName );
                networkPlugin = qobject_cast<NetworkPlugin *>( obj );
                d->m_networkPluginTemplates.append( networkPlugin );
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
