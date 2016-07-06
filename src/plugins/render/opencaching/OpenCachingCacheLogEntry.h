//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGCACHELOGENTRY_H
#define OPENCACHINGCACHELOGENTRY_H

#include <QString>
#include <QDateTime>
#include <QHash>

class QVariant;

namespace Marble
{

/**
 * Single log entry for a cache.
 */
class OpenCachingCacheLogEntry
{
public:
    OpenCachingCacheLogEntry( const QHash<QString, QVariant>& properties );

    void setCacheId( int cacheId );

    int cacheId() const;

    void setUserName( const QString& userName );

    const QString& userName() const;

    void setLogType( const QString& logType );

    const QString& logType() const;

    void setText( const QString& text );

    const QString& text() const;

    void setLogDate( const QDateTime& logDate );

    const QDateTime& logDate() const;

    void setCreatedDate( const QDateTime& createdDate );

    const QDateTime& createdDate() const;

    void setLastModifiedDate( const QDateTime& lastModifiedDate );

    const QDateTime& lastModifiedDate() const;

private:
    long m_cacheId;                      ///< Unique ID of the cache.

    QString m_userName;                  ///< Name of the user that created the log entry.

    QString m_logType;                   ///< Type of the log. @todo ?

    QString m_text;                      ///< Actual text of the log.

    QDateTime m_logDate;                 ///< Date the cache was found.

    QDateTime m_createdDate;             ///< Date the log was created.

    QDateTime m_lastModifiedDate;        ///< Date the log was last modified.
};

}

#endif // OPENCACHINGCACHELOGENTRY_H
