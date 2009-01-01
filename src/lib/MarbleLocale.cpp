//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleLocale.h"
#include "MarbleLocale_p.h"


namespace Marble {

MarbleLocalePrivate::MarbleLocalePrivate()
    : m_distanceUnit( Marble::Metric )
{
}

MarbleLocalePrivate::~MarbleLocalePrivate()
{
}

MarbleLocale::MarbleLocale()
    : d ( new MarbleLocalePrivate )
{
}

MarbleLocale::~MarbleLocale()
{
    delete d;
}

Marble::DistanceUnit MarbleLocale::distanceUnit() const
{
    return d->m_distanceUnit;
}

void MarbleLocale::setDistanceUnit( Marble::DistanceUnit distanceUnit )
{
    d->m_distanceUnit = distanceUnit;
}

}
