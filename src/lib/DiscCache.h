//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>"
//

#ifndef DISCCACHE_H
#define DISCCACHE_H

#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QString>

class DiscCache
{
    public:
        DiscCache( const QString &cacheDirectory );
        ~DiscCache();

        quint64 cacheLimit() const;
        void clear();
        bool exists( const QString &key ) const;
        bool find( const QString &key, QByteArray &data );
        bool insert( const QString &key, const QByteArray &data );
        void remove( const QString &key );
        void setCacheLimit( quint64 n );

    private:        
        QString keyToFileName( const QString& );
        void initialScan( const QStringList& );
        void cleanup();

        QString m_CacheDirectory;
        quint64 m_CacheLimit;
        quint64 m_CurrentCacheSize;

        typedef QPair<QDateTime, quint64> Entry;
        QMap<QString, Entry> m_Entries;
};

#endif
