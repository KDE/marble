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

class QByteArray;
class QImage;

namespace Marble
{
class Blending;
class TileLoader;
class StackedTileLoader;

/*!
    \class TextureTile
    \brief A class that resembles a texture tile .

    A texture tile provides a bitmap image for a certain (geographic) area and 
    for a given zoom level. Each TextureTile can be identified via a unique 
    TileId.
    
    A stack of TextureTiles that cover the same area and the same 
    zoom level can be stored (and painted) layer by layer in a StackedTile object. 
    For this purpose each TextureTile specifies a blending type. 

    Usually the tiles are organized in so called quad tiles: This means that
    with increasing zoom level four other tiles cover the same area as a 
    single "parent" tile in the previous zoom level. These four tiles have 
    the same pixel size as the "parent" tile.
    
    The process of "filling the tile with data is done in stages: The 
    State describes the current progress of loading the data (Empty, Scaled, 
    Expired, StateUptodate).
    
    The life time cycle of a TextureTile can also be influenced by its
    expiration time which will trigger a reload of the tile data.
*/

class TextureTile
{
    friend class TileLoader;
    friend class StackedTileLoader;

 public:
    TextureTile( TileId const & tileId, QImage const * image );
    ~TextureTile();

/*!
    \brief Returns a unique ID for the tile.
    \return A TileId object that encodes zoom level, position and map theme.
*/     
    TileId const & id() const;
    
/*!
    \brief Returns a unique ID for the StackedTile that contains this tile.
    \return A TileId object that encodes zoom level, position and map theme.
*/         
    TileId const & stackedTileId() const;
    
/*!
    \brief Returns the time at which the tile was previously loaded onto the harddisc.
    \return A QDateTime object that holds the modification time of the tile.
*/         
    QDateTime const & lastModified() const;
    
/*!
    \brief Returns whether the tile is considered outdated.
    \return Whether the tile needs to be reloaded according to its last modification time.
*/         
    bool isExpired() const;
    
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
    Q_DISABLE_COPY( TextureTile )

    void setImage( QImage * const );
    void setBlending( Blending const * const );
    void setStackedTileId( TileId const & );
    void setLastModified( QDateTime const & );
    void setExpireSecs( int const );

    TileId const m_id;
    TileId m_stackedTileId;
    Blending const * m_blending;
    QDateTime m_lastModified;
    int m_expireSecs;
    QImage const * m_image;
};


// inline definitions

inline TileId const & TextureTile::id() const
{
    return m_id;
}

inline TileId const & TextureTile::stackedTileId() const
{
    return m_stackedTileId;
}

inline QDateTime const & TextureTile::lastModified() const
{
    return m_lastModified;
}

inline bool TextureTile::isExpired() const
{
    return m_lastModified.secsTo( QDateTime::currentDateTime() ) >= m_expireSecs;
}

inline QImage const * TextureTile::image() const
{
    return m_image;
}

inline Blending const * TextureTile::blending() const
{
    return m_blending;
}

inline int TextureTile::byteCount() const
{
    Q_ASSERT( m_image );

    // FIXME: once Qt 4.6 is required for Marble, use QImage::byteCount()
    return m_image->numBytes();
}

inline void TextureTile::setImage( QImage * const image )
{
    Q_ASSERT( image );
    Q_ASSERT( !image->isNull() );

    delete m_image;

    m_image = image;
}

inline void TextureTile::setBlending( Blending const * const blending )
{
    m_blending = blending;
}

inline void TextureTile::setStackedTileId( TileId const & id )
{
    m_stackedTileId = id;
}

inline void TextureTile::setLastModified( QDateTime const & lastModified )
{
    m_lastModified = lastModified;
}

inline void TextureTile::setExpireSecs( int const expireSecs )
{
    m_expireSecs = expireSecs;
}

}

#endif
