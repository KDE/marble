//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>
//

#ifndef MARBLE_CACHESTORAGEPOLICY_H
#define MARBLE_CACHESTORAGEPOLICY_H

#include "DiscCache.h"
#include "StoragePolicy.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT CacheStoragePolicy : public StoragePolicy
{
    Q_OBJECT

    public:
        /**
         * Creates a new cache storage policy.
         *
         * @param cacheDirectory The directory which shall be used for the cache.
         */
        explicit CacheStoragePolicy( const QString &cacheDirectory );

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
         * Clears the cache.
         */
	void clearCache();

        /**
         * Returns the last error message.
         */
        QString lastErrorMessage() const;

        /**
         * Returns the data of a file.
         */
        QByteArray data( const QString &fileName );

        /**
         * Sets the limit of the cache in @p bytes.
         */
        void setCacheLimit( quint64 bytes );

        /**
         * Returns the limit of the cache in bytes.
         */
        quint64 cacheLimit() const;

    private:
        DiscCache m_cache;
        QString m_errorMsg;
};

}

#endif
