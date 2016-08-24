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
#include <QList>
#include <QPluginLoader>
#include <QTime>

// Local dir
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "RenderPlugin.h"
#include "PositionProviderPlugin.h"
#include "ParseRunnerPlugin.h"
#include "ReverseGeocodingRunnerPlugin.h"
#include "RoutingRunnerPlugin.h"
#include "SearchRunnerPlugin.h"
#include "config-marble.h"

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
    QList<const PositionProviderPlugin *> m_positionProviderPluginTemplates;
    QList<const SearchRunnerPlugin *> m_searchRunnerPlugins;
    QList<const ReverseGeocodingRunnerPlugin *> m_reverseGeocodingRunnerPlugins;
    QList<RoutingRunnerPlugin *> m_routingRunnerPlugins;
    QList<const ParseRunnerPlugin *> m_parsingRunnerPlugins;
    static QStringList m_blacklist;
    static QStringList m_whitelist;

#ifdef Q_OS_ANDROID
    QStringList m_pluginPaths;
#endif
};

QStringList PluginManagerPrivate::m_blacklist;
QStringList PluginManagerPrivate::m_whitelist;

PluginManagerPrivate::~PluginManagerPrivate()
{
    // nothing to do
}

PluginManager::PluginManager( QObject *parent ) : QObject( parent ),
    d( new PluginManagerPrivate() )
{
    //Checking assets:/plugins for uninstalled plugins
#ifdef Q_OS_ANDROID
        installPluginsFromAssets();
#endif
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

void PluginManager::addRenderPlugin( const RenderPlugin *plugin )
{
    d->loadPlugins();
    d->m_renderPluginTemplates << plugin;
    emit renderPluginsChanged();
}

QList<const PositionProviderPlugin *> PluginManager::positionProviderPlugins() const
{
    d->loadPlugins();
    return d->m_positionProviderPluginTemplates;
}

void PluginManager::addPositionProviderPlugin( const PositionProviderPlugin *plugin )
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

void PluginManager::addSearchRunnerPlugin( const SearchRunnerPlugin *plugin )
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

void PluginManager::addReverseGeocodingRunnerPlugin( const ReverseGeocodingRunnerPlugin *plugin )
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

void PluginManager::addParseRunnerPlugin( const ParseRunnerPlugin *plugin )
{
    d->loadPlugins();
    d->m_parsingRunnerPlugins << plugin;
    emit parseRunnerPluginsChanged();
}

void PluginManager::blacklistPlugin(const QString &filename)
{
    PluginManagerPrivate::m_blacklist << MARBLE_SHARED_LIBRARY_PREFIX + filename;
}

void PluginManager::whitelistPlugin(const QString &filename)
{
    PluginManagerPrivate::m_whitelist << MARBLE_SHARED_LIBRARY_PREFIX + filename;
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
    Q_ASSERT( m_positionProviderPluginTemplates.isEmpty() );
    Q_ASSERT( m_searchRunnerPlugins.isEmpty() );
    Q_ASSERT( m_reverseGeocodingRunnerPlugins.isEmpty() );
    Q_ASSERT( m_routingRunnerPlugins.isEmpty() );
    Q_ASSERT( m_parsingRunnerPlugins.isEmpty() );

    foreach( const QString &fileName, pluginFileNameList ) {
        QString const baseName = QFileInfo(fileName).baseName();
        if (!m_whitelist.isEmpty() && !m_whitelist.contains(baseName)) {
            mDebug() << "Ignoring non-whitelisted plugin " << fileName;
            continue;
        }
        if (m_blacklist.contains(baseName)) {
            mDebug() << "Ignoring blacklisted plugin " << fileName;
            continue;
        }

        // mDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QString const path = MarbleDirs::pluginPath( fileName );
#ifdef Q_OS_ANDROID
        QFileInfo targetFile( path );
        if ( !m_pluginPaths.contains( targetFile.canonicalFilePath() ) ) {
            // @todo Delete the file here?
            qDebug() << "Ignoring file " << path << " which is not among the currently installed plugins";
            continue;
        }
#endif
        QPluginLoader* loader = new QPluginLoader( path );

        QObject * obj = loader->instance();

        if ( obj ) {
            bool isPlugin = appendPlugin<RenderPlugin, RenderPluginInterface>
                       ( obj, loader, m_renderPluginTemplates );
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
            qWarning() << "Ignoring to load the following file since it doesn't look like a valid Marble plugin:" << path << endl
                       << "Reason:" << loader->errorString();
            delete loader;
        }
    }

    m_pluginsLoaded = true;

    mDebug() << Q_FUNC_INFO << "Time elapsed:" << t.elapsed() << "ms";
}

#ifdef Q_OS_ANDROID
    void PluginManager::installPluginsFromAssets() const
    {
        d->m_pluginPaths.clear();
        QStringList copyList = MarbleDirs::pluginEntryList(QString());
        QDir pluginHome(MarbleDirs::localPath());
        pluginHome.mkpath(MarbleDirs::pluginLocalPath());
        pluginHome.setCurrent(MarbleDirs::pluginLocalPath());

        QStringList pluginNameFilter = QStringList() << "lib*.so";
        QStringList const existingPlugins = QDir(MarbleDirs::pluginLocalPath()).entryList(pluginNameFilter, QDir::Files);
        foreach(const QString &existingPlugin, existingPlugins) {
            QFile::remove(existingPlugin);
        }

        foreach (const QString & file, copyList) {
            QString const target = MarbleDirs::pluginLocalPath() + QLatin1Char('/') + file;
            if (QFileInfo(MarbleDirs::pluginSystemPath() + QLatin1Char('/') + file).isDir()) {
                pluginHome.mkpath(target);
            }
            else {
                QFile temporaryFile(MarbleDirs::pluginSystemPath() + QLatin1Char('/') + file);
                temporaryFile.copy(target);
                QFileInfo targetFile(target);
                d->m_pluginPaths << targetFile.canonicalFilePath();
            }
        }
    }
#endif

}

#include "moc_PluginManager.cpp"
