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

namespace Marble {

int OsmRegion::m_idFactory = 0;

OsmRegion::OsmRegion() : m_identifier( ++m_idFactory ),
    m_longitude( 0.0 ), m_latitude( 0.0 )
{
    m_parents << 0;
}

int OsmRegion::identifier() const
{
    return m_identifier;
}

void OsmRegion::setIdentifier( int identifier )
{
    m_identifier = identifier;
}

QList<int> OsmRegion::parentIdentifiers() const
{
    return m_parents;
}

void OsmRegion::setParentIdentifiers( const QList<int> &identifiers )
{
    m_parents = identifiers;
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
