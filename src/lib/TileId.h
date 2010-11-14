//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008, 2010 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_TILEID_H
#define MARBLE_TILEID_H

#include <QtCore/QHash>
#include <QtCore/QString>

namespace Marble
{

class TileId
{
    friend bool operator==( TileId const& lhs, TileId const& rhs );
    friend uint qHash( TileId const& );

 public:
    TileId( QString const & mapThemeId, int zoomLevel, int tileX, int tileY );
    TileId( uint mapThemeIdHash, int zoomLevel, int tileX, int tileY );
    TileId();

    int zoomLevel() const;
    int x() const;
    int y() const;
    uint mapThemeIdHash() const;

    QString toString() const;
    static TileId fromString( QString const& );

 private:
    uint m_mapThemeIdHash;
    int m_zoomLevel;
    int m_tileX;
    int m_tileY;
};

bool operator==( TileId const& lhs, TileId const& rhs );
uint qHash( TileId const& );


// inline definitions

inline int TileId::zoomLevel() const
{
    return m_zoomLevel;
}

inline int TileId::x() const
{
    return m_tileX;
}

inline int TileId::y() const
{
    return m_tileY;
}

inline uint TileId::mapThemeIdHash() const
{
    return m_mapThemeIdHash;
}

inline QString TileId::toString() const
{
    return QString( "%1:%2:%3:%4" ).arg( m_mapThemeIdHash ).arg( m_zoomLevel ).arg( m_tileX ).arg( m_tileY );
}

inline bool operator==( TileId const& lhs, TileId const& rhs )
{
    return lhs.m_zoomLevel == rhs.m_zoomLevel
        && lhs.m_tileX == rhs.m_tileX
        && lhs.m_tileY == rhs.m_tileY
        && lhs.m_mapThemeIdHash == rhs.m_mapThemeIdHash;
}

inline uint qHash( TileId const& tid )
{
    const quint64 tmp = (( quint64 )( tid.m_zoomLevel ) << 36 )
        + (( quint64 )( tid.m_tileX ) << 18 )
        + ( quint64 )( tid.m_tileY );
    return ::qHash( tmp ) ^ tid.m_mapThemeIdHash;
}

}

#endif
