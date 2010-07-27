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

class TextureTile
{
    friend class TileLoader;
    friend class StackedTileLoader;

 public:
    TextureTile( TileId const & tileId, QImage const * image );
    ~TextureTile();

    TileId const & id() const;
    TileId const & stackedTileId() const;
    QDateTime const & lastModified() const;
    bool isExpired() const;
    QImage const * image() const;
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
