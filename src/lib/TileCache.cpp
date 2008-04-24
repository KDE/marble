//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn  <rahn@kde.org>"
//


#include "TileCache.h"

#include <QtCore/QDebug>


bool TileCache::find( TileId const& key, TextureTile* tile )
{
    if ( m_cache.contains( key ) )
    {
        tile = m_cache[ key ];
        return true;
    }
    return false;
}

bool TileCache::insert( TileId const& key, TextureTile* tile )
{
//    qDebug() << "Tile " << key.toString() << " cached. Cache size: " 
//             << m_cache.totalCost() / 1024 << " kB";
    return m_cache.insert( key, tile, tile->numBytes() );
}

bool TileCache::contains( TileId const& key )
{
    return m_cache.contains( key );
}

TextureTile* TileCache::take( TileId const& key )
{
//    qDebug() << "Tile " << key.toString() << " taken from cache. Cache size: " 
//             << m_cache.totalCost() / 1024 << " kB";
    return m_cache.take( key );
}

#include "TileCache.moc"
