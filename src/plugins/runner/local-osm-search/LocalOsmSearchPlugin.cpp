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
}

MarbleAbstractRunner* LocalOsmSearchPlugin::newRunner() const
{
    if ( !m_databaseLoaded ) {
        m_databaseLoaded = true;
        QString base = MarbleDirs::localPath() + "/placemarks/";
        QDir::Filters filters = QDir::AllDirs | QDir::Readable | QDir::NoDotDot;
        QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories | QDirIterator::FollowSymlinks;
        QDirIterator iter( base, filters, flags );
        while ( iter.hasNext() ) {
            iter.next();
            QDir directory( iter.filePath() );
            QStringList const nameFilters = QStringList() << "*.sqlite";
            QStringList const files( directory.entryList( nameFilters, QDir::Files ) );
            foreach( const QString &file, files ) {
                m_database.addFile( directory.filePath( file ) );
            }
        }
    }

    return new LocalOsmSearchRunner( &m_database );
}

}

Q_EXPORT_PLUGIN2( LocalOsmSearchPlugin, Marble::LocalOsmSearchPlugin )

#include "LocalOsmSearchPlugin.moc"
