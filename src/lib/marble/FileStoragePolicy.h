//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>
//

#ifndef MARBLE_FILESTORAGEPOLICY_H
#define MARBLE_FILESTORAGEPOLICY_H

#include "StoragePolicy.h"

namespace Marble
{

class FileStoragePolicy : public StoragePolicy
{
    Q_OBJECT
    
    public:
        /**
         * Creates a new file storage policy.
         *
         * @param dataDirectory The directory where the data should go to.
         */
        explicit FileStoragePolicy( const QString &dataDirectory = QString(), QObject *parent = 0 );

        /**
         * Destroys the cache storage policy.
         */
        ~FileStoragePolicy();

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

    private:
	Q_DISABLE_COPY( FileStoragePolicy )
	
        QString m_dataDirectory;
        QString m_errorMsg;
};

}

#endif
