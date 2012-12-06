//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright (C) 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoSceneLicense.h"

namespace Marble
{

QString GeoSceneLicense::license() const
{
    return m_fullLicense;
}

QString GeoSceneLicense::shortLicense() const
{
    return m_shortLicense;
}

void GeoSceneLicense::setLicense( const QString &license )
{
    m_fullLicense = license;
}

void GeoSceneLicense::setShortLicense( const QString &license )
{
    m_shortLicense = license;
}

}
