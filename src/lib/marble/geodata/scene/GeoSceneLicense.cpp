// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoSceneLicense.h"

#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneLicense::GeoSceneLicense() :
    m_attribution( OptOut )
{
    // nothing to do
}

QString GeoSceneLicense::license() const
{
    return m_fullLicense.isEmpty() ? m_shortLicense : m_fullLicense;
}

QString GeoSceneLicense::shortLicense() const
{
    return m_shortLicense.isEmpty() ? m_fullLicense : m_shortLicense;
}

GeoSceneLicense::Attribution GeoSceneLicense::attribution() const
{
    return m_attribution;
}

void GeoSceneLicense::setLicense(const QString &license )
{
    m_fullLicense = license;
}

void GeoSceneLicense::setShortLicense( const QString &license )
{
    m_shortLicense = license;
}

void GeoSceneLicense::setAttribution(Attribution attr )
{
    m_attribution = attr;
}

const char *GeoSceneLicense::nodeType() const
{
    return GeoSceneTypes::GeoSceneLicenseType;
}

}
