//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "MarbleGraphicsGridLayout.h"

// Marble
#include "ScreenGraphicsItem.h"

// Qt
#include <QtCore/QRectF>
#include <QtCore/QSizeF>

namespace Marble
{

class MarbleGraphicsGridLayoutPrivate
{
 public:
    MarbleGraphicsGridLayoutPrivate( int width, in height )
            : m_items( new ScreenGraphicsItem *[width][height] ),
              m_width( width ),
              m_height( height )
    {
        for ( int x = 0; x < width; x++ ) {
            for ( int y = 0; y < height; y++ ) {
                m_items[x][y] = 0;
            }
        }
    }

    int m_width;
    int m_height;
    // A two dimensional array of pointers to ScreenGraphicsItems
    ScreenGraphicsItem ***m_items;
};

MarbleGraphicsGridLayout::MarbleGraphicsGridLayout( int width, int height )
        : d( new MarbleGraphicsGridLayoutPrivate( width, height ) )
{
}

void MarbleGraphicsGridLayout::addItem( ScreenGraphicsItem *item, int x, int y )
{
    if ( x < d->m_width
         && y < d->m_height )
    {
        d->m_items[x][y] = item;
    }
}

void MarbleGraphicsGridLayout::updatePositions( MarbleGraphicsItem *parent )
{
    double *maxWidth = new double[d->m_width];
    double *maxHeight = new double[d->m_height];

    // Initialize with 0.0
    for ( int x = 0; x < d->m_width; x++ ) {
        maxWidth[x] = 0.0;
    }
    for ( int y = 0; y < d->m_height; y++ ) {
        maxHeight[y] = 0.0;
    }

    // Determining the cell sizes
    for ( int x = 0; x < d->m_width; x++ ) {
        for ( int y = 0; y < d->m_height; y++ ) {
            if ( d->m_items[x][y] == 0 ) {
                continue;
            }

            QSizeF size = d->m_items[x][y]->size();
            double width = size.width();
            double height = size.height();

            if ( width > maxWidth[x] ) {
                maxWidth[x] = width;
            }
            if ( height > maxHeight[y] ) {
                maxHeight[y] = height;
            }
        }
    }

    double *startX = new double[d->m_width+1];
    double *startY = new double[d->m_height+1];
    QRectF contentRect = parent->contentRect();

    startX[0] = contentRect.left();
    for ( int x = 1; x <= d->m_width; x++ ) {
        startX[x] = startX[x-1] + maxWidth[x-1];
    }

    startY[0] = contentRect.top();
    for ( int y = 1; y <= d->m_height; y++ ) {
        startY[y] = startY[y-1] + maxHeight[y-1];
    }

    // Setting the positions
    for ( int x = 0; x < d->m_width; x++ ) {
        for ( int y = 0; y < d->m_height; y++ ) {
            if ( d->m_items[x][y] == 0 ) {
                continue;
            }

            d->m_items[x][y]->setPosition( QPointF( startX[x], startY[y] ) );
        }
    }

    parent->setContentSize( QSizeF( startX[d->m_width], startY[d->m_height] ) );
}

} // namespace Marble
