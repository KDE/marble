//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Jens-Michael Hoffmann  <jensmh@gmx.de>"
//

// Own
#include "TileId.h"

#include <QtCore/QHash>


TileId::TileId( int zoomLevel, int tileX, int tileY )
  : m_zoomLevel( zoomLevel ), m_tileX( tileX ), m_tileY( tileY )
{
}

TileId::TileId()
  : m_zoomLevel( 0 ), m_tileX( 0 ), m_tileY( 0 )
{
}

QString TileId::toString() const
{
    return QString( "%1:%2:%3" ).arg( m_zoomLevel ).arg( m_tileX ).arg( m_tileY );
}

TileId TileId::fromString( QString const& idStr )
{
    return TileId( idStr.section( ':', 0, 0 ).toInt(),
                   idStr.section( ':', 1, 1 ).toInt(),
                   idStr.section( ':', 2, 2 ).toInt() );
}

int TileId::x() const
{
    return m_tileX;
}

int TileId::y() const
{
    return m_tileY;
}

int TileId::zoomLevel() const
{
    return m_zoomLevel;
}

bool operator==( TileId const& lhs, TileId const& rhs )
{
    return lhs.m_zoomLevel == rhs.m_zoomLevel
        && lhs.m_tileX == rhs.m_tileX
        && lhs.m_tileY == rhs.m_tileY;
}

uint qHash( TileId const& tid )
{
    quint64 tmp = (quint64)(tid.m_zoomLevel) << 36
        + (quint64)(tid.m_tileX) << 18
        + (quint64)(tid.m_tileY);
    return qHash( tmp );
}
