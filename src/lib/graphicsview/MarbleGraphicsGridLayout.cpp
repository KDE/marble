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
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QRectF>
#include <QtCore/QSizeF>

namespace Marble
{

class MarbleGraphicsGridLayoutPrivate
{
 public:
    MarbleGraphicsGridLayoutPrivate( int rows, int columns )
            : m_rows( rows ),
              m_columns( columns ),
              m_spacing( 0 ),
              m_alignment( Qt::AlignLeft | Qt::AlignTop )
    {
        m_items = new ScreenGraphicsItem **[rows];
        for ( int i = 0; i < rows; ++i ) {
            m_items[i] = new ScreenGraphicsItem *[columns];
        }
        for ( int row = 0; row < rows; row++ ) {
            for ( int column = 0; column < columns; column++ ) {
                m_items[row][column] = 0;
            }
        }
    }

    MarbleGraphicsGridLayoutPrivate()
    {
        delete m_items;
    }

    // A two dimensional array of pointers to ScreenGraphicsItems
    ScreenGraphicsItem ***m_items;
    int m_rows;
    int m_columns;
    int m_spacing;
    Qt::Alignment m_alignment;
    QHash<ScreenGraphicsItem*, Qt::Alignment> m_itemAlignment;
};

MarbleGraphicsGridLayout::MarbleGraphicsGridLayout( int rows, int columns )
        : d( new MarbleGraphicsGridLayoutPrivate( rows, columns ) )
{
}

MarbleGraphicsGridLayout::~MarbleGraphicsGridLayout()
{
    delete d;
}

void MarbleGraphicsGridLayout::addItem( ScreenGraphicsItem *item, int row, int column )
{
    if ( row < d->m_rows
         && column < d->m_columns )
    {
        d->m_items[row][column] = item;
    }
}

void MarbleGraphicsGridLayout::updatePositions( MarbleGraphicsItem *parent )
{
    double *maxWidth = new double[d->m_columns];
    double *maxHeight = new double[d->m_rows];

    // Initialize with 0.0
    for ( int i = 0; i < d->m_columns; i++ ) {
        maxWidth[i] = 0.0;
    }
    for ( int i = 0; i < d->m_rows; i++ ) {
        maxHeight[i] = 0.0;
    }

    // Determining the cell sizes
    for ( int row = 0; row < d->m_rows; row++ ) {
        for ( int column = 0; column < d->m_columns; column++ ) {
            if ( d->m_items[row][column] == 0 ) {
                continue;
            }

            QSizeF size = d->m_items[row][column]->size();
            double width = size.width();
            double height = size.height();

            if ( width > maxWidth[column] ) {
                maxWidth[column] = width;
            }
            if ( height > maxHeight[row] ) {
                maxHeight[row] = height;
            }
        }
    }

    double *startX = new double[d->m_columns];
    double *endX = new double[d->m_columns];
    double *startY = new double[d->m_rows];
    double *endY = new double[d->m_rows];
    QRectF contentRect = parent->contentRect();

    for ( int i = 0; i < d->m_columns; i++ ) {
        if ( i == 0 ) {
            startX[0] = contentRect.left();
        }
        else if ( maxWidth[i] == 0 ) {
            startX[i] = endX[i-1];
        }
        else {
            startX[i] = endX[i-1] + d->m_spacing;
        }

        endX[i] = startX[i] + maxWidth[i];
    }

    for ( int i = 0; i < d->m_rows; i++ ) {
        if ( i == 0 ) {
            startY[0] = contentRect.left();
        }
        else if ( maxHeight[i] == 0 ) {
            startY[i] = endY[i-1];
        }
        else {
            startY[i] = endY[i-1] + d->m_spacing;
        }

        endY[i] = startY[i] + maxHeight[i];
    }

    // Setting the positions
    for ( int row = 0; row < d->m_rows; row++ ) {
        for ( int column = 0; column < d->m_columns; column++ ) {
            if ( d->m_items[row][column] == 0 ) {
                continue;
            }

            double xPos, yPos;

            Qt::Alignment align = alignment( d->m_items[row][column] );

            if ( align & Qt::AlignRight ) {
                xPos = endX[column] - d->m_items[row][column]->size().width();
            }
            else if ( align & Qt::AlignHCenter ) {
                xPos = startX[column]
                       + ( maxWidth[column] - d->m_items[row][column]->size().width() ) / 2.0;
            }
            else {
                xPos = startX[column];
            }

            if ( align & Qt::AlignBottom ) {
                yPos = endY[row] - d->m_items[row][column]->size().height();
            }
            else if ( align & Qt::AlignVCenter ) {
                yPos = startY[row]
                       + ( maxHeight[row] - d->m_items[row][column]->size().height() ) / 2.0;
            }
            else {
                yPos = startY[row];
            }

            d->m_items[row][column]->setPosition( QPointF( xPos, yPos ) );
        }
    }

    parent->setContentSize( QSizeF( endX[d->m_columns - 1] - contentRect.left(),
                                    endY[d->m_rows - 1] - contentRect.top() ) );
}

Qt::Alignment MarbleGraphicsGridLayout::alignment() const
{
    return d->m_alignment;
}

Qt::Alignment MarbleGraphicsGridLayout::alignment( ScreenGraphicsItem *item ) const
{
    return d->m_itemAlignment.value( item, d->m_alignment );
}

void MarbleGraphicsGridLayout::setAlignment( Qt::Alignment align )
{
    d->m_alignment = align;
}

void MarbleGraphicsGridLayout::setAlignment( ScreenGraphicsItem *item, Qt::Alignment align )
{
    d->m_itemAlignment.insert( item, align );
}

int MarbleGraphicsGridLayout::spacing() const
{
    return d->m_spacing;
}

void MarbleGraphicsGridLayout::setSpacing( int spacing )
{
    d->m_spacing = spacing;
}

} // namespace Marble
