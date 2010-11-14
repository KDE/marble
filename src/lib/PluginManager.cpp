//
// This file is part of the Marble Virtual Globe.
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
#include <QtCore/QList>
#include <QtCore/QPluginLoader>
#include <QtCore/QTime>

// Local dir
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "RenderPlugin.h"
#include "NetworkPlugin.h"
#include "PositionProviderPlugin.h"
#include "AbstractFloatItem.h"
#include "RunnerPlugin.h"

namespace Marble
{

class PluginManagerPrivate
{
 public:
    PluginManagerPrivate()
            : m_pluginsLoaded(false)
    {
    }

    ~PluginManagerPrivate();

    void loadPlugins();

    bool m_pluginsLoaded;
    QMap<QPluginLoader*, RenderPlugin *> m_renderPluginTemplates;
    QMap<QPluginLoader*, NetworkPlugin *> m_networkPluginTemplates;
    QMap<QPluginLoader*, PositionProviderPlugin *> m_positionProviderPluginTemplates;
    QMap<QPluginLoader*, RunnerPlugin *> m_runnerPlugins;

private:
    bool cleanup( const QList<QPluginLoader*> loaders );
};

PluginManagerPrivate::~PluginManagerPrivate()
{
    QMap<QPluginLoader*, RunnerPlugin *>::const_iterator i = m_runnerPlugins.constBegin();
    bool allUnloaded = cleanup( m_renderPluginTemplates.keys() );
    allUnloaded = allUnloaded && cleanup( m_networkPluginTemplates.keys() );
    allUnloaded = allUnloaded && cleanup( m_positionProviderPluginTemplates.keys() );
    allUnloaded = allUnloaded && cleanup( m_runnerPlugins.keys() );

    if ( allUnloaded ) {
        mDebug() << "All plugins unloaded. Plugin instances still alive will crash now.";
    }
}

bool PluginManagerPrivate::cleanup( const QList<QPluginLoader*> loaders )
{
    bool allUnloaded = true;
    foreach( QPluginLoader* loader, loaders ) {
        allUnloaded = allUnloaded && loader->unload();
        delete loader;
    }

    return allUnloaded;
}

PluginManager::PluginManager( QObject *parent )
    : QObject(parent),
      d( new PluginManagerPrivate() )
{
}

PluginManager::~PluginManager()
{
    delete d;
}

QList<AbstractFloatItem *> PluginManager::createFloatItems() const
{
    QList<AbstractFloatItem *> floatItemList;

    d->loadPlugins();

    QMap<QPluginLoader*, RenderPlugin *>::const_iterator i = d->m_renderPluginTemplates.constBegin();
    QMap<QPluginLoader*, RenderPlugin *>::const_iterator const end = d->m_renderPluginTemplates.constEnd();
    for (; i != end; ++i) {
        AbstractFloatItem * const floatItem = qobject_cast<AbstractFloatItem *>(*i);
        if ( floatItem ) {
            floatItemList.append( qobject_cast<AbstractFloatItem *>( floatItem->
                                                                     newInstance() ));
        }
    }

    return floatItemList;
}

template<class T>
QList<T*> createPlugins( PluginManagerPrivate* d, const QMap<QPluginLoader*, T*> &loaders )
{
    QList<T*> result;
    d->loadPlugins();
    typename QMap<QPluginLoader*, T*>::const_iterator i = loaders.constBegin();
    typename QMap<QPluginLoader*, T*>::const_iterator const end = loaders.constEnd();
    for (; i != end; ++i) {
        result.append( (*i)->newInstance() );
    }
    return result;
}

QList<RenderPlugin *> PluginManager::createRenderPlugins() const
{
    return createPlugins( d, d->m_renderPluginTemplates );
}

QList<NetworkPlugin *> PluginManager::createNetworkPlugins() const
{
    return createPlugins( d, d->m_networkPluginTemplates );
}

QList<PositionProviderPlugin *> PluginManager::createPositionProviderPlugins() const
{
    return createPlugins( d, d->m_positionProviderPluginTemplates );
}

QList<RunnerPlugin *> PluginManager::runnerPlugins() const
{
    d->loadPlugins();
    return d->m_runnerPlugins.values();
}

/** Append obj to the given plugins list if it inherits both T and U */
template<class T, class U>
bool appendPlugin( QObject * obj, QPluginLoader* &loader, QMap<QPluginLoader*,T*> &plugins )
{
    if ( qobject_cast<T*>( obj ) && qobject_cast<U*>( obj ) ) {
        Q_ASSERT( obj->metaObject()->superClass() ); // all our plugins have a super class
        mDebug() <<  obj->metaObject()->superClass()->className()
                << "plugin loaded from" << loader->fileName();
        T* plugin = qobject_cast<T*>( obj );
        Q_ASSERT( plugin ); // checked above
        plugins[loader] = plugin;
        return true;
    }

    return false;
}

void PluginManagerPrivate::loadPlugins()
{
    if (m_pluginsLoaded)
    {
        return;
    }

    QTime t;
    t.start();
    mDebug() << "Starting to load Plugins.";

    QStringList pluginFileNameList = MarbleDirs::pluginEntryList( "", QDir::Files );

    MarbleDirs::debug();

    qDeleteAll( m_renderPluginTemplates );
    m_renderPluginTemplates.clear();

    qDeleteAll( m_networkPluginTemplates );
    m_networkPluginTemplates.clear();

    qDeleteAll( m_positionProviderPluginTemplates );
    m_positionProviderPluginTemplates.clear();

    // No need to delete runner plugins

    foreach( const QString &fileName, pluginFileNameList ) {
        // mDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QString const path = MarbleDirs::pluginPath( fileName );
        QPluginLoader* loader = new QPluginLoader( path );

        QObject * obj = loader->instance();

        if ( obj ) {
            bool isPlugin = appendPlugin<RenderPlugin, RenderPluginInterface>
                       ( obj, loader, m_renderPluginTemplates );
            isPlugin = isPlugin || appendPlugin<NetworkPlugin, NetworkPluginInterface>
                       ( obj, loader, m_networkPluginTemplates );
            isPlugin = isPlugin || appendPlugin<PositionProviderPlugin, PositionProviderPluginInterface>
                       ( obj, loader, m_positionProviderPluginTemplates );
            isPlugin = isPlugin || appendPlugin<RunnerPlugin, RunnerPlugin>
                       ( obj, loader, m_runnerPlugins ); // intentionally T==U
            if ( !isPlugin ) {
                mDebug() << "Plugin failure:" << fileName << "is a plugin, but it does not implement the "
                        << "right interfaces or it was compiled against an old version of Marble. Ignoring it.";
                delete loader;
            }
        } else {
            mDebug() << "Plugin failure:" << fileName << "is not a valid Marble Plugin:"
                     << loader->errorString();
        }
    }

    m_pluginsLoaded = true;
    qDebug("Time elapsed: %d ms", t.elapsed());
}

}

#include "PluginManager.moc"
