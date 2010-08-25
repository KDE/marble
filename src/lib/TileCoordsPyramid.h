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

#ifndef MARBLE_TILECOORDSPYRAMID_H
#define MARBLE_TILECOORDSPYRAMID_H

#include <QtCore/QRect>
#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT TileCoordsPyramid
{
 public:
    TileCoordsPyramid( int const topLevel, int const bottomLevel );
    TileCoordsPyramid( TileCoordsPyramid const & other );
    TileCoordsPyramid();
    TileCoordsPyramid & operator=( TileCoordsPyramid const & rhs );
    ~TileCoordsPyramid();

    int topLevel() const;
    int bottomLevel() const;
    void setBottomLevelCoords( QRect const & coords );
    QRect coords( int const level ) const;

    /**
      * @brief returns the number of tiles covered by one pyramid
      **/
    qint64 tilesCount() const;

 private:
    void swap( TileCoordsPyramid & other );
    class Private;
    Private * d; // not const, needs to be swapable
};

}

#endif
