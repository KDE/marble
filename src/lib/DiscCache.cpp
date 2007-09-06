//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>"
//

#include <QtCore/QDirIterator>
#include <QtCore/QFile>

#include "DiscCache.h"

static QString indexFileName( const QString &cacheDirectory )
{
    return cacheDirectory + "/cache_index.idx";
}

DiscCache::DiscCache( const QString &cacheDirectory )
    : mCacheDirectory( cacheDirectory ),
      mCacheLimit( 300 * 1024 * 1024 ),
      mCurrentCacheSize( 0 )
{
    Q_ASSERT( !mCacheDirectory.isEmpty() && "Passed empty cache directory!" );

    QFile file( indexFileName( mCacheDirectory ) );

    if ( file.exists() ) {
        if ( file.open( QIODevice::ReadOnly ) ) {
            QDataStream s( &file );
            s.setVersion( 8 );

            s >> mCacheLimit;
            s >> mCurrentCacheSize;
            s >> mEntries;

        } else {
            qWarning( "Unable to open cache directory %s", qPrintable( mCacheDirectory ) );
        }
    }
}

DiscCache::~DiscCache()
{
    QFile file( indexFileName( mCacheDirectory ) );

    if ( file.open( QIODevice::WriteOnly ) ) {
        QDataStream s( &file );
        s.setVersion( 8 );

        s << mCacheLimit;
        s << mCurrentCacheSize;
        s << mEntries;
    }

    file.close();
}

quint64 DiscCache::cacheLimit() const
{
    return mCacheLimit;
}

void DiscCache::clear()
{
    QDirIterator it( mCacheDirectory );

    // Remove all files from cache directory
    while ( it.hasNext() ) {
        it.next();

        if ( it.fileName() == indexFileName( mCacheDirectory ) ) // skip index file
            continue;

        QFile::remove( it.fileName() );
    }

    // Delete entries
    mEntries.clear();

    // Reset current cache size
    mCurrentCacheSize = 0;
}

bool DiscCache::exists( const QString &key ) const
{
    return mEntries.contains( key );
}

bool DiscCache::find( const QString &key, QByteArray &data )
{
    // Return error if we don't know this key
    if ( !mEntries.contains( key ) )
        return false;

    // If we can open the file, load all data and update access timestamp
    QFile file( keyToFileName( key ) );
    if ( file.open( QIODevice::ReadOnly ) ) {
        data = file.readAll();

        mEntries[ key ].first = QDateTime::currentDateTime();
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

    // If we overwrite an existing entry, substract the size first
    if ( mEntries.contains( key ) )
        mCurrentCacheSize -= mEntries.value( key ).second;

    // Store the data on disc
    file.write( data );

    // Create/Overwrite with a new entry
    mEntries.insert( key, QPair<QDateTime, quint64>(QDateTime::currentDateTime(), data.length()) );

    // Add the size of the new entry
    mCurrentCacheSize += data.length();

    cleanup();

    return true;
}

void DiscCache::remove( const QString &key )
{
    // Do nothing if we don't know the key
    if ( !mEntries.contains( key ) )
        return;

    // If we can't remove the file we don't remove
    // the entry to prevent inconsistency
    if ( !QFile::remove( keyToFileName( key ) ) )
        return;

    // Substract from current size
    mCurrentCacheSize -= mEntries.value( key ).second;

    // Finally remove entry
    mEntries.remove( key );
}

void DiscCache::setCacheLimit( quint64 n )
{
    mCacheLimit = n;

    cleanup();
}

QString DiscCache::keyToFileName( const QString &key )
{
    QString fileName( key );
    fileName.replace( "/", "_" );

    return mCacheDirectory + "/" + fileName;
}

void DiscCache::cleanup()
{
    // Calculate 5% of our current cache limit
    quint64 fivePercent = quint64( mCacheLimit * 0.05 );

    while ( mCurrentCacheSize > (mCacheLimit - fivePercent) ) {
        QDateTime oldestDate( QDateTime::currentDateTime() );
        QString oldestKey;

        QMapIterator<QString, QPair<QDateTime, quint64> > it( mEntries );
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
