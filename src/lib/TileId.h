//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008, 2010 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_TILEID_H
#define MARBLE_TILEID_H

#include <QtCore/QHash>
#include <QtCore/QString>

namespace Marble
{

class TileId
{
 public:
    TileId( QString const & mapThemeId, int zoomLevel, int tileX, int tileY );
    TileId( uint mapThemeIdHash, int zoomLevel, int tileX, int tileY );
    TileId();

    int zoomLevel() const;
    int x() const;
    int y() const;
    uint mapThemeIdHash() const;

    bool operator==( TileId const& rhs ) const;
    bool operator<( TileId const& rhs ) const;

    QString toString() const;
    static TileId fromString( QString const& );

 private:
    uint m_mapThemeIdHash;
    int m_zoomLevel;
    int m_tileX;
    int m_tileY;
};

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

inline bool TileId::operator==( TileId const& rhs ) const
{
    return m_zoomLevel == rhs.m_zoomLevel
        && m_tileX == rhs.m_tileX
        && m_tileY == rhs.m_tileY
        && m_mapThemeIdHash == rhs.m_mapThemeIdHash;
}

inline bool TileId::operator<( TileId const& rhs ) const
{
    if (m_zoomLevel < rhs.m_zoomLevel)
        return true;
    else if (m_zoomLevel == rhs.m_zoomLevel
             && m_tileX < rhs.m_tileX)
        return true;
    else if (m_zoomLevel == rhs.m_zoomLevel
             && m_tileX == rhs.m_tileX
             && m_tileY < rhs.m_tileY)
        return true;
    else if (m_zoomLevel == rhs.m_zoomLevel
             && m_tileX == rhs.m_tileX
             && m_tileY == rhs.m_tileY
             && m_mapThemeIdHash < rhs.m_mapThemeIdHash)
        return true;
    return false;
}

inline uint qHash( TileId const& tid )
{
    const quint64 tmp = (( quint64 )( tid.zoomLevel() ) << 36 )
        + (( quint64 )( tid.x() ) << 18 )
        + ( quint64 )( tid.y() );
    return ::qHash( tmp ) ^ tid.mapThemeIdHash();
}

}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<( QDebug, const Marble::TileId & );
#endif

#endif
