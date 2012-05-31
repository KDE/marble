//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_LOCALOSMSEARCHPLUGIN_H
#define MARBLE_LOCALOSMSEARCHPLUGIN_H

#include "SearchRunnerPlugin.h"
#include "OsmDatabase.h"

#include <QtCore/QFileSystemWatcher>

namespace Marble
{

class LocalOsmSearchPlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit LocalOsmSearchPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    virtual MarbleAbstractRunner* newRunner() const;

private Q_SLOTS:
    void updateDirectory( const QString &directory ) const;

    void updateFile( const QString &directory ) const;

private:
    void addDatabaseDirectory( const QString &path ) const;

    void updateDatabase() const;

    /** @todo: FIXME newRunner() is another virtual method that shouldn't be const */
    mutable bool m_databaseLoaded;
    mutable OsmDatabase m_database;
    QFileSystemWatcher m_watcher;
};

}

#endif
