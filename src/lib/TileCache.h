//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef TILECACHE_H
#define TILECACHE_H


/** @file
 * This file contains the headers for TileCache.
 * 
 * @author Torsten Rahn  <rahn@kde.org>
 */


#include <QtCore/QCache>
#include <QtCore/QObject>
#include "TextureTile.h"
#include "TileId.h"


/** 
 * @short A class that enables caching for tiles
 *
 */

class TileCache : public QObject
{
 Q_OBJECT

 public:
    TileCache(){ /* NOOP */ }
    virtual ~TileCache(){ /* NOOP */ }

 public:
    // For consistency and ease of use we have an API similar to QPixmapCache
    // In opposite to QPixmapCache we can create several caches
    void clear() { m_cache.clear(); }

    bool contains( TileId const& key );
    bool find( TileId const& key, TextureTile* tile );
    bool insert( TileId const& key, TextureTile* tile );
    void remove( TileId const& key ){ m_cache.remove( key ); }
    TextureTile* take( TileId const& key );

    int cacheLimit() const { return ( m_cache.maxCost() / 1024 ); }
    void setCacheLimit( int cacheLimit ) { m_cache.setMaxCost( 1024 * cacheLimit ); }

 private:
    QCache<TileId, TextureTile> m_cache;
};


#endif // TILECACHE_H
