//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "LocalOsmSearchPlugin.h"
#include "LocalOsmSearchRunner.h"
#include "MarbleDirs.h"

#include <QDirIterator>

namespace Marble
{

LocalOsmSearchPlugin::LocalOsmSearchPlugin( QObject *parent ) :
    SearchRunnerPlugin( parent ),
    m_databaseFiles()
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( true );

    QString const path = MarbleDirs::localPath() + QLatin1String("/maps/earth/placemarks/");
    QFileInfo pathInfo( path );
    if ( !pathInfo.exists() ) {
        QDir("/").mkpath( pathInfo.absolutePath() );
        pathInfo.refresh();
    }
    if ( pathInfo.exists() ) {
        m_watcher.addPath( path );
    }
    connect( &m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(updateDirectory(QString)) );
    connect( &m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(updateFile(QString)) );

    updateDatabase();
}

QString LocalOsmSearchPlugin::name() const
{
    return tr( "Local OSM Search" );
}

QString LocalOsmSearchPlugin::guiString() const
{
    return tr( "Offline OpenStreetMap Search" );
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
    return tr( "Searches for addresses and points of interest in offline maps." );
}

QString LocalOsmSearchPlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> LocalOsmSearchPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

SearchRunner* LocalOsmSearchPlugin::newRunner() const
{
    return new LocalOsmSearchRunner( m_databaseFiles );
}

void LocalOsmSearchPlugin::addDatabaseDirectory( const QString &path )
{
    QDir directory( path );
    QStringList const nameFilters = QStringList() << "*.sqlite";
    QStringList const files( directory.entryList( nameFilters, QDir::Files ) );
    foreach( const QString &file, files ) {
        m_databaseFiles << directory.filePath( file );
    }
}

void LocalOsmSearchPlugin::updateDirectory( const QString & )
{
    updateDatabase();
}

void LocalOsmSearchPlugin::updateFile( const QString &file )
{
    if ( file.endsWith( QLatin1String( ".sqlite" ) ) ) {
        updateDatabase();
    }
}

void LocalOsmSearchPlugin::updateDatabase()
{
    m_databaseFiles.clear();
    QStringList const baseDirs = QStringList() << MarbleDirs::systemPath() << MarbleDirs::localPath();
    foreach ( const QString &baseDir, baseDirs ) {
        const QString base = baseDir + QLatin1String("/maps/earth/placemarks/");
        addDatabaseDirectory( base );
        QDir::Filters filters = QDir::AllDirs | QDir::Readable | QDir::NoDotAndDotDot;
        QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories | QDirIterator::FollowSymlinks;
        QDirIterator iter( base, filters, flags );
        while ( iter.hasNext() ) {
            iter.next();
            addDatabaseDirectory( iter.filePath() );
        }
    }
}

}

#include "moc_LocalOsmSearchPlugin.cpp"
