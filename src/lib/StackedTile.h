//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2009  Torsten Rahn <tackat@kde.org>
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
//

//
// Description: StackedTile contains a single image quadtile 
// and jumptables for faster access to the pixel data
//

#ifndef MARBLE_STACKEDTILE_H
#define MARBLE_STACKEDTILE_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtGui/QColor>

#include "AbstractTile.h"
#include "global.h"

class QImage;

namespace Marble
{

class StackedTilePrivate;
class GeoSceneTexture;
class TextureTile;
class StackedTileLoader;

/*!
    \class StackedTile
    \brief A single tile that is composed one or more different tile layers.

    The StackedTile is a tile that covers a certain area and is used 
    for a particular zoom level. It consists of a <b>stack of several
    individual thematic TextureTiles</b> that cover the very same area and 
    are used for the very same zoom level: This stack of TextureTiles is
    built up from the ground: The first TextureTile at the bottom usually 
    represents the ground surface. Optionally there might be a range of other
    TextureTiles stacked on top which cover e.g. relief, streets and clouds.
    
    For rendering the whole stack of tiles gets merged and blended into a 
    single QImage. This merging/blending operation is usually only performed 
    once the stack of tiles changes. As a result access to the visual 
    composition of all TextureTile layers is very fast since it is reduced to 
    a single QImage that also covers very little memory.

    The whole mechanism is comparable to layers in applications like
    Gimp or Photoshop (TM) which can be blended on top of each other via 
    so called filters and can be merged into a single layer if required.

    Restrictions: The TextureTiles that are part of the stack need to be of 
    the same size and need to cover the same area at the same zoom level using 
    the very same projection.
*/

class StackedTile : public AbstractTile
{
    friend class StackedTileLoader;

 public:
    explicit StackedTile( TileId const& tid, QObject * parent = 0 );
    virtual ~StackedTile();

    int depth() const;
    int numBytes() const;
    bool isExpired() const;

    bool forMergedLayerDecorator() const;
    void setForMergedLayerDecorator();

/*!
    \brief Returns the stack of TextureTiles
    \return A non-zero pointer to the container of TextureTile objects.
*/
    QVector<QSharedPointer<TextureTile> > * tiles();

/*!
    \brief Returns the QImage that describes the merged stack of TextureTiles
    \return A non-zero pointer to the resulting QImage 
*/
    QImage const * resultTile() const; // Note: maybe this should be resultImage() ?
    QImage * resultTile();

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

 protected:
    StackedTile( StackedTilePrivate &dd, QObject *parent );

 private:
    Q_DECLARE_PRIVATE( StackedTile )
    Q_DISABLE_COPY( StackedTile )

    // TODO: move the private methods into the private class. 
    
/*!
    \brief Appends a TextureTile to the stack und updates the completion state.
*/    
    void addTile( QSharedPointer<TextureTile> const & );
    
/*!
    \brief Updates the completion state based on the state of each TextureTile.
*/    
    void deriveCompletionState();
    void initJumpTables();
    
    bool hasTiles() const;
    
/*!
    \brief This method performs the actual merge process.
*/    
    void initResultTile();

    StackedTilePrivate *d;
};

}

#endif
