/*
 *Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLE_VECTORTILE_H
#define MARBLE_VECTORTILE_H

#include <QtCore/QDateTime>
#include <QtGui/QImage>

#include "Tile.h"
#include "TileId.h"

#include "GeoDataContainer.h"
#include "MarbleDebug.h"

class QImage;

namespace Marble
{
class Blending;

/*!
    \class VectorTile
    \brief A class that resembles a tile with vector data (extends Tile).

    This tile provides vector data for a certain (geographic) area and
    for a given zoom level. It also has an image for rendering image layers
    such as clouds or hillshading layer.
    Each Tile can be identified via a unique
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

class VectorTile : public Tile
{
 public:
    VectorTile( TileId const & tileId, GeoDataDocument * vectordata, QString const &format, const Blending * blending );
    ~VectorTile();
/*!
    \brief Returns the GeoDataContainer containing de vector data of the Tile
    \return A non-zero pointer to a GeoDataContainer associated with the tile.
*/
    GeoDataDocument * vectorData() const;
    virtual int byteCount() const;

    virtual const char* nodeType() const;
    virtual QString type();

 private:
    Q_DISABLE_COPY( VectorTile )

    GeoDataDocument * m_vectordata;

};

inline GeoDataDocument * VectorTile::vectorData() const
{
    return m_vectordata;
}

inline int VectorTile::byteCount() const
{
    return 0;
}

}

#endif // MARBLE_VECTORTILE_H
