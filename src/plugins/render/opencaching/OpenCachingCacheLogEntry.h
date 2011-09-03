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

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QVariant>

namespace Marble
{

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
    long m_cacheId;

    QString m_userName;

    QString m_logType;

    QString m_text;

    QDateTime m_logDate;

    QDateTime m_createdDate;

    QDateTime m_lastModifiedDate;
};

}

#endif // OPENCACHINGCACHELOGENTRY_H
