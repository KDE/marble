//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>
//


// Own
#include "CacheStoragePolicy.h"

// Qt
#include <QtCore/QDir>

using namespace Marble;

CacheStoragePolicy::CacheStoragePolicy( const QString &cacheDirectory )
    : m_cache( cacheDirectory )
{
    if ( ! QDir( cacheDirectory ).exists() ) 
        QDir::root().mkpath( cacheDirectory );
}

CacheStoragePolicy::~CacheStoragePolicy()
{
}

bool CacheStoragePolicy::fileExists( const QString &fileName ) const
{
    return m_cache.exists( fileName );
}

bool CacheStoragePolicy::updateFile( const QString &fileName, const QByteArray &data )
{
    if ( !m_cache.insert( fileName, data ) ) {
        m_errorMsg = QObject::tr("Unable to insert data into cache");
        return false;
    }

    return true;
}

void CacheStoragePolicy::clearCache()
{
    m_cache.clear();
}

QString CacheStoragePolicy::lastErrorMessage() const
{
    return m_errorMsg;
}

QByteArray CacheStoragePolicy::data( const QString &fileName )
{
    QByteArray data;
    m_cache.find( fileName, data );

    return data;
}

void CacheStoragePolicy::setCacheLimit( quint64 bytes )
{
    m_cache.setCacheLimit( bytes );
}

quint64 CacheStoragePolicy::cacheLimit() const
{
    return m_cache.cacheLimit();
}

#include "CacheStoragePolicy.moc"
