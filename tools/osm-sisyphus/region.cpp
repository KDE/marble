//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "region.h"

Region::Region()
{
    // nothing to do
}

QString Region::name() const
{
    return m_name;
}

QString Region::continent() const
{
    return m_continent;
}

QString Region::country() const
{
    return m_country;
}

void Region::setName(QString arg)
{
    m_name = arg;
}

void Region::setContinent(QString arg)
{
    m_continent = arg;
}

void Region::setCountry(QString arg)
{
    m_country = arg;
}

QString Region::id() const
{
    return m_id;
}

void Region::setId(const QString &id)
{
    m_id = id;
}

QString Region::pbfFile() const
{
    return m_pbfFile;
}

void Region::setPbfFile(const QString &pbfFile)
{
    m_pbfFile = pbfFile;
}

QString Region::path() const
{
    return m_path;
}

void Region::setPath(const QString &path)
{
    m_path = path;
}

bool Region::operator ==(const Region &other) const
{
    return m_continent == other.m_continent &&
           m_country == other.m_country &&
           m_name == other.m_name &&
           m_id == other.m_id &&
           m_path == other.m_path &&
           m_pbfFile == other.m_pbfFile;
}
