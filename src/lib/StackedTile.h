//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2010  Torsten Rahn <tackat@kde.org>
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
// Copyright 2010       Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_STACKEDTILE_H
#define MARBLE_STACKEDTILE_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtGui/QColor>

#include "MarbleGlobal.h"

#include "GeoDataContainer.h"

class QImage;

namespace Marble
{

class StackedTilePrivate;
class Tile;
class TileId;

/*!
    \class StackedTile
    \brief A single tile that consists of a stack of Tile layers.

    The StackedTile is a tile container that covers a certain area and is used 
    for a particular zoom level. It consists of a <b>stack of several
    individual thematic Tiles</b> that cover the very same area and
    are used for the very same zoom level: This stack of Tiles is
    built up from the ground: The first Tile at the bottom usually
    represents the ground surface. Optionally there might be a range of other
    Tiles stacked on top which cover e.g. relief, streets and clouds.
    
    For rendering the whole stack of tiles gets merged and blended into a 
    single QImage. This merging/blending operation is usually only performed 
    once the stack of tiles changes visually. As a result access to the visual 
    composition of all TextureTile layers is very fast since it is reduced to 
    a single QImage that also consumes very little memory.

    The whole mechanism is comparable to layers in applications like
    Gimp or Photoshop (TM) which can be blended on top of each other via 
    so called filters and can be merged into a single layer if required.

    Restrictions: The Tiles that are part of the stack need to be of
    the same size and need to cover the same area at the same zoom level using 
    the very same projection.
*/

class StackedTile
{
    friend class StackedTileLoader;

 public:
    explicit StackedTile( TileId const &id, QImage const &resultImage, GeoDataDocument * resultVector, QVector<QSharedPointer<Tile> > const &tiles );
    virtual ~StackedTile();

/*!
    \brief Returns a unique ID for the tile.
    \return A TileId object that encodes zoom level, position and map theme.
*/
    TileId const& id() const;

    void setUsed( bool used );
    bool used() const;

    int depth() const;
    int numBytes() const;

/*!
    \brief Returns the stack of Tiles
    \return A container of Tile objects.
*/
    QVector<QSharedPointer<Tile> > tiles() const;

/*!
    \brief Returns the QImage that describes the merged stack of Tiles
    \return A non-zero pointer to the resulting QImage 
*/
    QImage const * resultImage() const;

/*!
    \brief Returns the GeoDataDocument that describes the merged stack of Tiles
    \return A non-zero pointer to the resulting GeoDataDocument
*/
    GeoDataDocument * resultVectorData() const;

/*!
    \brief Returns the color value of the result tile at the given integer position.
    \return The uint that describes the color value of the given pixel 
    
    Note: for gray scale images the color value of a single pixel is described
    via a uchar (1 byte) while for RGB(A) images uint (4 bytes) are used.
*/
    uint pixel( int x, int y ) const;
    
/*!
    \brief Returns the color value of the result tile at a given floating point position.
    \return The uint that describes the color value of the given pixel 
    
    Subpixel calculation is done via bilinear interpolation.
    
    Note: for gray scale images the color value of a single pixel is described
    via a uchar (1 byte) while for RGB(A) images uint (4 bytes) are used.
*/    
    uint pixelF( qreal x, qreal y ) const;
    // This method passes the top left pixel (if known already) for better performance
    uint pixelF( qreal x, qreal y, const QRgb& pixel ) const; 

 private:
    Q_DISABLE_COPY( StackedTile )

    StackedTilePrivate *d;
};

}

#endif
