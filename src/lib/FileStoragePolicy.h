//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>"
//

#ifndef FILESTORAGEPOLICY_H
#define FILESTORAGEPOLICY_H

#include "StoragePolicy.h"

class FileStoragePolicy : public StoragePolicy
{
    public:
        /**
         * Creates a new file storage policy.
         *
         * @param dataDirectory The directory where the data should go to.
         */
        FileStoragePolicy( const QString &dataDirectory = QString() );

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
         * Returns the last error message.
         */
        QString lastErrorMessage() const;

    private:
        QString m_dataDirectory;
        QString m_errorMsg;
};

#endif
