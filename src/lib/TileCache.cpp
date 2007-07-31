//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn  <rahn@kde.org>"
//


#include <QtCore/QDebug>
#include "TileCache.h"

bool TileCache::find( int key, TextureTile* tile )
{
    if ( m_cache.contains( key ) )
    {
        tile = m_cache[ key ];
        return true;
    }
    return false;
}

bool TileCache::insert( int key, TextureTile* tile )
{
//    qDebug() << "Tile " << tile->id() << " cached. Cache size: " 
//             << m_cache.totalCost() / 1024 << " kB";
    m_cache.insert( key, tile, tile->numBytes() );
    return true;
}

bool TileCache::contains( int key )
{
    return m_cache.contains( key );
}

TextureTile* TileCache::take( int key )
{
//    qDebug() << "Tile " << key << " taken from cache. Cache size: " 
//             << m_cache.totalCost() / 1024 << " kB";
    return m_cache.take( key );
}

#include "TileCache.moc"
