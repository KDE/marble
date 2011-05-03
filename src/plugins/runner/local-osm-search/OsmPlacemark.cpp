//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmPlacemark.h"

namespace Marble {

OsmPlacemark::OsmPlacemark() : m_regionId( 0 ),
    m_category( UnknownCategory ),
    m_longitude( 0.0 ), m_latitude( 0.0 )
{
    // nothing to do
}

OsmPlacemark::OsmCategory OsmPlacemark::category() const
{
    return m_category;
}

void OsmPlacemark::setCategory( OsmCategory category )
{
    m_category = category;
}

QString OsmPlacemark::name() const
{
    return m_name;
}

void OsmPlacemark::setName( const QString &name )
{
    m_name = name;
}

QString OsmPlacemark::houseNumber() const
{
    return m_houseNumber;
}

void OsmPlacemark::setHouseNumber( const QString &houseNumber )
{
    m_houseNumber = houseNumber;
}

int OsmPlacemark::regionId() const
{
    return m_regionId;
}

void OsmPlacemark::setRegionId( int id )
{
    m_regionId = id;
}

QString OsmPlacemark::additionalInformation() const
{
    return m_additionalInformation;
}

void OsmPlacemark::setAdditionalInformation( const QString &name )
{
    m_additionalInformation = name;
}

qreal OsmPlacemark::longitude() const
{
    return m_longitude;
}

void OsmPlacemark::setLongitude( qreal longitude )
{
    m_longitude = longitude;
}

qreal OsmPlacemark::latitude() const
{
    return m_latitude;
}

void OsmPlacemark::setLatitude( qreal latitude )
{
    m_latitude = latitude;
}


bool OsmPlacemark::operator<( const OsmPlacemark &other) const
{
    if ( name() != other.name() ) {
        return name() < other.name();
    }

    if ( regionId() != other.regionId() ) {
        return regionId() < other.regionId();
    }

    if ( longitude() != other.longitude() ) {
        return longitude() < other.longitude();
    }

    return latitude() < other.latitude();
}

}
