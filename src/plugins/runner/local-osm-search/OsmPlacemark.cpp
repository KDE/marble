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

#include <QtCore/QDataStream>

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

QString OsmPlacemark::regionName() const
{
    return m_regionName;
}

void OsmPlacemark::setRegionName( const QString &name )
{
    m_regionName = name;
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

QDataStream& operator<<( QDataStream& out, const Marble::OsmPlacemark& placemark )
{
    out << (qint16) placemark.regionId();
    out << (quint8) placemark.category();
    out << placemark.name();
    out << placemark.houseNumber();
    out << placemark.longitude();
    out << placemark.latitude();
    return out;
}

QDataStream& operator>>( QDataStream& out, Marble::OsmPlacemark& placemark )
{
    qint16 regionId;
    out >> regionId;
    placemark.setRegionId( regionId );
    quint8 category;
    out >> category;
    placemark.setCategory( (Marble::OsmPlacemark::OsmCategory) category );
    QString name;
    out >> name;
    placemark.setName( name );
    QString houseNumber;
    out >> houseNumber;
    placemark.setHouseNumber( houseNumber );
    qreal lon;
    out >> lon;
    placemark.setLongitude( lon );
    qreal lat;
    out >> lat;
    placemark.setLatitude( lat );
    return out;
}
