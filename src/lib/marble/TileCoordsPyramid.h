// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_TILECOORDSPYRAMID_H
#define MARBLE_TILECOORDSPYRAMID_H

#include <QtGlobal>
#include "marble_export.h"

class QRect;

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
