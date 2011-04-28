//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmDatabase.h"
#include "OsmRegion.h"
#include "MarbleDebug.h"

#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtCore/QStringList>

namespace Marble {

namespace {
    quint32 const osmDatabaseFileMagicNumber = 0xAEB4E6D8;
    quint32 const osmDatabaseFileVersion = 160;
}

OsmDatabase::OsmDatabase()
{
    // nothing to do
}

void OsmDatabase::addFile( const QString &fileName )
{
    QFile file( fileName );
    file.open( QFile::ReadOnly );

    QDataStream stream( &file );

    // Read and check the header
    quint32 magic;
    stream >> magic;
    if ( magic != osmDatabaseFileMagicNumber ) {
        mDebug() << "Wrong magic number:" << fileName << "is not an osm database file.";
        return;
    }
    quint32 version;
    stream >> version; // not needed atm
    stream.setVersion( QDataStream::Qt_4_0 );

    /** @todo: Use a B-tree or similar */
    stream >> m_regions;
    stream >> m_placemarks;

    file.close();
}

void OsmDatabase::save( const QString &filename )
{
    QFile file( filename );
    file.open( QFile::WriteOnly );

    QDataStream stream( &file );

    // Write a header with a "magic number" and a version
    stream << (quint32) osmDatabaseFileMagicNumber;
    stream << (quint32) osmDatabaseFileVersion;
    stream.setVersion( QDataStream::Qt_4_0 );

    /** @todo: Use a B-tree or similar */
    stream << m_regions;
    qSort( m_placemarks );
    stream << m_placemarks;

    file.close();
}

void OsmDatabase::addOsmRegion( const OsmRegion &region )
{
    m_regions.push_back( region );
}

void OsmDatabase::addOsmPlacemark( const OsmPlacemark &placemark )
{
    m_placemarks.push_back( placemark );
}

QList<OsmPlacemark> OsmDatabase::find( const QString &searchTerm ) const
{
    QStringList const terms = searchTerm.split( ",", QString::SkipEmptyParts );
    if ( terms.size() == 1 ) {
        return findOsmTerm( terms.first().trimmed() );
    } else if ( terms.size() == 2 ) {
        return findStreets( terms.at( 1 ).trimmed(), terms.first().trimmed() );
    } else {
        // Search terms with more than one ',' silently ignored for now
    }

    /** @todo: alternative words, etc. */

    return QList<OsmPlacemark>();
}

QList<OsmPlacemark> OsmDatabase::findOsmTerm( const QString &term ) const
{
    QList<OsmPlacemark> result;
    foreach( const OsmRegion &region, m_regions ) {
        if ( region.name().startsWith( term, Qt::CaseInsensitive ) ) {
            OsmPlacemark placemark;
            placemark.setLongitude( region.longitude() );
            placemark.setLatitude( region.latitude() );
            placemark.setName( region.name() );
            result << placemark;
        }
    }

    foreach( const OsmPlacemark &placemark, m_placemarks ) {
        if ( placemark.name().startsWith( term, Qt::CaseInsensitive ) ) {
            result << placemark;
        }
    }

    return result;
}

QList<OsmPlacemark> OsmDatabase::findStreets( const QString &reg, const QString &street ) const
{
    QList<OsmPlacemark> result;
    QList<int> regions;
    foreach( const OsmRegion &region, m_regions ) {
        if ( region.name().startsWith( reg, Qt::CaseInsensitive ) ) {
            regions << region.identifier();
        }
    }

    foreach( const OsmPlacemark &placemark, m_placemarks ) {
        if ( regions.contains( placemark.regionId() ) && placemark.name().startsWith( street, Qt::CaseInsensitive ) ) {
            result.push_back( placemark );
        }
    }

    return result;
}

}
