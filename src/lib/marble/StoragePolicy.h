// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_STORAGEPOLICY_H
#define MARBLE_STORAGEPOLICY_H

#include <QObject>

class QByteArray;
class QString;

namespace Marble
{

class StoragePolicy : public QObject
{
    Q_OBJECT

public:
    explicit StoragePolicy(QObject *parent = nullptr);

    ~StoragePolicy() override = default;

    virtual bool fileExists(const QString &fileName) const = 0;

    /**
     * Return true if file was written successfully.
     */
    virtual bool updateFile(const QString &fileName, const QByteArray &data) = 0;

    virtual void clearCache() = 0;

    virtual QString lastErrorMessage() const = 0;

Q_SIGNALS:
    void cleared();
    void sizeChanged(qint64);

private:
    Q_DISABLE_COPY(StoragePolicy)
};

}

#endif
