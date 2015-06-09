//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "jobparameters.h"

JobParameters::JobParameters() : m_cacheData(false)
{
    // nothing to do
}

QDir JobParameters::base()
{
    return m_base;
}

void JobParameters::setBase(const QDir &dir)
{
    m_base = dir;
}

bool JobParameters::cacheData() const
{
    return m_cacheData;
}

void JobParameters::setCacheData(bool cache)
{
    m_cacheData = cache;
}
