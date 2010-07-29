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

#include "TileCoordsPyramid.h"

#include <algorithm>

namespace Marble
{

class TileCoordsPyramid::Private
{
public:
    Private( int const topLevel, int const bottomLevel );

    int m_topLevel;
    int m_bottomLevel;
    QRect m_bottomLevelCoords;
};

TileCoordsPyramid::Private::Private( int const topLevel, int const bottomLevel )
    : m_topLevel( topLevel ),
      m_bottomLevel( bottomLevel )
{
    Q_ASSERT( m_topLevel <= m_bottomLevel );
}


TileCoordsPyramid::TileCoordsPyramid( int const topLevel, int const bottomLevel )
    : d( new Private( topLevel, bottomLevel ))
{
}

TileCoordsPyramid::TileCoordsPyramid( TileCoordsPyramid const & other )
    : d( new Private( *other.d ))
{
}

TileCoordsPyramid::TileCoordsPyramid()
    :d( new Private( 0, 0 ) )
{

}

TileCoordsPyramid & TileCoordsPyramid::operator=( TileCoordsPyramid const & rhs )
{
    TileCoordsPyramid temp( rhs );
    swap( temp );
    return *this;
}

TileCoordsPyramid& TileCoordsPyramid::operator <<( TileCoordsPyramid const & other )
{
    *d = *other.d;
    return *this;
}

TileCoordsPyramid::~TileCoordsPyramid()
{
    delete d;
}

int TileCoordsPyramid::topLevel() const
{
    return d->m_topLevel;
}

int TileCoordsPyramid::bottomLevel() const
{
    return d->m_bottomLevel;
}

void TileCoordsPyramid::setBottomLevelCoords( QRect const & coords )
{
    d->m_bottomLevelCoords = coords;
}

QRect TileCoordsPyramid::coords( int const level ) const
{
    Q_ASSERT( d->m_topLevel <= level && level <= d->m_bottomLevel );
    int bottomX1, bottomY1, bottomX2, bottomY2;
    d->m_bottomLevelCoords.getCoords( &bottomX1, &bottomY1, &bottomX2, &bottomY2 );
    int const deltaLevel = d->m_bottomLevel - level;
    int const x1 = bottomX1 >> deltaLevel;
    int const y1 = bottomY1 >> deltaLevel;
    int const x2 = bottomX2 >> deltaLevel;
    int const y2 = bottomY2 >> deltaLevel;
    QRect result;
    result.setCoords( x1, y1, x2, y2 );
    return result;
}

qint64 TileCoordsPyramid::tilesCount() const
{
    qint64 result = 0;
    for ( int level = d->m_topLevel; level <= d->m_bottomLevel; ++level ) {
        QRect const levelCoords = coords( level );
        result += levelCoords.width() * levelCoords.height();
    }
    return result;
}

void TileCoordsPyramid::swap( TileCoordsPyramid & other )
{
    std::swap( d, other.d );
}

}
