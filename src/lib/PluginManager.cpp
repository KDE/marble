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
#include "ParseRunnerPlugin.h"
#include "ReverseGeocodingRunnerPlugin.h"
#include "RoutingRunnerPlugin.h"
#include "SearchRunnerPlugin.h"

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
    QList<const SearchRunnerPlugin *> m_searchRunnerPlugins;
    QList<const ReverseGeocodingRunnerPlugin *> m_reverseGeocodingRunnerPlugins;
    QList<RoutingRunnerPlugin *> m_routingRunnerPlugins;
    QList<const ParseRunnerPlugin *> m_parsingRunnerPlugins;
};

PluginManagerPrivate::~PluginManagerPrivate()
{
    // nothing to do
}

PluginManager::PluginManager( QObject *parent ) : QObject( parent ),
    d( new PluginManagerPrivate() )
{
}

PluginManager::~PluginManager()
{
    delete d;
}

QList<const RenderPlugin *> PluginManager::renderPlugins() const
{
    d->loadPlugins();
    return d->m_renderPluginTemplates;
}

void PluginManager::addRenderPlugin( RenderPlugin *plugin )
{
    d->loadPlugins();
    d->m_renderPluginTemplates << plugin;
    emit renderPluginsChanged();
}

QList<const NetworkPlugin *> PluginManager::networkPlugins() const
{
    d->loadPlugins();
    return d->m_networkPluginTemplates;
}

void PluginManager::addNetworkPlugin( NetworkPlugin *plugin )
{
    d->loadPlugins();
    d->m_networkPluginTemplates << plugin;
    emit networkPluginsChanged();
}

QList<const PositionProviderPlugin *> PluginManager::positionProviderPlugins() const
{
    d->loadPlugins();
    return d->m_positionProviderPluginTemplates;
}

void PluginManager::addPositionProviderPlugin( PositionProviderPlugin *plugin )
{
    d->loadPlugins();
    d->m_positionProviderPluginTemplates << plugin;
    emit positionProviderPluginsChanged();
}

QList<const SearchRunnerPlugin *> PluginManager::searchRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_searchRunnerPlugins;
}

void PluginManager::addSearchRunnerPlugin( SearchRunnerPlugin *plugin )
{
    d->loadPlugins();
    d->m_searchRunnerPlugins << plugin;
    emit searchRunnerPluginsChanged();
}

QList<const ReverseGeocodingRunnerPlugin *> PluginManager::reverseGeocodingRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_reverseGeocodingRunnerPlugins;
}

void PluginManager::addReverseGeocodingRunnerPlugin( ReverseGeocodingRunnerPlugin *plugin )
{
    d->loadPlugins();
    d->m_reverseGeocodingRunnerPlugins << plugin;
    emit reverseGeocodingRunnerPluginsChanged();
}

QList<RoutingRunnerPlugin *> PluginManager::routingRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_routingRunnerPlugins;
}

void PluginManager::addRoutingRunnerPlugin( RoutingRunnerPlugin *plugin )
{
    d->loadPlugins();
    d->m_routingRunnerPlugins << plugin;
    emit routingRunnerPluginsChanged();
}

QList<const ParseRunnerPlugin *> PluginManager::parsingRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_parsingRunnerPlugins;
}

void PluginManager::addParseRunnerPlugin( ParseRunnerPlugin *plugin )
{
    d->loadPlugins();
    d->m_parsingRunnerPlugins << plugin;
    emit parseRunnerPluginsChanged();
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
    Q_ASSERT( m_searchRunnerPlugins.isEmpty() );
    Q_ASSERT( m_reverseGeocodingRunnerPlugins.isEmpty() );
    Q_ASSERT( m_routingRunnerPlugins.isEmpty() );
    Q_ASSERT( m_parsingRunnerPlugins.isEmpty() );

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
            isPlugin = isPlugin || appendPlugin<SearchRunnerPlugin, SearchRunnerPlugin>
                       ( obj, loader, m_searchRunnerPlugins ); // intentionally T==U
            isPlugin = isPlugin || appendPlugin<ReverseGeocodingRunnerPlugin, ReverseGeocodingRunnerPlugin>
                       ( obj, loader, m_reverseGeocodingRunnerPlugins ); // intentionally T==U
            isPlugin = isPlugin || appendPlugin<RoutingRunnerPlugin, RoutingRunnerPlugin>
                       ( obj, loader, m_routingRunnerPlugins ); // intentionally T==U
            isPlugin = isPlugin || appendPlugin<ParseRunnerPlugin, ParseRunnerPlugin>
                       ( obj, loader, m_parsingRunnerPlugins ); // intentionally T==U
            if ( !isPlugin ) {
                qWarning() << "Ignoring the following plugin since it couldn't be loaded:" << path;
                mDebug() << "Plugin failure:" << path << "is a plugin, but it does not implement the "
                        << "right interfaces or it was compiled against an old version of Marble. Ignoring it.";
                delete loader;
            }
        } else {
            qWarning() << "Ignoring to load the following file since it doesn't look like a valid Marble plugin:" << path;
            mDebug() << Q_FUNC_INFO << "Plugin failure:" << loader->errorString();
            delete loader;
        }
    }

    m_pluginsLoaded = true;

    mDebug() << Q_FUNC_INFO << "Time elapsed:" << t.elapsed() << "ms";
}

}

#include "PluginManager.moc"
