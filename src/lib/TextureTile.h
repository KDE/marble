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

#ifndef MARBLE_TEXTURE_TILE_H
#define MARBLE_TEXTURE_TILE_H

#include <QtCore/QDateTime>

#include "TileId.h"

class QByteArray;
class QImage;

namespace Marble
{
class TileLoader;
class StackedTileLoader;

class TextureTile
{
    friend class TileLoader;
    friend class StackedTileLoader;

 public:
    enum State {
        StateEmpty,
        StateScaled,
        StateExpired,
        StateUptodate
    };

    enum MergeRule {
        MergeCopy,
        MergeMultiply
    };

    explicit TextureTile( TileId const & );
    TextureTile( TileId const & tileId, QString const & fileName );
    ~TextureTile();

    TileId const & id() const;
    TileId const & stackedTileId() const;
    QDateTime const & lastModified() const;
    bool expired() const;
    QImage const * image() const;
    QImage * image();
    State state() const;
    MergeRule mergeRule() const;

 private:
    Q_DISABLE_COPY( TextureTile )

    void setState( State const );
    void setImage( QByteArray const & data );
    void setImage( QImage * const );
    void setMergeRule( MergeRule const );
    void setStackedTileId( TileId const & );
    void setLastModified( QDateTime const & );
    void setExpireSecs( int const );

    TileId const m_id;
    TileId m_stackedTileId;
    State m_state;
    MergeRule m_mergeRule;
    QDateTime m_lastModified;
    int m_expireSecs;
    QImage * m_image;
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

inline bool TextureTile::expired() const
{
    return m_lastModified.secsTo( QDateTime::currentDateTime() ) >= m_expireSecs;
}

inline QImage const * TextureTile::image() const
{
    return m_image;
}

inline QImage * TextureTile::image()
{
    return m_image;
}

inline TextureTile::State TextureTile::state() const
{
    return m_state;
}

inline TextureTile::MergeRule TextureTile::mergeRule() const
{
    return m_mergeRule;
}

inline void TextureTile::setState( State const state )
{
    m_state = state;
}

inline void TextureTile::setImage( QImage * const image )
{
    m_image = image;
}

inline void TextureTile::setMergeRule( MergeRule const mergeRule )
{
    m_mergeRule = mergeRule;
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
