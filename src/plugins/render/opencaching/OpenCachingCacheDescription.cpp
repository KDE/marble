//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingCacheDescription.h"

namespace Marble
{

OpenCachingCacheDescription::OpenCachingCacheDescription( const QHash<QString, QVariant>& properties  )
{
    m_cacheId = properties["cacheid"].toULongLong();
    m_language = properties["language"].toString();
    m_shortDescription = properties["shortdesc"].toString();
    m_description = properties["desc"].toString();
    m_hint = properties["hint"].toString();
    m_lastModifiedDate = properties["lastmodified"].toDateTime();
}

void OpenCachingCacheDescription::setCacheId( unsigned long long cacheId )
{
    m_cacheId = cacheId;
}

unsigned long long OpenCachingCacheDescription::cacheId() const
{
    return m_cacheId;
}

void OpenCachingCacheDescription::setLanguage( const QString& language )
{
    m_language = language;
}

const QString& OpenCachingCacheDescription::language() const
{
    return m_language;
}

void OpenCachingCacheDescription::setShortDescription( const QString& shortDescription )
{
    m_shortDescription = shortDescription;
}

const QString& OpenCachingCacheDescription::shortDescription() const
{
    return m_shortDescription;
}

void OpenCachingCacheDescription::setDescription( const QString& description )
{
    m_description = description;
}

const QString& OpenCachingCacheDescription::description() const
{
    return m_description;
}

void OpenCachingCacheDescription::setHint( const QString& hint )
{
    m_hint = hint;
}

const QString& OpenCachingCacheDescription::hint() const
{
    return m_hint;
}

void OpenCachingCacheDescription::setLastModifiedDate( const QDateTime& lastModifiedDate )
{
    m_lastModifiedDate = lastModifiedDate;
}

const QDateTime& OpenCachingCacheDescription::lastModifiedDate() const
{
    return m_lastModifiedDate;
}

}
