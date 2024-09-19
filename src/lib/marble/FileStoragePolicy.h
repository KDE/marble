// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
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
     * @param parent The parent object.
     */
    explicit FileStoragePolicy(const QString &dataDirectory = QString(), QObject *parent = nullptr);

    /**
     * Destroys the cache storage policy.
     */
    ~FileStoragePolicy() override;

    /**
     * Returns whether the @p fileName exists already.
     */
    bool fileExists(const QString &fileName) const override;

    /**
     * Updates the @p fileName with the given @p data.
     */
    bool updateFile(const QString &fileName, const QByteArray &data) override;

    /**
     * Clears the cache.
     */
    void clearCache() override;

    /**
     * Returns the last error message.
     */
    QString lastErrorMessage() const override;

private:
    Q_DISABLE_COPY(FileStoragePolicy)

    QString m_dataDirectory;
    QString m_errorMsg;
};

}

#endif
