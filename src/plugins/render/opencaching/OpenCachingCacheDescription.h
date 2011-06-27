//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGCACHEDESCRIPTION_H
#define OPENCACHINGCACHEDESCRIPTION_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QVariant>

namespace Marble
{

class OpenCachingCacheDescription
{
public:    
    OpenCachingCacheDescription( const QHash<QString, QVariant>& properties = QHash<QString, QVariant>() );

    void setCacheId( int cacheId );

    int cacheId() const;

    void setLanguage( const QString& language );

    const QString& language() const;

    void setShortDescription( const QString& shortDescription );

    const QString& shortDescription() const;

    void setDescription( const QString& description );

    const QString& description() const;

    void setHint( const QString& hint );

    const QString& hint() const;

    void setLastModifiedDate( const QDateTime& lastModifiedDate );

    const QDateTime& lastModifiedDate() const;

private:
    int m_cacheId;

    QString m_language;

    QString m_shortDescription;

    QString m_description;

    QString m_hint;

    QDateTime m_lastModifiedDate;
};

}

#endif // OPENCACHINGCACHEDESCRIPTION_H
