// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

// Own
#include "PluginManager.h"

// Qt
#include <QElapsedTimer>
#include <QMessageBox>
#include <QPluginLoader>

// Local dir
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ParseRunnerPlugin.h"
#include "PositionProviderPlugin.h"
#include "RenderPlugin.h"
#include "ReverseGeocodingRunnerPlugin.h"
#include "RoutingRunnerPlugin.h"
#include "SearchRunnerPlugin.h"
#include <config-marble.h>

namespace Marble
{

class PluginManagerPrivate
{
public:
    PluginManagerPrivate(PluginManager *parent)
        : m_pluginsLoaded(false)
        , m_parent(parent)
    {
    }

    ~PluginManagerPrivate();

    void loadPlugins();
    bool addPlugin(QObject *obj, const QPluginLoader *loader);

    bool m_pluginsLoaded;
    QList<const RenderPlugin *> m_renderPluginTemplates;
    QList<const PositionProviderPlugin *> m_positionProviderPluginTemplates;
    QList<const SearchRunnerPlugin *> m_searchRunnerPlugins;
    QList<const ReverseGeocodingRunnerPlugin *> m_reverseGeocodingRunnerPlugins;
    QList<RoutingRunnerPlugin *> m_routingRunnerPlugins;
    QList<const ParseRunnerPlugin *> m_parsingRunnerPlugins;
    PluginManager *m_parent;
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

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , d(new PluginManagerPrivate(this))
{
    // Checking assets:/plugins for uninstalled plugins
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

void PluginManager::addRenderPlugin(const RenderPlugin *plugin)
{
    d->loadPlugins();
    d->m_renderPluginTemplates << plugin;
    Q_EMIT renderPluginsChanged();
}

QList<const PositionProviderPlugin *> PluginManager::positionProviderPlugins() const
{
    d->loadPlugins();
    return d->m_positionProviderPluginTemplates;
}

void PluginManager::addPositionProviderPlugin(const PositionProviderPlugin *plugin)
{
    d->loadPlugins();
    d->m_positionProviderPluginTemplates << plugin;
    Q_EMIT positionProviderPluginsChanged();
}

QList<const SearchRunnerPlugin *> PluginManager::searchRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_searchRunnerPlugins;
}

void PluginManager::addSearchRunnerPlugin(const SearchRunnerPlugin *plugin)
{
    d->loadPlugins();
    d->m_searchRunnerPlugins << plugin;
    Q_EMIT searchRunnerPluginsChanged();
}

QList<const ReverseGeocodingRunnerPlugin *> PluginManager::reverseGeocodingRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_reverseGeocodingRunnerPlugins;
}

void PluginManager::addReverseGeocodingRunnerPlugin(const ReverseGeocodingRunnerPlugin *plugin)
{
    d->loadPlugins();
    d->m_reverseGeocodingRunnerPlugins << plugin;
    Q_EMIT reverseGeocodingRunnerPluginsChanged();
}

QList<RoutingRunnerPlugin *> PluginManager::routingRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_routingRunnerPlugins;
}

void PluginManager::addRoutingRunnerPlugin(RoutingRunnerPlugin *plugin)
{
    d->loadPlugins();
    d->m_routingRunnerPlugins << plugin;
    Q_EMIT routingRunnerPluginsChanged();
}

QList<const ParseRunnerPlugin *> PluginManager::parsingRunnerPlugins() const
{
    d->loadPlugins();
    return d->m_parsingRunnerPlugins;
}

void PluginManager::addParseRunnerPlugin(const ParseRunnerPlugin *plugin)
{
    d->loadPlugins();
    d->m_parsingRunnerPlugins << plugin;
    Q_EMIT parseRunnerPluginsChanged();
}

void PluginManager::blacklistPlugin(const QString &filename)
{
    PluginManagerPrivate::m_blacklist << QString::fromLatin1(MARBLE_SHARED_LIBRARY_PREFIX) + filename;
}

void PluginManager::whitelistPlugin(const QString &filename)
{
    PluginManagerPrivate::m_whitelist << QString::fromLatin1(MARBLE_SHARED_LIBRARY_PREFIX) + filename;
}

/** Append obj to the given plugins list if it inherits both T and U */
template<class Iface, class Plugin>
bool appendPlugin(QObject *obj, const QPluginLoader *loader, QList<Plugin> &plugins)
{
    if (qobject_cast<Iface *>(obj) && qobject_cast<Plugin>(obj)) {
        Q_ASSERT(obj->metaObject()->superClass()); // all our plugins have a super class
        mDebug() << obj->metaObject()->superClass()->className() << "plugin loaded from" << (loader ? loader->fileName() : QStringLiteral("<static>"));
        auto plugin = qobject_cast<Plugin>(obj);
        Q_ASSERT(plugin); // checked above
        plugins << plugin;
        return true;
    }

    return false;
}

bool PluginManagerPrivate::addPlugin(QObject *obj, const QPluginLoader *loader)
{
    bool isPlugin = appendPlugin<RenderPluginInterface>(obj, loader, m_renderPluginTemplates);
    isPlugin = isPlugin || appendPlugin<PositionProviderPluginInterface>(obj, loader, m_positionProviderPluginTemplates);
    isPlugin = isPlugin || appendPlugin<SearchRunnerPlugin>(obj, loader, m_searchRunnerPlugins);
    isPlugin = isPlugin || appendPlugin<ReverseGeocodingRunnerPlugin>(obj, loader, m_reverseGeocodingRunnerPlugins);
    isPlugin = isPlugin || appendPlugin<RoutingRunnerPlugin>(obj, loader, m_routingRunnerPlugins);
    isPlugin = isPlugin || appendPlugin<ParseRunnerPlugin>(obj, loader, m_parsingRunnerPlugins);
    if (!isPlugin) {
        qWarning() << "Ignoring the following plugin since it couldn't be loaded:" << (loader ? loader->fileName() : QStringLiteral("<static>"));
        mDebug() << "Plugin failure:" << (loader ? loader->fileName() : QStringLiteral("<static>")) << "is a plugin, but it does not implement the "
                 << "right interfaces or it was compiled against an old version of Marble. Ignoring it.";
    }
    return isPlugin;
}

void PluginManagerPrivate::loadPlugins()
{
    if (m_pluginsLoaded) {
        return;
    }

    QElapsedTimer t;
    t.start();
    mDebug() << "Starting to load Plugins.";

    QStringList pluginFileNameList = MarbleDirs::pluginEntryList(QString(), QDir::Files);

    MarbleDirs::debug();

    Q_ASSERT(m_renderPluginTemplates.isEmpty());
    Q_ASSERT(m_positionProviderPluginTemplates.isEmpty());
    Q_ASSERT(m_searchRunnerPlugins.isEmpty());
    Q_ASSERT(m_reverseGeocodingRunnerPlugins.isEmpty());
    Q_ASSERT(m_routingRunnerPlugins.isEmpty());
    Q_ASSERT(m_parsingRunnerPlugins.isEmpty());

    bool foundPlugin = false;
    for (const QString &fileName : pluginFileNameList) {
        QString const baseName = QFileInfo(fileName).baseName();
        QString const libBaseName = QString::fromLatin1(MARBLE_SHARED_LIBRARY_PREFIX) + QFileInfo(fileName).baseName();
        if (!m_whitelist.isEmpty() && !m_whitelist.contains(baseName) && !m_whitelist.contains(libBaseName)) {
            mDebug() << "Ignoring non-whitelisted plugin " << fileName;
            continue;
        }
        if (m_blacklist.contains(baseName) || m_blacklist.contains(libBaseName)) {
            mDebug() << "Ignoring blacklisted plugin " << fileName;
            continue;
        }

        // mDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QString const path = MarbleDirs::pluginPath(fileName);
#ifdef Q_OS_ANDROID
        QFileInfo targetFile(path);
        if (!m_pluginPaths.contains(targetFile.canonicalFilePath())) {
            // @todo Delete the file here?
            qDebug() << "Ignoring file " << path << " which is not among the currently installed plugins";
            continue;
        }
#endif
        auto loader = new QPluginLoader(path, m_parent);

        QObject *obj = loader->instance();

        if (obj) {
            bool isPlugin = addPlugin(obj, loader);
            if (!isPlugin) {
                delete loader;
            } else {
                foundPlugin = true;
            }
        } else {
            qWarning() << "Ignoring to load the following file since it doesn't look like a valid Marble plugin:" << path << Qt::endl
                       << "Reason:" << loader->errorString();
            delete loader;
        }
    }

    const auto staticPlugins = QPluginLoader::staticInstances();
    for (auto obj : staticPlugins) {
        if (addPlugin(obj, nullptr)) {
            foundPlugin = true;
        }
    }

    if (!foundPlugin) {
#ifdef Q_OS_WIN
        QString pluginPaths = "Plugin Path: " + MarbleDirs::marblePluginPath();
        if (MarbleDirs::marblePluginPath().isEmpty())
            pluginPaths = "";
        pluginPaths += "System Path: " + MarbleDirs::pluginSystemPath() + "\nLocal Path: " + MarbleDirs::pluginLocalPath();

        QMessageBox::warning(nullptr,
                             "No plugins loaded",
                             "No plugins were loaded, please check if the plugins were installed in one of the following paths:\n" + pluginPaths
                                 + "\n\nAlso check if the plugin is compiled against the right version of Marble. "
                                 + "Analyzing the debug messages inside a debugger might give more insight.");
#else
        qWarning() << "No plugins loaded. Please check if the plugins were installed in the correct path,"
                   << "or if any errors occurred while loading plugins.";
#endif
    }

    m_pluginsLoaded = true;

    mDebug() << "Time elapsed:" << t.elapsed() << "ms";
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
    for (const QString &existingPlugin : existingPlugins) {
        QFile::remove(existingPlugin);
    }

    for (const QString &file : copyList) {
        QString const target = MarbleDirs::pluginLocalPath() + QLatin1Char('/') + file;
        if (QFileInfo(MarbleDirs::pluginSystemPath() + QLatin1Char('/') + file).isDir()) {
            pluginHome.mkpath(target);
        } else {
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
