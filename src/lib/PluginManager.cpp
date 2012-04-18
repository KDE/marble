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
    QList<const RenderPlugin *> m_renderPluginTemplates;
    QList<const NetworkPlugin *> m_networkPluginTemplates;
    QList<const PositionProviderPlugin *> m_positionProviderPluginTemplates;
    QList<RunnerPlugin *> m_runnerPlugins;
};

PluginManagerPrivate::~PluginManagerPrivate()
{
    // nothing to do
}

PluginManager::PluginManager()
    : d( new PluginManagerPrivate() )
{
}

PluginManager::~PluginManager()
{
    delete d;
}

template<class T>
QList<T*> createPlugins( const QList<const T*> &factories )
{
    QList<T*> result;

    foreach ( const T *factory, factories ) {
        T *instance = factory->newInstance();
        Q_ASSERT( instance && "Plugin returned null when requesting a new instance." );
        result.append( instance );
    }

    return result;
}

QList<const RenderPlugin *> PluginManager::renderPlugins() const
{
    d->loadPlugins();
    return d->m_renderPluginTemplates;
}

QList<NetworkPlugin *> PluginManager::createNetworkPlugins() const
{
    d->loadPlugins();
    return createPlugins( d->m_networkPluginTemplates );
}

QList<const PositionProviderPlugin *> PluginManager::positionProviderPlugins() const
{
    d->loadPlugins();
    return d->m_positionProviderPluginTemplates;
}

QList<RunnerPlugin *> PluginManager::runnerPlugins() const
{
    d->loadPlugins();
    return d->m_runnerPlugins;
}

/** Append obj to the given plugins list if it inherits both T and U */
template<class T, class U>
bool appendPlugin( QObject * obj, QPluginLoader* &loader, QList<T*> &plugins )
{
    if ( qobject_cast<T*>( obj ) && qobject_cast<U*>( obj ) ) {
        Q_ASSERT( obj->metaObject()->superClass() ); // all our plugins have a super class
        mDebug() <<  obj->metaObject()->superClass()->className()
                << "plugin loaded from" << loader->fileName();
        T* plugin = qobject_cast<T*>( obj );
        Q_ASSERT( plugin ); // checked above
        plugins << plugin;
        return true;
    }

    return false;
}

/** Append obj to the given plugins list if it inherits both T and U */
template<class T, class U>
bool appendPlugin( QObject * obj, QPluginLoader* &loader, QList<const T*> &plugins )
{
    if ( qobject_cast<T*>( obj ) && qobject_cast<U*>( obj ) ) {
        Q_ASSERT( obj->metaObject()->superClass() ); // all our plugins have a super class
        mDebug() <<  obj->metaObject()->superClass()->className()
                << "plugin loaded from" << loader->fileName();
        T* plugin = qobject_cast<T*>( obj );
        Q_ASSERT( plugin ); // checked above
        plugins << plugin;
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

    Q_ASSERT( m_renderPluginTemplates.isEmpty() );
    Q_ASSERT( m_networkPluginTemplates.isEmpty() );
    Q_ASSERT( m_positionProviderPluginTemplates.isEmpty() );
    Q_ASSERT( m_runnerPlugins.isEmpty() );

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

    mDebug() << Q_FUNC_INFO << "Time elapsed:" << t.elapsed() << "ms";
}

}
