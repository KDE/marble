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

namespace Marble
{

class TileCoordsPyramid::Private
{
public:
    Private( int const topLevel, int const bottomLevel );

    int m_topLevel;
    int m_bottomLevel;
    QRect m_topLevelCoords;
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

void TileCoordsPyramid::setTopLevelCoords( QRect const & coords )
{
    d->m_topLevelCoords = coords;
}

QRect TileCoordsPyramid::coords( int const level ) const
{
    Q_ASSERT( d->m_topLevel <= level && level <= d->m_bottomLevel );
    int topX1, topY1, topX2, topY2;
    d->m_topLevelCoords.getCoords( &topX1, &topY1, &topX2, &topY2 );
    int const deltaLevel = level - d->m_topLevel;
    int const x1 = topX1 << deltaLevel;
    int const y1 = topY1 << deltaLevel;
    int const x2 = (( topX2 + 1 ) << deltaLevel ) -1;
    int const y2 = (( topY2 + 1 ) << deltaLevel ) -1;
    QRect result;
    result.setCoords( x1, y1, x2, y2 );
    return result;
}

int TileCoordsPyramid::tilesCount() const
{
    int const topLevelTilesCount = d->m_topLevelCoords.width() * d->m_topLevelCoords.height();
    int const levels = d->m_bottomLevel - d->m_topLevel + 1;
    int result = 0;
    for ( int i = 0; i < levels; ++i )
        result += topLevelTilesCount << ( 2 * i );
    return result;
}

}
