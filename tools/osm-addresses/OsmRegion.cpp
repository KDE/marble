//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmRegion.h"

#include <QtCore/QList>

namespace Marble
{

int OsmRegion::m_idFactory = 0;

OsmRegion::OsmRegion() : m_identifier( ++m_idFactory ),
    m_parent( 0 ), m_longitude( 0.0 ), m_latitude( 0.0 ),
    m_left( 0 ), m_right( 0 ), m_adminLevel( 10 )
{
    // nothing to do
}

int OsmRegion::identifier() const
{
    return m_identifier;
}

void OsmRegion::setIdentifier( int identifier )
{
    m_identifier = identifier;
}

int OsmRegion::parentIdentifier() const
{
    return m_parent;
}

void OsmRegion::setParentIdentifier( int identifier )
{
    m_parent = identifier;
}

QString OsmRegion::name() const
{
    return m_name;
}

void OsmRegion::setName( const QString &name )
{
    m_name = name;
}

qreal OsmRegion::longitude() const
{
    return m_longitude;
}

void OsmRegion::setLongitude( qreal longitude )
{
    m_longitude = longitude;
}

qreal OsmRegion::latitude() const
{
    return m_latitude;
}

void OsmRegion::setLatitude( qreal latitude )
{
    m_latitude = latitude;
}

bool OsmRegion::operator==( const OsmRegion &other ) const
{
    return identifier() == other.identifier();
}

void OsmRegion::setLeft( int left )
{
    m_left = left;
}

int OsmRegion::left() const
{
    return m_left;
}

void OsmRegion::setRight( int right )
{
    m_right = right;
}

int OsmRegion::right() const
{
    return m_right;
}

const GeoDataPolygon& OsmRegion::geometry() const
{
    return m_geometry;
}

void OsmRegion::setGeometry( const GeoDataPolygon &polygon )
{
    m_geometry = polygon;
}

int OsmRegion::adminLevel() const
{
    return m_adminLevel;
}

void OsmRegion::setAdminLevel( int level )
{
    m_adminLevel = level;
}


}
