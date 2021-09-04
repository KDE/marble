// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
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
