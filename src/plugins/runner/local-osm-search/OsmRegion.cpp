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

#include <QtCore/QDataStream>

namespace Marble {

int OsmRegion::m_idFactory = 0;

OsmRegion::OsmRegion() : m_identifier( ++m_idFactory ),
    m_longitude( 0.0 ), m_latitude( 0.0 )
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

}

QDataStream& operator<<( QDataStream& out, const Marble::OsmRegion& region )
{
    out << (qint32) region.identifier();
    out << region.name();
    out << region.longitude();
    out << region.latitude();
    return out;
}

QDataStream& operator>>( QDataStream& out, Marble::OsmRegion& region )
{
    qint32 identifier;
    out >> identifier;
    region.setIdentifier( identifier );
    QString name;
    out >> name;
    region.setName( name );
    qreal lon;
    out >> lon;
    region.setLongitude( lon );
    qreal lat;
    out >> lat;
    region.setLatitude( lat );
    return out;
}
