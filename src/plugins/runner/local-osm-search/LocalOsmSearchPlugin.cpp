// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "LocalOsmSearchPlugin.h"
#include "LocalOsmSearchRunner.h"
#include "MarbleDirs.h"

#include <QDirIterator>

namespace Marble
{

LocalOsmSearchPlugin::LocalOsmSearchPlugin(QObject *parent)
    : SearchRunnerPlugin(parent)
    , m_databaseFiles()
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline(true);

    QString const path = MarbleDirs::localPath() + QLatin1StringView("/maps/earth/placemarks/");
    QFileInfo pathInfo(path);
    if (!pathInfo.exists()) {
        QDir(QStringLiteral("/")).mkpath(pathInfo.absolutePath());
        pathInfo.refresh();
    }
    if (pathInfo.exists()) {
        m_watcher.addPath(path);
    }
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &LocalOsmSearchPlugin::updateDirectory);
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &LocalOsmSearchPlugin::updateFile);

    updateDatabase();
}

QString LocalOsmSearchPlugin::name() const
{
    return tr("Local OSM Search");
}

QString LocalOsmSearchPlugin::guiString() const
{
    return tr("Offline OpenStreetMap Search");
}

QString LocalOsmSearchPlugin::nameId() const
{
    return QStringLiteral("local-osm-search");
}

QString LocalOsmSearchPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString LocalOsmSearchPlugin::description() const
{
    return tr("Searches for addresses and points of interest in offline maps.");
}

QString LocalOsmSearchPlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QList<PluginAuthor> LocalOsmSearchPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

SearchRunner *LocalOsmSearchPlugin::newRunner() const
{
    return new LocalOsmSearchRunner(m_databaseFiles);
}

void LocalOsmSearchPlugin::addDatabaseDirectory(const QString &path)
{
    QDir directory(path);
    QStringList const nameFilters = QStringList() << QStringLiteral("*.sqlite");
    QStringList const files(directory.entryList(nameFilters, QDir::Files));
    for (const QString &file : files) {
        m_databaseFiles << directory.filePath(file);
    }
}

void LocalOsmSearchPlugin::updateDirectory(const QString &)
{
    updateDatabase();
}

void LocalOsmSearchPlugin::updateFile(const QString &file)
{
    if (file.endsWith(QLatin1StringView(".sqlite"))) {
        updateDatabase();
    }
}

void LocalOsmSearchPlugin::updateDatabase()
{
    m_databaseFiles.clear();
    QStringList const baseDirs = QStringList() << MarbleDirs::systemPath() << MarbleDirs::localPath();
    for (const QString &baseDir : baseDirs) {
        const QString base = baseDir + QLatin1StringView("/maps/earth/placemarks/");
        addDatabaseDirectory(base);
        QDir::Filters filters = QDir::AllDirs | QDir::Readable | QDir::NoDotAndDotDot;
        QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories | QDirIterator::FollowSymlinks;
        QDirIterator iter(base, filters, flags);
        while (iter.hasNext()) {
            iter.next();
            addDatabaseDirectory(iter.filePath());
        }
    }
}

}

#include "moc_LocalOsmSearchPlugin.cpp"
