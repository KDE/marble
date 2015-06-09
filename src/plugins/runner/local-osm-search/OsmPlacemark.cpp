//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "OsmPlacemark.h"
#include "DatabaseQuery.h"

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

    if ( additionalInformation() != other.additionalInformation() ) {
        return additionalInformation() < other.additionalInformation();
    }

    if ( houseNumber() != other.houseNumber() ) {
        return houseNumber() < other.houseNumber();
    }

    if ( regionId() != other.regionId() ) {
        return regionId() < other.regionId();
    }

    if ( longitude() != other.longitude() ) {
        return longitude() < other.longitude();
    }

    return latitude() < other.latitude();
}

bool OsmPlacemark::operator==( const OsmPlacemark &other ) const
{
    return m_regionId == other.m_regionId &&
           m_category == other.m_category &&
           m_longitude == other.m_longitude &&
           m_latitude == other.m_latitude &&
           m_name == other.m_name &&
           m_houseNumber == other.m_houseNumber &&
           m_additionalInformation == other.m_additionalInformation;
}

qreal OsmPlacemark::matchScore( const DatabaseQuery* query ) const
{
    qreal score = 0.0;

    if ( query && query->resultFormat() == DatabaseQuery::AddressFormat ) {
        if ( !query->region().isEmpty() ) {
            if ( m_additionalInformation.compare( query->region(), Qt::CaseInsensitive ) == 0 ) {
                score += 2.0;
            } else if ( m_additionalInformation.startsWith( query->region(), Qt::CaseInsensitive ) ) {
                score += 0.5;
            }
        }

        if ( !query->houseNumber().isEmpty() ) {
            if ( m_houseNumber.compare( query->houseNumber(), Qt::CaseInsensitive ) == 0 ) {
                score += 1.0;
            } else if ( m_houseNumber.startsWith( query->houseNumber(), Qt::CaseInsensitive ) ) {
                score += 0.5;
            }
        }

        if ( !query->street().isEmpty() ) {
            if ( m_name.compare( query->street(), Qt::CaseInsensitive ) == 0 ) {
                score += 2.0;
            } else if ( m_name.startsWith( query->street(), Qt::CaseInsensitive ) ) {
                score += 0.5;
            }
        }
    }

    return score;
}

}
