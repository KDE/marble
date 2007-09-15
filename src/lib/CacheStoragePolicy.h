//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>"
//

#ifndef CACHESTORAGEPOLICY_H
#define CACHESTORAGEPOLICY_H

#include "DiscCache.h"
#include "StoragePolicy.h"

class CacheStoragePolicy : public StoragePolicy
{
    public:
        /**
         * Creates a new cache storage policy.
         *
         * @param cacheDirectory The directory which shall be used for the cache.
         */
        CacheStoragePolicy( const QString &cacheDirectory );

        /**
         * Destroys the cache storage policy.
         */
        ~CacheStoragePolicy();

        /**
         * Returns whether the @p fileName exists already.
         */
        bool fileExists( const QString &fileName ) const;

        /**
         * Updates the @p fileName with the given @p data.
         */
        bool updateFile( const QString &fileName, const QByteArray &data );

        /**
         * Returns the last error message.
         */
        QString lastErrorMessage() const;

        /**
         * Returns the data of a file.
         */
        QByteArray data( const QString &fileName );

    private:
        DiscCache m_cache;
        QString m_errorMsg;
};

#endif
