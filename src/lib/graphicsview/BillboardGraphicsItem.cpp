//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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
        MarbleGraphicsItemPrivate( parent )
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

    void setProjection( const ViewportParams *viewport )
    {
        m_positions.clear();

        qreal x[100], y;
        int pointRepeatNumber;
        bool globeHidesPoint;

        if( viewport->screenCoordinates( m_coordinates,
                                         x, y,
                                         pointRepeatNumber,
                                         m_size,
                                         globeHidesPoint ) )
        {
            for( int i = 0; i < pointRepeatNumber; ++i ) {
                qint32 leftX = x[i] - ( m_size.width() / 2 );
                qint32 topY = y    - ( m_size.height() / 2 );

                m_positions.append( QPoint( leftX, topY ) );
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

BillboardGraphicsItem::Private *BillboardGraphicsItem::p() const
{
    return static_cast<Private *>( d );
}

} // Marble namespace
