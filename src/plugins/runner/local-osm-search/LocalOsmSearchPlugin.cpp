//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "LocalOsmSearchPlugin.h"
#include "LocalOsmSearchRunner.h"
#include "MarbleDirs.h"

#include <QtCore/QDirIterator>

namespace Marble
{

LocalOsmSearchPlugin::LocalOsmSearchPlugin( QObject *parent ) :
    RunnerPlugin( parent ),
    m_databaseLoaded( false )
{
    setCapabilities( Search );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Local OSM Search" ) );
    setNameId( "local-osm-search" );
    setDescription( tr( "Searches for addresses and points of interest in offline maps." ) );
    setGuiString( tr( "Offline OpenStreetMap Search")  );

    m_watcher.addPath( MarbleDirs::localPath() + "/maps/earth/placemarks/" );
    connect( &m_watcher, SIGNAL( directoryChanged( QString ) ), this, SLOT( updateDirectory( QString ) ) );
    connect( &m_watcher, SIGNAL( fileChanged( QString ) ), this, SLOT( updateFile( QString ) ) );
}

MarbleAbstractRunner* LocalOsmSearchPlugin::newRunner() const
{
    if ( !m_databaseLoaded ) {
        m_databaseLoaded = true;
        updateDatabase();
    }
    return new LocalOsmSearchRunner( &m_database );
}

void LocalOsmSearchPlugin::addDatabaseDirectory( const QString &path ) const
{
    QDir directory( path );
    QStringList const nameFilters = QStringList() << "*.sqlite";
    QStringList const files( directory.entryList( nameFilters, QDir::Files ) );
    foreach( const QString &file, files ) {
        m_database.addFile( directory.filePath( file ) );
    }
}

void LocalOsmSearchPlugin::updateDirectory( const QString & ) const
{
    m_databaseLoaded = false;
}

void LocalOsmSearchPlugin::updateFile( const QString &file ) const
{
    if ( file.endsWith( ".sqlite" ) ) {
        m_databaseLoaded = false;
    }
}

void LocalOsmSearchPlugin::updateDatabase() const
{
    m_database.clear();
    QStringList const baseDirs = QStringList() << MarbleDirs::systemPath() << MarbleDirs::localPath();
    foreach ( const QString &baseDir, baseDirs ) {
        QString base = baseDir + "/maps/earth/placemarks/";
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

Q_EXPORT_PLUGIN2( LocalOsmSearchPlugin, Marble::LocalOsmSearchPlugin )

#include "LocalOsmSearchPlugin.moc"
