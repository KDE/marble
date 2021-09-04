//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#ifndef MARBLE_LOCALOSMSEARCHPLUGIN_H
#define MARBLE_LOCALOSMSEARCHPLUGIN_H

#include "SearchRunnerPlugin.h"
#include "OsmDatabase.h"

#include <QFileSystemWatcher>

namespace Marble
{

class LocalOsmSearchPlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.LocalOsmSearchPlugin")
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit LocalOsmSearchPlugin( QObject *parent = nullptr );

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    SearchRunner* newRunner() const override;

private Q_SLOTS:
    void updateDirectory( const QString &directory );

    void updateFile( const QString &directory );

private:
    void addDatabaseDirectory( const QString &path );

    void updateDatabase();

    QStringList m_databaseFiles;
    QFileSystemWatcher m_watcher;
};

}

#endif
