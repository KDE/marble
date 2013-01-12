//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataUpdate.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataUpdatePrivate
{
public:
    GeoDataUpdatePrivate();

    QString m_targetHref;
};

GeoDataUpdatePrivate::GeoDataUpdatePrivate() :
    m_targetHref( "" )
{
}

GeoDataUpdate::GeoDataUpdate() :
    d( new GeoDataUpdatePrivate )
{
}

GeoDataUpdate::GeoDataUpdate( const Marble::GeoDataUpdate &other ) :
    GeoDataObject(), d( new GeoDataUpdatePrivate( *other.d ) )
{
}

GeoDataUpdate &GeoDataUpdate::operator=( const GeoDataUpdate &other )
{
    GeoDataObject::operator =( other );
    *d = *other.d;
    return *this;
}

GeoDataUpdate::~GeoDataUpdate()
{
    delete d;
}

const char *GeoDataUpdate::nodeType() const
{
    return GeoDataTypes::GeoDataUpdateType;
}

QString GeoDataUpdate::targetHref() const
{
    return d->m_targetHref;
}

void GeoDataUpdate::setTargetHref( const QString &targetHref )
{
    d->m_targetHref = targetHref;
}

}
