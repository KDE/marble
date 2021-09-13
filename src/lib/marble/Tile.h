 /*
 SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>

 SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

 SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_TILE_H
#define MARBLE_TILE_H

#include "TileId.h"

namespace Marble
{

/*!
    \class Tile
    \brief A class that resembles a tile (then it is extended to TextureTile or Vectortile).

    A tile provides a bitmap image or vector data for a certain (geographic) area and
    for a given zoom level. Each Tile can be identified via a unique
    TileId.
    
    A stack of Tiles that cover the same area and the same
    zoom level can be stored (and painted) layer by layer in a StackedTile object. 
    For this purpose each Texture Tile specifies a blending type.

    Usually the tiles are organized in so called quad tiles: This means that
    with increasing zoom level four other tiles cover the same area as a 
    single "parent" tile in the previous zoom level. These four tiles have 
    the same pixel size as the "parent" tile.
    
    The process of "filling the tile with data is done in stages: The 
    State describes the current progress of loading the data (Empty, Scaled, 
    Expired, StateUptodate).
    
    The life time cycle of a Tile can also be influenced by its
    expiration time which will trigger a reload of the tile data.
*/

class Tile
{
 public:
    explicit Tile( TileId const & tileId );
    virtual ~Tile();

/*!
    \brief Returns a unique ID for the tile.
    \return A TileId object that encodes zoom level, position and map theme.
*/     
    TileId const & id() const;

 private:
    Q_DISABLE_COPY( Tile )

    TileId const m_id;
};


// inline definitions

inline TileId const & Tile::id() const
{
    return m_id;
}

}

#endif
