// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_TEXTURETILE_H
#define MARBLE_TEXTURETILE_H

#include <QtCore/QDateTime>
#include <QtGui/QImage>

#include "TileId.h"

class QImage;

namespace Marble
{
class Blending;

/*!
    \class Tile
    \brief A class that resembles a tile (then it is extended to TextureTile or Vectortile).

    A tile provides a bitmap image or vector tile for a certain (geographic) area and
    for a given zoom level. Each Tile can be identified via a unique
    TileId.
    
    A stack of Tiles that cover the same area and the same
    zoom level can be stored (and painted) layer by layer in a StackedTile object. 
    For this purpose each Tile specifies a blending type.

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
    Tile( TileId const & tileId, QImage const & image, const Blending * blending );
    ~Tile();

/*!
    \brief Returns a unique ID for the tile.
    \return A TileId object that encodes zoom level, position and map theme.
*/     
    TileId const & id() const;

/*!
    \brief Returns the QImage that describes the look of the TextureTile
    \return A non-zero pointer to a QImage associated with the tile.
*/
    QImage const * image() const;
    
/*!
    \brief Returns the kind of blending used for the tile.
    \return A pointer to the blending object used for painting/merging the TextureTile.
    
    If no blending is set the pointer returned will be zero.
*/     
    Blending const * blending() const;
    int byteCount() const;

 private:
    Q_DISABLE_COPY( Tile )

    TileId const m_id;
    Blending const * const m_blending;
    QImage const m_image;
};


// inline definitions

inline TileId const & Tile::id() const
{
    return m_id;
}

inline QImage const * Tile::image() const
{
    return &m_image;
}

inline Blending const * Tile::blending() const
{
    return m_blending;
}

inline int Tile::byteCount() const
{
    return m_image.byteCount();
}

}

#endif
