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

OsmPlacemark::OsmPlacemark() : m_regionId( -1 ),
    m_longitude( 0.0 ), m_latitude( 0.0 )
{
    // nothing to do
}

QString OsmPlacemark::name() const
{
    return m_name;
}

void OsmPlacemark::setName( const QString &name )
{
    m_name = name;
}

int OsmPlacemark::regionId() const
{
    return m_regionId;
}

void OsmPlacemark::setOsmRegionId( int id )
{
    m_regionId = id;
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
    out << placemark.regionId();
    out << placemark.name();
    out << placemark.longitude();
    out << placemark.latitude();
    return out;
}

QDataStream& operator>>( QDataStream& out, Marble::OsmPlacemark& placemark )
{
    qint32 regionId;
    out >> regionId;
    placemark.setOsmRegionId( regionId );
    QString name;
    out >> name;
    placemark.setName( name );
    qreal lon;
    out >> lon;
    placemark.setLongitude( lon );
    qreal lat;
    out >> lat;
    placemark.setLatitude( lat );
    return out;
}
