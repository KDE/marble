// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGCACHEDESCRIPTION_H
#define OPENCACHINGCACHEDESCRIPTION_H

#include <QDateTime>
#include <QHash>
#include <QString>
#include <QVariant>

namespace Marble
{

/**
 * Stores the description of a cache in a single language.
 */
class OpenCachingCacheDescription
{
public:
    explicit OpenCachingCacheDescription(const QHash<QString, QVariant> &properties = QHash<QString, QVariant>());

    void setCacheId(unsigned long long cacheId);

    unsigned long long cacheId() const;

    void setLanguage(const QString &language);

    const QString &language() const;

    void setShortDescription(const QString &shortDescription);

    const QString &shortDescription() const;

    void setDescription(const QString &description);

    const QString &description() const;

    void setHint(const QString &hint);

    const QString &hint() const;

    void setLastModifiedDate(const QDateTime &lastModifiedDate);

    const QDateTime &lastModifiedDate() const;

private:
    unsigned long long m_cacheId; ///< Unique ID of the cache.

    QString m_language; ///< Language of the description.

    QString m_shortDescription; ///< Short description of the cache.

    QString m_description; ///< Actual description of the cache.

    QString m_hint; ///< Hint to find the cache.

    QDateTime m_lastModifiedDate; ///< Date the description was last modified.
};

}

#endif // OPENCACHINGCACHEDESCRIPTION_H
