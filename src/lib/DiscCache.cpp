//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>
//


// Own
#include "DiscCache.h"

// Qt
#include <QtCore/QtGlobal>
#include <QtCore/QFile>
#include <QtCore/QDirIterator>

using namespace Marble;

static QString indexFileName( const QString &cacheDirectory )
{
    return cacheDirectory + "/cache_index.idx";
}

DiscCache::DiscCache( const QString &cacheDirectory )
    : m_CacheDirectory( cacheDirectory ),
      m_CacheLimit( 300 * 1024 * 1024 ),
      m_CurrentCacheSize( 0 )
{
    Q_ASSERT( !m_CacheDirectory.isEmpty() && "Passed empty cache directory!" );

    QFile file( indexFileName( m_CacheDirectory ) );

    if ( file.exists() ) {
        if ( file.open( QIODevice::ReadOnly ) ) {
            QDataStream s( &file );
            s.setVersion( 8 );

            s >> m_CacheLimit;
            s >> m_CurrentCacheSize;
            s >> m_Entries;

        } else {
            qWarning( "Unable to open cache directory %s", qPrintable( m_CacheDirectory ) );
        }
    }
}

DiscCache::~DiscCache()
{
    QFile file( indexFileName( m_CacheDirectory ) );

    if ( file.open( QIODevice::WriteOnly ) ) {
        QDataStream s( &file );
        s.setVersion( 8 );

        s << m_CacheLimit;
        s << m_CurrentCacheSize;
        s << m_Entries;
    }

    file.close();
}

quint64 DiscCache::cacheLimit() const
{
    return m_CacheLimit;
}

void DiscCache::clear()
{
    QDirIterator it( m_CacheDirectory );

    // Remove all files from cache directory
    while ( it.hasNext() ) {
        it.next();

        if ( it.fileName() == indexFileName( m_CacheDirectory ) ) // skip index file
            continue;

        QFile::remove( it.fileName() );
    }

    // Delete entries
    m_Entries.clear();

    // Reset current cache size
    m_CurrentCacheSize = 0;
}

bool DiscCache::exists( const QString &key ) const
{
    return m_Entries.contains( key );
}

bool DiscCache::find( const QString &key, QByteArray &data )
{
    // Return error if we don't know this key
    if ( !m_Entries.contains( key ) )
        return false;

    // If we can open the file, load all data and update access timestamp
    QFile file( keyToFileName( key ) );
    if ( file.open( QIODevice::ReadOnly ) ) {
        data = file.readAll();

        m_Entries[ key ].first = QDateTime::currentDateTime();
        return true;
    }

    return false;
}

bool DiscCache::insert( const QString &key, const QByteArray &data )
{
    // If we can't open/create a file for this entry signal an error
    QFile file( keyToFileName( key ) );
    if ( !file.open( QIODevice::WriteOnly ) )
        return false;

    // If we overwrite an existing entry, subtract the size first
    if ( m_Entries.contains( key ) )
        m_CurrentCacheSize -= m_Entries.value( key ).second;

    // Store the data on disc
    file.write( data );

    // Create/Overwrite with a new entry
    m_Entries.insert( key, QPair<QDateTime, quint64>(QDateTime::currentDateTime(), data.length()) );

    // Add the size of the new entry
    m_CurrentCacheSize += data.length();

    cleanup();

    return true;
}

void DiscCache::remove( const QString &key )
{
    // Do nothing if we don't know the key
    if ( !m_Entries.contains( key ) )
        return;

    // If we can't remove the file we don't remove
    // the entry to prevent inconsistency
    if ( !QFile::remove( keyToFileName( key ) ) )
        return;

    // Subtract from current size
    m_CurrentCacheSize -= m_Entries.value( key ).second;

    // Finally remove entry
    m_Entries.remove( key );
}

void DiscCache::setCacheLimit( quint64 n )
{
    m_CacheLimit = n;

    cleanup();
}

QString DiscCache::keyToFileName( const QString &key )
{
    QString fileName( key );
    fileName.replace( '/', '_' );

    return m_CacheDirectory + '/' + fileName;
}

void DiscCache::cleanup()
{
    // Calculate 5% of our current cache limit
    quint64 fivePercent = quint64( m_CacheLimit * 0.05 );

    while ( m_CurrentCacheSize > (m_CacheLimit - fivePercent) ) {
        QDateTime oldestDate( QDateTime::currentDateTime() );
        QString oldestKey;

        QMapIterator<QString, QPair<QDateTime, quint64> > it( m_Entries );
        while ( it.hasNext() ) {
            it.next();

            if ( it.value().first < oldestDate ) {
                oldestDate = it.value().first;
                oldestKey = it.key();
            }
        }

        if ( !oldestKey.isEmpty() ) {
            // We found the oldest key, so using remove() to
            // remove it from cache
            remove( oldestKey );
        }
    }
}
