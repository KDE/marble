//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

// self
#include "BillboardGraphicsItem.h"
#include "MarbleGraphicsItem_p.h"

// Marble
#include "ViewportParams.h"

namespace Marble
{

class BillboardGraphicsItem::Private : public MarbleGraphicsItemPrivate
{
 public:
    Private( BillboardGraphicsItem *parent ) :
        MarbleGraphicsItemPrivate( parent ),
        m_alignment( Qt::AlignHCenter | Qt::AlignVCenter )
    {
    }

    QList<QPointF> positions() const
    {
        return m_positions;
    }

    QList<QPointF> absolutePositions() const
    {
        return m_positions;
    }

    Qt::Alignment m_alignment;

    void setProjection( const ViewportParams *viewport )
    {
        m_positions.clear();

        qreal x[100], y;
        int pointRepeatNumber = 0;
        bool globeHidesPoint;

        QRect const viewportRect = viewport->mapRegion().boundingRect();
        viewport->screenCoordinates( m_coordinates, x, y, pointRepeatNumber,
                                         m_size, globeHidesPoint );
        if ( !globeHidesPoint ) {
            for( int i = 0; i < pointRepeatNumber; ++i ) {
                // Handle vertical alignment
                qint32 topY =  ( viewport->height() -  m_size.height() ) / 2;
                if ( m_alignment & Qt::AlignTop ) {
                  topY = y - m_size.height();
                } else if ( m_alignment & Qt::AlignVCenter ) {
                  topY = y - ( m_size.height() / 2 );
                } else if ( m_alignment & Qt::AlignBottom ) {
                  topY = y;
                }

                // Handle horizontal alignment
                qint32 leftX = ( viewport->width() - m_size.width() ) / 2;
                if ( m_alignment & Qt::AlignLeft ) {
                  leftX =  x[i] - m_size.width();
                } else if ( m_alignment & Qt::AlignHCenter ) {
                  leftX = x[i] - ( m_size.width() / 2 );
                } else if ( m_alignment & Qt::AlignRight ) {
                  leftX = x[i];
                }

                QRect const position = QRect( QPoint ( leftX, topY ), m_size.toSize() );
                if ( position.intersects( viewportRect ) ) {
                  m_positions.append( QPoint( leftX, topY ) );
                }
            }
        }
    }

    GeoDataCoordinates m_coordinates;
    QList<QPointF> m_positions;
};

BillboardGraphicsItem::BillboardGraphicsItem()
    : MarbleGraphicsItem( new Private( this ) )
{
}

GeoDataCoordinates BillboardGraphicsItem::coordinate() const
{
    return p()->m_coordinates;
}

void BillboardGraphicsItem::setCoordinate( const GeoDataCoordinates &coordinates )
{
    p()->m_coordinates = coordinates;
}

QList<QPointF> BillboardGraphicsItem::positions() const
{
    return p()->positions();
}

QList<QRectF> BillboardGraphicsItem::boundingRects() const
{
    QList<QRectF> rects;
    QSizeF const size = p()->m_size;
    foreach(const QPointF &point, p()->m_positions) {
        rects << QRectF(point, size);
    }
    return rects;
}

Qt::Alignment BillboardGraphicsItem::alignment() const
{
    return p()->m_alignment;
}

void BillboardGraphicsItem::setAlignment(Qt::Alignment alignment)
{
    p()->m_alignment = alignment;
}

BillboardGraphicsItem::Private *BillboardGraphicsItem::p() const
{
    return static_cast<Private *>( d );
}

} // Marble namespace
